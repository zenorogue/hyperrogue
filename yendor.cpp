// Hyperbolic Rogue

// Copyright (C) 2011-2016 Zeno Rogue, see 'hyper.cpp' for details

// Yendor Quest, together with the Yendor Challenge
// also, the Pure Tactics Mode

namespace peace { extern bool on; }

int hiitemsMax(eItem it) {
  int mx = 0;
  for(auto& a: hiitems) if(a.second[it] > mx) mx = a.second[it];
  return mx;
  }

int modecode();

typedef vector<pair<int, string> > subscoreboard;

void displayScore(subscoreboard& s, int x) {
  int vf = min((vid.yres-64) / 70, vid.xres/80);

  if(syncstate == 1) {
    displayfr(x, 56, 1, vf, "(syncing)", 0xC0C0C0, 0);
    }
  else {
    sort(s.begin(), s.end());
    for(int i=0; i<size(s); i++) {
      int i0 = 56 + i * vf;
      displayfr(x, i0, 1, vf, its(-s[i].first), 0xC0C0C0, 16);
      displayfr(x+8, i0, 1, vf, s[i].second, 0xC0C0C0, 0);
      }
    }
  }

namespace yendor {

  bool on = false;
  bool generating = false;
  bool path = false;
  bool everwon = false;
  bool won = false;
  bool easy = false;
  
  int challenge; // id of the challenge
  int lastchallenge;
  
  #define YENDORLEVELS 30
  
  map<modecode_t, array<int, YENDORLEVELS>> bestscore;

  eLand nexttostart;

#define LAND_YENDOR_CHAOS 41

  yendorlevel levels[YENDORLEVELS] = {
    {laNone,      0},
    {laHell,      YF_DEAD}, // FORCE BARRIERS?
    {laGraveyard, YF_DEAD5}, 
    {laDesert,    YF_NEAR_IVY}, // IVY OR TENTACLE?
    {laMinefield, YF_END}, // NOT WON, SEEMS OKAY
    {laEmerald,   0}, // WON FINE
    {laOvergrown, 0}, // WON, TOO EASY?
    {laMotion,    YF_START_AL | YF_END}, // NOT WON, SEEMS OKAY
    {laAlchemist, 0}, // ALMOST WON
    {laIvoryTower,YF_START_CR | YF_NEAR_ELEM | YF_REPEAT}, // won cool
    {laMirrorOld, YF_NEAR_OVER}, // OK
    {laWhirlpool, 0}, // cool
    {laIce,       YF_NEAR_ELEM}, // OK
    {laHive,      YF_NEAR_RED}, // OK
    {laCaribbean, 0}, // seems OK
    {laOcean,     YF_WALLS}, // well... stupid, add Caribbean/Fjord
    {laPalace,    0}, // more trapdoors!
    {laZebra,     0}, // TOO HARD?
    {laWineyard,  0}, // hard-ish
    {laStorms,    0}, // ?
    {laLivefjord, 0},
    {laJungle,    0},
    {laPower,     YF_START_CR},
    {laWildWest,  0},
    {laWhirlwind, YF_NEAR_TENT},
    {laHell,      YF_CHAOS | YF_DEAD},
    {laDragon,    YF_DEAD},
    {laReptile,   0},
    {laTortoise,  YF_RECALL},
    {laCocytus,   YF_NEAR_FJORD}
    };
  
  int tscorelast;

  void uploadScore() {
    int tscore = 0;
    for(int i=1; i<YENDORLEVELS; i++)
      if(bestscore[0][i]) tscore += 999 + bestscore[0][i];
    // printf("Yendor score = %d\n", tscore);

    if(tscore > tscorelast) {
      tscorelast = tscore;
      if(tscore >= 1000) achievement_gain("YENDC1", 'x');
      if(tscore >= 5000) achievement_gain("YENDC2", 'x');
      if(tscore >= 15000) achievement_gain("YENDC3", 'x');
      }

    achievement_score(LB_YENDOR_CHALLENGE, tscore);
    }
    
  yendorlevel& clev() { return levels[challenge]; }
  
  eLand changeland(int i, eLand l) {
    if((clev().flags & YF_START_ANY) && i < 20 && l != clev().l) return clev().l;
    if((clev().flags & YF_END) && i > 80 && l == clev().l) return laIce;
    return laNone;
    }
  
  string name;
  eLand first, second, last;

  struct yendorinfo {
    cell *path[YDIST];
    bool found;
    bool foundOrb;
    int howfar;
    cell* key() { return path[YDIST-1]; }
    cell* orb() { return path[0]; }
    };
  
  vector<yendorinfo> yi;
  
#define NOYENDOR 999999
  int yii = NOYENDOR;
  
  int hardness() {
    if(peace::on) return 15; // just to generate monsters
    if(!yendor::generating && !yendor::path && !yendor::on) return 0;
    int thf = 0;
    for(int i=0; i<size(yi); i++) {
      yendorinfo& ye ( yi[i] );
      if(!ye.foundOrb && ye.howfar > 25)
        thf += (ye.howfar - 25);
      }
    thf -= 2 * (YDIST - 25); 
    if(thf<0) thf = 0;
    return items[itOrbYendor] * 5 + (thf * 5) / (YDIST-25);
    }

