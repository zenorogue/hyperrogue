// Hyperbolic Rogue -- sumotron puzzle
// Copyright (C) 2011-2019 Zeno Rogue, see 'hyper.cpp' for details

/** \file sumotron.cpp
 *  \brief dual geometry puzzle generator
 */

#include "../hyper.h"

namespace hr {

EX namespace sumotron {

map<cell*, bignum> value;
vector<cell*> seq;

bool dialog_shown = false;

void sumotron_puzzle() {

  clearMessages();

  if(currentmap->gamestart()->wall == waSea) {
    for(cell *c: currentmap->allcells()) {
      c->wall = waNone;
      c->landparam = 0xD0D0D0;
      }
    value.clear();
    seq.clear();
    }
  
  for(cell *c: currentmap->allcells()) {
    bool mo = c == mouseover;
    bool has = value.count(c);
    if(mo && has && c == seq.back())
      c->landparam = 0xC0FF80;
    else if(mo && has)
      c->landparam = 0xC0C080;
    else if(mo)
      c->landparam = 0xFFFF00;
    else if(has && c == seq.back())
      c->landparam = 0x80FF80;
    else if(has)
      c->landparam = 0xA0A0A0;
    else
      c->landparam = 0xF0F0F0;
    }
  getcstat = '-';
  cmode = 0;
  if(dialog_shown) cmode = sm::SIDE | sm::DIALOG_STRICT_X | sm::MAYDARK;
  dynamicval<bool> dp(mapeditor::drawplayer, false);
  gamescreen();

  initquickqueue();
  for(auto p: value) {
    cell *c = p.first;
    bignum& b = value[p.first];
    string s = short_form(b);
    ld size = min<ld>(1, 1.5 / isize(s));
    for(const shiftmatrix& V: current_display->all_drawn_copies[c])
      queuestr(V, size, s, dialog_shown ? gradient(backcolor, 0x800000, 0, .5, 1) : 0xFF0000, 1);
    }

  if(mouseover) queuecircleat(mouseover, .8, 0x800000FF);

  quickqueue();
  
  dialog::init("SUMOTRON", iinf[itPalace].color, 150, 100);
  dialog::addInfo("based on Sumotron by Marcos Donnantuoni");
  
  bignum max = 0;
  for(auto v: value) if(v.second > max) max = v.second;

  if(max > 0) {
    dialog::addInfo("your result:");
    dialog::addInfo(max.get_str(9999));
    }
  else {
    dialog::addInfo("click to place number");
    dialog::addInfo("arrows keys or middle button to scroll");
    }
  
  dialog::addItem("undo", 'z');
  dialog::add_action([] {
    if(seq.empty()) return;
    value.erase(seq.back());
    seq.pop_back();
    });
  dialog::addItem("restart", 'r');
  dialog::add_action([] {
    value.clear();
    seq.clear();
    });
  dialog::addItem("change geometry", 'g');
  dialog::add_action(runGeometryExperiments);
  dialog::addItem("change settings", 's');
  dialog::add_action_push(showSettings);
  dialog::addItem("hide menu", 'v');
  dialog::add_action([] { dialog_shown = false; });
  
  if(dialog_shown) {
    dialog::display();
    }
  else {
    displayButton(8, vid.yres-vid.fsize, max.get_str(9999), 'v', 0);
    displayButton(vid.xres-8, vid.yres-vid.fsize, XLAT("(v) menu"), 'v', 16);
    dialog::add_key_action('v', [] { dialog_shown = true; });
    }
  
  keyhandler = [] (int sym, int uni) {
    handlePanning(sym, uni);
    dialog::handleNavigation(sym, uni);
    if(sym == '-' && mouseover) {
      cell *c = mouseover;
      if(value.count(c)) return;
      bignum total = 0;
      for(cell *c1: adj_minefield_cells(c))
        if(value.count(c1)) 
          total = total + value[c1];
      if(total.approx_int() == 0) total = 1;
      value[c] = total;
      seq.push_back(c);
      }
    };
  }

void launch() {  
  /* setup */
  stop_game();
  specialland = firstland = laCanvas;
  canvas_default_wall = waSea;
  start_game();
  pushScreen(sumotron_puzzle);
  }

#if CAP_COMMANDLINE
int rugArgs() {
  using namespace arg;
           
  if(0) ;
  else if(argis("-sumotron")) {
    launch();
    }

  else return 1;
  return 0;
  }

auto rug_hook = 
  addHook(hooks_args, 100, rugArgs);
#endif

EX }

EX }

