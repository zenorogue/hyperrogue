// Hyperbolic Rogue

// Copyright (C) 2011-2016 Zeno Rogue, see 'hyper.cpp' for details

// land generation routines

bool safety = false;

eLand lastland;

int lastexplore;

bool randomPatternsMode = false;
int randompattern[landtypes];

bool chaosUnlocked = false;
bool chaosAchieved = false;

bool notDippingFor(eItem i) {
  if(peace::on) return false;
  int v = items[i] - currentLocalTreasure;
  if(v <= 10) return true;
  if(v >= 20) return false;
  return v >= hrand(10) + 10;
  }

bool notDippingForExtra(eItem i, eItem x) {
  if(peace::on) return false;
  int v = items[i] - min(items[x], currentLocalTreasure);
  if(v <= 10) return true;
  if(v >= 20) return false;
  return v >= hrand(10) + 10;
  }

void buildCamelotWall(cell *c) {
  c->wall = waCamelot;
  for(int i=0; i<c->type; i++) {
    cell *c2 = createMov(c, i);
    if(c2->wall == waNone && (euclid || (c2->master->alt && c->master->alt)) && celldistAlt(c2) > celldistAlt(c) && c2->monst == moNone)
      c2->wall = waCamelotMoat;
    }
  }

void buildRedWall(cell *c, int gemchance) {
  if(c->monst) return;
  int ki = PT(kills[moHexSnake] + kills[moRedTroll], 50);
  c->wall = waRed3;
  if(hrand(100+ki) < gemchance + ki)
    c->item = itRedGem;
  if(items[itRedGem] >= 10 && hrand(8000) < gemchance && !peace::on && !inv::on)
    c->item = itOrbSpace;
  else if(hrand(8000) < gemchance * PRIZEMUL)
    placePrizeOrb(c);
  }

#define RANDPATC(c) (randpattern(c,randompattern[c->land]))
#define RANDPATCL(c, l) (randpattern(c,l))
#define RANDPAT (randpattern(c,randompattern[c->land]))
#define RANDPAT3(i) (randpatternMajority(c,i,RANDITER))
#define RANDPATV(x) (randpattern(c,randompattern[x]))

bool reptilecheat = false;