  enum eState { ysUntouched, ysLocked, ysUnlocked };
  
  eState state(cell *yendor) {
    for(int i=0; i<size(yi); i++) if(yi[i].path[0] == yendor)
      return yi[i].found ? ysUnlocked : ysLocked;
    return ysUntouched;
    }
  
  bool check(cell *yendor) {
    int byi = size(yi);
    for(int i=0; i<size(yi); i++) if(yi[i].path[0] == yendor) byi = i;
    if(byi < size(yi) && yi[byi].found) return false;
    if(byi == size(yi)) {
      yendorinfo nyi;
      nyi.path[0] = yendor;
      nyi.howfar = 0;
      
      if(euclid) {
        int di = hrand(6);
        int dj = (di+1) % 6;
        int qty = hrand(YDIST-1);
        int tot = 0;
        for(int i=0; i<YDIST-1; i++) {
          tot += qty;
          if(tot >= YDIST-1) { 
            tot -= YDIST-1;
            nyi.path[i+1] = createMov(nyi.path[i], di);
            }
          else
            nyi.path[i+1] = createMov(nyi.path[i], dj);
          }
        }
      
      else {
        bool endorian_change = true;
        bool in_endorian = false;
        cellwalker lig(yendor, hrand(yendor->type));  
        for(int i=0; i<YDIST-1; i++) {
          if(lig.c->land == laEndorian)
            in_endorian = true;
          else if(!isTechnicalLand(lig.c->land))
            in_endorian = false;
          nyi.path[i] = lig.c;
          
          cwstep(lig);
          if(inmirror(lig)) lig = mirror::reflect(lig);
          cwspin(lig, 3);
          if(lig.c->type == 7) {
            if(in_endorian && endorian_change && i >= YDIST - 20) {
              // make the last leg a bit more difficult
              cwspin(lig, hrand(2)*3-1);
              endorian_change = false;
              }
            else
              cwspin(lig, hrand(2));
            }
          }
        nyi.path[YDIST-1] = lig.c;
        }
      
      generating = true;
  
      for(int i=1; i<YDIST-1; i++) {
        cell *c = nyi.path[i];
        cell *prev = nyi.path[i-1];
              
        setdist(c, 10, prev);
        setdist(c, 9, prev);
        setdist(c, 8, prev);
        setdist(c, 7, prev);
  
        if(challenge && i+BARLEV-7 < YDIST-5 && !euclid) {
          cell *c2 = nyi.path[i+BARLEV-7];
          if(c2->land == laIvoryTower) continue;
          eLand ycl = changeland(i, c2->land);
          if(ishept(c2) && ycl) {
            int bd = 2 + hrand(2) * 3;
//          printf("barrier at %d\n", i);
            buildBarrier(c2, bd, ycl);
            if(c2->bardir != NODIR && c2->bardir != NOBARRIERS) 
              extendBarrier(c2);
            }
          }
        }
      
      nyi.found = false;
      nyi.foundOrb = false;
  
      cell *key = nyi.path[YDIST-1];
  
      generating = false;
  
      for(int b=10; b>=5; b--) setdist(key, b, nyi.path[YDIST-2]);

      for(int i=-1; i<key->type; i++) {
        cell *c2 = i >= 0 ? key->mov[i] : key;
        checkTide(c2);
        c2->monst = moNone; c2->item = itNone;
        if(!passable(c2, NULL, P_MIRROR | P_MONSTER)) {
          if(c2->wall == waCavewall) c2->wall = waCavefloor;
          else if(c2->wall == waDeadwall) c2->wall = waDeadfloor2;
          else if(c2->wall == waLake) c2->wall = waFrozenLake;
          else if(c2->land == laCaribbean) c2->wall = waCIsland;
          else if(c2->land == laOcean) c2->wall = waCIsland;
          else if(c2->land == laRedRock) c2->wall = waRed3;
          else if(c2->land == laWhirlpool) 
            c2->wall = waBoat, c2->monst = moPirate, c2->item = itOrbWater;
          else c2->wall = waNone;
          }
        if(c2->wall == waReptile) c2->wall = waNone;
        if(c2->wall == waMineMine || c2->wall == waMineUnknown) 
          c2->wall = waMineOpen;
        if(c2->wall == waTrapdoor && i == -1)
          c2->wall = waGargoyleFloor;
        if(c2->land == laLivefjord) {
          c2->wall = waSea;
          for(int i=0; i<c2->type; i++) 
            c2->mov[i]->wall = waSea;
          }
        if(isGravityLand(c2->land) && key->land == c2->land &&
          c2->landparam < key->landparam && c2->wall != waTrunk)
            c2->wall = waPlatform;
        if(c2->land == laReptile && i >= 0)
          c2->wall = waChasm;
        if(c2->land == laMirrorWall && i == -1)
          c2->wall = waNone;
        }
      key->item = itKey;
  
      yi.push_back(nyi);
      }
    addMessage(XLAT("You need to find the right Key to unlock this Orb of Yendor!"));
    if(yii != byi) {
      yii = byi;
      achievement_gain("YENDOR1");
      playSound(yendor, "pickup-yendor");
      return true;
      }
    return false;
    }
  
