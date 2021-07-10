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

cell *cellToTarget() {
  if (isCrossroads(cwt.at->land)) {
    for (cell *c: dcal) {
      if (c->land == laCamelot && c->wall == waNone) {
        printf("Trying to teleport into Camelot\n");
        items[itOrbTeleport] += 1;
        return c;
      }
    }
  }

  if(cwt.at->land == laCamelot) {
    int oldDist = celldistAltRelative(cwt.at);
    if (oldDist > 3) {
      for (cell *c: dcal) {
        if (c->land == laCamelot) {
          int dist = celldistAltRelative(c);
          if (-1 <= dist && dist <= 1 && hrand(10) == 0) {
            printf("Trying to teleport near the round table (%d to %d)\n", oldDist, dist);
            items[itOrbTeleport] += 1;
            return c;
          }
        }
      }
    }
  }

  return dcal[hrand(isize(dcal))];
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
      printf("%10dcc %5dt %5de %5d$ %5dK %5dgc %-30s H%d\n", cellcount, turncount, celldist(cwt.at), gold(), tkills(), gcount, dnameof(cwt.at->land).c_str(), hrand(1000000));
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

    /* if(gcount < 500) for(int i=1; i<isize(dcal); i++) {
      c2 = dcal[i];
      if(lcount >= 50 && !sameland(lland, c2->land) && !sameland(lland2, c2->land)) break;
      else if(lcount < 50 && c2->item && c2->item != itOrbSafety) break;
      } */

    // Use a random cheat
    int croll = hrand(50);
    if (croll < 25) {
      eItem b = (eItem) hrand(ittypes);
      printf("Gain item: %s\n", iinf[b].name);
      items[b] = (1 << hrand(11)) - 1;
      items[itOrbYendor] &= 15;
      reduceOrbPowers(); // in particlar, cancel out slaying+weakness, since the combination may confuse shadow
    } else if (croll == 25) {
      printf("Gain kills\n");
      kills[hrand(motypes)] = (1 << hrand(11)) - 1;
    } else if (croll == 26) {
      printf("Princess soon\n");
      princess::saved = true;
      princess::everSaved = true;
      items[itSavedPrincess]++;
      items[itOrbLove] = 1;
      items[itOrbTime] = 0;
    } else if (croll == 27) {
      printf("Gain allies\n");
      forCellEx(cz, cwt.at)
        if (!cz->monst)
          cz->monst = pick(moMouse, moFriendlyGhost, moGolem, moTameBomberbird, moKnight);
    } else if (croll == 28) {
      printf("Place orbs with pickup effects\n");
      forCellEx(cz, cwt.at)
        if (!cz->item)
          cz->item = pick(itOrbLife, itOrbFriend, itOrbSpeed, itOrbShield, itOrbChaos, itOrbPurity);
    } else if (croll == 29) {
      printf("Place fun walls\n");
      forCellEx(cz, cwt.at)
        if (!cz->wall)
          cz->wall = pick(waExplosiveBarrel, waBigStatue, waThumperOff, waBonfireOff, waCloud, waMirror);
    } else if (croll == 30) {
      cell *ct = dcal[hrand(isize(dcal))];
      if (!isPlayerOn(ct) && !ct->monst && !ct->wall) {
        eWall hazard = pick(waRose, waFireTrap, waMineMine, waTrapdoor, waChasm, waCavewall);
        printf("Spam a hazard: %s\n", winf[hazard].name);
        ct->wall = hazard;
      }
    } else if (croll == 31 && !memory_saving_mode) {
      //printf("Saving memory\n");
      //memory_saving_mode = true;
      //save_memory();
      //memory_saving_mode = false;
    } else if (croll == 33) {
      cell *ct = dcal[hrand(isize(dcal))];
      if (!isPlayerOn(ct) && !ct->monst && !ct->wall) {
        printf("Spam some slime\n");
        ct->item = itNone;
        ct->wall = hrand(2) ? waFloorA : waFloorB;
        ct->monst = hrand(2) ? moSlime : moNone;
      }
    }

    // don't show warning dialogs
    items[itWarning] = 1;

    // Make a random move
    int roll = hrand(50);
    if (roll == 0) {
      // drop dead orb
      bool res = movepcto(MD_DROP, 1);
      printf("DROP: %d\n", res);
    } else if (roll < 5) {
      // skip turn
      bool res = movepcto(MD_WAIT, 1);
      printf("WAIT: %d\n", res);
    } else if (roll < 42) {
      // move to or attack a neighbor cell
      int i = hrand(cwt.at->type);
      cell *c2 = cwt.at->move(i);
      cwt.spin = 0;
      int d = neighborId(cwt.at, c2);
      if (d >= 0) {
        int subdir = (roll%2==0)?1:-1;
        string c2info = dnameof(c2->wall) + "; " + dnameof(c2->monst) + "; " + dnameof(c2->item);
        bool res = movepcto(d, subdir, false);
        printf("MOVE %d [%s] sub %d: %d\n", d, c2info.c_str(), subdir, res);
        if (!res && c2->monst) {
          printf("clearing the monster (%s)\n", minf[c2->monst].name);
          killMonster(c2, moNone);
        }
      } else {
        printf("MOVE CONFUSED %d\n", d);
        return;
      }
    } else {
      // try to use a ranged orb
      cell *ct = cellToTarget();
      eItem ti = targetRangedOrb(ct, roMouseForce);
      const char *tm = (ti == eItem(-1)) ? "orb cannot be used (see message log)" : iinf[ti].name;
      printf("TARGET %p: %s\n", (void*)ct, tm);
    }

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
      if (cellcount < 0) {
        //printf("How did cellcount become negative?\n");
        cellcount = 1;
        }
      }

    if(cwt.at->land == laWestWall && cwt.at->landparam >= 30) {
      printf("Safety generated\n");
      forCellEx(c2, cwt.at) c2->item = itOrbSafety;
      }
    
    if(isIcyLand(cwt.at)) {
      float heat = HEAT(cwt.at);
      // Checking for extreme values as well as NaNs and infinities
      if (!(-1e10 < heat && heat < 1e10)) {
        printf("Heat is out of expected range\n");
        canmove = true;
        doAutoplay = false;
        }
      }

    if (cwt.at->land == laCamelot) {
      for(int i=0; i<isize(dcal); i++) {
        cell *c = dcal[i];
        if(c->land == laCamelot && celldistAltRelative(c) == 0 && c->wall != waRoundTable) {
          printf("The round table of camelot has been breached!\n");
          kills[moPlayer] = 0;
          canmove = true;
          doAutoplay = false;
          }
        }
      }

    if(cwt.at->monst && !isMultitile(cwt.at->monst)) {
      printf("on a non-multitile monster: %s\n", minf[cwt.at->monst].name);
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
      if(c->item == itBuggy || c->item == itBuggy2) {
        printf("buggy item found\n");
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
