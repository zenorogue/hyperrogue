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

EX bool delayed_start;

#if CAP_COMMANDLINE
EX string scorefile = "hyperrogue.log";

EX namespace arg {

template<class T> T read_index(T fallback, int max, const string& ss) {
  if(ss[0] < '0' && ss[0] > '9') return fallback;
  int val = atoi(ss.c_str());
  if(val < 0 || val >= max) return fallback;
  return T(val);
  }

EX eLand readland(const string& ss) {
  if(ss == "II") return laCrossroads2;
  if(ss == "III") return laCrossroads3;
  if(ss == "IV") return laCrossroads4;
  if(ss == "V") return laCrossroads5;
  for(int l=0; l<landtypes; l++) if(appears(linf[l].name, ss)) {
    return eLand(l);
    break;
    }
  return read_index(laNone, landtypes, ss);
  }

EX eItem readItem(const string& ss) {
  for(int i=0; i<ittypes; i++) if(appears(iinf[i].name, ss)) {
    return eItem(i);
    break;
    }
  return read_index(itNone, ittypes, ss);
  }

EX eMonster readMonster(const string& ss) {
  if(ss == "Pike") return moPike;
  for(int i=0; i<motypes; i++) if(appears(minf[i].name, ss)) {
    return eMonster(i);
    break;
    }
  return read_index(moNone, motypes, ss);
  }
EX }