  void onpath() {
    path = false;
    if(yii < size(yi)) {
      for(int i=0; i<YDIST; i++) if(yi[yii].path[i]->cpdist <= 7) {
        if(i > yi[yii].howfar) yi[yii].howfar = i;
        path = true;
        }
      }
    }
  
  void init(int phase) {
    if(!on) return;
    
    if(phase == 1) {
      won = false;
      if(!easy) items[itOrbYendor] = bestscore[modecode()][challenge];
      chaosmode = (clev().flags & YF_CHAOS);
      specialland = firstland = clev().l;
      if(clev().flags & YF_START_AL) {
        firstland = laAlchemist;
        items[itElixir] = 50;
        items[itFeather] = 50;
        }
      if(firstland == laPower) 
        items[itOrbSpeed] = 60, items[itOrbWinter] = 60;
      if(clev().flags & YF_START_CR) {
        firstland = laCrossroads;
        }
      if(firstland == laGraveyard) items[itBone] = 10;
      if(firstland == laEmerald)   items[itEmerald] = 10;
      if(firstland == laCocytus)   items[itFjord] = 10;
      if(!euclid) {
        if(clev().flags & YF_DEAD)   items[itGreenStone] = 100;
        if(clev().flags & YF_DEAD5)  items[itGreenStone] = 5;
        }
      if(clev().flags & YF_RECALL) {
        int yq = items[itOrbYendor];
        items[itOrbRecall] = 60 - yq;
        items[itOrbTime] = 60 - yq;
        items[itOrbEnergy] = 60 - yq;
        items[itOrbTeleport] = 60 - yq;
        items[itOrbSpace] = 60 - yq;
        items[itOrbDash] = 60 - yq;
        items[itOrbFrog] = 60 - yq;
        }
      nexttostart = laNone;
      }
    
    if(phase == 2) {
      cell *c2 = cwt.c->mov[0];
      c2->land = firstland;
      if(firstland == laRlyeh) c2->wall = waNone;
      yendor::check(c2);
      if(clev().flags & YF_NEAR_IVY)
        nexttostart = laJungle;
      if(clev().flags & YF_NEAR_FJORD)
        nexttostart = laLivefjord;
      if(clev().flags & YF_NEAR_TENT)
        nexttostart = laRlyeh;
      if(clev().flags & YF_NEAR_ELEM) {
        if(firstland == laIce) {
          nexttostart = laEWater;
          items[itWaterShard] = 10;
          }
        else nexttostart = laEAir;
        }
      if(clev().flags & YF_NEAR_OVER)
        nexttostart = laOvergrown;
      if(clev().flags & YF_NEAR_RED) {
        nexttostart = laRedRock;
        items[itRedGem] = 25;
        }
      if(clev().flags & YF_WALLS) {
        items[itPirate] += 25;
        items[itFjord] += 25;
        }
      if(clev().l == laWhirlpool) {
        items[itWhirlpool] += 10;
        items[itOrbWater] += 150;
        }
      }

    if(phase == 3) {
      cell *c2 = cwt.c->mov[0];
      makeEmpty(c2);
      c2->item = itOrbYendor;
      nexttostart = laNone;
      if(clev().flags & YF_RECALL) recallCell = cwt.c;
      }
    }
  
  bool levelUnlocked(int i) {
    yendorlevel& ylev(levels[i]);

    eItem t = treasureType(ylev.l);
    if(ylev.l != laWildWest && hiitemsMax(t) < 10) return false;
    if((ylev.flags & YF_NEAR_ELEM) && hiitemsMax(itElemental) < 10) return false;
    if((ylev.flags & YF_NEAR_RED) && hiitemsMax(itRedGem) < 10) return false;
    if((ylev.flags & YF_NEAR_OVER) && hiitemsMax(itMutant) < 10) return false;
    if((ylev.flags & YF_NEAR_TENT) && hiitemsMax(itStatue) < 10) return false;
    if((ylev.flags & YF_NEAR_FJORD) && hiitemsMax(itFjord) < 10) return false;
    if((ylev.flags & YF_CHAOS) && !chaosUnlocked) return false;
    if((ylev.flags & (YF_DEAD|YF_DEAD5)) && hiitemsMax(itBone) < 10) return false;
    if((ylev.flags & YF_RECALL) && hiitemsMax(itSlime) < 10) return false;
    return true;
    }
  
  struct scoredata {
    string username;
    int scores[landtypes];
    };
  vector<scoredata> scoreboard;

  const char *chelp = 
    "There are many possible solutions to the Yendor Quest. In the Yendor "
    "Challenge, you will try many of them!\n\n"    
    "Each challenge takes part in a specific land, and you have to use what "
    "you have available.\n\n"
    
