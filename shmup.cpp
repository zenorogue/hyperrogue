// Hyperbolic Rogue

// Copyright (C) 2011-2018 Zeno Rogue, see 'hyper.cpp' for details

// implementation of the shoot'em up mode

namespace hr {

extern int mousex, mousey;
extern bool clicked;

// joysticks for controlling the mobile shmup mode
namespace shmupballs {
  int xmove, xfire, yb, rad;

  void calc() {      
    int rr = int(realradius());
    rad = int(rr * (vid.mobilecompasssize ? vid.mobilecompasssize : 14) / 100);
    xmove = max(current_display->xcenter - rr - rad, rad);
    xfire = min(current_display->xcenter + rr + rad, vid.xres - rad);
    yb = current_display->ycenter + rr - rad;
    }
  }

namespace multi {

  charstyle scs[MAXPLAYER];

  int players = 1;
  cellwalker player[MAXPLAYER];
  vector<int> revive_queue; // queue for revival
  
  cell *origpos[MAXPLAYER], *origtarget[MAXPLAYER];

  bool flipped[MAXPLAYER];
  
  int treasures[MAXPLAYER], kills[MAXPLAYER], deaths[MAXPLAYER];
  
  bool alwaysuse = false;

  void recall() {
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
  
  transmatrix whereis[MAXPLAYER];
  transmatrix crosscenter[MAXPLAYER];
  double ccdist[MAXPLAYER];
  cell *ccat[MAXPLAYER];
  
  bool combo[MAXPLAYER];

  int cpid; // player id -- an extra parameter for player-related functions
  int cpid_edit; // cpid currently being edited
  
  movedir whereto[MAXPLAYER]; // player's target cell  

  double mdx[MAXPLAYER], mdy[MAXPLAYER]; // movement vector for the next move
  
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

#define SHMUPAXES_BASE 4
#define SHMUPAXES ((SHMUPAXES_BASE) + 4 * (MAXPLAYER))
#define SHMUPAXES_CUR ((SHMUPAXES_BASE) + 4 * playercfg)

const char* axemodes[SHMUPAXES] = {
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

int centerplayer = -1;

char* axeconfigs[24]; int numaxeconfigs;
int* dzconfigs[24];

string listkeys(int id) {
#if CAP_SDL
  string lk = "";
  for(int i=0; i<512; i++)
    if(vid.scfg.keyaction[i] == id)
      lk = lk + " " + SDL_GetKeyName(SDLKey(i));
#if CAP_SDLJOY
  for(int i=0; i<numsticks; i++) for(int k=0; k<SDL_JoystickNumButtons(sticks[i]) && k<MAXBUTTON; k++)
    if(vid.scfg.joyaction[i][k] == id) {
      lk = lk + " " + cts('A'+i)+"-B"+its(k);
      }
  for(int i=0; i<numsticks; i++) for(int k=0; k<SDL_JoystickNumHats(sticks[i]) && k<MAXHAT; k++)
    for(int d=0; d<4; d++)
      if(vid.scfg.hataction[i][k][d] == id) {
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
  char buf[64];
  sprintf(buf, " (%d $$$, %d kills, %d deaths)", 
    multi::treasures[id],
    multi::kills[id],
    multi::deaths[id]
    );
  return buf;
  }

void resetScores() {
  for(int i=0; i<MAXPLAYER; i++)
    multi::treasures[i] = multi::kills[i] = multi::deaths[i] = 0;
  }
 
bool configdead;

void handleConfig(int sym, int uni);

string player_count_name(int p) {
  return XLAT(
    p == 2 ? "two players" : 
    p == 3 ? "three players" : 
    p == 4 ? "four players" : 
    p == 5 ? "five players" : 
    p == 6 ? "six players" : 
    p == 7 ? "seven players" : 
    "one player");
  }

struct key_configurer {

  int sc;
  vector<string>& shmupcmdtable;
  int setwhat;

  key_configurer(int sc, vector<string>& sct) : sc(sc), shmupcmdtable(sct), setwhat(0) {}

  void operator() () {
      
    dialog::init(
      XLAT(sc == 1 ? "configure player 1" :
      sc == 2 ? "configure player 2" :
      sc == 3 ? "configure panning" :
      sc == 4 ? "configure player 3" :
      sc == 5 ? "configure player 4" :
      sc == 6 ? "configure player 5" :
      sc == 7 ? "configure player 6" :
      sc == 8 ? "configure player 7" : ""
      ));
  
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
          vid.scfg.keyaction[sym] = setwhat;
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
           vid.scfg.joyaction[joyid][button] = setwhat;
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
          vid.scfg.hataction[joyid][hat][dir] = setwhat;
          setwhat = 0;
          return true;
          }
        }
      return false;
      };
#endif
    }
  };

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
        axeconfigs[numaxeconfigs] = &(vid.scfg.axeaction[j][ax]);
        dzconfigs[numaxeconfigs] = &(vid.scfg.deadzoneval[j][ax]);
        char aa = *axeconfigs[numaxeconfigs];
        string what = configdead ? its(vid.scfg.deadzoneval[j][ax]) : XLAT(axemodes[aa%SHMUPAXES]);
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
          if(configdead) {
            int& dz = (*dzconfigs[xuni - 'a']);
            dz += int(shiftmul * 100);
            if(dz < 0) dz = 0;
            if(dz > 65000) dz = 65000;
            }
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

struct shmup_configurer {

  bool shmupcfg;
  int playercfg;
  
  shmup_configurer() { shmupcfg = shmup::on; playercfg = multi::players; }
  
  void operator()() {
  #if CAP_SDL
    cmode = sm::SHMUPCONFIG;
    dialog::init(SHMUPTITLE);
  
    dialog::addItem(player_count_name(playercfg), 'n');
  
    dialog::addItem(XLAT(shmupcfg ? "shoot'em up mode" : "turn-based mode"), 's');
    
    dialog::addItem(XLAT(shmup::on == shmupcfg && players == playercfg ?
          "continue playing"
        : "start a new game"), '0');
  
    if(shmupcfg || multi::alwaysuse || playercfg > 1)
      dialog::addItem(XLAT("configure player 1") + dsc(0), '1');
    else
      dialog::addBreak(100);
    if(playercfg > 1)
      dialog::addItem(XLAT("configure player 2") + dsc(1), '2');
    else if(playercfg == 1 && !shmupcfg)
      dialog::addSelItem(XLAT("input"), XLAT(multi::alwaysuse ? "config" : "default"), 'a');
    else
      dialog::addBreak(100);
    if(playercfg > 2)
      dialog::addItem(XLAT("configure player 3") + dsc(2), '3');
  #if CAP_SDLJOY
    else if(playercfg == 1 && !shmupcfg && !shmupcfg && !multi::alwaysuse)
      dialog::addItem(XLAT("old style joystick configuration"), 'b');
  #endif
    else dialog::addBreak(100);
    if(playercfg > 3)
      dialog::addItem(XLAT("configure player 4") + dsc(3), '4');
    else if(!shmup::on && !multi::alwaysuse) {
      dialog::addBoolItem(XLAT("smooth scrolling"), smooth_scrolling, 'c');
      }
    else dialog::addBreak(100);
      
    if(playercfg > 4)
      dialog::addItem(XLAT("configure player 5") + dsc(4), '5');
    else dialog::addBreak(100);
  
    if(playercfg > 5)
      dialog::addItem(XLAT("configure player 6") + dsc(5), '6');
    else dialog::addBreak(100);
  
    if(playercfg > 6)
      dialog::addItem(XLAT("configure player 7") + dsc(6), '7');
    else dialog::addBreak(100);
      
    if(shmupcfg || multi::alwaysuse || playercfg > 1)
      dialog::addItem(XLAT("configure panning and general keys"), 'p');
    else dialog::addBreak(100);
  
  #if CAP_SDLJOY
    if(numsticks > 0) {
      if(shmupcfg || multi::alwaysuse || playercfg > 1) 
        dialog::addItem(XLAT("configure joystick axes"), 'j');
      else dialog::addBreak(100);
      }
  #endif
  
    if(multi::players > 1) 
      dialog::addItem(XLAT("reset per-player statistics"), 'r');
    else dialog::addBreak(100);
    
    dialog::addBreak(50);
  
  #if CAP_CONFIG
    dialog::addItem(XLAT("save the configuration"), 'c');
  #endif
  
    dialog::addHelp();
  
    dialog::addBack();
    dialog::display();
    
    keyhandler = [this] (int sym, int uni) { return handleConfig(sym, uni); };
  #endif
    }

