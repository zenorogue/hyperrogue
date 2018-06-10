// Hyperbolic Rogue

// Copyright (C) 2011-2018 Zeno Rogue, see 'hyper.cpp' for details

// routines for: initializing/closing, loading/saving, and cheating

bool need_reset_geometry = true;

bool game_active;

bool cblind;
bool autocheat;
bool canvas_invisible;

int truelotus;
int gamecount;

time_t timerstart, savetime;
bool timerstopped;
int savecount;
bool showoff = false, doCross = false;

eLand top_land;

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
    (peace::on || tactic::on || geometry || gp::on || randomPatternsMode || yendor::on);
  }

void welcomeMessage() {
  if(tactic::trailer) return;
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
  else if(sphere && specialland == laHalloween)
    addMessage(XLAT("Welcome to Halloween!"));
  else if(elliptic)
    addMessage(XLAT("Good luck in the elliptic plane!"));
  else if(sphere)
    addMessage(XLAT("Welcome to Spherogue!"));
#if CAP_ROGUEVIZ
  else if(rogueviz::on)
    addMessage(XLAT("Welcome to RogueViz!"));
#endif
  else {
    addMessage(XLAT("Welcome to HyperRogue!"));
    }

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
void initgame() {
  DEBB(DF_INIT, (debugfile,"initGame\n"));
  callhooks(hooks_initgame); 

  yendor::init(1);
  
  if(safety && safetyseed) {
    shrand(safetyseed);
    firstland = safetyland;
    }
  
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
  if((isGravityLand(firstland) && !isCyclic(firstland)) || (firstland == laOcean && !safety)) 
    firstland = weirdhyperbolic ? laCrossroads4 : laCrossroads;
  
  cwt.c = currentmap->gamestart(); cwt.spin = 0; cwt.mirrored = false;
  cwt.c->land = firstland;
  
  chaosAchieved = false;

  if(firstland == laElementalWall) cwt.c->land = randomElementalLand();
  
  createMov(cwt.c, 0);
  
  setdist(cwt.c, BARLEV, NULL);

  if((tactic::on || yendor::on || peace::on) && isCyclic(firstland)) {
    anthraxBonus = items[itHolyGrail];
    cwt.c->mov[0]->land = firstland;
    if(firstland == laWhirlpool) cwt.c->mov[0]->wall = waSea;
    
    setdist(cwt.c->mov[0], BARLEV-1, cwt.c);
    if(!sphere && !euclid) {
      heptagon *h = createAlternateMap(cwt.c, 2, hsA);
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
  
  yendor::init(2);
  
  clear_euland(specialland);

  if(euclid && specialland == laPrincessQuest) {
    cell *c = euclideanAtCreate(pair_to_vec(EPX, EPY));
    princess::generating = true;
    c->land = laPalace;
    setdist(c, 7 - getDistLimit() - genrange_bonus, NULL);
    princess::generating = false;
    }

  if(cwt.c->land == laCrossroads2) {
    cwt.c->landparam = 12;
    createMov(cwt.c, 0)->landparam = 44;
    createMov(cwt.c, 0)->land = laCrossroads2;
    }
    
  for(int i=0; i<numplayers(); i++) sword::angle[i] = 11;

  if(!safety) multi::players = vid.scfg.players;
  if(multi::players < 1 || multi::players > MAXPLAYER)
    multi::players = 1;
  multi::whereto[0].d = MD_UNDECIDED;
  multi::cpid = 0;

  if(shmup::on) shmup::init();
  
  #if CAP_DAILY
  daily::split();
  #endif
  
  // extern int sightrange; sightrange = 9;
  // cwt.c->land = laHell; items[itHell] = 10;
  for(int i=BARLEV; i>=7 - getDistLimit() - genrange_bonus; i--) {
    if(tactic::trailer && cwt.c->land != laClearing) safety = trailer_safety;
    setdist(cwt.c, i, NULL);
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
    for(int i=0; i<size(currentmap->allcells()); i++)
      setdist(currentmap->allcells()[i], 8, NULL);
    }

  
  if(multi::players > 1 && !shmup::on) for(int i=0; i<numplayers(); i++) {
    int idir = (3 * i) % cwt.c->type;
    multi::player[i].c = cwt.c->mov[idir];
    // special case -- otherwise they land on a wall
    if(firstland == laCrossroads2 && i == 1)
      multi::player[1].c = cwt.c;
    if(firstland == laCrossroads2 && i == 6)
      multi::player[6].c = createMov(createMov(cwt.c, 0), 3);
    setdist(cwt.c->mov[idir], 7 - getDistLimit() - genrange_bonus, cwt.c);
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
      makeEmpty(cwt.c);
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
    timerghost = true;
    truelotus = 0;
    survivalist = true;
#if CAP_INV
    if(inv::on) inv::init();
#endif
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

  if(!cheater) gamerange_bonus = genrange_bonus = 0;
  }

bool havesave = true;

#if CAP_SAVE
#define MAXBOX 500
#define POSSCORE 344 // update this when new boxes are added!

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

void applyBoxNum(int& i, string name) {
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
  if(saving) applyBoxSave(items[itOrbSafety] ? safetyland : cwt.c->land, "");
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
  applyBoxBool(nonbitrunc, "heptagons only"); 
  applyBoxI(itRose);
  applyBoxOrb(itOrbBeauty);
  applyBoxI(itCoral);
  applyBoxOrb(itOrb37);
  applyBoxOrb(itOrbEnergy);
  applyBoxM(moRatling);
  applyBoxM(moFalsePrincess);
  applyBoxM(moRoseLady);
  applyBoxM(moRoseBeauty);
  applyBoxBool(chaosmode, "Chaos mode");
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
  
  applyBoxBool(gp::on);
  applyBox(gp::param.first);
  applyBox(gp::param.second);

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
  dynamicval<bool> sp4(chaosmode, savebox[196]);
  dynamicval<bool> sp5(nonbitrunc, savebox[186]);
  dynamicval<bool> sp6(gp::on, savebox[341]);
  dynamicval<int> sp7(gp::param.first, savebox[342]);
  dynamicval<int> sp8(gp::param.second, savebox[343]);

  if(savebox[238]) geometry = gSphere;
  if(savebox[239]) geometry = gElliptic;

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

#include <unistd.h>
#include <sys/types.h>

void remove_emergency_save() {
#if !ISWINDOWS
  if(saveposition >= 0) { 
    if(truncate(scorefile, saveposition)) {}
    saveposition = -1;
    }
#endif
  }

void saveStats(bool emergency = false) {
  DEBB(DF_INIT, (debugfile,"saveStats [%s]\n", scorefile));

  if(autocheat) return;
  #if CAP_TOUR
  if(tour::on) return;
  #endif
  if(randomPatternsMode) return;
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

    int xcode = modecode();

    if(tactic::on) {
      int score = items[treasureType(specialland)];
      
      if(score) {
        int c = 
          anticheat::certify(dnameof(specialland), turncount, t, (int) timerstart,
            xcode*999 + tactic::id + 256 * score);
        fprintf(f, "TACTICS %s %d %d %d %d %d %d %d %d date: %s\n", VER,
          tactic::id, specialland, score, turncount, t, int(timerstart), 
          c, xcode, buf);
        tactic::record(specialland, score);
        anticheat::nextid(tactic::id, VER, c);
        }
      }

    if(yendor::on)
      fprintf(f, "YENDOR %s %d %d %d %d %d %d %d %d date: %s\n", VER,
        yendor::lastchallenge, items[itOrbYendor], yendor::won, turncount, t, int(timerstart), 
        anticheat::certify(yendor::won ? "WON" : "LOST", turncount, t, (int) timerstart,
          xcode*999 + yendor::lastchallenge + 256 * items[itOrbYendor]),
        xcode,
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
  if(nonbitrunc) fprintf(f, "Heptagons only mode\n");
  if(geometry) fprintf(f, "Geometry: %s\n", ginf[geometry].name);
  if(chaosmode) fprintf(f, "Chaos mode\n");
  if(shmup::on) fprintf(f, "Shoot-em up mode\n");
  if(inv::on) fprintf(f, "Inventory mode\n");
  if(multi::players > 1) fprintf(f, "Multi-player (%d players)\n", multi::players);
  fprintf(f, "Number of cells explored, by distance from the player:\n"); 
  {for(int i=0; i<10; i++) fprintf(f, " %d", explore[i]);} fprintf(f, "\n");
  if(kills[0]) fprintf(f, "walls melted: %d\n", kills[0]);
  fprintf(f, "cells travelled: %d\n", celldist(cwt.c));
  
  fprintf(f, "\n");

  for(int i=0; i<ittypes; i++) if(items[i])  
    fprintf(f, "%4dx %s\n", items[i], iinf[i].name);
    
  fprintf(f, "\n");
  
  for(int i=1; i<motypes; i++) if(kills[i])  
    fprintf(f, "%4dx %s\n", kills[i], minf[i].name);
  
  fprintf(f, "\n\n\n");
  
#if ISMOBILE==0
  DEBB(DF_INIT, (debugfile, "Game statistics saved to %s\n", scorefile));
  if(!tactic::trailer)
    addMessage(XLAT("Game statistics saved to %1", scorefile));
#endif
  fclose(f);
  }

// load the save
void loadsave() {
  if(autocheat) return;
#if CAP_TOUR
  if(tour::on) return;
#endif
  DEBB(DF_INIT, (debugfile,"loadSave\n"));

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

namespace gamestack {

  struct gamedata {
    hrmap *hmap;
    cellwalker cwt;
    cellwalker ctover;
    heptspin viewctr;
    transmatrix View;
    eGeometry geometry;
    bool shmup;
    bool hepta;
    };

  vector<gamedata> gd;
  
  bool pushed() { return size(gd); }
  
  void push() {
    if(geometry) {
      printf("ERROR: push implemented only in non-hyperbolic geometry\n");
      exit(1);
      }
    gamedata gdn;
    gdn.hmap = currentmap;
    gdn.cwt = cwt;
    gdn.viewctr = viewctr;
    gdn.View = View;
    gdn.geometry = geometry;
    gdn.shmup = shmup::on;
    gdn.hepta = nonbitrunc;
    gdn.ctover = centerover;
    gd.push_back(gdn);
    }
    
  void pop() {
    gamedata& gdn = gd[size(gd)-1];
    currentmap = gdn.hmap;
    cwt = gdn.cwt;
    viewctr = gdn.viewctr;
    View = gdn.View;
    geometry = gdn.geometry;
    nonbitrunc = gdn.hepta;
    if(shmup::on) shmup::clearMonsters();
    shmup::on = gdn.shmup;
    resetGeometry();
    gd.pop_back();
    centerover = gdn.ctover;
    bfs();
    }
  
  };

void pop_game() {
  if(gamestack::pushed()) {
    gamestack::pop();
    game_active = true;
    }
  }

void popAllGames() {
  while(gamestack::pushed()) {
    gamestack::pop();
    }
  }

void stop_game() {
  if(!game_active) return;
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
  if(daily::on && !among(switchWhat, rg::daily, rg::daily_off))
    daily::turnoff();
#endif
  }

void push_game() {
  gamestack::push();
  pd_from = NULL;
  centerover.c = NULL;
  game_active = false;
  }

void switch_game_mode(char switchWhat) {
  switch(switchWhat) {
    case rg::peace:
      peace::on = !peace::on;
      tactic::on = yendor::on = princess::challenge = 
      randomPatternsMode = inv::on = false;
      break;
    
    case rg::inv:
      inv::on = !inv::on;
      if(tactic::on) firstland = laIce;
      tactic::on = yendor::on = princess::challenge = 
      randomPatternsMode = peace::on = false;
      break;

    case rg::chaos:
      if(euclid || sphere || quotient)
        geometry = gNormal;
      if(tactic::on) firstland = laIce;
      yendor::on = tactic::on = princess::challenge = false;
      need_reset_geometry = true;
      chaosmode = !chaosmode;
      break;

#if CAP_TOUR
    case rg::tour:
      geometry = gNormal;
      yendor::on = tactic::on = princess::challenge = peace::on = inv::on = false;
      chaosmode = nonbitrunc = randomPatternsMode = false;
      shmup::on = false;
      need_reset_geometry = true;    
      tour::on = !tour::on;
      break;
#endif

    case rg::bitrunc:
    case rg::gp:
      if(euclid6) geometry = gNormal;
      nonbitrunc = !nonbitrunc;
      gp::on = (switchWhat == rg::gp && !gp::on);
      need_reset_geometry = true;
      #if CAP_TEXTURE
      if(texture::config.tstate == texture::tsActive) 
        texture::config.tstate = texture::tsAdjusting;
      if(texture::config.tstate_max == texture::tsActive)
        texture::config.tstate = texture::tsAdjusting;
      #endif
      break;

    case rg::geometry:
      if(geometry == targetgeometry) geometry = gNormal;
      else geometry = targetgeometry;
      if(chaosmode && (euclid || sphere || quotient)) chaosmode = false;
      if(nonbitrunc && euclid6) nonbitrunc = false;
      if(gp::on && (S3 != 3 || elliptic)) gp::on = false;
  
      need_reset_geometry = true; 
      #if CAP_TEXTURE
      if(texture::config.tstate == texture::tsActive) 
        texture::config.tstate = texture::tsOff;
      if(texture::config.tstate_max == texture::tsActive)
        texture::config.tstate = texture::tsAdjusting;
      #endif
      break;
    
    case rg::yendor:
      yendor::on = !yendor::on;
      tactic::on = false;
      peace::on = false;
      inv::on = false;
      princess::challenge = false;
      randomPatternsMode = false;
      chaosmode = false;
      if(!yendor::on) firstland = laIce;
      break;

    case rg::tactic:
      tactic::on = !tactic::on;
      yendor::on = false;
      peace::on = false;
      inv::on = false;
      randomPatternsMode = false;
      princess::challenge = false;
      chaosmode = false;
      if(!tactic::on) firstland = laIce;
      break;
    
    case rg::shmup:
      shmup::on = !shmup::on;
      princess::challenge = false;
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

void start_game() {
  if(game_active) return;
  game_active = true;
  if(need_reset_geometry) resetGeometry();
  initcells();

  if(randomPatternsMode) {
    for(int i=0; i<landtypes; i++) {
      randompattern[i] = hrandpos();
      // change probability 1/5 to 2/6
      if(hrand(5) == 0) {
        randompattern[i] -= (randompattern[i] % 5);
        }
      }
    if(randomPatternsMode) firstland = pickLandRPM(laNone);
    clearMemoRPM();
    }

  initgame();
  canmove = true;
  restartGraph();
  resetmusic();
  resetmusic();
  }

void restart_game(char switchWhat) {
  popScreenAll();  
  popAllGames();
  stop_game();
  switch_game_mode(switchWhat);
  start_game();
  }

void stop_game_and_switch_mode(char switchWhat) {
  stop_game();
  switch_game_mode(switchWhat);
  }

purehookset clearmemory;

void clearMemory() {
  callhooks(clearmemory);
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
  }) + 
addHook(hooks_removecells, 0, [] () {
  eliminate_if(crush_next, is_cell_removed);
  eliminate_if(crush_now, is_cell_removed);
  eliminate_if(buggycells, is_cell_removed);
  eliminate_if(butterflies, [] (pair<cell*,int>& p) { return is_cell_removed(p.first); });
  for(int i=0; i<SHSIZE; i++) for(int p=0; p<MAXPLAYER; p++)
    set_if_removed(shpos[p][i], NULL);
  });;
