// Hyperbolic Rogue -- smart memory cleaner
// Copyright (C) 2011-2018 Zeno Rogue, see 'hyper.cpp' for details

namespace hr {

bool memory_saving_mode = true;

static const int LIM = 150;

heptagon *last_cleared;

void destroycellcontents(cell *c) {
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

vector<cell*> removed_cells;
  

void slow_delete_cell(cell *c) {
  while(c->mpdist < BARLEV)
    degrade(c);
  for(int i=0; i<c->type; i++)
    if(c->mov[i])
      c->mov[i]->mov[c->spn(i)] = NULL;
  removed_cells.push_back(c);
  delete c;
  }

void delete_heptagon(heptagon *h2) {
  cell *c = h2->c7;
  if(!nonbitrunc) {
    for(int i=0; i<c->type; i++)
      if(c->mov[i])
        slow_delete_cell(c->mov[i]);
    }
  slow_delete_cell(c);
  for(int i=0; i<S7; i++)
    if(h2->move[i])
      h2->move[i]->move[h2->spin(i)] = NULL;
  delete h2;
  }

void recursive_delete(heptagon *h, int i) {
  heptagon *h2 = h->move[i];
  { for(int i=1; i<S7; i++)
    if(h2->move[i] && h2->move[i]->move[0] == h2)
      recursive_delete(h2, i); }
  if(h2->alt && h2->alt->alt == h2->alt) {
    DEBSM(printf("destroying alternate map %p\n", h2->alt);)
    for(hrmap *& hm: allmaps) {
      auto hm2 = dynamic_cast<hrmap_alternate*> (hm);
      if(hm2 && hm2->origin == h2->alt) {
        delete hm;
        hm = allmaps.back();
        allmaps.pop_back();
        DEBSM(printf("map found (%d altmaps total)\n", size(allmaps));)
        break;
        }
      }
    }
  if(h2->alt) {
    h2->alt->cdata = NULL;
    }
  delete_heptagon(h2);
  h->move[i] = NULL;  
  }

bool unsafeLand(cell *c) {
  return
    isCyclic(c->land) || isGravityLand(c->land) || isHaunted(c->land) ||
    among(c->land, laCaribbean, laOcean, laGraveyard, laPrincessQuest);
  }

void save_memory() {
  if(quotient || !hyperbolic) return;
  if(!memory_saving_mode) return;
  if(unsafeLand(cwt.c)) return;
  int d = celldist(cwt.c);
  if(d < LIM+10) return;

  heptagon *at = cwt.c->master;
  heptagon *orig = currentmap->gamestart()->master;
  
  if(recallCell) {
    if(unsafeLand(recallCell)) return;
    heptagon *at2 = recallCell->master;
    int t = 0;
    while(at != at2) {
      t++; if(t > 10000) return;
      if(celldist(at->c7) > celldist(at2->c7))
        at = at->move[0];
      else
        at2 = at2->move[0];
      }
    }
  
  while(celldist(at->c7) > d-LIM) at = at->move[0];
  
  // go back to such a point X that all the heptagons adjacent to the current 'at'
  // are the children of X. This X becomes the new 'at'
  if(true) {
    heptagon *allh[9];
    int hcount = 0;
    allh[hcount++] = at;
    for(int j=0; j<S7; j++) 
      if(allh[0]->move[j])
        allh[hcount++] = at->move[j];
      
    int deuniq_steps = 0;
    
    int i = 1;
    while(i < hcount) {
      if(allh[i] == allh[0])
        allh[i] = allh[hcount-1], hcount--;
      else if(celldist(allh[i]->c7) > celldist(allh[0]->c7))
        allh[i] = allh[i]->move[0];
      else {
        if(allh[0] == orig) return;
        allh[0] = allh[0]->move[0];
        i = 1;
        deuniq_steps++;
        if(deuniq_steps == 10) return;
        }
      }
    
    at = allh[0];
    }
  
  if(last_cleared && celldist(at->c7) < celldist(last_cleared->c7))
    return;

  DEBSM(printf("celldist = %d, %d\n", celldist(cwt.c), celldist(at->c7));)
  
  heptagon *at1 = at;
  while(at != last_cleared && at != orig) {
    heptagon *atn = at;
    at = at->move[0];
    
    for(int i=1; i<S7; i++)
      if(at->move[i] && at->move[i] != atn)
        recursive_delete(at, i);
    }
  
  last_cleared = at1;
  DEBSM(printf("current cellcount = %d\n", cellcount);)
  
  sort(removed_cells.begin(), removed_cells.end());
  callhooks(hooks_removecells);
  removed_cells.clear();
  }

purehookset hooks_removecells;

bool is_cell_removed(cell *c) {
  return binary_search(removed_cells.begin(), removed_cells.end(), c);
  }

void set_if_removed(cell*& c, cell *val) {
  if(is_cell_removed(c)) c = val;
  }

}
