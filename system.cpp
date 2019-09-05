// Hyperbolic Rogue -- starting and ending games
// Copyright (C) 2011-2019 Zeno Rogue, see 'hyper.cpp' for details

/** \file system.cpp
 *  \brief changing game modes, starting, closing, loading and saving games
 */

#include "hyper.h"
namespace hr {

#if HDR
namespace rg {
  // possible parameters e.g. for restart_game and wrongmode
  static const char nothing = 0;
  static const char peace = 'P';
  static const char inv = 'i';
  static const char chaos = 'C';
  static const char tactic = 't';
  static const char tour = 'T';
  static const char yendor = 'y';
  static const char shmup = 's';
  static const char randpattern = 'r';
  static const char princess = 'p';
  static const char daily = 'd';
  static const char daily_off = 'D';
  static const char racing = 'R';
  static const char dualmode = 'U';
  
  // wrongmode only -- marks 'global' achievements not related to the current mode
  static const char global = 'x'; 
  // wrongmode only -- change vid.scfg.players then restart_game(rg::nothing) instead
  static const char multi = 'm';
  // wrongmode only -- mark achievements for special geometries/variations
  static const char special_geometry = 'g';
  }
#endif

EX bool game_active;

EX bool cblind;
EX bool autocheat;
EX bool canvas_invisible;

EX int truelotus;
EX int gamecount;

EX int asteroids_generated, asteroid_orbs_generated;

EX time_t timerstart, savetime;
EX bool timerstopped;
int savecount;
EX bool showoff = false;
EX bool doCross = false;

bool gamegen_failure;

EX eLand top_land;

bool verless(string v, string cmp) {
  if(isdigit(v[0]) && isdigit(v[1])) 
    v = "A" + v;
  if(isdigit(cmp[0]) && isdigit(cmp[1]))
    cmp = "A" + cmp;
  return v < cmp;
  }

bool do_use_special_land() {
  return
    !safety &&
    (peace::on || tactic::on || geometry || NONSTDVAR || randomPatternsMode || yendor::on || racing::on);
  }

hookset<bool()> *hooks_welcome_message;

void welcomeMessage() {
  if(callhandlers(false, hooks_welcome_message)) return;
  else if(tactic::trailer) return;
#if CAP_TOUR
  else if(tour::on) return; // displayed by tour
#endif
  else if(princess::challenge) {
    kills[moVizier] = 1;
    princess::forceMouse = true;
    if(yendor::everwon)
      items[itGreenStone] = 99;
    addMessage(XLAT("Welcome to %the1 Challenge!", moPrincess));
    addMessage(XLAT("The more Hypersian Rugs you collect, the harder it is.", moPrincess));
    }  
  else if(randomPatternsMode)
    addMessage(XLAT("Welcome to the Random Pattern mode!"));
  else if(tactic::on)
    addMessage(XLAT("You are playing %the1 in the Pure Tactics mode.", firstland));
  else if(yendor::on)
    addMessage(XLAT("Welcome to the Yendor Challenge %1!", its(yendor::challenge)));
  else if(peace::on) ; // no welcome message
  else if(shmup::on) ; // welcome message given elsewhere
  else if(euclid)
    addMessage(XLAT("Welcome to the Euclidean mode!"));
  else if(specialland == laHalloween && BITRUNCATED && among(geometry, gSphere, gElliptic))
    addMessage(XLAT("Welcome to Halloween!"));
  else if(elliptic && WDIM == 2)
    addMessage(XLAT("Good luck in the elliptic plane!"));
  else if(elliptic)
    addMessage(XLAT("Good luck in the elliptic space!"));
  else if(sphere)
    addMessage(XLAT("Welcome to Spherogue!"));
  else if(sol)
    addMessage(XLAT("Welcome to SolvRogue!"));
  else if(nil)
    addMessage(XLAT("Welcome to NilRogue!"));
  else if(sl2) {
    addMessage(XLAT("Welcome to PSL(2,R)-ogue!"));
    if(hybrid::underlying == gNormal && BITRUNCATED)
      addMessage(XLAT("Hint: this is more playable with pure {7,3} or pure {5,4}"));
    }
  else if(PURE && geometry == gNormal && !cheater)
    addMessage(XLAT("Welcome to the Heptagonal Mode!"));
  else if(cheater || autocheat)
    addMessage(XLAT("Welcome to HyperRogue! (cheat mode on)"));
  else
    addMessage(XLAT("Welcome to HyperRogue!"));

  if(do_use_special_land() || firstland != laIce) if(!daily::on) {
    auto lv = land_validity(specialland);
    if(lv.flags & lv::display_error_message)
      addMessage(XLAT(lv.msg));
    }

#if ISMAC
  addMessage(XLAT("Press F1 or right-shift-click things for help."));
#elif !ISMOBILE
  addMessage(XLAT("Press F1 or right-click things for help."));
#endif
  }

int trailer_cash0 = 5;
int trailer_cash1 = 15;
bool trailer_safety = true;

hookset<void()> *hooks_initgame;

// initialize the game
EX void initgame() {
  DEBBI(DF_INIT, ("initGame"));
  callhooks(hooks_initgame); 

  if(multi::players < 1 || multi::players > MAXPLAYER)
    multi::players = 1;
  multi::whereto[0].d = MD_UNDECIDED;
  multi::cpid = 0;
  
  yendor::init(1);
  
  if(safety && safetyseed) {
    shrand(safetyseed);
    firstland = safetyland;
    }

  #if CAP_RACING  
  if(racing::on) racing::apply_seed();
  #endif
  
  bool use_special_land = do_use_special_land();
    
  if(use_special_land) firstland = specialland;
  
  if(firstland == laNone || firstland == laBarrier)
    firstland = laCrossroads;

  if(firstland == laCrossroads5 && !tactic::on) firstland = laCrossroads2; // could not fit!
  if(firstland == laOceanWall) firstland = laOcean; 
  if(firstland == laHauntedWall) firstland = laGraveyard; 
  if(firstland == laMercuryRiver) firstland = laTerracotta;
  if(firstland == laMountain && !tactic::on) firstland = laJungle;
  if(firstland == laPrincessQuest) firstland = laPalace;
  if((isGravityLand(firstland) && !isCyclic(firstland)) || (firstland == laOcean && !safety && !yendor::on)) 
    firstland = weirdhyperbolic ? laCrossroads4 : laCrossroads;
  
  clear_euland(specialland);

  cwt.at = currentmap->gamestart(); cwt.spin = 0; cwt.mirrored = false;
  cwt.at->land = firstland;

  if(firstland == laBrownian) brownian::init(cwt.at);
  
  chaosAchieved = false;

  if(firstland == laElementalWall) cwt.at->land = randomElementalLand();
  
  resetview();  
  createMov(cwt.at, 0);
  
  pregen();  
  setdist(cwt.at, BARLEV, NULL);

  if(!use_special_land && !safety) {
    if(firstland != (princess::challenge ? laPalace : laIce)) cheater++;
    }

  if((tactic::on || yendor::on || peace::on) && isCyclic(firstland)) {
    anthraxBonus = items[itHolyGrail];
    cwt.at->move(0)->land = firstland;
    if(firstland == laWhirlpool) cwt.at->move(0)->wall = waSea;
    
    setdist(cwt.at->move(0), BARLEV-1, cwt.at);
    if(!sphere && !eubinary && !quotient) {
      heptagon *h = createAlternateMap(cwt.at, 2, hsA);
      if(!h) printf("FAIL\n");
      }
    }
  
  if(tactic::on && firstland == laPower) {
    items[itOrbSpeed] = 30;
    items[itOrbWinter] = 30;
    items[itOrbFlash] = 30;
    }

  if(tactic::on && firstland == laCaribbean) {
    if(hiitemsMax(itRedGem) >= 25) items[itRedGem] = min(hiitemsMax(itRedGem), 50);
    if(hiitemsMax(itFernFlower) >= 25) items[itFernFlower] = min(hiitemsMax(itFernFlower), 50);
    if(hiitemsMax(itWine) >= 25) items[itWine] = min(hiitemsMax(itWine), 50);
    }
  
  if(tactic::on && tactic::trailer)
    items[treasureType(firstland)] = trailer_cash0;

  yendor::lastchallenge = yendor::challenge;
  
  if(shmup::on) shmup::init();
  
  yendor::init(2);
  
  #if CAP_RACING
  if(racing::on) racing::generate_track();
  #endif
  
  if(gamegen_failure) return;
  
  if(euclid && specialland == laPrincessQuest) {
    cell *c = *euclideanAtCreate(pair_to_vec(EPX, EPY)).first;
    princess::generating = true;
    c->land = laPalace;
    setdist(c, 7 - getDistLimit() - genrange_bonus, NULL);
    princess::generating = false;
    }

  if(cwt.at->land == laCrossroads2) {
    cell *c = cwt.at;
    if(hybri) { c = hybrid::get_where(c).first; hybrid::in_underlying_map([&] { c->cmove(0); }); }
    c->landparam = 12;
    c->cmove(0)->landparam = 44;
    c->cmove(0)->land = laCrossroads2;
    }
    
  sword::determine_sword_angles();
  for(int i=0; i<numplayers(); i++) 
    sword::dir[i] = sword::initial(cwt.at);

  #if CAP_DAILY
  daily::split();
  #endif
  
  // extern int sightrange; sightrange = 9;
  // cwt.at->land = laHell; items[itHell] = 10;
  for(int i=BARLEV; i>=7 - getDistLimit() - genrange_bonus; i--) {
    if(tactic::trailer && cwt.at->land != laClearing) safety = trailer_safety;
    setdist(cwt.at, i, NULL);
    if(tactic::trailer) safety = false;

    currentmap->verify();
    }
  
  if(doCross) {
    for(int i=0; i<ittypes; i++) if(itemclass(eItem(i)) == IC_TREASURE) items[i] = 50;
    for(int i=0; i<motypes; i++) kills[i] = 30;
    items[itSavedPrincess] = 0;
    kills[moPrincessMoved] = 0;
    kills[moPrincessArmedMoved] = 0;
    kills[moPlayer] = 0;
    }
  
  if(quotient && generateAll(firstland)) {
    for(int i=0; i<isize(currentmap->allcells()); i++)
      setdist(currentmap->allcells()[i], 8, NULL);
    }

  
  if(multi::players > 1 && !shmup::on) for(int i=0; i<numplayers(); i++) {
    int idir = (3 * i) % cwt.at->type;
    multi::player[i].at = cwt.at->move(idir);
    // special case -- otherwise they land on a wall
    if(firstland == laCrossroads2 && i == 1)
      multi::player[1].at = cwt.at;
    if(firstland == laCrossroads2 && i == 6)
      multi::player[6].at = createMov(createMov(cwt.at, 0), 3);
    setdist(cwt.at->move(idir), 7 - getDistLimit() - genrange_bonus, cwt.at);
    multi::player[i].spin = 0;
    multi::flipped[i] = true;
    multi::whereto[i].d = MD_UNDECIDED;
    }
    
  if(tactic::on && tactic::trailer)
    items[treasureType(firstland)] = trailer_cash1;
  
  yendor::init(3);
  peace::simon::init();
  
  multi::revive_queue.clear();
#if CAP_TOUR
  if(tour::on) tour::presentation(tour::pmRestart);
#endif
  
  if(multi::players > 1 && !shmup::on) {
    for(int i=0; i<numplayers(); i++) 
      makeEmpty(playerpos(i));
    }
  else {
    for(int i=0; i<numplayers(); i++) 
      makeEmpty(cwt.at);
    }
  
  if(specialland == laMinefield && bounded) {
    bfs();
    generate_mines();
    }
  
  princess::squeaked = false;
  clearing::current_root = NULL;
  
  if(!safety) {
    usedSafety = false;
    timerstart = time(NULL); turncount = 0; rosewave = 0; rosephase = 0;
    patterns::whichShape = 0;
    noiseuntil = 0;
    sagephase = 0; hardcoreAt = 0;
    timerstopped = false;
    savecount = 0; savetime = 0;
    cheater = 0;
    if(autocheat) cheater = 1;
    hauntedWarning = false;
    if(!autocheat) {
      timerghost = true;
      gen_wandering = true;
      }
    truelotus = 0;
    asteroids_generated = 0;
    asteroid_orbs_generated = 0;
    survivalist = true;
    #if CAP_CRYSTAL
    crystal::used_compass_inside = false;
    #endif
    got_survivalist = false;
#if CAP_INV
    if(inv::on) inv::init();
#endif
    auto_teleport_charges();
    if(!use_special_land) {
      if(firstland != (princess::challenge ? laPalace : laIce)) cheater++;
      }
    welcomeMessage();
    }
  else {
    usedSafety = true;
    safety = false;
    }
  
  havewhat = hadwhat = 0; rosemap.clear();
  
  elec::lightningfast = 0;
  
  lastsafety = gold();
  bfs();
  checkmove();
  playermoved = true;
  
  if(quotient || sphere)
    for(cell *c: currentmap->allcells()) setdist(c, 8, NULL);

  if(!allowChangeRange()) {
    gamerange_bonus = genrange_bonus = 0;
    if(vid.use_smart_range == 2) vid.use_smart_range = 1;
    }
  if(!allowIncreasedSight()) vid.use_smart_range = 0;
  }

bool havesave = true;

#if CAP_SAVE
#define MAXBOX 500
#define POSSCORE 371 // update this when new boxes are added!

struct score {
  string ver;
  int box[MAXBOX];
  };

int savebox[MAXBOX], boxid;
bool saving, loading, loadingHi;

string boxname[MAXBOX];
bool fakebox[MAXBOX];
bool monsbox[MAXBOX];

void applyBox(int& t) {
  if(saving) savebox[boxid++] = t;
  else if(loading) t = savebox[boxid++];
  else boxid++;
  }

EX void applyBoxNum(int& i, string name IS("")) {
  fakebox[boxid] = (name == "");
  boxname[boxid] = name;
  monsbox[boxid] = false;
  applyBox(i);
  }

void applyBoxBool(bool& b, string name = "") {
  int i = b;
  applyBoxNum(i, name);
  monsbox[boxid] = false;
  b = i;
  }

// just skips the value when loading
void applyBoxSave(int i, string name = "") {
  fakebox[boxid] = (name == "");
  boxname[boxid] = name;
  applyBox(i);
  }

int applyBoxLoad(string name = "") {
  fakebox[boxid] = (name == "");
  boxname[boxid] = name;
  int i=0; applyBox(i);
  return i;
  }

void applyBoxI(eItem it, bool f = false) {
  boxname[boxid] = iinf[it].name;
  fakebox[boxid] = f;
  monsbox[boxid] = false;
  if(loadingHi) { 
    updateHi(it, savebox[boxid++]); 
    }
  else applyBox(items[it]);
  }

vector<eItem> invorb;

void addinv(eItem it) {
  invorb.push_back(it);
  }

void applyBoxOrb(eItem it) {
  applyBoxI(it, true);
  invorb.push_back(it);
  }  

void list_invorb() {
  for(eItem it: invorb) {
#if CAP_INV
    if(true) {
      inv::applyBox(it);
      continue;
      }
#endif
    int u = 0;
    applyBoxNum(u);
    }
  invorb.clear();
  }

void applyBoxM(eMonster m, bool f = false) {
  fakebox[boxid] = f;
  boxname[boxid] = minf[m].name;
  monsbox[boxid] = true;
  applyBox(kills[m]);
  }

void applyBoxes() {
  invorb.clear();

  eLand lostin = laNone;

  applyBoxSave((int) timerstart, "time elapsed");
  time_t timer = time(NULL);
  applyBoxSave((int) timer, "date");
  applyBoxSave(gold(), "treasure collected");
  applyBoxSave(tkills(), "total kills");
  applyBoxNum(turncount, "turn count");
  applyBoxNum(cellcount, "cells generated");

  if(loading) timerstart = time(NULL);
  
  for(int i=0; i<itOrbLightning; i++) 
    if(i == 0) items[i] = 0, applyBoxI(itFernFlower);
    else applyBoxI(eItem(i));
  
  for(int i=0; i<43; i++) {
    if(loading) kills[i] = 0;
    bool fake = 
      i == moLesserM || i == moNone || i == moWolfMoved || i == moTentacletail ||
      i == moIvyNext;
    if(i == moWormtail) applyBoxM(moCrystalSage);
    else if(i == moWormwait) applyBoxM(moFireFairy);
    else if(i == moTentacleEscaping) applyBoxM(moMiner);
    else if(i == moGolemMoved) applyBoxM(moIllusion);
    else if(i == moTentaclewait) applyBoxOrb(itOrbThorns);
    else if(i == moGreater) applyBoxOrb(itOrbDragon);
    else if(i == moGreaterM) applyBoxOrb(itOrbIllusion);
    else applyBoxM(eMonster(i), fake);
    }
    
  if(saving) {
    int totaltime = savetime;
    if(!timerstopped) totaltime += timer - timerstart;
    applyBoxSave((int) totaltime, "time played");
    }
  else if(loading) savetime = applyBoxLoad("time played");
  else boxname[boxid] = "time played", boxid++;
  
  if(saving) savecount++; 
  applyBoxNum(savecount, "number of saves"); 
  if(saving) savecount--;
  applyBoxNum(cheater, "number of cheats");
  
  fakebox[boxid] = true;
  if(saving) applyBoxSave(items[itOrbSafety] ? safetyland : cwt.at->land, "");
  else if(loading) firstland = safetyland = eLand(applyBoxLoad());
  else lostin = eLand(savebox[boxid++]);
  
  for(int i=itOrbLightning; i<25; i++) applyBoxOrb(eItem(i));
  
  applyBoxI(itRoyalJelly);
  applyBoxI(itWine);
  applyBoxI(itSilver);
  applyBoxI(itEmerald);
  applyBoxI(itPower);
  applyBoxOrb(itOrbFire);
  applyBoxOrb(itOrbInvis);
  applyBoxOrb(itOrbAether);
  applyBoxOrb(itOrbPsi);
  applyBoxM(moBug0);
  applyBoxM(moBug1);
  applyBoxM(moBug2);
  applyBoxM(moVineBeast);
  applyBoxM(moVineSpirit);
  applyBoxM(moLancer);
  applyBoxM(moFlailer);
  applyBoxM(moEarthElemental);
  applyBoxM(moDarkTroll);
  applyBoxM(moWitch);
  applyBoxM(moWitchFire);
  applyBoxM(moWitchFlash);
  applyBoxM(moWitchGhost);
  applyBoxM(moWitchSpeed);
  applyBoxM(moEvilGolem);
  applyBoxM(moWitchWinter);
  applyBoxI(itHolyGrail);
  applyBoxI(itGrimoire);
  applyBoxM(moKnight);
  applyBoxM(moCultistLeader);
  
  applyBoxM(moPirate);
  applyBoxM(moCShark);
  applyBoxM(moParrot);
  applyBoxI(itPirate);
  applyBoxOrb(itOrbTime);
  
  applyBoxM(moHexSnake);
  applyBoxM(moRedTroll);
  applyBoxI(itRedGem);
  applyBoxOrb(itOrbSpace);
  
  int geo = geometry;
  applyBoxNum(geo, ""); geometry = eGeometry(geo);
  applyBoxBool(hardcore, "hardcore");
  applyBoxNum(hardcoreAt, "");
  applyBoxBool(shmup::on, "shmup");
  if(saving) applyBoxSave(specialland, "euclid land");
  else if(loading) specialland = eLand(applyBoxLoad("euclid land"));
  else fakebox[boxid++] = true;
  
  applyBoxI(itCoast);
  applyBoxI(itWhirlpool);
  applyBoxI(itBombEgg);
  applyBoxM(moBomberbird);
  applyBoxM(moTameBomberbird);
  applyBoxM(moAlbatross);
  applyBoxOrb(itOrbFriend);
  applyBoxOrb(itOrbAir);
  applyBoxOrb(itOrbWater);
  
  applyBoxI(itPalace);
  applyBoxI(itFjord);
  applyBoxOrb(itOrbFrog);
  applyBoxOrb(itOrbDiscord);
  applyBoxM(moPalace);
  applyBoxM(moFatGuard);
  applyBoxM(moSkeleton);
  applyBoxM(moVizier);
  applyBoxM(moViking);
  applyBoxM(moFjordTroll);
  applyBoxM(moWaterElemental);
  
  applyBoxI(itSavedPrincess);
  applyBoxOrb(itOrbLove);
  applyBoxM(moPrincess);
  applyBoxM(moPrincessMoved, false); // live Princess for Safety
  applyBoxM(moPrincessArmedMoved, false); // live Princess for Safety
  applyBoxM(moMouse);
  applyBoxNum(princess::saveArmedHP, "");
  applyBoxNum(princess::saveHP, "");
  
  applyBoxI(itIvory);
  applyBoxI(itElemental);
  applyBoxI(itZebra);
  applyBoxI(itFireShard);
  applyBoxI(itWaterShard);
  applyBoxI(itAirShard);
  applyBoxI(itEarthShard);
  
  applyBoxM(moAirElemental);
  applyBoxM(moFireElemental);
  applyBoxM(moFamiliar);
  applyBoxM(moGargoyle);
  applyBoxM(moOrangeDog);
  applyBoxOrb(itOrbSummon);
  applyBoxOrb(itOrbMatter);

  applyBoxM(moForestTroll);
  applyBoxM(moStormTroll);
  applyBoxM(moOutlaw);
  applyBoxM(moMutant);
  applyBoxM(moMetalBeast);
  applyBoxM(moMetalBeast2);
  applyBoxI(itMutant);
  applyBoxI(itFulgurite);
  applyBoxI(itBounty);
  applyBoxOrb(itOrbLuck);
  applyBoxOrb(itOrbStunning);
  
  applyBoxBool(tactic::on, "");
  applyBoxNum(elec::lightningfast, "");
  
  // if(savebox[boxid]) printf("lotus = %d (lost = %d)\n", savebox[boxid], isHaunted(lostin));
  if(loadingHi && isHaunted(lostin)) boxid++;
  else applyBoxI(itLotus);
  applyBoxOrb(itOrbUndeath);
  applyBoxI(itWindstone);
  applyBoxOrb(itOrbEmpathy);
  applyBoxM(moWindCrow);
  applyBoxOrb(itMutant2);
  applyBoxOrb(itOrbFreedom);
  applyBoxM(moRedFox);
  applyBoxBool(survivalist);
  if(loadingHi) applyBoxI(itLotus);
  else applyBoxNum(truelotus, "lotus/escape");
  
  int v = int(variation);
  applyBoxNum(v, "variation"); 
  variation = eVariation(v);
  applyBoxI(itRose);
  applyBoxOrb(itOrbBeauty);
  applyBoxI(itCoral);
  applyBoxOrb(itOrb37);
  applyBoxOrb(itOrbEnergy);
  applyBoxM(moRatling);
  applyBoxM(moFalsePrincess);
  applyBoxM(moRoseLady);
  applyBoxM(moRoseBeauty);
  applyBoxNum(chaosmode, "Chaos mode");
  applyBoxNum(multi::players, "shmup players");
  if(multi::players < 1 || multi::players > MAXPLAYER)
    multi::players = 1;
  applyBoxM(moRatlingAvenger);
  // printf("applybox %d\n", shmup::players); 
  
  applyBoxI(itApple);
  applyBoxM(moSparrowhawk);
  applyBoxM(moResearcher);
  applyBoxI(itDragon);
  applyBoxM(moDragonHead);
  applyBoxOrb(itOrbDomination);
  applyBoxI(itBabyTortoise);
  applyBoxNum(tortoise::seekbits, "");
  applyBoxM(moTortoise);
  applyBoxOrb(itOrbShell);
  
  applyBoxNum(safetyseed);

  // (+18)
  for(int i=0; i<6; i++) {
    applyBoxNum(multi::treasures[i]);
    applyBoxNum(multi::kills[i]);
    applyBoxNum(multi::deaths[i]);
    }
  // (+8)
  applyBoxM(moDragonTail);
  applyBoxI(itKraken);
  applyBoxM(moKrakenH);
  applyBoxM(moKrakenT);
  applyBoxOrb(itOrbSword);
  applyBoxI(itBarrow);
  applyBoxM(moDraugr);
  applyBoxOrb(itOrbSword2);
  applyBoxI(itTrollEgg);
  applyBoxOrb(itOrbStone);
  
  bool sph;
  sph = false; applyBoxBool(sph, "sphere"); if(sph) geometry = gSphere;
  sph = false; applyBoxBool(sph, "elliptic"); if(sph) geometry = gElliptic;
  applyBox(princess::reviveAt);
  
  applyBoxI(itDodeca);
  applyBoxI(itAmethyst);
  applyBoxI(itSlime);
  applyBoxOrb(itOrbNature);
  applyBoxOrb(itOrbDash); 
  addinv(itOrbRecall);
  applyBoxM(moBat);
  applyBoxM(moReptile);
  applyBoxM(moFriendlyIvy);
  
  applyBoxI(itGreenGrass);
  applyBoxI(itBull);
  applyBoxOrb(itOrbHorns);
  applyBoxOrb(itOrbBull);
  applyBoxM(moSleepBull);
  applyBoxM(moRagingBull);
  applyBoxM(moHerdBull);
  applyBoxM(moButterfly);
  applyBoxM(moGadfly);

  // 10.0:
  applyBoxNum(hinttoshow); // 258
  addinv(itOrbMirror);
  addinv(itGreenStone);
  list_invorb();
  applyBoxBool(inv::on, "inventory"); // 306
  #if CAP_INV
  applyBoxNum(inv::rseed);
  #else
  { int u; applyBoxNum(u); }
  #endif

  // 10.1:
  applyBoxI(itLavaLily);
  applyBoxI(itHunting);
  applyBoxI(itBlizzard);
  applyBoxI(itTerra);
  applyBoxOrb(itOrbSide1);
  applyBoxOrb(itOrbSide2);
  applyBoxOrb(itOrbSide3);
  applyBoxOrb(itOrbLava);
  applyBoxOrb(itOrbMorph);
  applyBoxM(moHunterDog);
  applyBoxM(moIceGolem);
  applyBoxM(moVoidBeast);
  applyBoxM(moJiangshi);
  applyBoxM(moTerraWarrior);
  applyBoxM(moSalamander);
  applyBoxM(moLavaWolf);
  
  applyBoxOrb(itOrbSlaying);
  applyBoxOrb(itOrbMagnetism);
  applyBoxOrb(itOrbPhasing);
  applyBoxI(itDock);
  applyBoxI(itGlowCrystal);
  applyBoxI(itMagnet);
  applyBoxI(itRuins);
  applyBoxI(itSwitch);
  applyBoxM(moNorthPole);
  applyBoxM(moSouthPole);
  applyBoxM(moSwitch1);
  applyBoxM(moSwitch2);
  applyBoxM(moAltDemon);
  applyBoxM(moHexDemon);
  applyBoxM(moPair);
  applyBoxM(moCrusher);
  applyBoxM(moMonk);
  
  bool v2 = false;
  applyBoxBool(v2); if(loading && v2) variation = eVariation::goldberg;
  applyBox(gp::param.first);
  applyBox(gp::param.second);
  
  v2 = false; applyBoxBool(v2); if(loading && v2) variation = eVariation::irregular;
  applyBox(irr::cellcount);

  list_invorb();

  applyBox(irr::bitruncations_performed);
  
  applyBoxI(itVarTreasure);
  applyBoxI(itBrownian);
  applyBoxI(itWest);
  applyBoxM(moAcidBird);
  applyBoxM(moBrownBug);
  applyBoxM(moVariantWarrior);
  applyBoxM(moWestHawk);
  applyBoxM(moFallingDog);
  applyBoxOrb(itOrbIntensity);
  applyBoxOrb(itOrbChoice);
  applyBoxOrb(itOrbGravity);
  list_invorb();
  
  applyBoxM(moNarciss);
  applyBoxM(moMirrorSpirit);  

  if(POSSCORE != boxid) printf("ERROR: %d boxes\n", boxid);
  }

void saveBox() {
  boxid = 0; saving = true; applyBoxes(); saving = false;
  }

void loadBox() {
  // have boxid
  boxid = 0; loading = true; applyBoxes(); loading = false;
  }

void loadBoxHigh() {

  dynamicval<int> sp1(multi::players, savebox[197]);
  dynamicval<eGeometry> sp2(geometry, (eGeometry) savebox[116]);
  dynamicval<bool> sp3(shmup::on, savebox[119]);
  dynamicval<int> sp4(chaosmode, savebox[196]);
  dynamicval<eVariation> sp5(variation, (eVariation) savebox[186]);
  dynamicval<int> sp7(gp::param.first, savebox[342]);
  dynamicval<int> sp8(gp::param.second, savebox[343]);
  dynamicval<bool> spinv(inv::on, savebox[306]);

  if(savebox[238]) geometry = gSphere;
  if(savebox[239]) geometry = gElliptic;
  if(savebox[341]) variation = eVariation::goldberg;
  if(savebox[344]) variation = eVariation::irregular;

  if(multi::players < 1 || multi::players > MAXPLAYER)
    multi::players = 1;

  if(shmup::on && multi::players == 1) ;
  else {
    // have boxid
    boxid = 0; loadingHi = true; applyBoxes(); loadingHi = false;
    }
  }

// certify that saves and achievements were received
// in an official version of HyperRogue
#if CAP_CERTIFY
#include "private/certify.cpp"
#else

namespace anticheat {
  bool tampered;
  void save(FILE *f) {}
  bool load(FILE *f, score& sc, const string& ver) {return true; }
  int certify(const string& s, int a, int b, int c, int d=0) { return d; }
  int check(int cv, const string& ver, const string& s, int a, int b, int c, int d=0) { return cv==d; }
  void nextid(int& tid, const string& ver, int cert) { tid++; }
  };

#endif

long long saveposition = -1;

void remove_emergency_save() {
#if !ISWINDOWS
  if(saveposition >= 0) { 
    if(truncate(scorefile, saveposition)) {}
    saveposition = -1;
    }
#endif
  }

EX void saveStats(bool emergency IS(false)) {
  DEBBI(DF_INIT, ("saveStats [%s]", scorefile));

  if(autocheat) return;
  #if CAP_TOUR
  if(tour::on) return;
  #endif
  if(randomPatternsMode) return;
  if(dual::state) return;
  if(archimedean) return;
  if(hybri) return;
  if(daily::on) return;
  if(peace::on) return;
  if(!gold()) return;
  
  remove_emergency_save();

  FILE *f = fopen(scorefile, "at");
  
  if(!f) {
    // printf("Could not open the score file '%s'!\n", scorefile);
    addMessage(XLAT("Could not open the score file: ") + scorefile);
    return;
    }

  if(emergency) {
    saveposition = ftell(f);
//  if(!timerghost) addMessage(XLAT("Emergency save at ") + its(saveposition));
    }
  
  if(showoff) { fclose(f); return; }
  
  time_t timer;
  timer = time(NULL);
  char sbuf[128]; strftime(sbuf, 128, "%c", localtime(&timerstart));
  char buf[128]; strftime(buf, 128, "%c", localtime(&timer));
  
  if((tactic::on || yendor::on) && !items[itOrbSafety] && !cheater) {
    int t = (int) (timer - timerstart);

    modecode_t xcode = modecode();

    if(tactic::on) {
      int score = items[treasureType(specialland)];
      
      if(score) {
        int c = 
          anticheat::certify(dnameof(specialland), turncount, t, (int) timerstart,
            int(xcode)*999 + tactic::id + 256 * score + (xcode>>32)*7);
        fprintf(f, "TACTICS %s %d %d %d %d %d %d %d %d date: %s\n", VER,
          tactic::id, specialland, score, turncount, t, int(timerstart), 
          c, int(xcode), buf);
        tactic::record(specialland, score);
        anticheat::nextid(tactic::id, VER, c);
        }
      }

    if(yendor::on)
      fprintf(f, "YENDOR %s %d %d %d %d %d %d %d %d date: %s\n", VER,
        yendor::lastchallenge, items[itOrbYendor], yendor::won, turncount, t, int(timerstart), 
        anticheat::certify(yendor::won ? "WON" : "LOST", turncount, t, (int) timerstart,
          int(xcode)*999 + yendor::lastchallenge + 256 * items[itOrbYendor] + (xcode>>32)*7),
        int(xcode),
        buf);

    fclose(f);
    return;
    }

  fprintf(f, "HyperRogue: game statistics (version " VER ")\n");
  if(cheater)
    fprintf(f, "CHEATER! (cheated %d times)\n", cheater);
  if(true) {

    fprintf(f, VER);
    if(!shmup::on) items[itOrbLife] = countMyGolems(moGolem); 
    if(!shmup::on) items[itOrbFriend] = countMyGolems(moTameBomberbird); 
    if(!shmup::on) kills[moPrincessMoved] = countMyGolems(moPrincess); 
    if(!shmup::on) kills[moPrincessArmedMoved] = countMyGolems(moPrincessArmed); 
    if(!shmup::on) princess::saveHP = countMyGolemsHP(moPrincess); 
    if(!shmup::on) princess::saveArmedHP = countMyGolemsHP(moPrincessArmed); 
    saveBox();
    
    for(int i=0; i<boxid; i++) fprintf(f, " %d", savebox[i]);
    anticheat::save(f);

    fprintf(f, "\n");
    }
  fprintf(f, "Played on: %s - %s (%d %s)\n", sbuf, buf, turncount, 
    shmup::on ? "knives" : "turns");
  fprintf(f, "Total wealth: %d\n", gold());
  fprintf(f, "Total enemies killed: %d\n", tkills());
  fprintf(f, "cells generated: %d\n", cellcount);
  if(pureHardcore()) fprintf(f, "Pure hardcore mode\n");
  if(geometry) fprintf(f, "Geometry: %s/%s/%s\n", gp::operation_name().c_str(), ginf[geometry].tiling_name.c_str(), ginf[geometry].quotient_name.c_str());
  if(chaosmode) fprintf(f, "Chaos mode\n");
  if(shmup::on) fprintf(f, "Shoot-em up mode\n");
  if(inv::on) fprintf(f, "Inventory mode\n");
  if(multi::players > 1) fprintf(f, "Multi-player (%d players)\n", multi::players);
  fprintf(f, "Number of cells explored, by distance from the player:\n"); 
  {for(int i=0; i<10; i++) fprintf(f, " %d", explore[i]);} fprintf(f, "\n");
  if(kills[0]) fprintf(f, "walls melted: %d\n", kills[0]);
  fprintf(f, "cells travelled: %d\n", celldist(cwt.at));
  
  fprintf(f, "\n");

  for(int i=0; i<ittypes; i++) if(items[i])  
    fprintf(f, "%4dx %s\n", items[i], iinf[i].name);
    
  fprintf(f, "\n");
  
  for(int i=1; i<motypes; i++) if(kills[i])  
    fprintf(f, "%4dx %s\n", kills[i], minf[i].name);
  
  fprintf(f, "\n\n\n");
  
#if ISMOBILE==0
  DEBB(DF_INIT, ("Game statistics saved to ", scorefile));
  if(!tactic::trailer)
    addMessage(XLAT("Game statistics saved to %1", scorefile));
#endif
  fclose(f);
  }

// load the save
EX void loadsave() {
  if(autocheat) return;
#if CAP_TOUR
  if(tour::on) return;
#endif
  DEBBI(DF_INIT, ("loadSave"));

  gamecount = 0;

  FILE *f = fopen(scorefile, "rt");
  havesave = f;
  if(!f) return;
  score sc;
  bool ok = false;
  bool tamper = false;
  int coh = counthints();
  while(!feof(f)) {
    char buf[120];
    if(fgets(buf, 120, f) == NULL) break;
    if(buf[0] == 'H' && buf[1] == 'y') {
      gamecount++;
      if(fscanf(f, "%s", buf) <= 0) break;
      sc.ver = buf;
      if(sc.ver[1] != '.') sc.ver = '0' + sc.ver;
      if(verless(sc.ver, "4.4") || sc.ver == "CHEATER!") { ok = false; continue; }
      ok = true;
      for(int i=0; i<MAXBOX; i++) {
        if(fscanf(f, "%d", &sc.box[i]) <= 0) {
          boxid = i;
                    
          tamper = anticheat::load(f, sc, sc.ver);

          for(int i=0; i<boxid; i++) savebox[i] = sc.box[i];
          for(int i=boxid; i<MAXBOX; i++) savebox[i] = 0, sc.box[i] = 0;

          if(savebox[258] >= 0 && savebox[258] < coh) {
             hints[savebox[258]].last = savebox[1];
             }

          loadBoxHigh();

          break;
          }
        }
      }
    if(buf[0] == 'A' && buf[1] == 'C' && buf[2] == 'H') {
      char buf1[80], buf2[80];
      sscanf(buf, "%70s%70s", buf1, buf2);
      if(buf2 == string("PRINCESS1")) princess::everSaved = true;
      if(buf2 == string("YENDOR2")) yendor::everwon = true;
      if(buf2 == string("CR4")) chaosUnlocked = true;
      }

    if(buf[0] == 'T' && buf[1] == 'A' && buf[2] == 'C') {
      ok = false;
      char buf1[80], ver[10];
      int tid, land, score, tc, t, ts, cert;
      int xc = -1;
      sscanf(buf, "%70s%10s%d%d%d%d%d%d%d%d",
        buf1, ver, &tid, &land, &score, &tc, &t, &ts, &cert, &xc);
      
      eLand l2 = eLand(land);
      if(land == laMirror && verless(ver, "10.0")) l2 = laMirrorOld;

      if(xc == -1)
        for(xc=0; xc<32768; xc++)
          if(anticheat::check(cert, ver, dnameof(l2), tc, t, ts, xc*999+unsigned(tid) + 256 * score)) 
            break;
      
      if(tid == tactic::id && (anticheat::check(cert, ver, dnameof(l2), tc, t, ts, xc*unsigned(999)+ unsigned(tid) + 256 * score))) {
        if(score != 0 
          && !(land == laOcean && verless(ver, "8.0f"))
          && !(land == laTerracotta && verless(ver, "10.3e"))
        ) tactic::record(l2, score, xc);
        anticheat::nextid(tactic::id, ver, cert);
        }
      }

    if(buf[0] == 'Y' && buf[1] == 'E' && buf[2] == 'N') {
      char buf1[80], ver[10];
      int cid, oy, won, tc, t, ts, cert=0, xc = -1;
      sscanf(buf, "%70s%10s%d%d%d%d%d%d%d%d",
        buf1, ver, &cid, &oy, &won, &tc, &t, &ts, &cert, &xc);

      if(xc == -1)
        for(xc=0; xc<32768; xc++)
          if(anticheat::check(cert, ver, won ? "WON" : "LOST", tc, t, ts, xc*999 + cid + 256 * oy)) 
            break;
      
      if(won) if(anticheat::check(cert, ver, won ? "WON" : "LOST", tc, t, ts, xc*999 + cid + 256 * oy)) {
        if(xc == 19 && cid == 25) xc = 0;
        if(cid > 0 && cid < YENDORLEVELS) 
        if(!(verless(ver, "8.0f") && oy > 1 && cid == 15)) 
        if(!(verless(ver, "9.3b") && oy > 1 && (cid == 27 || cid == 28))) 
          {
          yendor::bestscore[xc][cid] = max(yendor::bestscore[xc][cid], oy);
          }
        }
      }

    }
  fclose(f);
  if(ok && sc.box[65 + 4 + itOrbSafety - itOrbLightning]) {
    anticheat::tampered = tamper;
//  printf("box = %d (%d)\n", sc.box[65 + 4 + itOrbSafety - itOrbLightning], boxid);
//  printf("boxid = %d\n", boxid);
    for(int i=0; i<boxid; i++) savebox[i] = sc.box[i];
    for(int i=boxid; i<MAXBOX; i++) savebox[i] = 0;
//  for(int i=160; i<200; i++) printf("%d: %d ", i, savebox[i]);
    loadBox();
//  printf("boxid = %d\n", boxid);
    if(items[itHolyGrail]) {
      items[itHolyGrail]--;
      knighted = newRoundTableRadius();
      items[itHolyGrail]++;
      }
    else knighted = 0;
    safety = true;
    if(items[itSavedPrincess] < 0) items[itSavedPrincess] = 0;
    addMessage(XLAT("Game loaded."));
    showstartmenu = false;
    // reset unsavable special modes just in case
    peace::on = false;
    randomPatternsMode = false;
    yendor::on = false;
    tour::on = false;
    }
  }
#endif

EX void stop_game() {
  if(!game_active) return;
  if(dual::split(stop_game)) return;
  DEBBI(DF_INIT, ("stop_game"));
  achievement_final(true);
#if CAP_SAVE
  saveStats();
#endif
  for(int i=0; i<ittypes; i++) items[i] = 0;
  lastkills = 0; for(int i=0; i<motypes; i++) kills[i] = 0;
  for(int i=0; i<10; i++) explore[i] = 0;
  for(int i=0; i<10; i++) for(int l=0; l<landtypes; l++)
    exploreland[i][l] = 0;

  for(int i=0; i<numplayers(); i++)
    if(multi::playerActive(i)) 
      multi::deaths[i]++;

#if CAP_SAVE
  anticheat::tampered = false; 
#endif
  achievementsReceived.clear();
  princess::saved = false;
  princess::nodungeon = false;
  princess::reviveAt = 0;
  princess::forceVizier = false;
  princess::forceMouse = false;
  knighted = 0;
  // items[itGreenStone] = 100;
  clearMemory();
  game_active = false;
#if CAP_DAILY
  if(daily::on)
    daily::turnoff();
#endif
  }

EX void set_geometry(eGeometry target) {
  if(geometry != target) {
    int old_DIM = GDIM;
    stop_game();
    ors::reset();
    if(among(target, gProduct, gRotSpace)) hybrid::configure(target);
    geometry = target;
  
    if(chaosmode && bounded) chaosmode = false;
    #if CAP_RACING
    if(bounded) racing::on = false;
    #endif
    if(euclid6) variation = eVariation::bitruncated;
    #if CAP_IRR
    if(IRREGULAR) variation = eVariation::bitruncated;
    #endif
    #if CAP_GP
    if(GOLDBERG && gp::param == gp::loc(1,1) && S3 == 3) {
      variation = eVariation::bitruncated;
      }
    if(GOLDBERG && nonorientable) {
      if(gp::param.second && gp::param.second != gp::param.first)
        gp::param.second = 0;
      }
    #endif
    if(DUAL && geometry != gArchimedean) 
      variation = ginf[geometry].default_variation;
    #if CAP_BT
    if(binarytiling || WDIM == 3 || penrose) if(!hybri) variation = eVariation::pure;
    #endif
    if(GDIM == 3 && old_DIM == 2 && pmodel == mdDisk) pmodel = mdPerspective;
    if(nonisotropic && old_DIM == 2) pmodel = mdGeodesic;
    if(GDIM == 2 && among(pmodel, mdPerspective, mdGeodesic)) pmodel = mdDisk;
    if(nonisotropic && old_DIM == 2 && vid.texture_step < 4) vid.texture_step = 4;
    if(prod) { pmodel = mdPerspective; if(vid.texture_step < 4) vid.texture_step = 4; }
    if(sl2) nisot::geodesic_movement = true;
    }
  }

EX void set_variation(eVariation target) {
  if(variation != target) {
    stop_game();
    if(euclid6 || binarytiling || sol || penrose || WDIM == 3) if(!prod) geometry = gNormal;
    auto& cd = ginf[gCrystal];
    if(target == eVariation::bitruncated && cryst && cd.sides == 8 && cd.vertex == 4) {
      cd.vertex = 3;
      cd.tiling_name = "{8,3}";
      target = eVariation::pure;
      }
    variation = target;
    }
  }
  
void stop_tour() {
  while(gamestack::pushed()) {
    gamestack::pop();
    stop_game();
    }
  }

EX void switch_game_mode(char switchWhat) {
  DEBBI(DF_INIT, ("switch_game_mode ", switchWhat));
  switch(switchWhat) {
    case rg::peace:
      peace::on = !peace::on;
      tactic::on = yendor::on = princess::challenge = 
      randomPatternsMode = inv::on = false;
      racing::on = false;
      break;
    
    case rg::dualmode:
      stop_tour(); tour::on = false;
      racing::on = false;
      yendor::on = tactic::on = princess::challenge = false;
      if(!dual::state) dual::enable();
      else dual::disable();
      break;
    
    case rg::inv:
      inv::on = !inv::on;
      if(tactic::on) firstland = laIce;
      tactic::on = yendor::on = princess::challenge = 
      randomPatternsMode = peace::on = false;
      racing::on = false;
      break;

    case rg::chaos:
      if(tactic::on) firstland = laIce;
      yendor::on = tactic::on = princess::challenge = false;
      chaosmode = !chaosmode;
      if(bounded) set_geometry(gNormal);
      racing::on = false;
      break;

#if CAP_TOUR
    case rg::tour:
      if(tour::on) stop_tour();
      geometry = gNormal;
      yendor::on = tactic::on = princess::challenge = peace::on = inv::on = false;
      dual::disable();
      chaosmode = randomPatternsMode = false;
      variation = eVariation::bitruncated;
      #if CAP_GP
      gp::param = gp::loc(1, 1);
      #endif
      shmup::on = false;
      tour::on = !tour::on;
      racing::on = false;
      break;
#endif

    case rg::yendor:
      yendor::on = !yendor::on;
      tactic::on = false;
      peace::on = false;
      inv::on = false;
      princess::challenge = false;
      randomPatternsMode = false;
      chaosmode = false;
      racing::on = false;
      if(!yendor::on) firstland = laIce;
      dual::disable();
      break;

#if CAP_RACING    
    case rg::racing:
      racing::on = !racing::on;
      shmup::on = racing::on;
      peace::on = false;
      tour::on = false;
      inv::on = false;
      chaosmode = false;
      princess::challenge = false;
      if(sol || bounded) set_geometry(gNormal);
      dual::disable();
      break;
#endif

    case rg::tactic:
      tactic::on = !tactic::on;
      yendor::on = false;
      peace::on = false;
      inv::on = false;
      randomPatternsMode = false;
      princess::challenge = false;
      racing::on = false;
      chaosmode = false;
      if(!tactic::on) firstland = laIce;
      dual::disable();
      break;
    
    case rg::shmup:
      shmup::on = !shmup::on;
      princess::challenge = false;
      if(!shmup::on) racing::on = false;
      break;
    
    case rg::randpattern:
      randomPatternsMode = !randomPatternsMode;
      tactic::on = false;
      yendor::on = false;
      peace::on = false;
      inv::on = false;
      princess::challenge = false;
      break;
    
    case rg::princess:
      princess::challenge = !princess::challenge;
      firstland = princess::challenge ? laPalace : laIce;
      shmup::on = false;
      tactic::on = false;
      yendor::on = false;
      chaosmode = false;
      inv::on = false;
      racing::on = false;
      dual::disable();
      break;
    
#if CAP_DAILY
    case rg::daily:
      daily::setup();
      break;
    
    case rg::daily_off:
      daily::turnoff();
      break;
#endif
    }
  }

EX void start_game() {
  if(game_active) return;
  DEBBI(DF_INIT, ("start_game"));
  if(dual::state == 1) dual::assign_landsides();  
  if(dual::split(start_game)) return;
  restart:
  game_active = true;
  gamegen_failure = false;
  ignored_memory_warning = false;
  check_cgi();
  cgi.require_basics();
  arcm::current.compute_geometry();
  initcells();
  expansion.reset();

  if(randomPatternsMode) {
    for(int i=0; i<landtypes; i++) {
      randompattern[i] = hrandpos();
      // change probability 1/5 to 2/6
      if(hrand(5) == 0) {
        randompattern[i] -= (randompattern[i] % 5);
        }
      }
    if(randomPatternsMode) specialland = pickLandRPM(laNone);
    clearMemoRPM();
    }

  initgame();
  if(gamegen_failure) {
    stop_game();
    goto restart;
    }
  canmove = true;
  restartGraph();
  resetmusic();
  resetmusic();
#if CAP_TEXTURE
  texture::config.remap();
#endif
  subscreens::prepare();
  }

// popAllScreens + popAllGames + stop_game + switch_game_mode + start_game
EX void restart_game(char switchWhat IS(rg::nothing)) {
  popScreenAll();  
  stop_game();
  switch_game_mode(switchWhat);
  start_game();
  }

// stop_game + switch_game_mode
EX void stop_game_and_switch_mode(char switchWhat IS(rg::nothing)) {
  stop_game();
  switch_game_mode(switchWhat);
  }

EX purehookset clearmemory;

EX void clearMemory() {
  callhooks(clearmemory);
  }

EX bool fixseed = false;
EX int startseed = 0;

EX eLand firstland0;

EX void initAll() {
  init_floorcolors();
  showstartmenu = true;
  ca::init();
#if CAP_COMMANDLINE
  arg::read(1);
#endif
  srand(time(NULL));
  shrand(fixseed ? startseed : time(NULL));

  achievement_init(); // not in ANDROID

  firstland0 = firstland;
  
  // initlanguage();
  initgraph();
#if CAP_SAVE
  loadsave();
  if(IRREGULAR) irr::auto_creator();
#endif
  start_game();
  
  if(!shmup::on) {
    restoreGolems(items[itOrbLife], moGolem); items[itOrbLife] = 0;
    restoreGolems(items[itOrbFriend], moTameBomberbird); items[itOrbFriend] = 0;
    restoreGolems(kills[moPrincessMoved], moPrincess, princess::saveHP); kills[moPrincessMoved] = 0;
    restoreGolems(kills[moPrincessArmedMoved], moPrincessArmed, princess::saveArmedHP); kills[moPrincessArmedMoved] = 0;
    }
  
  firstland = firstland0;
  polygonal::solve();
  }

EX void finishAll() {
  achievement_final(!items[itOrbSafety]);
  
#if CAP_SAVE
  saveStats();
#endif
  offscreen.clear();
  clearMemory();
#if ISMOBILE==0
  cleargraph();
#endif
  
  achievement_close();  
  }


auto cgm = addHook(clearmemory, 40, [] () {
  pathq.clear();
  dcal.clear();
  clearshadow();
  for(int i=0; i<MAXPLAYER; i++) lastmountpos[i] = NULL;
  seenSevenMines = false;
  recallCell = NULL;
  butterflies.clear();
  buggycells.clear();
  crush_next.clear(); 
  crush_now.clear();
  rosemap.clear();
  adj_memo.clear();
  }) + 
addHook(hooks_gamedata, 0, [] (gamedata* gd) {
  gd->store(pathq);
  gd->store(dcal);
  gd->store(recallCell);
  gd->store(butterflies);
  gd->store(buggycells);
  gd->store(crush_now);
  gd->store(crush_next);
  gd->store(rosemap);
  gd->store(airmap);
  gd->store(adj_memo);
  gd->store(pd_from);
  gd->store(pd_range);
  gd->store(pathqm);
  gd->store(reachedfrom);
  gd->store(gravity_state);
  gd->store(last_gravity_state);
  gd->store(shpos);
  gd->store(cshpos);
  }) +
addHook(hooks_removecells, 0, [] () {
  eliminate_if(crush_next, is_cell_removed);
  eliminate_if(crush_now, is_cell_removed);
  eliminate_if(buggycells, is_cell_removed);
  eliminate_if(butterflies, [] (pair<cell*,int>& p) { return is_cell_removed(p.first); });
  for(int i=0; i<SHSIZE; i++) for(int p=0; p<MAXPLAYER; p++)
    set_if_removed(shpos[p][i], NULL);
  });;
}
