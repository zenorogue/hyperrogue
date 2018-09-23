// Hyperbolic Rogue

// Copyright (C) 2011-2018 Zeno Rogue, see 'hyper.cpp' for details

namespace hr {

// land generation routines

bool safety = false;

eLand lastland;

int lastexplore;

bool randomPatternsMode = false;
int randompattern[landtypes];

int genrange_bonus = 0;

bool chaosUnlocked = false;
bool chaosAchieved = false;

void doOvergenerate() {
  for(int i=0; i<numplayers(); i++)
    setdist(playerpos(i), 7 - getDistLimit() - genrange_bonus, NULL);
  }

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

void buildRedWall(cell *c, int gemchance) {
  if(c->monst) return;
  int ki = PT(kills[moHexSnake] + kills[moRedTroll], 50);
  c->wall = waRed3;
  if(hrand(100+ki) < gemchance + ki)
    c->item = itRedGem;
  else if(gemchance) placeLocalSpecial(c, 8000, gemchance, gemchance);
  }

#define RANDPATC(c) (randpattern(c,randompattern[c->land]))
#define RANDPATCL(c, l) (randpattern(c,l))
#define RANDPAT (randpattern(c,randompattern[c->land]))
#define RANDPAT3(i) (randpatternMajority(c,i,RANDITER))
#define RANDPATV(x) (randpattern(c,randompattern[x]))

bool reptilecheat = false;

#define ONEMPTY if(d == 7 && passable(c, NULL, 0) && !safety)

bool blizzard_no_escape1(cell *c, manual_celllister &cl) {
  if(!cl.add(c)) return true;
  if(c->item == itOrbSafety)
    return false;
  forCellEx(c2, c) {
    if(c2->land == laBarrier)
      continue;
    if(c2->land != laBlizzard && passable(c2, NULL, 0))
      return false;
    if(!againstWind(c2, c) && !blizzard_no_escape1(c2, cl))
      return false;
    }
  return true;
  }

bool blizzard_no_escape(cell *c) {
  manual_celllister cl;
  return blizzard_no_escape1(c, cl);
  }

bool out_ruin(cell *c) {
  if(sphere)
    return getHemisphere(c, 0) > 0;
  else if(stdhyperbolic) {
    int cd = getCdata(c, 3);
    cd &= 31;
    return cd >= 16;
    }
  else 
    return windmap::at(c) >= 128;
  }

eMonster genRuinMonster(cell *c) {
  eMonster m = pick(moHexDemon, moHexDemon, moHexDemon, moAltDemon, moAltDemon, moMonk, moMonk, moSkeleton, moSkeleton, moCrusher);
  if(m == moHexDemon && pseudohept(c))
    return genRuinMonster(c);
  if(m == moMonk) {
    if(c->item) return genRuinMonster(c);
    forCellCM(c2, c) if(c2->item) return genRuinMonster(c);
    }
  return m;
  }

void start_brownian(cell *c, int d) {
  while(true) {
    if(c->wall == waBrownian) c->wall = waNone;
    c->landparam++;
    // snakepile(c, moRedTroll);
    // c->wall = waSea;
    c->bardir = NOBARRIERS; 
    if(d == 0) {
      if(c->mpdist >= BARLEV) {
        c->wall = waBrownian;
        return;
        }
      start_brownian(c, 1);
      d = 1;
      continue;
      }
    int q = 0;
    cell *good = NULL;
    forCellCM(c2, c)
      if(c2->mpdist > 7 && (c2->land == laBrownian || c2->land == laNone)) {
        q++;
        if(q==1 || hrand(q) == 0) good = c2;
        }
    if(!q) break;
    d++; if(d == 7) d = 0;
    c = good;
    }
  }

// the giant switch generating most of the lands...

void giantLandSwitch(cell *c, int d, cell *from) {
  switch(c->land) {

    case laPrairie: // -------------------------------------------------------------

      if(d == 7) {
        if(prairie::isriver(c)) {
          if(shmup::on) prairie::beaststogen.push_back(c); 
          else prairie::generateBeast(c);
          }
        else if(!prairie::nearriver(c) && c->LHU.fi.flowerdist > 7) {
          if(hrand(9000) < items[itGreenGrass] - (prairie::isriver(cwt.at) ? 40 : 0))
            c->monst = moGadfly;
          else buildPrizeMirror(c, 1000);
          }
        prairie::generateTreasure(c);
        }
      break;
    
    case laDungeon: // -------------------------------------------------------------
    case laIvoryTower:    
      dungeon::all(c, d);
      ONEMPTY {
        if(c->land == laIvoryTower) {
          if(hrand(20000) < 20 + items[itIvory] + yendor::hardness()) {
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
          if(c->landparam >= 10 && hrand(20000) < 5*lp + items[itSlime] + yendor::hardness() && !cellEdgeUnstable(c) && !peace::on) {
            c->monst = moSkeleton, c->hitpoints = 3;
            }
          else if(c->landparam >= 10 && hrand(50000) < lp/2 + items[itSlime] + yendor::hardness()) {
            c->monst = moGhost;
            }
          else if(c->landparam >= 10 && hrand(50000) < 3*(100-lp) + 80 + items[itSlime]/3 + yendor::hardness()) {
            c->monst = moBat;
            }
          else if(c->landparam >= 15 && hrand(800) < min(PT(150 + 2 * kills[moBat], 250), 250) && !cellEdgeUnstable(c) && c->wall != waOpenGate) {
            c->item = itSlime;
            }
          }
        }      
      break;
    
    case laPalace: // -------------------------------------------------------------
    
      if(hyperbolic_not37 || torus || S7 < 5 || archimedean) {
        if(d == 9) {
          int i = hrand(100);
          if(i < 10) 
            c->wall = waPalace;
          else if(i < 15) 
            c->wall = waClosePlate;
          else if(i < 20) 
            c->wall = waOpenPlate;
          else if(i < 25) 
            c->wall = waTrapdoor;
          else if(i < 30) 
            c->wall = waClosedGate;
          else if(i < 35) 
            c->wall = waOpenGate;
          }
        }
      else {

        if(d == 9) {
          cell *c2 = NONSTDVAR ? c->master->c7 : c;
          if(cdist50(c2) == 3 && polarb50(c2))
            c->wall = waPalace;
          }
      
        if(d == 8 && sphere) {
          int gs = getHemisphere(c,0);
          if(NONSTDVAR) {
            int v = 1;
            forCellEx(c2, c) if(getHemisphere(c2, 0) != gs)
              v = 2;
            if(v == 2)
              c->wall = pick(waClosedGate, waOpenGate, waPalace);
            else if(hrand(100) < 10)
              c->wall = pick(waClosePlate, waOpenPlate, waTrapdoor);
            }
          else 
          if(gs == 1)
            c->wall = waPalace;
          if(gs == 3)
            c->wall = PURE ? waOpenGate : waClosedGate;
          if(gs == 4 && hrand(100) < 40)
            c->wall = waClosePlate;
          if(gs == 6)
            c->wall = waOpenPlate;
          if(gs == -3)
            c->wall = pick(waClosePlate, waOpenPlate);
          if(gs == -6)
            c->wall = waTrapdoor;
          }
    
        if(d == 8 && !sphere) {
        
          // note: Princess Challenge brings back the normal Palace generation
          bool lookingForPrincess = !euclid && c->master->alt && !princess::challenge;
          
          bool pgate = false;
          if(PURE) {
            int i = fiftyval049(c->master->c7);
            if(i >= 8 && i <= 14 && !polarb50(c->master->c7)) pgate = true;
            if(GOLDBERG) {
              bool good = false;
              forCellEx(c2, c) {
                int i2 = fiftyval049(c2->master->c7);
                if((i2 < 8) && polarb50(c2->master->c7)) good = true;
                }
              pgate = pgate && good;
              }
            }
          
          if(pgate) {
            switch(princess::generating ? 0 : hrand(2)) {
              case 0: 
                c->wall = waClosedGate;
                toggleGates(c, waClosePlate, 1);
                break;
              case 1:
                c->wall = waOpenGate;
                toggleGates(c, waOpenPlate, 1);
                break;
              }
            }
          else if(cdist50(c) == 3 && polarb50(c) && !ishept(c)) {
            if(GOLDBERG) ; 
            else {
              int q = 0, s = 0;
              if(!ishept(c)) for(int i=0; i<c->type; i++)
                if(cdist50(c->move(i)) == 3 && polarb50(c->move(i)) && !ishept(c->move(i)))
                  q++, s += i;
              if(q == 1 && c->move(s)->land == laPalace) {
                switch(princess::generating ? 0 : hrand(2)) {
                  case 0: 
                    c->wall = waClosedGate;
                    c->move(s)->wall = waClosedGate;
                    break;
                  case 1:
                    c->wall = waOpenGate;
                    c->move(s)->wall = waOpenGate;
                    break;
                  }
                }
              }
            }
          else if(c->wall == waPalace) ;
          else if((hrand(100) < (lookingForPrincess ? (PURE ? 11 : 7) : 5) && cdist50(c)) ||
            (cdist50(c) == 0 && polarb50(c) && hrand(100) < 60)) {
            c->wall = hrand(100) < (lookingForPrincess ? (PURE ? 25 : 30):50) ? waClosePlate : waOpenPlate;
            }
          else if(hrand(100) < (lookingForPrincess ? 3 : 5))
            c->wall = waTrapdoor;
          
          if(cdist50(c) == 0 && yendor::path) {
            cell *c2 = c->move(hrand(c->type));
            if(c2->wall == waNone) c2->wall = waTrapdoor;
            }
    
          if((c->wall == waClosePlate || c->wall == waTrapdoor) && peace::on) c->wall = waNone;
          }
  
        if(princess::generating) {
          // no Opening Plates nearby
          if(d <= 7 && c->wall == waOpenPlate && BITRUNCATED)
            c->wall = waNone;
          if(d <= 7 && c->wall == waClosePlate && PURE)
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
              cw = cw + wstep + 4 + wstep + 2 + wstep + 4 + wstep + (2 + hrand(3)) + wstep;
              if(!c2) c2 = cw.at;
              else if(celldist(cw.at) > celldist(c2)) c2 = cw.at;
              cw.at->monst = moMouse;
              }
            c2->wall = waOpenPlate;
            }
          }
        }
      
      ONEMPTY {
        bool lookingForPrincess0 = !euclid && c->master->alt;
        bool lookingForPrincess = lookingForPrincess0 && !princess::challenge;
        int hardness = lookingForPrincess ? 5 : items[itPalace] + yendor::hardness();
        
        if(hrand(5000) < PT(100 + 2 * (kills[moPalace] + kills[moFatGuard] + kills[moVizier] + kills[moSkeleton]), 200) && notDippingFor(itPalace) &&
          c->wall != waOpenGate && !lookingForPrincess0)
          c->item = itPalace;
        if(items[itPalace] >= treasureForLocal() && hrand(5000) < 16 && c->wall != waOpenGate && !inv::on && !peace::on)
          c->item = hrand(100) < 80 ? itOrbFrog : itOrbDiscord;
        if(hrand(5000) < 20*PRIZEMUL && c->wall != waOpenGate)
          placePrizeOrb(c);
        if(c->wall == waNone) buildPrizeMirror(c, 250);
        if(c->land == laPalace && (eubinary || c->master->alt) && celldistAlt(c) <= 150 && !(havewhat&HF_MOUSE) && !princess::generating &&
          princess::getPrisonInfo(c) &&
          (eubinary || (princess::getPrisonInfo(c)->bestdist < 6 && princess::getPrisonInfo(c)->princess))) {
          c->monst = moMouse;
          if(!princess::squeaked) {
            addMessage(XLAT("You hear a distant squeak!"));
            playSound(c, "mousesqueak");
            drawBigFlash(c);
            princess::squeaked = true;
            }
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
        else if(princess::forceVizier && hrand(100) < 10 && canReachPlayer(c, moVizier)) {
          c->monst = moVizier;
          c->hitpoints = palaceHP();
          princess::forceVizier = false;
          }
        }
      break;
  
    case laEmerald: // -------------------------------------------------------------

      if(d==8) {
        if(randomPatternsMode)
          c->wall = RANDPAT3(0) ? waCavewall : waCavefloor;
        else if(torus) {
          c->wall = waCavefloor;
          }
        else if(euclid) {
          int x, y;
          tie(x,y) = cell_to_pair(c);
          if(((y-2)&7) < 4) c->wall = waCavewall;
          else c->wall = waCavefloor;
          }
        else if(sphere) {
          if(getHemisphere(c, 0) < 0) 
            c->wall = waCavewall;
          else c->wall = waCavefloor;
          }
        else if(a4 || archimedean)
          c->wall = hrand(100) < 50 ? waCavefloor : waCavewall;
        else if(!BITRUNCATED) {
          if(polarb50(c)) 
            c->wall = waCavewall;
          else c->wall = waCavefloor;
          }
        else {
          int v = emeraldval(c);
          if(v == 0)
            c->wall = waStone;
          else if((v&3) >= 2) 
            c->wall = waCavewall;
          else c->wall = waCavefloor;
          }
        }

      if(d == 7 && c->wall == waCavewall && hrand(5000) < items[itEmerald] + yendor::hardness() && !safety)
        c->monst = moSeep;
    
      ONEMPTY {
        if(hrand(PURE?400:1000) < PT(100 + 2 * (kills[moMiner] + kills[moLancer] + kills[moFlailer]), 200) && notDippingFor(itEmerald)) {
          // do not destroy walls!
          bool ok = true;
          for(int i=0; i<c->type; i++) if(c->move(i)->wall == waCavewall) ok = false;
          if(ok) c->item = itEmerald;
          }
        if(hrand(8000) < 50 + 10 * (items[itEmerald] + yendor::hardness())) {
          static eMonster emeraldmonsters[4] = { moHedge, moLancer, moFlailer, moMiner };
          c->monst = emeraldmonsters[hrand(4)];
          }
        if(sphere && c->type != 6 && c->master->fiftyval == 5) {
          c->monst = moMiner;
          c->stuntime = 7;
          }
        }
      break;
    
    case laDryForest:
      if(d == 9) {
        if(randomPatternsMode)
          c->wall = RANDPAT ? waNone : RANDPATV(laHell) ? waBigTree : waSmallTree;
        else
          c->wall = 
            (hrand(100) < 50) ? (hrand(100) < 50 ? waBigTree : waSmallTree) : waNone;
        }
      if(d == 8) c->landparam = 0;
      ONEMPTY {
        if(hrand(5000) < PT(100 + 2 * (kills[moFireFairy]*2 + kills[moHedge]), 160) && notDippingFor(itFernFlower))
          c->item = itFernFlower;
        if(hrand(4000) < 40 + items[itFernFlower] + yendor::hardness())
          c->monst = moHedge;
        else if(hrand(8000) < 2 * items[itFernFlower] + yendor::hardness() && !peace::on)
          c->monst = moFireFairy;
        }
      break;    

    case laPower:    
      if(d==8) {
        int v;
        if(randomPatternsMode)
          v = RANDPAT ? 24 : 1;
        else if(sphere) {
          int gs = getHemisphere(c, 0);
          if(GOLDBERG) {
            v = 6;
            if(gs == 0) v = 24;
            forCellEx(c2, c) if(getHemisphere(c2, 0) * gs < 0)
              v = 24;
            }
          else if(gs == -3 || gs == -1 || gs == 1 || gs == 3)
            v = 24;
          else
            v = 6;
          }
        else if(archimedean && arcm::current.have_line)
          v = arcm::linespattern(c) ? 24 : 16;
        else if(torus || hyperbolic_not37 || quotient || archimedean) {
          v = hrand(100) < 25 ? 24 : 16;
          }
        else if(euclid) {
          int x, y;
          tie(x,y) = cell_to_pair(c);
          int y0 = gmod(y, 6);
          if(y0 == 3 || y0 == 4) v=24; else v=0;
          }
        else v = emeraldval(c);
        if(v == 0 && !euclid) c->wall = waStone;
        else {
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
        }
      // seal entrances to the Land of Power.
      if(d == 7 && ctof(c)) {
        bool onwall = false;
        for(int i=0; i<c->type; i++) if(c->move(i) && c->move(i)->land == laBarrier)
          onwall = true;
        if(!onwall) for(int i=0; i<c->type; i++) {
          cell *c2 = c->move(i);
          if(!c2) continue;
          cell *c3 = c2->modmove(c->c.spin(i) + 3);
          if(c3->land != laPower && c3->land != laBarrier)
          if(c2->wall != waFire && c2->wall != waGlass) {
            if(isFire(c)) c->monst = moWitchWinter;
            else if(c->wall == waGlass) c->monst = moWitchGhost;
            else c->monst = moEvilGolem;
            }
          }
        }
      ONEMPTY {
        if(hrand(5000+50*items[itPower]) < 1200) {
          eItem powerorbs[6] = {
            itOrbFlash, itOrbSpeed, itOrbFire, itOrbWinter, itOrbAether, itOrbLife};
          c->item = powerorbs[hrand(6)];
          }
        else if(hrand(5000) < 10 && mirror::build(c)) ;
        else if(hrand(1000) < 10 + (items[itPower] ? 10:0) + (items[itPower] + yendor::hardness()))
          c->monst = eMonster(moWitch + hrand(NUMWITCH));
        }
      break;
    
    case laZebra:
      if(d==8) {
        if(torus) ;
        else if(archimedean && arcm::current.have_line)
          c->wall = arcm::linespattern(c) ? waTrapdoor : waNone;        
        else if(euclid && !archimedean) {
          int x,y;
          tie(x,y) = cell_to_pair(c);
          if(y&1) c->wall = waTrapdoor;
          else c->wall = waNone;
          }
        else 
          if(archimedean) c->wall = hrand(2) ? waTrapdoor : waNone;
        else
          c->wall = (randomPatternsMode ? RANDPAT : (zebra40(c)&2)) ? waTrapdoor : waNone;
        }
      ONEMPTY {
        if(c->wall == waNone && hrand(2500) < PT(100 + 2 * (kills[moOrangeDog]), 300) && notDippingFor(itZebra))
          c->item = itZebra;
        if(hrand(10000) < 40 + 2*(items[itZebra] + yendor::hardness()))
          c->monst = moOrangeDog;
        }
      break;
    
    case laWineyard:
      if(d==8) {
        if(torus) ;
        else if(archimedean && arcm::current.have_line)
          c->wall = arcm::linespattern(c) ? waVinePlant : waNone;
        else if(euclid && !archimedean) {
          int x,y;
          tie(x,y) = cell_to_pair(c);
          int dy = gmod(y, 3);
          if(dy == 1) c->wall = waVinePlant;
          }
        else if(a4 || sphere || archimedean)
          c->wall = hrand(100) < 50 ? waNone : waVinePlant;
        else {
          int v = emeraldval(c);
          if(v == 0) c->wall = waStone;
          else {
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
            if(NONSTDVAR && cellHalfvine(c)) {
              c->wall = waNone;
              forCellCM(c2, c) if(emeraldval(c2) == (v^1))
                c->wall = waVinePlant;
              }
            }
          }
        }
      if(d == 7 && c->wall == waVinePlant && hrand(100) < (randomPatternsMode ? 2 : 10) && !peace::on)
        c->monst = moVineSpirit;
      ONEMPTY {
        if(hrand(5000) < PT(100 + 2 * (kills[moVineBeast] + kills[moVineSpirit]), 200) && notDippingFor(itWine))
          c->item = itWine;
        if(hrand(8000) < 12 * (items[itWine] + yendor::hardness()))
          c->monst = moVineBeast;
        }
      break;
    
    case laReptile:
      if(d == 9) {
        if(randomPatternsMode) {
          int ch =  hrand(200);
          if(RANDPAT) c->wall = waChasm;
          else c->wall = ch < (50 + items[itDodeca] + yendor::hardness()) ? waReptile : waNone;
          c->wparam = 1 + hrand(reptilemax());
          if(c->wall == waNone && hrand(5000) < 25)
            c->item = itDodeca;
          }
        else {
          int i = archimedean ? hrand(50) : zebra40(c);
          if(i < 40) {
            int cd = hyperbolic_37 ? getCdata(c, 3) : hrand(16);
            cd &= 15;
            if(cd >= 4 && cd < 12) c->wall = waChasm;
            else {
              int ch =  hrand(200);
              c->wall = ch < (50 + items[itDodeca] + yendor::hardness()) ? waReptile : 
               waNone;
              }
            c->wparam = 1 + hrand(reptilemax());
            }
          if(reptilecheat) {
            c->wall = waReptile; c->wparam = 100;
            }
          }
        }
      if(d == 7 && c->wall != waChasm) {
        bool nonchasm = false;
        forCellEx(c2, c) if(c2->wall != waChasm) nonchasm = true;
        if(!nonchasm) c->item = itDodeca;
        }        
      break;
    
    case laBurial:
      if(d == 9) {
        if(hrand(5000) < 25 && celldist(c) >= 5 && !safety) {
          bool goodland = true;
          cell *c2 = createMov(c, hrand(c->type));
          for(auto cx: {c, c2})
            forCellCM(c3,cx) {
              if(c3->land != laNone && c3->land != laBurial)
                goodland = false;
              if(c3->bardir != NODIR && c3->bardir != NOBARRIERS) 
                goodland = false;
              }
          if(goodland) {
            c->item = itBarrow;
            c->landparam = 2 + hrand(2);
            c->wall = waBarrowDig;
            forCellCM(c3, c) 
              c3->wall = waBarrowWall, c3->item = itNone,
              c3->bardir = NOBARRIERS;
            c2->wall = waBarrowDig;
            forCellCM(c3, c2) {
              if(c3 == c || isNeighbor(c3, c)) continue;
              c3->bardir = NOBARRIERS;
              bool adj = false;
              forCellEx(c4, c)
                if(c4 != c2 && isNeighborCM(c3, c4)) adj = true;
              if(adj)
                c3->wall = waBarrowDig;
              else
                c3->wall = waBarrowWall, c3->item = itNone;
              }
            }
          }
        }
      ONEMPTY if(!peace::on) {
        if(hrand(15000) < 5 + 3 * items[itBarrow] + 4 * yendor::hardness())
          c->monst = moDraugr;
        else if(hrand(5000) < 20 + (quickfind(laBurial) ? 40 : 0))
          c->item = itOrbSword;
        }
      break;
    
    case laTrollheim:
      if(d == 9) {
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
      if(d == 7 && !c->monst && !c->item && celldist(c) >= 3) {
        int cd = getCdata(c, 3);
        cd %= 16;
        if(cd<0) cd += 16;
        if(cd >= 4 && cd < 10 && hrand(100) < 40)
          dieTroll(c, pickTroll(c));
        }
      ONEMPTY {
        if(hrand(8000) < items[itTrollEgg] + hardness_empty())
          c->monst = pickTroll(c);
        }
      break;
    
    case laIce:
      if(d == 9) {
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
              setdist(c->move(i), d+2, c);
              if(c3->wall == waBarrier || c3->land != laIce) continue;
              c3->wall = waIcewall;
              }
            }
          }
        if(c->wall == waIcewall && items[itDiamond] >= 5 && hrand(200) == 1)
          c->wall = waBonfireOff;
        }
      if(d == 8) c->landparam = 0;
      ONEMPTY {
        if(hrand(5000) < PT(100 + 2 * (kills[moYeti] + kills[moWolf]), 200) && notDippingFor(itDiamond))
          c->item = itDiamond;
        if(hrand(8000) < 2 * (items[itDiamond] + yendor::hardness()))
          c->monst = hrand(2) ? moYeti : moWolf;
        }      
      break;
    
    case laSwitch:
      ONEMPTY {
        if(hrand(5000) < PT(100 + 2 * (kills[moSwitch1] + kills[moSwitch2]), 200) && notDippingFor(itSwitch))
          c->item = itSwitch;
        if(hrand(8000) < 40 + 5 * (items[itSwitch] + yendor::hardness()))
          c->monst = hrand(2) ? moSwitch1 : moSwitch2;
        }      
      break;
    
    case laDragon:
      if(d == 9) {
        if(randomPatternsMode) {
          if(RANDPAT) c->wall = waChasm;
          }
        else {
          int cd = getCdata(c, 3);
          cd &= 31;
          if(cd >= 6 && cd<12) c->wall = waChasm;
          else if(hrand(1000) == 0) c->wall = waEternalFire;
          }
        }
      ONEMPTY {
        // 40 is the usual rate of dragon generation
        int dchance = 40;
        // but it grows to 400 if no Dragons in sight, to make it faster
        if(cwt.at->land == laDragon && !(havewhat&HF_DRAGON))
          dchance = 400;
        // also, don't generate additional Dragons for newbies
        else if((havewhat&HF_DRAGON) && items[itDragon] < 10)
          dchance = 5; 
          
        if(hrand(150000) < dchance && !c->monst && (!c->wall || c->wall == waChasm) && !peace::on) {
          havewhat |= HF_DRAGON;
          // printf("dragon generated with dchance = %d\n", dchance);
          vector<int> possi;
          for(int t=0; t<c->type; t++) if(c->move(t)->mpdist > c->mpdist) possi.push_back(t);
          if(isize(possi)) {
            int i = possi[hrand(isize(possi))];
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
              int j = c2->c.spin(i);
              cell *c3 = c2->move(i);
              if(c3->monst || c3->bardir != NODIR || c3->wall || c3->mpdist <= 7) break;
              c2 = c3;
              c2->monst = moDragonTail;
              c2->hitpoints = 1;
              i = j + (c2->type <= 3 ? 1 : 2 + hrand(c2->type-3));
              i %= c2->type;
              }
            if(isize(dragon) < 5 || isize(dragon) < dragonlength / 2) {
              for(int i=0; i<isize(dragon); i++) 
                dragon[i]->monst = moNone;
              }
            else c2->mondir = NODIR;
            }
          }
        if(!c->monst && !tactic::on && !yendor::on && !randomPatternsMode && !peace::on && !euclid && hrand(4000) < 10 && !safety) {
          c->item = itBabyTortoise;
          tortoise::babymap[c] = getBits(c) ^ tortoise::getRandomBits();
          }
        }
      break;
    