  void handleConfig(int sym, int uni) {
    auto& cmdlist = shmupcfg ? (DIM == 3 ? playercmds_shmup3 : playercmds_shmup) : playercmds_turn;
    
    #if CAP_SDL
    if(uni == '1') pushScreen(key_configurer(1, cmdlist));
    else if(uni == '2') pushScreen(key_configurer(2, cmdlist));
    else if(uni == 'p') pushScreen(key_configurer(3, DIM == 3 ? pancmds3 : pancmds));
    else if(uni == '3') pushScreen(key_configurer(4, cmdlist));
    else if(uni == '4') pushScreen(key_configurer(5, cmdlist));
    else if(uni == '5') pushScreen(key_configurer(6, cmdlist));
    else if(uni == '6') pushScreen(key_configurer(7, cmdlist));
    else if(uni == '7') pushScreen(key_configurer(8, cmdlist));
  #if CAP_SDLJOY
    else if(uni == 'j') pushScreen(joy_configurer(playercfg));
  #endif
    else if(uni == 'a') multi::alwaysuse = !multi::alwaysuse;
  #if CAP_SDLJOY
    else if(uni == 'b') pushScreen(showJoyConfig);
  #endif
    else if(uni == 'c') smooth_scrolling = !smooth_scrolling;
    else if(uni == 'r') 
      for(int i=0; i<MAXPLAYER; i++) 
        kills[i] = deaths[i] = treasures[i] = 0;
    else if(uni == 's' || uni == 't') 
      shmupcfg = !shmupcfg;
  #if CAP_CONFIG
    else if(uni == 'c')
      hr::saveConfig();
  #endif
    else if(uni == 'n' || uni == 'N') {
      playercfg += shiftmul > 0 ? 1 : -1;
      playercfg %= MAXPLAYER;
      if(playercfg <= 0) playercfg += MAXPLAYER;
      }
    else if(sym == SDLK_F1 || uni == '?' || uni == 'h') {
      gotoHelp("");

      help = XLAT(
        "Shmup (shoot'em up) mode: You can play a hyperbolic shoot'em up game. The game is based "
        "on the usual turn-based grid-based HyperRogue, but there are some changes. You fight by "
        "throwing knives, and you have three extra lives. There are no allies, so all Orbs "
        "related to allies give you extra lives instead (up to 5). Some other rules have been "
        "adapted too.\n\n");
  
      help += XLAT(
        "Multiplayer: Play cooperatively (locally); treasures, kills, and deaths are calculated "
        "for each player too, for more competitive play. Orbs and treasures are shared, orbs drain "
        "faster, knives recharge slower, and player characters are not allowed to separate.\n\n");
  
      help += XLAT(
        "Turn-based multiplayer: Turns are executed in parallel. A player can leave the game "
        "by pressing a designated key (useful when about to get killed or lost). The following "
        "Orbs work to bring such players back: ");
  
      help += XLATN(iinf[itOrbLife].name); help += ", ";
      help += XLATN(iinf[itOrbFriend].name); help += ", ";
      help += XLATN(iinf[itOrbUndeath].name); help += ", ";
      help += XLATN(iinf[itOrbTeleport].name); help += ", ";
      help += XLATN(iinf[itOrbSafety].name); help += "\n\n";
      
      help += XLAT("This menu can be also used to configure keys.\n\n");          
      }
    else if(doexiton(sym, uni)) {
      popScreen();
      auto sc = shmupcfg;
      auto pc = playercfg;
      if(shmup::on != sc || pc != players) dialog::do_if_confirmed([sc, pc] {
        if(shmup::on != sc) { 
          stop_game(); 
          switch_game_mode(rg::shmup); 
          resetScores(); 
          }
        if(pc != players) { 
          stop_game(); 
          players = pc;
          resetScores();
          }
        start_game();
        });
      }
    #endif
    }
  };

void configure() {
  pushScreen(shmup_configurer());
  }

#define NUMACT 128

enum pcmds {
  pcForward, pcBackward, pcTurnLeft, pcTurnRight,
  pcMoveUp, pcMoveRight, pcMoveDown, pcMoveLeft,
  pcFire, pcFace, pcFaceFire,
  pcDrop, pcCenter, pcOrbPower, pcOrbKey
  };
  
int actionspressed[NUMACT], axespressed[SHMUPAXES], lactionpressed[NUMACT];

void pressaction(int id) {
  if(id >= 0 && id < NUMACT)
    actionspressed[id]++;
  }

bool notremapped(int sym) {
  int k = vid.scfg.keyaction[sym];
  if(k == 0) return true;
  k /= 16;
  if(k > 3) k--; else if(k==3) k = 0;
  return k > multi::players;
  }

void initConfig() {
  
  char* t = vid.scfg.keyaction;
  
  t[(int)'w'] = 16 + 4;
  t[(int)'d'] = 16 + 5;
  t[(int)'s'] = 16 + 6;
  t[(int)'a'] = 16 + 7;

#if ISMOBILE==0
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

#if ISMOBILE==0
  t[SDLK_UP] = 48 ;
  t[SDLK_RIGHT] = 48 + 1;
  t[SDLK_DOWN] = 48 + 2;
  t[SDLK_LEFT] = 48 + 3;
  t[SDLK_PAGEUP] = 48 + 4;
  t[SDLK_PAGEDOWN] = 48 + 5;
  t[SDLK_HOME] = 48 + 6;
#endif

  vid.scfg.joyaction[0][0] = 16 + pcFire;
  vid.scfg.joyaction[0][1] = 16 + pcOrbPower;
  vid.scfg.joyaction[0][2] = 16 + pcDrop;
  vid.scfg.joyaction[0][3] = 16 + pcCenter;
  vid.scfg.joyaction[0][4] = 16 + pcFace;
  vid.scfg.joyaction[0][5] = 16 + pcFaceFire;

  vid.scfg.joyaction[1][0] = 32 + pcFire;
  vid.scfg.joyaction[1][1] = 32 + pcOrbPower;
  vid.scfg.joyaction[1][2] = 32 + pcDrop;
  vid.scfg.joyaction[1][3] = 32 + pcCenter;
  vid.scfg.joyaction[1][4] = 32 + pcFace;
  vid.scfg.joyaction[1][5] = 32 + pcFaceFire;

  vid.scfg.axeaction[0][0] = 4;
  vid.scfg.axeaction[0][1] = 5;
  vid.scfg.axeaction[0][3] = 2;
  vid.scfg.axeaction[0][4] = 3;

  vid.scfg.axeaction[1][0] = 8;
  vid.scfg.axeaction[1][1] = 9;
  
  // ULRD
  vid.scfg.hataction[0][0][0] = 16 + 0;
  vid.scfg.hataction[0][0][1] = 16 + 3;
  vid.scfg.hataction[0][0][2] = 16 + 1;
  vid.scfg.hataction[0][0][3] = 16 + 2;
  vid.scfg.hataction[0][1][0] = 16 + 4;
  vid.scfg.hataction[0][1][1] = 16 + 7;
  vid.scfg.hataction[0][1][2] = 16 + 5;
  vid.scfg.hataction[0][1][3] = 16 + 6;

  vid.scfg.hataction[1][0][0] = 32 + 0;
  vid.scfg.hataction[1][0][1] = 32 + 3;
  vid.scfg.hataction[1][0][2] = 32 + 1;
  vid.scfg.hataction[1][0][3] = 32 + 2;
  vid.scfg.hataction[1][1][0] = 32 + 4;
  vid.scfg.hataction[1][1][1] = 32 + 7;
  vid.scfg.hataction[1][1][2] = 32 + 5;
  vid.scfg.hataction[1][1][3] = 32 + 6;

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
  
  addsaver(multi::players, "mode-number of players");
  addsaver(alwaysuse, "use configured keys");  
  // unfortunately we cannot use key names here because SDL is not yet initialized
  for(int i=0; i<512; i++)
    addsaver(vid.scfg.keyaction[i], string("key:")+its(i));
  for(int i=0; i<MAXJOY; i++) {
    string pre = "joystick "+cts('A'+i);
    for(int j=0; j<MAXBUTTON; j++) 
      addsaver(vid.scfg.joyaction[i][j], pre+"-B"+its(j));
    for(int j=0; j<MAXAXE; j++) {
      addsaver(vid.scfg.axeaction[i][j], pre+" axis "+its(j));
      addsaver(vid.scfg.deadzoneval[i][j], pre+" deadzone "+its(j));
      }
    for(int j=0; j<MAXHAT; j++) for(int k=0; k<4; k++) {
      addsaver(vid.scfg.hataction[i][j][k], pre+" hat "+its(j)+" "+"URDL"[k]);
      }
    }
  for(int i=0; i<7; i++) addsaver(multi::scs[i], "player"+its(i));
  }

#if CAP_CONFIG
void scanchar(FILE *f, char& c) {
  int i = c;
  int err = fscanf(f, "%d", &i);
  if(err == 1) c = i;
  }
  
void loadConfig(FILE *f) {
  int xvernum;
  int err = fscanf(f, "%d %d", &xvernum, &multi::players);
  if(multi::players < 1 || multi::players > MAXPLAYER)
    multi::players = 1;
  if(err != 2) return;
  if(xvernum >= 8990) { int b=alwaysuse; err=fscanf(f, " %d", &b); alwaysuse = b; }
  for(int i=0; i<512; i++) scanchar(f, vid.scfg.keyaction[i]);
  for(int i=0; i<MAXJOY; i++) for(int j=0; j<MAXBUTTON; j++) scanchar(f, vid.scfg.joyaction[i][j]);
  for(int i=0; i<MAXJOY; i++) for(int j=0; j<MAXAXE; j++) scanchar(f, vid.scfg.axeaction[i][j]);
  
  if(xvernum >= 9007)
    for(int i=0; i<MAXJOY; i++) for(int j=0; j<MAXAXE; j++) err = fscanf(f, " %d", &vid.scfg.deadzoneval[i][j]);
  for(int i=0; i<MAXJOY; i++) for(int j=0; j<MAXHAT; j++) for(int k=0; k<4; k++)
    scanchar(f, vid.scfg.hataction[i][j][k]);
  for(int i=0; i<(xvernum < 8990 ? 4 : 7); i++) loadcs(f, scs[i], xvernum);
  }
#endif

void handleInput(int delta) {
#if CAP_SDL
  double d = delta / 500.;

  Uint8 *keystate = SDL_GetKeyState(NULL);

  for(int i=0; i<NUMACT; i++) 
    lactionpressed[i] = actionspressed[i],
    actionspressed[i] = 0;

  for(int i=0; i<SHMUPAXES; i++) axespressed[i] = 0;
  
  for(int i=0; i<SDLK_LAST; i++) if(keystate[i]) 
    pressaction(vid.scfg.keyaction[i]);

#if CAP_SDLJOY  
  for(int j=0; j<numsticks; j++) {

    for(int b=0; b<SDL_JoystickNumButtons(sticks[j]) && b<MAXBUTTON; b++)
      if(SDL_JoystickGetButton(sticks[j], b))
        pressaction(vid.scfg.joyaction[j][b]);

    for(int b=0; b<SDL_JoystickNumHats(sticks[j]) && b<MAXHAT; b++) {
      int stat = SDL_JoystickGetHat(sticks[j], b);
      if(stat & SDL_HAT_UP) pressaction(vid.scfg.hataction[j][b][0]);
      if(stat & SDL_HAT_RIGHT) pressaction(vid.scfg.hataction[j][b][1]);
      if(stat & SDL_HAT_DOWN) pressaction(vid.scfg.hataction[j][b][2]);
      if(stat & SDL_HAT_LEFT) pressaction(vid.scfg.hataction[j][b][3]);
      }
    
    for(int b=0; b<SDL_JoystickNumAxes(sticks[j]) && b<MAXAXE; b++) {
      int value = SDL_JoystickGetAxis(sticks[j], b);
      int dz = vid.scfg.deadzoneval[j][b];
      if(value > dz) value -= dz; else if(value < -dz) value += dz;
      else value = 0;
      axespressed[vid.scfg.axeaction[j][b] % SHMUPAXES] += value;
      }
    }
#endif

  if(keystate[SDLK_LCTRL] || keystate[SDLK_RCTRL]) d /= 5;
  
  double panx = 
    actionspressed[49] - actionspressed[51] + axespressed[2] / 32000.0;
  double pany = 
    actionspressed[50] - actionspressed[48] + axespressed[3] / 20000.0;
    
  double panspin = actionspressed[52] - actionspressed[53] + axespressed[1] / 20000.0;
  
  double panmove = actionspressed[59] - actionspressed[60] + axespressed[4] / 20000.0;
  
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
    pushScreen(showMainMenu);
    
  panx *= d;
  pany *= d;
  panspin *= d;
  panmove *= d;
  
  if(lctrlclick) {
    panx += mouseaim_x / 2;
    pany += mouseaim_y / 2;
    mouseaim_x = mouseaim_y = 0;
    }

  if(panx || pany || panspin || (DIM == 3 && panmove)) {
    if(DIM == 2)
      View = xpush(-panx) * ypush(-pany) * spin(panspin) * View;
    else
      View = cspin(0, 2, panx*2) * cspin(0, 1, pany*2) * spin(panspin) * cpush(2, panmove*2) * View;
    playermoved = false;
    }
#endif
  }

  int tableid[7] = {1, 2, 4, 5, 6, 7, 8};


  void leaveGame(int i) {
    multi::player[i].at = NULL;
    multi::deaths[i]++;
    revive_queue.push_back(i);
    checklastmove();
    }

  bool playerActive(int p) {
    if(multi::players == 1 || shmup::on) return true;
    return player[p].at;
    }
  
  int activePlayers() {
    int q = 0;
    for(int i=0; i<players; i++) if(playerActive(i)) q++;
    return q;
    }
  
  cell *multiPlayerTarget(int i) {
    cellwalker cwti = multi::player[i];
    if(!cwti.at) return NULL;
    int dir = multi::whereto[i].d;
    if(dir == MD_UNDECIDED) return NULL;
    if(dir == MD_USE_ORB) return multi::whereto[i].tgt;
    if(dir >= 0) 
      cwti = cwti + dir + wstep;
    return cwti.at;
    }
  