    "You need to obtain an Orb of Yendor in the normal game to activate "
    "this challenge, and (ever) collect 10 treasures in one or two lands "
    "to activate a specific level.\n\n"
    
    "After you complete each challenge, you can try it again, on a harder "
    "difficulty level.\n\n"
    
    "All the solutions showcased in the Yendor Challenge work in the normal "
    "play too. However, passages to other lands, and (sometimes) some land features "
    "are disabled in the Yendor "
    "Challenge, so that you have to use the expected method. Also, "
    "the generation rules are changed slightly for the Palace "
    "and Minefield while you are looking for the Orb of Yendor, "
    "to make the challenge more balanced "
    "(but these changes are also active during the normal Yendor Quest).\n\n"
    
    "You get 1000 points for each challenge won, and 1 extra point for "
    "each extra difficulty level.";

  void showMenu() {
    int s = vid.fsize;
    vid.fsize = vid.fsize * 4/5;
    dialog::init(XLAT("Yendor Challenge"), iinf[itOrbYendor].color, 150, 100);

    for(int i=1; i<YENDORLEVELS; i++) {
      string s;
      yendorlevel& ylev(levels[i]);
      
      if(autocheat || levelUnlocked(i)) {
      
        s = XLATT1(ylev.l);
        
        if(!euclid) {  
          if(ylev.flags & YF_CHAOS) { s = "Chaos mode"; }
          if(ylev.flags & YF_NEAR_IVY) { s += "+"; s += XLATT1(laJungle); }
          if(ylev.flags & YF_NEAR_FJORD) { s += "+"; s += XLATT1(laLivefjord); }
          if(ylev.flags & YF_NEAR_TENT) { s += "+"; s += XLATT1(laRlyeh); }
          if(ylev.flags & YF_NEAR_ELEM) { s += "+"; s += XLATT1(laElementalWall); }
          if(ylev.flags & YF_NEAR_OVER) { s += "+"; s += XLATT1(laOvergrown); }
          if(ylev.flags & YF_NEAR_RED) { s += "+"; s += XLATT1(laRedRock); }
          if(ylev.flags & YF_START_AL) { s += "+"; s += XLATT1(laAlchemist); }
          if(ylev.flags & YF_DEAD) { s += "+"; s += XLATT1(itGreenStone); }
          if(ylev.flags & YF_RECALL) { s += "+"; s += XLATT1(itOrbRecall); }
          }
        }

      else {
        s = "(locked)";
        }
      
      string v;
      if(bestscore[modecode()][i] == 1)
        v = XLAT(" (won!)");
      else if(bestscore[modecode()][i])
        v = XLAT(" (won at level %1!)", its(bestscore[modecode()][i]));
      
      dialog::addSelItem(s, v, 'a' + i-1);
      }

    dialog::addBreak(60);
    dialog::addItem(XLAT("Return to the normal game"), '0');
    dialog::addSelItem(XLAT(
      easy ? "Challenges do not get harder" : "Each challenge gets harder after each victory"),
      " " + XLAT(easy ? "easy" : "challenge"), '1');
    
    dialog::display();
    
    int yc = getcstat - 'a' + 1;
    if(yc > 0 && yc < YENDORLEVELS) {
      subscoreboard scorehere;

      for(int i=0; i<size(scoreboard); i++) {
        int sc = scoreboard[i].scores[yc];
        if(sc > 0) 
          scorehere.push_back(
            make_pair(-sc, scoreboard[i].username));
        }
      
      displayScore(scorehere, vid.xres / 4);
      }

    yendor::uploadScore();
    vid.fsize = s;
    
    keyhandler = [] (int sym, int uni) {
      dialog::handleNavigation(sym, uni);
      if(uni >= 'a' && uni < 'a'+YENDORLEVELS-1) {
        challenge = uni-'a' + 1;
        if(levelUnlocked(challenge) || autocheat) {
          restartGame(yendor::on ? 0 : 'y');
          }
        else 
          addMessage("Collect 10 treasures in various lands to unlock the challenges there");
        }
      else if(uni == '0') {
        if(yendor::on) restartGame('y');
        }
      else if(uni == '1') easy = !easy;
      else if(uni == '2' || sym == SDLK_F1) gotoHelp(chelp);
      else if(doexiton(sym, uni)) popScreen();
      };
    }
    
