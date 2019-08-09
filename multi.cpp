// Hyperbolic Rogue

// Copyright (C) 2011-2018 Zeno Rogue, see 'hyper.cpp' for details

// implementation of the multiplayer features

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

  EX int players = 1;
  EX cellwalker player[MAXPLAYER];
  EX vector<int> revive_queue; // queue for revival
  
  EX cell *origpos[MAXPLAYER], *origtarget[MAXPLAYER];

  EX bool flipped[MAXPLAYER];
  
  // treasure collection, kill, and death statistics
  EX int treasures[MAXPLAYER], kills[MAXPLAYER], deaths[MAXPLAYER];
  
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
  
  transmatrix whereis[MAXPLAYER];
  transmatrix crosscenter[MAXPLAYER];
  double ccdist[MAXPLAYER];
  cell *ccat[MAXPLAYER];
  
  bool combo[MAXPLAYER];

  EX int cpid; // player id -- an extra parameter for player-related functions
  EX int cpid_edit; // cpid currently being edited
  
  EX movedir whereto[MAXPLAYER]; // player's target cell  

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

const char* axemodes3[4] = {
  "do nothing",
  "camera forward",
  "camera rotate X",
  "camera rotate Y"
  };

int centerplayer = -1;

char* axeconfigs[24]; int numaxeconfigs;
int* dzconfigs[24];

string listkeys(int id) {
#if CAP_SDL
  string lk = "";
  for(int i=0; i<512; i++)
    if(scfg.keyaction[i] == id)
      lk = lk + " " + SDL_GetKeyName(SDLKey(i));
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
          (DIM == 3 && (aa%SHMUPAXES < 4)) ? XLAT(axemodes3[aa%SHMUPAXES]) :
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

struct shmup_configurer {

  void operator()() {
  #if CAP_SDL
    cmode = sm::SHMUPCONFIG;
    gamescreen(3);
    dialog::init(XLAT("keyboard & joysticks"));
    
    bool haveconfig = shmup::on || players > 1 || multi::alwaysuse;
  
    if(haveconfig)
      dialog::addItem(XLAT("configure player 1"), '1');
    else
      dialog::addBreak(100);
    if(players > 1)
      dialog::addItem(XLAT("configure player 2"), '2');
    else if(players == 1 && !shmup::on)
      dialog::addSelItem(XLAT("input"), XLAT(multi::alwaysuse ? "config" : "default"), 'a');
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
        const char *axmodes[5] = {"OFF", "auto", "light", "heavy", "arrows"};
        dialog::addSelItem(XLAT("help for keyboard users"), XLAT(axmodes[vid.axes]), 'h');
        dialog::add_action([] {vid.axes += 60 + (shiftmul > 0 ? 1 : -1); vid.axes %= 5; } );
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
    else if(uni == '1') pushScreen(key_configurer(1, cmdlist));
    else if(uni == '2') pushScreen(key_configurer(2, cmdlist));
    else if(uni == 'p') pushScreen(key_configurer(3, GDIM == 3 ? pancmds3 : pancmds));
    else if(uni == '3') pushScreen(key_configurer(4, cmdlist));
    else if(uni == '4') pushScreen(key_configurer(5, cmdlist));
    else if(uni == '5') pushScreen(key_configurer(6, cmdlist));
    else if(uni == '6') pushScreen(key_configurer(7, cmdlist));
    else if(uni == '7') pushScreen(key_configurer(8, cmdlist));
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
  gamescreen(1);
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

  if(multi::players > 1) {
    dialog::addItem(XLAT("reset per-player statistics"), 'r');
    dialog::add_action([] {
      for(int i=0; i<MAXPLAYER; i++) 
        kills[i] = deaths[i] = treasures[i] = 0;
      });

    dialog::addSelItem(XLAT("keyboard & joysticks"), "", 'k');
    dialog::add_action(multi::configure);
    }
  else dialog::addBreak(200);
  
  dialog::addBack();
  dialog::display();
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
  int k = scfg.keyaction[sym];
  if(k == 0) return true;
  k /= 16;
  if(k > 3) k--; else if(k==3) k = 0;
  return k > multi::players;
  }

EX void initConfig() {
  
  char* t = scfg.keyaction;
  
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
  
  addsaver(multi::players, "mode-number of players");
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
  }

void handleInput(int delta) {
#if CAP_SDL
  double d = delta / 500.;

  Uint8 *keystate = SDL_GetKeyState(NULL);

  for(int i=0; i<NUMACT; i++) 
    lactionpressed[i] = actionspressed[i],
    actionspressed[i] = 0;

  for(int i=0; i<SHMUPAXES; i++) axespressed[i] = 0;
  
  for(int i=0; i<SDLK_LAST; i++) if(keystate[i]) 
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

  if(keystate[SDLK_LCTRL] || keystate[SDLK_RCTRL]) d /= 5;
  
  double panx = 
    actionspressed[49] - actionspressed[51] + axespressed[2] / 32000.0;
  double pany = 
    actionspressed[50] - actionspressed[48] + axespressed[3] / 32000.0;
    
  double panspin = actionspressed[52] - actionspressed[53];
  
  double panmove = actionspressed[59] - actionspressed[60];
  
  if(DIM == 3)
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
    pushScreen(showMainMenu);
    
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
    if(DIM == 2) {
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

  int tableid[7] = {1, 2, 4, 5, 6, 7, 8};

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
  
  EX }

}