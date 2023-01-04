// Hyperbolic Rogue -- debugging routines
// Copyright (C) 2011-2018 Zeno Rogue, see 'hyper.cpp' for details

/** \file debug.cpp
 *  \brief Debugging and cheating
 */

#include "hyper.h"
namespace hr {

EX int steplimit = 0;
EX int cstep;
EX bool buggyGeneration = false;
EX bool debug_cellnames = false;

EX vector<cell*> buggycells;

#if HDR
template<class... T>
void limitgen(T... args) {
  if(steplimit) {
    cstep++;
    printf("%6d ", cstep);
    printf(args...);
    if(cstep == steplimit) buggyGeneration = true;
    }
  }
#endif

EX cell *pathTowards(cell *pf, cell *pt) {

  while(celldist(pt) > celldist(pf)) {
    if(isNeighbor(pf, pt)) return pt;
    cell *pn = NULL;
    forCellEx(pn2, pt) if(celldist(pn2) < celldist(pt)) pn = pn2;
    pt = pn;
    }

  if(isNeighbor(pf, pt)) return pt;
  forCellEx(pn2, pt) if(celldist(pn2) < celldist(pt)) return pn2;
  return NULL;
  }

bool errorReported = false;

EX void describeCell(cell *c) {
  if(!c) { printf("NULL\n"); return; }
  print(hlog, "describe ", lalign(6, c), ": ");
  vector<cell*> nei;
  for(int i=0; i<c->type; i++) nei.push_back(c->move(i));
  println(hlog, ">> ", nei);
  }

static int orbid = 0;

eItem nextOrb() {
  orbid++;
  eItem i = eItem(orbid % ittypes);
  if(itemclass(i) == IC_ORB) return i;
  else return nextOrb();
  }

eItem randomTreasure() {
  eItem i = eItem(hrand(ittypes));
  if(itemclass(i) == IC_TREASURE) return i;
  else return randomTreasure();
  }

eItem randomTreasure2(int cv) {
  int bq = 60000, cq = 0;
  eItem best = itDiamond;
  eItem lt = localTreasureType();
  for(int a=1; a<ittypes; a++) {
    eItem i = eItem(a);
    if(itemclass(i) != IC_TREASURE) continue;
    int q = 2*items[i];
    if(a == lt) q -= (2*cv-1);
    if(a == itEmerald && bearsCamelot(cwt.at->land)) q -= 8;
    if(a == itElixir && isCrossroads(cwt.at->land)) q -= 7;
    if(a == itIvory && isCrossroads(cwt.at->land)) q -= 6;
    if(a == itPalace && isCrossroads(cwt.at->land)) q -= 5;
    if(a == itIvory && cwt.at->land == laJungle) q -= 5;
    if(a == itIvory && cwt.at->land == laPalace) q -= 5;
    if(q < bq) bq = q, cq = 0;
    if(q == bq) { cq++; if(hrand(cq) == 0) best = i; }
    }
  return best;
  }

EX eLand cheatdest;

EX void cheatMoveTo(eLand l) {
  cheatdest = l;
  if(l == laCrossroads5) l = laCrossroads;
  activateSafety(l);
  cheatdest = laNone;
  }

struct cheatkey {
  int key;
  string desc;
  reaction_t action;
  };

vector<cheatkey> cheats = {
  cheatkey{'C', "Hyperstone Quest", [] {
    cheater++; 
    cheatMoveTo(laCrossroads);
    addMessage(XLAT("Activated the Hyperstone Quest!"));

    for(int t=1; t<ittypes; t++) 
      if(t != itHyperstone && t != itBounty && itemclass(eItem(t)) == IC_TREASURE) {
        items[t] = inv::on ? 50 : 10;
        }
    int qkills = inv::on ? 1000 : 200;
    kills[moYeti] = qkills;
    kills[moDesertman] = qkills;
    kills[moRunDog] = qkills;
    kills[moZombie] = qkills;
    kills[moMonkey] = qkills;
    kills[moCultist] = qkills;
    kills[moTroll] = qkills;
    }},
  cheatkey{'M', "deplete orb powers", [] {
    for(int i=0; i<ittypes; i++) 
      if(itemclass(eItem(i)) == IC_ORB) 
        items[i] = 0;
    cheater++; addMessage(XLAT("Orb power depleted!"));
    }},
  cheatkey{'O', "summon orbs", [] {
    cheater++; addMessage(XLAT("Orbs summoned!"));
    for(int i=0; i<cwt.at->type; i++) 
      if(passable(cwt.at->move(i), NULL, 0)) {
        eItem it = nextOrb();
        cwt.at->move(i)->item = it;
        }
    }},
  cheatkey{'F', "gain orb powers", [] {
    if(hardcore && !canmove) { 
      canmove = true; 
      addMessage(XLAT("Revived!"));
      }
    else {
      items[itOrbFlash] += 1;
      items[itOrbTeleport] += 1;
      items[itOrbLightning] += 1;
      items[itOrbSpeed] += 1;
      items[itOrbShield] += 1;
      kills[moPlayer] = 0;
      cheater++; addMessage(XLAT("Orb power gained!"));
      canmove = true;
      }
    }},
  cheatkey{'R'-64, "advance the rose wave", buildRosemap},
  #if CAP_EDIT
  cheatkey{'A', "start the Map Editor", [] {
    lastexplore = turncount;
    pushScreen(mapeditor::showMapEditor);
    }},
  cheatkey{'A'-64, "start the Vector Graphics Editor", [] {
    mapeditor::drawcell = mouseover ? mouseover : cwt.at;
    pushScreen(mapeditor::showDrawEditor);
    }},
  #else
  cheatkey{'A', "take screenshot", [] {
    pushScreen(shot::menu);
    }},
  #endif
  cheatkey{'T', "summon treasure", [] {
    items[randomTreasure2(10)] += 10;
    cheater++; addMessage(XLAT("Treasure gained!"));
    }},
  cheatkey{'T'-64, "summon lots of treasure", [] {
    items[randomTreasure2(100)] += 100;
    cheater++; addMessage(XLAT("Lots of treasure gained!"));
    }},
  cheatkey{'Z', "rotate the character", [] {
    if (flipplayer) {
      cwt += cwt.at->type/2;
      flipplayer = false;
      }
    cwt++;
    mirror::act(1, mirror::SPINSINGLE);
    cwt.at->mondir++;
    cwt.at->mondir %= cwt.at->type;

    if(shmup::on) shmup::pc[0]->at = Id;
    }},
  cheatkey{'J', "lose all treasure", [] {
    if(items[localTreasureType()] > 0)
      items[localTreasureType()] = 0;
    else for(int i=1; i<ittypes; i++) 
      if(itemclass(eItem(i)) == IC_TREASURE) 
        items[i] = 0;
    cheater++; addMessage(XLAT("Treasure lost!"));
    }},
  cheatkey{'K', "gain kills", [] {
    for(int i=0; i<motypes; i++) kills[i] += 10;
    kills[moPlayer] = 0;
    cheater++; addMessage(XLAT("Kills gained!"));
    }},
  cheatkey{'Y', "unlock Orbs of Yendor", [] {
    for(auto& y: yendor::yi) {
      if(y.path[YDIST-1]->item == itKey)
        y.path[YDIST-1]->item = itNone;
      if(!y.found) items[itKey]++;
      y.found = true;
      }
    cheater++; addMessage(XLAT("Collected the keys!"));
    }},
  cheatkey{'Y'-64, "gain Orb of Yendor", [] {
    yendor::collected(cwt.at);
    cheater++;
    }},
  cheatkey{'P', "save a Princess", [] {
    items[itSavedPrincess]++;
    princess::saved = true;
    princess::everSaved = true;
    if(inv::on && !princess::reviveAt)
      princess::reviveAt = gold(NO_LOVE);
    cheater++; addMessage(XLAT("Saved the Princess!"));
    }},
  cheatkey{'S', "Safety (quick save)", [] {
    canmove = true;
    cheatMoveTo(cwt.at->land);
    items[itOrbSafety] += 3;
    cheater++; addMessage(XLAT("Activated Orb of Safety!"));
    }},
  cheatkey{'W'-64, "switch web display", [] {
    pushScreen(linepatterns::showMenu);
    }},
  cheatkey{'G'-64, "switch ghost timer", [] {
    timerghost = !timerghost;
    cheater++; 
    addMessage(XLAT("turn count = %1 last exploration = %2 ghost timer = %3",
      its(turncount), its(lastexplore), ONOFF(timerghost)));
    }},
  cheatkey{'G', "edit cell values", push_debug_screen},
  cheatkey{'L'-64, "cell info", [] {
    debug_cellnames = !debug_cellnames;
    cell *c = mouseover;
    if(!c) return;
    describeCell(c);
    }},
  cheatkey{'P'-64, "peaceful mode", [] {
    peace::on = !peace::on;
    }},
#ifdef CHEAT_DISABLE_ALLOWED
  cheatkey{'D'-64, "cheat disable", [] {
    cheater = 0; autocheat = 0;
    }
#endif
  };

EX bool applyCheat(char u) {
  for(auto& ch: cheats) if(u == ch.key) {
    ch.action();
    return true;
    }
  return false;
  }

template<class T> string dnameof2(T x) {
  string s = dnameof(x);
  return s + " (" + its(x) + ")";
  }

template<class T> string dnameof2(T x, int p) {
  string s = dnameof(x);
  return s + " (" + its(x) + "/" + its(p) + ")";
  }

EX vector<pair<cellwalker,int> > drawbugs;

bool debugmode = false;

// static apparently does not work in old compilers
int bitfield_v;

template<class T> void bitfield_editor(int val, T setter, string help = "") {
  bitfield_v = val;
  dialog::editNumber(bitfield_v, 0, 100, 1, bitfield_v, help, "");
  dialog::reaction = [setter] () { setter(bitfield_v); };
  }

struct debugScreen {

  cell *debugged_cell;
  
  bool show_debug_data;
  
  debugScreen() { debugged_cell = NULL; show_debug_data = false; }
  
  void operator () () {
    cmode = sm::SIDE | sm::DIALOG_STRICT_X;
    gamescreen();
    getcstat = '-';

    dialog::init(show_debug_data ? XLAT("debug values") : XLAT("internal details"));
    
    for(auto& p: drawbugs)
      drawBug(p.first, p.second);
    
    cell *what = debugged_cell;
    if(!what && current_display->sidescreen) what = mouseover;
    
    if(what) {
      #if CAP_SHAPES
      queuepoly(gmatrix[what], cgi.shAsymmetric, 0x80808080);
      #endif
      char buf[200];
      sprintf(buf, "%p", hr::voidp(what));
      dialog::addSelItem("mpdist", its(what->mpdist), 'd');
      dialog::add_action([what] () { 
        bitfield_editor(what->mpdist, [what] (int i) { what->mpdist = 0; }, "generation level");        
        });
      dialog::addSelItem("land", dnameof2(what->land), 0);
      dialog::addSelItem("land param (int)", its(what->landparam), 'p');
      dialog::add_action([what] () { dialog::editNumber(what->landparam, 0, 100, 1, what->landparam, "landparam",
        "Extra value that is important in some lands. The specific meaning depends on the land."); });
      dialog::addSelItem("land param (hex)", itsh8(what->landparam), 0);
      dialog::addSelItem("land param (heat)", fts(HEAT(what)), 't');
      dialog::addSelItem("cdata", 
        its(getCdata(what, 0))+"/"+its(getCdata(what,1))+"/"+its(getCdata(what,2))+"/"+its(getCdata(what,3))+"/"+itsh(getBits(what)), 't');
      dialog::add_action([what] () { 
        static ld d = HEAT(what);
        dialog::editNumber(d, -2, 2, 0.1, d, "landparam",
          "Extra value that is important in some lands. The specific meaning depends on the land."); 
        dialog::reaction = [what] () { HEAT(what) = d; };
        });
      dialog::addSelItem("land flags", its(what->landflags)+"/"+itsh2(what->landflags), 'f');
      dialog::add_action([what] () { 
        bitfield_editor(what->landflags, [what] (int i) { what->landflags = i; }, "Rarely used.");
        });
      dialog::addSelItem("barrier dir", its(what->bardir), 'b');
      dialog::add_action([what] () {
        bitfield_editor(what->bardir, [what] (int i) { what->bardir = i; });
        });
      dialog::addSelItem("barrier left", dnameof2(what->barleft), 0);
      dialog::addSelItem("barrier right", dnameof2(what->barright), 0);
      if(what->item == itBabyTortoise) {
        dialog::addSelItem(XLAT("baby Tortoise flags"), itsh(tortoise::babymap[what]), 'B');
        dialog::add_action([what] () {
          dialog::editNumber(tortoise::babymap[what], 0, (1<<21)-1, 1, getBits(what), "", "");
          dialog::use_hexeditor();
          });
        }
      if(what->monst == moTortoise) {
        dialog::addSelItem(XLAT("adult Tortoise flags"), itsh(tortoise::emap[what]), 'A');
        dialog::add_action([what] () {
          tortoise::emap[what] = tortoise::getb(what);
          dialog::editNumber(tortoise::emap[what], 0, (1<<21)-1, 1, getBits(what), "", "");
          dialog::use_hexeditor();
          });
        }
      #if CAP_COMPLEX2
      if(dice::on(what)) {
        dialog::addSelItem(XLAT("die shape"), dice::die_name(dice::data[what].which), 'A');
        dialog::add_action_push([what] {
          dialog::init("die shape");
          char key = 'a';
          for(auto shape: dice::die_list) {
            dialog::addItem(dice::die_name(shape), key++);
            dialog::add_action([what, shape] {
              dice::data[what].which = shape;
              dice::data[what].val = 0;
              popScreen();
              });
            }
          dialog::display();
          });
        dialog::addSelItem(XLAT("die face"), its(dice::data[what].val), 'B');
        dialog::add_action([what] {
          auto& dd = dice::data[what];
          int maxv = shape_faces(dd.which)-1;
          dialog::editNumber(dd.val, 0, maxv, 1, 0, XLAT("die face"), "");
          dialog::bound_low(0);
          dialog::bound_up(maxv);
          });
        dialog::addSelItem(XLAT("die direction"), its(dice::data[what].dir), 'C');
        dialog::add_action([what] {
          auto& dd = dice::data[what];
          dialog::editNumber(dd.dir, 0, what->type-1, 1, dd.dir, XLAT("die direction"), "");
          dialog::bound_low(0);
          dialog::bound_up(what->type-1);
          });
        dialog::addBoolItem_action(XLAT("die mirror status"), dice::data[what].mirrored, 'D');
        }
      #endif
      dialog::addBreak(50);
      
      if(show_debug_data) {
        dialog::addSelItem("pointer", s0+buf+"/"+index_pointer(what), 0);
        dialog::addSelItem("cpdist", its(what->cpdist), 0);
        dialog::addSelItem("celldist", its(celldist(what)), 0);
        dialog::addSelItem("celldistance", its(celldistance(cwt.at, what)), 0);
        dialog::addSelItem("pathdist", its(what->pathdist), 0);
        dialog::addSelItem("celldistAlt", eubinary ? its(celldistAlt(what)) : "--", 0);
        dialog::addSelItem("temporary", its(what->listindex), 0);
        #if CAP_GP
        if(GOLDBERG)
          dialog::addSelItem("whirl", sprint(gp::get_local_info(what).relative), 0);
        #endif
        #if CAP_RACING
        if(racing::on) racing::add_debug(what);
        #endif
        }
      else {
        dialog::addSelItem("wall", dnameof2(what->wall, what->wparam), 'w');
        dialog::add_action([what] () {
          bitfield_editor(what->wparam, [what] (int i) { what->wparam = i; },
          "wall parameter");
          });
        dialog::addSelItem("item", dnameof(what->item), 0);
        #if CAP_ARCM
        if(arcm::in())
          dialog::addSelItem("ID", its(arcm::id_of(what->master)), 0);    
        #endif
        dialog::addBreak(50);
        dialog::addSelItem("monster", dnameof2(what->monst, what->mondir), 'm');
        dialog::add_action([what] () {
          bitfield_editor(what->mondir, [what] (int i) { what->mondir = i; },
          "monster direction");
          dialog::extra_options = [what] () { 
            dialog::addBoolItem(XLAT("mirrored"), what->monmirror, 'M');
            };
          });
        dialog::addSelItem("stuntime", its(what->stuntime), 's');
        dialog::add_action([what] () {
          bitfield_editor(what->stuntime, [what] (int i) { what->stuntime = i; });
          });
        dialog::addSelItem("hitpoints", its(what->hitpoints), 'h');
        dialog::add_action([what] () {
          bitfield_editor(what->hitpoints, [what] (int i) { what->hitpoints = i; });
          });
        dialog::addBreak(50);
        dialog::addBreak(50);
        dialog::addItem("show debug data", 'x');
        dialog::add_action([this] () { show_debug_data = true; });
        if(!debugged_cell) dialog::addItem("click a cell to edit it", 0);
        }
      }
    else {
      dialog::addItem(XLAT("click a cell to view its data"), 0);
      dialog::addBreak(1000);
      }
    dialog::addBack();
    dialog::display();

    keyhandler = [this] (int sym, int uni) {
      handlePanning(sym, uni);
      dialog::handleNavigation(sym, uni);
      if(applyCheat(uni)) ;
      else if(sym == PSEUDOKEY_WHEELUP || sym == PSEUDOKEY_WHEELDOWN) ;
      else if(sym == '-') debugged_cell = mouseover;
      else if(doexiton(sym, uni)) {
        popScreen();
        if(debugmode) quitmainloop = true;
        }
      };  
    }
  };

EX void push_debug_screen() {
  debugScreen ds;
  pushScreen(ds);
  }

/** show the cheat menu */
EX void showCheatMenu() {
  cmode = sm::SIDE | sm::MAYDARK;
  gamescreen();
  dialog::init("cheat menu");
  for(auto& ch: cheats) {
    dialog::addItem(XLAT(ch.desc), ch.key);
    dialog::add_action([ch] { ch.action(); popScreen(); });
    }
  dialog::addBreak(50);
  dialog::addBack();
  dialog::display();
  }

/** view all the monsters and items */
EX void viewall() {
  celllister cl(cwt.at, 20, 2000, NULL);
  
  vector<eMonster> all_monsters;
  for(int i=0; i<motypes; i++) {
    eMonster m = eMonster(i);
    if(!isMultitile(m)) all_monsters.push_back(m);
    }
  
  for(cell *c: cl.lst) {
    if(isPlayerOn(c)) continue;
    bool can_put_monster = true;
    forCellEx(c2, c) if(c2->monst || isPlayerOn(c2)) can_put_monster = false;
    if(can_put_monster) {
      for(int k=0; k<isize(all_monsters); k++)
        if(passable_for(all_monsters[k], c, nullptr, 0)) {
          c->monst = all_monsters[k];
          all_monsters[k] = all_monsters.back();
          all_monsters.pop_back();
          }
      }
    }

  vector<cell*> itemcells;
  for(cell *c: cl.lst) {
    if(isPlayerOn(c) || c->monst || c->item) continue;
    itemcells.push_back(c);
    }
  int id = 0;
  for(int it=1; it<ittypes; it++) if(it != itBarrow) {
    if(id >= isize(itemcells)) break;
    itemcells[id++]->item = eItem(it);
    }
  }

#if CAP_COMMANDLINE
/** perform a move for the -cmove command */

int cheat_move_gen = 7;

void cheat_move(char c) {
  using arg::cheat;
  if(c >= '0' && c <= '9' && cheat_move_gen == -1) cheat_move_gen = (c - '0');
  else if(c >= '0' && c <= '9') cheat(), cwt += (c - '0');
  else if(c == 's') {
    cheat();
    cwt += wstep; 
    playermoved = false;
    setdist(cwt.at, cheat_move_gen, cwt.peek());
    if(geometry_supports_cdata()) getCdata(cwt.at, 0);
    }
  else if(c == 'r') cheat(), cwt += rev;
  else if(c == 'm') cheat(), cwt += wmirror;
  else if(c == 'z') cheat(), cwt.spin = 0, cwt.mirrored = false;
  else if(c == 'F') centering = eCentering::face, fullcenter();
  else if(c == 'E') centering = eCentering::edge, fullcenter();
  else if(c == 'V') centering = eCentering::vertex, fullcenter();
  else if(c == 'a') cheat(), history::save_end();
  else if(c == 'g') cheat_move_gen = -1;
  else println(hlog, "unknown move command: ", c);
  }
#endif

/** launch a debugging screen, and continue normal working only after this screen is closed */
EX void modalDebug(cell *c) {
  centerover = c; View = Id;
  if(noGUI) {
    fprintf(stderr, "fatal: modalDebug called on %p without GUI\n", hr::voidp(c));
    exit(1);
    }
  push_debug_screen();
  debugmode = true;
  mainloop();
  debugmode = false;
  quitmainloop = false;
  }

void test_distances(int max) {
  int ok = 0, bad = 0;
  celllister cl(cwt.at, max, 100000, NULL);
  for(cell *c: cl.lst) {
    bool is_ok = cl.getdist(c) == celldistance(c, cwt.at);
    if(is_ok) ok++; else bad++;
    }
  println(hlog, "ok=", ok, " bad=", bad);
  }

EX void raiseBuggyGeneration(cell *c, const char *s) {

  printf("procgen error (%p): %s\n", hr::voidp(c), s);
  
  if(!errorReported) {
    addMessage(string("something strange happened in: ") + s);
    errorReported = true;
    }

#ifdef BACKTRACE
  void *array[1000];
  size_t size;

  // get void*'s for all entries on the stack
  size = backtrace(array, 1000);

  // print out all the frames to stderr
  backtrace_symbols_fd(array, size, STDERR_FILENO);
#endif

  // return;
  
  if(cheater || autocheat) {
    drawbugs.emplace_back(cellwalker(c,0), 0xFF000080);
    modalDebug(c);
    drawbugs.pop_back();
    }
  else
    c->item = itBuggy;
  }

#if CAP_COMMANDLINE

int read_cheat_args() {
  using namespace arg;
  if(argis("-ch")) { cheat(); }
  else if(argis("-rch")) {    
    PHASEFROM(2); cheat(); reptilecheat = true;
    }
// cheats
  else if(argis("-g")) {
    /* debugging mode */
    if(curphase == 1) {
      /* use no score file */
      scorefile = "";
      /* set seed for reproducible results */
      if(!fixseed) {
        fixseed = true; autocheat = true;
        startseed = 1;      
        }
      }
    PHASE(2);
    /* causes problems in gdb */
    mouseaim_sensitivity = 0;
    /* do not any play sounds while debugging */
    effvolume = 0;
    musicvolume = 0;
    }
  else if(argis("-WS")) {
    PHASE(3);
    shift(); 
    activateSafety(readland(args()));
    cheat();
    }
  else if(argis("-WT")) {
    PHASE(3);
    shift(); 
    teleportToLand(readland(args()), false);
    cheat();
    }
  else if(argis("-W2")) {
    shift(); cheatdest = readland(args()); cheat();
    showstartmenu = false;
    cheatdest_list.clear();
    }
  else if(argis("-W3")) {
    shift(); cheatdest_list.push_back(readland(args())); cheat();
    showstartmenu = false;
    }
  else if(argis("-I")) {
    PHASE(3) cheat();
    shift(); eItem i = readItem(args());
    shift(); items[i] = argi(); 
    }
  else if(argis("-IP")) {
    PHASE(3) cheat();
    shift(); eItem i = readItem(args());
    shift(); int q = argi();
    placeItems(q, i);
    }
  else if(argis("-SM")) {
    PHASEFROM(2);
    shift(); vid.stereo_mode = eStereo(argi());
    }
  else if(argis("-cmove")) {
    PHASE(3); shift();
    for(char c: args()) cheat_move(c);
    }
  else if(argis("-ipd")) {
    PHASEFROM(2);
    shift_arg_formula(vid.ipd);
    }
#if CAP_INV
  else if(argis("-IU")) {
    PHASE(3) cheat();
    shift(); eItem i = readItem(args());
    shift(); inv::usedup[i] += argi();
    inv::compute();
    }
  else if(argis("-IX")) {
    PHASE(3) cheat();
    shift(); eItem i = readItem(args());
    shift(); inv::extra_orbs[i] += argi();
    inv::compute();
    }
#endif
#if CAP_COMPLEX2
  else if(argis("-ambush")) {
    // make all ambushes use the given number of dogs
    // example: hyper -W Hunt -IP Shield 1 -ambush 60
    PHASE(3) cheat();
    shift(); ambush::fixed_size = argi();
    }
#endif
  else if(argis("-testdistances")) {
    PHASE(3); shift(); test_distances(argi());
    }
  else if(argis("-M")) {
    PHASE(3) cheat(); start_game(); if(WDIM == 3) { drawthemap(); bfs(); }
    shift(); eMonster m = readMonster(args());
    shift(); int q = argi();
    printf("m = %s q = %d\n", dnameof(m).c_str(), q);
    restoreGolems(q, m, 7);
    }
  else if(argis("-MK")) {
    PHASE(3) cheat();
    shift(); eMonster m = readMonster(args());
    shift(); kills[m] += argi();
    }
  else if(argis("-killeach")) {
    PHASEFROM(2); start_game();
    shift(); int q = argi(); cheat();
    for(int m=0; m<motypes; m++)
      if(monsterclass(eMonster(m)) == 0)
        kills[m] = q;
    }
  else if(argis("-each")) {
    PHASEFROM(2); start_game();
    shift(); int q = argi(); cheat();
    for(int i=0; i<ittypes; i++)
      if(itemclass(eItem(i)) == IC_TREASURE)
        items[i] = q;
    }
  else if(argis("-each-random")) {
    PHASEFROM(2); start_game(); cheat();
    for(int i=0; i<ittypes; i++)
      if(itemclass(eItem(i)) == IC_TREASURE) {
        items[i] = 10 + hrand(21);
        if(i == itElemental) items[i] = 12;
        }
      else
        items[i] = 0;
    }
  else if(argis("-viewall")) {
    PHASE(3); start_game();
    viewall();
    }
  else if(argis("-unlock-all")) {
    cheat(); all_unlocked = true;
    }
  else if(argis("-wef")) {
    PHASEFROM(2);
    shift(); int index = argi(); 
    shift_arg_formula(whatever[index]);
    }
  else if(argis("-wei")) {    
    PHASEFROM(2);
    shift(); int index = argi();
    shift(); whateveri[index] = argi();
    }
  else if(argis("-W3")) {
    shift(); top_land = readland(args()); cheat();
    showstartmenu = false;
    }
  else if(argis("-top")) {
    PHASE(3); View = View * spin(-90._deg);
    }
  else if(argis("-idv")) {
    PHASE(3); View = Id;
    }
  else if(argis("-gencells")) {
    PHASEFROM(2); shift(); start_game();
    printf("Generating %d cells...\n", argi());
    celllister cl(cwt.at, 50, argi(), NULL);
    printf("Cells generated: %d\n", isize(cl.lst));
    for(int i=0; i<isize(cl.lst); i++)
      setdist(cl.lst[i], 7, NULL);
    }
  else if(argis("-sr")) {    
    PHASEFROM(2);
    shift(); sightrange_bonus = argi(); vid.use_smart_range = 0;
    }
  else if(argis("-srx")) {    
    PHASEFROM(2); cheat();
    shift(); sightrange_bonus = genrange_bonus = gamerange_bonus = argi(); vid.use_smart_range = 0;
    }
  else if(argis("-smart")) {
    PHASEFROM(2); cheat();
    vid.use_smart_range = 2;
    shift_arg_formula(WDIM == 3 ? vid.smart_range_detail_3 : vid.smart_range_detail);
    }
  else if(argis("-smartarea")) {
    PHASEFROM(2); cheat();
    shift(); vid.smart_area_based = argi();
    }
  else if(argis("-smartn")) {
    PHASEFROM(2);
    vid.use_smart_range = 1;
    shift_arg_formula(vid.smart_range_detail);
    }
  else if(argis("-smartlimit")) {
    PHASEFROM(2); 
    shift(); vid.cells_drawn_limit = argi();
    }
  else if(argis("-genlimit")) {
    PHASEFROM(2); 
    shift(); vid.cells_generated_limit = argi();
    }
  else if(argis("-sight3")) {
    PHASEFROM(2); 
    shift_arg_formula(sightranges[geometry]);
    }
  else if(argis("-sloppy")) {
    PHASEFROM(2); 
    vid.sloppy_3d = true;
    }
  else if(argis("-gen3")) {
    PHASEFROM(2); 
    shift_arg_formula(extra_generation_distance);
    }
  else if(argis("-quantum")) {
    cheat();
    quantum = true;
    }
  else if(argis("-lands")) {
    PHASEFROM(2);
    cheat();
    stop_game();
    shift(); land_structure = (eLandStructure) (argi());
    }
  else if(argis("-fix")) {
    PHASE(1);
    fixseed = true; autocheat = true;
    }
  else if(argis("-cellnames")) {
    cheat(); debug_cellnames = true;
    }
  else if(argis("-fixx")) {
    PHASE(1);
    fixseed = true; autocheat = true;
    shift(); startseed = argi();
    }
  else if(argis("-reseed")) {
    PHASEFROM(2);
    shift(); shrand(argi());
    }
  else if(argis("-steplimit")) {
    fixseed = true; autocheat = true;
    shift(); steplimit = argi();
    }
  else if(argis("-dgl")) {
    #if CAP_GL
    glhr::debug_gl = true;
    #endif
    }
  else if(argis("-mgen-off")) {
    PHASEFROM(3);
    cheat();
    gen_wandering = false;
    }
  else if(argis("-canvasfloor")) {
    shift(); canvasfloor = argi();
    for(int i=0; i<caflEND; i++) if(appears(mapeditor::canvasFloorName(i), args()))
      canvasfloor = i;
    }
  else if(argis("-keys")) {
    shift(); string s = args();
    bool quote = false;
    for(char c: s)
      if(quote) {
        quote = false;
        if(c == '\\') dialog::queue_key(c), quote = false;
        else if(c >= '1' && c <= '9') dialog::queue_key(SDLK_F1 + c - '1');
        else if(c == 'e') dialog::queue_key(SDLK_ESCAPE);
        else if(c == 'r') dialog::queue_key(SDLK_RETURN);
        else if(c == 't') dialog::queue_key(SDLK_TAB);
        else if(c == 'b') dialog::queue_key(SDLK_BACKSPACE);
        else if(c == 'R') dialog::queue_key(SDLK_RIGHT);
        else if(c == 'L') dialog::queue_key(SDLK_LEFT);
        else if(c == 'U') dialog::queue_key(SDLK_UP);
        else if(c == 'D') dialog::queue_key(SDLK_DOWN);
        else if(c == 'H') dialog::queue_key(SDLK_HOME);
        else if(c == 'E') dialog::queue_key(SDLK_END);
        else if(c == 'P') dialog::queue_key(SDLK_PAGEUP);
        else if(c == 'Q') dialog::queue_key(SDLK_PAGEDOWN);
        }
      else if(c == '\\') quote = true;
      else dialog::queue_key(c);
    }
  else if(argis("-hroll")) {
    shift();
    int i = argi();
    while(i>0) i--, hrand(10);
    }
  else if(argis("-W")) {
    PHASEFROM(2);
    shift(); 
    firstland0 = firstland = specialland = readland(args());
    if (!landUnlocked(firstland))
      cheat();
    stop_game_and_switch_mode(rg::nothing);
    showstartmenu = false;
    }
  else return 1;
  return 0;
  }

auto ah_cheat = addHook(hooks_args, 0, read_cheat_args);
#endif

EX bool ldebug = false;

EX void breakhere() {
  exit(1);
  }

}
