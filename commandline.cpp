// Hyperbolic Rogue -- commandline options
// Copyright (C) 2011-2018 Zeno Rogue, see 'hyper.cpp' for details

namespace hr {

#if CAP_COMMANDLINE
const char *scorefile = "hyperrogue.log";

namespace arg {
eLand readland(const string& ss) {
  if(ss == "II") return laCrossroads2;
  if(ss == "III") return laCrossroads3;
  if(ss == "IV") return laCrossroads4;
  if(ss == "V") return laCrossroads5;
  for(int l=0; l<landtypes; l++) if(appears(linf[l].name, ss)) {
    return eLand(l);
    break;
    }
  return laNone;
  }

eItem readItem(const string& ss) {
  for(int i=0; i<ittypes; i++) if(appears(iinf[i].name, ss)) {
    return eItem(i);
    break;
    }
  return itNone;
  }

eMonster readMonster(const string& ss) {
  for(int i=0; i<motypes; i++) if(appears(minf[i].name, ss)) {
    return eMonster(i);
    break;
    }
  return moNone;
  }
}

void initializeCLI() {
  printf("HyperRogue by Zeno Rogue <zeno@attnam.com>, version " VER "\n");

#ifndef NOLICENSE
  printf("released under GNU General Public License version 2 and thus\n");
  printf("comes with absolutely no warranty; see COPYING for details\n");
#endif

  #ifdef FHS
  static string sbuf, cbuf;
  if(getenv("HOME")) {
    sbuf = getenv("HOME"); sbuf += "/."; sbuf += scorefile;
    cbuf = getenv("HOME"); cbuf += "/."; cbuf += conffile;
    scorefile = sbuf.c_str();
    conffile = cbuf.c_str();
    }
  #endif
  }

namespace arg {
  vector<string> argument;
  int pos;

  void lshift() { pos++; }

  void shift() {
    lshift(); if(pos > isize(argument)) { printf("Missing parameter\n"); exit(1); }
    }
  
  const string& args() { return argument[pos]; }
  const char* argcs() { return args().c_str(); }
  int argi() { return atoi(argcs()); }
  unsigned arghex() { return strtoll(argcs(), NULL, 16); }
  ld argf() { return parseld(args()); }
  bool argis(const string& s) { if(args()[0] == '-' && args()[1] == '-') return args().substr(1) == s; return args() == s; }
  
  void init(int argc, char **argv) { for(int i=0; i<argc; i++) argument.push_back(argv[i]); shift(); }
 
