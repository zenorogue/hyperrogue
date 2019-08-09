// Hyperbolic Rogue

// namespaces for new complex lands for HR11

// Copyright (C) 2011-2019 Zeno Rogue, see 'hyper.cpp' for details

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

  void dissolve_brownian(cell *c, int x) {
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

  void dissolve(cell *c, int x) {
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

  void init_further(cell *c) {
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
  
  void apply_futures(cell *c) {
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

  colortable colors = { 0x603000, 0x804000, 0xA05000, 0xC09050, 0xE0D0A0 };

  color_t get_color(int y) {
     return
        y < level ? gradient(colors[0], colors[1], 1, y, level-1) :
        y < 2 * level ? colors[2] :
        y < 3 * level ? colors[3] :
        colors[4];
     }

  color_t& get_color_edit(int y) {
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

  }

namespace westwall {

  void switchTreasure(cell *c) {
    c->item = itNone;
    if(safety) return;
    if(hrand(5000) < PT(100 + 2 * (kills[moAirElemental] + kills[moWindCrow]), 200) && c->landparam >= 5 + items[itWest])
      c->item = itWest;
    else if(hrand(5000) < 20*PRIZEMUL)
      placeLocalOrbs(c);
    }

  int coastvalEdge1(cell *c) {
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
        animateMovement(whirlline[i+1], whirlline[i], LAYER_BOAT, NOHINT);
      }
    for(int i=0; i<z; i++) 
      pickupMovedItems(whirlline[i]);
    }
  
  void move() {
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
  }

struct variant_feature {
  color_t color_change;
  int rate_change;
  eMonster wanderer;
  void (*build)(cell*);
  };

#define VF [] (cell *c)

const array<variant_feature, 21> variant_features {{
  variant_feature{(color_t)(-0x202020), 5, moNecromancer, VF {
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
  {0x1C0700, 1, moHedge, VF { if(c->wall == waNone && !c->monst && hrand_monster(80000) < 25 + items[itVarTreasure] && VALENCE == 3) c->monst = moHedge; } },
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

}
#endif