  void collected(cell* c2) {
    int pg = gold();
    playSound(c2, "tada");
    items[itOrbShield] += 31;
    for(int i=0; i<size(yendor::yi); i++)
      if(yendor::yi[i].path[0] == c2) 
        yendor::yi[i].foundOrb = true;
    // Shielding always, so that we know that it protects!
    for(int i=0; i<4; i++) switch(hrand(13)) {
      case 0: items[itOrbSpeed] += 31; break;
      case 1: items[itOrbLightning] += 78; break;
      case 2: items[itOrbFlash] += 78; break;
      case 3: items[itOrbTime] += 78; break;
      case 4: items[itOrbWinter] += 151; break;
      case 5: items[itOrbDigging] += 151; break;
      case 6: items[itOrbTeleport] += 151; break;
      case 7: items[itOrbThorns] += 151; break;
      case 8: items[itOrbInvis] += 151; break;
      case 9: items[itOrbPsi] += 151; break;
      case 10: items[itOrbAether] += 151; break;
      case 11: items[itOrbFire] += 151; break;
      case 12: items[itOrbSpace] += 78; break;
      }
    items[itOrbYendor]++; 
    items[itKey]--;
    yendor::everwon = true;
    if(yendor::on) {
      yendor::won = true;
      if(!cheater) {
        dynamicval<bool> c(chaosmode, false);
        yendor::bestscore[modecode()][yendor::challenge] = 
          max(yendor::bestscore[modecode()][yendor::challenge], items[itOrbYendor]);
        yendor::uploadScore();        
        }
      }
    addMessage(XLAT("CONGRATULATIONS!"));
    achievement_collection(itOrbYendor, pg, gold());
    achievement_victory(false);
    }
  
  auto hooks = addHook(clearmemory, 0, [] () {
    yendor::yii = NOYENDOR; yendor::yi.clear();
    });
  };

#define MAXTAC 20
namespace tactic {

  bool trailer = false;
  bool on = false;
  int id;
  
  map<modecode_t, array<int, landtypes>> recordsum;
  map<modecode_t, array<array<int, MAXTAC>, landtypes> > lsc;
  
  eLand lasttactic;
  
  struct scoredata {
    string username;
    int scores[landtypes];
    };
  map<modecode_t, vector<scoredata>> scoreboard;
  
  int chances(eLand l) {
    if(modecode() != 0 && l != laCamelot) return 3;
    for(auto& ti: land_tac)
      if(ti.l == l) 
        return ti.tries;
    return 0;
    }
  
  int tacmultiplier(eLand l) {
    if(modecode() != 0 && l != laCamelot) return 1;
    if(modecode() != 0 && l == laCamelot) return 3;
    for(auto& ti: land_tac)
      if(ti.l == l)
        return ti.multiplier;
    return 0;
    }
  
  bool tacticUnlocked(eLand l) {
    if(autocheat) return true;
    if(l == laWildWest || l == laDual) return true;
    return hiitemsMax(treasureType(l)) * landMultiplier(l) >= 20;
    }

  void record(eLand land, int score, int xc = modecode()) {
    if(land >=0 && land < landtypes) {
      for(int i=MAXTAC-1; i; i--) lsc[xc][land][i] = lsc[xc][land][i-1];
      tactic::lsc[xc][land][0] = score;
      }
    int t = chances(land);
    int csum = 0;
    for(int i=0; i<t; i++) if(lsc[xc][land][i] > 0) csum += lsc[xc][land][i];
    if(csum > recordsum[xc][land]) recordsum[xc][land] = csum;
    }

  void record() {
    record(lasttactic, items[treasureType(lasttactic)]);
    }

  void unrecord(eLand land, int xc = modecode()) {
    if(land >=0 && land < landtypes) {
      for(int i=0; i<MAXTAC-1; i++) lsc[xc][land][i] = lsc[xc][land][i+1];
      lsc[xc][land][MAXTAC-1] = -1;
      }
    }

  void unrecord() {
    unrecord(lasttactic);
    }
  
  int tscorelast;

  void uploadScoreCode(int code, int lb) {
    int tscore = 0;
    for(int i=0; i<landtypes; i++) 
      tscore += recordsum[code][i] * tacmultiplier(eLand(i));
    // printf("PTM score = %d\n", tscore);
    
    if(code == 0 && tscore > tscorelast) {
      tscorelast = tscore;
      if(tscore >= 1000) achievement_gain("PTM1", 'x');
      if(tscore >= 5000) achievement_gain("PTM2", 'x');
      if(tscore >= 15000) achievement_gain("PTM3", 'x');
      }
    achievement_score(lb, tscore);
    }

  void uploadScore() {
    uploadScoreCode(0, LB_PURE_TACTICS);
    uploadScoreCode(2, LB_PURE_TACTICS_SHMUP);
    uploadScoreCode(4, LB_PURE_TACTICS_COOP);
    }
  