  void phaseerror(int x) {
    printf("Command line error: cannot read command '%s' from phase %d in phase %d\n", args().c_str(), x, curphase);
    exit(1);
    }

  }

int arg::readCommon() {

// first phase options

  if(argis("-s")) { PHASE(1); shift(); scorefile = argcs(); }
  else if(argis("-nogui")) { PHASE(1); noGUI = true; }
#ifndef EMSCRIPTEN
  else if(argis("-font")) { PHASE(1); shift(); fontpath = args(); }
#endif

  else if(argis("-test")) 
    callhooks(hooks_tests);
  else if(argis("-offline")) {
    PHASE(1);
    offlineMode = true;
    }
  else if(argis("-debugf")) {
    debugfile = fopen("hyperrogue-debug.txt", "w");
    shift(); debugflags = argi();
    }
  else if(argis("-debuge")) {
    debugfile = stderr;
    shift(); debugflags = argi();
    }
  else if(argis("-run")) {
    PHASE(3); 
    start_game();
    mainloop(); quitmainloop = false;
    }
  else if(argis("-msg")) {
    shift(); addMessage(args());
    printf("%s\n", args().c_str());
    }
  else if(argis("-msg0")) {
    clearMessages();
    }
#if CAP_TOUR
  else if(argis("-tour")) {
    PHASEFROM(2); start_game(); tour::start();
    }
  else if(argis("-presentation")) {
    PHASEFROM(2); tour::texts = false;
    start_game(); tour::start();
    }
#endif
  else if(argis("-draw")) {
    PHASE(3); drawscreen();
    }
  else if(argis("-rotate")) {
    PHASE(3); 
    shift(); ld a = argf();
    shift(); ld b = argf();
    View = View * spin(M_PI * 2 * a / b);
    }
  else if(argis("-exit")) {
    PHASE(3); printf("Success.\n");
    exit(0);
    }

// graphical options
  else if(argis("-noscr")) {
    PHASE(3);
    popScreenAll();
    showstartmenu = false;
    }

// informational
  else if(argis("-version") || argis("-v")) {
    printf("HyperRogue version " VER "\n");
    exit(0);
    }
  else if(argis("-L")) {
    printf("Treasures:\n");
    for(int i=1; i<ittypes; i++) 
      if(itemclass(eItem(i)) == IC_TREASURE)
        printf("    %s\n", iinf[i].name);
    printf("\n");
    printf("Orbs:\n");
    for(int i=1; i<ittypes; i++) 
      if(itemclass(eItem(i)) == IC_ORB)
        printf("    %s\n", iinf[i].name);
    printf("\n");
    printf("Other items:\n");
    for(int i=1; i<ittypes; i++) 
      if(itemclass(eItem(i)) == IC_OTHER)
        printf("    %s\n", iinf[i].name);
    printf("\n");
    printf("Monsters:\n");
    for(int i=1; i<motypes; i++) 
      printf("    %s\n", minf[i].name);
    printf("\n");
    printf("Lands:\n");
    for(int i=1; i<landtypes; i++) 
      printf("    %s\n", linf[i].name);
    printf("\n");
    printf("Walls:\n");
    for(int i=0; i<walltypes; i++) 
      printf("    %s\n", winf[i].name);
    printf("\n");
    exit(0);
    }
  else if(argis("")) {}
  else if(argis("-help") || argis("-h")) {
    printf("Press F1 while playing to get ingame options.\n\n");
    printf("HyperRogue accepts the following command line options:\n");
    printf("  -c FILE        - use the specified configuration file\n");
    printf("  -s FILE        - use the specified highscore file\n");
    printf("  -m FILE        - use the specified soundtrack (music)\n");
    printf("  -se DIR        - the directory containing sound effects\n");
    printf("  -lev FILE      - use the specified filename for the map editor (without loading)\n");
    printf("  -load FILE     - use the specified filename for the map editor\n");
    printf("  -canvas COLOR  - set background color or pattern code for the canvas\n");
    printf("  --version, -v  - show the version number\n");
    printf("  --help, -h     - show the commandline options\n");
    printf("  -f*            - toggle fullscreen mode\n");
    printf("  -wm n, -mm n   - start in the given wallmode or monmode\n");
    printf("  -r WxHxF       - use the given resolution and font size\n");
    printf("  -o*            - toggle the OpenGL mode\n");
    printf("  -W LAND        - start in the given land (cheat)\n");
    printf("  -W2 LAND       - make the given land easy to find (also turns on autocheat)\n");
    printf("  -ch            - auto-enable cheat mode\n");
    printf("  -geo n         - switch geometry (1=Euclidean, 2=spherical, 3=elliptic, 4/5=quotient)\n");
    printf("  -qs <desc>     - fieldpattern: quotient by the given <desc> (must be followed by qpar)\n");
    printf("  -qpar <prime>  - fieldpattern: use the given prime instead of 43\n");
    printf("  -cs <desc>     - fieldpattern: set subpath to the given <desc> (cannot be followed by qpar)\n");
    printf("  -csp           - fieldpattern: find the subpath of order <prime> (cannot be followed by qpar)\n");
    printf("  -S*            - toggle Shmup\n");
    printf("  -P n           - switch Shmup number of players (n=1..7)\n");
    printf("  -PM            - switch the model index\n");
    printf("  -H*            - toggle Hardcore\n");
    printf("  -T*            - toggle Tactical\n");
    printf("  -7*            - toggle heptagonal mode\n");
    printf("  -C*            - toggle Chaos mode\n");
    printf("  -R*            - toggle Random Pattern\n");
    printf("  -Y id          - enable Yendor, level id\n");
    printf("  -D             - disable all the special game modes\n");
    printf("  -L             - list of features\n");
    printf("  -debugf 7      - output debugging information to hyperrogue-debug.txt\n");
    printf("  -debuge 7      - output debugging information to stderr\n");
    printf("  -offline       - don't connect to Steam (for Steam versions)\n");
    printf("  -I ITEM n      - start with n of ITEM (activates cheat and disables ghosts)\n");
    printf("  -fix           - fix the seed\n");
    printf("Toggles: -o0 disables, -o1 enables, -o switches");
    printf("Not all options are documented, see hyper.cpp");
    exit(0);
    }
  else return 1;
  return 0;
  }

purehookset hooks_config;

hookset<int()> *hooks_args;

namespace arg {
  int curphase;

  auto ah = addHook(hooks_args, 0, readCommon);
  
  void read(int phase) { 
    curphase = phase;
    callhooks(hooks_config);
    while(pos < isize(argument)) {
      for(auto& h: *hooks_args) {
        int r = h.second(); if(r == 2) return; if(r == 0) { lshift(); goto cont; }
        }
      printf("Unknown option: %s\n", argcs());
      exit(3);
      cont: ;
      }
    }
  }
#endif

}