// This function generates all lands. Warning: it's very long!
void setdist(cell *c, int d, cell *from) {
  
  if(signed(c->mpdist) <= d) return;
  if(c->mpdist > d+1 && d != BARLEV) setdist(c, d+1, from);
  c->mpdist = d;
  // printf("setdist %p %d [%p]\n", c, d, from);
  
  // this fixes the following problem:
  // http://steamcommunity.com/app/342610/discussions/0/1470840994970724215/
  if(!generatingEquidistant && from && d >= 7) {
    int cdi = celldist(c);
    if(celldist(from) > cdi) {
      forCellCM(c2, c) if(celldist(c2) < cdi) {
        setdist(c2, d, c);
        from = c2;
        break;
        }
      }
    }

  if(d <= 3) lastexplore = shmup::on ? shmup::curtime : turncount;

  if(buggyGeneration) {
    if(d < BARLEV) for(int i=0; i<c->type; i++) {
      setdist(createMov(c, i), d+(purehepta?2:1), c);
      }
    if(d >= BARLEV) c->item = itBuggy2;
    return;
    }
  
  if(d >= BARLEV) {
  
    if(!c->land && from->land != laElementalWall && from->land != laHauntedWall && from->land != laOceanWall &&
      from->land != laBarrier) {
        if(!hasbardir(c)) setland(c, from->land);
        }
    if(c->land == laTemple && !tactic::on && !chaosmode) setland(c, laRlyeh);
    if(c->land == laMountain && !tactic::on && !chaosmode) setland(c, laJungle);
    if(c->land == laClearing && !tactic::on) setland(c, laOvergrown);
    if(c->land == laWhirlpool && !tactic::on && !yendor::on) setland(c, laOcean);
    if(c->land == laCamelot && !tactic::on) setland(c, laCrossroads);

    if(sphere || torus) setLandSphere(c);
    else if(euclid) setLandEuclid(c);
    if(quotient) { setland(c, specialland); setLandQuotient(c); }
    
    // if(chaosmode) setland(c, getCLand(c));
    }
  
  if(d == BARLEV && c->land == laCanvas)  {
      c->landparam = mapeditor::generateCanvas(c);
      }

  int hard = yendor::hardness();

  if(d >= BARLEV-1 && c->land == laPrairie)
    prairie::spread(c, from);

  if(d < BARLEV && c->land == laPrairie && !c->landparam) {
    printf("d=%d/%d\n", d, BARLEV);
    raiseBuggyGeneration(c, "No landparam set");
    return;
    }
    
  if(d == BARLEV && !euclid && c != cwt.c) 
    buildBigStuff(c, from);
  
  if(buggyGeneration) return;
  
  if(d < 10) {
    explore[d]++;
    exploreland[d][c->land]++;
    
    if(d < BARLEV) for(int i=0; i<c->type; i++) {
      setdist(createMov(c, i), d+(purehepta && d>=3 && d<7?2:1), c);
      if(buggyGeneration) return;
      }
    
    if(d == BARLEV-2 && c->land == laOcean) 
      buildEquidistant(c);

    if(d == BARLEV-2 && inmirror(c)) 
      buildEquidistant(c);

    if(d == BARLEV-2 && (c->land == laGraveyard || c->land == laHauntedBorder || c->land == laHaunted))
      buildEquidistant(c);
    
    if(d == 7 && c->land == laPrairie) {
      if(prairie::isriver(c)) {
        if(shmup::on) prairie::beaststogen.push_back(c); 
        else prairie::generateBeast(c);
        }
      else if(!prairie::nearriver(c) && c->LHU.fi.flowerdist > 7) {
        if(hrand(9000) < items[itGreenGrass] - (prairie::isriver(cwt.c) ? 40 : 0))
          c->monst = moGadfly;
        else buildPrizeMirror(c, 1000);
        }
      }
    
    if(d == 7)
      prairie::generateTreasure(c);
    
    if(d <= 7 && (c->land == laGraveyard || c->land == laHauntedBorder)) {
      c->land = (c->landparam >= 1 && c->landparam <= HAUNTED_RADIUS) ? laHauntedBorder : laGraveyard;
      }

    if(d == 8 && isGravityLand(c->land)) {
      buildEquidistant(c);
      }

    dungeon::all(c, d);

    if(d == 9 && c->land == laPalace) {
      if(cdist50(c) == 3 && polarb50(c) == 1)
        c->wall = waPalace;
      }
  
    if(d == 8 && c->land == laPalace && sphere) {
      if(getHemisphere(c,0) == 1)
        c->wall = waPalace;
      if(getHemisphere(c,0) == 3)
        c->wall = purehepta ? waOpenGate : waClosedGate;
      if(getHemisphere(c,0) == 4 && hrand(100) < 40)
        c->wall = waClosePlate;
      if(getHemisphere(c,0) == 6)
        c->wall = waOpenPlate;
      if(getHemisphere(c,0) == -3)
        c->wall = pick(waClosePlate, waOpenPlate);
      if(getHemisphere(c,0) == -6)
        c->wall = waTrapdoor;
      }

    if(d == 8 && c->land == laPalace && !sphere) {
    
      // note: Princess Challenge brings back the normal Palace generation
      bool lookingForPrincess = !euclid && c->master->alt && !princess::challenge;
      
      bool pgate = false;
      if(purehepta) {
        int i = fiftyval049(c);
        if(i >= 8 && i <= 14 && !polarb50(c)) pgate = true;
        }
      
      if(pgate) {
        switch(princess::generating ? 0 : hrand(2)) {
          case 0: 
            c->wall = waClosedGate;
            break;
          case 1:
            c->wall = waOpenGate;
            break;
          }
        for(int s=0; s<7; s++) if(c->mov[s] && 
          (c->mov[s]->wall == waClosedGate || c->mov[s]->wall == waOpenGate))
            c->wall = c->mov[s]->wall;
        }
      else if(cdist50(c) == 3 && polarb50(c) == 1) {
        int q = 0, s = 0;
        if(!ishept(c)) for(int i=0; i<6; i++)
          if(cdist50(c->mov[i]) == 3 && polarb50(c->mov[i]) == 1 && !ishept(c->mov[i]))
            q++, s += i;
        if(q == 1 && c->mov[s]->land == laPalace) {
          switch(princess::generating ? 0 : hrand(2)) {
            case 0: 
              c->wall = waClosedGate;
              c->mov[s]->wall = waClosedGate;
              break;
            case 1:
              c->wall = waOpenGate;
              c->mov[s]->wall = waOpenGate;
              break;
            }
          }
        }
      else if((hrand(100) < (lookingForPrincess ? (purehepta ? 11 : 7) : 5) && cdist50(c)) ||
        (cdist50(c) == 0 && polarb50(c) && hrand(100) < 60)) {
        c->wall = hrand(100) < (lookingForPrincess ? (purehepta ? 25 : 30):50) ? waClosePlate : waOpenPlate;
        }
      else if(hrand(100) < (lookingForPrincess ? 3 : 5))
        c->wall = waTrapdoor;
      
      if(cdist50(c) == 0 && yendor::path) {
        cell *c2 = c->mov[hrand(c->type)];
        if(c2->wall == waNone) c2->wall = waTrapdoor;
        }

      if((c->wall == waClosePlate || c->wall == waTrapdoor) && peace::on) c->wall = waNone;
      }
  
    if(d==8 && c->land == laEmerald) {
      if(randomPatternsMode)
        c->wall = RANDPAT3(0) ? waCavewall : waCavefloor;
      else if(torus) ;
      else if(euclid) {
        eucoord x, y;
        decodeMaster(c->master, x, y);
        if(((y-2)&7) < 4) c->wall = waCavewall;
        else c->wall = waCavefloor;
        }
      else if(sphere) {
        if(getHemisphere(c, 0) < 0) 
          c->wall = waCavewall;
        else c->wall = waCavefloor;
        }
      else if(purehepta) {
        if(polarb50(c)) 
          c->wall = waCavewall;
        else c->wall = waCavefloor;
        }
      else {
        int v = emeraldval(c);
        if((v&3) >= 2) 
          c->wall = waCavewall;
        else c->wall = waCavefloor;
        }
      }
    
    if(d == 8 && isIcyLand(c)) c->landparam = 0;
    if(d == 8 && c->land == laDryForest) c->landparam = 0;

    if(d==8 && c->land == laPower) {
      int v;
      if(randomPatternsMode)
        v = RANDPAT ? 24 : 0;
      else if(torus) v=0;
      else if(euclid) {
        eucoord x, y;
        decodeMaster(c->master, x, y);
        int y0 = ((short)y) % 6;
        if(y0<0) y0+=6;
        if(y0 == 3 || y0 == 4) v=24; else v=0;
        }
      else v = emeraldval(c);
      v &= ~3;
      if((v == 24 || v == 32 || v == 56))
        c->wall = waEternalFire;
      else if(hrand(100) < 10) {
        c->wall = waGlass;
        eItem protectedItems[18] = {
          itPower, itPower, itPower, itPower, itPower, itPower,
          itOrbLightning, itOrbLightning, itOrbThorns, itOrbThorns,
          itOrbInvis, itOrbInvis,
          itOrbShield, itOrbTeleport, itOrbPsi,
          itOrbDragon, itOrbIllusion, itOrbTime
          };
        c->item = protectedItems[hrand(18)];
        }
      }

    if(d==8 && c->land == laZebra) {
      if(torus) ;
      else if(euclid) {
        eucoord x, y;
        decodeMaster(c->master, x, y);
        if(y&1) c->wall = waTrapdoor;
        else c->wall = waNone;
        }
      else
      c->wall = (randomPatternsMode ? RANDPAT : (zebra40(c)&2)) ? waTrapdoor : waNone;
      }
    
    if(d==8 && isElemental(c->land)) {
      if(hrand(c->land == laEAir ? 6 : 25) == 0) {
        if(c->land == laEFire) c->wall = waEternalFire;
        else if(c->land == laEWater) c->wall = waSea;
        else if(c->land == laEAir) c->wall = waChasm;
        else if(c->land == laEEarth) c->wall = waStone;
        }
      }
    
    if(d==8 && c->land == laWineyard) {
      if(torus) ;
      else if(euclid) {
        eucoord x, y;
        decodeMaster(c->master, x, y);
        int dy = ((short)y)%3; if(dy<0) dy += 3;
        if(dy == 1) c->wall = waVinePlant;
        }
      else {
        int v = emeraldval(c);
        int w = v / 4;
        if(randomPatternsMode) c->wall = RANDPAT ? waVinePlant : waNone;
        else if(w == 9 || w == 10 || w == 7 || w == 8) {
          c->wall = waVinePlant;
          }
        else if(v == 24 || v == 58 || v == 26 || v == 56)
          c->wall = waVineHalfA;
        else if(v == 25 || v == 59 || v == 27 || v == 57)
          c->wall = waVineHalfB;
        else c->wall = waNone;
        }
      }
    
    if(d == 7 && cellHalfvine(c)) {
      int i = -1;
      for(int k=0; k<c->type; k++) if(c->mov[k] && c->mov[k]->wall == c->wall)
        i = 0;
      if(i == -1) c->wall = waNone;
      }

    // 24-58
    // 26-56
        
    if(d == 9) {
    
      // mapgen9

      if(isWarped(c->land) && randomPatternsMode)
        setland(c, RANDPAT ? laWarpCoast : laWarpSea);
      
      // if(c->land == laIce && ((celldist(c) % 10) + 10) % 10 == 5)
      //   c->wall = waColumn;
      
      if(c->land == laReptile) {
        int i = zebra40(c);
        if(i < 40) {
          int cd = getCdata(c, 3);
          cd &= 15;
          if(cd >= 4 && cd < 12) c->wall = waChasm;
          else {
            int ch =  hrand(200);
            c->wall = ch < (50 + items[itDodeca] + hard) ? waReptile : 
             waNone;
            }
          c->wparam = 1 + hrand(reptilemax());
          }
        if(reptilecheat) {
          c->wall = waReptile; c->wparam = 100;
          }
        }

      if(c->land == laBurial) {
        if(hrand(5000) < 25 && celldist(c) >= 5 && !safety) {
          c->item = itBarrow;
          c->landparam = 2 + hrand(2);
          c->wall = waBarrowDig;
          forCellCM(c2, c) c2->wall = waBarrowWall, c2->item = itNone;
          cell *c2 = createMov(c, hrand(c->type));
          c2->wall = waBarrowDig;
          forCellCM(c3, c2) {
            if(c3 == c || isNeighbor(c3, c)) continue;
            bool adj = false;
            forCellEx(c4, c)
              if(c4 != c2 && isNeighborCM(c3, c4)) adj = true;
            if(adj)
              c3->wall = waBarrowDig;
            else
              c3->wall = waBarrowWall, c3->item = itNone;
            }
          }

        /* if(hrand(25000) < PT(25 + 2 * kills[moDraugr], 40) && notDippingFor(itBarrow)) {
          c->item = itBarrow;
          c->wall = waBarrowCenter;
          forCellCM(c2, c) c2->wall = waBarrowCenter, c2->item = itBarrow;
          forCellCM(c2, c) forCellCM(c3, c2) if(!isNeighbor(c3, c) && c3 != c)
            c3->wall = waBarrowWall, c3->item = itNone;
          int i = hrand(c->type);
          cell *c2, *c3;
          while(true) { 
            c2 = createMov(c, i);
            c3 = createMov(c2, hrand(c2->type));
            if(c3 != c && !isNeighbor(c3, c)) break;
            }
          c3->wall = waBarrowCenter;
          forCellCM(c4, c3) {
            if(c4 == c2 || isNeighbor(c4, c2)) continue;
            bool adj = false;
            forCellEx(c5, c2) if(c5 != c3 && isNeighbor(c5, c4)) adj = true;
            if(adj)
              c4->wall = waBarrowCenter;
            else
              c4->wall = waBarrowWall, c4->item = itNone;
            }
          } */
        }

      if(c->land == laTrollheim) {
        if(hrand(50000) < (chaosmode?1000:5) && c->wall != waBarrier && celldist(c) >= 7 && !safety && !peace::on) {
          bool okay = true;
          forCellCM(c2, c) forCellCM(c3, c2) forCellCM(c4, c3) forCellCM(c5, c4) {
            cell *cx = chaosmode ? c3 : c5;
            if(cx->land != laTrollheim && cx->land != laNone)
              okay = false;
            if(cx->bardir != NODIR) okay = false;
            }
          if(okay) {
            forCellCM(c2, c) forCellCM(c3, c2) forCellCM(c4, c3) forCellCM(c5, c4) {
              cell *cx = chaosmode ? c3 : c5;
              cx->bardir = NOBARRIERS;
              setland(cx, laTrollheim);
              }
            c->item = itTrollEgg;
            forCellCM(c2, c) forCellCM(c3, c2) {
              c3->monst = pickTroll(c);
              c2->item = itTrollEgg;
              }
            }
          }
        }

      if(c->land == laIce) {
        if(randomPatternsMode) c->wall = RANDPAT ? waIcewall : waNone;
        else if(hrand(100) < 5 && c->wall != waBarrier) {
          c->wall = waIcewall;
          for(int i=0; i<c->type; i++) if(hrand(100) < 50) {
            cell *c2 = createMov(c, i);
            setdist(c2, d+1, c);
            if(c2->wall == waBarrier || c2->land != laIce) continue;
            c2->wall = waIcewall;
            for(int j=0; j<c2->type; j++) if(hrand(100) < 20) {
              cell *c3 = createMov(c2, j);
              setdist(c->mov[i], d+2, c);
              if(c3->wall == waBarrier || c3->land != laIce) continue;
              c3->wall = waIcewall;
              }
            }
          }
        }

      if(c->land == laIce || c->land == laCocytus) if(c->wall == waIcewall && items[itDiamond] >= 5 && hrand(200) == 1)
        c->wall = waBonfireOff;

      if(c->land == laDragon) {
        int cd = getCdata(c, 3);
        cd &= 31;
        if(cd >= 6 && cd<12) c->wall = waChasm;
        else if(hrand(1000) == 0) c->wall = waEternalFire;
        }
      
      if(c->land == laCaves) 
        c->wall = (randomPatternsMode ? RANDPAT3(1) : hrand(100) < 55) ? waCavewall : waCavefloor;
      
      if(c->land == laCA) 
        c->wall = (hrand(1000000) < ca::prob * 1000000) ? waFloorA : waNone;
      
      if(c->land == laLivefjord) { 
        int die = (randomPatternsMode ? (RANDPAT3(2)?100:0) : hrand(100));
        if(die < 50)
          c->wall = waSea;
        else
          c->wall = waNone;
        }
      
      if(c->land == laDeadCaves) {
        int die = (randomPatternsMode ? (RANDPAT?100:0) : hrand(100));
        if(die<50) c->wall = waDeadwall;
        else if(die<55) c->wall = waDeadfloor2;
        else c->wall = waDeadfloor;
        }
      
      if(c->land == laAlchemist) 
        c->wall = (randomPatternsMode ? RANDPAT : hrand(2)) ? waFloorA : waFloorB;

      if(c->land == laVolcano) {
        c->wall = waNone;
        if(hrand(20000) < (items[itLavaLily] + hard))
          c->monst = moSalamander,
          c->hitpoints = 3;
        }
      
      if(c->land == laBlizzard) {
        bool windless = true;
        int w = windmap::at(c);
        forCellCM(c2, c) {
          int w2 = windmap::at(c2);
          if(((w2-w) & 255) >= windmap::NOWINDBELOW)
          if(((w-w2) & 255) >= windmap::NOWINDBELOW)
            windless = false;
          }
        if(windless) {
          c->wall = waIcewall;
          if(hrand(500) < PT(100 + 2 * kills[moVoidBeast] + 2 * kills[moIceGolem], 200) && notDippingFor(itBlizzard))
            c->item = itBlizzard;
          }
        }
      
      if(c->land == laTerracotta) {
        if(hrand(500) < 15) {
          cellwalker cw(c, hrand(c->type));
          cell* cc[5];
          cc[2] = c;
          cellwalker cw2 = cw;
          cwstep(cw); cc[3] = cw.c; cwrevstep(cw); cc[4] = cw.c;
          cwrevstep(cw2); cc[1] = cw2.c; cwrevstep(cw2); cc[0] = cw2.c;
          bool ok = true;
          for(int i=0; i<5; i++) {
            if(cc[i]->land != laNone && cc[i]->land != laTerracotta) ok = false;
            if(cc[i]->bardir != NODIR) ok = false;
            }
          for(int i=1; i<4; i++) {
            forCellEx(c2, cc[i]) if(c2->wall == waArrowTrap) ok = false;
            }
          if(ok) {
            for(int i=1; i<4; i++) 
              cc[i]->wall = waArrowTrap,
              cc[i]->wparam = 0;
            for(int i=0; i<5; i++) 
              cc[i]->bardir = NOBARRIERS;
            cc[0]->wall = waStone;
            cc[4]->wall = waStone;
            }
          }
        if(pseudohept(c) && hrand(100) < 40 && c->wall == waNone) {
          c->wall = waTerraWarrior;
          c->landparam = randterra ? 0 : 3 + hrand(3);
          }
        }

      if(c->land == laDryForest) {
        if(randomPatternsMode)
          c->wall = RANDPAT ? waNone : RANDPATV(laHell) ? waBigTree : waSmallTree;
        else
          c->wall = 
            (hrand(100) < 50) ? (hrand(100) < 50 ? waBigTree : waSmallTree) : waNone;
        }

      if(c->land == laOvergrown) {
        // 0: 60%
        // 10: 50%
        if(randomPatternsMode)
          c->wall = RANDPAT ? waNone : RANDPATV(laWineyard) ? waBigTree : waSmallTree;
        else
          c->wall = 
            (hrand(50+items[itMutant]/2+hard) < 30) ? (hrand(100) < 50 ? waBigTree : waSmallTree) : waNone;
        }

      if(c->land == laHalloween) {
        if(purehepta) {
          int fv = c->master->fiftyval;
          if(fv == 1 || fv == 4 || fv == 2) 
            c->wall = waChasm;
          if(fv == 3) c->item = itTreat;
          }
        else {
          if(c->type == 5) {
            int fv = c->master->fiftyval;
            if(fv == 3 || fv == 4 || fv == 2 || fv == 5) 
              c->wall = waChasm;
            if(fv == 2) halloween::dragoncells[0] = c;
            if(fv == 5) halloween::dragoncells[3] = c;
            if(fv == 1) c->item = itTreat;
            }
          if(c->type == 6 && !euclid) {
            int fvset = 0;
            for(int i=0; i<6; i+=2) fvset |= 1 << createMov(c, i)->master->fiftyval;
            if(fvset == 35 || fvset == 7) c->wall = waChasm;
            if(fvset == 7) halloween::dragoncells[1] = c;
            if(fvset == 35) halloween::dragoncells[2] = c;
            }
          }
        if(quotient && zebra40(c) == 7) {
          c->item = itTreat;
          halloween::dragoncells[0] = NULL;
          }
        if(quotient && zebra40(c) == 5) {
          c->wall = waChasm;
          }
        }

      if(c->land == laWildWest) {
        if(randomPatternsMode)
          c->wall = RANDPAT ? waNone : waSaloon;
        else if(cdist50(c) <= 2) c->wall = waSaloon;
        
        /*if(i == 3 && polarb50(c) == 1) {
          for(int j=0; j<c->type; j++) if(c->mov[j] && polarb50(c->mov[j]) == 0)
            c->wall = waFloorA;
          } */
        }

      if(c->land == laWhirlwind) {
        if(sphere) 
          c->wall = (pseudohept(c) && (c->master->fiftyval == 0 || c->master->fiftyval == 6)) ?
            waFan : waNone;
        else if(!euclid && zebra3(c) == 0) c->wall = waFan;
        else if(pseudohept(c) && hrand(2000) < 150)
          c->wall = waChasm;
        else if(c->type == 6 && buildPrizeMirror(c, 1000))
          {}
        else
          whirlwind::switchTreasure(c);

        /* bool edge = false;
        for(int i=0; i<c->type; i++) 
         if(zebra3(c->mov[i]) != z)
           edge = true; */
        // if(edge && hrand(100) < 5)
        //    c->wall = waSaloon;
        }
      
      if(c->land == laStorms) {
      
        if(torus) ;
        else if(euclid) {
          eucoord x, y;
          decodeMaster(c->master, x, y);
          if(short(x+1)%3 == 0 && short(y)%3 == 0) {
            if(hrand(100) < 50)
              c->wall = hrand(2) ? waCharged : waGrounded;
            }
          else if(ishept(c)) {
            bool sand = false;
            for(int i=0; i<c->type; i++) {
              createMov(c, i);
              decodeMaster(c->mov[i]->master, x, y);
              if(short(x+1)%3 == 0 && short(y)%3 == 0) sand = true;
              }
            if(sand && hrand(100) < 20)
              c->wall = waSandstone;
            }
          }
        else if(sphere) { 
          if(c->type != 6) {
            int id = c->master->fiftyval;
            if(id == 1) c->wall = waCharged;
            if(id == (elliptic && !purehepta ? 3 : 9)) c->wall = waGrounded;
            }
          }
        else if(purehepta) {
          int i = zebra40(c);
          if((i == 5 || i == 8) && hrand(100) < 20) c->wall = hrand(2) ? waCharged : waGrounded;
          else if(i == 15) c->wall = waSandstone;
          }
        else {
          int i = zebra40(c);
          if(i >= 40 && hrand(100) < 50) 
            c->wall = hrand(2) ? waCharged : waGrounded;
          else if(ishept(c) && hrand(100) < 20)
            c->wall = waSandstone;
          }
        
        for(int i=0; i<c->type; i++)
          if(c->mov[i] && c->mov[i]->land != laStorms && c->mov[i]->land != laNone)
            c->wall = waNone;
        }
      
      if(c->land == laGraveyard) {
        if(randomPatternsMode)
          c->wall = RANDPAT ? ((RANDPATV(laCrossroads) || RANDPATV(laCrossroads2)) ? waAncientGrave : waFreshGrave) : waNone;
        else if(pseudohept(c)) 
          c->wall = hrand(5) ? waAncientGrave : waFreshGrave;
        }
      
      if(c->land == laRlyeh)  {
        if(randomPatternsMode) {
          c->wall = RANDPAT ? waColumn : waNone;
          }
        else {
          if(hrand(500) < 5) {
            for(int i=0; i<c->type; i++) {
              cell *c2 = createMov(c, i);
              setdist(c2, d+1, c);
              if(c2 && c2->land == laRlyeh)
                c2->wall = waColumn;
              }
            
            for(int j=0; j<2; j++) {
              int i = hrand(c->type);
              if(c->mov[i] && c->mov[i]->land == laRlyeh)
                c->mov[i]->wall = waNone;
              }
            }
          if(pseudohept(c) && hrand(2)) c->wall = waColumn;
          }
        }
      
      if(c->land == laHell) {
        if(hrand(1000) < (purehepta ? 16 : 36) && celldist(c) >= 3) {
          for(int i=0; i<c->type; i++) {
            cell *c2 = createMov(c, i);
            setdist(c2, d+1, c);
            if(c2 && c2->land == laHell)
              if(c2->wall != waSulphurC)
                c2->wall = waSulphur;
            }
          
          c->wall = waSulphurC;
          }
        }
      
      if(c->land == laCocytus)  {
        if(c->wall == waNone) c->wall = waFrozenLake;
        if(hrand(100) < 5 && !safety && celldist(c) >= 3) {
          for(int i=0; i<c->type; i++) {
            cell *c2 = createMov(c, i);
            setdist(c2, d+1, c);
            if(c2 && c2->land == laCocytus)
              c2->wall = waLake;
            }
          
          c->wall = waLake;

          if(hrand(500) < 100 + 2 * (items[itSapphire] + hard))
            c->monst = moShark;
          }
        }

      if(isHive(c->land) && hrand(2000) < (chaosmode ? 1000 : purehepta?200:2) && !safety) 
        hive::createBugArmy(c);
      
      // landbigstuff
      
      if(c->land == laPalace && !euclid && c->master->alt) {
        int d = celldistAlt(c);
        if(d <= PRADIUS1) generateAlts(c->master);
        }
      
      if((bearsCamelot(c->land) && !euclid && !quotient) || c->land == laCamelot) 
      if(euclid || c->master->alt) {
        int d = celldistAltRelative(c);
        if(tactic::on || (d <= 14 && roundTableRadius(c) > 20)) {
          if(!euclid) generateAlts(c->master);
          preventbarriers(c);
          if(d == 10) {
            if(pseudohept(c)) buildCamelotWall(c);
            else {
              if(!euclid) for(int i=0; i<7; i++) generateAlts(c->master->move[i]);
              int q = 0;
              for(int t=0; t<6; t++) {
                createMov(c, t);
                if(celldistAltRelative(c->mov[t]) == 10 && !pseudohept(c->mov[t])) q++;
                }
              if(q == 1) buildCamelotWall(c);
              // towers of Camelot
              if(q == 0 && !purehepta) {
                c->monst = moKnight;
                c->wall = waTower;
                forCellEx(c2, c) {
                  int cr = celldistAltRelative(c2);
                  if(cr == 9) ;
                  else {
                    buildCamelotWall(c2);
                    if(c2->type == 6)
                      c2->wall = waTower, c2->wparam = 1;
                    }
                  }
                for(int i=0; i<c->type; i++) if(celldistAltRelative(c->mov[i]) < d)
                  c->mondir = i;
                }
              }
            }
          if(d == 0) c->wall = waRoundTable;
          if(celldistAlt(c) == 0 && !tactic::on) c->item = itHolyGrail;
          if(d < 0 && hrand(7000) <= 10 + items[itHolyGrail] * 5) {
            eMonster m[3] = { moHedge, moLancer, moFlailer };
            c->monst = m[hrand(3)];
            }
          if(d == 1) {
            // roughly as many knights as table cells
            if(hrand(purehepta ? 2618 : 1720) < 1000) 
              c->monst = moKnight;
            if(!euclid) for(int i=0; i<7; i++) generateAlts(c->master->move[i]);
            for(int i=0; i<c->type; i++) 
              if(c->mov[i] && celldistAltRelative(c->mov[i]) < d)
                c->mondir = (i+3) % 6;
            }
          if(tactic::on && d >= 2 && d <= 8 && hrand(1000) < 10)
            c->item = itOrbSafety;
          if(d == 5 && tactic::on)
            c->item = itGreenStone;
          if(d <= 10) c->land = laCamelot;
          if(d > 10 && !euclid && !tactic::on) {
            setland(c, eLand(c->master->alt->alt->fiftyval));
            if(c->land == laNone) printf("Camelot\n"); // NONEDEBUG
            }
          }
        }
      
      if((c->land == laStorms && !euclid)) {
        if(c->master->alt && c->master->alt->distance <= 2) {
          generateAlts(c->master);
          preventbarriers(c);
          int d = celldistAlt(c);
          if(d <= -2) {
            c->wall = (c->master->alt->alt->emeraldval & 1) ? waCharged : waGrounded;
            c->item = itNone;
            c->monst = moNone;
            }
          else if(d <= -1)
            c->wall = (hrand(100) < 20) ? waSandstone : waNone;
          else if(d <= 0)
            c->wall = waNone;
          }
        }
      
      if(chaosmode && c->land == laTemple) {
        for(int i=0; i<c->type; i++)
          if(pseudohept(c) && c->mov[i] && c->mov[i]->land != laTemple)
            c->wall = waColumn;
        }
      
      else if((c->land == laRlyeh && !euclid) || c->land == laTemple) {
        if(euclid || (c->master->alt && (tactic::on || c->master->alt->distance <= 2))) {
          if(!euclid && !chaosmode) generateAlts(c->master);
          preventbarriers(c);
          int d = celldistAlt(c);
          if(d <= 0) {
            c->land = laTemple, c->wall = waNone, c->monst = moNone, c->item = itNone;
            }
          if(d % TEMPLE_EACH==0) {
            if(pseudohept(c)) 
              c->wall = waColumn;
            else {
              if(!euclid) for(int i=0; i<7; i++) generateAlts(c->master->move[i]);
              int q = 0;
              for(int t=0; t<6; t++) {
                createMov(c, t);
                if(celldistAlt(c->mov[t]) % TEMPLE_EACH == 0 && !ishept(c->mov[t])) q++;
                }
              if(q == 2) c->wall = waColumn;
              }
            }
          }
        }

     if((c->land == laOvergrown && !euclid) || c->land == laClearing) {
        if(euclid || (c->master->alt && (tactic::on || c->master->alt->distance <= 2))) {
          if(!euclid) generateAlts(c->master);
          preventbarriers(c);
          int d = celldistAlt(c);
          if(d <= 0) {
            c->land = laClearing, c->wall = waNone; // , c->monst = moNone, c->item = itNone;
            }
          else if(d == 1 && !tactic::on && !euclid)
            c->wall = waSmallTree, c->monst = moNone, c->item = itNone;
          }
        }

     if((c->land == laJungle && !euclid) || c->land == laMountain) {
        if(euclid || (c->master->alt && (tactic::on || c->master->alt->distance <= 2))) {
          if(!euclid) generateAlts(c->master);
          preventbarriers(c);
          int d = celldistAlt(c);
          if(d <= 0 || (firstland == laMountain && tactic::on)) {
            c->land = laMountain, c->wall = waNone; // , c->monst = moNone, c->item = itNone;
            }
          }
        }

      if(c->land == laOcean || c->land == laWhirlpool) {
        bool fullwhirlpool = false;
        if(tactic::on && tactic::lasttactic == laWhirlpool)
          fullwhirlpool = true;
        if(yendor::on && yendor::clev().l == laWhirlpool)
          fullwhirlpool = true;
        if(euclid || (c->master->alt && (fullwhirlpool || c->master->alt->distance <= 2))) {
          if(!euclid) generateAlts(c->master);
          preventbarriers(c);
          int dd = celldistAlt(c);
          if(dd <= 0 || fullwhirlpool) {
            c->land = laWhirlpool, c->wall = waSea, c->monst = moNone, c->item = itNone;
            }
          }
        }
      
      if(c->land == laKraken) {
        c->wall = waSea;
        }

      if(c->land == laCaribbean) {
        if(!euclid) {
          if(c->master->alt && c->master->alt->distance <= 2) {
            if(!euclid) generateAlts(c->master);
            preventbarriers(c);
            int d = celldistAlt(c);
            if(d <= 0) 
              // c->wall = waChasm;
              generateTreasureIsland(c);
            else
              c->wall = waSea;
            }
          else c->wall = waSea;
          }
        else {
          int d = celldistAlt(c);
          if(d <= 0) 
            generateTreasureIsland(c);
          else
            c->wall = waSea;
          }
        }
      
      if(c->land == laTortoise) {
        using namespace tortoise;
        int f = hrand(30);
        if(f < 21 && ((getBits(c)>>f) & 1)) {
          tflag tf = tflag(f);
          // long neck and saddleback shell help with big trees
          #define PSH(x,y) if(tf == x || tf == y) tf = pseudohept(c) ? x : y;
          PSH(tfLongNeck, tfShell)
          if(tf == tfLongNeck) c->wall = waBigTree;
          if(tf == tfShell) c->wall = waBigTree;
          // brown body elements help with (small) trees
          PSH(tfShellHue, tfScuteHue)
          if(tf == tfShellHue) c->wall = waSmallTree;
          if(tf == tfScuteHue) c->wall = waSmallTree;
          if(tf == tfSkinHue) c->wall = waSmallTree;
          // long legs help with rocks
          PSH(tfFront, tfRear)
          if(tf == tfFront) c->wall = waStone;
          if(tf == tfRear) c->wall = waStone;
          // broad tail and desaturated shell helps with water
          PSH(tfTail, tfShellSat)
          if(tf == tfTail) c->wall = waSea;
          if(tf == tfShellSat) c->wall = waSea;
          #undef PSH
          }
        }

      if(isHive(c->land) && hrand(2000) < 100 && !c->wall && !c->item && !c->monst) {
        int nww = 0;
        for(int i=0; i<c->type; i++) if(c->mov[i] && c->mov[i]->wall == waWaxWall)
          nww++;
        if(nww == 0) {
          c->wall = waWaxWall;
          c->monst = moNone;
          c->landparam = hrand(0xFFFFFF + 1);
          }
        /* for(int i=0; i<c->type; i++) {
          if(hrand(6) < 5) {
            createMov(c,i);
            cell *c2 = c->mov[i];
            c2->wall = waWaxWall;
            c2->monst = moNone;
            }
          } */
        }

      if(c->land == laDesert) {
        if(randomPatternsMode)
          c->wall = RANDPAT ? waDune : waNone;
        else {
          if(hrand(100) < 5) {
            for(int i=0; i<c->type; i++) {
              cell *c2 = createMov(c, i);
              setdist(c2, d+1, c);
              if(c2 && c2->land == laDesert)
                c2->wall = waDune;
              }
            
            for(int j=0; j<2; j++) {
              int i = hrand(c->type);
              if(c->mov[i] && c->mov[i]->land == laDesert)
                c->mov[i]->wall = waNone;
              }
            }
          }
  
        if(hrand(300) == 1 && items[itSpice] >= 5) c->wall = waThumperOff;
        }

      if(c->land == laRedRock) {
        if(randomPatternsMode) {
          c->wall = waNone;
          if(!ishept(c)) { if(RANDPAT) buildRedWall(c, 20); }
          else {
            int k = 0;
            for(int i=0; i<20; i++) 
              if(RANDPATCL(createMov(c, i%7), laRedRock) && !RANDPATCL(createMov(c, (i+1)%7), laRedRock))
                k++;
            if(k>=4) buildRedWall(c, 20);
            }
          }
        else if(ishept(c) && hrand(100) < (doCross?20:8) && !c->monst) {
          buildRedWall(c, 80);
          int i = hrand(7);
          buildRedWall(createMovR(c, i), 33);
          if(hrand(2) == 0) 
            buildRedWall(createMovR(createMovR(c, i), c->spn(i)+(hrand(2)?2:4)), 20);
          i += 3 + hrand(2);
          if(hrand(6) < 4)
            buildRedWall(createMovR(c, i), 33);
          if(hrand(2) == 0) 
            buildRedWall(createMovR(createMovR(c, i), c->spn(i)+(hrand(2)?2:4)), 20);
          }
        }
      }

     if(d == 7 && c->land == laReptile && c->wall != waChasm) {
       bool nonchasm = false;
       forCellEx(c2, c) if(c2->wall != waChasm) nonchasm = true;
       if(!nonchasm) c->item = itDodeca;
       }
        

     if(d == 7 && c->land == laKraken && c->wall == waSea && !c->item && !c->monst && !safety) {
       if(hrand(2000) < 3) {
         c->wall = waBoat;
         c->item = itOrbFish;
         c->monst = moViking;
         }
       else if(hrand(2000) < 16) c->item = itOrbFish;
       else if(hrand(500) < kills[moKrakenH]) c->item = itKraken;
       else placeLocalOrbs(c);
       }
      
    if(d == 7 && c->land == laTrollheim && !c->monst && !c->item && celldist(c) >= 3) {
      int cd = getCdata(c, 3);
      cd %= 16;
      if(cd<0) cd += 16;
      if(cd >= 4 && cd < 10 && hrand(100) < 40)
        dieTroll(c, pickTroll(c));
      }

    if(d == 8 && c->land == laBull && !c->monst && !c->item && celldist(c) >= 3) {
      /* int cd = getCdata(c, 3);
      cd &= 15;
      int ce = getCdata(c, 2);
      ce &= 15;
      if(cd >= 8 && ce >= 8) */
      if(hrand(100) < 25)
        c->wall = safety ? pick(waBigTree, waSmallTree) : pick(waStone, waBigTree, waSmallTree);
      }
    
    if(d == 7 && c->land == laBull && c->wall == waNone && !safety) {
      if(hrand(1000) < 20) c->monst = moSleepBull;
      else if(hrand(2500) < items[itBull] + hard - 10) c->monst = moGadfly;
      else if(hrand(1000) < 50) {
        int nearwall = 0;
        int walls[8];
        forCellIdEx(c2, i, c) if(c2->wall) walls[nearwall++] = i;
        if(nearwall && nearwall < c->type) {
          c->monst = moButterfly;
          c->mondir = walls[hrand(nearwall)];
          }
        }
      }

    if(d == 8) {

      if(c->land == laKraken && !pseudohept(c) && hrand(20000) < 10 + 3 * items[itKraken] + 2 * hard && c->wall == waSea && !c->item && !c->monst && !safety) {
        bool ok = true;
        forCellEx(c2, c)
          if(c2->wall != waSea || c2->item || c2->monst) 
            ok = false;
          
        if(ok) {
          c->monst = moKrakenH;
          forCellIdEx(c2, i, c) {
            c2->monst = moKrakenT;
            c2->hitpoints = 1;
            c2->mondir = c->spn(i);
            }
          if(!peace::on) playSound(c, "seen-kraken");
          }
        }

      if(c->land == laWarpCoast) {
        if(hrand(1000) < 150 && celldist(c) >= 3 && !pseudohept(c)) 
          c->wall = waSmallTree;
        int q = 0;
        if(!purehepta && !chaosmode) for(int i=0; i<c->type; i++) 
          if(c->mov[i] && !isWarped(c->mov[i]->land) && c->mov[i]->land != laCrossroads4) 
            q++;
        if(q == 1) c->wall = waWarpGate;
        }
      
      if(c->land == laWarpSea) {
        c->wall = waSea;
        int q = 0;
        if(!purehepta && !chaosmode) for(int i=0; i<c->type; i++) 
          if(c->mov[i] && !isWarped(c->mov[i]->land)) q++;
        if(q == 1) c->wall = waWarpGate;
        }      
      }

    if(d == 8 && c->land == laKraken && pseudohept(c) && hrand(1000) <= 2) {
      c->wall = waNone;
      forCellEx(c2, c) c2->wall = waNone;
      }

    if(d == 8 && c->land == laOvergrown) {
      if(hrand(doCross ?450:15000) < 20 + (2 * items[itMutant] + hard) && !safety) {
        if(!peace::on) c->item = itMutant;
        c->landparam = items[itMutant] + 5 + hrand(11);
        c->wall = waNone;
        for(int i=0; i<c->type; i++) 
          if(c->mov[i] && (c->mov[i]->wall == waBigTree || c->mov[i]->wall == waSmallTree)) 
            c->mov[i]->wall = waNone;
        }
      else if(hrand(15000) < 20 + (2 * items[itMutant] + hard) && !safety) {
        // for the Yendor Challenge, use only Mutants
        if(!(yendor::on && yendor::clev().l == laMirror)) {
          c->monst = moForestTroll;
          c->wall = waNone;
          }
        }
      }
    
    if(d == 7 && c->item == itMutant) {
      if(c->monst != moMutant && isMultitile(c->monst))
        c->item = itNone;
      else {
        c->stuntime = mutantphase;
        c->monst = moMutant;
        c->mondir = NODIR;
        }
      }

    if(d == 8 && c->land == laCaribbean && !euclid) {
      int mindist  = 9;
      for(int i=0; i<c->type; i++) {
        cell *c2 = c->mov[i];
        if((euclid || c2->master->alt) && celldistAlt(c2) < mindist)
          mindist = celldistAlt(c2);
        }
      if(mindist == 0) beCIsland(c);
      }
                                       
    if(d == 7 && isHaunted(c->land)) {
      c->wall = waNone;

      c->item = itNone; c->monst = moNone;
      
      if(hrand(100) < 25)
        c->wall = hrand(2) ? waBigTree : waSmallTree;
        
      else if(hrand(500) < 2 && ishept(c))
        c->wall = hrand(10) < 3 ? waFreshGrave : waAncientGrave;
        
      else {
        if(hrand(5000) < 30)
          c->item = itGreenStone;
  
        if(hrand(4000) < 10 + items[itLotus] + hard && !safety) 
          c->monst = moGhost;
          
        int depth = getHauntedDepth(c);
        
        if(hrand(500 + depth) < depth - items[itLotus] && !safety && !peace::on)
          c->item = itLotus;
        }
      }
    
    if(d == 7 && c->land == laRedRock && c->wall == waNone)
      buildPrizeMirror(c, 1000);
    
    if(d == 8 && c->land == laRose && hrand(2000) < 100 && !c->wall && !c->item && !c->monst) {
      int nww = 0;
      for(int i=0; i<c->type; i++) {
        cell *c2 = c->mov[i];          
        if(c2 && c2->wall == waRose) nww++;
        for(int j=0; j<c2->type; j++) {
          cell *c3 = c2->mov[j];
          // note: c3->land is required for Android --
          // not strictly equivalent since another land there might be not yet generated
          if(c3 && c3->land != laRose && c3->land) nww++;
          }
        };

      if(nww == 0) {
        c->wall = waRose;
        c->monst = moNone;
        }
      }
    
    if(d == 7 && c->land == laCaribbean && c->wall == waSea) {
      bool coast = false;
      for(int i=0; i<c->type; i++) {
        cell *c2 = c->mov[i];
        if(passable(c2, c, 0) && c2->wall != waBoat) coast = true;
        }
      if(coast && hrand(10) < 5) {
        c->wall = waBoat;
        if(items[itPirate] >= 10 && hrand(100) < 2 && !safety && !peace::on && !inv::on)
          c->item = itOrbTime;
        else if(hrand(100) < 2*PRIZEMUL && !safety) 
          placePrizeOrb(c);
        }
      }

    if(d == 7 && c->land == laCaves && c->wall == waCavewall && hrand(5000) < items[itGold] + hard && !safety)
      c->monst = moSeep;

    if(d == 7 && c->land == laDogPlains && false) {
      if(hrand(1000) < 10) {
        if(openplains(c)) {
          c->item = itDogPlains;
          vector<cell*> next;
          forCellEx(c2, c) if(c2->mpdist > 7) next.push_back(c2);
          if(size(next) && items[itDogPlains] < 10) {
            cell *c3 = next[hrand(size(next))];
            forCellEx(c4, c3) if(c4->mpdist > 7 && !isNeighbor(c4, c))
              c4->monst = moHunterGuard;
            }
          }
        }
      if(hrand(5000) < items[itDogPlains]- 17 + hard)
        c->monst = moHunterDog;
      }
    
    if(d == 7 && c->land == laLivefjord && c->wall == waSea && hrand(5000) < 15 + items[itFjord] + hard && !safety) {
      if(items[itFjord] >= 5 && hrand(100) < 20 && !peace::on)
        c->monst = moWaterElemental;
      else {
        c->monst = moViking;
        c->wall = waBoat;
        }
      }
    
    else if(d == 7 && c->land == laLivefjord && yendor::path && c->wall == waSea && hrand(5000) < 20) {
      c->monst = moViking;
      c->wall = waBoat;
      }
    
    if(d == 7 && c->land == laLivefjord && hrand(2000) < PT(50 + kills[moViking], 100) && notDippingFor(itFjord) && !safety) {
      c->item = itFjord;
      }
    
    if(d == 7 && c->land == laLivefjord && items[itFjord] >= 10 && hrand(2000) < 2 && !peace::on && !inv::on)
      c->item = itOrbFish;
    
    if(d == 7 && c->land == laLivefjord && hrand(2000) < 2*PRIZEMUL)
      placePrizeOrb(c);
    
    if(d == 7 && c->land == laLivefjord)
      buildPrizeMirror(c, 1000);
    
    if(d == 7 && c->land == laEmerald && c->wall == waCavewall && hrand(5000) < items[itEmerald] + hard && !safety)
      c->monst = moSeep;
    
    if(d == 7 && c->land == laDeadCaves && c->wall == waDeadwall && hrand(1000) < items[itSilver] + hard && !safety)
      c->monst = moSeep;
    
    if(d == 7 && c->wall == waVinePlant && hrand(100) < (randomPatternsMode ? 2 : 10) && !peace::on)
      c->monst = moVineSpirit;

    if(d == 7 && c->land == laOcean && !safety) {
      bool placecolumn = false;
      if(c->landparam % TEMPLE_EACH == 0 && c->landparam <= 24) {
        int q = 0;
        forCellEx(c2, c)
          if(c2->landparam % TEMPLE_EACH == 0 && !ishept(c2)) q++;
        placecolumn = q == 2;
        if(placecolumn) {
          placecolumn = false;
          cell *c2 = c;
          seek:
          forCellEx(c3, c2) if(c3->landparam < c2->landparam) {
            c2 = c3; goto seek;
            }
          forCellEx(c3, c2) forCellEx(c4, c3)
            if(c4->barleft == laRlyeh || c4->barright == laRlyeh || c4->land == laRlyeh)
              placecolumn = true;
          }
        }

      if(placecolumn)
        c->wall = waColumn;
      else if((c->landparam >= 1 && c->landparam <= 25) || chaosmode) {
        if(hrand(1000) < 5)
          c->wall = waBoat;
        if(hrand(1000) < PT(50 + kills[moAlbatross]/2, 150) && !peace::on)
          c->item = itCoast;
        if(hrand(15000) < 10 + 2 * items[itCoast] + 2 * hard)
          c->monst = moAlbatross;
        if(items[itCoast] >= 10 && hrand(10000) < 5 && !peace::on && !inv::on)
          c->item = itOrbAir;
        else if(items[itCoast] >= 10 && hrand(10000) < 6 && !peace::on && !inv::on)
          c->item = itOrbEmpathy;
        if(hrand(10000) < 5*PRIZEMUL)
          placePrizeOrb(c);
        buildPrizeMirror(c, 2000);
        }
      else if(c->landparam > 25) {
        int amberbonus = items[itCoast] - 50;
        // 50 => 10
        if(hrand(15000) < 10 + amberbonus + 2 * hard)
          c->monst = moAlbatross;
        if(hrand(30000) < 20 + 2 * hard) {
          c->wall = waBoat;
          c->monst = moPirate;
          // orbs are possible!
          placeOceanOrbs(c);
          }
        else if(hrand(30000) < 10 + 2 * hard)
          c->monst = moCShark;
        }
      }

    if(d == 7 && c->land == laWarpSea && c->wall == waSea && !safety) {
      if(sphere && c->type != 6 && c->master->fiftyval == 6) {
        c->wall = waBoat;
        c->monst = moRatling;
        }
      if(hrand(12000) < 30 + 2 * items[itCoral] + hard) {
        c->wall = waBoat;
        c->monst = moRatling;
        }
      else if(hrand(6000) < PT(120 + min(kills[moRatling], 250), 240) && notDippingFor(itCoral)) {
        c->wall = waBoat;
        c->item = itCoral;
        }
      }

    if(d == 7 && c->land == laCaribbean && c->wall == waSea && hrand(10000) < 20 + items[itPirate] + 2 * hard && !safety)
      c->monst = moCShark;
    
    if(d == 7 && c->wall == waCTree && hrand(5000) < 100 + items[itPirate] + hard)
      c->monst = moParrot;
    
    // repair the buggy walls flowing in from another land, like ice walls flowing into the Caves
    if(d == 7 && c->land == laCaves && c->wall != waCavewall && c->wall != waCavefloor)
      c->wall = waCavefloor;
    
    if(d == 7 && c->land == laDeadCaves && c->wall != waDeadwall && 
      c->wall != waDeadfloor && c->wall != waDeadfloor2 && c->wall != waEarthD)
      c->wall = waDeadfloor2;
    
    if(d == 7 && c->land == laCocytus && c->wall != waFrozenLake && c->wall != waLake && c->wall != waIcewall)
      c->wall = waFrozenLake;
    
    if(d == 7 && c->land == laAlchemist && c->wall != waFloorA && c->wall != waFloorB)
      c->wall = waFloorA;
    
    if(d == 3 && c->land == laMinefield && safety && (c->wall == waMineMine || c->wall == waMineUnknown))
      c->wall = waMineOpen;

    if(c->land == laCamelot && tactic::on && d == 0) {
      int r = roundTableRadius(c);
      if(r > 28 + 3 * items[itHolyGrail])
        items[itHolyGrail]++;
      }
    
    if(d == 7 && c->land == laMinefield) {
      c->wall = waMineUnknown;
      // 250: rare mines
      // 1250: at 25
      int minefreq = 0;
      int treas = items[itBombEgg];
      // if(treas <= 10) minefreq = 250 + 30 * treas;
      if(treas <= 110) minefreq = 550 + 10 * (treas-10);
      else minefreq = 1550 + (treas - 110);
      
      // experimentation says that 600 is slightly too hard to find the Orb of Yendor
      if(yendor::generating || yendor::path)
        if(minefreq < 550)
          minefreq = 550;
      
      int tfreq = 
        treas < 10 ? 50 + 5 * treas :
        treas < 25 ? 100  + (treas-10) * 2:
        treas < 50 ? 150 + (treas-25) :
        175;

      if(hrand(5000) < minefreq)
        c->wall = waMineMine;
      else if(hrand(5000) < tfreq && !safety && !peace::on) {
        c->item = itBombEgg;
        c->landparam = items[itBombEgg] + 5 + hrand(11);
        }
      else if(hrand(5000) < treas - 20 + yendor::hardness() && !safety)
        c->monst = moBomberbird;
      else if(treas >= 10 && hrand(5000) < 10 && !safety && !peace::on && !inv::on)
        c->item = itOrbFriend;
      else if(hrand(5000) < 10*PRIZEMUL && !safety)
        placePrizeOrb(c);
      }

    if(d == 7 && c->wall == waIcewall && c->land != laIce && c->land != laCocytus && c->land != laBlizzard)
      c->wall = waNone;
    
    if(d == 7 && c->wall == waRed3 && c->land != laRedRock)
      c->wall = waNone;
    
    if(d == 7 && c->item == itRedGem && c->land != laRedRock)
      c->item = itNone;
    
    if(d == 7 && c->item == itRose && c->land != laRose)
      c->item = itNone;
    
    if(d == 7 && c->wall == waChasmD) {
      chasmify(c);
      }
      
    if(d == 7 && c->wall == waEarthD) {
      chasmifyEarth(c);
      }
    
    if(d == 7 && c->wall == waElementalD) {
      chasmifyElemental(c);
      }
    
    // seal entrances to the Land of Power.
    if(d == 7 && c->land == laPower && c->type == 7) {
      bool onwall = false;
      for(int i=0; i<7; i++) if(c->mov[i] && c->mov[i]->land == laBarrier)
        onwall = true;
      if(!onwall) for(int i=0; i<7; i++) {
        cell *c2 = c->mov[i];
        if(!c2) continue;
        cell *c3 = c2->mov[(c->spn(i) + 3) % 6];
        if(c3->land != laPower && c3->land != laBarrier)
        if(c2->wall != waFire && c2->wall != waGlass) {
          if(isFire(c)) c->monst = moWitchWinter;
          else if(c->wall == waGlass) c->monst = moWitchGhost;
          else c->monst = moEvilGolem;
          }
        }
      }

    if(d == 7 && passable(c, NULL, 0)) {
      if(c->land == laBarrier) c->wall = waBarrier;
      if(c->land == laOceanWall) 
        c->wall = c->type == 7 ? waBarrier : waSea;
      }

    if(d == 7 && c->land == laWhirlpool) 
      whirlpool::generate(c);
    
    if(c->land == laPalace && princess::generating) {
      // no Opening Plates nearby
      if(d <= 7 && c->wall == waOpenPlate && !purehepta)
        c->wall = waNone;
      if(d <= 7 && c->wall == waClosePlate && purehepta)
        c->wall = waOpenPlate;
      // no monsters nearby
      if(d>0) c->monst = moNone;
      // no Plates or Trapdoors in the Princess cell
      if(d < 3 && (c->wall == waClosePlate || c->wall == waOpenPlate || c->wall == waTrapdoor))
        c->wall = waNone;
      if(d > 1) c->item = itNone;
      // the Princess herself
      if(d == 0) {
        c->monst = moPrincess;
        c->hitpoints = palaceHP();
        c->wall = waGiantRug;
        cell *c2 = NULL;
        for(int i=0; i<c->type; i++) {
          cellwalker cw(c, i);
          cwstep(cw);
          cwspin(cw, 4);
          cwstep(cw);
          cwspin(cw, 2);
          cwstep(cw);
          cwspin(cw, 4);
          cwstep(cw);
          cwspin(cw, 2 + hrand(3));
          cwstep(cw);
          if(!c2) c2 = cw.c;
          else if(celldist(cw.c) > celldist(c2)) c2 = cw.c;
          cw.c->monst = moMouse;
          }
        c2->wall = waOpenPlate;
        }          
      }
    
    if(d == 7 && c->land == laMountain) {
      if(hrand(50000) < 100)
        buildIvy(c, 0, 3);
      else if(hrand(125000) < 100 - celldistAlt(c))
        c->monst = moMonkey;
      else if(hrand(200000) < min(100, -10*celldistAlt(c)) - celldistAlt(c))
        c->monst = moEagle;
      else if(hrand(100) < 5) 
        c->wall = waPlatform;
      else if(hrand(100) < 20)
        c->wall = waBigBush;
      else if(hrand(100) < 12)
        c->wall = waSmallBush;
      else if(hrand(500) < -celldistAlt(c) / 5 - items[itAmethyst])
        c->item = itAmethyst;
      }
    
    if(d == 7 && passable(c, NULL, 0) && !safety) {
      
      if(c->land == laIvoryTower) {
        if(hrand(20000) < 20 + items[itIvory] + hard) {
          if(cellEdgeUnstable(c))
            c->monst = moGargoyle;
          else
            c->monst = moFamiliar;
          }
        else if(c->landparam >= 14 && hrand(2000) < PT(50+kills[moGargoyle]+kills[moFamiliar], 150) && !cellEdgeUnstable(c) && !peace::on) {
          c->item = itIvory;
          }
        }
      
      if(c->land == laDungeon) {
        int lp = c->landparam * c->landparam;
        if(lp > 100) lp = 100;
        if(c->landparam >= 10 && hrand(20000) < 5*lp + items[itSlime] + hard && !cellEdgeUnstable(c) && !peace::on) {
          c->monst = moSkeleton, c->hitpoints = 3;
          }
        else if(c->landparam >= 10 && hrand(50000) < lp/2 + items[itSlime] + hard) {
          c->monst = moGhost;
          }
        else if(c->landparam >= 10 && hrand(50000) < 3*(100-lp) + 80 + items[itSlime]/3 + hard) {
          c->monst = moBat;
          }
        else if(c->landparam >= 15 && hrand(800) < min(PT(150 + 2 * kills[moBat], 250), 250) && !cellEdgeUnstable(c) && c->wall != waOpenGate) {
          c->item = itSlime;
          }
        }
      
      if(c->land == laEndorian) {
        if(c->wall == waNone && coastval(c, laEndorian) >= 10 && hrand(5000) < 10 + 2 * (items[itApple] + hard))
          c->monst = moSparrowhawk;
        else if(c->wall != waNone && hrand(5000) < 10 + 2 * (items[itApple] + hard))
          c->monst = moResearcher;
        else if(c->wall == waCanopy && !checkInTree(c, 3) && hrand(5000) < PT(300 + 5 * (kills[moSparrowhawk] + kills[moResearcher]), 750))
          c->item = itApple;
        }
    
      if(c->land == laIce) {
        if(hrand(5000) < PT(100 + 2 * (kills[moYeti] + kills[moWolf]), 200) && notDippingFor(itDiamond))
          c->item = itDiamond;
        if(hrand(8000) < 2 * (items[itDiamond] + hard))
          c->monst = hrand(2) ? moYeti : moWolf;
        }
      
      if(c->land == laBlizzard) {
        if(hrand(8000) < 10 + 2 * (items[itBlizzard] + hard))
          c->monst = pick(moVoidBeast, moIceGolem);
        }

      if(c->land == laVolcano) {
        if(hrand(5000) < PT(100 + 2 * kills[moLavaWolf], 200) && notDippingFor(itLavaLily))
          c->item = itLavaLily;
        if(hrand(8000) < (items[itLavaLily] + hard))
          c->monst = moLavaWolf;
        }

      if(c->land == laTerracotta) {
        bool nearwarrior = false;
        forCellEx(c2, c) if(c2->wall == waTerraWarrior) nearwarrior = true;
        if(nearwarrior && hrand(5000) < PT(130 + 2 * kills[moJiangshi], 260) && notDippingFor(itTerra))
          c->item = itTerra;
        if(hrand(20000) < 2 * (items[itTerra] + hard))
          c->monst = moJiangshi;
        }

      if(c->land == laTrollheim && !safety) {
        if(hrand(8000) < items[itTrollEgg] + hardness_empty())
          c->monst = pickTroll(c);
        }

      if(c->land == laTortoise) {
        if(hrand(4000) < (peace::on ? 750 : 50 + items[itBabyTortoise]*2 + hard * 6) && !safety) {
          c->monst = moTortoise;
          c->hitpoints = 3;
          }
        
        int chance = 50 + items[itBabyTortoise]*2;
        if(quickfind(laTortoise)) chance += 150;
        if((tactic::on || euclid || peace::on) && hrand(4000) < chance && !safety) {
          c->item = itBabyTortoise;
          tortoise::babymap[c] = getBits(c) ^ tortoise::getRandomBits();
          }
        }

      if(c->land == laPalace) {
        bool lookingForPrincess0 = !euclid && c->master->alt;
        bool lookingForPrincess = lookingForPrincess0 && !princess::challenge;
        int hardness = lookingForPrincess ? 5 : items[itPalace] + hard;
        
        if(hrand(5000) < PT(100 + 2 * (kills[moPalace] + kills[moFatGuard] + kills[moVizier] + kills[moSkeleton]), 200) && notDippingFor(itPalace) &&
          c->wall != waOpenGate && !lookingForPrincess0)
          c->item = itPalace;
        if(items[itPalace] >= 10 && hrand(5000) < 16 && c->wall != waOpenGate && !inv::on && !peace::on)
          c->item = hrand(100) < 80 ? itOrbFrog : itOrbDiscord;
        if(hrand(5000) < 20*PRIZEMUL && c->wall != waOpenGate)
          placePrizeOrb(c);
        if(c->wall == waNone) buildPrizeMirror(c, 250);
        if(c->land == laPalace && (euclid || c->master->alt) && celldistAlt(c) <= 150 && !(havewhat&HF_MOUSE) && !princess::generating &&
          princess::getPrisonInfo(c) &&
          (euclid || (princess::getPrisonInfo(c)->bestdist < 6 && princess::getPrisonInfo(c)->princess))) {
          c->monst = moMouse;
          if(!princess::squeaked) {
            addMessage(XLAT("You hear a distant squeak!"));
            playSound(c, "mousesqueak");
            drawBigFlash(c);
            princess::squeaked = true;
            }
/*          {
  cell *c2= c;          
  z:
  c2->item = itPirate;
  printf("AT %p\n", c2);
  for(int i=0; i<c2->type; i++) if(c2->mov[i] && c2->mov[i]->mpdist < c2->mpdist) {
    c2 = c2->mov[i];
    goto z;
    } 
          } */
          havewhat |= HF_MOUSE;
          }
        else if(hrand(15000) < 10 + hardness) {
          c->monst = moPalace;
          c->hitpoints = palaceHP();
          if(hrand(10 + items[itPalace] + yendor::hardness()) >= 14 && !lookingForPrincess)
            c->monst = moSkeleton;
          }
        else if(hrand(20000) < hardness) {
          c->monst = moFatGuard;
          c->hitpoints = palaceHP();
          }
        else if(hrand(20000) < hardness - 7) {
          c->monst = moVizier;
          c->hitpoints = palaceHP();
          }
        else if(princess::forceVizier && from->pathdist != INF) {
          c->monst = moVizier;
          c->hitpoints = palaceHP();
          princess::forceVizier = false;
          }
        }
      if(c->land == laCaves) {
        if(hrand(5000) < PT(100 + 2 * min(kills[moTroll] + kills[moGoblin], 150), 200) && notDippingFor(itGold))
          c->item = itGold;
        if(hrand(8000) < 10 + 2 * (items[itGold] + hard))
          c->monst = hrand(2) ? moTroll : moGoblin;
        }
      if(c->land == laLivefjord) {
        if(hrand(16000) < 10 + 2 * (items[itFjord] + hard) + (yendor::path ? 90:0))
          c->monst = moFjordTroll;
        }
      if(c->land == laDeadCaves) {
        if(hrand(5000) < PT(100 + 2 * (kills[moDarkTroll] + kills[moEarthElemental]), 200) && notDippingFor(itSilver))
          c->item = itSilver;
        if(hrand(16000) < (items[itSilver] + hard)) {
          c->monst = moEarthElemental;
          for(int i=0; i<c->type; i++) {
            cell *c2 = c->mov[i];
            earthFloor(c2);
            }
          for(int i=0; i<c->type; i++) if(c->mov[i]->mpdist < c->mpdist) c->mondir = i;
          chasmifyEarth(c); c->wall = waDeadfloor2;
          }
        else if(hrand(8000) < 60 + 8 * (items[itSilver] + hard)) {
          if(hrand(100) < 25) {
            }
          else c->monst = hrand(2) ? moDarkTroll : moGoblin;
          }
        }
      if(c->land == laDesert) {
        if(hrand(5000) < PT(100 + 2 * (kills[moWorm] + kills[moDesertman]), 200) && notDippingFor(itSpice))
          c->item = itSpice;
        if(hrand(8000) < 10 + 2 * (items[itSpice] + hard) && !c->monst)
          c->monst = (hrand(2) && !peace::on) ? moWorm : moDesertman,
          c->mondir = NODIR;
        }
      if(c->land == laRedRock) {
        if(hrand((doCross && celldist(c) <= 5) ?450:16000) < 30+items[itRedGem]+hard && !pseudohept(c) && !c->monst && !c->wall) {
          int i = -1;
          for(int t=0; t<6; t++) if(c->mov[t]->mpdist > c->mpdist && !pseudohept(c->mov[t]))
            i = t;
          if(i != -1 && !peace::on) {
            c->monst = moHexSnake;
            preventbarriers(c);
            int len = purehepta ? 2 : ROCKSNAKELENGTH;
            cell *c2 = c;
            vector<cell*> rocksnake;
            while(--len) {
              rocksnake.push_back(c2);
              preventbarriers(c2);
              c2->mondir = i;
              createMov(c2, i);
              int j = c2->spn(i);
              cell *c3 = c2->mov[i];
              if(c3->monst || c3->bardir != NODIR || c3->wall) break;
              c2 = c3;
              c2->monst = moHexSnakeTail;
              i = (j + (len%2 ? 2 : 4)) % 6;
              }
            if(size(rocksnake) < ROCKSNAKELENGTH/2 && !purehepta) {
              for(int i=0; i<size(rocksnake); i++) 
                rocksnake[i]->monst = moNone;
              }
            else c2->mondir = NODIR;
            }
          }
        else if(hrand(16000) < 50+items[itRedGem]+hard && (purehepta?hrand(10)<3:!ishept(c)) && !c->monst)
          c->monst = moRedTroll,
          c->mondir = NODIR;
        }
      if(c->land == laDragon) {
        // 40 is the usual rate of dragon generation
        int dchance = 40;
        // but it grows to 400 if no Dragons in sight, to make it faster
        if(cwt.c->land == laDragon && !(havewhat&HF_DRAGON))
          dchance = 400;
        // also, don't generate additional Dragons for newbies
        else if((havewhat&HF_DRAGON) && items[itDragon] < 10)
          dchance = 5; 
          
        if(hrand(150000) < dchance && !c->monst && (!c->wall || c->wall == waChasm) && !peace::on) {
          havewhat |= HF_DRAGON;
          // printf("dragon generated with dchance = %d\n", dchance);
          vector<int> possi;
          for(int t=0; t<6; t++) if(c->mov[t]->mpdist > c->mpdist) possi.push_back(t);
          if(size(possi)) {
            int i = possi[hrand(size(possi))];
            int dragonlength = 6 + items[itDragon] / 2;
            c->monst = moDragonHead; c->hitpoints = 1;
            preventbarriers(c);
            cell *c2 = c;
            int len = dragonlength;
            vector<cell*> dragon;
            while(--len) {
              dragon.push_back(c2);
              preventbarriers(c2);
              c2->mondir = i;
              createMov(c2, i);
              int j = c2->spn(i);
              cell *c3 = c2->mov[i];
              if(c3->monst || c3->bardir != NODIR || c3->wall || c3->mpdist <= 7) break;
              c2 = c3;
              c2->monst = moDragonTail;
              c2->hitpoints = 1;
              i = j + 2 + hrand(c2->type-3);
              i %= c2->type;
              }
            if(size(dragon) < 5 || size(dragon) < dragonlength / 2) {
              for(int i=0; i<size(dragon); i++) 
                dragon[i]->monst = moNone;
              }
            else c2->mondir = NODIR;
            }
          }
        if(!c->monst && !tactic::on && !yendor::on && !peace::on && !euclid && hrand(4000) < 10 && !safety) {
          c->item = itBabyTortoise;
          tortoise::babymap[c] = getBits(c) ^ tortoise::getRandomBits();
          }
        }
      if(c->land == laWineyard) {
        if(hrand(5000) < PT(100 + 2 * (kills[moVineBeast] + kills[moVineSpirit]), 200) && notDippingFor(itWine))
          c->item = itWine;
        if(hrand(8000) < 12 * (items[itWine] + hard))
          c->monst = moVineBeast;
        }
      if(c->land == laZebra) {
        if(c->wall == waNone && hrand(2500) < PT(100 + 2 * (kills[moOrangeDog]), 300) && notDippingFor(itZebra))
          c->item = itZebra;
        if(hrand(10000) < 40 + 2*(items[itZebra] + hard))
          c->monst = moOrangeDog;
        }
      if(isElemental(c->land) && c->land != laElementalWall) {
        eItem localshard = localshardof(c->land);
        int danger = 5 * items[localshard] * items[localshard];
        eMonster elof = elementalOf(c->land);
        int elkills = PT(kills[elof], 25);
        
        if(hrand(8000) < 12 + (items[itElemental] + danger + hard)) {
          c->monst = elof;
          if(c->land != laEAir) chasmifyElemental(c);
          c->wall = waNone;
          if(c->land == laEWater) c->wall = waSea;
          }
        else if(hrand(5000) < 100 + elkills*3 && notDippingFor(itElemental))
          c->item = localshard;
        else if(hrand(5000) < 10 && items[itElemental] >= 10 && !inv::on && !peace::on)
          c->item = itOrbSummon;
        else if(hrand(5000) < 10*PRIZEMUL)
          placePrizeOrb(c);
        }
      if(c->land == laEmerald) {
        if(hrand(purehepta?400:1000) < PT(100 + 2 * (kills[moMiner] + kills[moLancer] + kills[moFlailer]), 200) && notDippingFor(itEmerald)) {
          // do not destroy walls!
          bool ok = true;
          for(int i=0; i<c->type; i++) if(c->mov[i]->wall == waCavewall) ok = false;
          if(ok) c->item = itEmerald;
          }
        if(hrand(8000) < 50 + 10 * (items[itEmerald] + hard)) {
          static eMonster emeraldmonsters[4] = { moHedge, moLancer, moFlailer, moMiner };
          c->monst = emeraldmonsters[hrand(4)];
          }
        if(sphere && c->type != 6 && c->master->fiftyval == 5) {
          c->monst = moMiner;
          c->stuntime = 7;
          }
        }
      if(c->land == laBurial && !safety && !peace::on) {
        if(hrand(15000) < 5 + 3 * items[itBarrow] + 4 * hard)
          c->monst = moDraugr;
        else if(hrand(5000) < 20 + (quickfind(laBurial) ? 40 : 0))
          c->item = itOrbSword;
        }
      if(c->land == laJungle) {
        if(hrand(5000) < PT(25 + 2 * (kills[moIvyRoot] + kills[moMonkey]), 40) && notDippingFor(itRuby))
          c->item = itRuby;
        if(hrand(15000) < 5 + 1 * (items[itRuby] + hard))
          c->monst = moMonkey;
        else if(hrand(80000) < 5 + items[itRuby] + hard)
          c->monst = moEagle;
        else if(ishept(c) && c != currentmap->gamestart() && hrand(4000) < 300 + items[itRuby] && !c->monst) {
          int hardchance = items[itRuby] + hard;
          if(hardchance > 25) hardchance = 25;
          bool hardivy = hrand(100) < hardchance;
          if((hardivy ? buildIvy(c, 1, 9) : buildIvy(c, 0, c->type)) && !peace::on)
            c->item = itRuby;
          }
        }
      if(c->land == laWhirlwind) {
        if(hrand(4500) < items[itWindstone] + hard)
          c->monst = moWindCrow;
        if(hrand(doCross?3000:30000) < items[itWindstone] + hard - 5)
          c->monst = moAirElemental;
        }
    
      if(c->land == laStorms) {
        if(hrand(7500) < 25 + (items[itFulgurite] + hard))
          c->monst = (hrand(5) ? moMetalBeast : moMetalBeast2),
          c->hitpoints = 3;
        if(hrand(10000) < 20 + (items[itFulgurite] + hard) && !ishept(c)) {
          c->monst = moStormTroll;
          }
        }
      if(c->land == laWildWest) {
        if(hrand(25000) < 2 + (2 * items[itBounty] + hard) + (items[itRevolver] ? 150:0))
          c->monst = moOutlaw;
        if(hrand(1000) < PT(20 + kills[moOutlaw], 40))
          c->item = itRevolver;
        }
      if(c->land == laAlchemist) {
        if(hrand(5000) < PT(25 + min(kills[moSlime], 200), 100) && notDippingFor(itElixir))
          c->item = itElixir;
        else if(hrand(3500) < 10 + items[itElixir] + hard)
          c->monst = moSlime;
        }
      if(c->land == laRose) {
        if(hrand(2000) < PT(25 + min(kills[moFalsePrincess] + kills[moRoseBeauty] + kills[moRoseLady], 200), 100) && notDippingFor(itRose)) {
          for(int t=0; t<c->type; t++) if(c->mov[t] && c->mov[t]->wall == waRose)
            c->item = itRose;
          }
        else {
          int p = hrand(10000);
          if(p >= 10 + items[itRose] + hard) ;
          else if(p < 10) c->monst = moFalsePrincess;
          else if(p < 13) c->monst = moRoseBeauty;
          else if(p < 18) c->monst = moRoseLady;
          else if(p < 20) c->monst = moRoseBeauty;
          else if(p < 30) c->monst = moFalsePrincess;
          else if(p < 35) c->monst = moRoseLady;
          else if(p < 60) c->monst = moFalsePrincess;
          else if(p < 65) c->monst = moRoseBeauty;
          else c->monst = moFalsePrincess;
          }
        }
      if(c->land == laPower) {
        if(hrand(5000+50*items[itPower]) < 1200) {
          eItem powerorbs[6] = {
            itOrbFlash, itOrbSpeed, itOrbFire, itOrbWinter, itOrbAether, itOrbLife};
          c->item = powerorbs[hrand(6)];
          }
        else if(c->type == 6 && hrand(5000) < 10) 
          c->wall = hrand(2) ? waMirror : waCloud;
        else if(hrand(1000) < 10 + (items[itPower] ? 10:0) + (items[itPower] + hard))
          c->monst = eMonster(moWitch + hrand(NUMWITCH));
        }
      if(isCrossroads(c->land)) {
        if(purehepta && c->land == laCrossroads5 && hrand(100) < 60)
          c->wall = waBarrier;
        else if(c->type == 6 && !inv::on && items[itShard] >= 10 && hrand(8000) < 120*orbcrossfun(items[itShard]))
          c->wall = hrand(2) ? waMirror : waCloud;
        else if(c->type == 6 && hyperstonesUnlocked() && hrand(8000) < 100)
          c->wall = hrand(2) ? waMirror : waCloud;
        else if(c->type == 6 && tactic::on && isCrossroads(tactic::lasttactic) && hrand(8000) < 120)
          c->wall = hrand(2) ? waMirror : waCloud;
        else if(c->land == laCrossroads4 && hrand(24000) < 10 && tactic::on)
          c->wall = waRose;
        else {
          if(hyperstonesUnlocked() && hrand(25000) < min(PT(tkills(), 2000), 5000) && notDippingFor(itHyperstone))
            c->item = itHyperstone;
          if(hrand(4000) < items[itHyperstone] && !c->monst) {
            // only interesting monsters here!
            eMonster cm = crossroadsMonster();
            if(cm == moIvyRoot) buildIvy(c, 0, c->type);
            else c->monst = cm;
            if(cm == moWorm || cm == moTentacle)
              c->mondir = NODIR;
            c->hitpoints = palaceHP();
            }
          }
        }
      if(c->land == laMirrored || c->land == laMirrorWall || c->land == laMirrorWall2 ||
        c->land == laMirrored2)
        c->wall = waMirrorWall;
      if(c->land == laMirrorOld) {
        if((purehepta?pseudohept(c):!ishept(c)) && hrand(5000) < 120 && (peace::on || notDippingFor(itShard)))
          c->wall = hrand(2) ? waMirror : waCloud;
        else if(ishept(c) && hrand(5000) < 10 * PRIZEMUL)
          placePrizeOrb(c);
        else if(hrand(12000) < 8 + items[itShard] + hard)
          c->monst = moRanger;
        else if(hrand(60000) < 8 + items[itShard] + hard)
          c->monst = moEagle;
        }
      if(c->land == laMirror) {
        if((purehepta?pseudohept(c):!ishept(c)) && hrand(1250) < 120 && (peace::on || notDippingFor(itShard)))
          c->wall = hrand(2) ? waMirror : waCloud;
        else if(ishept(c) && hrand(5000) < 10 * PRIZEMUL)
          placePrizeOrb(c);
        else if(hrand(cwt.c->land == laMirror ? 600 : 2400) < 8 + items[itShard] + hard) {
          if(items[itShard] >= 5 && hrand(120) <= 20)
            c->monst = moMirrorSpirit;
          else
            c->monst = moNarciss;
          }
        }
      if(c->land == laGraveyard) {
        if(hrand(5000) < PT(30 + 4*kills[moZombie] + 6*kills[moNecromancer], 120) && notDippingFor(itBone))
          c->item = itBone;
        if(hrand(20000) < 10 + items[itBone] + hard) {
          eMonster grm[6] = { moZombie, moZombie, moZombie, moGhost, moGhost, moNecromancer};
          c->monst = grm[hrand(6)];
          }
        }
      if(c->land == laRlyeh) {
        if(hrand(5000) < PT(30 + 2 * (kills[moCultist] + kills[moTentacle] + kills[moPyroCultist]), 100) && notDippingFor(itStatue))
          c->item = itStatue;
        if(hrand(8000) < 5 + items[itStatue] + hard && !c->monst && !peace::on)
          c->monst = moTentacle, c->item = itStatue, c->mondir = NODIR;
        else if(hrand(12000) < 5 + items[itStatue] + hard)
          c->monst = hrand(3) ? ((hrand(40) < items[itStatue]-25) ? moCultistLeader : moCultist) : moPyroCultist;
        else if(hrand(8000) < 5 + items[itStatue] + hard && c->type == 6 && !(yendor::on && (yendor::clev().flags & YF_NEAR_TENT)) && celldist(c)>=3) {
          for(int t=0; t<c->type; t++) {
            if(c->mov[t] && c->mov[t]->monst == moNone && (c->wall == waNone || c->wall == waColumn))
              c->mov[t]->wall = ishept(c->mov[t]) ? waColumn : waNone;
            if(c->mov[t]->wall == waColumn)
              c->mov[t]->item = itNone;
            }
          if(buildIvy(c, 0, 3) && !peace::on) c->item = itStatue;
          }
        }
      if(c->land == laTemple) {
        // depth!
        int d = 
          chaosmode ? -15 * items[itGrimoire]:
          (euclid || c->master->alt) ? celldistAlt(c) : 10;
        // remember: d is negative
        if(chaosmode ? hrand(100) < 25 : d % TEMPLE_EACH == 0) {
          if(hrand(5000) < 20 - 2*d && !c->monst && !peace::on)
            c->monst = moTentacle, c->mondir = NODIR;
          }
        else {
          // int d0 = d % TEMPLE_EACH;
          // if(d0<0) d0=-d0;
          if(hrand(100) < (peace::on ? 15 : 30)) //  && d0 != 1 && d0 != TEMPLE_EACH-1)
            c->wall = waBigStatue;
          else if(hrand(20000) < -d)
            c->monst = hrand(3) ? moCultist : moPyroCultist;
          else if(hrand(100000) < -d)
            c->monst = moCultistLeader;
          else if(hrand(5000) < 250 && !peace::on)
            c->item = itGrimoire;
          else if(hrand(5000) < 10 && (chaosmode ? items[itGrimoire] >= 10 : -d > TEMPLE_EACH * 10) && !peace::on && !inv::on)
            c->item = itOrbDragon;
          }
        }
      if(c->land == laClearing) {
        clearing::generate(c);
        if(pseudohept(c)) {
          int d = -celldistAlt(c);
          if(hrand(2500) < items[itMutant2] + hard - 10)
            c->monst = moRedFox;
          else if(hrand(100 + d) < d)
            c->item = itMutant2;          
          }
        }

      if(c->land == laDryForest) {
        if(hrand(5000) < PT(100 + 2 * (kills[moFireFairy]*2 + kills[moHedge]), 160) && notDippingFor(itFernFlower))
          c->item = itFernFlower;
        if(hrand(4000) < 40 + items[itFernFlower] + hard)
          c->monst = moHedge;
        else if(hrand(8000) < 2 * items[itFernFlower] + hard && !peace::on)
          c->monst = moFireFairy;
        }
      if(c->land == laHell) {
        if(hrand(6000) < PT(120 + (kills[moLesser]), 240) && notDippingFor(itHell))
          c->item = itHell;
        if(hrand(8000) < 40 + items[itHell] * (chaosmode?4:1) + hard)
          c->monst = moLesser;
        else if(hrand(24000) < 40 + items[itHell] * (chaosmode?4:1) + hard)
          c->monst = moGreater;
        }
      if(c->land == laWarpCoast) {
        if(hrand(12000) < 20 + 2 * items[itCoral] + hard) {
          c->monst = moRatling;
          c->stuntime = hrand(2);
          }
        }
      if(c->land == laCocytus) {
        if(hrand(5000) < PT(100 + 2 * (kills[moShark] + kills[moGreaterShark] + kills[moCrystalSage]), 200) && notDippingFor(itSapphire))
          c->item = itSapphire;
        if(hrand(5000) < 2 * (items[itSapphire] + hard)) {
          eMonster ms[3] = { moYeti, moGreaterShark, moCrystalSage };
          c->monst = ms[hrand(3)];
          if(c->monst == moGreaterShark) c->wall = waLake;
          }
        }
      if(c->land == laMotion) {
        if(hrand(1500) < PT(30 + kills[moRunDog], 100) && notDippingFor(itFeather))
          c->item = itFeather;
        if(hrand(20000) < 25 + items[itFeather] + hard) {
          c->monst = moRunDog;
          // preset the movement direction
          // this will make the dog go in the direction of the center,
          // if the player is unreachable/invisible
          for(int d=0; d<c->type; d++) if(c->mov[d] == from) {
            c->mondir = (d+3) % c->type;
            }
          chasmify(c);
          c->wall = shmup::on ? waNone : waChasm;
          }
        }
      if(c->land == laHive) {
        if(isHive(c->land) && hrand(6000) < (hive::hivehard() - 15)) 
          c->monst = hive::randomHyperbug();

        /* if(hrand(1500) < 30 + (kills[moBug0] + kills[moBug1] + kills[moBug2]) && notDippingFor(itRoyalJelly))
          c->item = itRoyalJelly; */
        /* if(hrand(2000) < 2)
          c->monst = eMonster(moBug0 + hrand(3)); */
        }
      if(c->land == laCaribbean) {
        if(hrand(1500) < 4 && celldistAlt(c) <= -5 && peace::on)
          c->item = itCompass;
        if(hrand(16000) < 40 + (items[itPirate] + hard))
          c->monst = moPirate;
        }
      if(!c->item && c->wall != waCloud && c->wall != waMirror) {
        if(isCrossroads(c->land))
          placeCrossroadOrbs(c);
        else
          placeLocalOrbs(c);
        }
      }
    }

  if(purehepta && c->wall == waMirrorWall && c->land == laMirror)
    c->land = laMirrorWall; // , c->item = itPirate; // not really a proper bugfix

  if(d == 7) playSeenSound(c);
  
#if CAP_EDIT
  if(d >= 7 && mapeditor::whichPattern)
    mapeditor::applyModelcell(c);
#endif
  }