  void showMenu() {
    cmode = sm::ZOOMABLE;
    mouseovers = XLAT("pure tactics mode") + " - " + mouseovers;
    
    { 
    dynamicval<bool> t(tactic::on, true);
    generateLandList(isLandValid2);
    }
    
    int nl = size(landlist);

    int nlm;
    int ofs = dialog::handlePage(nl, nlm, nl/2);
        
    int vf = min((vid.yres-64-vid.fsize) / nlm, vid.xres/40);
    
    int xr = vid.xres / 64;
    
    if(on) record(firstland, items[treasureType(firstland)]);
    
    int xc = modecode();
    
    getcstat = SDLK_ESCAPE;

    for(int i=0; i<nl; i++) {
      int i1 = i + ofs;
      eLand l = landlist[i1];

      int i0 = 56 + i * vf;
      int col;
      
      int ch = chances(l);

      if(!ch) continue;
      
      bool unlocked = tacticUnlocked(l);
      
      if(unlocked) col = linf[l].color; else col = 0x202020;
      
      if(displayfrZ(xr*1, i0, 1, vf-4, XLAT1(linf[l].name), col, 0) && unlocked) {
        getcstat = 1000 + i1;
        }
        
      if(unlocked || autocheat) {
        for(int ii=0; ii<ch; ii++)
          if(displayfrZ(xr*(24+2*ii), i0, 1, (vf-4)*4/5, lsc[xc][l][ii] > 0 ? its(lsc[xc][l][ii]) : "-", col, 16)) 
            getcstat = 1000 + i1;

        if(displayfrZ(xr*(24+2*10), i0, 1, (vf-4)*4/5, 
          its(recordsum[xc][l]) + " x" + its(tacmultiplier(l)), col, 0)) 
            getcstat = 1000 + i1;
        }
      else {
        int m = landMultiplier(l);
        displayfrZ(xr*26, i0, 1, (vf-4)*4/5, 
          XLAT("Collect %1x %2 to unlock", its((20+m-1)/m), treasureType(l)), 
          col, 0);
        }
      }
    
    dialog::displayPageButtons(3, true);

    uploadScore();
    if(on) unrecord(firstland);
    
    if(getcstat >= 1000 && getcstat < 1000 + size(landlist)) {
      int ld = landlist[getcstat-1000];
      subscoreboard scorehere;
      for(int i=0; i<size(scoreboard[xc]); i++) {
        int sc = scoreboard[xc][i].scores[ld];
        if(sc > 0) 
          scorehere.push_back(
            make_pair(-sc, scoreboard[xc][i].username));
        }      
      displayScore(scorehere, xr * 50);
      }
     
    keyhandler = [] (int sym, int uni) {
      if(uni >= 1000 && uni < 1000 + size(landlist)) {
        firstland = specialland = landlist[uni - 1000];
        restartGame(tactic::on ? 0 : 't');
        }
      else if(uni == '0') {
        firstland = laIce;
        if(tactic::on) restartGame('t');
        else popScreen();
        }

      else if(sym == SDLK_F1) gotoHelp(
        "In the pure tactics mode, you concentrate on a specific land. "
        "Your goal to obtain as high score as possible, without using "
        "features of the other lands. You can then compare your score "
        "with your friends!\n\n"
        
        "You need to be somewhat proficient in the normal game to "
        "unlock the given land in this challenge "
        "(collect 20 treasure in the given land, or 2 in case of Camelot).\n\n"
        
        "Since getting high scores in some lands is somewhat luck dependent, "
        "you play each land N times, and your score is based on N consecutive "
        "plays. The value of N depends on how 'fast' the land is to play, and "
        "how random it is.\n\n"
        
        "In the Caribbean, you can access Orbs of Thorns, Aether, and "
        "Space if you have ever collected 25 treasure in their native lands.\n\n"
        
        "The rate of treasure spawn is static in this mode. It is not "
        "increased by killing monsters.\n\n"
        
        "Good luck, and have fun!"
        );
      else if(dialog::handlePageButtons(uni)) ;
      else if(doexiton(sym, uni)) popScreen();
      };
    }

  };

int modecodetable[42][6] = {
  {  0, 38, 39, 40, 41, 42}, // softcore hyperbolic
  {  7, 43, 44, 45, 46, 47}, // hardcore hyperbolic
  {  2,  4,  9, 11, 48, 49}, // shmup hyperbolic
  { 13, 50, 51, 52, 53, 54}, // softcore heptagonal hyperbolic
  { 16, 55, 56, 57, 58, 59}, // hardcore heptagonal hyperbolic
  { 14, 15, 17, 18, 60, 61}, // shmup heptagonal hyperbolic
  {  1, 62, 63, 64, 65, 66}, // softcore euclidean
  {  8, 67, 68, 69, 70, 71}, // hardcore euclidean
  {  3,  5, 10, 12, 72, 73}, // shmup euclidean
  {110,111,112,113,114,115}, // softcore spherical
  {116,117,118,119,120,121}, // hardcore spherical
  {122,123,124,125,126,127}, // shmup spherical
  {128,129,130,131,132,133}, // softcore heptagonal spherical
  {134,135,136,137,138,139}, // hardcore heptagonal spherical
  {140,141,142,143,144,145}, // shmup heptagonal spherical
  {146,147,148,149,150,151}, // softcore elliptic
  {152,153,154,155,156,157}, // hardcore elliptic
  {158,159,160,161,162,163}, // shmup elliptic
  {164,165,166,167,168,169}, // softcore heptagonal elliptic
  {170,171,172,173,174,175}, // hardcore heptagonal elliptic
  {176,177,178,179,180,181}, // shmup heptagonal elliptic
  { 19, 74, 75, 76, 77, 78}, // softcore hyperbolic chaosmode
  { 26, 79, 80, 81, 82, 83}, // hardcore hyperbolic chaosmode
  { 21, 23, 28, 30, 84, 85}, // shmup hyperbolic chaosmode
  { 32, 86, 87, 88, 89, 90}, // softcore heptagonal hyperbolic chaosmode
  { 35, 91, 92, 93, 94, 95}, // hardcore heptagonal hyperbolic chaosmode
  { 33, 34, 36, 37, 96, 97}, // shmup heptagonal hyperbolic chaosmode
  { 20, 98, 99,100,101,102}, // softcore euclidean chaosmode
  { 27,103,104,105,106,107}, // hardcore euclidean chaosmode
  { 22, 24, 29, 31,108,109}, // shmup euclidean chaosmode
  {182,183,184,185,186,187}, // softcore spherical chaosmode
  {188,189,190,191,192,193}, // hardcore spherical chaosmode
  {194,195,196,197,198,199}, // shmup spherical chaosmode
  {200,201,202,203,204,205}, // softcore heptagonal spherical chaosmode
  {206,207,208,209,210,211}, // hardcore heptagonal spherical chaosmode
  {212,213,214,215,216,217}, // shmup heptagonal spherical chaosmode
  {218,219,220,221,222,223}, // softcore elliptic chaosmode
  {224,225,226,227,228,229}, // hardcore elliptic chaosmode
  {230,231,232,233,234,235}, // shmup elliptic chaosmode
  {236,237,238,239,240,241}, // softcore heptagonal elliptic chaosmode
  {242,243,244,245,246,247}, // hardcore heptagonal elliptic chaosmode
  {248,249,250,251,252,253}, // shmup heptagonal elliptic chaosmode
  };