    case laCaves:
      if(d == 9)
        c->wall = (randomPatternsMode ? RANDPAT3(1) : hrand(100) < 55) ? waCavewall : waCavefloor;
      if(d == 7 && c->wall == waCavewall && hrand(5000) < items[itGold] + yendor::hardness() && !safety)
        c->monst = moSeep;
      ONEMPTY {
        if(hrand(5000) < PT(100 + 2 * min(kills[moTroll] + kills[moGoblin], 150), 200) && notDippingFor(itGold))
          c->item = itGold;
        if(hrand(8000) < 10 + 2 * (items[itGold] + yendor::hardness()))
          c->monst = hrand(2) ? moTroll : moGoblin;
        }
      break;
    
    case laCA:
      if(d == 9)
        c->wall = (hrand(1000000) < ca::prob * 1000000) ? waFloorA : waNone;
      break;
    
    case laLivefjord:
      if(d == 9) {
        int die = (randomPatternsMode ? (RANDPAT3(2)?100:0) : hrand(100));
        if(die < 50)
          c->wall = waSea;
        else
          c->wall = waNone;
        }

      if(d == 7) {
        if(c->wall == waSea && hrand(5000) < 15 + items[itFjord] + yendor::hardness() && !safety) {
          if(items[itFjord] >= 5 && hrand(100) < 20 && !peace::on)
            c->monst = moWaterElemental;
          else {
            c->monst = moViking;
            c->wall = waBoat;
            }
          }
      
        else if(yendor::path && c->wall == waSea && hrand(5000) < 20) {
          c->monst = moViking;
          c->wall = waBoat;
          }

        if(hrand(2000) < PT(50 + kills[moViking], 100) && notDippingFor(itFjord) && !safety) {
          c->item = itFjord;
          }
        
        placeLocalSpecial(c, 1000);
        
        buildPrizeMirror(c, 1000);      
        }
      
