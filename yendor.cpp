// Hyperbolic Rogue

// Copyright (C) 2011-2016 Zeno Rogue, see 'hyper.cpp' for details

// Yendor Quest, together with the Yendor Challenge
// also, the Pure Tactics Mode

#define MODECODES 38

int hiitemsMax(eItem it) {
  int mx = 0;
  for(int i=0; i<MODECODES; i++) if(hiitems[i][it] > mx) mx = hiitems[i][it];
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
  
  struct yendorlevel {
    eLand l;
    int flags;
    };
  
  int challenge; // id of the challenge
  int lastchallenge;
  
  #define YENDORLEVELS 27

  int bestscore[MODECODES][YENDORLEVELS];

  #define YF_DEAD 1
  #define YF_WALLS 2
  #define YF_END 4
  #define YF_DEAD5 8

  #define YF_NEAR_IVY   16
  #define YF_NEAR_ELEM  32
  #define YF_NEAR_OVER  64
  #define YF_NEAR_RED   128
  #define YF_REPEAT     512
  #define YF_NEAR_TENT  1024

  #define YF_START_AL   2048
  #define YF_START_CR   4096
  #define YF_CHAOS      8192
  
  #define YF_START_ANY  (YF_START_AL|YF_START_CR)
  
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
    {laMirror,    YF_NEAR_OVER}, // OK
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
    {laDragon,    YF_DEAD}
    };

  void uploadScore() {
    int tscore = 0;
    for(int i=1; i<YENDORLEVELS; i++)
      if(bestscore[0][i]) tscore += 999 + bestscore[0][i];
    // printf("Yendor score = %d\n", tscore);
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
    };
  
  vector<yendorinfo> yi;
  
  int yii = 0;
  
  int hardness() {
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
  
  bool check(cell *yendor, bool checkonly) {
    int byi = size(yi);
    for(int i=0; i<size(yi); i++) if(yi[i].path[0] == yendor) byi = i;
    if(byi < size(yi) && yi[byi].found) return true;
    if(checkonly) return false;
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
        }
      key->item = itKey;
  
      yi.push_back(nyi);
      }
    yii = byi;
    addMessage(XLAT("You need to find the right Key to unlock this Orb of Yendor!"));
    achievement_gain("YENDOR1");
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
      euclidland = firstland = clev().l;
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
      if(!euclid) {
        if(clev().flags & YF_DEAD)   items[itGreenStone] = 100;
        if(clev().flags & YF_DEAD5)  items[itGreenStone] = 5;
        }
      nexttostart = laNone;
      }
    
    if(phase == 2) {
      cell *c2 = cwt.c->mov[0];
      c2->land = firstland;
      if(firstland == laRlyeh) c2->wall = waNone;
      yendor::check(c2, false);
      if(clev().flags & YF_NEAR_IVY)
        nexttostart = laJungle;
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
    if((ylev.flags & YF_CHAOS) && !chaosUnlocked) return false;
    if((ylev.flags & (YF_DEAD|YF_DEAD5)) && hiitemsMax(itBone) < 10) return false;
    return true;
    }
  
  struct scoredata {
    string username;
    int scores[landtypes];
    };
  vector<scoredata> scoreboard;

  void showMenu() {
    int s = vid.fsize;
    vid.fsize = vid.fsize * 4/5;
    displayStatHelp(-8, XLAT("Yendor Challenge"));

    for(int i=1; i<YENDORLEVELS; i++) {
      string s;
      yendorlevel& ylev(levels[i]);
      
      if(levelUnlocked(i)) {
      
        s = XLATT1(ylev.l);
        
        if(!euclid) {  
          if(ylev.flags & YF_CHAOS) { s = "Chaos mode"; }
          if(ylev.flags & YF_NEAR_IVY) { s += "+"; s += XLATT1(laJungle); }
          if(ylev.flags & YF_NEAR_TENT) { s += "+"; s += XLATT1(laRlyeh); }
          if(ylev.flags & YF_NEAR_ELEM) { s += "+"; s += XLATT1(laElementalWall); }
          if(ylev.flags & YF_NEAR_OVER) { s += "+"; s += XLATT1(laOvergrown); }
          if(ylev.flags & YF_NEAR_RED) { s += "+"; s += XLATT1(laRedRock); }
          if(ylev.flags & YF_START_AL) { s += "+"; s += XLATT1(laAlchemist); }
          if(ylev.flags & YF_DEAD) { s += "+"; s += XLATT1(itGreenStone); }
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
      
      displayStat(i-6, s, v, 'a' + i-1);
      }

    displayStat(YENDORLEVELS+1-6, XLAT("Return to the normal game"), "", '0');
    displayStat(YENDORLEVELS+1-5, XLAT(
      easy ? "Challenges do not get harder" : "Each challenge gets harder after each victory"),
      " " + XLAT(easy ? "easy" : "challenge"), '1');
    
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
    }
    
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

  void handleKey(int uni, int sym) {
    if(uni >= 'a' && uni < 'a'+YENDORLEVELS-1) {
      challenge = uni-'a' + 1;
      if(levelUnlocked(challenge)) {
        restartGame(yendor::on ? 0 : 'y');
        cmode = emNormal;
        }
      else 
        addMessage("Collect 10 treasures in various lands to unlock the challenges there");
      }
    else if(uni == '0') {
      if(yendor::on) restartGame('y');
      cmode = emNormal;
      }
    else if(uni == '1') easy = !easy;
    else if(uni == '2' || sym == SDLK_F1) {
      lastmode = cmode;
      cmode = emHelp;
      help = chelp;
      }
    else if(uni) cmode = emNormal;
    }
  };

#define MAXTAC 20
namespace tactic {

  bool trailer = false;
  bool on = false;
  int id;
  int recordsum[MODECODES][landtypes];
  int lsc[MODECODES][landtypes][MAXTAC];
  eLand lasttactic;
  
  struct scoredata {
    string username;
    int scores[landtypes];
    };
  vector<scoredata> scoreboard[MODECODES];
  
  int chances(eLand l) {
    if(modecode() != 0 && l != laCamelot) return 3;
    for(int i=0; i<LAND_TAC; i++)
      if(land_tac[i].l == l) {
        return land_tac[i].tries;
        }
    return 0;
    }
  
  int tacmultiplier(eLand l) {
    if(modecode() != 0 && l != laCamelot) return 1;
    if(modecode() != 0 && l == laCamelot) return 3;
    for(int i=0; i<LAND_TAC; i++)
      if(land_tac[i].l == l) return land_tac[i].multiplier;
    return 0;
    }
  
  bool tacticUnlocked(int i) {
    eLand l = land_tac[i].l;
    if(l == laWildWest) return true;
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
  
  void uploadScoreCode(int code, int lb) {
    int tscore = 0;
    for(int i=0; i<landtypes; i++) 
      tscore += recordsum[code][i] * tacmultiplier(eLand(i));
    // printf("PTM score = %d\n", tscore);
    achievement_score(lb, tscore);
    }

  void uploadScore() {
    uploadScoreCode(0, LB_PURE_TACTICS);
    uploadScoreCode(2, LB_PURE_TACTICS_SHMUP);
    uploadScoreCode(4, LB_PURE_TACTICS_COOP);
    }

  void showMenu() {
    mouseovers = XLAT("pure tactics mode") + " - " + mouseovers;

    int nl = LAND_TAC; 
    
    int vf = min((vid.yres-64) / nl, vid.xres/40);
    
    int xr = vid.xres / 64;
    
    if(on) record(firstland, items[treasureType(firstland)]);
    
    int xc = modecode();

    for(int i=0; i<nl; i++) {
      eLand l = land_tac[i].l;
      int i0 = 56 + i * vf;
      int col;
      
      int ch = chances(l);

      if(!ch) continue;
      
      bool unlocked = tacticUnlocked(i);
      
      if(unlocked) col = linf[l].color; else col = 0x202020;
      
      if(displayfr(xr*1, i0, 1, vf-4, XLAT1(linf[l].name), col, 0) && unlocked) {
        getcstat = 1000 + i;
        }
        
      if(unlocked) {
        for(int ii=0; ii<ch; ii++)
          if(displayfr(xr*(24+2*ii), i0, 1, (vf-4)*4/5, lsc[xc][l][ii] >= 0 ? its(lsc[xc][l][ii]) : "-", col, 16)) 
            getcstat = 1000 + i;

        if(displayfr(xr*(24+2*10), i0, 1, (vf-4)*4/5, 
          its(recordsum[xc][l]) + " x" + its(tacmultiplier(l)), col, 0)) 
            getcstat = 1000 + i;
        }
      else {
        int m = landMultiplier(l);
        displayfr(xr*26, i0, 1, (vf-4)*4/5, 
          XLAT("Collect %1x %2 to unlock", its((20+m-1)/m), treasureType(l)), 
          col, 0);
        }
      }
    
    if(on || ISIOS) {
      int i0 = 56 + nl * vf;
      if(displayfr(xr*24, i0, 1, vf-4, "press 0 to leave this mode", 0xFFD500, 8))
        getcstat = '0';
      }

    uploadScore();
    if(on) unrecord(firstland);
    
    if(getcstat >= 1000) {
      int ld = land_tac[getcstat-1000].l;      
      subscoreboard scorehere;
      for(int i=0; i<size(scoreboard[xc]); i++) {
        int sc = scoreboard[xc][i].scores[ld];
        if(sc > 0) 
          scorehere.push_back(
            make_pair(-sc, scoreboard[xc][i].username));
        }      
      displayScore(scorehere, xr * 50);
      }
    }

  void handleKey(int uni, int sym) {
    if(uni >= 1000 && uni < 1000 + LAND_TAC) {
      firstland = land_tac[uni - 1000].l;
      restartGame(tactic::on ? 0 : 't');
      cmode = emNormal;
      }
    else if(uni == '0') {
      cmode = emNormal;
      firstland = laIce;
      if(tactic::on) restartGame('t');
      }
    else if(uni == '2' || sym == SDLK_F1) {
      lastmode = cmode;
      cmode = emHelp;
      help = 
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
        
        "Good luck, and have fun!";
        
      }
    else if(uni) cmode = emNormal;
    }
  };

int modecode() {
  int xcode = 0;
  if(euclid) xcode += 1;
  if(shmup::on) {
    if(numplayers() == 1) xcode += 2;
    if(numplayers() == 2) xcode += 4;
    if(numplayers() == 3) xcode += 9;
    if(numplayers() == 4) xcode += 11;
    }
  if(pureHardcore() && !shmup::on) xcode += 7;
  if(anticheat::tampered || cheater) return 6;
  if(purehepta) {
    if(xcode > 6) xcode--;
    xcode /= 2;
    xcode += 13;
    }
  if(chaosmode && !yendor::on && cmode != emYendor) xcode += 19;
  return xcode;
  }

