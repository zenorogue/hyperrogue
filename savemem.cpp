// Hyperbolic Rogue -- memory saving
// Copyright (C) 2011-2018 Zeno Rogue, see 'hyper.cpp' for details

/** \file memory.cpp
 *  \brief memory saving: memory saving mode, memory warnings, etc.
 */

#include "hyper.h"
namespace hr {

#if HDR
static const int PSEUDOKEY_MEMORY = 16397;
#endif

EX bool memory_saving_mode = true;

EX bool show_memory_warning = true;
EX bool ignored_memory_warning;

static const int LIM = 150;

EX heptagon *last_cleared;

EX void destroycellcontents(cell *c) {
  c->land = laMemory;
  c->wall = waChasm;
  c->item = itNone;
  if(!isMultitile(c->monst) && c->monst != moPair)
    c->monst = moNone;
  }

void degrade(cell *c) {
  c->mpdist++;
  forCellEx(c2, c)
    if(c2->mpdist < c->mpdist - 1)
      degrade(c2);
  destroycellcontents(c);
  }

EX vector<cell*> removed_cells;  

void slow_delete_cell(cell *c) {
  while(c->mpdist < BARLEV)
    degrade(c);
  for(int i=0; i<c->type; i++)
    if(c->move(i))
      c->move(i)->move(c->c.spin(i)) = NULL;
  removed_cells.push_back(c);
  destroy_cell(c);
  }

void delete_heptagon(heptagon *h2) {
  cell *c = h2->c7;
  if(BITRUNCATED) {
    for(int i=0; i<c->type; i++)
      if(c->move(i))
        slow_delete_cell(c->move(i));
    }
  slow_delete_cell(c);
  for(int i=0; i<S7; i++)
    if(h2->move(i))
      h2->move(i)->move(h2->c.spin(i)) = NULL;
  tailored_delete(h2);
  }

void recursive_delete(heptagon *h, int i) {
  heptagon *h2 = h->move(i);
  { for(int i=1; i<S7; i++)
    if(h2->move(i) && h2->move(i)->move(0) == h2)
      recursive_delete(h2, i); }
  if(h2->alt && h2->alt->alt == h2->alt) {
    DEBB(DF_MEMORY, ("destroying alternate map ", h2->alt));
    for(hrmap *& hm: allmaps) {
      if(hm->getOrigin() == h2->alt) {
        delete hm;
        hm = allmaps.back();
        allmaps.pop_back();
        DEBB(DF_MEMORY, ("map found (", isize(allmaps), " altmaps total)"));
        break;
        }
      }
    }
  if(h2->alt) {
    h2->alt->cdata = NULL;
    }
  delete_heptagon(h2);
  h->move(i) = NULL;  
  }

bool unsafeLand(cell *c) {
  return
    isCyclic(c->land) || isGravityLand(c->land) || isHaunted(c->land) ||
    among(c->land, laCaribbean, laOcean, laGraveyard, laPrincessQuest);
  }

EX void save_memory() {
  if(quotient || !hyperbolic || NONSTDVAR) return;
  if(!memory_saving_mode) return;
  if(unsafeLand(cwt.at)) return;
  int d = celldist(cwt.at);
  if(d < LIM+10) return;

  heptagon *at = cwt.at->master;
  heptagon *orig = currentmap->gamestart()->master;
  
  if(recallCell.at) {
    if(unsafeLand(recallCell.at)) return;
    heptagon *at2 = recallCell.at->master;
    int t = 0;
    while(at != at2) {
      t++; if(t > 10000) return;
      if(celldist(at->c7) > celldist(at2->c7))
        at = at->move(0);
      else
        at2 = at2->move(0);
      }
    }
  
  while(celldist(at->c7) > d-LIM) at = at->move(0);
  
  // go back to such a point X that all the heptagons adjacent to the current 'at'
  // are the children of X. This X becomes the new 'at'
  if(true) {
    heptagon *allh[9];
    int hcount = 0;
    allh[hcount++] = at;
    for(int j=0; j<S7; j++) 
      if(allh[0]->move(j))
        allh[hcount++] = at->move(j);
      
    int deuniq_steps = 0;
    
    int i = 1;
    while(i < hcount) {
      if(allh[i] == allh[0])
        allh[i] = allh[hcount-1], hcount--;
      else if(celldist(allh[i]->c7) > celldist(allh[0]->c7))
        allh[i] = allh[i]->move(0);
      else {
        if(allh[0] == orig) return;
        allh[0] = allh[0]->move(0);
        i = 1;
        deuniq_steps++;
        if(deuniq_steps == 10) return;
        }
      }
    
    at = allh[0];
    }
  
  if(last_cleared && celldist(at->c7) < celldist(last_cleared->c7))
    return;

  DEBB(DF_MEMORY, ("celldist = ", make_pair(celldist(cwt.at), celldist(at->c7))));
  
  heptagon *at1 = at;
  while(at != last_cleared && at != orig) {
    heptagon *atn = at;
    at = at->move(0);
    
    for(int i=1; i<S7; i++)
      if(at->move(i) && at->move(i) != atn)
        recursive_delete(at, i);
    }
  
  last_cleared = at1;
  DEBB(DF_MEMORY, ("current cellcount = ", cellcount));
  
  sort(removed_cells.begin(), removed_cells.end());
  callhooks(hooks_removecells);
  removed_cells.clear();
  }

EX purehookset hooks_removecells;

EX bool is_cell_removed(cell *c) {
  return binary_search(removed_cells.begin(), removed_cells.end(), c);
  }

EX void set_if_removed(cell*& c, cell *val) {
  if(is_cell_removed(c)) c = val;
  }

typedef array<char, 1048576> reserve_block;

EX int reserve_count = 0;
EX int reserve_limit = 128;

const int max_reserve = 4096;
array<reserve_block*, max_reserve> reserve;

std::new_handler default_handler;

EX purehookset hooks_clear_cache;

void reserve_handler() {
  if(reserve_count) {
    reserve_count--;
    delete reserve[reserve_count];
    }
  if(reserve_count < 32) callhooks(hooks_clear_cache);
  if(!reserve_count) std::set_new_handler(default_handler);
  }

EX void apply_memory_reserve() {
#if CAP_MEMORY_RESERVE
  if(reserve_count > 0) std::set_new_handler(default_handler);
  if(reserve_limit > max_reserve) reserve_limit = max_reserve;
  if(reserve_limit < 0) reserve_limit = 0;
  while(reserve_count > reserve_limit) { reserve_count--; delete reserve[reserve_count]; }
  try {
    while(reserve_count < reserve_limit) { 
      reserve[reserve_count] = new reserve_block;
      /* only if successful */
      reserve_count++;
      }
    }
  catch(std::bad_alloc&) {}
  #if ISWINDOWS
  // no get_new_handler on this compiler...
  default_handler = [] { throw std::bad_alloc(); };
  #else
  default_handler = std::get_new_handler();
  #endif
  if(reserve_count > 0) std::set_new_handler(reserve_handler);
#endif
  }

EX void memory_for_lib() {
  if(reserve_count) { reserve_count--; delete reserve[reserve_count]; }
  }

EX void show_memory_menu() {
  gamescreen();
  dialog::init(XLAT("memory"));

  dialog::addHelp(XLAT(
    "HyperRogue takes place in a world that is larger than anything Euclidean. "
    "Unfortunately, in some cases running it on an Euclidean computer might be "
    "a problem -- the computer could simply run out of memory. Some lands (such as the Ocean or the Brown Islands) "
    "may use up memory very fast!\n\n"
    ));

  if(sizeof(void*) <= 4)
  dialog::addHelp(XLAT(
    "You are playing a 32-bit HyperRogue executable, which can only use 4GB of memory.\n\n"));
    
  dialog::addHelp(XLAT(
    "Although you are extremely unlikely to return to a place you have already been to, "
    "the game never forgets these areas, unless you start a new game, use an Orb of "
    "Safety (found in Land of Eternal Motion, the Prairie, and the Ocean), or activate the memory "
    "saving mode, which tries to intelligently predict which cells you will never find "
    "again and can be safely forgotten.\n\n")
    );
  
  if(cheater) dialog::addSelItem(XLAT("cells in memory"), its(cellcount) + "+" + its(heptacount), 0);
  
  dialog::addBoolItem(XLAT("memory saving mode"), memory_saving_mode, 'f');
  dialog::add_action([] { memory_saving_mode = !memory_saving_mode; if(memory_saving_mode) save_memory(), apply_memory_reserve(); });

  dialog::addBoolItem_action(XLAT("show memory warnings"), show_memory_warning, 'w');
  
#if CAP_MEMORY_RESERVE
  if(reserve_limit > 0 && reserve_count < reserve_limit) {
    dialog::addItem(XLAT("just let me find Orb of Safety or finish the game"), 'l');
    dialog::add_action([] { ignored_memory_warning = true; popScreen(); });
    }

  dialog::addSelItem("memory reserve", its(reserve_count) + "/" + its(reserve_limit) + " MB", 'r');
  dialog::add_action([] {
    dialog::editNumber(reserve_limit, 0, max_reserve, 16, 128, XLAT("memory reserve"), 
      XLAT("When to show a memory warning.")
      );
    dialog::bound_low(0);
    dialog::bound_up(max_reserve);
    dialog::reaction = apply_memory_reserve;
    });
#endif

  dialog::addItem(XLAT("clear caches"), 'c');
  dialog::add_action([] { callhooks(hooks_clear_cache); });

  dialog::addBack();
  dialog::display();
  }

EX bool protect_memory() {
#if CAP_MEMORY_RESERVE
  apply_memory_reserve();
  if(reserve_limit && reserve_count < reserve_limit && !ignored_memory_warning) {
    pushScreen(show_memory_menu);
    return true;
    }
  if(reserve_limit && reserve_count < 8) {
    pushScreen(show_memory_menu);
    return true;
    }
#endif
  return false;
  }

EX bool memory_issues() {
  return reserve_limit && reserve_count < 16;
  }

}
