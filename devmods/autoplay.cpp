// This addon plays HyperRogue by itself.
// Well, not really -- it performs illegal moves, and it gets tons of treasure, orbs, and kills out of nowhere.
// Useful for debugging.

#include "../hyper.h"

namespace hr {

bool doAutoplay;

namespace prairie { extern long long enter; }

bool sameland(eLand ll, eLand ln) {
  if(ln == laBarrier || ln == laOceanWall)
    return true;
  if(ln == ll) return true;
  if(isElemental(ln) && isElemental(ll)) return true;
  if(isHaunted(ln) && isHaunted(ll)) return true;
  return false;
  }

/* auto a3 = addHook(hooks_nextland, 100, [] (eLand l) { 
  eLand l2;
  do { l2 = pick(laRuins, laTerracotta, laPrairie); } while(l2 == l);
  return l2;
  }); */

void autoplay(int num_moves = 1000000000) {
  // drawMesh();
  // exit(0);

  doAutoplay = true;

  cheater = 1;
  eLand lland = laIce;
  eLand lland2 = laIce;
  int lcount = 0;
  int gcount = 0;
  int lastgold = 0;
  
  // landlist = { laRuins, laTerracotta, laPrairie };

  generateLandList(isLandIngame);

#ifndef NOSDL
  int lastdraw = 0;
#endif
  while(doAutoplay) {
  
    long long ienter = (long long) prairie::enter;
    if(ienter && ienter < 100000) {
      printf("ERROR: enter has incorrect value\n");
      exit(1);
      }
  
    if(gold() > lastgold) { 
      lastgold = gold();
      gcount = 0;
      }
    else gcount++;
    
    if(false && sameland(lland, cwt.at->land)) lcount++;
    else {
      lcount = 0; lland2 = lland; lland = cwt.at->land;
      printf("%10dcc %5dt %5de %5d$ %5dK %5dgc %-30s H%d\n", cellcount, turncount, celldist(cwt.at), gold(), tkills(), gcount, dnameof(cwt.at->land), hrand(1000000));
      fflush(stdout);
#ifndef NOSDL
      if(int(SDL_GetTicks()) > lastdraw + 3000) {
        lastdraw = SDL_GetTicks();
        fullcenter();
        msgs.clear();
        popScreenAll();
        drawscreen();
        clearAnimations();
        }
#endif
      //mainloop();
      }
    int i = hrand(cwt.at->type);
    cell *c2 = cwt.at->move(i);
    /* if(gcount < 500) for(int i=1; i<isize(dcal); i++) {
      c2 = dcal[i];
      if(lcount >= 50 && !sameland(lland, c2->land) && !sameland(lland2, c2->land)) break;
      else if(lcount < 50 && c2->item && c2->item != itOrbSafety) break;
      } */
    items[hrand(ittypes)] = 1 << hrand(11);
    kills[hrand(motypes)] = 1 << hrand(11);
    items[itOrbYendor] &= 15;
    again:
    if(c2->cpdist > 1) {
      for(int j=0; j<c2->type; j++)
        if(c2->move(j) && c2->move(j)->cpdist < c2->cpdist) {
          c2 = c2->move(j);
          goto again;
          }
      }
    killMonster(c2, moNone);
    jumpTo(c2, itNone);
    if(false) if(turncount % 5000 == 0) {
      printf("cells travelled: %d\n", celldist(cwt.at));
      
      printf("\n");
    
      for(int i=0; i<ittypes; i++) if(items[i])  
        printf("%4dx %s\n", items[i], iinf[i].name);
        
      printf("\n");
      
      for(int i=1; i<motypes; i++) if(kills[i])  
        printf("%4dx %s <%d>\n", kills[i], minf[i].name, i);
      
      printf("\n\n\n");      
      }

    if(hrand(5000) == 0 || (isGravityLand(cwt.at->land) && coastvalEdge(cwt.at) >= 100) || gcount > 2000 || cellcount >= 20000000) {
      printf("RESET\n");
      gcount = 0;
      cellcount = 0;
      activateSafety(laCrossroads);
      // landlist[hrand(isize(landlist))]);
      }

    if(cwt.at->land == laWestWall && cwt.at->landparam >= 30) {
      printf("Safety generated\n");
      forCellEx(c2, cwt.at) c2->item = itOrbSafety;
      }
    
    for(int i=0; i<isize(dcal); i++) {
      cell *c = dcal[i];
      if(isChild(c, NULL)) {
        }
      if(childbug) doAutoplay = false;
      if(c->land == laNone) { 
        printf("no-land found\n"); 
        kills[moPlayer] = 0;
        canmove = true;
        doAutoplay = false; 
        }
      }
    
    if(buggyGeneration || isize(buggycells)) {
      if(noGUI) {
        printf("Fatal: buggy generation\n");
        exit(1);
        }
      println(hlog, "buggy generation");
      return;
      }

    if(turncount >= num_moves) return;
    }
  }

int readArgs() {
  using namespace arg;
           
  if(0) ;
  else if(argis("-autoplay")) {
    PHASE(3); 
    autoplay();
    }
  else if(argis("-autoplayN")) {
    PHASE(3); 
    shift();
    autoplay(argi());
    }

  else return 1;
  return 0;
  }

auto hook = addHook(hooks_args, 100, readArgs);

}
