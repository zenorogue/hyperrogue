// Hyperbolic Rogue -- commandline options
// Copyright (C) 2011-2018 Zeno Rogue, see 'hyper.cpp' for details

/** \file commandline.cpp 
 *  \brief Commandline options support
 */

#include "hyper.h"
namespace hr {

#ifdef RESOURCEDESTDIR
EX string rsrcdir = RESOURCEDESTDIR;
#endif

#ifndef RESOURCEDESTDIR
EX string rsrcdir = "";
#endif

#if CAP_COMMANDLINE
EX string scorefile = "hyperrogue.log";

EX namespace arg {
EX eLand readland(const string& ss) {
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

EX eItem readItem(const string& ss) {
  for(int i=0; i<ittypes; i++) if(appears(iinf[i].name, ss)) {
    return eItem(i);
    break;
    }
  return itNone;
  }

EX eMonster readMonster(const string& ss) {
  if(ss == "Pike") return moPike;
  for(int i=0; i<motypes; i++) if(appears(minf[i].name, ss)) {
    return eMonster(i);
    break;
    }
  return moNone;
  }
EX }

EX void initializeCLI() {
  printf("HyperRogue by Zeno Rogue <zeno@attnam.com>, version " VER "\n");

#if !NOLICENSE
  printf("released under GNU General Public License version 2 and thus\n");
  printf("comes with absolutely no warranty; see COPYING for details\n");
#endif

  #ifdef FHS
  static string sbuf, cbuf;
  if(getenv("HOME")) {
    sbuf = getenv("HOME"); sbuf += "/."; sbuf += scorefile;
    cbuf = getenv("HOME"); cbuf += "/."; cbuf += conffile;
    scorefile = sbuf;
    conffile = cbuf.c_str();
    }
  #endif
  }

EX namespace arg {
  EX int curphase;

  EX vector<string> argument;
  EX int pos;

  EX void lshift() { pos++; }
  EX void unshift() { pos--; }

  EX void shift() {
    lshift(); if(pos >= isize(argument)) { printf("Missing parameter\n"); exit(1); }
    }
  
  EX bool nomore() { return pos >= isize(argument); }
  
  EX const string& args() { return argument[pos]; }
  EX const char* argcs() { return args().c_str(); }
  EX int argi() { return atoi(argcs()); }
  EX long long argll() { return atoll(argcs()); }

  EX int shift_argi() { shift(); return argi(); }
  EX const string& shift_args() { shift(); return args(); }

  EX unsigned arghex() { return strtoll(argcs(), NULL, 16); }
  EX ld argf() { 
    try {
      return parseld(args());
      }
    catch(hr_parse_exception& ex) {
      println(hlog, "error parsing commandline parameters: ", ex.s);
      exit(1);
      }
    }
  EX bool argis(const string& s) { if(args()[0] == '-' && args()[1] == '-') return args().substr(1) == s; return args() == s; }

  EX color_t argcolor(int bits) {
    string s = args();
    auto p = find_color_by_name(s);
    if(p && bits == 24) return p->second;
    if(p && bits == 32) return (p->second << 8) | 0xFF;
    return strtoll(argcs(), NULL, 16);
    }
  
  EX void shift_arg_formula(ld& x, const reaction_t& r IS(reaction_t())) {
    shift(); ld old = x; x = argf(); 
    #if CAP_ANIMATIONS
    anims::animate_parameter(x, args(), r); 
    #endif
    if(old != x && r) r();
    }
  
  #if HDR

  // an useful macro
  #define PHASE(x) { if(arg::curphase > x) arg::phaseerror(x); else if(arg::curphase < x) return 2; }
  #define PHASEFROM(x) { if(arg::curphase < x) return 2; }

  #define TOGGLE(x, param, act) \
  else if(args()[0] == '-' && args()[1] == x && !args()[2]) { PHASEFROM(2); showstartmenu = false; act; } \
  else if(args()[0] == '-' && args()[1] == x && args()[2] == '1') { PHASEFROM(2); showstartmenu = false; if(!param) act; } \
  else if(args()[0] == '-' && args()[1] == x && args()[2] == '0') { PHASEFROM(2); showstartmenu = false; if(param) act; }

  #endif

  EX void cheat() { autocheat = true; cheater++; timerghost = false; }
  
  EX void run_arguments(const vector<string> vec) {
    dynamicval<int> p(pos, 0);
    dynamicval<vector<string>> orig(argument, vec);
    read(3);
    }

  EX void init(int argc, char **argv) { for(int i=0; i<argc; i++) argument.push_back(argv[i]); lshift(); }
 
  EX void phaseerror(int x) {
    printf("Command line error: cannot read command '%s' from phase %d in phase %d\n", args().c_str(), x, curphase);
    exit(1);
    }

  bool dialog_launched = false;
  
  EX void launch_dialog(const reaction_t& r IS(reaction_t())) {
    if(!dialog_launched) {
      popScreenAll();
      showstartmenu = false;
      dialog_launched = true;
      }
    if(r) pushScreen(r);
    }

  EX int readCommon();
  EX int readLocal();  
  EX void read(int phase);
EX }


int arg::readCommon() {

// first phase options

  if(argis("-s")) { PHASE(2); shift(); scorefile = args(); savefile_selection = false; }
  else if(argis("-no-s")) { PHASE(2); scorefile = ""; savefile_selection = false; }
  else if(argis("-rsrc")) { PHASE(1); shift(); rsrcdir = args(); }
  else if(argis("-nogui")) { PHASE(1); noGUI = true; }
#ifndef EMSCRIPTEN
#if CAP_SDL
  else if(argis("-font")) { PHASE(1); shift(); fontpath = args();
    #ifdef FONTCONFIG
    font_to_find = "";
    #endif
    }
#ifdef FONTCONFIG
  else if(argis("-find-font")) { PHASE(1); shift(); font_to_find = args(); }
#endif
#endif
#endif

  else if(argis("-test-ach")) 
    test_achievements = true;

  else if(argis("-test")) 
    callhooks(hooks_tests);
  else if(argis("-offline")) {
    PHASE(1);
    offlineMode = true;
    }
  else if(argis("-no-stamp")) {
    debugflags &=~ DF_TIME;
    }
  else if(argis("-debf")) {
    shift(); 
    string s = args();
    for(char c: s) {
      for(int i=0; i<int(strlen(DF_KEYS)); i++) {
        if(DF_KEYS[i] == c) debugflags |= (1<<i);
        else if(DF_KEYS[i] == (c ^ 32)) debugflags &= ~(1<<i);
        }
      if(c >= '0' && c <= '9') {
        debugflags &= DF_TIME;
        if(c >= '1')
          debugflags |= DF_INIT | DF_WARN | DF_MSG | DF_ERROR;
        if(c >= '2')
          debugflags |= DF_GEOM | DF_GP | DF_LOG | DF_FIELD | DF_POLY;
        if(c >= '3')
          debugflags |= DF_TURN | DF_STEAM;
        if(c >= '4')
          debugflags |= DF_GRAPH | DF_MEMORY;
        }
      else if(c == '+') {
        if(debugfile) fclose(debugfile);
        shift(); 
        println(hlog, "writing to ", argcs());
        debugfile = fopen(argcs(), "at");  
        }
      else if(c == '@') {
        if(debugfile) fclose(debugfile);
        shift(); 
        println(hlog, "writing to ", argcs());
        debugfile = fopen(argcs(), "wt");  
        }
      }
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
    showstartmenu = false;
    PHASEFROM(2); start_game(); tour::start();
    }
  else if(argis("-presentation")) {
    PHASEFROM(2); tour::texts = false;
    start_game(); tour::start();
    }
  else if(argis("-print-tour")) {
    tour::print();
    }
#endif
  else if(argis("-draw")) {
    PHASE(3); start_game(); drawscreen();
    }
  else if(argis("-rotate")) {
    PHASE(3);  start_game();
    shift(); ld a = argf();
    shift(); ld b = argf();
    View = View * spin(TAU * a / b);
    playermoved = false;
    }
  else if(argis("-rotate-up")) {
    start_game();
    shiftmatrix S = ggmatrix(cwt.at->master->move(0)->c7);
    View = spin90() * spintox(S.T*C0) * View;
    playermoved = false;
    }
  else if(argis("-rotate3")) {
    PHASE(3);  start_game();
    shift(); ld a = argf();
    shift(); ld b = argf();
    View = View * cspin(1, 2, TAU * a / b);
    playermoved = false;
    }
  else if(argis("-face-vertex")) {
    PHASE(3);  start_game();
    auto &ss = currentmap->get_cellshape(cwt.at);
    View = cspin90(0, 2) * spintox(ss.vertices_only_local[0]);
    playermoved = false;
    }
  else if(argis("-face-face")) {
    PHASE(3);  start_game();
    View = cspin90(0, 2);
    }
  else if(argis("-grotate")) {
    PHASE(3);  start_game();
    shift(); int i = argi();
    shift(); int j = argi();
    shift(); View = View * cspin(i, j, argf());
    playermoved = false;
    }
  else if(argis("-grotatei")) {
    PHASE(3);  start_game();
    shift(); int i = argi();
    shift(); int j = argi();
    shift(); rotate_view(cspin(i, j, argf()));
    playermoved = false;
    }
  else if(argis("-cview")) {
    PHASE(3);  start_game();
    View = Id;
    }
  else if(argis("-exit")) {
    PHASE(3);
    int t = SDL_GetTicks();
    if(t > 1800 * 1000)
      println(hlog, "Great Success!\n");
    else
      println(hlog, "Success.\n");
    fflush(stdout);
    exit(0);
    }

// graphical options
  else if(argis("-noscr")) {
    PHASE(3);
    popScreenAll();
    showstartmenu = false;
    }

  else if(argis("-viz")) {
    PHASE(3);
    showstartmenu = false;
    start_game();
    popScreenAll();
    clearMessages();
    nohud = true;
    mapeditor::drawplayer = false;
    }

  else if(argis("-vizhr")) {
    PHASE(3);
    showstartmenu = false;
    popScreenAll();
    clearMessages();
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
    printf("Toggles: -o0 disables, -o1 enables, -o switches\n");
    printf("Not all options are documented, see hyper.cpp\n");
    exit(0);
    }
  else return 1;
  return 0;
  }

EX purehookset hooks_config;

EX hookset<int()> hooks_args;

EX map<string, pair<int, reaction_t>> *added_commands;

EX namespace arg {

  int read_added_commands() {
    if(!added_commands) return 1;
    if(added_commands->count(args())) {
      auto& ac = (*added_commands)[args()];
      if(ac.first == 2)
        PHASEFROM(2);
      if(ac.first == 3)
        PHASE(3);
      ac.second();
      return 0;
      }
    return 1;
    }
  
  EX int add_at(const string& s, int at, const reaction_t& r) {
    if(!added_commands) added_commands = new map<string, pair<int, reaction_t>> ();
    if(added_commands->count(s)) throw hr_exception("arg::add conflict");
    (*added_commands)[s] = {at, r};
    return 1;
    }

  EX int add1(const string& s, const reaction_t& r) { return add_at(s, 1, r); }
  EX int add2(const string& s, const reaction_t& r) { return add_at(s, 2, r); }
  EX int add3(const string& s, const reaction_t& r) { return add_at(s, 3, r); }

  auto ah = addHook(hooks_args, 0, readCommon) + addHook(hooks_args, 200, read_added_commands);
  
  void read(int phase) { 
    curphase = phase;
    callhooks(hooks_config);
    while(pos < isize(argument)) {
      int r = callhandlers(1, hooks_args);
      switch (r) {
        case 0: lshift(); break;
        case 1: printf("Unknown option: %s\n", argcs()); exit(3); break;
        case 2: return;
        default: assert(false);
        }
      }
    }
EX }
#endif

#if !CAP_COMMANDLINE
EX namespace arg {
  EX int add1(const string& s, const reaction_t& r) { return 0; }
  EX int add2(const string& s, const reaction_t& r) { return 0; }
  EX int add3(const string& s, const reaction_t& r) { return 0; }
EX }
#endif
}
