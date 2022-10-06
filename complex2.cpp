// Hyperbolic Rogue - Complex features part II
// Copyright (C) 2011-2019 Zeno Rogue, see 'hyper.cpp' for details

/** \file complex2.cpp 
 *  \brief Continuation of complex.cpp
 *
 *  Includes: Brownian, Irradiated, Free Fall
 */

#include "hyper.h"

namespace hr {

#if CAP_COMPLEX2
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
      auto m = std::move(futures[c]);
      futures.erase(c);
      for(auto p: m)
        recurse(p.first, p.second);
      futures.erase(c);
      printf("centersteps = %d futures = %d totalsteps = %d\n", centersteps, isize(futures), totalsteps);
      }
    }

  EX void build(cell *c, int d) {
  
    if(!hyperbolic) c->wall = waNone, c->landparam = 256;
    
    if(c->landparam == 0 && ls::single()) c->land = laOcean;

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
    }) + addHook(hooks_clearmemory, 0, [] () { futures.clear(); })
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
    if(looped(whirlline)) return;
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
      pickupMovedItems(whirlline[i], i<z-1 ? whirlline[i+1] : whirlline[0]);
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

bool hrand_var(int i) { return hrand_monster(i) < 25 + items[itVarTreasure] + yendor::hardness(); }

array<feature, 21> features {{
  feature{(color_t)(-0x202020), 5, moNecromancer, VF {
    if(c->wall == waNone && hrand(1500) < 20) c->wall = waFreshGrave;
    if(hrand(20000) < 10 + items[itVarTreasure])
      c->monst = moNecromancer;
    }},
  {0x000010, 5, moLancer, VF { if(c->wall == waNone && !c->monst && hrand_var(80000)) c->monst = moLancer; } },
  {0x100008,15, moMonk, VF { if(c->wall == waNone && !c->monst && hrand_var(80000)) c->monst = moMonk; } },
  {0x080010, 5, moCrusher, VF { if(c->wall == waNone && !c->monst && hrand_var(80000)) c->monst = moCrusher; } },
  {0x181418, 5, moSkeleton, VF { if(c->wall == waNone && !c->monst && hrand_var(80000)) c->monst = moSkeleton, c->hitpoints = 3; } },
  {0x180000, 5, moPyroCultist, VF { if(c->wall == waNone && !c->monst && hrand_var(80000)) c->monst = moPyroCultist; } },
  {0x00000C, 2, moFlailer, VF { if(c->wall == waNone && !c->monst && hrand_var(80000)) c->monst = moFlailer; } },
  {0x1C0700, 1, moHedge, VF { if(c->wall == waNone && !c->monst && hrand_var(80000) && valence() == 3) c->monst = moHedge; } },
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
  {0x100A10, 1, moRagingBull, VF { if(c->wall == waNone && !c->monst && hrand(10000) < 10 + items[itVarTreasure]) c->monst = moSleepBull, c->hitpoints = 3; }},
  {0x00110C, 0, moNone, VF { if(c->wall == waNone && !c->monst && hrand(5000) < 100) c->wall = waBigTree; }},
  {0x000A28, 1, moNone, VF { if(hrand(500) < 10) build_pool(c, false); } },
  {0x100A00, 2, moVariantWarrior, VF { if(c->wall == waNone && !c->monst && hrand_var(40000)) c->monst = moVariantWarrior; }},
  {0x100708, 1, moRatling, VF { if(c->wall == waNone && !c->monst && hrand_var(50000)) c->monst = moRatling; }}
  }};
#undef VF
EX }

