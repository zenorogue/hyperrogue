// Hyperbolic Rogue - Complex features part II
// Copyright (C) 2011-2019 Zeno Rogue, see 'hyper.cpp' for details

/** \file complex2.cpp 
 *  \brief Continuation of complex.cpp
 *
 *  Includes: Brownian, Irradiated, Free Fall
 */

#include "hyper.h"
#ifdef CAP_COMPLEX2

namespace hr {

EX namespace brownian {

  #if HDR
  const int level = 5;
  #endif


  map<cell*, vector<pair<cell*, int >> > futures;
  int centersteps = 0;
  int totalsteps = 0;
  
  void rise(cell *c, int val) {
    if(c->wall == waSea) c->wall = waNone;
    if(c->land == laOcean || c->land == laNone) {
      c->land = laBrownian;
      c->landparam = 0;
      }
    c->bardir = NOBARRIERS; 
    forCellCM(c1, c) {
      c1->bardir = NOBARRIERS;
      if(c1->mpdist > BARLEV) {
        setdist(c1, BARLEV, c);
        }
      if(c1->land == laOcean) {
        c1->land = laBrownian;
        c1->landparam = 0;
        c1->wall = waSea;
        }
      }
    c->landparam += val;
    }
  
  static const int FAT = (-100); // less than 0

  void recurse(cell *c, int fatten_from) {
    int dl = getDistLimit();
    while(true) {
      int cd = celldist(c);
      bool fat = cd > fatten_from;
      totalsteps++;
      if(cd >= dl * (fat ? 4 : ISMOBILE ? 2 : 3) + celldist(cwt.at)) {
        cell *c1 = c;
        while(true) {
          cell *c2 = ts::left_parent(c1, celldist);
          if(!c2 || c2->mpdist < BARLEV) break;
          setdist(c2, BARLEV, c1);
          c1 = c2;
          }
        futures[c1].emplace_back(c, fatten_from);
        return;
        }
      if(c->mpdist <= 7 || !among(c->land, laNone, laOcean, laBrownian) || (c->land != laBrownian && c->bardir != NODIR)) {
        centersteps++; return;
        }
      cell *c2 = c->cmove(hrand(c->type));
      int cd2 = celldist(c2);
      // while(hrand(1000) < 1000 * chance) recurse(c);
      if(!fat && (cd2 > fatten_from || hrand(100000) == 0)) {
        recurse(c, FAT);
        fatten_from = FAT;
        }
      else if(fat) recurse(c, cd + dl * 6);
      rise(c, fat ? 256 : 1);
      c = c2;
      }
    }

  EX void dissolve_brownian(cell *c, int x) {
    if(c->land == laBrownian) {
      if(among(c->wall, waNone, waStrandedBoat, waMineOpen, waFire)) {
        if(c->landparam >= 4 * level) c->landparam = 4 * level - 1;
        c->landparam -= level * x;
        c->wall = waNone;
        if(c->landparam < 0) c->wall = waSea, c->landparam = 0;
        if(c->landparam == 0) c->landparam = 1;
        }
      }
    }

  EX void dissolve(cell *c, int x) {
    destroyTrapsAround(c);
    if(c->land == laBrownian) 
      dissolve_brownian(c, x);
    else if(c->wall == waRed2) c->wall = waRed1;
    else if(c->wall == waRed3) c->wall = waRed2;
    else if(among(c->wall, waRed1, waDeadfloor2, waRubble, waBoat, waFire, waCIsland, waCIsland2, waBigBush, waSmallBush)) c->wall = waNone;
    else if(c->wall == waStrandedBoat) c->wall = waNone;
    else if(c->wall == waFrozenLake) c->wall = waLake;
    else if(among(c->wall, waReptile, waGargoyleFloor) || cellUnstable(c)) c->wall = waChasm;
    else if(among(c->wall, waNone, waDock, waBurningDock, waFloorA, waFloorB, waCavefloor, waDeadfloor, waMineMine, waMineUnknown, waMineOpen, waOpenGate, waClosePlate, waOpenPlate, waGargoyleBridge, waReptileBridge))
      c->wall = waSea;
    else if(cellHalfvine(c)) destroyHalfvine(c, waNone, 4);
    }
  
  EX void init(cell *c) {
    if(!hyperbolic) return;
    recurse(c, FAT);
    recurse(c, FAT);
    }

  EX void init_further(cell *c) {
    if(!hyperbolic) return;
    int dl = getDistLimit();
    dynamicval<bool> be(generatingEquidistant, true);
    int gdir = -1;
    for(int i=0; i<c->type; i++) {
      if(c->move(i) && c->move(i)->mpdist < c->mpdist) gdir = i;
      }
    if(gdir < 0) return;
    
    cellwalker cw(c, gdir); 
    for(int i=0; i<4; i++) {
      cw += revstep;
      setdist(cw.at, BARLEV, cw.peek());
      buildEquidistant(cw.at);
      println(hlog, "from ", cw.peek(), " to ", cw.at, ", land = ", dnameof(cw.at->land), " lp = ", cw.at->landparam);
      }

    if(c->land != laOcean || !no_barriers_in_radius(cw.at, 2)) return;

    println(hlog, "brownian::init ", cw.at, " in distance ", celldistance(cw.at, cwt.at));

    recurse(cw.at, celldist(c) + dl * 3);
    recurse(cw.at, celldist(c) + dl * 3);

    cell *c2 = c;
    while(c2->mpdist > 7) {      
      forCellEx(c3, c2) if(c3->mpdist < c2->mpdist) { c2 = c3; goto next; }
      break;
      next: ;
      }
    if(!c2->monst && c2->wall != waBoat) c2->monst = moAcidBird;
    }
  
  EX void apply_futures(cell *c) {
    if(futures.count(c)) {
      auto m = move(futures[c]);
      futures.erase(c);
      for(auto p: m)
        recurse(p.first, p.second);
      futures.erase(c);
      printf("centersteps = %d futures = %d totalsteps = %d\n", centersteps, isize(futures), totalsteps);
      }
    }

  EX void build(cell *c, int d) {
  
    if(!hyperbolic) c->wall = waNone, c->landparam = 256;

    ONEMPTY {
      if(hrand(10000) < min(250, 100 + 2 * PT(kills[moAcidBird] + kills[moBrownBug], 50)) * (25 + min(items[itBrownian], 100)) / 25 && c->landparam >= 4 && c->landparam < 24)
        c->item = itBrownian;
      if(hrand_monster(8000) < 15 + items[itBrownian])
        c->monst = moAcidBird;
      else if(hrand_monster(8000) < 15)
        c->monst = moAlbatross;
      else if(hrand_monster(8000) < 15 + items[itBrownian]) {
        c->monst = moBrownBug;
        c->hitpoints = 3;
        }
      }
    }

  EX colortable colors = { 0x603000, 0x804000, 0xA05000, 0xC09050, 0xE0D0A0 };

  EX color_t get_color(int y) {
     return
        y < level ? gradient(colors[0], colors[1], 1, y, level-1) :
        y < 2 * level ? colors[2] :
        y < 3 * level ? colors[3] :
        colors[4];
     }

  EX color_t& get_color_edit(int y) {
     return
        y < level/2 ? colors[0] :
        y < level ? colors[1] :
        y < 2 * level ? colors[2] :
        y < 3 * level ? colors[3] :
        colors[4];
     }
  
  int hrc = addHook(hooks_removecells, 0, [] () {
    vector<cell*> to_remove;
    for(auto p: futures) if(is_cell_removed(p.first)) to_remove.push_back(p.first);
    for(auto r: to_remove) futures.erase(r);
    }) + addHook(clearmemory, 0, [] () { futures.clear(); })
    + addHook(hooks_gamedata, 0, [] (gamedata* gd) { gd->store(futures); });

EX }

EX namespace westwall {

  EX void switchTreasure(cell *c) {
    c->item = itNone;
    if(safety) return;
    if(hrand(5000) < PT(100 + 2 * (kills[moAirElemental] + kills[moWindCrow]), 200) && c->landparam >= 5 + items[itWest])
      c->item = itWest;
    else if(hrand(5000) < 20*PRIZEMUL)
      placeLocalOrbs(c);
    }

  EX int coastvalEdge1(cell *c) {
    if(c->land == laWestWall && !c->landparam) buildEquidistant(c);
    return coastvalEdge(c);
    }
  
  void build(vector<cell*>& whirlline, int d) {
    again: 
    cell *at = whirlline[isize(whirlline)-1];
    cell *prev = whirlline[isize(whirlline)-2];
    for(int i=0; i<at->type; i++) 
      if(at->move(i) && coastvalEdge1(at->move(i)) == d && at->move(i) != prev) {
        whirlline.push_back(at->move(i));
        goto again;
        }
    }
  
  void moveAt(cell *c, manual_celllister& cl) {
    if(cl.listed(c)) return;
    if(c->land != laWestWall) return;
    vector<cell*> whirlline;
    int d = coastvalEdge(c);
    whirlline.push_back(c);
    whirlline.push_back(gravity_state == gsAnti ? ts::right_of(c, coastvalEdge1) : ts::left_of(c, coastvalEdge1));
    build(whirlline, d);
    reverse(whirlline.begin(), whirlline.end());
    build(whirlline, d);
    int z = isize(whirlline);
    for(int i=0; i<z; i++) {
      cl.add(whirlline[i]);
      if(whirlline[i]->mpdist == BARLEV)
        switchTreasure(whirlline[i]);
      }
    for(int i=0; i<z-1; i++) {
      moveItem(whirlline[i], whirlline[i+1], true);
      if(whirlline[i]->item)
        animateMovement(match(whirlline[i+1], whirlline[i]), LAYER_BOAT);
      }
    for(int i=0; i<z; i++) 
      pickupMovedItems(whirlline[i]);
    }
  
  EX void move() {
    manual_celllister cl;
    if(gravity_state == gsLevitation) return;
    for(cell *c: dcal) moveAt(c, cl);
    // Keys and Orbs of Yendor always move
    using namespace yendor;
    for(int i=0; i<isize(yi); i++) {
      moveAt(yi[i].path[0], cl);
      // println(hlog, "coastval of actual key is ", coastvalEdge1(yi[i].actual_key()), " and item is ", dnameof(yi[i].actual_key()->item), "and mpdist is ", yi[i].actual_key()->mpdist);
      moveAt(yi[i].actual_key(), cl);
      if(yi[i].actualKey) {
        if(gravity_state == gsAnti) yi[i].age--;
        else yi[i].age++;
        setdist(yi[i].actual_key(), 8, NULL);
        }
      }
    }
EX }

EX namespace variant {
#if HDR
struct feature {
  color_t color_change;
  int rate_change;
  eMonster wanderer;
  void (*build)(cell*);
  };

extern array<feature, 21> features;
#endif

#define VF [] (cell *c)

array<feature, 21> features {{
  feature{(color_t)(-0x202020), 5, moNecromancer, VF {
    if(c->wall == waNone && hrand(1500) < 20) c->wall = waFreshGrave;
    if(hrand(20000) < 10 + items[itVarTreasure])
      c->monst = moNecromancer;
    }},
  {0x000010, 5, moLancer, VF { if(c->wall == waNone && !c->monst && hrand_monster(80000) < 25 + items[itVarTreasure]) c->monst = moLancer; } },
  {0x100008,15, moMonk, VF { if(c->wall == waNone && !c->monst && hrand_monster(80000) < 25 + items[itVarTreasure]) c->monst = moMonk; } },
  {0x080010, 5, moCrusher, VF { if(c->wall == waNone && !c->monst && hrand_monster(80000) < 25 + items[itVarTreasure]) c->monst = moCrusher; } },
  {0x181418, 5, moSkeleton, VF { if(c->wall == waNone && !c->monst && hrand_monster(80000) < 25 + items[itVarTreasure]) c->monst = moSkeleton, c->hitpoints = 3; } },
  {0x180000, 5, moPyroCultist, VF { if(c->wall == waNone && !c->monst && hrand_monster(80000) < 25 + items[itVarTreasure]) c->monst = moPyroCultist; } },
  {0x00000C, 2, moFlailer, VF { if(c->wall == waNone && !c->monst && hrand_monster(80000) < 25 + items[itVarTreasure]) c->monst = moFlailer; } },
  {0x1C0700, 1, moHedge, VF { if(c->wall == waNone && !c->monst && hrand_monster(80000) < 25 + items[itVarTreasure] && valence() == 3) c->monst = moHedge; } },
  {0x000c00,-1, moNone, VF { if(hrand(1500) < 30) createArrowTrapAt(c, laVariant); } },
  {0x001200,-1, moNone, VF { if(hrand(1500) < 50 && c->wall == waNone) c->wall = waTrapdoor; } },
  {0x000c18,-1, moNone, VF { if(hrand(1500) < 30) build_pool(c, true); } },
  {0x040A00,-1, moNone, VF { if(c->wall == waNone && !c->monst && !c->monst && hrand(1500) < 10) c->wall = waThumperOff; } },
  {0x080A00,-1, moNone, VF { if(hrand(1500) < 20 && !c->monst && !c->wall) c->wall = waFireTrap; } },
  {0x0C0A00, 0, moNone, VF { 
    bool inyendor = yendor::on && specialland == laVariant && celldist(c) < 7;
    int chance = inyendor ? 800 : 100;
    if(c->wall == waNone && !c->monst && hrand(5000) < chance) c->wall = waExplosiveBarrel; 
    } },
  {0x060D04, 0, moNone, VF { 
    if(c->wall == waNone && !c->monst && pseudohept(c) && hrand(30000) < 25 + items[itVarTreasure]) 
      if(buildIvy(c, 0, c->type) && !peace::on) c->item = itVarTreasure;
    }},
  {0x000A08, 0, moNone, VF { if(c->wall == waNone && !c->monst && hrand(5000) < 100) c->wall = waSmallTree; }},
  {0x100A10, 1, moRagingBull, VF { if(c->wall == waNone && hrand(10000) < 10 + items[itVarTreasure]) c->monst = moSleepBull, c->hitpoints = 3; }},
  {0x00110C, 0, moNone, VF { if(c->wall == waNone && !c->monst && hrand(5000) < 100) c->wall = waBigTree; }},
  {0x000A28, 1, moNone, VF { if(hrand(500) < 10) build_pool(c, false); } },
  {0x100A00, 2, moVariantWarrior, VF { if(c->wall == waNone && !c->monst && hrand(40000) < 25 + items[itVarTreasure]) c->monst = moVariantWarrior; }},
  {0x100708, 1, moRatling, VF { if(c->wall == waNone && !c->monst && hrand(50000) < 25 + items[itVarTreasure]) c->monst = moRatling; }}
  }};
#undef VF
EX }

EX namespace camelot {
/** number of Grails collected, to show you as a knight */
EX int knighted = 0;

/** this value is used when using Orb of Safety in the Camelot in Pure Tactics Mode */
EX int anthraxBonus = 0;

EX void roundTableMessage(cell *c2) {
  if(!euclid && !cwt.at->master->alt) return;
  if(!euclid && !c2->master->alt) return;
  int dd = celldistAltRelative(c2) - celldistAltRelative(cwt.at);

  bool tooeasy = (roundTableRadius(c2) < newRoundTableRadius());
            
  if(dd>0) {
    if(grailWasFound(cwt.at)) {
      addMessage(XLAT("The Knights congratulate you on your success!"));
      knighted = roundTableRadius(cwt.at);
      }
    else if(!tooeasy)
      addMessage(XLAT("The Knights laugh at your failure!"));
    }
  else {
    if(grailWasFound(cwt.at))
      addMessage(XLAT("The Knights stare at you!"));
    else if(tooeasy) {
      if(!tactic::on)
        addMessage(XLAT("Come on, this is too easy... find a bigger castle!"));
      }
    else
      addMessage(XLAT("The Knights wish you luck!"));
    }
  }

EX void knightFlavorMessage(cell *c2) {

  if(!eubinary && !c2->master->alt) {
    addMessage(XLAT("\"I am lost...\""));
    return;
    }

  if(tactic::on) {
    addMessage(XLAT("\"The Knights of the Horocyclic Table salute you!\""));
    return;
    }

  bool grailfound = grailWasFound(c2);
  int rad = roundTableRadius(c2);
  bool tooeasy = (rad < newRoundTableRadius());

  static int msgid = 0;

  retry:
  if(msgid >= 32) msgid = 0;  
  
  if(msgid == 0 && grailfound) {
    addMessage(XLAT("\"I would like to congratulate you again!\""));
    }
  else if(msgid == 1 && !tooeasy) {
    addMessage(XLAT("\"Find the Holy Grail to become one of us!\""));
    }
  else if(msgid  == 2 && !tooeasy) {
    addMessage(XLAT("\"The Holy Grail is in the center of the Round Table.\""));
    }
  #if CAP_CRYSTAL
  else if(msgid == 3 && cryst) {
    if(crystal::pure())
      addMessage(XLAT("\"Each piece of the Round Table is exactly %1 steps away from the Holy Grail.\"", its(roundTableRadius(c2))));
    else
      addMessage(XLAT("\"According to Merlin, the Round Table is a perfect Euclidean sphere in %1 dimensions.\"", its(ginf[gCrystal].sides/2)));
    }
  #endif
  else if(msgid == 3 && !peace::on && in_full_game()) {
    addMessage(XLAT("\"I enjoy watching the hyperbug battles.\""));
    }
  else if(msgid == 4 && in_full_game()) {
    addMessage(XLAT("\"Have you visited a temple in R'Lyeh?\""));
    }
  else if(msgid == 5 && in_full_game()) {
    addMessage(XLAT("\"Nice castle, eh?\""));
    }
  else if(msgid == 6 && items[itSpice] < 10 && !peace::on && in_full_game()) {
    addMessage(XLAT("\"The Red Rock Valley is dangerous, but beautiful.\""));
    }
  else if(msgid == 7 && items[itSpice] < 10 && !peace::on && in_full_game()) {
    addMessage(XLAT("\"Train in the Desert first!\""));
    }
  else if(msgid == 8 && sizes_known() && !tactic::on) {
    string s = "";
    if(0) ;
    #if CAP_CRYSTAL
    else if(cryst)
      s = crystal::get_table_boundary();
    #endif
    else if(!quotient)
      s = expansion.get_descendants(rad).get_str(100);
    if(s == "") { msgid++; goto retry; }
    addMessage(XLAT("\"Our Table seats %1 Knights!\"", s));
    }
  else if(msgid == 9 && sizes_known() && !tactic::on) {
    string s = "";
    if(0);
    #if CAP_CRYSTAL
    else if(cryst)
      s = crystal::get_table_volume();
    #endif
    else if(!quotient)
      s = expansion.get_descendants(rad-1, expansion.diskid).get_str(100);
    if(s == "") { msgid++; goto retry; }
    addMessage(XLAT("\"There are %1 floor tiles inside our Table!\"", s));
    }
  else if(msgid == 10 && !items[itPirate] && !items[itWhirlpool] && !peace::on && in_full_game()) {
    addMessage(XLAT("\"Have you tried to take a boat and go into the Ocean? Try it!\""));
    }
  else if(msgid == 11 && !princess::saved && in_full_game()) {
    addMessage(XLAT("\"When I visited the Palace, a mouse wanted me to go somewhere.\""));
    }
  else if(msgid == 12 && !princess::saved && in_full_game()) {
    addMessage(XLAT("\"I wonder what was there...\""));
    }
  else if(msgid == 13 && !peace::on && in_full_game()) {
    addMessage(XLAT("\"Be careful in the Rose Garden! It is beautiful, but very dangerous!\""));
    }
  else if(msgid == 14) {
    addMessage(XLAT("\"There is no royal road to geometry.\""));
    }
  else if(msgid == 15) {
    addMessage(XLAT("\"There is no branch of mathematics, however abstract, "));
    addMessage(XLAT("which may not some day be applied to phenomena of the real world.\""));
    }
  else if(msgid == 16) {
    addMessage(XLAT("\"It is not possession but the act of getting there, "));
    addMessage(XLAT("which grants the greatest enjoyment.\""));
    }
  else if(msgid == 17) {
    addMessage(XLAT("\"We live in a beautiful and orderly world, "));
    addMessage(XLAT("and not in a chaos without norms.\""));
    }                                                          
  else if(msgid == 25) {
    addMessage(XLAT("\"Thank you very much for talking, and have a great rest of your day!\""));
    }
  else {
    msgid++; goto retry;
    }

  msgid++;
  }
EX }

EX namespace mine {

EX bool uncoverMines(cell *c, int lev, int dist, bool just_checking) {
  bool b = false;
  if(c->wall == waMineMine && just_checking) return true;
  if(c->wall == waMineUnknown) {
    if(just_checking)
      return true;
    else {
      c->wall = waMineOpen;
      b = true;
      }
    }
  
  bool minesNearby = false;
  bool nominesNearby = false;
  bool mineopens = false;
  
  auto adj = adj_minefield_cells(c);

  for(cell *c2: adj) {
    if(c2->wall == waMineMine) minesNearby = true;
    if(c2->wall == waMineOpen) mineopens = true;
    if(c2->wall == waMineUnknown && !c2->item) nominesNearby = true;
    }

  if(lev && (nominesNearby || mineopens) && !minesNearby) for(cell *c2: adj)
    if(c2->wall == waMineUnknown || c2->wall == waMineOpen) {
      b |= uncoverMines(c2, lev-1, dist+1, just_checking);
      if(b && just_checking) return true;
      }

  if(minesNearby && !nominesNearby && dist == 0) {
    for(cell *c2: adj)
      if(c2->wall == waMineMine && c2->land == laMinefield)
        c2->landparam |= 1;
    }
  
  return b;
  }

EX bool mightBeMine(cell *c) {
  return c->wall == waMineUnknown || c->wall == waMineMine;
  }

EX hookset<bool(cell*)> *hooks_mark;

EX void performMarkCommand(cell *c) {
  if(!c) return;
  if(callhandlers(false, hooks_mark, c)) return;
  if(c->land == laCA && c->wall == waNone) 
    c->wall = waFloorA;
  else if(c->land == laCA && c->wall == waFloorA)
    c->wall = waNone; 
  if(c->land != laMinefield) return;
  if(c->item) return;
  if(!mightBeMine(c)) return;
  bool adj = false;
  forCellEx(c2, c) if(c2->wall == waMineOpen) adj = true;
  if(adj) c->landparam ^= 1;
  }

EX bool marked_mine(cell *c) {
  if(!mightBeMine(c)) return false;
  if(c->item) return false;
  if(c->land != laMinefield) return true;
  return c->landparam & 1;
  }

EX bool marked_safe(cell *c) {
  if(!mightBeMine(c)) return false;
  if(c->item) return true;
  if(c->land != laMinefield) return false;
  return c->landparam & 2;
  }

EX bool safe() {
  return items[itOrbAether];
  }

EX void uncover_full(cell *c2) {
  int mineradius = 
    bounded ? 3 :
    (items[itBombEgg] < 1 && !tactic::on) ? 0 :
    items[itBombEgg] < 20 ? 1 :
    items[itBombEgg] < 30 ? 2 :
    3;
  
  bool nomine = !normal_gravity_at(c2);
  if(!nomine && uncoverMines(c2, mineradius, 0, true) && markOrb(itOrbAether))
    nomine = true;
  
  if(!nomine) {
    uncoverMines(c2, mineradius, 0, false);
    mayExplodeMine(c2, moPlayer);
    }  
  }

EX void auto_teleport_charges() {
  if(specialland == laMinefield && firstland == laMinefield && bounded)
    items[itOrbTeleport] = isFire(cwt.at->wall) ? 0 : 1;
  }

EX }

EX namespace terracotta {
#if HDR
// predictable or not
static constexpr bool randterra = false;
#endif

EX void check(cell *c) {
  if(c->wall == waTerraWarrior && !c->monst && !racing::on) {
    bool live = false;
    if(randterra) {
      c->landparam++;
      if((c->landparam == 3 && hrand(3) == 0) ||
        (c->landparam == 4 && hrand(2) == 0) || 
        c->landparam == 5)
          live = true;
      }
    else {
      c->landparam--;
      live = !c->landparam;
      }
    if(live)
      c->monst = moTerraWarrior,
      c->hitpoints = 7,
      c->wall = waNone;
    }
  }

EX void check_around(cell *c) {
  forCellEx(c2, c)
    check(c2);
  }

EX void check() {
  for(int i=0; i<numplayers(); i++)
    forCellEx(c, playerpos(i)) {
      if(shmup::on) {
        forCellEx(c2, c)
          check(c2);
        }
      else
        check(c);
      }
  }
EX }

EX namespace ambush {

EX void mark(cell *c, manual_celllister& cl) {
  if(!cl.add(c)) return;
  forCellEx(c2, c) 
    if(c2->cpdist < c->cpdist) 
      mark(c2, cl);
  }

EX int distance;
EX bool ambushed;

EX void check_state() {
  if(havewhat & HF_HUNTER) {
    manual_celllister cl;
    for(cell *c: dcal) {
      if(c->monst == moHunterDog) {
        if(c->cpdist > distance)
          distance = c->cpdist;
        mark(c, cl);
        }
      if(c->monst == moHunterGuard && c->cpdist <= 4) 
        mark(c, cl);
      }
    if(items[itHunting] > 5 && items[itHunting] <= 22) {
      int q = 0;
      for(int i=0; i<numplayers(); i++) 
        forCellEx(c2, playerpos(i))
          if(cl.listed(c2))
            q++;
      if(q == 1) havewhat |= HF_FAILED_AMBUSH;
      if(q == 2) {
        for(int i=0; i<numplayers(); i++) 
        forCellEx(c2, playerpos(i))
          if(cl.listed(c2))
            forCellEx(c3, playerpos(i)) 
              if(c3 != c2 && isNeighbor(c2,c3))
              if(cl.listed(c3))
                havewhat |= HF_FAILED_AMBUSH;
        }
      if(havewhat & HF_FAILED_AMBUSH && ambushed) {
        addMessage(XLAT("The Hunting Dogs give up."));
        ambushed = false;
        }        
      }
    }
  
  }

EX int fixed_size;

EX int size(cell *c, eItem what) {
  bool restricted = false;
  for(cell *c2: dcal) {
    if(c2->cpdist > 3) break;
    if(c2->monst && !isFriendly(c2) && !slowMover(c2) && !isMultitile(c2)) restricted = true;
    }

  int qty = items[itHunting];
  if(fixed_size)
    return fixed_size;
  switch(what) {
    case itCompass:
      return 0;
    
    case itHunting:
      return min(min(qty, max(33-qty, 6)), 15);
    
    case itOrbSide3:
      return restricted ? 10 : 20;
    
    case itOrbFreedom:
      return restricted ? 10 : 60;
    
    case itOrbThorns:
    case itOrb37:
      return 20;
    
    case itOrbLava:
      return 20;
    
    case itOrbBeauty:
      return 35;
    
    case itOrbShell:
      return 35;

    case itOrbPsi:
      // return 40; -> no benefits
      return 20;

    case itOrbDash:
    case itOrbFrog:
      return 40;
    
    case itOrbAir:
    case itOrbDragon:
      return 50;

    case itOrbStunning:
      // return restricted ? 50 : 60; -> no benefits
      return 30;
    
    case itOrbBull:
    case itOrbSpeed:
    case itOrbShield:
      return 60;

    case itOrbInvis:
      return 80;

    case itOrbTeleport:
      return 300;
    
    case itGreenStone:
    case itOrbSafety:
    case itOrbYendor:
      return 0;
    
    case itKey:
      return 16;

    case itWarning:
      return qty;
    
    default:
      return restricted ? 6 : 10;
      break;    
    
    // Flash can survive about 70, but this gives no benefits
    }
  
  }

EX int ambush(cell *c, eItem what) {
  int maxdist = gamerange();
  celllister cl(c, maxdist, 1000000, NULL);
  cell *c0 = c;
  int d = 0;
  int dogs0 = 0;
  for(cell *cx: cl.lst) {
    int dh = cl.getdist(cx);
    if(dh <= 2 && cx->monst == moHunterGuard)
      cx->monst = moHunterDog, dogs0++;
    if(dh > d) c0 = cx, d = dh;
    }
  if(sphere) {
    int dogs = size(c, what);  
    for(int i = cl.lst.size()-1; i>0 && dogs; i--) 
      if(!isPlayerOn(cl.lst[i]) && !cl.lst[i]->monst)
        cl.lst[i]->monst = moHunterDog, dogs--;
    }
  vector<cell*> around;
  cell *clast = NULL;
  cell *ccur = c0;
  int v = valence();
  if(v > 4) {
    for(cell *c: cl.lst) if(cl.getdist(c) == d) around.push_back(c);
    hrandom_shuffle(&around[0], isize(around));
    }
  else {
    for(int tries=0; tries<10000; tries++) {
      cell *c2 = NULL;
      if(v == 3) {
        forCellEx(c1, ccur)
          if(c1 != clast && cl.listed(c1) && cl.getdist(c1) == d)
            c2 = c1;
        }
      if(v == 4) {
        for(int i=0; i<ccur->type; i++) {
          cell *c1 = (cellwalker(ccur, i) + wstep + 1).peek();
          if(!c1) continue;
          if(c1 != clast && cl.listed(c1) && cl.getdist(c1) == d)
            c2 = c1;        
          }
        }
      if(!c2) break;
      if(c2->land == laHunting && c2->wall == waNone && c2->monst == moNone)
        around.push_back(c2);
      clast = ccur; ccur = c2;
      if(c2 == c0) break;
      }
    }
  int N = isize(around);
  int dogs = size(c, what);
  
  int gaps = dogs;
  if(!N) return dogs0;
  ambushed = true;
  int shift = hrand(N);
  dogs = min(dogs, N);
  gaps = min(gaps, N);
  for(int i=0; i<dogs; i++) {
    int pos = (shift + (N * i) / gaps) % N;
    cell *nextdog = around[pos];
    nextdog->monst = moHunterDog;
    nextdog->stuntime = 1;
    drawFlash(nextdog);
    }
  return dogs + dogs0;
  }
EX }

}
#endif