      ONEMPTY {
        if(hrand(16000) < 10 + 2 * (items[itFjord] + yendor::hardness()) + (yendor::path ? 90:0))
          c->monst = moFjordTroll;
        }
      break;
    
    case laDeadCaves:
      if(d == 9) {
        int die = (randomPatternsMode ? (RANDPAT?100:0) : hrand(100));
        if(die<50) c->wall = waDeadwall;
        else if(die<55) c->wall = waDeadfloor2;
        else c->wall = waDeadfloor;
        }

      if(d == 7 && c->wall == waDeadwall && hrand(1000) < items[itSilver] + yendor::hardness() && !safety)
        c->monst = moSeep;
    
      ONEMPTY {
        if(hrand(5000) < PT(100 + 2 * (kills[moDarkTroll] + kills[moEarthElemental]), 200) && notDippingFor(itSilver))
          c->item = itSilver;
        if(hrand(16000) < (items[itSilver] + yendor::hardness())) {
          c->monst = moEarthElemental;
          for(int i=0; i<c->type; i++) {
            cell *c2 = c->move(i);
            earthFloor(c2);
            }
          for(int i=0; i<c->type; i++) if(c->move(i)->mpdist < c->mpdist) c->mondir = i;
          chasmifyEarth(c); c->wall = waDeadfloor2;
          }
        else if(hrand(8000) < 60 + 8 * (items[itSilver] + yendor::hardness())) {
          if(hrand(100) < 25) {
            }
          else c->monst = hrand(2) ? moDarkTroll : moGoblin;
          }
        }
      break;
    