// unused code: 25
int newmodecode = 255;

int modecode() {
#if CAP_SAVE
  if(anticheat::tampered || cheater) return 6;
#endif

  int xcode = 0;

  if(shmup::on) xcode += 2;
  else if(pureHardcore()) xcode ++;
  
  if(euclid) xcode += 6;
  else if(nonchamfered) xcode += 3;
  
  if(sphere) {
    xcode += 9;
    if(elliptic) xcode += 6;
    if(nonchamfered) xcode += 3;
    }
  
  if(chaosmode) xcode += 21;
  
  int np = numplayers()-1; if(np<0 || np>5) np=5;
  
  int mct = modecodetable[xcode][np];

  if(geometry == gTorus) mct += 512;
  if(geometry == gQuotient) mct += 1024;
  if(geometry == gQuotient2) mct += 1536;
#if CAP_INV
  if(inv::on) mct += 2048;
#endif
  if(peace::on) mct += 4096;
#if CAP_TOUR
  if(tour::on) mct += 8192;
#endif
  if(numplayers() == 7) mct += 16384;
  
  mct += ginf[geometry].xcode;
  
  return mct;
  }

void buildmodetable() {
  bool codeused[600];
  for(int q=0; q<600; q++) codeused[q] = 0;
  
  codeused[6] = true; // cheater
  
  printf("int modecodetable[42][6] = {\n");
  
  for(int b=0; b<42; b++) {
    extern bool hardcore;
    hardcore = (b%3 == 1);
    shmup::on = (b%3 == 2);
    nonchamfered = (b/3)%7 == 1 || (b/3)%7 == 4 || (b/3)%7 == 6;
    geometry = gNormal;
    if((b/3)%7 == 2) geometry = gEuclid;
    if((b/3)%7 >= 3) geometry = gSphere;
    if((b/3)%7 >= 5) geometry = gElliptic;
    chaosmode = b >= 21;
    printf("  {");
    for(int p=0; p<6; p++) {
      multi::players = p+1;
      if(p) printf(","); 
      int mc = modecode();
      if(codeused[mc]) mc = newmodecode++;
      codeused[mc] = true;
      printf("%3d", mc);
      }
    printf("}, //");
    if(hardcore) printf(" hardcore");
    else if(shmup::on) printf(" shmup");
    else printf(" softcore");
    if(nonchamfered) printf(" heptagonal");
    if(euclid) printf(" euclidean");
    else if(elliptic) printf(" elliptic");
    else if(sphere) printf(" spherical");
    else printf(" hyperbolic");
    if(chaosmode) printf(" chaosmode");
    printf("\n");
    }
  printf("  }\n");
  for(int i=0; i<newmodecode; i++) if(!codeused[i]) printf("// unused code: %d\n", i);
  printf("int newmodecode = %d;\n", newmodecode);
  }

namespace peace {

  bool on = false;
  bool hint = false;
  
  bool otherpuzzles;
  
  eLand simonlevels[] = {
    laCrossroads, laCrossroads2, laDesert, laCaves, laAlchemist, laRlyeh, laEmerald,
    laWineyard, laDeadCaves, laRedRock, laPalace,
    laLivefjord, laDragon,
    laNone
    };

  eLand explorelevels[] = {
    laBurial, laTortoise, laCamelot, laPalace,
    laIce, laJungle, laMirror, laDryForest, laCaribbean, laOcean, laZebra,
    laOvergrown, laWhirlwind, laWarpCoast, laReptile,
    laElementalWall, laAlchemist,
    laNone
    };                       

  eLand *levellist;
  int qty;
  