  void checklastmove() {
    for(int i=0; i<numplayers(); i++) if(playerActive(i)) {
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
  
  void handleMulti(int delta) {
    multi::handleInput(delta);
    
    transmatrix bcwtV = cwtV;
    cellwalker bcwt = cwt;
    
    bool alldecided = !needinput;
    
    if(multi::players == 1) {
      multi::cpid = 0;
      multi::whereis[0] = cwtV;
      multi::player[0] = cwt;
      }
    
    for(int i=0; i<numplayers(); i++) if(playerActive(i)) {
    
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
  
      if(DIM == 2) {
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

      for(int i=0; i<numplayers(); i++) if(playerActive(i)) {
        origpos[i] = player[i].at;
        origtarget[i] = multiPlayerTarget(i);
        }
  
      for(int i=0; i<numplayers(); i++) if(playerActive(i))
      for(int j=0; j<numplayers(); j++) if(playerActive(j)) if(i != j) {
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
        monstersTurn();
        checklastmove();
        }
      }
    }
  
  void mousemovement(cell *c) {
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
            }
          }
        }
    
    needinput = 
      ((countplayers == 2 && !countplayers_undecided) || countplayers_undecided >= 2);
    }
  
  }

/*
const char *lastprofile = "";
int lt = 0;

void profile(const char *buf) {
  int gt = SDL_GetTicks();
  printf("%4d %s\n", gt - lt, lastprofile);
  lt = gt;
  lastprofile = buf;
  }
*/

#define SCALE scalefactor
#define SCALE2 (SCALE*SCALE)

namespace shmup {

using namespace multi;

eItem targetRangedOrbKey(orbAction a);
eItem keyresult[MAXPLAYER];

ld fabsl(ld x) { return x>0?x:-x; }

bool on = false, safety = false;

bool lastdead = false;

struct monster;

multimap<cell*, monster*> monstersAt;

typedef multimap<cell*, monster*>::iterator mit;

vector<monster*> active, nonvirtual, additional;

cell *findbaseAround(hyperpoint p, cell *around) {
  #if MAXMDIM == 4 && CAP_BT
  // this needs a more precise algorithm because the cells have curved faces
  if(DIM == 3 && binarytiling) {
    hyperpoint h = binary::deparabolic3(inverse(ggmatrix(around)) * p);
    if(h[0] < -log(2)/2) return around->cmove((h[1] > 0 ? 1 : 0) + (h[2] > 0 ? 2 : 0));
    if(h[1] < -1) return around->cmove(4);
    if(h[1] > +1) return around->cmove(5);
    if(h[2] < -1) return around->cmove(6);
    if(h[2] > +1) return around->cmove(7);
    if(h[0] > +log(2)/2) return around->cmove(8);
    return around;
    }
  #endif
  cell *best = around;
  double d0 = intval(p, ggmatrix(around) * C0);
  for(int i=0; i<around->type; i++) {
    cell *c2 = around->move(i);
    if(c2) {
      double d1 = intval(p, ggmatrix(c2) * C0);
      if(d1 < d0) { best = c2; d0 = d1; }
      }
    }
  return best;
  }

cell *findbaseAround(const transmatrix& H, cell *around) {
  return findbaseAround(tC0(H), around);
  }

cell *findbaseAroundRepeat(hyperpoint p, cell *around) {
  while(true) {
    cell *c = findbaseAround(p, around);
    if(c == around) return c;
    around = c;
    }
  }

cell *findbaseAroundRepeat(const transmatrix& H, cell *around) {
  return findbaseAroundRepeat(tC0(H), around);
  }

/* double distance(hyperpoint h) {
  h = spintox(h) * h;
  return asinh(h[2]);
  } */

void monster::store() {
  monstersAt.insert(make_pair(base, this));
  }

void monster::findpat() {
  isVirtual = !gmatrix.count(base);
  if(!isVirtual) pat = gmatrix[base] * at;
  else pat = at;
  }

cell *monster::findbase(const transmatrix& T) {
  if(isVirtual) {
    cell *c = base;
    auto cT = T;
    virtualRebase(c, cT, true);
    return c;
    }
  else return findbaseAround(T, base);
  }

void monster::rebasePat(const transmatrix& new_pat) {
  if(isVirtual) {
    at = new_pat;
    virtualRebase(this, true);
    fixmatrix(at); pat = at;
    return;
    }
  if(among(geometry, gZebraQuotient, gTorus, gKleinQuartic, gBolza, gBolza2, gMinimal)) {
    at = inverse(gmatrix[base]) * new_pat;
    virtualRebase(this, true);
    fixmatrix(at);
    return;
    }
  pat = new_pat;
  cell *c2 = findbase(pat);
  // if(c2 != base) printf("rebase %p -> %p\n", base, c2);
  base = c2;
  at = inverse(gmatrix[c2]) * pat;
  fixmatrix(at);
  fixelliptic(at);
  }

bool trackroute(monster *m, transmatrix goal, double spd) {
  cell *c = m->base;
  
  // queuepoly(goal, shGrail, 0xFFFFFFC0);

  transmatrix mat = inverse(m->pat) * goal;
  
  transmatrix mat2 = spintox(mat*C0) * mat;
  
  double d = 0, dist = asinh(mat2[0][2]);

  while(d < dist) {
    d += spd;
    transmatrix nat = m->pat * rspintox(mat * C0) * xpush(d); 

    // queuepoly(nat, shKnife, 0xFFFFFFC0);

    cell *c2 = findbaseAround(nat, c);
    if(c2 != c && !passable_for(m->type, c2, c, P_CHAIN | P_ONPLAYER)) {
      return false;
      }
    c = c2;
    }
  return true;
  }

monster *pc[MAXPLAYER], *mousetarget, *lmousetarget;

int curtime, nextmove, nextdragon;

bool isBullet(monster *m) { 
  return isBulletType(m->type);
  }
bool isPlayer(monster *m) { return m->type == moPlayer; }
bool isMonster(monster *m) { return m->type != moPlayer && m->type != moBullet; }

hookset<bool(shmup::monster*)> *hooks_kill;

void killMonster(monster* m, eMonster who_kills, int flags = 0) {
  int tk = tkills();
  if(callhandlers(false, hooks_kill, m)) return;
  if(m->dead) return;
  m->dead = true;
  if(isBullet(m) || isPlayer(m)) return;
  m->stk = m->base->monst;
  if(m->inBoat && isWatery(m->base)) {
    m->base->wall = waBoat;
    m->base->mondir = 0;
    m->inBoat = false;
    }
  else if(m->inBoat && m->base->wall == waNone && (
    (m->base->land == laOcean || m->base->land == laLivefjord))) {
    m->base->wall = waStrandedBoat;
    m->base->mondir = 0;
    m->inBoat = false;
    }
  m->base->monst = m->type;
  killMonster(m->base, who_kills, flags);
  m->base->monst = m->stk;
  if(multi::cpid >= 0)
    multi::kills[multi::cpid] += tkills() - tk;
  }

void pushmonsters() {
  for(monster *m: nonvirtual) {
    m->notpushed = isPlayer(m) || m->dead || (m->base->monst && m->base->monst != m->type);
    if(!m->notpushed) {
      m->stk = m->base->monst;
      m->base->monst = m->type;
      }
    }
  }

void popmonsters() {
  for(int i=isize(nonvirtual)-1; i>=0; i--) {
    monster *m = nonvirtual[i];
    if(!m->notpushed) {
      if(m->type == m->base->monst)
        m->base->monst = m->stk;
      else {
        m->dead = true; // already killed
        // also kill all the other monsters pushed there
        for(int j=0; j<i; j++) {
          monster *m2 = active[j];
          if(m2->base == m->base && !m2->notpushed) 
            killMonster(m2, moNone);
          }
        }
      }
    }
  }

void degradeDemons() {
  for(monster* m: nonvirtual) {
    if(m->type == moGreater) m->type = moLesser;
    if(m->stk == moGreater) m->type = moLesser;
    }
  }

// we need these for the Mimics!
double playerturn[MAXPLAYER], playergo[MAXPLAYER], playerstrafe[MAXPLAYER], playerturny[MAXPLAYER];
bool playerfire[MAXPLAYER];

void awakenMimics(monster *m, cell *c2) {
  for(auto& mi: mirror::mirrors) {
    cell *c = mi.second.at;

    transmatrix mirrortrans = Id;    
    if(mi.second.mirrored) mirrortrans[0][0] = -1;

    if(!gmatrix.count(c)) continue;
    monster *m2 = new monster;
    m2->base = c;
    
    if(isBullet(m)) {
      m2->parenttype = m->parenttype;
      m2->type = m->type;
      m2->vel = m->vel;
      m2->parent = m->parent;
      m2->pid = m->pid;
      }
    else
      m2->type = moMimic;
    
    hyperpoint H = inverse(gmatrix[c2]) * gmatrix[c] * C0;
    
    transmatrix xfer = rgpushxto0(H);

    if(mi.second.mirrored) {
      hyperpoint H2 = spintox(H) * H;
      xfer = rspintox(H) * rpushxto0(H2) * mirrortrans * spintox(H);
      }

    m2->pat = gmatrix[c2] * xfer * inverse(gmatrix[c2]) * m->pat;
      
    m2->at = inverse(gmatrix[c]) * m2->pat;
    m2->pid = cpid;
    
    additional.push_back(m2);

    // if you don't understand it, don't worry,
    // I don't understand it either
    }
  mirror::mirrors.clear();
  }

int visibleAt;

void visibleFor(int t) {
  visibleAt = max(visibleAt, curtime + t);
  }

void shootBullet(monster *m) {
  monster* bullet = new monster;
  bullet->base = m->base;
  bullet->at = m->at;
  bullet->type = moBullet;
  bullet->parent = m;
  bullet->pid = m->pid;
  bullet->parenttype = m->type;
  additional.push_back(bullet);
  
  eItem orbdir[8] = {
    itNone, itOrbSide1, itOrbThorns, itOrbSide2, itOrbSide3, itOrbSide2, itOrbThorns, itOrbSide1
    };
  
  for(int i=1; i<8; i++) if(markOrb(orbdir[i])) {
    monster* bullet = new monster;
    bullet->base = m->base;
    bullet->at = m->at * spin(M_PI/4*i);
    bullet->type = moBullet;
    bullet->parent = m;
    bullet->pid = m->pid;
    bullet->parenttype = m->type;
    additional.push_back(bullet);
    }
  }

void killThePlayer(eMonster m) {
  if(cpid >= 0 && cpid < MAXPLAYER && pc[cpid])
    pc[cpid]->dead = true;
  }

monster *playerCrash(monster *who, hyperpoint where) {
  if(who->isVirtual) return NULL;
  // in the racing mode, neither crashing nor getting too far away is a problem
  if(racing::on) return NULL;
  for(int j=0; j<players; j++) if(pc[j] && pc[j]!=who) {
    if(pc[j]->isVirtual) continue;
    double d = intval(pc[j]->pat*C0, where);
    if(d < 0.1 * SCALE2 || d > 100) return pc[j];
    }
  return NULL;
  }

void oceanCurrents(transmatrix& nat, monster *m, int delta) {
  cell *c = m->base;
  if(c->land == laWhirlpool) {
    for(int i=0; i<c->type; i++) {
      cell *c2 = c->move(i);
      if(!c2 || !gmatrix.count(c2)) continue;
      
      double spd = 0;
      
      if(celldistAlt(c2) < celldistAlt(c)) 
        spd = SCALE * delta / 3000.;
      else if(c2 == whirlpool::get(c, 1))
        spd = SCALE * delta / 900.;
        
      if(spd) {
        transmatrix goal = gmatrix[c2];

        // transmatrix t = spintox(H) * xpush(delta/300.) * rspintox(H);

        hyperpoint H = inverse(m->pat) * goal * C0;
        nat = nat * rspintox(H);
        nat = nat * xpush(spd);
        nat = nat * spintox(H);
        }
      }
    }
  }

bool airCurrents(transmatrix& nat, monster *m, int delta) {
  bool carried = false;
  cell *c = m->base;
  #if CAP_COMPLEX2
  if(c->land == laWestWall) {
    cell *c2 = ts::left_of(c, westwall::coastvalEdge1);
      
    double spd = SCALE * delta / 900.;
        
    if(m->type == moVoidBeast) spd = -spd;
    if(spd) {
      transmatrix goal = gmatrix[c2];

      // transmatrix t = spintox(H) * xpush(delta/300.) * rspintox(H);

      hyperpoint H = inverse(m->pat) * goal * C0;
      nat = nat * rspintox(H);
      nat = nat * xpush(spd);
      nat = nat * spintox(H);
      carried = true; 
      }
    }
  #endif
  if(c->land == laWhirlwind) {
    whirlwind::calcdirs(c);
    for(int i=0; i<whirlwind::qdirs; i++) {
      cell *c2 = c->move(whirlwind::dto[i]);
      if(!c2 || !gmatrix.count(c2)) continue;
      
      double spd = SCALE * delta / 900.;
        
      if(m->type == moVoidBeast) spd = -spd;
      if(spd) {
        transmatrix goal = gmatrix[c2];

        // transmatrix t = spintox(H) * xpush(delta/300.) * rspintox(H);

        hyperpoint H = inverse(m->pat) * goal * C0;
        nat = nat * rspintox(H);
        nat = nat * xpush(spd);
        nat = nat * spintox(H);
        carried = true; 
        }
      }
    }
  #if CAP_FIELD
  if(c->land == laBlizzard) {
    int wmc = windmap::at(c);
    forCellEx(c2, c) { 
      if(!c2 || !gmatrix.count(c2)) continue;
      int z = (windmap::at(c2) - wmc) & 255;
      if(z >= 128) z -= 256;
      if(m->type == moVoidBeast) z = -z;
      if(z < windmap::NOWINDFROM && z > -windmap::NOWINDFROM) {
        transmatrix goal = gmatrix[c2];

        // transmatrix t = spintox(H) * xpush(delta/300.) * rspintox(H);

        hyperpoint H = inverse(m->pat) * goal * C0;
        nat = nat * rspintox(H);
        nat = nat * xpush(z * SCALE * delta / 50000.);
        nat = nat * spintox(H);
        carried = true; 
        }
      }
    }
  #endif
  return carried;
  }

void roseCurrents(transmatrix& nat, monster *m, int delta) {
  if(ignoresSmell(m->type)) return;
  cell *c = m->base;
  
  int qty = 0;

  for(int i=0; i<c->type; i++) {
    cell *c2 = c->move(i);
    if(c2 && rosedist(c2) == 2) qty++;
    }
  
  for(int i=0; i<c->type; i++) {
    cell *c2 = c->move(i);
    if(!c2 || !gmatrix.count(c2)) continue;
    if(rosedist(c2) != 2) continue;
    
    double spd = SCALE * delta / 300. / qty;
        
    if(spd) {
      transmatrix goal = gmatrix[c2];

      // transmatrix t = spintox(H) * xpush(delta/300.) * rspintox(H);

      hyperpoint H = inverse(m->pat) * goal * C0;
      nat = nat * rspintox(H);
      nat = nat * xpush(spd);
      nat = nat * spintox(H);
      }
    }
  }

hyperpoint keytarget(int i) {
  double d = 2 + sin(curtime / 350.);
  return pc[i]->pat * cpush0(DIM == 3 ? 2 : 0, d * scalefactor);
  }

/* int charidof(int pid) {
  if(players == 1) return bak_charid;
  if(players == 2 || players == 4) return pid;
  if(players == 3) return pid < 2 ? pid : 2+(bak_charid&1);
  return 0;
  } */

double getSwordSize() { return 0.7255; }
double getHornsSize() { return 0.33; }

hyperpoint swordpos(int id, bool rev, double frac) {
  return pc[id]->pat * xspinpush0(pc[id]->swordangle, (rev?-frac:frac) * getSwordSize());
  }

hyperpoint hornpos(int id) {
  return pc[id]->pat * xpush0(getHornsSize());
  }

#define IGO 9

double igospan[IGO+1] = { 0, 
  M_PI/6, -M_PI/6, 
  M_PI/4, -M_PI/4,
  M_PI/3, -M_PI/3,
  M_PI/2.1, -M_PI/2.1,
  0
  };

bool swordKills(eMonster m) {
  return 
    m != moHedge && m != moMetalBeast && m != moMetalBeast2
    && m != moTortoise && m != moGreater && m != moRoseBeauty
    && m != moReptile && !isBull(m) && m != moButterfly &&
    m != moSalamander && m != moTerraWarrior && m != moBrownBug;
  }

bool hornKills(eMonster m) {
  return 
    m != moHedge && m != moMetalBeast && m != moMetalBeast2
    && m != moTortoise && m != moGreater && m != moSkeleton
    && m != moDraugr && m != moRoseBeauty
    && m != moReptile && !isBull(m) && m != moButterfly && !isBulletType(m)
    && m != moPalace && m != moFatGuard && m != moVizier &&
    m != moSalamander && m != moTerraWarrior && m != moBrownBug;
  }

queue<pair<int, cell*>> traplist, firetraplist;

void activateArrow(cell *c) {
  if(isCentralTrap(c))
    traplist.emplace(ticks + 500, c);
  }

monster arrowtrap_fakeparent;

void doTraps() {
  while(true) { 
    if(traplist.empty()) break;
    auto t = traplist.front();
    if(t.first > ticks) break;
    int d = t.second->wparam;
    if(d == 2) {
      auto tl = traplimits(t.second);
      for(int i=1; i<4; i++) if(tl[i]) tl[i]->wparam = 3;
      traplist.emplace(t.first + 500, t.second);
      
      for(int i=0; i<5; i += 4) try {
        transmatrix& tu = gmatrix.at(tl[i]);
        transmatrix& tv = gmatrix.at(tl[4-i]);
        monster* bullet = new monster;
        bullet->base = tl[i];
        bullet->at = rspintox(inverse(tu) * tC0(tv));
        bullet->type = moArrowTrap;
        bullet->parent = &arrowtrap_fakeparent;
        bullet->pid = 0;
        bullet->parenttype = moArrowTrap;
        additional.push_back(bullet);
        }
      catch(out_of_range&) {}
      }
    else if(d == 3) {
      auto tl = traplimits(t.second);
      for(int i=1; i<4; i++) if(tl[i]) tl[i]->wparam = 0;
      }
    traplist.pop();
    }

  while(true) { 
    if(firetraplist.empty()) break;
    auto t = firetraplist.front();
    if(t.first > ticks) return;
    int d = t.second->wparam;
    if(d == 2) {
      t.second->wparam = 0;
      t.second->wall = waNone;
      explosion(t.second, 5, 10);
      }
    firetraplist.pop();
    }
  }

bool hornStuns(eMonster m) {
  return !isBulletType(m) && m != moRoseBeauty;
  }

bool noncrashable(monster *m, monster *by) {
  eMonster mt = m->type;
  if(mt == moGreater) return true;
  if(mt == moDraugr && by->type != moDraugr) return true;
  if(isBull(mt)) return true;
  if(mt == moReptile) return true;
  if(mt == moSalamander) return true;
  if(mt == moRoseBeauty && by->type != moRoseLady) return true;
  if(mt == moTortoise) return true;
  if(mt == moTerraWarrior) return true;
  if(mt == moSkeleton) return true;
  return false;
  }

int bulltime[MAXPLAYER];

// set to P_MIRRORWALL to allow the PCs to go through mirrors
static const int reflectflag = P_MIRRORWALL;

void movePlayer(monster *m, int delta) {

  cpid = m->pid;

  #if CAP_RACING
  if(racing::on && cpid != subscreens::current_player) return;
  #endif
  
  double mturn = 0, mgo = 0, mdx = 0, mdy = 0;
  
  bool shotkey = false, dropgreen = false, facemouse = false;
  if(facemouse) {
    // silence warning that facemouse unused
    }
  
  int b = 16*tableid[cpid];

    for(int i=(DIM == 3 ? 4 : 0); i<8; i++) if(actionspressed[b+i]) playermoved = true;
  
  int jb = 4*tableid[cpid];
  for(int i=0; i<4; i++) if(axespressed[jb+i]) playermoved = true;
  
#if !ISMOBILE
  mgo = actionspressed[b+pcForward] - actionspressed[b+pcBackward] + axespressed[jb+2]/30000.;
  mturn = actionspressed[b+pcTurnLeft] - actionspressed[b+pcTurnRight] + axespressed[jb+3]/30000.;
  mdx = actionspressed[b+pcMoveRight] - actionspressed[b+pcMoveLeft] + axespressed[jb]/30000.;
  mdy = actionspressed[b+pcMoveDown] - actionspressed[b+pcMoveUp] + axespressed[jb+1]/30000.;
  
  shotkey = actionspressed[b+pcFire] || actionspressed[b+pcFaceFire];
  facemouse = actionspressed[b+pcFace] || actionspressed[b+pcFaceFire];
  dropgreen = actionspressed[b+pcDrop];
  
#else
  mdx = mdy = mgo = mturn = 0;
  facemouse = shotkey = false;
  dropgreen = getcstat == 'g';
  using namespace shmupballs;

  if(clicked && hypot(mousex - xfire, mousey - yb) < rad) {
    shotkey = true;
    mdx = (mousex - xfire) / (rad/2.);
    mdy = (mousey - yb) / (rad/2.);
    }
  if(clicked && hypot(mousex - xmove, mousey - yb) < rad) {
    mdx = (mousex - xmove) / (rad/2.);
    mdy = (mousey - yb) / (rad/2.);
    }
  #endif
  
  if(actionspressed[b+pcOrbPower] && !lactionpressed[b+pcOrbPower] && mouseover) {
    cwt.at = m->base;
    targetRangedOrb(mouseover, roKeyboard);
    }

#if !ISMOBILE
  if(haveRangedOrb()) {
    cwt.at = m->base;
    if(actionspressed[b+pcOrbKey] && !lactionpressed[b+pcOrbKey])
      keyresult[cpid] = targetRangedOrbKey(roKeyboard);
    else
      keyresult[cpid] = targetRangedOrbKey(roCheck);
    }
  else
#endif
    keyresult[cpid] = itNone;

  if(actionspressed[b+pcCenter]) {
    if(!racing::on) {
      centerplayer = cpid; centerpc(100); playermoved = true; 
      }
    #if CAP_RACING
    if(racing::on) 
      racing::player_relative = !racing::player_relative;
    #endif
    }
  
  transmatrix nat = m->pat;
  
  // if(ka == b+pcOrbPower) dropgreen = true;
  
  // if(mturn > 1) mturn = 1;
  // if(mturn < -1) mturn = -1;
  
  #if CAP_RACING
  if(racing::on) {
    if(DIM == 2) {
      if(abs(mdy) > abs(mgo)) mgo = -mdy;
      if(abs(mdx) > abs(mturn)) mturn = -mdx;
      mdx = mdy = 0;
      }
    facemouse = shotkey = dropgreen = false;
    if(ticks < racing::race_start_tick || !racing::race_start_tick) (DIM == 2 ? mgo : mdy) = 0;
    }
  #endif
  
  playerturn[cpid] = mturn * delta / 150.0;

  if(DIM == 2) {
    double mdd = hypot(mdx, mdy);
    
    if(mdd > 1e-6) {
      hyperpoint jh = hpxy(mdx/100.0, mdy/100.0);
      hyperpoint ctr = m->pat * C0;
  
      if(sphere && vid.alpha > 1.001) for(int i=0; i<3; i++) ctr[i] = -ctr[i];
  
      hyperpoint h = inverse(m->pat) * rgpushxto0(ctr) * jh;
      
      playerturn[cpid] = -atan2(h[1], h[0]);
      mgo += mdd;
      }
    }

#if CAP_SDL
  Uint8 *keystate = SDL_GetKeyState(NULL);
  bool forcetarget = (keystate[SDLK_RSHIFT] | keystate[SDLK_LSHIFT]);
  if(((mousepressed && !forcetarget) || facemouse) && delta > 0 && !mouseout() && !racing::on) {
    // playermoved = true;
    hyperpoint h = inverse(m->pat) * mouseh;
    playerturn[cpid] = -atan2(h[1], h[0]);
    // nat = nat * spin(alpha);
    // mturn += alpha * 150. / delta;
    }
#endif

  bool blown = m->blowoff > curtime;
    
  if(playerturn[cpid] && canmove && !blown && DIM == 2) {
    m->swordangle -= playerturn[cpid];
    nat = nat * spin(playerturn[cpid]);
    }
  transmatrix nat0 = nat;
  
  if(m->base->land == laWhirlpool && !markOrb(itOrbWater))
    oceanCurrents(nat, m, delta);
    
  airCurrents(nat, m, delta);
    
  if(rosedist(m->base) == 1)
    roseCurrents(nat, m, delta);
    
  if(!canmove) mgo = 0;
  
  if(DIM == 2) {
    if(mgo > 1) mgo = 1;
    if(mgo < -1) mgo = -1;
    if(racing::on) {
      // braking is more efficient
      if(m->vel * mgo < 0) mgo *= 3;
      m->vel += mgo * delta / 600;
      playergo[cpid] = m->vel * SCALE * delta / 600;
      }
    
    else {
      playergo[cpid] = mgo * SCALE * delta / 600;
      }
    }
  
  else if(DIM == 3) {
    if(mdy > 1) mdy = 1;
    if(mdy < -1) mdy = -1;
    if(mdx > 1) mdx = 1;
    if(mdx < -1) mdx = -1;
    if(racing::on) {
      if(m->vel * -mdy < 0) mdy *= 3;
      m->vel += -mdy * delta / 600;
      playergo[cpid] = m->vel * SCALE * delta / 600;
      playerstrafe[cpid] = m->vel * mdx * SCALE * delta / 600;
      }
    else {
      playergo[cpid] = -mdy * SCALE * delta / 600;
      playerstrafe[cpid] = mdx * SCALE * delta / 600;
      }
    playerturn[cpid] = mgo * SCALE * delta / 200;
    playerturny[cpid] = mturn * SCALE * delta / 200;

    if(!lctrlclick && cpid == 0) {
      playerturn[cpid] += mouseaim_x;
      playerturny[cpid] += mouseaim_y;
      mouseaim_x = mouseaim_y = 0;
      }
    }
  
  if(playergo[cpid] && markOrb(itOrbDash)) playergo[cpid] *= 1.5;

  bool go = false; 
  
  cell *c2 = m->base;
  
  if(blown) {
    playergo[cpid] = -SCALE * delta / 1000.;
    playerturn[cpid] = 0;
    }

  m->footphase += playergo[cpid];

  if(isReptile(m->base->wall)) m->base->wparam = reptilemax();
  
  int steps = 1 + abs(int(playergo[cpid] / (.2 * scalefactor)));
  
  playergo[cpid] /= steps;
  
  nextstep:

  transmatrix nat1 = nat;
    
  for(int igo=0; igo<IGO && !go; igo++) {
  
    go = true;
    
    if(DIM == 3)
      nat = nat1 * cpush(0, playerstrafe[cpid]) * cpush(2, playergo[cpid]) * cspin(0, 2, playerturn[cpid]) * cspin(1, 2, playerturny[cpid]);
    else if(playergo[cpid]) 
      nat = nat1 * spin(igospan[igo]) * xpush(playergo[cpid]) * spin(-igospan[igo]);
    
    // spin(span[igo]) * xpush(playergo[cpid]) * spin(-span[igo]);
  
    c2 = m->findbase(nat);
    if(reflectflag & P_MIRRORWALL) reflect(c2, m->base, nat);
    
    // don't have several players in one spot
    // also don't let them run too far from each other!
    monster* crashintomon = NULL;
    
    if(!m->isVirtual) {
      crashintomon = playerCrash(m, nat*C0);
      for(monster *m2: nonvirtual) if(m2!=m && m2->type == passive_switch) {
        double d = intval(m2->pat*C0, nat*C0);
        if(d < SCALE2 * 0.2) crashintomon = m2;
        }
      }
    if(crashintomon) go = false;
  
    if(go && c2 != m->base) {

      if(c2->wall == waLake && markOrb(itOrbWinter) && !nonAdjacent(c2, m->base)) {
        c2->wall = waFrozenLake;
        if(HEAT(c2) > .5) HEAT(c2) = .5;
        }
  
      else if(c2->wall == waBigStatue && canPushStatueOn(m->base) && !nonAdjacent(c2, m->base)) {
        visibleFor(300);
        c2->wall = m->base->wall;
        if(cellUnstable(cwt.at))
          m->base->wall = waChasm; 
        else {
          m->base->wall = waBigStatue;
          animateMovement(c2, m->base, LAYER_BOAT, NOHINT);
          }
        }
      else if(m->inBoat && !isWateryOrBoat(c2) && passable(c2, m->base, P_ISPLAYER | P_MIRROR | reflectflag)) {
        if(boatGoesThrough(c2) && markOrb(itOrbWater)) {
          c2->wall = isIcyLand(m->base) ? waLake : waSea;
          }
        else {
          if(isWatery(m->base)) 
            m->base->wall = waBoat, m->base->mondir = dirfromto(m->base, c2);
          else if(boatStrandable(m->base))
            m->base->wall = waStrandedBoat;
          else if(boatStrandable(c2))
            m->base->wall = waStrandedBoat;
          m->inBoat = false;
          }
        }
      else if(isPushable(c2->wall) && !nonAdjacent(c2, m->base)) {
        int sd = dirfromto(c2, m->base);
        int subdir = 1;
        double bestd = 9999;
        pushmonsters();
        for(int di=-1; di<2; di+=2) {
          cell *c = getMovR(c2, sd+di);
          if(!c) continue;
          if(m->isVirtual || !gmatrix.count(c)) continue;
          double d = intval(gmatrix[c] * C0, m->pat * C0);
          // printf("di=%d d=%lf\n", di, d);
          if(d<bestd) bestd=d, subdir = di;
          }
        visibleFor(300);
        cellwalker push(c2, dirfromto(c2, m->base));
        push = push + 3 * (-subdir) + wstep;
        if(!canPushThumperOn(push.at, c2, m->base) && c2->type == 7) {
          push = push + wstep - subdir + wstep;
          }
        if(!canPushThumperOn(push.at, c2, m->base)) {
          go = false;
          }
        else pushThumper(c2, push.at);
        popmonsters();
        }
      else if(c2->wall == waRose && !nonAdjacent(m->base, c2)) {
        m->dead = true;
        go = false;
        }
      else if(
        (blown ? !passable(c2, m->base, P_ISPLAYER | P_BLOW) : !passable(c2, m->base, P_ISPLAYER | P_MIRROR | reflectflag)) && 
        !(isWatery(c2) && m->inBoat && !nonAdjacent(m->base,c2)))
        go = false;
      
      }
    }
  
  if(!go || abs(playergo[cpid]) < 1e-3 || abs(playerturn[cpid]) > 1e-3) bulltime[cpid] = curtime;
  
  if(go) {
    if(c2 != m->base) {
      if(cellUnstable(m->base) && !markOrb(itOrbAether))
        doesFallSound(m->base);
    
      if(items[itOrbFire]) {
        visibleFor(800);
        if(makeflame(m->base, 10, false)) markOrb(itOrbFire);
        }

      if(isIcyLand(m->base) && m->base->wall == waNone && markOrb(itOrbWinter)) {
        invismove = false;
        m->base->wall = waIcewall;
        }
      
      if(items[itOrbDigging]) {
        visibleFor(400);
        int d = dirfromto(m->base, c2);
        if(d >= 0 && earthMove(m->base, d)) markOrb(itOrbDigging);
        }
      
      cwt.at = c2; afterplayermoved();
      if(c2->item && c2->land == laAlchemist) c2->wall = m->base->wall;
      if(m->base->wall == waRoundTable)
        roundTableMessage(c2);
      if(c2->wall == waCloud || c2->wall == waMirror) {
        visibleFor(500);
        cellwalker cw(c2, 0, false);
        mirror::createHere(cw, cpid);
        mirror::breakMirror(cw, cpid);
        awakenMimics(m, c2);
        #if CAP_RACING
        if(racing::on) racing::race_won();
        #endif
        }
      if(c2->wall == waGlass && items[itOrbAether]) {
        items[itOrbAether] = 0;
        addMessage(XLAT("Your Aether powers are drained by %the1!", c2->wall));
        }
      movecost(m->base, c2, 1);

      if(c2->wall == waMineMine && !markOrb(itOrbAether) && !markOrb(itOrbWinter)) {
        items[itOrbLife] = 0;
        m->dead = true;
        }
      uncoverMinesFull(c2);
      
      if(isWatery(c2) && isWatery(m->base) && m->inBoat)
        moveItem(m->base, c2, true);
  
      destroyWeakBranch(m->base, c2, moPlayer);

      if(c2->wall == waClosePlate || c2->wall == waOpenPlate)
        toggleGates(c2, c2->wall);

      if(c2->wall == waArrowTrap && c2->wparam == 0 && !markOrb(itOrbAether))
        activateArrowTrap(c2);
  
      if(c2->wall == waFireTrap && c2->wparam == 0 && !markOrb(itOrbAether)) {
        c2->wparam = 2;
        firetraplist.emplace(ticks + 800, c2);
        }
  
      if(c2->item == itOrbYendor && !peace::on) yendor::check(c2);
      collectItem(c2);
      movecost(m->base, c2, 2);
      }
    }

  if(go) m->rebasePat(nat);
  else m->rebasePat(nat0);

  if(m->base->wall == waBoat && !m->inBoat) {
    m->inBoat = true; m->base->wall = waSea;
    }
  
  if(m->base->wall == waStrandedBoat && !m->inBoat && markOrb(itOrbWater)) {
    m->inBoat = true; m->base->wall = waSea;
    }

  if(m->inBoat && boatStrandable(c2)) {
    c2->wall = waStrandedBoat;
    m->inBoat = false;
    }

  if(!markOrb(itOrbAether)) {
    if(m->base->wall == waChasm || m->base->wall == waClosedGate)
      m->dead = true;

    if(isWatery(m->base) && !m->inBoat && !markOrb(itOrbFish))
      m->dead = true;

    if(isFireOrMagma(m->base) && !markOrb(itOrbWinter))
      m->dead = true;
    }

  landvisited[m->base->land] = true;
  
  playerfire[cpid] = false;
  
  if(items[itOrbHorns] && !m->isVirtual) {
    hyperpoint H = hornpos(cpid);

    for(monster *m2: nonvirtual) {
      if(m2 == m) continue;
      
      double d = intval(m2->pat*C0, H);
    
      if(d < SCALE2 * 0.1) {
        if(hornKills(m2->type))
          killMonster(m2, moPlayer);
        else if(hornStuns(m2->type))
          m2->stunoff = max(m2->stunoff, curtime + 150);
        }
      }
    }
    
  for(int b=0; b<2; b++) if(sword::orbcount(b) && !m->isVirtual) {
  
    for(double d=0; d<=1.001; d += .1) {
      hyperpoint H = swordpos(cpid, b, d);
  
      for(monster *m2: nonvirtual) {
        if(m2 == m) continue;
        
        double d = intval(m2->pat*C0, H);
      
        if(d < SCALE2 * 0.1) {
          if(swordKills(m2->type) && !(isBullet(m2) && m2->pid == cpid))
              killMonster(m2, moPlayer);
        }
      }
  
      cell *c3 = findbaseAroundRepeat(H, m->base);
      if(c3->wall == waSmallTree || c3->wall == waBigTree || c3->wall == waBarrowDig || c3->wall == waCavewall ||
        (c3->wall == waBarrowWall && items[itBarrow] >= 25))
        c3->wall = waNone;
      
      else if(isWall(c3)) break;
      }
    }

  if(go) {
    // printf("#%3d: at %s\n", steps, display(nat * C0));
    steps--;
    if(steps > 0) {
      nat0 = nat;
      go = false;
      goto nextstep;
      }
    }

  #if CAP_RACING  
  if(!go && racing::on) m->vel = 0;
  #endif
  
  if(shotkey && canmove && curtime >= m->nextshot) {

    visibleFor(500);
    if(items[itOrbFlash]) {
      pushmonsters();
      killMonster(m->base, moNone);
      cwt.at = m->base;
      activateFlash();
      popmonsters();
      return;
      }
    
    if(items[itOrbLightning]) {
      pushmonsters();
      killMonster(m->base, moLightningBolt);
      cwt.at = m->base;
      activateLightning();
      popmonsters();
      return;
      }
    
    playerfire[cpid] = true;
    m->nextshot = curtime + (250 + 250 * players);
    
    turncount++;    
    shootBullet(m);
    }
  
  if(dropgreen && m->base->item == itNone)
    dropGreenStone(m->base);
  }

monster *getPlayer() {
  return pc[cpid];
  }

void virtualize(monster *m) {
  if(doall) forCellCM(c2, m->base) if(!gmatrix.count(c2)) {
    m->isVirtual = true;
    m->pat = m->at;
    return;
    }
  }

bool reflectmatrix(transmatrix& M, cell *c1, cell *c2, bool onlypos) {
  if(!gmatrix.count(c1) || !gmatrix.count(c2)) return false;
  transmatrix H = inverse(gmatrix[c1]) * gmatrix[c2];
  transmatrix S = spintox(tC0(H));
  ld d = hdist0(tC0(H));
  transmatrix T = xpush(-d/2) * S * inverse(gmatrix[c1]) * M;
  if(onlypos && tC0(T)[0] < 0) return false;
  M = gmatrix[c1] * inverse(S) * xpush(d/2) * MirrorX * T;
  return true;
  }

int reflect(cell*& c2, cell*& mbase, transmatrix& nat) {
  int reflections = 0;
  if(c2 != mbase && c2->wall == waMirrorWall && inmirror(c2)) {
    if(reflectmatrix(nat, mbase, c2, false)) {
      c2 = mbase;
      reflections++;
      }
    }

  if(c2 == mbase && inmirror(c2)) {
    forCellEx(c3, c2) if(c3->land == laMirrorWall) {
      cell *c1 = mbase;
      mbase = c3;
      reflect(c3, mbase, nat);
      mbase = c1;
      c2 = c3;
      reflections++;
      }
    }
  
  if(c2 == mbase && c2->wall == waMirrorWall && c2->land == laMirrorWall) {
    int d = mirror::mirrordir(c2);
    if(d != -1) {
      for(int k=0; k<7; k++) {
        cell *ca = createMovR(c2, d-k);
        cell *cb = createMovR(c2, d+k);
        if(ca->land == laMirror && inmirror(cb)) {
          reflectmatrix(nat, ca, cb, true);
          reflections++;
          break;
          }
        }
      }
    else {
      for(int k=0; k<6; k++) {
        cell *cb = createMovR(c2, k+1);
        cell *cc = createMovR(c2, k+2);
        if(cb->land != laMirrorWall || cc->land != laMirrorWall) continue;
        cell *ca = createMovR(c2, k);
        cell *cd = createMovR(c2, k+3);
        if(reflectmatrix(nat, cc, ca, true)) reflections++;
        for(int limit=0; limit<10 && reflectmatrix(nat, cb, cd, true) && (reflections++, reflectmatrix(nat, cc, ca, true)); limit++) reflections+=2;
        }
      } 
    }
  return reflections;
  }

void moveMimic(monster *m) {
  virtualize(m);
  transmatrix nat = m->pat;
  cpid = m->pid;
  m->footphase = getPlayer()->footphase;
  
  // no need to care about Mirror images, as they already have their 'at' matrix reversed :|
  nat = nat * spin(playerturn[cpid]) * xpush(playergo[cpid]);

  cell *c2 = m->findbase(nat);
  reflect(c2, m->base, nat);
  if(c2 != m->base && !passable(c2, m->base, P_ISPLAYER | P_MIRROR | P_MIRRORWALL))
    killMonster(m, moNone);
  else {
    m->rebasePat(nat);
    if(playerfire[cpid]) shootBullet(m);
    }  

  if(c2->wall == waCloud || c2->wall == waMirror) {
    cellwalker cw(c2, 0, false);
    mirror::createHere(cw, cpid);
    mirror::breakMirror(cw, -1);
    awakenMimics(m, c2);
    }

  if(!doall && c2->cpdist >= 6)
    m->dead = true;
  }

bool isPlayerOrImage(eMonster m) {
  return isMimic(m) || m == moPlayer;
  }

monster *parentOrSelf(monster *m) {
  return m->parent ? m->parent : m;
  }

bool verifyTeleport() {
  if(!on) return true;
  if(playerCrash(pc[cpid], mouseh)) return false;
  return true;
  }

void destroyMimics() {
  for(monster *m: active)
    if(isMimic(m->type)) 
      m->dead = true;
  }

void teleported() {
  monster *m = pc[cpid];
  m->base = cwt.at;
  m->at = rgpushxto0(inverse(gmatrix[cwt.at]) * mouseh) * spin(rand() % 1000 * M_PI / 2000);
  m->findpat();
  destroyMimics();
  }

void shoot(eItem it, monster *m) {
  monster* bullet = new monster;
  bullet->base = m->base;
  bullet->at = m->at * rspintox(inverse(m->pat) * mouseh);
  bullet->type = it == itOrbDragon ? moFireball : it == itOrbAir ? moAirball : moBullet;
  bullet->parent = m;
  bullet->pid = m->pid;
  bullet->parenttype = m->type;
  items[it]--;
  additional.push_back(bullet);
  }

eItem targetRangedOrbKey(orbAction a) {
  hyperpoint h = mouseh;
  cell *b = mouseover;
  monster *mt = mousetarget;

  mouseh = keytarget(cpid);
  
  mouseover = pc[cpid]->base;
  
  while(true) {
    cell *c2 = findbaseAround(mouseh, mouseover);
    if(c2 == mouseover) break;
    mouseover = c2;
    }
  mousetarget = NULL;

  for(monster *m2: nonvirtual) {
    if(m2->dead) continue;
    if(m2->no_targetting) continue;
    if(!mousetarget || intval(mouseh, mousetarget->pat*C0) > intval(mouseh, m2->pat*C0)) 
      mousetarget = m2;
    }

  eItem r = targetRangedOrb(mouseover, a);
  // printf("A%d i %d h %p t %p ov %s => %s\n", a, cpid, mouseover, mousetarget, display(mouseh), dnameof(r));
  
  mouseh = h;
  mousetarget = mt;
  mouseover = b;
  return r;
  }

eItem targetRangedOrb(orbAction a) {
  if(!on) return itNone;
  monster *wpc = pc[cpid];
  if(a != roCheck && !wpc) return itNone;  
  
  if(items[itOrbPsi] && shmup::mousetarget && intval(mouseh, shmup::mousetarget->pat*C0) < SCALE2 * .1) {
    if(a == roCheck) return itOrbPsi;
    addMessage(XLAT("You kill %the1 with a mental blast!", mousetarget->type));
    killMonster(mousetarget, moNone);
    items[itOrbPsi] -= 30;
    if(items[itOrbPsi]<0) items[itOrbPsi] = 0;
    return itOrbPsi;
    }
  
  if(items[itOrbStunning] && shmup::mousetarget && intval(mouseh, shmup::mousetarget->pat*C0) < SCALE2 * .1) {
    if(a == roCheck) return itOrbStunning;
    mousetarget->stunoff = curtime + 1000;
    items[itOrbStunning] -= 10;
    if(items[itOrbStunning]<0) items[itOrbStunning] = 0;
    return itOrbStunning;
    }
  
  if(on && items[itOrbDragon]) {
    if(a == roCheck) return itOrbDragon;
    shoot(itOrbDragon, wpc);
    return itOrbDragon;
    }
  
  if(on && items[itOrbAir]) {
    if(a == roCheck) return itOrbAir;
    shoot(itOrbAir, wpc);
    return itOrbAir;
    }
  
  if(on && items[itOrbIllusion]) {
    if(a == roCheck) return itOrbIllusion;
    shoot(itOrbIllusion, wpc);
    return itOrbIllusion;
    }

  return itNone;
  }

int speedfactor() {
  return items[itOrbSpeed]?2:1;
  }

transmatrix frontpush(ld x) {
  if(DIM == 2) return xpush(x);
  else return cpush(2, x);
  }
  
void moveBullet(monster *m, int delta) {
  cpid = m->pid;
  m->findpat();
  virtualize(m);
  
  transmatrix nat0 = m->pat;
  transmatrix nat = m->pat;
  

  if(isReptile(m->base->wall)) m->base->wparam = reptilemax();
    
  if(m->type == moFlailBullet) {
    m->vel -= delta  / speedfactor() / 600000.0;
    if(m->vel < 0 && m->parent) {
      // return to the flailer!
      nat = nat * rspintox(inverse(m->pat) * m->parent->pat * C0) * spin(M_PI);
      }
    }
  else if(m->type == moBullet)
    m->vel = 1/300.;
  else if(m->type == moFireball)
    m->vel = 1/500.;
  else if(m->type == moCrushball)
    m->vel = 1/1000.;
  else if(m->type == moAirball)
    m->vel = 1/200.;
  else if(m->type == moArrowTrap)
    m->vel = 1/200.;
  else if(m->type == moTongue) {
    m->vel = 1/1500.;
    if(m->isVirtual || !m->parent || intval(nat*C0, m->parent->pat*C0) > SCALE2 * 0.4)
      m->dead = true;
    }
  nat = nat * frontpush(delta * SCALE * m->vel / speedfactor());
  cell *c2 = m->findbase(nat);

  if(m->parent && isPlayer(m->parent) && markOrb(itOrbLava) && c2 != m->base && !isPlayerOn(m->base)) 
    makeflame(m->base, 5, false);

  if(isActivable(c2)) activateActiv(c2, true);
  
  // knives break mirrors and clouds
  if(c2->wall == waCloud || c2->wall == waMirror) {
    cellwalker cw(c2, 0, false);
    mirror::createHere(cw, cpid);
    mirror::breakMirror(cw, -1);
    awakenMimics(m, c2);
    }
  
  reflect(c2, m->base, nat);
  
  bool godragon = m->type == moFireball && isDragon(c2->monst);
  
  if(m->type != moTongue && !(godragon || (c2==m->base && m->type == moArrowTrap) || passable(c2, m->base, P_BULLET | P_MIRRORWALL))) {
    m->dead = true;
    if(m->type != moAirball) killMonster(c2, m->parent ? m->parent->type : moNone);
    // cell *c = m->base;
    if(m->parent && isPlayer(m->parent)) {
      if(c2->wall == waBigTree) {
        addMessage(XLAT("You start chopping down the tree."));
        c2->wall = waSmallTree;
        }
      else if(c2->wall == waSmallTree) {
        addMessage(XLAT("You chop down the tree."));
        c2->wall = waNone;
        }
      else if(isActivable(c2)) 
        activateActiv(c2, true);
      else if(c2->wall == waExplosiveBarrel)
        explodeBarrel(c2);
      }
    if(m->type == moCrushball && c2->wall == waRuinWall)
      c2->wall = waNone;
    if(m->type == moFireball) {
      makeflame(c2, 20, false) || makeflame(m->base, 20, false);
      }
    }
  m->rebasePat(nat);
  
  // destroy stray bullets
  if(!doall) for(int i=0; i<m->base->type; i++) 
    if(!m->base->move(i) || !gmatrix.count(m->base->move(i)))
      m->dead = true;

  // items[itOrbWinter] = 100; items[itOrbLife] = 100;
  
  if(!m->isVirtual) for(monster* m2: nonvirtual) {
    if(m2 == m || (m2 == m->parent && m->vel >= 0) || m2->parent == m->parent) 
      continue;

    eMonster ptype = parentOrSelf(m)->type;
    bool slayer = m->type == moCrushball ||
      (markOrb(itOrbSlaying) && (markOrb(itOrbEmpathy) ? isPlayerOrImage(ptype) : ptype == moPlayer));
    
    // Flailers only killable by themselves
    if(m2->type == moFlailer && m2 != m->parent) continue;
    // be nice to your images! would be too hard otherwise...
    if(isPlayerOrImage(parentOrSelf(m)->type) && isPlayerOrImage(parentOrSelf(m2)->type) &&
      m2->pid == m->pid)
      continue;
    // fireballs/airballs don't collide
    if(m->type == moFireball && m2->type == moFireball) continue;
    if(m->type == moAirball && m2->type == moAirball) continue;
    double d = intval(m2->pat*C0, m->pat*C0);
    
    if(d < SCALE2 * 0.1) {

      if(m2->type == passive_switch) { m->dead = true; continue; }
      
      if(m->type == moAirball && isBlowableMonster(m2->type)) {

        if(m2->blowoff < curtime) {
          hyperpoint h = inverse(m2->pat) * nat0 * C0;
          m2->swordangle += atan2(h[1], h[0]);
          m2->rebasePat(m2->pat * rspintox(h));
          }
        m2->blowoff = curtime + 1000;
        continue;
        }
      // Hedgehog Warriors only killable outside of the 45 degree angle
      if(m2->type == moHedge && !peace::on && !slayer) {
        hyperpoint h = inverse(m2->pat) * m->pat * C0;
        if(h[0] > fabsl(h[1])) { m->dead = true; continue; }
        }
      if(peace::on && !isIvy(m2->type)) {
        m->dead = true;
        m2->stunoff = curtime + 600;
        continue;
        }
      // multi-HP monsters
      if((m2->type == moPalace || m2->type == moFatGuard || m2->type == moSkeleton ||
        m2->type == moVizier || isMetalBeast(m2->type) || m2->type == moTortoise || m2->type == moBrownBug || 
        m2->type == moReptile || m2->type == moSalamander || m2->type == moTerraWarrior) && m2->hitpoints > 1 && !slayer) {
        m2->rebasePat(m2->pat * rspintox(inverse(m2->pat) * nat0 * C0));
        if(m2->type != moSkeleton && !isMetalBeast(m2->type) && m2->type != moReptile && m2->type != moSalamander && m2->type != moBrownBug) 
          m2->hitpoints--;
        m->dead = true;
        if(m2->type == moVizier) ;
        else if(m2->type == moFatGuard)
          m2->stunoff = curtime + 600;
        else if(m2->type == moTerraWarrior)
          m2->stunoff = curtime + 300 * (6 - m2->hitpoints);
        else if(m2->type == moMetalBeast || m2->type == moMetalBeast2)
          m2->stunoff = curtime + 3000;
        else if(m2->type == moReptile)
          m2->stunoff = curtime + 3000;
        else if(m2->type == moTortoise)
          m2->stunoff = curtime + 3000;
        else if(m2->type == moSkeleton && m2->base->land != laPalace)
          m2->stunoff = curtime + 2100;
        else
          m2->stunoff = curtime + 900;
        continue;
        }
      // conventional missiles cannot hurt some monsters
      bool conv = (m->type == moBullet || m->type == moFlailBullet || m->type == moTongue || m->type == moArrowTrap) && !slayer;

      // Raiders are unaffected
      if((m2->type == moCrusher || m2->type == moPair || m2->type == moMonk ||
        m2->type == moAltDemon || m2->type == moHexDemon) && conv) {
        m->dead = true;
        continue;
        }
      if(m2->type == moGreater && conv) {
        m->dead = true;
        continue;
        }
      if(m2->type == moRoseBeauty && conv && !markOrb(itOrbBeauty)) {
        m->dead = true;
        continue;
        }
      if(m2->type == moDraugr && conv) {
        m->dead = true;
        continue;
        }
      if(isBull(m2->type) && conv) {
        m->dead = true;
        // enrage herd bulls, awaken sleeping bulls
        m2->type = moRagingBull;
        continue;
        }
      // Knights reflect bullets
      if(m2->type == moKnight) {
        if(m->parent && m->parent != &arrowtrap_fakeparent) {
          nat = nat * rspintox(inverse(m->pat) * m->parent->pat * C0);
          m->rebasePat(nat);
          }
        m->parent = m2;
        continue;
        }
      m->dead = true;
      if(m->type == moFireball) makeflame(m->base, 20, false);
      // Orb of Winter protects from fireballs
      if(m->type == moFireball && ((isPlayer(m2) && markOrb(itOrbWinter)) || m2->type == moWitchWinter)) 
        continue;
      bool revive = m2->type == moMirrorSpirit && !m2->dead;
      killMonster(m2, m->parent ? m->parent->type : moNone);
      if(revive && m2->dead) {
        hr::kills[moMirrorSpirit]--;
        multi::kills[cpid]--;
        mirrorspirits++;
        }
      }
    }
  }

hyperpoint closerTo;

bool closer(monster *m1, monster *m2) {
  return intval(m1->pat*C0,  closerTo) < intval(m2->pat*C0, closerTo);
  }

bool dragonbreath(cell *dragon) {
  int randplayer = hrand(numplayers());
  monster* bullet = new monster;
  bullet->base = dragon;
  bullet->at = rspintox(inverse(gmatrix[dragon]) * pc[randplayer]->pat * C0);
  bullet->type = moFireball;
  bullet->parent = bullet;
  bullet->pid = randplayer;
  additional.push_back(bullet);
  return true;
  }

#define CHARGING (-777)
#define BULLSTUN (1500)

void moveMonster(monster *m, int delta) {

  bool stunned = m->stunoff > curtime || m->blowoff > curtime;
  
  if(stunned && cellUnstable(m->base))
    doesFallSound(m->base);

  if(isReptile(m->base->wall)) m->base->wparam = reptilemax();
  
  if(m->base->wall == waChasm && !survivesChasm(m->type) && m->type != moReptile)
    killMonster(m, moNone, AF_FALL);

  if(m->base->wall == waRose && !survivesThorns(m->type))
    killMonster(m, moNone);

  if(isWatery(m->base) && !survivesWater(m->type) && !m->inBoat && m->type != moReptile)
    killMonster(m, moNone);

  if(isFireOrMagma(m->base)) {
    if(m->type == moSalamander)
      m->stunoff = max(ticks+500, m->stunoff);
    else if(!survivesFire(m->type))
      killMonster(m, moNone);
    }
  
  if(m->base->wall == waClosedGate && !survivesWall(m->type))
    killMonster(m, moNone);

  if(m->dead) return;
    
  cell *c = m->base;
  transmatrix goal = gmatrix[c];
  
  bool direct = false; // is there a direct path to the target?
  int directi = 0; // which player has direct path (to set as pid in missiles)
  
  double step = SCALE * delta/1000.0;
  if(m->type == moWitchSpeed)
    step *= 2;
  else if(m->type == moEagle)
    step *= 1.6;
  else if(m->type == moHunterDog)
    step *= (1 + .5 / numplayers());
  else if(m->type == moLancer)
    step *= 1.25;
  else if(isDemon(m->type)) {
    if(m->type == moLesserM) m->type = moLesser;
    if(m->type == moGreaterM) m->type = moGreater;
    step /= 2;
    }
  else if(m->type == moMetalBeast || m->type == moMetalBeast2) 
    step /= 2;
  else if(m->type == moTortoise && peace::on)
    step = 0;
  else if(m->type == moTortoise)
    step /= 3;
  else if(isBull(m->type))
    step *= 1.5;
  else if(m->type == moAltDemon || m->type == moHexDemon || m->type == moCrusher || m->type == moMonk)
    step *= 1.4;

  if(m->type == passive_switch) step = 0;
  
  if(items[itOrbBeauty] && !m->isVirtual) {
    bool nearplayer = false;
    for(int pid=0; pid<players; pid++) if(!pc[pid]->isVirtual) {
      double dist = intval(pc[pid]->pat*C0, m->pat*C0);
      if(dist < SCALE2) nearplayer = true;
      }
    if(nearplayer) markOrb(itOrbBeauty), step /= 2;
    }

  if(m->isVirtual) return;
  transmatrix nat = m->pat;

  if(stunned) {
    if(m->blowoff > curtime) {
      step = SCALE * -delta / 1000.;
      }
    else if(m->type == moFatGuard || m->type == moTortoise || m->type == moRagingBull || m->type == moTerraWarrior)
      step = 0;
    else if(m->type == moReptile)
      step = SCALE * -delta / 1000. * (m->stunoff - curtime) / 3000.;
    else if(m->type == moSalamander) {
      if(isFireOrMagma(m->base)) step = 0;
      else
        step = SCALE * -delta / 2000.;
      }
    else step = SCALE * -delta/2000.;
    }
  
  else if(m->type == moRagingBull && m->stunoff == CHARGING) ;

  else {
  
    if(m->type == moSleepBull && !m->isVirtual) {
      for(monster *m2: nonvirtual) if(m2!=m && m2->type != moBullet && m2->type != moArrowTrap) {
        double d = intval(m2->pat*C0, nat*C0);
        if(d < SCALE2*3 && m2->type == moPlayer) m->type = moRagingBull;
        }
      }
    
    if(m->type == moWitchFlash) for(int pid=0; pid<players; pid++) {
      if(pc[pid]->isVirtual) continue;
      if(m->isVirtual) continue;
      bool okay = intval(pc[pid]->pat*C0, m->pat*C0) < 2 * SCALE2;
      for(monster *m2: nonvirtual) {
        if(m2 != m && isWitch(m2->type) && intval(m2->pat*C0, m->pat*C0) < 2 * SCALE2)
          okay = false;
        }
      if(okay) {
        addMessage(XLAT("%The1 activates her Flash spell!", m->type));
        pushmonsters();
        activateFlashFrom(m->base, moWitchFlash, AF_MAGIC | AF_GETPLAYER | AF_MSG);
        popmonsters();
        m->type = moWitch;
        pc[pid]->dead = true;
        }
      }
    if(isBug(m->type)) {
      vector<monster*> bugtargets;
      for(monster *m2: nonvirtual) 
        if(!isBullet(m2))
        if(m2->type != m->type)
        if(!isPlayer(m2) || !invismove)
        if(!m2->dead)
          bugtargets.push_back(m2);
      
      closerTo = m->pat * C0;
      sort(bugtargets.begin(), bugtargets.end(), closer);
  
      if(step) for(monster *m2: bugtargets)
        if(trackroute(m, m2->pat, step)) {
          goal = m2->pat;
          direct = true;
          break;
          }
      }
    else if(m->type == moWolf && !peace::on) {
      cell *cnext = c;
      for(int i=0; i<c->type; i++) {
        cell *c2 = c->move(i);
        if(c2 && gmatrix.count(c2) && (c2->land == laVolcano || (isIcyLand(c2) && HEAT(c2) > HEAT(c))) && passable(c2, c, 0))
          cnext = c2;
        }
      goal = gmatrix[cnext];
      direct = true;
      directi = 0;
      }
    #if CAP_FIELD
    else if(m->type == moHerdBull) {
      cell *cnext = prairie::next(c);
      if(cnext && gmatrix.count(cnext)) {
        goal = gmatrix[cnext];
        direct = true;
        directi = 0;
        }
      else m->dead = true;
      }
    #endif
    else if(m->type == moButterfly) {
      int d = neighborId(m->base, m->torigin);
      cell *cnext = NULL;
      for(int u=2; u<m->base->type; u++) {
        cell *c2 = createMov(m->base, (d+u) % m->base->type);
        if(passable_for(m->type, c2, m->base, P_ONPLAYER)) {
          cnext = c2;
          break;
          }
        }

      if(cnext && gmatrix.count(cnext)) {
        goal = gmatrix[cnext];
        direct = true;
        directi = 0;
        }
      }
    else if(!direct && !invismove && !peace::on) {
      for(int i=0; i<players; i++) 
        if(step && trackroute(m, pc[i]->pat, step) && (!direct || intval(pc[i]->pat*C0, m->pat*C0) < intval(goal*C0,m->pat*C0))) {
          goal = pc[i]->pat;
          direct = true;
          directi = i;
          // m->trackrouteView(pc->pat, step);
          }
        }
  
    if(!direct && !peace::on) while(true) {
      if(step && trackroute(m, gmatrix[c], step))
        goal = gmatrix[c];
      cell *cnext = c;
      for(int i=0; i<c->type; i++) {
        cell *c2 = c->move(i);
        if(c2 && gmatrix.count(c2) && c2->pathdist < c->pathdist &&
          passable_for(m->type, c2, c, P_CHAIN | P_ONPLAYER))
          cnext = c2;
        }
      if(cnext == c) break;
      c = cnext;
      }

    if(m->type == moHedge) {
      hyperpoint h = inverse(m->pat) * goal * C0;
      if(h[1] < 0)
        nat = nat * spin(M_PI * delta / 3000 / speedfactor());
      else
        nat = nat * spin(M_PI * -delta / 3000 / speedfactor());
      m->rebasePat(nat);
      // at most 45 degrees
      if(h[0] < fabsl(h[1])) return;
      }
    else if(!peace::on) {
      nat = nat * rspintox(inverse(m->pat) * goal * C0);
      }
    }
  
  if(m->type == moVampire && !m->isVirtual) for(int i=0; i<players; i++) 
  if(!pc[i]->isVirtual && intval(m->pat*C0, pc[i]->pat*C0) < SCALE2 * 2) {
    for(int i=0; i<ittypes; i++)
      if(itemclass(eItem(i)) == IC_ORB && items[i] && items[itOrbTime] && !orbused[i])
        orbused[i] = true;
    step = 0;
    }

  bool carried = false;

  if(c->land == laWhirlpool && (m->type == moShark || m->type == moCShark || m->type == moPirate))
    oceanCurrents(nat, m, delta), carried = true;

  if(m->type != moGhost && m->type != moFriendlyGhost && m->type != moAirElemental)
    carried |= airCurrents(nat, m, delta);
  
  if(rosedist(m->base) == 1)
    roseCurrents(nat, m, delta), carried = true;

  step /= speedfactor();

  if(DIM == 3) step /= 3;
  
  int igo = 0;
  
  transmatrix nat0 = nat;
  
  igo_retry:
  
  if(igo == IGO && peace::on) 
    nat0 = nat0 * spin(rand() % 16); 
  
  else if(igo >= IGO) {
    if(m->type == moHerdBull) m->type = moRagingBull;
    return;
    }
  
  if(igo == 1 && m->type == moRagingBull && m->stunoff == CHARGING) {
    m->stunoff = curtime + BULLSTUN;
    return;
    }
  
  nat = nat0 * spin(igospan[igo]) * xpush(step) * spin(-igospan[igo]);; // * spintox(wherePC);

  if(m->type != moRagingBull && !peace::on)
  if(intval(nat*C0, goal*C0) >= intval(m->pat*C0, goal*C0) && !stunned && !carried) {
    igo++; goto igo_retry; }

  for(int i=0; i<multi::players; i++) for(int b=0; b<2; b++) if(sword::orbcount(b)) {  
    if(pc[i]->isVirtual) continue;
    hyperpoint H = swordpos(i, b, 1);
    double d = intval(H, nat*C0);
    if(d < SCALE2 * 0.12) { igo++; goto igo_retry; }
    }

  m->footphase += step;

  monster* crashintomon = NULL;
  
  if(!m->isVirtual) for(monster *m2: nonvirtual) if(m2!=m && m2->type != moBullet && m2->type != moArrowTrap) {
    double d = intval(m2->pat*C0, nat*C0);
    if(d < SCALE2 * 0.1) crashintomon = m2;
    }
  
  if(!peace::on) 
  for(int i=0; i<players; i++) 
    if(crashintomon == pc[i]) 
      pc[i]->dead = true;

  if(peace::on) ; 

  else if(crashintomon && isMimic(crashintomon->type)) {
    killMonster(crashintomon, m->type);
    crashintomon = NULL;
    }
  
  else if(crashintomon && (
    items[itOrbDiscord] || isBull(m->type) ||
       ((isBug(m->type) || isBug(crashintomon->type)) && m->type != crashintomon->type)) 
    && !isBullet(crashintomon)) {
    if(noncrashable(crashintomon, m)) {
      if(isBull(crashintomon->type)) crashintomon->type = moRagingBull;
      }
    else {
      killMonster(crashintomon, m->type, isBull(m->type) ? AF_BULL : 0);
      crashintomon = NULL;
      }
    }
  
  if(crashintomon) { igo++; goto igo_retry; }

  cell *c2 = m->findbase(nat);
  if(reflectflag & P_MIRRORWALL) reflect(c2, m->base, nat);

  if(m->type == moButterfly && !passable_for(m->type, c2, m->base, P_CHAIN | reflectflag)) {
    igo++; goto igo_retry;
    }

  if(isPlayerOn(c2) && !peace::on) {
    bool usetongue = false;
    if(isSlimeMover(m->type) || m->type == moWaterElemental) usetongue = true;
    if(isWatery(c2) && !survivesWater(m->type) && !m->inBoat) usetongue = true;
    if(c2->wall == waChasm && !survivesChasm(m->type)) usetongue = true;
    if(isFireOrMagma(c2) && !survivesFire(m->type) && !m->inBoat) usetongue = true;
    if(isBird(m->type) && !passable_for(moEagle, c2, c, 0)) usetongue = true;
    if((m->type == moMonk || m->type == moAltDemon || m->type == moHexDemon) && !passable_for(m->type, c2, c, 0))
      usetongue = true;
    if(usetongue) {
      if(curtime < m->nextshot) return;
      // m->nextshot = curtime + 25;
      monster* bullet = new monster;
      bullet->base = m->base;
      bullet->at = m->at;
      bullet->type = moTongue;
      bullet->parent = m;
      bullet->parenttype = m->type;
      bullet->pid = whichPlayerOn(c2);
      additional.push_back(bullet);
      return;
      }
    }

  if(!ignoresPlates(m->type)) destroyWeakBranch(m->base, c2, m->type);

  if(c2 != m->base && (c2->wall == waClosePlate || c2->wall == waOpenPlate) && !ignoresPlates(m->type))
    toggleGates(c2, c2->wall, 3);

  if(c2 != m->base && c2->wall == waArrowTrap && c2->wparam == 0 && !ignoresPlates(m->type))
    activateArrowTrap(c2);

  if(c2 != m->base && c2->wall == waFireTrap && c2->wparam == 0 && !ignoresPlates(m->type)) {
    c2->wparam = 2;
    firetraplist.emplace(ticks + 800, c2);
    }

  if(c2 != m->base && mayExplodeMine(c2, m->type)) 
    killMonster(m, moNone);
  
  if(c2 != m->base && c2->wall == waRose && !nonAdjacent(m->base, c2) && !survivesThorns(m->type))
    killMonster(m, moNone);
  
  if(c2 != m->base && cellUnstable(m->base) && !ignoresPlates(m->type))
    doesFallSound(m->base);

  if(m->type == moWolf && c2->land == laVolcano) m->type = moLavaWolf;
  if(m->type == moLavaWolf && isIcyLand(c2)) m->type = moWolf;
  
  if(c2 != m->base && m->type == moWitchFire) makeflame(m->base, 10, false);
  if(c2 != m->base && m->type == moFireElemental) makeflame(m->base, 20, false);
  if(c2 != m->base && m->type == moWaterElemental) placeWater(c2, m->base);
  if(c2 != m->base && m->type == moEarthElemental) {
    int d = dirfromto(m->base, c2);
    if(d >= 0) earthMove(m->base, d);
    }
  
  if(m->type == moReptile && c2 != m->base) {
    if(c2->wall == waChasm) {
      c2->wall = waReptile;
      c2->wparam = reptilemax();
      playSound(c, "click");
      m->dead = true;
      }
    else if(isChasmy(c2) || isWatery(c2)) {
      c2->wall = waReptileBridge;
      c2->item = itNone;
      c2->wparam = reptilemax();
      playSound(c, "click");
      m->dead = true;
      }
    }
  
  if(c2 != m->base && m->type == moNecromancer && !c2->monst) {
    for(int i=0; i<m->base->type; i++) {
      cell *c3 = m->base->move(i);
      if(dirfromto(c3, c2) != -1 && c3->wall == waFreshGrave && gmatrix.count(c3)) {
        bool monstersNear = false;
        for(monster *m2: nonvirtual) {
          if(m2 != m && intval(m2->pat*C0, gmatrix[c3]*C0) < SCALE2 * .3)
            monstersNear = true;
          if(m2 != m && intval(m2->pat*C0, gmatrix[c2]*C0) < SCALE2 * .3)
            monstersNear = true;
          }
        if(!monstersNear) {

          monster* undead = new monster;
          undead->base = c2;
          undead->at = Id;
          undead->type = moZombie;
          undead->parent = m;
          undead->parenttype = m->type;
          undead->pid = 0;
          undead->findpat();
          additional.push_back(undead);

          undead = new monster;
          undead->base = c3;
          undead->at = Id;
          undead->type = moGhost;
          undead->parent = m;
          undead->parenttype = m->type;
          undead->findpat();
          undead->pid = 0;
          additional.push_back(undead);

          c3->wall = waAncientGrave;
          addMessage(XLAT("%The1 raises some undead!", m->type));
          }
        }
      }
    }
  if(m->type == moGreaterShark) {
    if(c2->wall == waBoat)
      c2->wall = waNone;
    if(c2->wall == waFrozenLake)
      c2->wall = waLake;
    }
  if(m->type == moDarkTroll && c2->wall == waCavefloor) {
    m->type = moTroll;
    }
  if(isLeader(m->type)) {
    if(c2 != m->base) {
      if(c2->wall == waBigStatue && canPushStatueOn(m->base)) {
        c2->wall = m->base->wall;
        if(cellUnstable(m->base))
          m->base->wall = waChasm;
        else
          m->base->wall = waBigStatue;
        animateMovement(c2, m->base, LAYER_BOAT, NOHINT);
        }
      if(passable_for(m->type, c2, m->base, P_CHAIN | P_ONPLAYER | reflectflag) && !isWatery(c2) && m->inBoat) {
        if(isWatery(m->base)) 
          m->base->wall = waBoat, m->base->mondir = dirfromto(m->base, c2);
        else if(boatStrandable(c2)) c2->wall = waStrandedBoat;
        else if(boatStrandable(m->base)) m->base->wall = waStrandedBoat;
        m->inBoat = false;
        }
      if(isWatery(c2) && isWatery(m->base) && m->inBoat)
        moveItem(m->base, c2, true);
      }
    if(c2->wall == waBoat && !m->inBoat) {
      m->inBoat = true; c2->wall = waSea;
      m->base = c2;
      }
    }
  
  if(peace::on && c2->mpdist > 7) return;
  
  if(!(m->type == moRoseBeauty && c2->land != laRose)) {
    if(stunned ? passable(c2, m->base, P_BLOW | reflectflag) : passable_for(m->type, c2, m->base, P_CHAIN | reflectflag)) {
      if(c2 != m->base && m->type == moButterfly) 
        m->torigin = m->base;
      m->rebasePat(nat);
      if(m->type == moRagingBull && step > 1e-6) m->stunoff = CHARGING;
      }
    else {
      if(peace::on) { igo++; goto igo_retry; }
      if(m->type == moRagingBull && m->stunoff == CHARGING)
        m->stunoff = curtime + BULLSTUN;
      }
    }

  if(direct) {
    if((m->type == moPyroCultist || m->type == moCrystalSage) && curtime >= m->nextshot) {
      monster* bullet = new monster;
      bullet->base = m->base;
      bullet->at = m->at;
      bullet->type = moFireball;
      bullet->parent = m;
      additional.push_back(bullet);
      bullet->pid = directi;
      if(m->type == moPyroCultist) 
        m->type = moCultist;
      else
        m->nextshot = curtime + 100;
      }
    if(m->type == moOutlaw && curtime >= m->nextshot) {
      monster* bullet = new monster;
      bullet->base = m->base;
      bullet->at = m->at;
      bullet->type = moBullet;
      bullet->parent = m;
      bullet->parenttype = moOutlaw;
      bullet->pid = directi;
      additional.push_back(bullet);
      m->nextshot = curtime + 1500;
      }
    for(int i=0; i<players; i++) if(!pc[i]->isVirtual)
    if((m->type == moAirElemental) && curtime >= m->nextshot && intval(m->pat*C0, pc[i]->pat*C0) < SCALE2 * 2) {
      monster* bullet = new monster;
      bullet->base = m->base;
      bullet->at = m->at;
      bullet->type = moAirball;
      bullet->parent = m;
      bullet->pid = i;
      additional.push_back(bullet);
      m->nextshot = curtime + 1500;
      }
    for(int i=0; i<players; i++) if(!pc[i]->isVirtual)
      if(m->type == moTortoise && tortoise::seek() && !tortoise::diff(getBits(m->torigin)) && intval(m->pat*C0, pc[i]->pat*C0) < SCALE2) {
        items[itBabyTortoise] += 4;
        m->dead = true;
        addMessage(XLAT(playergender() == GEN_F ? "You are now a tortoise heroine!" : "You are now a tortoise hero!"));
        }
    for(int i=0; i<players; i++) if(!pc[i]->isVirtual)
    if(m->type == moFlailer && curtime >= m->nextshot && 
      intval(m->pat*C0, pc[i]->pat*C0) < SCALE2 * 2) {
      m->nextshot = curtime + 3500;
      monster* bullet = new monster;
      bullet->base = m->base;
      bullet->at = m->at;
      bullet->type = moFlailBullet;
      bullet->parent = m;
      bullet->vel = 1/400.0;
      bullet->pid = i;
      additional.push_back(bullet);
      break;
      }
    for(int i=0; i<players; i++) if(!pc[i]->isVirtual)
    if(m->type == moCrusher && intval(m->pat*C0, pc[i]->pat*C0) < SCALE2 * .75) {    
      m->stunoff = curtime + 1500;
      monster* bullet = new monster;
      bullet->base = m->base;
      bullet->at = m->at;
      bullet->type = moCrushball;
      bullet->parent = m;
      bullet->pid = i;
      additional.push_back(bullet);
      break;
      }
    }
  }

void activateMonstersAt(cell *c) {
  pair<mit, mit> p = 
    monstersAt.equal_range(c);
  for(mit it = p.first; it != p.second;) {
    mit itplus = it;
    itplus++;
    active.push_back(it->second);
    monstersAt.erase(it);
    it = itplus;
    }
  if(c->monst && isMimic(c->monst)) c->monst = moNone;
  // mimics are awakened by awakenMimics
  if(c->monst && !isIvy(c) && !isWorm(c) && !isMutantIvy(c) && !isKraken(c->monst) && c->monst != moPrincess && c->monst != moHunterGuard) {
    // awaken as a monster
    monster *enemy = new monster;
    enemy->at = Id;
    enemy->base = c;
    if(enemy->type == moButterfly)
      enemy->torigin = createMov(c, (c->mondir + 419) % c->type);
    enemy->torigin = c;
    enemy->type = c->monst;
    enemy->hitpoints = c->hitpoints;
    if(c->wall == waBoat && isLeader(c->monst)) 
      enemy->inBoat = true, c->wall = waSea;
    c->monst = moNone;
    active.push_back(enemy);
    }
  }

void fixStorage() {

  vector<monster*> restore;

  for(auto it = monstersAt.begin(); it != monstersAt.end(); it++) 
    restore.push_back(it->second);

  monstersAt.clear();

  for(monster *m: restore) m->store();
  }

hookset<bool(int)> *hooks_turn;

void turn(int delta) {

  if(racing::on && subscreens::split( [delta] () { turn(delta); })) return;

  if(callhandlers(false, hooks_turn, delta)) return;
  if(!shmup::on) return;
  
  if(!(cmode & sm::NORMAL)) {
    #if CAP_RACING
    if(racing::on) {
      if(racing::race_start_tick) racing::race_start_tick += delta;
      for(int& i: racing::race_finish_tick) if(i) i += delta;
      }
    #endif
    return;
    }

  timetowait = 0;

  passive_switch = (gold() & 1) ? moSwitch1 : moSwitch2;
  lmousetarget = NULL;
  if(mousetarget && !mousetarget->isVirtual && intval(mouseh, mousetarget->pat*C0) < 0.1)
    lmousetarget = mousetarget;
  
  if(delta > 1000) delta = 1000;
  
  if(delta > 200) { turn(200); delta -= 200; if(!delta) return; }

  curtime += delta;

  handleInput(delta);
  
  invismove = (curtime >= visibleAt) && markOrb(itOrbInvis);

  // detect active monsters
  if(doall)
    for(cell *c: currentmap->allcells()) activateMonstersAt(c);
  else
    for(unordered_map<cell*, transmatrix>::iterator it = gmatrix.begin(); it != gmatrix.end(); it++) 
      activateMonstersAt(it->first);
  
  /* printf("size: gmatrix = %ld, active = %ld, monstersAt = %ld, delta = %d\n", 
    gmatrix.size(), active.size(), monstersAt.size(),
    delta); */
  
  bool exists[motypes];
  
  for(int i=0; i<motypes; i++) exists[i] = false;

  nonvirtual.clear();
  for(monster *m: active) {
    m->findpat();
    if(m->isVirtual) continue;
    else nonvirtual.push_back(m);
    exists[movegroup(m->type)] = true;
    }
  
  for(monster *m: active) {
    
    switch(m->type) {
      case moPlayer: 
        movePlayer(m, delta);
        break;
      
      case moBullet: case moFlailBullet: case moFireball: case moTongue: case moAirball:
      case moArrowTrap: case moCrushball:
        moveBullet(m, delta);
        break;
      
      default: ;
      }
    }

  for(monster *m: active) {
    if(isMimic(m->type))
      moveMimic(m);
    }

  for(int t=1; t<motypes; t++) if(exists[t]) {
  
    pathdata pd(1);
        
    // build the path data
    
    for(cell *c: targets)
      onpath(c, isPlayerOn(c) ? 0 : 1);

    int qb = 0;
    for(qb=0; qb < isize(pathq); qb++) {
      cell *c = pathq[qb];
      int d = c->pathdist;
      if(d == PINFD-1) continue;
      for(int i=0; i<c->type; i++) {
        cell *c2 = c->move(i);
        // printf("i=%d cd=%d\n", i, c->move(i)->cpdist);
        if(c2 && c2->pathdist == PINFD && gmatrix.count(c2) && 
          (passable_for(eMonster(t), c, c2, P_CHAIN | P_ONPLAYER) || c->wall == waThumperOn)) {
          onpath(c2, d+1);
          }
        }
      }
    
    // printf("time %d, t=%d, q=%d\n", curtime, t, qb);
  
    // move monsters of this type
    
    for(monster *m: nonvirtual)
      if(movegroup(m->type) == t)
        moveMonster(m, delta);
    }
  
  if(shmup::on) {

    doTraps();

    bool tick = curtime >= nextmove;
    keepLightning = ticks <= lightat + 1000;
    cwt.at = pc[0]->base;
    bfs(); moverefresh(tick);
    countLocalTreasure();
    pushmonsters();
    if(items[itOrbFreedom])
      for(int i=0; i<players; i++)
        checkFreedom(pc[i]->base);
    heat::processheat(delta / 350.0);
    markOrb(itOrbSpeed);
    
    if((havewhat&HF_DRAGON) && curtime >= nextdragon) {
      groupmove(moDragonHead, 0);
      nextdragon = curtime + 1500;
      }
  
    if(tick) {
      nextmove += 1000;
      flashMessages();
      reduceOrbPowers();
      if(items[itOrbBull]) for(int p=0; p<players; p++) 
        if(bulltime[p] < curtime - 600) orbbull::gainBullPowers();
        
      if(!((items[itOrbSpeed]/players) & 1)) {
        if(havewhat&HF_KRAKEN) kraken::attacks(), groupmove(moKrakenH, 0);     
        moveworms();
        moveivy();
        movemutant();
        if(havewhat&HF_HEX) movehex_all();
        wandering();
        livecaves();
        terracotta();
        heat::processfires();
        if(havewhat&HF_WHIRLPOOL) whirlpool::move();
        if(havewhat&HF_WHIRLWIND) whirlwind::move();
        #if CAP_COMPLEX2
        if(havewhat & HF_WESTWALL) westwall::move();
        #endif
        buildRosemap();
        if(havewhat&HF_RIVER) prairie::move();
        }
      if(recallCell && !markOrb(itOrbRecall)) activateRecall();
      save_memory();
      }
    if(elec::havecharge) elec::act();
    popmonsters();
    
    bool lastcanmove = canmove;
    
    canmove = true;
    
    for(int i=0; i<players; i++) {
      if(pc[i]->dead && items[itOrbShield]) {
        pc[i]->dead = false;
        orbused[itOrbShield] = true;
        }
    
      if(pc[i]->dead && items[itOrbFlash]) {
        pc[i]->dead = false;
        pushmonsters();
        killMonster(pc[i]->base, moNone);
        activateFlash();
        popmonsters();
        }
        
      if(pc[i]->dead && items[itOrbLightning]) {
        pc[i]->dead = false;
        pushmonsters();
        killMonster(pc[i]->base, moLightningBolt);
        activateLightning();
        popmonsters();
        }
      
      if(pc[i]->dead && items[itOrbShell]) {
        pc[i]->dead = false;
        useupOrb(itOrbShell, 10);
        items[itOrbShield] = 1;
        orbused[itOrbShield] = true;
        }
    
      if(pc[i]->dead && items[itOrbLife]) {
        multi::deaths[i]++;
        items[itOrbLife]--;
        items[itOrbShield] += 3;
        items[itOrbAether] += 3;
        pc[i]->dead = false;
        orbused[itOrbShield] = true;
        }
    
      if(pc[i]->dead && !lastdead) {
        multi::deaths[i]++;
        achievement_final(true);
        }
      lastdead = pc[i]->dead;
      
      if(lastcanmove && pc[i]->dead) showMissionScreen();
        
      canmove = canmove && !pc[i]->dead;
      }
    }
  
  for(monster *m: additional) 
    active.push_back(m);
  additional.clear();
  
  // deactivate all monsters
  for(monster *m: active)
    if(m->dead && m->type != moPlayer) {
      for(monster *m2: active) if(m2->parent == m)
        m2->parent = m->parent;
      if(m == mousetarget) mousetarget = NULL;
      if(m == lmousetarget) lmousetarget = NULL;
      delete m;
      }
    else {
      m->store();
      }
    
  active.clear();

  if(safety) { 
    activateSafety(pc[0]->base->land);
    safety = false;
    }
  }

void recall() {
  for(int i=0; i<players; i++) {
    pc[i]->base = cwt.at;
    if(players == 1)
      pc[i]->at = Id;
    else
      pc[i]->at = spin(2*M_PI*i/players) * xpush(firstland == laMotion ? .5 : .3) * Id;
    /* ggmatrix(cwt.at);
    display(gmatrix[cwt.at]);
    pc[i]->findpat(); */
    }
  destroyMimics();
  }

void init() {

  for(int i=0; i<players; i++) pc[i] = NULL;
  
  for(int i=0; i<players; i++) {
    pc[i] = new monster;
    pc[i]->type = moPlayer;
    pc[i]->pid = i;
    if(players == 1)
      pc[i]->at = Id;
    else
      pc[i]->at = spin(2*M_PI*i/players) * xpush(firstland == laMotion ? .5 : .3) * Id;
    pc[i]->base = cwt.at;
    pc[i]->vel = 0;
    pc[i]->inBoat = (firstland == laCaribbean || firstland == laOcean || firstland == laLivefjord ||
      firstland == laWhirlpool);
    pc[i]->store();
    }
  
  if(!safety) {
    items[itOrbLife] = 3;
    if(!racing::on) 
      addMessage(XLAT("Welcome to the Shoot'em Up mode!"));
    // addMessage(XLAT("F/;/Space/Enter/KP5 = fire, WASD/IJKL/Numpad = move"));
    }
  else safety = false;
  }

bool boatAt(cell *c) {
  pair<mit, mit> p = 
    monstersAt.equal_range(c);
  for(mit it = p.first; it != p.second; it++) {
    monster* m = it->second;
    if(m->inBoat) return true;
    }
  return false;
  }

hookset<bool(const transmatrix&, cell*, shmup::monster*)> *hooks_draw;

bool drawMonster(const transmatrix& V, cell *c, const transmatrix*& Vboat, transmatrix& Vboat0, const transmatrix *Vdp) {
  #if CAP_SHAPES

  pair<mit, mit> p = 
    monstersAt.equal_range(c);
    
  if(p.first == p.second) return false;
  ld zlev = -geom3::factor_to_lev(zlevel(tC0((*Vdp))));
   
  vector<monster*> monsters;

  for(mit it = p.first; it != p.second; it++) {
    monster* m = it->second;
    if(c != m->base) continue; // may happen in RogueViz Collatz
    m->pat = ggmatrix(m->base) * m->at;
    transmatrix view = V * m->at;
    
    if(!mouseout()) {
      if(m->no_targetting) ; else
      if(mapeditor::drawplayer || m->type != moPlayer)
      if(!mousetarget || intval(mouseh, mousetarget->pat*C0) > intval(mouseh, m->pat*C0)) 
        mousetarget = m;
      }
    
    if(m->inBoat) {
      view = m->pat;
      Vboat = &Vboat0;
      if(DIM == 2) Vboat0 = view;
      if(DIM == 3) Vboat0 = view * spin(-M_PI/2);
      
      bool magic = m->type == moPlayer && items[itOrbWater];
      color_t outcolor = magic ? watercolor(0) : 0xC06000FF;
      color_t incolor = magic ? 0x0060C0FF : 0x804000FF;
      
      if(DIM == 2) {
        queuepoly(Vboat0, shBoatOuter, outcolor);
        queuepoly(Vboat0, shBoatInner, incolor);
        }
      if(DIM == 3) {
        queuepoly(mscale(Vboat0, scalefactor/2), shBoatOuter, outcolor);
        queuepoly(mscale(Vboat0, scalefactor/2-0.01), shBoatInner, incolor);
        }
      }

    if(doHighlight())
      poly_outline = 
        isBullet(m) ? 0x00FFFFFF :
        (isFriendly(m->type) || m->type == moPlayer) ? 0x00FF00FF : 0xFF0000FF;

    int q = ptds.size();
    if(q != isize(ptds) && !m->inBoat) pushdown(c, q, view, zlev, true, false);

    if(callhandlers(false, hooks_draw, V, c, m)) continue;

    switch(m->type) {
      case moPlayer: 
        playerfound = true;

        if(!hide_player() || !subscreens::is_current_player(m->pid)) {
          dynamicval<int> d(cpid, m->pid);
          if(DIM == 3) view = view * spin(-M_PI/2);
          drawPlayerEffects(view, c, true);
          if(m->inBoat) m->footphase = 0;
          if(mapeditor::drawplayer) drawMonsterType(moPlayer, c, view, 0xFFFFFFC0, m->footphase);
          }

        if(subscreens::is_current_player(m->pid) && keyresult[cpid]) {
          hyperpoint h = keytarget(cpid);
          if(DIM == 2) 
            queuechr(h, vid.fsize, '+', iinf[keyresult[cpid]].color);
          else {
            dynamicval<color_t> p(poly_outline, darkena(iinf[keyresult[cpid]].color, 0, 255));
            queuepoly(rgpushxto0(h) * cspin(0, 1, ticks / 140.), shGem[1], 0);
            }
          }

        break;
      case moBullet: {
        color_t col;
        cpid = m->pid;
        if(m->parenttype == moPlayer)
          col = getcs().swordcolor;
        else if(m->parenttype == moMimic)
          col = (mirrorcolor(det(view) < 0) << 8) | 0xFF;
        else
          col = (minf[m->parenttype].color << 8) | 0xFF;
        if(getcs().charid >= 4) {
          queuepoly(mmscale(view, 1.15), shPHead, col);
          ShadowV(view, shPHead);
          }
        else if(peace::on) {
          queuepolyat(mmscale(view, 1.15), shDisk, col, PPR::MISSILE);
          ShadowV(view, shPHead);
          }
        else {
          transmatrix t = view * spin(curtime / 50.0);
          queuepoly(mmscale(t, 1.15), shKnife, col);
          ShadowV(t, shKnife);
          }
        break;
        }
      case moArrowTrap: {
        queuepoly(mmscale(view, 1.15), shTrapArrow, 0xFFFFFFFF);
        ShadowV(view, shTrapArrow);
        break;
        }
      case moTongue: {
        queuepoly(mmscale(view, 1.15), shTongue, (minf[m->parenttype].color << 8) | 0xFF);
        ShadowV(view, shTongue);
        break;
        }
      case moFireball:  case moAirball: { // case moLightningBolt:
        queuepoly(mmscale(view, 1.15), shPHead, (minf[m->type].color << 8) | 0xFF);
        ShadowV(view, shPHead);
        break;
        }
      case moFlailBullet: case moCrushball: {
        transmatrix t = view * spin(curtime / 50.0);
        queuepoly(mmscale(t, 1.15), shFlailMissile, (minf[m->type].color << 8) | 0xFF);
        ShadowV(view, shFlailMissile);
        break;
        }

      default:
        if(m->inBoat) m->footphase = 0;
        color_t col = minf[m->type].color;
        if(m->type == moMimic) 
          col = mirrorcolor(det(view) < 0);
        if(m->type == moSlime) {
          col = winf[c->wall].color;
          col |= (col >> 1);
          }
        cpid = m->pid; 
        if(m->stunoff > curtime)
          c->stuntime = 1 + (m->stunoff - curtime-1)/300;
        if(hasHitpoints(m->type))
          c->hitpoints = m->hitpoints;
        if(m->type == moTortoise) tortoise::emap[c] = getBits(m->torigin);
        if(DIM == 3)
          drawMonsterType(m->type, c, view * cspin(0, 2, M_PI/2), col, m->footphase);
        else
          drawMonsterType(m->type, c, view, col, m->footphase);
        if(m->type == moTortoise) tortoise::emap.erase(c);
        break;
      }
    }

  #endif
  return false;
  }

void clearMonsters() {
  for(mit it = monstersAt.begin(); it != monstersAt.end(); it++)
    delete(it->second);
  for(monster *m: active) delete m;
  mousetarget = NULL;
  lmousetarget = NULL;
  monstersAt.clear();
  active.clear();
  }

void clearMemory() {
  clearMonsters();
  gmatrix.clear();
  while(!traplist.empty()) traplist.pop();
  curtime = 0;
  nextmove = 0;
  nextdragon = 0;
  visibleAt = 0;
  for(int i=0; i<MAXPLAYER; i++) pc[i] = NULL;
  }

cell *playerpos(int i) {
  if(!pc[i]) return NULL;
  return pc[i]->base;
  }

bool playerInBoat(int i) {
  if(!pc[i]) return false;
  return pc[i]->inBoat;
  }

void destroyBoats(cell *c) {
  for(monster *m: active)
    if(m->base == c && m->inBoat)
      m->inBoat = false;
  }

transmatrix master_relative(cell *c, bool get_inverse) {
  if(0); 
  #if CAP_IRR
  else if(IRREGULAR) {
    int id = irr::cellindex[c];
    ld alpha = 2 * M_PI / S7 * irr::periodmap[c->master].base.spin;
    return get_inverse ? irr::cells[id].rpusher * spin(-alpha-master_to_c7_angle()): spin(alpha + master_to_c7_angle()) * irr::cells[id].pusher;
    }
  #endif
  #if CAP_GP
  else if(GOLDBERG) {
    if(c == c->master->c7) {
      return spin((get_inverse?-1:1) * master_to_c7_angle());
      }
    else {
      auto li = gp::get_local_info(c);
      transmatrix T = spin(master_to_c7_angle()) * gp::Tf[li.last_dir][li.relative.first&31][li.relative.second&31][gp::fixg6(li.total_dir)];
      if(get_inverse) T = inverse(T);
      return T;
      }
    }
  #endif
  else if(BITRUNCATED && !euclid) {
    for(int d=0; d<S7; d++) if(c->master->c7->move(d) == c)
      return (get_inverse?invhexmove:hexmove)[d];
    return Id;
    }
  else
    return pispin * Id;
  }

void virtualRebase(shmup::monster *m, bool tohex) {
  virtualRebase(m->base, m->at, tohex);
  }

hookset<bool(shmup::monster*, string&)> *hooks_describe;

void addShmupHelp(string& out) {
  if(shmup::mousetarget && intval(mouseh, tC0(shmup::mousetarget->pat)) < .1) {
    if(callhandlers(false, hooks_describe, shmup::mousetarget, out)) return;
    out += XLAT1(minf[shmup::mousetarget->type].name);
    help = generateHelpForMonster(shmup::mousetarget->type);
    }
  }

auto hooks = addHook(clearmemory, 0, shmup::clearMemory) +
  addHook(hooks_removecells, 0, [] () {
    for(mit it = monstersAt.begin(); it != monstersAt.end();) {
      if(is_cell_removed(it->first)) {
        monstersAt.insert(make_pair(nullptr, it->second));
        auto it0 = it; it++;
        monstersAt.erase(it0);
        }
      else it++;
      }
    });
    
}
}