EX void initializeCLI() {
  printf("HyperRogue by Zeno Rogue <zeno@attnam.com>, version " VER "\n");

#if !NOLICENSE
  printf("released under GNU General Public License version 2 and thus\n");
  printf("comes with absolutely no warranty; see COPYING for details\n");
#endif

  #ifdef FHS
  auto try_place = [&] (string env, string suffix, string& which_file, bool only_file) {
    char *res = getenv(env.c_str());
    if(!res) return false;
    string buf = res; buf += suffix; buf += "/hyperrogue";
    if(only_file && file_exists(buf + "/" + which_file)) {
      which_file = buf + "/" + which_file;
      return true;
      }
    if(!file_exists(buf)) system(("mkdir -p " +buf).c_str());
    which_file = buf + "/" + which_file;
    return true;
    };

  auto try_dot = [&] (string& which_file) {
    char *res = getenv("HOME");
    string buf = res;
    buf += "/."; buf += which_file;
    if(!file_exists(buf)) return false;
    which_file = buf;
    return true;
    };

  if(try_dot(scorefile)) {}
  else if(try_place("XDG_DATA_HOME", "", scorefile, true)) {}
  else if(try_place("HOME", "/.local/share", scorefile, false)) {}
  else if(try_place("XDG_DATA_HOME", "", scorefile, false)) {}

  if(try_dot(conffile)) {}
  else if(try_place("XDG_CONFIG_HOME", "", conffile, true)) {}
  else if(try_place("HOME", "/.config", conffile, true)) {}
  else if(try_place("XDG_CONFIG_HOME", "", conffile, false)) {}
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
    return parsecolor(args(), bits == 32);
    }
  
  int parameter_id;

  EX void shift_arg_formula(ld& x, const reaction_t& r IS(reaction_t())) {
    shift();
    auto par = anims::find_param(&x);
    if(!par) par = param_f(x, "tmp_parameter_" + its(parameter_id++))->set_reaction(r);
    par->load_as_animation(args());
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
#if CAP_SDLTTF
  else if(argis("-font")) { PHASE(1); shift(); font_id = isize(font_filenames); font_filenames.push_back(args()); font_names.push_back({args(), "commandline"}); }
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
  else if(argis("-run")) {
    PHASE(3); 
    start_game();
    mainloop(); quitmainloop = false;
    }
  else if(argis("-msg")) {
    shift(); addMessage(args());
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
  else if(argis("-sview")) {
    PHASE(3);  start_game();
    playermoved = false;
    transmatrix T = View;
    shift(); View = parsematrix(args());
    println(hlog, "View is set to ", View);
    current_display->which_copy = View * inverse(T) * current_display->which_copy;
    }
  else if(argis("-rotate-up")) {
    start_game();
    shiftmatrix S = ggmatrix(cwt.at->master->move(0)->c7);
    View = spin90() * spintox(S.T*C0) * View;
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
  else if(argis("-center-vertex")) {
    PHASE(3); shift(); int i = argi();
    shift(); int j = argi();
    shift(); int k = argi();
    start_game();
    auto fh = currentmap->get_cellshape(cwt.at).faces[j][k];
    hyperpoint h = View * fh;
    if(i == 0) {
      shift_view_to(shiftless(h));
      playermoved = false;
      }
    if(i == 1) {
      rotate_view(spintox(h));
      rotate_view(cspin90(0, 2));
      }
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
    no_find_player = true;
    }

  else if(argis("-vizs")) {
    PHASE(3);
    showstartmenu = false;
    start_game();
    popScreenAll();
    clearMessages();
    nohud = true;
    mapeditor::drawplayer = false;
    no_find_player = true;
    game_keys_scroll = true;
    smooth_scrolling = true;
    touchmode = tmode::info;
    }

  else if(argis("-vizhr")) {
    PHASE(3);
    showstartmenu = false;
    popScreenAll();
    clearMessages();
    }

  else if(argis("-save-mode")) {
    PHASEFROM(2);
    save_mode_to_file(shift_args());
    }

  else if(argis("-load-mode")) {
    PHASEFROM(2);
    try {
      load_mode_from_file(shift_args());
      }
    catch(hstream_exception& e) {
      println(hlog, "exception!");
      }
    }

// informational
  else if(argis("-version") || argis("-v")) {
    printf("HyperRogue version " VER "\n");
    exit(0);
    }
  else if(argis("-L")) {
    printf("+ Treasures:\n");
    int qty = 0;
    for(int i=1; i<ittypes; i++) 
      if(itemclass(eItem(i)) == IC_TREASURE)
        printf("    %s\n", iinf[i].name), qty++;
    printf("    total = %d\n", qty); qty = 0;
    printf("\n");
    printf("+ Orbs:\n");
    for(int i=1; i<ittypes; i++) 
      if(itemclass(eItem(i)) == IC_ORB)
        printf("    %s\n", iinf[i].name), qty++;
    printf("    total = %d\n", qty); qty = 0;
    printf("\n");
    printf("+ Other items:\n");
    for(int i=1; i<ittypes; i++) 
      if(itemclass(eItem(i)) == IC_OTHER)
        printf("    %s\n", iinf[i].name), qty++;
    printf("    total = %d\n", qty); qty = 0;
    printf("\n");
    printf("+ Monsters:\n");
    for(int i=1; i<motypes; i++) 
      printf("    %s\n", minf[i].name), qty++;
    printf("    total = %d\n", qty); qty = 0;
    printf("\n");
    printf("+ Lands:\n");
    for(int i=1; i<landtypes; i++)
      printf("    %s\n", linf[i].name), qty++;
    printf("    total = %d\n", qty); qty = 0;
    printf("\n");
    printf("+ Walls:\n");
    for(int i=0; i<walltypes; i++) 
      printf("    %s\n", winf[i].name), qty++;
    printf("    total = %d\n", qty); qty = 0;
    printf("\n");
    exit(0);
    }
  else if(argis("")) {}
  else if(argis("-help") || argis("-h")) {
    printf("\nCommandline help follows. If you meant in-game help, press F1 while playing.\n\n");
    printf("Some commadline options automatically enable the cheat mode.\n");
    printf("ID is a part of the name of the chosen option; indices are also allowed.\n");
    printf("You can change various configuration settings using the [name]=[value] format, e.g. , `unlock_all=1`.\n");
    printf("The names are as in the config file, and formulae are allowed.\n");
    printf("HyperRogue has lots of options; only the basic ones are documented here.\n");

    printf("\nBasic options:\n");
    printf("  --version, -v      - show the version number\n");
    printf("  --help, -h         - show the commandline options\n");
    printf("  -offline           - do not connect to Steam (for Steam versions)\n");
    printf("  -L                 - list of features\n");
    printf("  -fix               - fix the RNG seed (cheat)\n");
    printf("  -fixx SEED         - fix the RNG seed to the given value (cheat)\n");
    printf("  -noscr             - skip the start menu\n");
    printf("  -viz               - visualization mode: do not display the game elements\n");
    printf("  -vizs              - visualization mode with smooth control\n");
    printf("  -g                 - debug mode: do not use scorefile, fix seed, enable cheat mode\n");
    printf("  -nogui             - do not start the GUI\n");
    printf("  -run               - run (before handling other options) until the user presses F10\n");
    printf("  -exit              - exit the program\n");

    printf("\nFile options:\n");
    printf("  -c FILE            - use the specified configuration file\n");
    printf("  -s FILE            - use the specified highscore file\n");
    printf("  -m FILE            - use the specified soundtrack (music)\n");
    printf("  -se DIR            - the directory containing sound effects\n");
    printf("  -rsrc DIR          - the directory containing resources\n");
    printf("  -font FILE         - use the specified font file\n");

    printf("\nLogging:\n");
    printf("  -log PATTERN       - log events matching the pattern\n");
    printf("  -no-log PATTERN    - do not log events matching the pattern\n");
    printf("  -log-to FILE       - log to the given file\n");
    printf("  -log-append FILE   - append log to the given file\n");

    printf("\nQuick testing:\n");
    printf("  -load FILE         - load a level file created using the map editor\n");
    printf("  -geo ID            - choose the geometry\n");
    printf("  -PM ID             - choose the projection/model\n");
    printf("  -W ID              - start in the given land\n");
    printf("  -W2 ID             - make the given land easy to find (cheat)\n");
    printf("  -W3 ID             - like -W2 but can be used multiple times (cheat)\n");
    printf("  -I ID n            - start with n of item ID nearby (cheat)\n");
    printf("  -M ID n            - start with n of monster ID nearby (cheat)\n");
    printf("  -MK ID n           - start with n of kills of monster ID (cheat)\n");
    printf("  -canvas ID         - set background color or pattern code for the canvas\n");

    printf("\nTroubleshooting suggestions:\n");
    printf("  -c test1 -s test2              - to check if config/score files are responsible\n");
    printf("  -noshaders -o0 -wm 3 -mm 2     - to disable complex graphics (-o0 disables OpenGL, -wm and -mm disable 3D walls/monsters)\n");
    printf("  -log-all -log-to error.txt     - to log everything to a file error.txt\n");
    printf("  -offline                       - to troubleshoot online connection issues\n");

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
    dynamicval<bool> ds(delayed_start, true);
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