  void listLevels() {
    levellist = otherpuzzles ? explorelevels : simonlevels;
 
    for(qty = 0; levellist[qty]; qty++);
    }
    
  eLand getNext(eLand last) {
    if(!peace::on) return laNone;
    if(!qty) listLevels();
    if(isElemental(last) && hrand(100) < 90)
      return laNone;
    else if(createOnSea(last))
      return getNewSealand(last);
    else if(isCrossroads(last)) {
      while(isCrossroads(last) || last == laCaribbean || last == laCamelot)
        last = levellist[hrand(qty)];
      if(last == laElementalWall) last = laEFire;
      return last;
      }
    else return pick(laCrossroads, laCrossroads2);
    }
    
  bool isAvailable(eLand l) {
    for(int i=0; explorelevels[i]; i++) if(explorelevels[i] == l) return true;
    return false;
    }
  
  const char *chelp = 
    "In the peaceful mode, you just explore the world, "
    "without any battles; there are also several "
    "navigational puzzles available. In the memory game, "
    "you have to collect as many Dodecahedra as you can, "
    "and return to the starting point -- hyperbolic geometry "
    "makes this extremely difficult! Other hyperbolic puzzles "
    "include the Burial Grounds (excavate the treasures " 
    "using your magical sword), Galápagos (try to find an adult "
    "tortoise matching the baby), Camelot (find the center of "
    "a large hyperbolic circle), and Palace (follow the mouse). "
    "Other places listed are for exploration.";
    
  namespace simon {

    vector<cell*> path;
    int tobuild;
    
    void build() {
      if(otherpuzzles || !on) return;
      while(size(path) < tobuild) {
        cell *cp = path[size(path)-1];
        cell *cp2 = path[size(path)-2];
        vector<pair<cell*, cell*>> clister;
        clister.emplace_back(cp, cp);
        
        int id = 0;
        sval++;
        while(id < size(clister)) {
          cell *c = clister[id].first;
          cell *fr = clister[id].second;
          setdist(c, 5, NULL);
          
          forCellEx(c2,c)
            if(!eq(c2->aitmp, sval) && passable(c2, c, 0) && (c2->land == specialland || c2->land == laTemple) && !c2->item) {
              if(!id) fr = c2;
              bool next;
              if(specialland == laRlyeh)
                next = c2->land == laTemple && (cp2->land == laRlyeh || celldistAlt(c2) < celldistAlt(cp2) - 8); 
              else 
                next = celldistance(c2, cp2) == 8;
              if(next) {
                path.push_back(fr);
                fr->item = itDodeca;
                goto again;
                }              
              clister.emplace_back(c2, fr); 
              c2->aitmp = sval;
              }
          id++;
          }
        printf("Path broken, searched = %d\n", id);
        for(auto t: clister) t.first->item = itPirate;
        return;
        again: ;      
        }
      }
  
    void extend() {
      int i = 0;
      while(i<size(path) && path[i]->item != itDodeca) i++;
      if(tobuild == i+9)
        addMessage("You must collect all the dodecahedra on the path!");
      tobuild = i + 9;
      build();
      }
    
    void init() {
      tobuild = 0;
      if(!on) return;
      if(otherpuzzles) { items[itGreenStone] = 500; return; }
      cell *c2 = cwt.c->mov[0];
      makeEmpty(c2);
      c2->item = itOrbYendor;
      
      path.clear();
      path.push_back(cwt.c);
      path.push_back(c2);
      extend(); 
      }
    
    void restore() {
      for(int i=1; i<size(path); i++)
        if(path[i]->item == itNone && items[itDodeca])
          path[i]->item = itDodeca, items[itDodeca]--;
      }
    }
  
  void showMenu() {
    listLevels();
    dialog::init(XLAT(otherpuzzles ? "puzzles and exploration" : "memory game"), 0x40A040, 150, 100);

    for(int i = 0; i<qty; i++) 
      dialog::addItem(XLAT1(linf[levellist[i]].name), 'a'+i);

    dialog::addBreak(100);
    dialog::addItem(XLAT(otherpuzzles ? "memory game" : "puzzles and exploration"), '1');
    dialog::addBoolItem(XLAT("display hints"), hint, '2');
    dialog::addItem(XLAT("help"), SDLK_F1);
    dialog::addItem(XLAT("Return to the normal game"), '0');
    
    dialog::display();
    
    keyhandler = [] (int sym, int uni) {
      dialog::handleNavigation(sym, uni);
      
      if(uni == '1') otherpuzzles = !otherpuzzles;
      else if(uni >= 'a' && uni < 'a' + qty) {
        specialland = levellist[uni - 'a'];
        restartGame(peace::on ? 0 : 'P');
        }
      else if(uni == '2') { hint = !hint; popScreen(); }
      else if(uni == '0') {
        firstland = laIce;
        if(peace::on) restartGame('P');
        }
      else if(uni == 'h' || sym == SDLK_F1) gotoHelp(chelp);
      else if(doexiton(sym, uni)) popScreen();
      };
    }
    
  auto aNext = addHook(hooks_nextland, 100, getNext);
  };