EX namespace camelot {
/** number of Grails collected, to show you as a knight */
EX int knighted = 0;

/** this value is used when using Orb of Safety in the Camelot in Pure Tactics Mode */
EX int anthraxBonus = 0;

vector<string> knight_names = {
  "DivisionByZero", "Phoenyx", "tricosahedron", "BillSYZ0317", "rjdimo", "Person", "Strange Yeah", "paradoxica", "godamnsteemsux", "Allalinor", "Spuddnik",
  "QETC Crimson", "aca", "cannobeens", "Dylgear", "Patashu", "SirLight_XXVII", "Lord Ignus", "Lotho", "hotdogPi", "vincent", "pofoss", "Zekava", "Chimera245",
  "BreedPineapple", "TaeK", "Aliased", "Vipul", "jkvw3", "english5040", "gregwar69", "marvincast", "Panacea108", "FEDPOL1", "Jenkar", "inakilbss", "*****-mail",
  "Wheat Wizard", "xiqnyl", "zelda0x181e", "ad-jones", "mtomato", "Fififionek", "J Pystynen", "krstefan11", "green orange", "ZombieGirl1616", "z.chlebicki",
  "9427", "Lokki", "gabbalis", "The Horrendous Space Kablooie", "zeno", "ETsc2", "Fulgur14", "Roger", "uqblf", "Grep", "Abigail", "Cyberfox VII", "ballom", "CtrlAltDestroy",
  "RushSecond", "ozir", "Vee Nought", "archmageomega", "Wroclaw", "lunaduskjr", "LucLucLuc", "loper", "RedsToad", "Sharklilly", "Dasypus Daimon", "Mateusz", "joshua", "guysige",
  "MrSprucetree", "Atia", "nerdyjoe", "florrat", "bderoo121", "Sprite Guard", "fischpferd", "Fluffiest Princess", "no stop", "Kieroshark", "elgan65536", "T-Jski",
  "martinus de monte", "Inthar", "mj.titze", "thelast19digitsofpi", "abacussssss", "ann_dec", "juhdanad", "lllllllllwith10ls", "rsstein", "The Emerald Derp Leader",
  "NattieGilgamesh", "chocokels", "oren", "y", "Nathanbananas", "juliuskwan", "wojtekor9", "CoderBro", "RidiculousPyro64", "pringle", "Spicy", "Cheetahs", "ShiningKatana",
  "xy2", "starchewer", "2jjy", "Nibaw", "Goon in Chief", "the.goosh222", "Toras", "ooshoo", "TravelDemon", "The Big Extent", "droplet739", "AlliterativeAnchovies",
  "fones4jenke13", "ekisacik", "j0eymcgrath", "EatChangmyeong", "Strichcoder", "jwhirpl", "craus", "akei_arkay", "lilypad", "Ichigo Jam", "supernewton", "Neapolitan Sixth",
  "Nemo", "Westville", "Huja", "lapinozz", "Just A Witch", "Borador", "Particles", "dewhi100", "nat.mayer", "ddtizm", "Tism Jones", "amagikuser", "vkozulya", "gassa",
  "Factitious", "wonderfullizardofoz", "woofmao", "CandorVelexion", "Toricon", "Vectis99", "RobotNerd277", "jerrypoons", "MagmaMcFry", "unczane", "glass", "Wegener",
  "JeLomun", "kip", "Warmonger", "Fooruman", "Zyalin", "Prezombie", "ashley89", "bjobae", "MFErtre", "Roaringdragon2", "howilovepi", "Yulgash", "Sir Endipitous", "Roshlev",
  "BTernaryTau", "HiGuy", "coper", "Tirear", "qoala _", "Tyzone", "Tiegon", "Airin", "Metroid26", "Sklorg", "Fumblestealth", "Toph", "Tzaphqiel", "jruderman", "ray",
  "Deathroll", "Sinquetica", "mootmoot", "Noobinator", "freeofme", "Helyea", "Snakebird Priestess", "brisingre", "Khashishi", "Shiny", "kabado", "Berenthas", "Misery", "Altripp", "Aldrenean",
  };

map<cell*, int> knight_id;

EX string knight_name(cell *c) {
  if(!knight_id.count(c)) knight_id[c] = rand() % isize(knight_names);
  return knight_names[knight_id[c]];
  }

EX void move_knight(cell *c1, cell *c2) {
  LATE ( move_knight(c1, c2); )
  if(knight_id.count(c1)) {
    knight_id[c2] = knight_id[c1];
    knight_id.erase(c1);
    }
  }

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
  changes.value_keep(msgid);

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
    else if(!quotient && rad)
      s = get_expansion().get_descendants(rad).get_str(100);
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
    else if(!quotient && rad)
      s = get_expansion().get_descendants(rad-1, get_expansion().diskid).get_str(100);
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

EX int victory_time;

EX void count_status() {
  bool last = kills[moBomberbird];
  kills[moBomberbird] = 0;
  kills[moTameBomberbird] = 0;
  for(cell *c: currentmap->allcells()) if(c->wall == waMineUnknown) kills[moBomberbird]++;
  for(cell *c: currentmap->allcells()) if(among(c->wall, waMineMine, waMineUnknown) && mine::marked_mine(c)) kills[moTameBomberbird]++;
  if(last && !kills[moBomberbird]) {
    mine::victory_time = getgametime();
    showMissionScreen();
    }
  }

EX bool in_minesweeper() {
  return closed_or_bounded && specialland == laMinefield;
  }

EX bool uncoverMines(cell *c, int lev, int dist, bool just_checking) {
  bool b = false;
  if(c->wall == waMineMine && just_checking) return true;
  if(c->wall == waMineUnknown) {
    if(just_checking)
      return true;
    else {
      changes.ccell(c);
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
        changes.ccell(c2),
        c2->landparam |= 1;
    }
  
  return b;
  }

EX bool mightBeMine(cell *c) {
  return c->wall == waMineUnknown || c->wall == waMineMine;
  }

EX hookset<bool(cell*)> hooks_mark;

EX bool mark_always = true;

EX void performMarkCommand(cell *c) {
  if(!c) return;
  if(callhandlers(false, hooks_mark, c)) return;
  if(c->land == laCA && c->wall == waNone) {
    c->wall = ca::wlive;
    ca::list_adj(c);
    }
  else if(c->land == laCA && c->wall == ca::wlive) {
    c->wall = waNone; 
    ca::list_adj(c);
    }
  if(c->land != laMinefield) return;
  if(c->item) return;
  if(!mightBeMine(c)) return;
  bool adj = false;
  if(mark_always) adj = true;
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
    closed_or_bounded ? 3 :
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
  if(in_minesweeper()) {
    mine::count_status();
    items[itOrbTeleport] = isFire(cwt.at->wall) ? 0 : 1;
    }
  }

EX }

EX namespace terracotta {
#if HDR
// predictable or not
static constexpr bool randterra = false;
#endif

EX void check(cell *c) {
  if(c->wall == waTerraWarrior && !c->monst && !racing::on) {
    changes.ccell(c);
    bool live = false;
    if(randterra) {
      c->wparam++;
      if((c->wparam == 3 && hrand(3) == 0) ||
        (c->wparam == 4 && hrand(2) == 0) ||
        c->wparam == 5)
          live = true;
      }
    else {
      c->wparam--;
      live = !c->wparam;
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
  for(cell *pc: player_positions()) 
    forCellEx(c, pc) {
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
      for(cell *pc: player_positions()) 
        forCellEx(c2, pc)
          if(cl.listed(c2))
            q++;
      if(q == 1) havewhat |= HF_FAILED_AMBUSH;
      if(q == 2) {
        for(cell *pc: player_positions())
        forCellEx(c2, pc)
          if(cl.listed(c2))
            forCellEx(c3, pc)
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
    
    case itOrbImpact:
    case itOrbPlague:
      return 10;
    
    case itOrbThorns:
    case itOrb37:
    case itOrbChaos:
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

EX void ambush(cell *c, int dogs) {
  LATE ( ambush(c, dogs); )
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
    hrandom_shuffle(around);
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
  
  int gaps = dogs;
  int result = dogs0;
  if(N) {
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
    result += dogs;
    }

  if(result)
    addMessage(XLAT("You are ambushed!"));
  }

EX }

EX namespace dice {

  struct die_structure {
    vector<vector<int>> sides;
    vector<vector<int>> spins;
    vector<int> hardness;
    int faces;
    int facesides;
    int order;
    int highest_hardness;
    die_structure(int ord, const vector<vector<int>>& v) {
      sides = v;
      spins = sides;
      faces = isize(sides);
      facesides = isize(sides[0]);
      order = ord;
      for(int i=0; i<faces; i++)
        for(int j=0; j<isize(sides[i]); j++) {
          int i1 = sides[i][j];
          spins[i][j] = -1;
          for(int k=0; k<isize(sides[i1]); k++)
            if(sides[i1][k] == i)
              spins[i][j] = k;
          if(spins[i][j] == -1)
            println(hlog, "asymmetric");
          }
      hardness.resize(faces, 99);
      hardness.back() = 0;
      for(int it=0; it<faces; it++)
      for(int i=0; i<faces; i++)
        for(int j: sides[i])
          hardness[i] = min(hardness[i], hardness[j]+1);
      highest_hardness = 0;
      for(int i=0; i<faces; i++)
        highest_hardness = max(highest_hardness, hardness[i]);
      }
    };
  
  die_structure d20(5, {
    {13-1, 7-1, 19-1}, {20-1, 12-1, 18-1}, {19-1, 17-1, 16-1}, {14-1, 18-1, 11-1}, {13-1, 18-1, 15-1}, 
    {14-1, 9-1, 16-1}, { 1-1, 15-1, 17-1}, {20-1, 16-1, 10-1}, {19-1,  6-1, 11-1}, { 8-1, 17-1, 12-1},
    {13-1, 9-1,  4-1}, { 2-1, 10-1, 15-1}, { 1-1, 11-1,  5-1}, {20-1,  4-1,  6-1}, { 7-1,  5-1, 12-1},
    { 8-1, 6-1,  3-1}, { 7-1, 10-1,  3-1}, { 2-1,  5-1,  4-1}, { 1-1,  3-1,  9-1}, { 8-1,  2-1, 14-1}
    });
  
  die_structure d8(4, {
    {1, 3, 5}, {0, 4, 2}, {3, 1, 7}, {2, 6, 0}, {5, 7, 1}, {4, 0, 6}, {7, 5, 3}, {6, 2, 4}
    });

  die_structure d4(3, {{3,2,1}, {3,0,2}, {1,0,3}, {0,1,2}});

  die_structure d6(3, {{1,2,4,3}, {5,2,0,3}, {5,4,0,1}, {5,1,0,4}, {0,2,5,3}, {4,2,1,3}});

  die_structure d12(3, {
    {3,5,4,9,1}, {0,9,6,7,3}, {11,10,5,3,7}, {0,1,7,2,5}, {0,5,10,8,9}, {0,3,2,10,4},
    {11,7,1,9,8}, {11,2,3,1,6}, {11,6,9,4,10}, {0,4,8,6,1}, {11,8,4,5,2}, {8,10,2,7,6}
    });
  
  vector<die_structure*> die_list = {&d4, &d6, &d8, &d12, &d20};
    
  #if HDR
  extern vector<struct die_structure*> die_list;

  struct die_data {
    struct die_structure *which;
    int val; /* the current face value */
    int dir; /* which direction is the first side (which->sides[val][0]) of the current face */
    bool mirrored;
    int happy();
    };
  #endif
  
  EX int shape_faces(die_structure *w) { return w->faces; }
  
  EX string die_name(die_structure *w) { return its(w->faces); }

  int die_data::happy() {
    if(val == which->faces-1) return 1;
    if(val == 0) return -1;
    return 0;
    }
  
  EX die_structure *get_by_id(unsigned i) { return die_list[i % isize(die_list)]; }
  EX int get_die_id(die_structure *ds) { 
    for(int i=0; i<isize(die_list); i++)
      if(die_list[i] == ds)
        return i;
    return -1;
    }
  
  EX map<cell*, die_data> data;
        
  EX void generate_specific(cell *c, die_structure *ds, int min_hardness, int max_hardness) {
    auto& dd = data[c];
    dd.which = ds;
    vector<int> dirs;
    for(int i=0; i<c->type; i++) createMov(c, i);
    for(int i=0; i<c->type; i++) 
    for(int j=0; j<c->type; j++) if(can_roll(ds->facesides, i, movei(c, j)))
      dirs.push_back(i);
    if(dirs.empty())
      dd.dir = hrand(c->type);
    else
      dd.dir = hrand_elt(dirs);
    vector<int> sides;
    for(int i=0; i<ds->faces; i++) 
      if(ds->hardness[i] >= min_hardness && ds->hardness[i] <= max_hardness)
        sides.push_back(i);
    dd.val = hrand_elt(sides);
    }
  
  EX int die_possible(cell *c) {
    vector<int> res;
    for(int i: {3, 4, 5})
      if((c->type % i) == 0)
        res.push_back(i);
    if(res.empty()) return 0;
    return hrand_elt(res);
    }

  EX bool can_roll(int sides, int cur, movei mi) {
    if(mi.t->type % sides) return false;
    if((cur - mi.d) % (mi.s->type / sides)) return false;
    return true;
    }
  
  EX bool can_roll(movei mi) {
    auto& dd = data[mi.s];
    auto& dw = dd.which;
    return can_roll(dw->facesides, dd.dir, mi);
    }

  EX bool generate_random(cell *c) {
    vector<die_structure*> ds;
    for(die_structure* pds: {&d4, &d6, &d8, &d12, &d20})
      if(c->type % pds->facesides == 0)
        ds.push_back(pds);
    if(ds.empty()) return false;
    generate_specific(c, hrand_elt(ds), 0, 99);
    return true;
    }
  
  EX void generate_full(cell *c, int hard) {  
    int dp = die_possible(c);
    if(!dp) return;
    if(safety) return;
    int pct = hrand(100);
    int pct2 = hrand(6000);
    if(dp == 4) {
      if(pct < 20) {
        c->wall = (pct < (items[itOrbLuck] ? 9 : 11)) ? waRichDie : waHappyDie;
        generate_specific(c, &d6, 1, 2);
        }
      else if(pct2 < 40 + hard) {
        c->monst = moAnimatedDie;
        generate_specific(c, &d6, 0, 99);
        }
      return;
      }
    if(dp == 5) {
      if(pct < 20) {
        c->wall = (pct < (items[itOrbLuck] ? 9 : 11)) ? waRichDie : waHappyDie;
        generate_specific(c, &d12, 2, 3);
        }
      else if(pct2 < 40 + hard) {
        c->monst = moAnimatedDie;
        generate_specific(c, &d12, 0, 99);
        }
      return;
      }
    if(pct < 3) {
      c->wall = waHappyDie;
      generate_specific(c, &d4, 0, 0);
      }
    else if(pct < 6) {
      c->wall = waHappyDie;
      generate_specific(c, &d8, 0, 1);
      }
    else if(pct < (items[itOrbLuck] ? 8 : 9)) {
      c->wall = waHappyDie;
      generate_specific(c, &d20, 0, 1);
      }
    else if(pct < 14) {
      c->wall = waRichDie;
      if(items[itOrbLuck])
        generate_specific(c, &d20, 2, 3);
      else
        generate_specific(c, &d20, 4, 5);
      }
    else if(pct < 15) {
      c->wall = waRichDie;
      if(items[itOrbLuck])
        generate_specific(c, &d8, 1, 2);
      else
        generate_specific(c, &d8, 2, 3);
      }
    else if(pct2 < (items[itOrbLuck] ? 5 : 1)) {
      c->monst = moAnimatedDie;
      generate_specific(c, &d4, 0, 99);
      }
    else if(pct2 < 40) {
      c->monst = moAnimatedDie;
      generate_specific(c, &d8, 0, 99);
      }
    else if(pct2 < 40 + hard) {
      c->monst = moAnimatedDie;
      generate_specific(c, &d20, 0, 99);
      }    
    }

  EX die_data roll_effect(movei mi, die_data dd) {
    auto &cto = mi.t;
    auto &th = mi.s;
    
    int rdir = mi.dir_force();
    int t = th->type;

    int val = dd.val;
    int dir = dd.dir;
    
    auto& dw = dd.which;
    
    int si = dw->facesides;
    
    if(t % si) { println(hlog, "error: bad roll"); return dd; }
    
    int sideid = gmod((rdir - dir) * (dd.mirrored?-1:1) * si / t, si);
    
    int val1 = dw->sides[val][sideid];
    
    int si1 = isize(dw->sides[val1]);
    
    int sideid1 = dw->spins[val][sideid];
    
    int t1 = cto->type;
    if(t1 % si1) { println(hlog, "error: bad roll target"); return dd; }
    
    int rdir1 = mi.rev_dir_force();
    
    bool mirror1 = dd.mirrored ^ mi.mirror();
    
    int dir1 = rdir1 - (mirror1?-1:1) * sideid1 * t1 / si1;
    
    dir1 = gmod(dir1, t1);
    
    dd.mirrored = mirror1;
    dd.val = val1;
    dd.dir = dir1;
    return dd;
    }
  
  EX bool on(cell *c) {
    return isDie(c->wall) || isDie(c->monst);
    }
      
  EX string describe(cell *c) {
    if (!data.count(c)) return "BUG: die data missing";
    else if (!data[c].which) return "BUG: die data default-initialized";
    else return XLAT("d%1 rolled %2", its(data[c].which->faces), its(data[c].val + 1));
  }

  EX void roll(movei mi) {
    auto &cto = mi.t;
    auto &th = mi.s;
    
    changes.map_value(data, cto);
    changes.map_value(data, th);
    data[cto] = roll_effect(mi, data[th]);
    data.erase(th);
    }

  EX void draw_die(cell *c, const shiftmatrix& V, ld scale, color_t color) {
    if(!data.count(c)) {
      queuepoly(V, cgi.shAsymmetric, 0xFF0000FF);
      return;
      }

    /* priority used for dice */
    const auto prio = PPR::BIGSTATUE;

    eGeometry orig = geometry;
    bool fpp = GDIM == 3;

    auto& dd = data[c];
    int val = dd.val;
    int dir = dd.dir;
    auto& dw = dd.which;
    
    int si = dw->facesides;

    if(c == lmouseover_distant) {
      set<cell*> visited;
      struct celldata_t {
        cell* c;
        die_data dd;
        shiftmatrix V;
        };      
      vector<celldata_t> data;
      auto visit = [&] (cell *c, die_data dd, shiftmatrix V) {
        if(visited.count(c)) return;
        visited.insert(c);
        data.emplace_back(celldata_t{c, dd, V});
        };
      visit(c, dd, V);
      for(int i=0; i<isize(data); i++) {
        auto dat = data[i];
        int wa = dw->hardness[dat.dd.val];
        int wb = dw->highest_hardness;
        unsigned int red = ((wa * 0x00) + ((wb-wa) * 0xff)) / wb;
        color_t col = 0xFF0000FF + (red << 16);
        queuestr(fpp ? dat.V * zpush(cgi.FLOOR) : dat.V, .5, its(dat.dd.val+1), col);
        if(i <= 22)
        forCellIdEx(c2, id, dat.c) if(can_roll(si, dat.dd.dir, movei(dat.c, id)) && !visited.count(c2)) {
          auto re = roll_effect(movei(dat.c, id), dat.dd);
          shiftmatrix V2 = dat.V * currentmap->adj(dat.c, id);
          gridline(dat.V, C0, V2, C0, 0xFF800080, 0);
          visit(c2, re, V2);
          }
        }
      }
    else if(!lmouseover_distant || !on(lmouseover_distant)) if(CAP_EXTFONT || vid.usingGL == false) {
      queuestr(V, .5, its(val+1), 0xFFFFFFFF);
      auto& side = dw->sides[val];
      for(int i=0; i<si; i++) {
        int d = dir + c->type * i / isize(side);
        d = gmod(d, c->type);
        hyperpoint nxt = tC0(currentmap->adj(c, d));
        hyperpoint mid = normalize(C0 * 1.3 + nxt * -.3);
        queuestr(V * rgpushxto0(mid), .25, its(side[i]+1), 0xFFFFFFFF);
        }
      }
    
    shiftmatrix V1 = V * ddspin(c, dir) * spin(M_PI);
    if(dd.mirrored) V1 = V1 * MirrorY;
    
    // loop:

    vector<bool> face_drawn(dd.which->faces, false);
    
    vector<pair<transmatrix, int> > facequeue;
    
    auto add_to_queue = [&] (const transmatrix& T, int d) {
      if(face_drawn[d]) return;
      face_drawn[d] = true;
      facequeue.emplace_back(T, d);
      };
    
    transmatrix S = Id;
    
    ld outradius, inradius;
    
    if(1) {
      dynamicval<eGeometry> g(geometry, gSphere);
      ld alpha = 360 * degree / dw->order;
      ld beta = 180 * degree / dw->facesides;
      inradius  = edge_of_triangle_with_angles(alpha, beta, beta);
      outradius = edge_of_triangle_with_angles(beta, alpha, beta);
      }

    hyperpoint shift = inverse_shift(V1, tC0(die_target));
    hyperpoint log_shift = inverse_exp(shiftless(shift)) * (inradius / cgi.hexhexdist);
    
    if(1) {
      dynamicval<eGeometry> g(geometry, gSphere);
      hyperpoint de = direct_exp(log_shift);
      S = rgpushxto0(de);
      if(GDIM == 3) {
        for(int i=0; i<4; i++) swap(S[i][2], S[i][3]);
        for(int i=0; i<4; i++) swap(S[2][i], S[3][i]);
        }
      for(int i=0; i<4; i++) S[i][1] *= -1;
      }
    
    add_to_queue(S, val);

    ld dieradius = cgi.scalefactor * scale / 2;
    
    if(dw->faces == 20) dieradius /= 1.3;
    if(dw->faces == 8) dieradius /= 1.15;
    if(dw->faces == 12) dieradius /= 1.15;
    
    if(hyperbolic) dieradius /= 1.3;
    
    ld base_to_base;
    
    bool osphere = sphere && GDIM == 2;
    bool oeuclid = euclid && GDIM == 2;
    eGeometry highdim = 
      (GDIM == 3) ? geometry :
      hyperbolic ? gSpace534 : gCubeTiling;
    
    if(1) {
      dynamicval<eGeometry> g(geometry, highdim);
      hyperpoint h = cspin(2, 0, M_PI-outradius) * zpush0(-dieradius);
      if(osphere || oeuclid)
        base_to_base = -h[2];
      else {
        ld lim = sphere ? 1 : 5;
        base_to_base = binsearch(-lim, lim, [h] (ld d) {
          return (zpush(d) * h)[2] >= sin_auto(vid.depth);
          });
        }
      }
    
    vector<pair<ld, int> > ordering;
    
    for(int i=0; i<dw->faces; i++) {
    
      transmatrix T = facequeue[i].first;
      int ws = facequeue[i].second;
      
      for(int d=0; d<si; d++) {
        dynamicval<eGeometry> g(geometry, highdim);
        add_to_queue(T * cspin(0, 1, 2*M_PI*d/si) * cspin(2, 0, inradius) * cspin(0, 1, M_PI-2*M_PI*dw->spins[ws][d]/si), dw->sides[ws][d]);
        }
      
      if(1) {
        dynamicval<eGeometry> g(geometry, highdim);
        hyperpoint h = zpush(base_to_base) * T * zpush0(dieradius);
        ld z = fpp ? hdist0(h) : asin_auto(h[2]);
        ordering.emplace_back(-z, i);
        }
      }
    
    sort(ordering.begin(), ordering.end());
    
    for(auto o: ordering) {
      int i = o.second;
      transmatrix T = facequeue[i].first;

      array<hyperpoint, 5> face;
      
      hyperpoint dctr;
      if(1) {
        dynamicval<eGeometry> g(geometry, highdim);
        dctr = zpush(base_to_base) * C0;
        }
      
      auto sphere_to_space = [&] (hyperpoint h) {
        if(fpp) return h;
        if(osphere) {
          h[2] = 1 - h[2]; h[3] = 0;
          return h;
          }
        if(oeuclid) { h[2] = 1-h[2]; return h; }
        ld z = asin_auto(h[2]);
        h = zpush(-z) * h;
        h[2] = h[3]; h[3] = 0;
        dynamicval<eGeometry> g(geometry, orig);
        return zshift(h, geom3::scale_at_lev(z));
        };

      for(int d=0; d<=si; d++) {
        hyperpoint h, hs;
        if(1) {
          dynamicval<eGeometry> g(geometry, highdim);
          h = zpush(base_to_base) * T * cspin(0, 1, 2*M_PI*(d+.5)/si) * cspin(2, 0, outradius) * zpush0(dieradius);
          if(d < si) face[d] = h;
          hs = sphere_to_space(h);
          }
        curvepoint(hs);
        }
      
      hyperpoint ctr, cx, cy;
      if(dw->facesides == 3) {
        dynamicval<eGeometry> g(geometry, highdim);
        ctr = (face[0] + face[1] + face[2]) / 3;
        ctr = ctr * 1.01 - dctr * 0.01;
        cx = face[2] - face[0];
        cy = face[1] - (face[0] + face[2]) / 2;
        }
      if(dw->facesides == 4) {
        dynamicval<eGeometry> g(geometry, highdim);
        ctr = (face[0] + face[1] + face[2] + face[3]) / 4;
        ctr = ctr * 1.01 - dctr * 0.01;
        cx = face[1] - face[2];
        cy = face[0] - face[1];
        }
      if(dw->facesides == 5) {
        dynamicval<eGeometry> g(geometry, highdim);
        ctr = (face[0] + face[1] + face[2] + face[3] + face[4]) / 5;
        ctr = ctr * 1.01 - dctr * 0.01;
        cx = (face[2] - face[0]) * .75;
        cy = face[1] - (face[3] + face[4]) * .4;
        }
      
      queuecurve(V1, (poly_outline == OUTLINE_NONE) ? 0xFFFFFFFF : poly_outline, color & 0xFFFFFF9F, prio);
      
      #if !CAP_EXTFONT
      if(!vid.usingGL) continue;
      #if CAP_GL
      pointfunction pf = [&] (ld x, ld y) {
        dynamicval<eGeometry> g(geometry, highdim);
        return sphere_to_space(normalize(ctr + cx * x + cy * y));
        };
      
      if(dw == &d4) {
        int q = facequeue[i].second;
        for(int j=0; j<3; j++) {
          int j1 = (j+1)%3;
          int j2 = (j+2)%3;
          if(1) {
            dynamicval<eGeometry> g(geometry, highdim);
            ctr = (face[0] + face[1] + face[2] + face[j1] * 3) / 6;
            ctr = ctr * 1.01 - dctr * 0.01;
            cx = (face[j2] - face[j]) / 2;
            cy = face[j1] - (face[j] + face[j2]) / 4;
            }
          write_in_space(V1, max_glfont_size, -1.2, its(1+dw->sides[q][j]), 0xFFFFFFFF, 0, 8, prio, pf);
          }
        }
      else {
        int fid = dw->faces - facequeue[i].second;
        string s;
        if(fid == 6) s = "6.";
        else if(fid == 9) s = "9.";
        else s = its(fid);
        write_in_space(V1, max_glfont_size, dw->faces < 10 ? -1.2 : -.75, s, 0xFFFFFFFF, 0, 8, prio, pf);
        }
      #endif
      #endif
      }
    }

  EX bool swap_forbidden(cell *ca, cell *cb) {
    if(!on(ca)) return false;
    return cb->type % data[ca].which->facesides;
    }

  EX void chaos_swap(cellwalker wa, cellwalker wb) {
    swap_data(data, wa.at, wb.at);
    if(on(wa.at)) {
      auto& d = data[wa.at];
      d.dir = chaos_mirror_dir(d.dir, wb, wa);
      d.mirrored = !d.mirrored;
      }
    if(on(wb.at)) {
      auto& d = data[wb.at];
      d.dir = chaos_mirror_dir(d.dir, wa, wb);
      d.mirrored = !d.mirrored;
      }
    }

  int hook = addHook(hooks_clearmemory, 0, [] () { data.clear(); });
EX }

#endif

#if !CAP_COMPLEX2
EX namespace dice {
  EX bool on(cell *c) { return false; }
  EX bool swap_forbidden(cell *a, cell *b) { return false; }
  EX void chaos_swap(cellwalker wa, cellwalker wb) {}
EX }

EX namespace mine {
  EX bool in_minesweeper() { return false; }
EX }
#endif
}
