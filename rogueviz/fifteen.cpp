// Hyperbolic Rogue -- fifteen+4 puzzle
// Copyright (C) 2011-2019 Zeno Rogue, see 'hyper.cpp' for details

/** \file fifteen.cpp
 *  \brief fifteen+4 puzzle
 */

#include "../hyper.h"

namespace hr {

EX namespace fifteen {

static const int Empty = 0;

struct celldata {
  int target, targetdir;
  int current, currentdir;
  };

map<cell*, celldata> fif;

eWall empty = waChasm;

bool in = false;

enum ePenMove { pmJump, pmRotate, pmAdd };
ePenMove pen;

void init_fifteen() {  
  println(hlog, "init_fifteen");
  auto ac = currentmap->allcells();
  for(int i=0; i<min(isize(ac), 20); i++) {
    fif[ac[i]] = {i, 0, i, 0};
    }  
  cwt.at = ac[0];
  println(hlog, "ok");
  }

string dotted(int i) {
  string s = its(i);
  bool confusing = true;
  for(char c: s) if(!among(c, '0', '6', '8', '9'))
    confusing = false;
  if(confusing) s += ".";
  return s;
  }

/** where = empty square */
void make_move(cell *where, int dir) {
  auto nw = where->cmove(dir);
  auto& f0 = fif[where];
  auto& f1 = fif[nw];
  f0.current = f1.current;
  f0.currentdir = gmod(f1.currentdir + dir - where->c.spin(dir) + nw->type/2, nw->type);
  f1.current = Empty;
  }

void check_move() {
  for(int i=0; i<cwt.at->type; i++) {
    cell *c1 = cwt.at->cmove(i);
    if(fif.count(c1) && fif[c1].current == Empty) {
      make_move(c1, cwt.at->c.spin(i));
      }
    }
  }

void scramble() {
  for(int i=0; i<1000; i++) {
    int d = hrand(cwt.at->type);
    cell *c1 = cwt.at->move(d);
    if(fif.count(c1)) {
      make_move(cwt.at, d);
      cwt.at = c1;
      }
    }     
  }

bool draw_fifteen(cell *c, const shiftmatrix& V) {
  if(!in) return false;
  if(!fif.count(c)) { c->land = laNone; c->wall = waChasm; return false; }
  check_move();
    
  auto& cd = fif[c];
  
  int cur = anyshiftclick ? cd.target : cd.current;
  int cdir = anyshiftclick ? cd.targetdir : cd.currentdir;
  
  if(cur == Empty) {
    c->land = laCanvas;
    c->wall = waNone;
    c->landparam = 0x101080;
    }
  else {
    c->land = laCanvas;
    c->wall = waNone;
    c->landparam = 0xFFFFFF;
    if(cdir < 0 || cdir >= c->type) {
      println(hlog, "ERROR: invalid dir ", cdir);
      cdir = 0;
      }
    write_in_space(V * ddspin(c,cdir,0), 72, 1, dotted(cur), 0xFF, 0, 8);
    }
  
  return false;
  }

void edit_fifteen() {

  if(!fif.count(cwt.at)) 
    init_fifteen();

  clearMessages();

  getcstat = '-';
  cmode = 0;
  cmode = sm::SIDE | sm::DIALOG_STRICT_X | sm::MAYDARK;

  auto ss = mapstream::save_start();
  ss->item = itGold;
  gamescreen(0);
  ss->item = itNone;
  
  dialog::init("Fifteen Puzzle", iinf[itPalace].color, 150, 100);

  dialog::addBoolItem("jump", pen == pmJump, 'j');
  dialog::add_action([] { pen = pmJump; });

  dialog::addBoolItem("rotate", pen == pmRotate, 'r');
  dialog::add_action([] { pen = pmRotate; });

  dialog::addBoolItem("add", pen == pmAdd, 'a');
  dialog::add_action([] { pen = pmAdd; });

  dialog::addBreak(100);

  dialog::addItem("this is the goal", 'g');
  dialog::add_action([] { 
    for(auto& sd: fif) sd.second.target = sd.second.current, sd.second.targetdir = sd.second.currentdir;
    });

  dialog::addItem("scramble", 's');
  dialog::add_action(scramble);
  
  dialog::addItem("save this puzzle", 'S');
  dialog::add_action([] { 
    mapstream::saveMap("fifteen.lev");
    #if ISWEB
    offer_download("fifteen.lev", "mime/type");
    #endif
    });

  dialog::addItem("settings", 'X');
  dialog::add_action_push(showSettings);

  mine_adjacency_rule = true;
  
  dialog::addItem("new geometry", 'G');
  dialog::add_action(runGeometryExperiments);

  dialog::addItem("load a puzzle", 'L');
  dialog::add_action([] { 
    #if ISWEB
    offer_choose_file([] {
      mapstream::loadMap("data.txt");
      });
    #else
    mapstream::loadMap("fifteen.lev");
    #endif
    mapeditor::drawplayer = false;
    });

  dialog::addBack();
  
  dialog::display();
  
  keyhandler = [] (int sym, int uni) {
    dialog::handleNavigation(sym, uni);
    
    if(sym == '-' && mouseover && !holdmouse) {
      cell *c = mouseover;
      
      if(pen == pmJump) {
        if(fif.count(c)) {
          auto& f0 = fif[cwt.at];
          auto& f1 = fif[c];
          swap(f0, f1);
          cwt.at = c;
          }
        else {
          fif[c] = fif[cwt.at];
          fif.erase(cwt.at);
          cwt.at = c;
          }
        }
      
      if(pen == pmRotate) {
        if(fif.count(c) == 0) return;
        auto& f1 = fif[c];
        f1.currentdir = gmod(1+f1.currentdir, c->type);        
        f1.targetdir = gmod(1+f1.targetdir, c->type);        
        }
      
      if(pen == pmAdd) {
        if(fif.count(c) == 0) {
          auto& f = fif[c];
          f.current = f.target = isize(fif)-1;
          f.currentdir = f.targetdir == 0;
          }
        else {
          auto& f = fif[c];
          if(f.current == isize(fif)-1)
            fif.erase(c);
          }
        }
      }
    };
  }

void launch() {  
  /* setup */
  stop_game();
  specialland = firstland = laCanvas;
  canvas_default_wall = waChasm;
  start_game();
  init_fifteen();

  in = true;
  
  showstartmenu = false;
  mapeditor::drawplayer = false;
  }

#if CAP_COMMANDLINE
int rugArgs() {
  using namespace arg;
           
  if(0) ;
  else if(argis("-fifteen")) {
    PHASEFROM(3);
    launch();
    #if ISWEB
    mapstream::loadMap("1");    
    #endif
    }

  else return 1;
  return 0;
  }

void o_key(o_funcs& v) {
  if(in) v.push_back(named_dialog("edit the Fifteen puzzle", edit_fifteen));
  }

auto fifteen_hook = 
  addHook(hooks_args, 100, rugArgs) +
  addHook(hooks_o_key, 80, o_key) +
  addHook(hooks_drawcell, 100, draw_fifteen) +
  addHook(mapstream::hooks_savemap, 100, [] (fhstream& f) {
    if(!in) return;
    f.write<int>(isize(fif));
    for(auto cd: fif) {
      f.write(mapstream::cellids[cd.first]);
      println(hlog, cd.first, " has id ", mapstream::cellids[cd.first]);
      f.write(cd.second.target);
      f.write(cd.second.targetdir);
      f.write(cd.second.current);
      f.write(cd.second.currentdir);
      }
    }) +
  addHook(hooks_clearmemory, 40, [] () {
    fif.clear();
    }) +
  addHook(mapstream::hooks_loadmap, 100, [] (fhstream& f) {
    int num;
    if(!in) return;
    f.read(num);
    fif.clear();
    println(hlog, "read num = ", num);
    for(int i=0; i<num; i++) {
      int32_t at = f.get<int>();
      println(hlog, "at = ", at);
      cell *c = mapstream::cellbyid[at];
      auto& cd = fif[c];      
      f.read(cd.target);
      f.read(cd.targetdir);
      cd.targetdir = mapstream::fixspin(mapstream::relspin[at], cd.targetdir, c->type, f.vernum);
      f.read(cd.current);
      f.read(cd.currentdir);
      cd.currentdir = mapstream::fixspin(mapstream::relspin[at], cd.currentdir, c->type, f.vernum);
      println(hlog, "assigned ", cd.current, " to ", c);
      }
    });
#endif

EX }

EX }