    case laAlchemist:
      if(d == 9) 
        c->wall = (randomPatternsMode ? RANDPAT : hrand(2)) ? waFloorA : waFloorB;
      ONEMPTY {
        if(hrand(5000) < PT(25 + min(kills[moSlime], 200), 100) && notDippingFor(itElixir))
          c->item = itElixir;
        else if(hrand(3500) < 10 + items[itElixir] + yendor::hardness())
          c->monst = moSlime;
        }
      break;
    
    case laVolcano:
      if(d == 9) {
        c->wall = waNone;
        if(hrand(20000) < (items[itLavaLily] + yendor::hardness()))
          c->monst = moSalamander,
          c->hitpoints = 3;
        if(hrand(8000) < PT(100 + 2 * kills[moLavaWolf], 200) && notDippingFor(itLavaLily))
          c->item = itLavaLily;
        else placeLocalSpecial(c, 1000, 2, 10);
        // prize orbs get a bonus, because most of them are not allowed anyway
        }
      ONEMPTY {
        if(hrand(8000) < (items[itLavaLily] + yendor::hardness()))
          c->monst = moLavaWolf;
        }
      break;
        
    case laBlizzard:
      if(d == 9) {
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
          else placeLocalSpecial(c, 100);
          }
        }
      if(d == 8) c->landparam = 0;
      ONEMPTY {
        if(hrand(8000) < 10 + (items[itBlizzard] + yendor::hardness()))
          c->monst = pick(moVoidBeast, moIceGolem);
        }
      if((d == 7 || d == 6) && blizzard_no_escape(c))
        c->item = itOrbSafety, c->monst = moNone;

      break;
    
    case laTerracotta: 
      if(d == 9) {
        if(hrand(500) < 15) {
          cellwalker cw(c, hrand(c->type));
          cell* cc[5];
          cc[2] = c;
          cellwalker cw2 = cw;
          cw += wstep; cc[3] = cw.at; cw += revstep; cc[4] = cw.at;
          cw2 += revstep; cc[1] = cw2.at; cw2 += revstep; cc[0] = cw2.at;
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
          if(hrand(100) < items[itTerra]-10)
            c->landparam--;
          if(hrand(100) < items[itTerra]-10)
            c->landparam--;
          }
        }
      if(d == 7) {
        int nearriver = 0;
        forCellEx(c2, c) 
          if(c2->wall == waMercury && pseudohept(c2)) 
            nearriver++;
        if(nearriver == 2) c->land = laMercuryRiver;
        }
      ONEMPTY {
        bool nearwarrior = false;
        forCellEx(c2, c) if(c2->wall == waTerraWarrior) nearwarrior = true;
        if(nearwarrior && hrand(5000) < PT(130 + 2 * kills[moJiangshi], 260) && notDippingFor(itTerra))
          c->item = itTerra;
        int t = 2 * (items[itTerra] + yendor::hardness());
        if(t < 60) t += (60-t) * (60 - t) / 120;
        if(hrand(20000) < t)
          c->monst = moJiangshi;
        }
      break;
    
    case laOvergrown:
      if(d == 9) {
        // 0: 60%
        // 10: 50%
        if(randomPatternsMode)
          c->wall = RANDPAT ? waNone : RANDPATV(laWineyard) ? waBigTree : waSmallTree;
        else
          c->wall = 
            (hrand(50+items[itMutant]/2+yendor::hardness()) < 30) ? (hrand(100) < 50 ? waBigTree : waSmallTree) : waNone;
        }
      if(d == 8) {
        if(hrand(doCross ?450:15000) < 20 + (2 * items[itMutant] + yendor::hardness()) && !safety) {
          if(!peace::on) c->item = itMutant;
          c->landparam = items[itMutant] + 5 + hrand(11);
          c->wall = waNone;
          for(int i=0; i<c->type; i++) 
            if(c->move(i) && (c->move(i)->wall == waBigTree || c->move(i)->wall == waSmallTree)) 
              c->move(i)->wall = waNone;
          }
        else if(hrand(15000) < 20 + (2 * items[itMutant] + yendor::hardness()) && !safety) {
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
      break;
    
    case laHalloween:
      if(d == 9) {
        if(GOLDBERG) {
          int fv = c->master->fiftyval;
          if(fv == 1 || fv == 4 || fv == 10) 
            c->wall = waChasm;
          if(c == c->master->c7 && fv == 3)
            c->item = itTreat;
          }
        else if(!BITRUNCATED && !euclid) {
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
        if(torus) {
          int i = hrand(100);
          if(i == 0) c->item = itTreat;
          else if(i < 5) c->wall = waChasm;
          }
        }
      break;
    
    case laWildWest:
      if(d == 9) {
        if(randomPatternsMode)
          c->wall = RANDPAT ? waNone : waSaloon;
        else if(hyperbolic_37 ? cdist50(c) <= 2 : hrand(100) < 20) c->wall = waSaloon;
        }
      ONEMPTY {
        if(hrand(25000) < 2 + (2 * items[itBounty] + yendor::hardness()) + (items[itRevolver] ? 150:0))
          c->monst = moOutlaw;
        if(hrand(1000) < PT(20 + kills[moOutlaw], 40))
          c->item = itRevolver;
        }
      break;
    
    case laWhirlwind:
      if(d == 9) {
        if(S7 == 5) 
          c->wall = (pseudohept(c) && (c->master->fiftyval == 0 || c->master->fiftyval == 6)) ?
            waFan : waNone;
        else if(sphere) {
          if(whirlwind::fzebra3(c) == 0) c->wall = waFan;
          if(S7 == 4 && celldistance(c, currentmap->gamestart()) == 2 && ctof(c))
            c->wall = waChasm;
          }
        else if(archimedean) ;
        else if(!euclid && zebra3(c) == 0) c->wall = waFan;
        else if(pseudohept(c) && hrand(2000) < 150 && !reptilecheat)
          c->wall = waChasm;
        else if(c->type == 6 && buildPrizeMirror(c, 1000))
          {}
        else
          whirlwind::switchTreasure(c);
        }
      ONEMPTY {
        if(hrand(4500) < items[itWindstone] + yendor::hardness())
          c->monst = moWindCrow;
        if(hrand(doCross?3000:30000) < items[itWindstone] + yendor::hardness() - 5)
          c->monst = moAirElemental;
        }
      break;
    
    case laStorms: {
      bool randstorm = hyperbolic_not37 || NONSTDVAR || (quotient && geometry != gZebraQuotient);
      if(d == 9) {
      
        if(torus) {
          int pid = decodeId(c->master);
          if(pid == torusconfig::qty/3) c->wall = waCharged;
          if(pid == torusconfig::qty*2/3) c->wall = waGrounded;
          }
        else if(euclid) {
          int x,y;
          tie(x,y) = cell_to_pair(c);
          if((x+1)%3 == 0 && y%3 == 0) {
            if(hrand(100) < 50)
              c->wall = hrand(2) ? waCharged : waGrounded;
            }
          else if(ishept(c)) {
            bool sand = false;
            for(int i=0; i<c->type; i++) {
              createMov(c, i);
              tie(x,y) = cell_to_pair(c->move(i));
              if((x+1)%3 == 0 && (y)%3 == 0) sand = true;
              }
            if(sand && hrand(100) < 20)
              c->wall = waSandstone;
            }
          }
        else if(sphere) { 
          if(S7 == 5) {
            if(ctof(c)) {
              int id = c->master->fiftyval;
              if(id == 1) c->wall = waCharged;
              if(id == (elliptic && BITRUNCATED ? 3 : 9)) c->wall = waGrounded;
              }
            }
          else if(S7 == 4) {
            if(ctof(c)) {
              int id = c->master->fiftyval;
              if(id == 1) c->wall = waCharged;
              if(id == 3) c->wall = waGrounded;
              }
            }
          else if(S7 == 3) {
            if(ctof(c)) {
              int id = c->master->fiftyval;
              if(id == 1) c->wall = waCharged;
              }
            else {
              c->wall = waGrounded;
              forCellEx(c2, c) if(ctof(c2) && c2->master->fiftyval == 1)
                c->wall = waNone;
              }
            }
          }
        else if(randstorm) ;
        else if(PURE) {
          int i = zebra40(c);
          if((i == 5 || i == 8) && hrand(100) < 20) c->wall = hrand(2) ? waCharged : waGrounded;
          else if(i == 15) c->wall = waSandstone;
          }
        else if(S3 > 3) {
          int z = zebra40(c);
          int i = z;
          int b = 0;
          while(i) { if(i&1) b++; i>>=1; }
          if(ctof(c) && (b&1) && hrand(100) < 20)  c->wall = (z&2) ? waCharged : waGrounded;
          }
        else {
          int i = zebra40(c);
          if(c->wall); 
          else if(i >= 40 && hrand(100) < 50) 
            c->wall = hrand(2) ? waCharged : waGrounded;
          else if(ishept(c) && hrand(100) < 20)
            c->wall = waSandstone;
          }
        
        for(int i=0; i<c->type; i++)
          if(c->move(i) && c->move(i)->land != laStorms && c->move(i)->land != laNone)
            c->wall = waNone;
        }
      if(d == BARLEV && randstorm) {
        c->landparam = hrand(1000000);
        }
      if(d == BARLEV-1 && randstorm) {
        /* static int stormlevel, stormgroup;
        if(!stormlevel) stormgroup = 0;
        stormlevel++; stormgroup++; */
        forCellEx(c2, c) if(c2->land == laStorms && c2->mpdist >= BARLEV && c2->landparam > c->landparam) {
          setdist(c2, BARLEV-1, c);
          }
        /*
        stormlevel--;
        if(stormlevel == 0) printf("stormgroup = %4d (cellcount = %6d hcount = %6d)\n", stormgroup, cellcount, heptacount); */
        bool bad = false;
        forCellEx(c2, c) if(c2->land != laStorms) bad = true;
        if(!bad) {
          int wallsnow = 0;
          int slp = 0;
          forCellEx(c2, c) {
            if(c2->wall) wallsnow++;
            if(c2->landparam == 1) slp++;
            }
          if(wallsnow == 0) {
            if(hrand(100) < 10)
              c->wall = pick(waCharged, waGrounded, waSandstone, waSandstone);
            else c->landparam = 1;
            }
          else if(wallsnow == 1 && slp == 0) {
            c->wall = waSandstone;
            }
          else if(wallsnow)
            c->landparam = 1;
          }
        }
      ONEMPTY {
        if(hrand(7500) < 25 + (items[itFulgurite] + yendor::hardness()))
          c->monst = (hrand(5) ? moMetalBeast : moMetalBeast2),
          c->hitpoints = 3;
        if(hrand(10000) < 20 + (items[itFulgurite] + yendor::hardness()) && !ishept(c)) {
          c->monst = moStormTroll;
          }
        }
      break;
      }
    
    case laGraveyard:
      if(d == 9) {
        if(randomPatternsMode)
          c->wall = RANDPAT ? ((RANDPATV(laCrossroads) || RANDPATV(laCrossroads2)) ? waAncientGrave : waFreshGrave) : waNone;
        else if(pseudohept(c)) 
          c->wall = hrand(5) ? waAncientGrave : waFreshGrave;
        }
      ONEMPTY {
        if(hrand(5000) < PT(30 + 4*kills[moZombie] + 6*kills[moNecromancer], 120) && notDippingFor(itBone))
          c->item = itBone;
        if(hrand(20000) < 10 + items[itBone] + yendor::hardness()) {
          eMonster grm[6] = { moZombie, moZombie, moZombie, moGhost, moGhost, moNecromancer};
          c->monst = grm[hrand(6)];
          }
        }
      break;

    case laRlyeh:
      if(d == 9)  {
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
              if(c->move(i) && c->move(i)->land == laRlyeh)
                c->move(i)->wall = waNone;
              }
            }
          if(pseudohept(c) && hrand(2)) c->wall = waColumn;
          }
        }
      ONEMPTY {
        if(hrand(5000) < PT(30 + 2 * (kills[moCultist] + kills[moTentacle] + kills[moPyroCultist]), 100) && notDippingFor(itStatue))
          c->item = itStatue;
        if(hrand(8000) < 5 + items[itStatue] + yendor::hardness() && !c->monst && !peace::on)
          c->monst = moTentacle, c->item = itStatue, c->mondir = NODIR;
        else if(hrand(12000) < 5 + items[itStatue] + yendor::hardness())
          c->monst = hrand(3) ? ((hrand(40) < items[itStatue]-25) ? moCultistLeader : moCultist) : moPyroCultist;
        else if(hrand(8000) < 5 + items[itStatue] + yendor::hardness() && c->type == 6 && !(yendor::on && (yendor::clev().flags & YF_NEAR_TENT)) && celldist(c)>=3) {
          for(int t=0; t<c->type; t++) {
            if(c->move(t) && c->move(t)->monst == moNone && (c->wall == waNone || c->wall == waColumn))
              c->move(t)->wall = ishept(c->move(t)) ? waColumn : waNone;
            if(c->move(t)->wall == waColumn)
              c->move(t)->item = itNone;
            }
          if(buildIvy(c, 0, 3) && !peace::on) c->item = itStatue;
          }
        }
      break;
    
    case laTemple:
      ONEMPTY {
        // depth!
        int d = 
          chaosmode ? -15 * items[itGrimoire]:
          (eubinary || c->master->alt) ? celldistAlt(c) : 10;
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
          else if(hrand(5000) < 10 && (chaosmode ? items[itGrimoire] >= treasureForLocal() : -d > TEMPLE_EACH * 10) && !peace::on && !inv::on)
            c->item = itOrbDragon;
          }
        }
      break;

    case laHell:      
      if(d == 9) {
        if(hrand(1000) < (PURE ? 16 : 36) && celldist(c) >= 3) {
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
      ONEMPTY {
        if(hrand(6000) < PT(120 + (kills[moLesser]), 240) && notDippingFor(itHell))
          c->item = itHell;
        if(hrand(8000) < 40 + items[itHell] * (chaosmode?4:1) + yendor::hardness())
          c->monst = moLesser;
        else if(hrand(24000) < 40 + items[itHell] * (chaosmode?4:1) + yendor::hardness())
          c->monst = moGreater;
        }
      break;

    case laCocytus:      
      if(d == 9)  {
        if(c->wall == waNone) c->wall = waFrozenLake;
        if(hrand(100) < 5 && !safety && celldist(c) >= 3) {
          for(int i=0; i<c->type; i++) {
            cell *c2 = createMov(c, i);
            setdist(c2, d+1, c);
            if(c2 && c2->land == laCocytus)
              c2->wall = waLake;
            }
          
          c->wall = waLake;

          if(hrand(500) < 100 + 2 * (items[itSapphire] + yendor::hardness()))
            c->monst = moShark;
          }
        }
      if(d == 8) c->landparam = 0;
      ONEMPTY {
        if(hrand(5000) < PT(100 + 2 * (kills[moShark] + kills[moGreaterShark] + kills[moCrystalSage]), 200) && notDippingFor(itSapphire))
          c->item = itSapphire;
        if(hrand(5000) < 2 * (items[itSapphire] + yendor::hardness())) {
          eMonster ms[3] = { moYeti, moGreaterShark, moCrystalSage };
          c->monst = ms[hrand(3)];
          if(c->monst == moGreaterShark) c->wall = waLake;
          }
        }
      break;
    
    case laHive:
      if(d == 9) {
        if(hrand(2000) < (chaosmode ? 1000 : PURE ?200:2) && !safety) 
          hive::createBugArmy(c);
        if(hrand(2000) < 100 && !c->wall && !c->item && !c->monst) {
          int nww = 0;
          for(int i=0; i<c->type; i++) if(c->move(i) && c->move(i)->wall == waWaxWall)
            nww++;
          if(nww == 0) {
            c->wall = waWaxWall;
            c->monst = moNone;
            c->landparam = hrand(0xFFFFFF + 1);
            }
          }
        }
      ONEMPTY {
        if(isHive(c->land) && hrand(6000) < (hive::hivehard() - 15)) 
          c->monst = hive::randomHyperbug();

        /* if(hrand(1500) < 30 + (kills[moBug0] + kills[moBug1] + kills[moBug2]) && notDippingFor(itRoyalJelly))
          c->item = itRoyalJelly; */
        /* if(hrand(2000) < 2)
          c->monst = eMonster(moBug0 + hrand(3)); */
        }
      break;
    
    case laKraken:
      if(d == 9) c->wall = waSea;

      if(d == 8 && c->land == laKraken && kraken_pseudohept(c) && hrand(1000) <= 2) {
        c->wall = waNone;
        forCellEx(c2, c) c2->wall = waNone;
        }
  
      if(d == 8 && !kraken_pseudohept(c) && hrand(20000) < 10 + 3 * items[itKraken] + 2 * yendor::hardness() && c->wall == waSea && !c->item && !c->monst && !safety) {
        bool ok = true;
        forCellEx(c2, c)
          if(c2->wall != waSea || c2->item || c2->monst) 
            ok = false;
          
        if(ok) {
          c->monst = moKrakenH;
          forCellIdEx(c2, i, c) {
            c2->monst = moKrakenT;
            c2->hitpoints = 1;
            c2->mondir = c->c.spin(i);
            }
          if(!peace::on) playSound(c, "seen-kraken");
          }
        }

      if(d == 7 && c->wall == waSea && !c->item && !c->monst && !safety) {
        if(hrand(2000) < 3) {
          c->wall = waBoat;
          c->item = itOrbFish;
          c->monst = moViking;
          }
        else if(hrand(2000) < 16) c->item = itOrbFish;
        else if(hrand(500) < kills[moKrakenH]) c->item = itKraken;
        else placeLocalOrbs(c);
        }      
      break;
    
    case laCaribbean:
      if(d == 9) {
        if(!eubinary) {
          if(c->master->alt && c->master->alt->distance <= 2) {
            if(!eubinary) generateAlts(c->master);
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
      if(d == 8 && !eubinary) {
        int mindist  = 9;
        for(int i=0; i<c->type; i++) {
          cell *c2 = c->move(i);
          if((eubinary || c2->master->alt) && celldistAlt(c2) < mindist)
            mindist = celldistAlt(c2);
          }
        if(mindist == 0) beCIsland(c);
        }                                         
      if(d == 7 && c->wall == waSea) {
        bool coast = false;
        for(int i=0; i<c->type; i++) {
          cell *c2 = c->move(i);
          if(passable(c2, c, 0) && c2->wall != waBoat) coast = true;
          }
        if(coast && hrand(10) < 5) {
          c->wall = waBoat;
          
          placeLocalSpecial(c, 50);
          }
        }  
      if(d == 7 && c->wall == waSea && hrand(10000) < 20 + items[itPirate] + 2 * yendor::hardness() && !safety)
        c->monst = moCShark;  
      if(d == 7 && c->wall == waCTree && hrand(5000) < 100 + items[itPirate] + yendor::hardness())
        c->monst = moParrot;    
      ONEMPTY {
        if(hrand(1500) < 4 && celldistAlt(c) <= -5 && peace::on)
          c->item = itCompass;
        if(hrand(16000) < 40 + (items[itPirate] + yendor::hardness()))
          c->monst = moPirate;
        }
      break;
    
    case laTortoise:
      if(d == 9) {
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
      ONEMPTY {
        if(hrand(4000) < (peace::on ? 750 : 50 + items[itBabyTortoise]*2 + yendor::hardness() * 6) && !safety) {
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
      break;
    
    case laDesert:
      if(d == 9) {
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
              if(c->move(i) && c->move(i)->land == laDesert)
                c->move(i)->wall = waNone;
              }
            }
          }
  
        if(hrand(300) == 1 && items[itSpice] >= 5) c->wall = waThumperOff;
        }
      ONEMPTY {
        if(hrand(5000) < PT(100 + 2 * (kills[moWorm] + kills[moDesertman]), 200) && notDippingFor(itSpice))
          c->item = itSpice;
        if(hrand(8000) < 10 + 2 * (items[itSpice] + yendor::hardness()) && !c->monst)
          c->monst = (hrand(2) && !peace::on) ? moWorm : moDesertman,
          c->mondir = NODIR;
        }
      break;
      
    case laRedRock:
      if(d == 9) {
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
          int i = hrand(S7);
          buildRedWall(createMovR(c, i), 33);
          if(hrand(2) == 0) 
            buildRedWall(createMovR(createMovR(c, i), c->c.spin(i)+(hrand(2)?2:4)), 20);
          i = (i + 3 + hrand(2)) % S7;
          if(hrand(6) < 4)
            buildRedWall(createMovR(c, i), 33);
          if(hrand(2) == 0) 
            buildRedWall(createMovR(createMovR(c, i), c->c.spin(i)+(hrand(2)?2:4)), 20);
          }
        }
      if(d == 7 && c->wall == waNone)
        buildPrizeMirror(c, 1000);    
      ONEMPTY {
        if(hrand((doCross && celldist(c) <= 5) ?450:16000) < 30+items[itRedGem]+yendor::hardness() && !pseudohept(c) && !c->monst && !c->wall && !(!BITRUNCATED && S3==4)) {
          int i = -1;
          for(int t=0; t<c->type; t++) if(c->move(t)->mpdist > c->mpdist && !pseudohept(c->move(t)))
            i = t;
          if(i != -1 && !peace::on) 
            generateSnake(c, i, 1);
          }
        else if(hrand(16000) < 50+items[itRedGem]+yendor::hardness() && (PURE?hrand(10)<3:!ishept(c)) && !c->monst)
          c->monst = moRedTroll,
          c->mondir = NODIR;
        }
      break;
    
    case laSnakeNest:
      ONEMPTY {
        if(hrand(30000) < 30+items[itSnake]+yendor::hardness() && !c->monst && !c->wall && !peace::on) {
          vector<int> gooddir;
          for(int t=0; t<c->type; t++) if(c->move(t)->mpdist > c->mpdist)
            gooddir.push_back(t);
          if(isize(gooddir))
            generateSnake(c, gooddir[hrand(isize(gooddir))], 2);
          }
        else if(hrand(10000) < items[itSnake] - 10 + yendor::hardness() && !c->monst && !c->wall && !peace::on) {
          c->monst = pick(moRedTroll, moMiner, moSkeleton, moBomberbird);
          c->hitpoints = 3;
          }
        else if(hrand(16000) < sqrt(kills[moHexSnake] * 10000) && c->wall == waNone) {
          c->item = itSnake;
          if(hrand(30) >= items[itSnake] - 10) c->wall = waRed3;
          }
        }
      break;
    
    case laWarpSea:
    case laWarpCoast:
      if(d == 9 && randomPatternsMode)
        setland(c, RANDPAT ? laWarpCoast : laWarpSea); 
      
      if(d == 8) {
        if(c->land == laWarpCoast) {
          if(hrand(1000) < 150 && celldist(c) >= 3 && !pseudohept(c)) 
            c->wall = waSmallTree;
          int q = 0;
          if(BITRUNCATED && !chaosmode && !weirdhyperbolic) for(int i=0; i<c->type; i++) 
            if(c->move(i) && !isWarped(c->move(i)->land) && c->move(i)->land != laCrossroads4) 
              q++;
          if(q == 1) c->wall = waWarpGate;
          }
        
        if(c->land == laWarpSea) {
          c->wall = waSea;
          int q = 0;
          if(BITRUNCATED && !chaosmode) for(int i=0; i<c->type; i++) 
            if(c->move(i) && !isWarped(c->move(i)->land)) q++;
          if(q == 1) c->wall = waWarpGate;
          }      
        }

      if(d == 7 && c->land == laWarpSea && c->wall == waSea && !safety) {
        if(sphere && c->type != 6 && c->master->fiftyval == 6) {
          c->wall = waBoat;
          c->monst = moRatling;
          }
        if(hrand(12000) < 30 + 2 * items[itCoral] + yendor::hardness()) {
          c->wall = waBoat;
          c->monst = moRatling;
          }
        else if(hrand(6000) < PT(120 + min(kills[moRatling], 250), 240) && notDippingFor(itCoral)) {
          c->wall = waBoat;
          c->item = itCoral;
          }
        }
  
      ONEMPTY if(c->land == laWarpCoast) {
        if(hrand(12000) < 20 + 2 * items[itCoral] + yendor::hardness()) {
          c->monst = moRatling;
          c->stuntime = hrand(2);
          }
        }
      break;
     
    case laBull:
      if(d == 8 && !c->monst && !c->item && celldist(c) >= 3) {
        /* int cd = getCdata(c, 3);
        cd &= 15;
        int ce = getCdata(c, 2);
        ce &= 15;
        if(cd >= 8 && ce >= 8) */
        
        if(randomPatternsMode) {
          int i = (RANDPAT ? 1 : 0)  + (RANDPATV(laHell) ? 2 : 0);
          eWall w[4] = {waNone, waSmallTree, waBigTree, waStone};
          c->wall = w[i];
          }
        else if(hrand(100) < 25)
          c->wall = safety ? pick(waBigTree, waSmallTree) : pick(waStone, waBigTree, waSmallTree);
        }    

      if(d == 7 && c->wall == waNone && !safety) {
        if(hrand(1000) < 20) c->monst = moSleepBull;
        else if(hrand(2500) < items[itBull] + yendor::hardness() - 10) c->monst = moGadfly;
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
      break;

    case laRose:
      if(d == 8 && c->land == laRose && hrand(2000) < 100 && !c->wall && !c->item && !c->monst) {
        int nww = 0;
        for(int i=0; i<c->type; i++) {
          cell *c2 = c->move(i);          
          if(c2 && c2->wall == waRose) nww++;
          if(!chaosmode) for(int j=0; j<c2->type; j++) {
            cell *c3 = c2->move(j);
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
      ONEMPTY {
        if(hrand(2000) < PT(25 + min(kills[moFalsePrincess] + kills[moRoseBeauty] + kills[moRoseLady], 200), 100) && notDippingFor(itRose)) {
          for(int t=0; t<c->type; t++) if(c->move(t) && c->move(t)->wall == waRose)
            c->item = itRose;
          }
        else {
          int p = hrand(10000);
          if(p >= 10 + items[itRose] + yendor::hardness()) ;
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
      break;
    
    case laHunting:
      if(d == 7 && c->land == laHunting) {
        if(hrand(1000) < 20) {
          if(openplains(c)) {
            if(hrand(2) == 0) {
              if(!items[itHunting]) {
                vector<pair<cell*, cell*>> next;
                int bonus = c->type - 4;
                for(int i=0; i<c->type; i++) {
                  cellwalker ci(c, i);
                  cellwalker cw0 = ci + wstep + revstep;
                  cellwalker cw1 = ci + bonus + wstep + revstep;
                  if(cw0.at->mpdist > 7)
                  if(cw1.at->mpdist > 7)
                    next.emplace_back(cw0.at, cw1.at);
                  }
                if(isize(next)) {
                  c->item = itHunting;
                  auto& p = next[hrand(isize(next))];
                  p.first->monst = moHunterGuard;
                  p.second->monst = moHunterGuard;
                  }
                }
              else if(items[itHunting] < 10) {
                vector<cell*> next;
                forCellEx(c2, c) if(c2->mpdist > 7 && (!BITRUNCATED || !ctof(c2))) next.push_back(c2);
                if(isize(next)) {
                  c->item = itHunting;
                  cell *c3 = next[hrand(isize(next))];
                  vector<cell*> dogcells;
                  forCellEx(c4, c3) if(c4->mpdist > 7 && !isNeighbor(c4, c))
                    dogcells.push_back(c4);
                  if(items[itHunting] < 10 && isize(dogcells) >= 2) {
                    for(int t=0;; t++) {
                      if(t == 50) { c->item = itNone; break; }
                      cell *dog1 = dogcells[hrand(isize(dogcells))];
                      cell *dog2 = dogcells[hrand(isize(dogcells))];
                      if(isNeighbor(dog1, dog2)) {
                        dog1->monst = moHunterGuard;
                        dog1->landparam = 0;
                        dog2->monst = moHunterGuard;
                        dog2->landparam = 1;
                        break;
                        }
                      }
                    }
                  else if(isize(dogcells)) {
                    c->item = itHunting;
                    dogcells[hrand(isize(dogcells))]->monst = moHunterGuard;
                    }
                  }
                }
              else c->item = itHunting;
              }
            else placeLocalSpecial(c, 10);
            }
          }
        if(hrand(5000) < items[itHunting]- 17 + yendor::hardness())
          c->monst = moHunterDog;
        }
      break;
    
    case laOcean:
      if(d >= 8) c->wall = waSea;
      if(d == 7 && !safety) {
        bool placecolumn = false;
        if(c->landparam % TEMPLE_EACH == 0 && c->landparam <= 24) {
          int q = 0;
          forCellEx(c2, c)
            if(c2->landparam % TEMPLE_EACH == 0 && !pseudohept(c2)) q++;
          placecolumn = q == 2;
          if(placecolumn && weirdhyperbolic && !BITRUNCATED && hrand(100) >= 50)
            placecolumn = false;
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
          if(hrand(15000) < 10 + 2 * items[itCoast] + 2 * yendor::hardness())
            c->monst = moAlbatross;
          if(items[itCoast] >= treasureForLocal() && hrand(10000) < 5 && !peace::on && !inv::on)
            c->item = itOrbAir;
          else placeLocalSpecial(c, 10000, 6, 5);
          buildPrizeMirror(c, 2000);
          }
        else if(c->landparam > 25) {
          int amberbonus = items[itCoast] - 50;
          // 50 => 10
          if(hrand(15000) < 10 + amberbonus + 2 * yendor::hardness())
            c->monst = moAlbatross;
          if(hrand(30000) < 20 + 2 * yendor::hardness()) {
            c->wall = waBoat;
            c->monst = moPirate;
            // orbs are possible!
            placeOceanOrbs(c);
            }
          else if(hrand(30000) < 10 + 2 * yendor::hardness())
            c->monst = moCShark;
          }
        }
      break;

    case laMinefield:  
      if(d == 7) {
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
        else placeLocalSpecial(c, 500);
        }
      if(d == 3 && safety && (c->wall == waMineMine || c->wall == waMineUnknown))
        c->wall = waMineOpen;
      break;
    
    case laWhirlpool:
      if(d == 7) 
        whirlpool::generate(c);
      break;
    
    case laMountain:
      if(d == 7) {
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
      break;

    case laEndorian:
      ONEMPTY {
        if(c->wall == waNone && coastval(c, laEndorian) >= 10 && hrand(5000) < 10 + 2 * (items[itApple] + yendor::hardness()))
          c->monst = moSparrowhawk;
        else if(c->wall != waNone && hrand(5000) < 10 + 2 * (items[itApple] + yendor::hardness()))
          c->monst = moResearcher;
        else if(c->wall == waCanopy && !checkInTree(c, 3) && hrand(5000) < PT(300 + 5 * (kills[moSparrowhawk] + kills[moResearcher]), 750))
          c->item = itApple;
        }
      break;

    case laJungle:
      ONEMPTY {
        if(hrand(5000) < PT(25 + 2 * (kills[moIvyRoot] + kills[moMonkey]), 40) && notDippingFor(itRuby))
          c->item = itRuby;
        if(hrand(15000) < 5 + 1 * (items[itRuby] + yendor::hardness()))
          c->monst = moMonkey;
        else if(hrand(80000) < 5 + items[itRuby] + yendor::hardness())
          c->monst = moEagle;
        else if(pseudohept(c) && c != currentmap->gamestart() && hrand(4000) < 300 + items[itRuby] && !c->monst) {
          int hardchance = items[itRuby] + yendor::hardness();
          if(hardchance > 25) hardchance = 25;
          bool hardivy = hrand(100) < hardchance;
          if((hardivy ? buildIvy(c, 1, 9) : buildIvy(c, 0, c->type)) && !peace::on)
            c->item = itRuby;
          }
        }
      break;
    
    case laMirrorOld:
      ONEMPTY {
        if(hrand(5000) < 120 && (peace::on || notDippingFor(itShard)) && mirror::build(c));
        else if(ishept(c) && hrand(5000) < 10 * PRIZEMUL)
          placePrizeOrb(c);
        else if(hrand(12000) < 8 + items[itShard] + yendor::hardness())
          c->monst = moRanger;
        else if(hrand(60000) < 8 + items[itShard] + yendor::hardness())
          c->monst = moEagle;
        }
      break;
    
    case laMirror:
      ONEMPTY {
        if(hrand(1250) < 120 && (peace::on || notDippingFor(itShard)) && mirror::build(c)) ;
        else if(ishept(c) && hrand(5000) < 10 * PRIZEMUL)
          placePrizeOrb(c);
        else if(hrand(cwt.at->land == laMirror ? 600 : 2400) < 8 + items[itShard] + yendor::hardness()) {
          if(items[itShard] >= 5 && hrand(120) <= 20)
            c->monst = moMirrorSpirit;
          else
            c->monst = moNarciss;
          }
        }
      break;

    case laClearing:    
      if(d == 7) {
        clearing::generate(c);
        if(pseudohept(c)) {
          int d = -celldistAlt(c);
          if(hrand(2500) < items[itMutant2] + yendor::hardness() - 10)
            c->monst = moRedFox;
          else if(hrand(100 + d) < d)
            c->item = itMutant2;          
          }
        }
      break;
    
    case laMotion:
       ONEMPTY {
         if(hrand(1500) < PT(30 + kills[moRunDog], 100) && notDippingFor(itFeather))
           c->item = itFeather;
         if(hrand(20000) < 25 + items[itFeather] + yendor::hardness()) {
           c->monst = moRunDog;
           // preset the movement direction
           // this will make the dog go in the direction of the center,
           // if the player is unreachable/invisible
           for(int d=0; d<c->type; d++) if(c->move(d) == from) {
             c->mondir = (d+3) % c->type;
             }
           chasmify(c);
           c->wall = shmup::on ? waNone : waChasm;
           }
         }
       break;
    
    case laMagnetic:
      if(d == 8) {
        if(hrand(10000) < 30 && !c->monst && !c->wall) {
          cell *c1 = c;
          cell *c2 = createMov(c1, hrand(c1->type));
          if(c2->monst || c2->wall) return;
          if(hrand(2)) swap(c1, c2);
          forCellEx(c3, c1) if(c3->monst == moNorthPole) return;
          forCellEx(c3, c2) if(c3->monst == moSouthPole) return;
          c1->monst = moNorthPole;
          c2->monst = moSouthPole;
          c1->mondir = neighborId(c1, c2);
          c2->mondir = neighborId(c2, c1);
          }
        }
      break;
    
    case laRuins: {
      int kf = 10 + items[itRuins] + yendor::hardness();
      if(d == 8) {
        if(randomPatternsMode) {
          c->wall = RANDPAT ? waRuinWall : waNone;
          }
        else {
          if(out_ruin(c)) {
            if(hrand(100) < 3)
              c->wall = waRuinWall;
            }
          else if(hrand(100) < 75) {
            forCellEx(c2, c) if(out_ruin(c2))
              c->wall = waRuinWall;
            }
          }
        if(hrand(40000) < kf && !c->monst && !c->wall && !shmup::on) {
          cell *c1 = c;
          cell *c2 = createMov(c1, hrand(c1->type));
          if(c2->monst || c2->wall) return;
          c1->monst = moPair;
          c2->monst = moPair;
          c1->mondir = neighborId(c1, c2);
          c2->mondir = neighborId(c2, c1);
          }
        }
      ONEMPTY {
        if(hrand(1500) < PT(30 + kills[moHexDemon] + kills[moSkeleton] + kills[moMonk] + kills[moPair], 100) && notDippingFor(itRuins)) {
          c->item = itRuins;
          forCellEx(c2, c) if(c2->monst == moMonk)
            c->item = itNone;
          }
        if(hrand(7000) < kf && !c->monst) {
          c->monst = genRuinMonster(c);
          c->hitpoints = 3;
          }
        }
      break;
      }
    
    case laDocks: {
      if(d == 8) {
        if(randomPatternsMode) {
          c->wall = RANDPAT ? waDock : waSea;
          if(c->wall == waSea && RANDPATV(laEAir))
            c->wall = waBoat;
          }
        else {
          patterns::patterninfo si;
          if(a38) 
            patterns::val38(c, si, patterns::SPF_DOCKS, patterns::PAT_COLORING);
          else
            si.id = archimedean ? (hrand(6)*4) : (zebra40(c)&2) ? 0 : zebra40(c) == 4 ? 8 : 1;
          c->wall = waSea;
          if(among(si.id, 0, 4, 16, PURE ? -1 : 24))
            c->wall = waDock;
          if(si.id == 8 && hrand(100) < 75) {
            c->wall = waBoat;
            if(a38) for(int i=0; i<c->type; i++) {
              patterns::val38(createMov(c, i), si, patterns::SPF_DOCKS, patterns::PAT_COLORING);
              if(si.id == 0) c->mondir = i;
              }
            }
          }
        }
      if(d == 7 && !safety) {
        if(randomPatternsMode) {
          if(hrand(2000) < PT(30 + kills[moRatling] + kills[moCShark] + kills[moAlbatross] + kills[moPirate] + kills[moFireFairy], 100)) c->item = itDock;
          }
        else {
          patterns::patterninfo si;
          if(a38)
            patterns::val38(c, si, patterns::SPF_DOCKS, patterns::PAT_COLORING);
          else
            si.id = hrand(20);
          if(si.id == 16 && hrand(250) < PT(30 + kills[moRatling] + kills[moCShark] + kills[moAlbatross] + kills[moPirate] + kills[moFireFairy], 100))
            c->item = itDock;
          }
        if(c->wall == waDock && hrand(6000) < 25 + items[itDock] + yendor::hardness())
          c->monst = pick(moPirate, moRatling, moFireFairy);
        if(c->wall == waSea && hrand(6000) < 25 + items[itDock] + yendor::hardness())
          c->monst = pick(moCShark, moAlbatross);
        }
      break;
      }
    
    case laEAir:
    case laEWater:
    case laEEarth:
    case laEFire:
    // but not case laElementalWall:
      if(d==8) {
        if(hrand(c->land == laEAir ? 6 : 25) == 0) {
          if(c->land == laEFire) c->wall = waEternalFire;
          else if(c->land == laEWater) c->wall = waSea;
          else if(c->land == laEAir) c->wall = waChasm;
          else if(c->land == laEEarth) c->wall = waStone;
          }
        }
      ONEMPTY {
        eItem localshard = localshardof(c->land);
        int danger = 5 * items[localshard] * items[localshard];
        eMonster elof = elementalOf(c->land);
        int elkills = PT(kills[elof], 25);
        
        if(hrand(8000) < 12 + (items[itElemental] + danger + yendor::hardness())) {
          c->monst = elof;
          if(c->land != laEAir) chasmifyElemental(c);
          c->wall = waNone;
          if(c->land == laEWater) c->wall = waSea;
          }
        else if(hrand(5000) < 100 + elkills*3 && notDippingFor(itElemental))
          c->item = localshard;
        else placeLocalSpecial(c, 500);
        }
      break;
    
    case laBrownian:
      if(c->wall == waBrownian || (d == 9 && hrand(10000) < 5))
        start_brownian(c, 0);
      break;
    
    case laMirrored:
    case laMirrorWall:
    case laMirrorWall2:
    case laMirrored2:
      c->wall = waMirrorWall;
      break;
    
    case laCrossroads:
    case laCrossroads2:
    case laCrossroads3:
    case laCrossroads4:
    case laCrossroads5:
      if(c->wall == waTower) c->land = laCamelot;
      ONEMPTY {
        if(!BITRUNCATED && c->land == laCrossroads5 && hrand(100) < 60)
          c->wall = waBarrier;
        else if(!inv::on && items[itShard] >= 10 && hrand(8000) < 120*orbcrossfun(items[itShard]) && mirror::build(c)) ;
        else if(hyperstonesUnlocked() && hrand(8000) < 100 && mirror::build(c)) ;
        else if(tactic::on && isCrossroads(specialland) && hrand(8000) < 120 && mirror::build(c)) ;
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
      break;
 
    case laHaunted:
    case laHauntedBorder:
    case laHauntedWall:
      if(d == 7) {
        c->wall = waNone;
  
        c->item = itNone; c->monst = moNone;
        
        if(hrand(100) < 25)
          c->wall = hrand(2) ? waBigTree : waSmallTree;
          
        else if(hrand(500) < 2 && ishept(c))
          c->wall = hrand(10) < 3 ? waFreshGrave : waAncientGrave;
          
        else {
          if(hrand(5000) < 30)
            c->item = itGreenStone;
    
          if(hrand(4000) < 10 + items[itLotus] + yendor::hardness() && !safety) 
            c->monst = moGhost;
            
          int depth = getHauntedDepth(c);
          
          if(hrand(500 + depth) < depth - items[itLotus] && !safety && !peace::on)
            c->item = itLotus;
          }
        }
      break;
    
    case laCamelot:
      // generated mostly in moreBigStuff
      if(tactic::on && d == 0) {
        int r = roundTableRadius(c);
        if(r > 28 + 3 * items[itHolyGrail])
          items[itHolyGrail]++;
        }      
      break;
    
    case laNone:
    case laBarrier:
    case laOceanWall:
    case laElementalWall:
    case laCanvas:
    case laPrincessQuest:
    case laMercuryRiver:
      // do nothing!
      break;

    case laDual:
      if(d == 7) {
        if(pseudohept(c))
          c->wall = waChasm;
        else {
          c->landparam = 1;
          if(S7%2 == 0) {
            c->landparam = 2;
            forCellCM(c2, c) {
              if(c2->landparam == 2) c->landparam = 3;
              if(!pseudohept(c2)) forCellCM(c3, c2) if(c3->landparam == 3) c->landparam = 3;
              }
            }
          int hr = hrand(100) / 5;
          if(hr == 0)
            c->wall = waTrapdoor;
          else if(hr == 1)
            c->wall = waSmallTree;
          else if(hr == 2)
            c->wall = waStone;
          }
        }
      ONEMPTY {
        if(hrand(5000) < PT(100 + 2 * kills[moRatling], 200) && notDippingFor(itGlowCrystal))
          c->item = itGlowCrystal;
        if(hrand(2000) < 2 * (items[itGlowCrystal] + yendor::hardness()))
          c->monst = moRatling;
        }      
      break;
    
    case laMemory:
      c->wall = waChasm;
      if(d == 7 && !c->monst && hrand(2000) < 4)
        c->monst = moGhost;
      break;
    }
  }

// repair the buggy walls flowing in from another land, like ice walls flowing into the Caves

void repairLandgen(cell *c) {
  if(c->land == laCaves && c->wall != waCavewall && c->wall != waCavefloor)
    c->wall = waCavefloor;
  
  if(c->land == laDeadCaves && c->wall != waDeadwall && 
    c->wall != waDeadfloor && c->wall != waDeadfloor2 && c->wall != waEarthD)
    c->wall = waDeadfloor2;
  
  if(c->land == laCocytus && c->wall != waFrozenLake && c->wall != waLake && c->wall != waIcewall)
    c->wall = waFrozenLake;
  
  if(c->land == laAlchemist && c->wall != waFloorA && c->wall != waFloorB)
    c->wall = waFloorA;
  
  if(c->wall == waIcewall && c->land != laIce && c->land != laCocytus && c->land != laBlizzard)
    c->wall = waNone;
  
  if(c->wall == waRed3 && c->land != laRedRock && c->land != laSnakeNest && c->land != laBrownian)
    c->wall = waNone;
  
  if(c->item == itRedGem && c->land != laRedRock)
    c->item = itNone;
  
  if(c->item == itRose && c->land != laRose)
    c->item = itNone;
  
  if(c->wall == waChasmD) {
    chasmify(c);
    }
    
  if(c->wall == waEarthD) {
    chasmifyEarth(c);
    }
  
  if(c->wall == waElementalD) {
    chasmifyElemental(c);
    }
  
  if(passable(c, NULL, 0)) {
    if(c->land == laBarrier) c->wall = waBarrier;
    if(c->land == laOceanWall) 
      c->wall = c->type == 7 ? waBarrier : waSea;
    }

  if(cellHalfvine(c)) {
    int i = -1;
    for(int k=0; k<c->type; k++) if(c->move(k) && c->move(k)->wall == c->wall)
      i = 0;
    if(i == -1) c->wall = waNone;
    }
  }

void setdist(cell *c, int d, cell *from) {
  
  if(c->mpdist <= d) return;
  if(c->mpdist > d+1 && d != BARLEV) setdist(c, d+1, from);
  c->mpdist = d;
  // printf("setdist %p %d [%p]\n", c, d, from);
  
  // this fixes the following problem:
  // http://steamcommunity.com/app/342610/discussions/0/1470840994970724215/
  if(!generatingEquidistant && from && d >= 7 && c->land && !binarytiling && !archimedean) {
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
      setdist(createMov(c, i), d+1, c);
      }
    if(d >= BARLEV) c->item = itBuggy2;
    return;
    }
  
  if(d >= BARLEV) {
  
    if(!c->land && from && from->land != laElementalWall && from->land != laHauntedWall && from->land != laOceanWall &&
      from->land != laBarrier && !quotient) {
        if(!hasbardir(c)) setland(c, from->land);
        }
    if(c->land == laTemple && !tactic::on && !chaosmode) setland(c, laRlyeh);
    if(c->land == laMountain && !tactic::on && !chaosmode) setland(c, laJungle);
    if(c->land == laClearing && !tactic::on) setland(c, laOvergrown);
    if(c->land == laWhirlpool && !tactic::on && !yendor::on) setland(c, laOcean);
    if(c->land == laCamelot && !tactic::on) setland(c, laCrossroads);

#if CAP_DAILY
    if(!daily::on) {
#else
    if(true) {
#endif
      if(sphere || torus) setLandSphere(c);
      else if(euclid) setLandEuclid(c);
      if(weirdhyperbolic) setLandWeird(c);
      if(quotient) { setland(c, specialland); setLandQuotient(c); }
      }
    
    // if(chaosmode) setland(c, getCLand(c));
    }
  
  if(d == BARLEV && c->land == laCanvas)  {
    c->landparam = patterns::generateCanvas(c);
    if(canvas_invisible)
      c->wall = waInvisibleFloor;
    }

  if(d >= BARLEV-1 && c->land == laPrairie)
    prairie::spread(c, from);

  if(d < BARLEV && c->land == laPrairie && !c->landparam) {
    printf("d=%d/%d\n", d, BARLEV);
    raiseBuggyGeneration(c, "No landparam set");
    return;
    }
    
  if(d == BARLEV && !euclid && c != cwt.at) 
    buildBigStuff(c, from);
  
  if(buggyGeneration) return;
  
  if(d < 10) {
    if(d >= 0) {
      explore[d]++;
      exploreland[d][c->land]++;
      }
    
    if(d < BARLEV) for(int i=0; i<c->type; i++) {
      
      setdist(createMov(c, i), d+1, c);
      if(buggyGeneration) return;
      }
    
    if(d == BARLEV-2 && c->land == laOcean) 
      buildEquidistant(c);

    if(d == BARLEV-2 && inmirror(c)) 
      buildEquidistant(c);

    if(d == BARLEV-2 && (c->land == laGraveyard || c->land == laHauntedBorder || c->land == laHaunted) && !tactic::on)
      buildEquidistant(c);
    }
  
  if(d <= 7 && (c->land == laGraveyard || c->land == laHauntedBorder)) {
    c->land = (c->landparam >= 1 && c->landparam <= HAUNTED_RADIUS) ? laHauntedBorder : laGraveyard;
    }

  if(d == 8 && isGravityLand(c->land)) {
    buildEquidistant(c);
    }

  giantLandSwitch(c, d, from);
  
  if(d == 9) moreBigStuff(c);

  if(d == 7) repairLandgen(c);

  ONEMPTY if(!c->item) {
    if(isCrossroads(c->land))
      placeCrossroadOrbs(c);
    else
      placeLocalOrbs(c);
    }

  if(PURE && c->wall == waMirrorWall && c->land == laMirror)
    c->land = laMirrorWall; // , c->item = itPirate; // not really a proper bugfix

  if(d == 7) playSeenSound(c);
  
#if CAP_EDIT
  if(d >= 7 && patterns::whichPattern)
    mapeditor::applyModelcell(c);
#endif
  }

}
