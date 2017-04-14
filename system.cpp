// Hyperbolic Rogue

// Copyright (C) 2011-2016 Zeno Rogue, see 'hyper.cpp' for details

// routines for: initializing/closing, loading/saving, and cheating

bool cblind;
bool autocheat;

int truelotus;
int gamecount;

time_t timerstart, savetime;
bool timerstopped;
int savecount;
bool showoff = false;

// initialize the game
void initgame() {
  DEBB(DF_INIT, (debugfile,"initGame\n"));

  yendor::init(1);
  
  if(safety && safetyseed) {
    shrand(safetyseed);
    firstland = safetyland;
    }
  
  if(tactic::on && (euclid || sphere)) euclidland = firstland;
  
  if(firstland == laNone || firstland == laBarrier)
    firstland = laCrossroads;

  if(firstland == laCrossroads5 && !tactic::on) firstland = laCrossroads2; // could not fit!
  if(firstland == laOceanWall) firstland = laOcean; 
  if(firstland == laHauntedWall) firstland = laGraveyard; 
  if(firstland == laMountain && !tactic::on) firstland = laJungle;
  if(isGravityLand(firstland) && !tactic::on) firstland = laCrossroads;
  
  cwt.c = currentmap->gamestart(); cwt.spin = 0; cwt.mirrored = false;
  cwt.c->land = (euclid || sphere) ? euclidland : firstland;
  
  chaosAchieved = false;

  if(firstland == laElementalWall) cwt.c->land = randomElementalLand();
  
  if(tactic::on && (isGravityLand(firstland) || firstland == laOcean) && firstland != laMountain)
    cwt.c->land = purehepta ? laCrossroads : laCrossroads2;
  createMov(cwt.c, 0);
  
  setdist(cwt.c, BARLEV, NULL);

  if((tactic::on || yendor::on) && isCyclic(firstland)) {
    anthraxBonus = items[itHolyGrail];
    cwt.c->mov[0]->land = firstland;
    if(firstland == laWhirlpool) cwt.c->mov[0]->wall = waSea;
    
    setdist(cwt.c->mov[0], BARLEV-1, cwt.c);
    heptagon *h = createAlternateMap(cwt.c, 2, hsA);
    if(!h) printf("FAIL\n");
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
    items[treasureType(firstland)] = 5;
  
  tactic::lasttactic = firstland;
  yendor::lastchallenge = yendor::challenge;
  
  yendor::init(2);

  for(int i=0; i<65536; i++) euland[i] = laNone;
  
  if(euclid && euclidland == laPrincessQuest) {
    cell *c = euclideanAtCreate(EPX, EPY);
    princess::generating = true;
    c->land = laPalace;
    for(int j=BARLEV; j>=0; j--) setdist(c, j, NULL);
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
  
  // extern int sightrange; sightrange = 9;
  // cwt.c->land = laHell; items[itHell] = 10;
  for(int i=BARLEV; i>=0; i--) {
    if(tactic::trailer && cwt.c->land != laClearing) safety = true;
    setdist(cwt.c, i, NULL);
    if(tactic::trailer) safety = false;
    
    currentmap->verify();
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
    setdist(cwt.c->mov[idir], 0, cwt.c);
    multi::player[i].spin = 0;
    multi::flipped[i] = true;
    multi::whereto[i].d = MD_UNDECIDED;
    }
    
  if(tactic::on && tactic::trailer)
    items[treasureType(firstland)] = 15;
  
  yendor::init(3);
  multi::revive_queue.clear();
#ifdef TOUR
  if(tour::on) tour::presentation(5);
#endif
  
  if(multi::players > 1 && !shmup::on) {
    for(int i=0; i<numplayers(); i++) 
      makeEmpty(playerpos(i));
    }
  else {
    for(int i=0; i<numplayers(); i++) 
      makeEmpty(cwt.c);
    }
  
  if(!safety) {
    usedSafety = false;
    timerstart = time(NULL); turncount = 0; rosewave = 0; rosephase = 0;
    if(!quotient) mapeditor::whichPattern = 0;
    mapeditor::whichShape = 0;
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
    if(!randomPatternsMode && !tactic::on && !yendor::on) {
      if(firstland != (princess::challenge ? laPalace : laIce)) cheater++;
      }
    if(tactic::trailer) ;
#ifdef TOUR
    else if(tour::on) ; // displayed by tour
#endif
    else if(princess::challenge) {
      kills[moVizier] = 1;
      princess::forceMouse = true;
      if(yendor::everwon)
        items[itGreenStone] = 99;
      addMessage(XLAT("Welcome to %the1 Challenge!", moPrincess));
      addMessage(XLAT("The more Hypersian Rugs you collect, the harder it is.", moPrincess));
      }
    /* if(tactic::on && isCrossroads(firstland)) {
      for(int i=0; i<ittypes; i++) 
        if(itemclass(eItem(i)) == IC_TREASURE && i != itHyperstone)
          items[i] = 10;
      kills[moYeti] = 1000;
      } */
    
    else if(randomPatternsMode)
      addMessage(XLAT("Welcome to the Random Pattern mode!"));
    else if(tactic::on)
      addMessage(XLAT("You are playing %the1 in the Pure Tactics mode.", firstland));
    else if(yendor::on)
      addMessage(XLAT("Welcome to the Yendor Challenge %1!", its(yendor::challenge)));
    else if(shmup::on) ; // welcome message given elsewhere
    else if(euclid)
      addMessage(XLAT("Welcome to the Euclidean mode!"));
    else if(sphere && euclidland == laHalloween)
      addMessage(XLAT("Welcome to Halloween!"));
    else if(elliptic)
      addMessage(XLAT("Good luck in the elliptic plane!"));
    else if(sphere)
      addMessage(XLAT("Welcome to Spherogue!"));
#ifdef ROGUEVIZ
    else if(rogueviz::on)
      addMessage(XLAT("Welcome to RogueViz!"));
#endif
    else {
      addMessage(XLAT("Welcome to HyperRogue!"));
#ifndef MOBILE
#ifdef IOS
      addMessage(XLAT("Press F1 or right-shift-click things for help."));
#else
      addMessage(XLAT("Press F1 or right-click things for help."));
#endif
#endif
      }
    }
  else {
    usedSafety = true;
    safety = false;
    }
  
  havewhat = hadwhat = 0; rosemap.clear();
  
  elec::lightningfast = 0;
  
  lastsafety = gold();
  bfs();
  }

bool havesave = true;

#ifndef NOSAVE
#define MAXBOX 300
#define POSSCORE 258 // update this when new boxes are added!

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

void applyBoxNum(int& i, string name = "") {
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

void applyBoxM(eMonster m, bool f = false) {
  fakebox[boxid] = f;
  boxname[boxid] = minf[m].name;
  monsbox[boxid] = true;
  applyBox(kills[m]);
  }

void applyBoxes() {

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
    else if(i == moTentaclewait) applyBoxI(itOrbThorns, true);
    else if(i == moGreater) applyBoxI(itOrbDragon, true);
    else if(i == moGreaterM) applyBoxI(itOrbIllusion, true);
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
  
  for(int i=itOrbLightning; i<25; i++) applyBoxI(eItem(i), true);
  
  applyBoxI(itRoyalJelly);
  applyBoxI(itWine);
  applyBoxI(itSilver);
  applyBoxI(itEmerald);
  applyBoxI(itPower);
  applyBoxI(itOrbFire, true);
  applyBoxI(itOrbInvis, true);
  applyBoxI(itOrbAether, true);
  applyBoxI(itOrbPsi, true);
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
  applyBoxI(itOrbTime, true);
  
  applyBoxM(moHexSnake);
  applyBoxM(moRedTroll);
  applyBoxI(itRedGem);
  applyBoxI(itOrbSpace, true);
  
  int geo = geometry;
  applyBoxNum(geo, ""); geometry = eGeometry(geo);
  applyBoxBool(hardcore, "hardcore");
  applyBoxNum(hardcoreAt, "");
  applyBoxBool(shmup::on, "shmup");
  if(saving) applyBoxSave(euclidland, "euclid land");
  else if(loading) euclidland = eLand(applyBoxLoad("euclid land"));
  else fakebox[boxid++] = true;
  
  applyBoxI(itCoast);
  applyBoxI(itWhirlpool);
  applyBoxI(itBombEgg);
  applyBoxM(moBomberbird);
  applyBoxM(moTameBomberbird);
  applyBoxM(moAlbatross);
  applyBoxI(itOrbFriend, true);
  applyBoxI(itOrbAir, true);
  applyBoxI(itOrbWater, true);
  
  applyBoxI(itPalace);
  applyBoxI(itFjord);
  applyBoxI(itOrbFrog, true);
  applyBoxI(itOrbDiscord, true);
  applyBoxM(moPalace);
  applyBoxM(moFatGuard);
  applyBoxM(moSkeleton);
  applyBoxM(moVizier);
  applyBoxM(moViking);
  applyBoxM(moFjordTroll);
  applyBoxM(moWaterElemental);
  
  applyBoxI(itSavedPrincess);
  applyBoxI(itOrbLove, true);
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
  applyBoxI(itOrbSummon, true);
  applyBoxI(itOrbMatter, true);

  applyBoxM(moForestTroll);
  applyBoxM(moStormTroll);
  applyBoxM(moOutlaw);
  applyBoxM(moMutant);
  applyBoxM(moMetalBeast);
  applyBoxM(moMetalBeast2);
  applyBoxI(itMutant);
  applyBoxI(itFulgurite);
  applyBoxI(itBounty);
  applyBoxI(itOrbLuck, true);
  applyBoxI(itOrbStunning, true);
  
  applyBoxBool(tactic::on, "");
  applyBoxNum(elec::lightningfast, "");
  
  // if(savebox[boxid]) printf("lotus = %d (lost = %d)\n", savebox[boxid], isHaunted(lostin));
  if(loadingHi && isHaunted(lostin)) boxid++;
  else applyBoxI(itLotus);
  applyBoxI(itOrbUndeath, true);
  applyBoxI(itWindstone);
  applyBoxI(itOrbEmpathy, true);
  applyBoxM(moWindCrow);
  applyBoxI(itMutant2);
  applyBoxI(itOrbFreedom, true);
  applyBoxM(moRedFox);
  applyBoxBool(survivalist);
  if(loadingHi) applyBoxI(itLotus);
  else applyBoxNum(truelotus, "lotus/escape");
  applyBoxBool(purehepta, "heptagons only"); 
  applyBoxI(itRose);
  applyBoxI(itOrbBeauty, true);
  applyBoxI(itCoral);
  applyBoxI(itOrb37, true);
  applyBoxI(itOrbEnergy, true);
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
  applyBoxI(itOrbDomination, true);
  applyBoxI(itBabyTortoise);
  applyBoxNum(tortoise::seekbits, "");
  applyBoxM(moTortoise);
  applyBoxI(itOrbShell, true);
  
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
  applyBoxI(itOrbSword, true);
  applyBoxI(itBarrow);
  applyBoxM(moDraugr);
  applyBoxI(itOrbSword2, true);
  applyBoxI(itTrollEgg);
  applyBoxI(itOrbStone, true);
  
  bool sph;
  sph = false; applyBoxBool(sph, "sphere"); if(sph) geometry = gSphere;
  sph = false; applyBoxBool(sph, "elliptic"); if(sph) geometry = gElliptic;
  applyBox(princess::reviveAt);
  
  applyBoxI(itDodeca);
  applyBoxI(itAmethyst);
  applyBoxI(itSlime);
  applyBoxI(itOrbNature, true);
  applyBoxI(itOrbDash, true); 
  // itOrbRecall should not be here
  applyBoxM(moBat);
  applyBoxM(moReptile);
  applyBoxM(moFriendlyIvy);
  
  applyBoxI(itGreenGrass);
  applyBoxI(itBull);
  applyBoxI(itOrbHorns, true);
  applyBoxI(itOrbBull, true);
  applyBoxM(moSleepBull);
  applyBoxM(moRagingBull);
  applyBoxM(moHerdBull);
  applyBoxM(moButterfly);
  applyBoxM(moGadfly);
  
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
  dynamicval<bool> sp5(purehepta, savebox[186]);
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
#ifdef CERTIFY
#include "certify.cpp"
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
#ifndef WINDOWS
  if(saveposition >= 0) { 
/*    if(!timerghost) 
      addMessage(XLAT("Emergency truncate to ") + its(saveposition)); */
    if(truncate(scorefile, saveposition)) {}
    saveposition = -1;
    }
#endif
  }

void saveStats(bool emergency = false) {
  DEBB(DF_INIT, (debugfile,"saveStats [%s]\n", scorefile));

  if(autocheat) return;
  #ifdef TOUR
  if(tour::on) return;
  #endif
  if(randomPatternsMode) return;
  
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
      int score = items[treasureType(tactic::lasttactic)];
      
      if(score) {
        int c = 
          anticheat::certify(dnameof(tactic::lasttactic), turncount, t, (int) timerstart,
            xcode*999 + tactic::id + 256 * score);
        fprintf(f, "TACTICS %s %d %d %d %d %d %d %d date: %s\n", VER,
          tactic::id, tactic::lasttactic, score, turncount, t, int(timerstart), 
          c, buf);
        tactic::record(tactic::lasttactic, score);
        anticheat::nextid(tactic::id, VER, c);
        }
      }

    if(yendor::on)
      fprintf(f, "YENDOR %s %d %d %d %d %d %d %d date: %s\n", VER,
        yendor::lastchallenge, items[itOrbYendor], yendor::won, turncount, t, int(timerstart), 
        anticheat::certify(yendor::won ? "WON" : "LOST", turncount, t, (int) timerstart,
          xcode*999 + yendor::lastchallenge + 256 * items[itOrbYendor]),
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
  if(purehepta) fprintf(f, "Heptagons only mode\n");
  if(chaosmode) fprintf(f, "Chaos mode\n");
  if(shmup::on) fprintf(f, "Shoot-em up mode\n");
  if(multi::players > 1) fprintf(f, "Multi-player (%d players)\n", multi::players);
  fprintf(f, "Number of cells explored, by distance from the player:\n"); 
  {for(int i=0; i<10; i++) fprintf(f, " %d", explore[i]);} fprintf(f, "\n");
/*for(int j=0; j<landtypes; j++) {
    bool haveland = false;
    for(int i=0; i<10; i++) 
      if(exploreland[i][j]) 
        haveland = true;
    if(haveland)
      for(int i=0; i<10; i++) 
        fprintf(f, " %d", exploreland[i][j]);
    fprintf(f, " %s\n", linf[j].name);
    } */
  if(kills[0]) fprintf(f, "walls melted: %d\n", kills[0]);
  fprintf(f, "cells travelled: %d\n", celldist(cwt.c));
  
  fprintf(f, "\n");

  for(int i=0; i<ittypes; i++) if(items[i])  
    fprintf(f, "%4dx %s\n", items[i], iinf[i].name);
    
  fprintf(f, "\n");
  
  for(int i=1; i<motypes; i++) if(kills[i])  
    fprintf(f, "%4dx %s\n", kills[i], minf[i].name);
  
  fprintf(f, "\n\n\n");
  
#ifndef MOBILE
  DEBB(DF_INIT, (debugfile, "Game statistics saved to %s\n", scorefile));
  if(!tactic::trailer)
    addMessage(XLAT("Game statistics saved to %1", scorefile));
#endif
  fclose(f);
  }

// load the save
void loadsave() {
  if(autocheat) return;
#ifdef TOUR
  if(tour::on) return;
#endif
  DEBB(DF_INIT, (debugfile,"loadSave\n"));

  for(int xc=0; xc<MODECODES; xc++)
  for(int i=0; i<landtypes; i++) for(int j=0; j<MAXTAC; j++)
    tactic::lsc[xc][i][j] = -1;
  
  gamecount = 0;

  FILE *f = fopen(scorefile, "rt");
  havesave = f;
  if(!f) return;
  score sc;
  bool ok = false;
  bool tamper = false;
  while(!feof(f)) {
    char buf[120];
    if(fgets(buf, 120, f) == NULL) break;
    if(buf[0] == 'H' && buf[1] == 'y') {
      gamecount++;
      if(fscanf(f, "%s", buf) <= 0) break;
      sc.ver = buf;
      if(sc.ver < "4.4" || sc.ver == "CHEATER!") { ok = false; continue; }
      ok = true;
      for(int i=0; i<MAXBOX; i++) {
        if(fscanf(f, "%d", &sc.box[i]) <= 0) {
          boxid = i;
                    
          tamper = anticheat::load(f, sc, sc.ver);

          for(int i=0; i<boxid; i++) savebox[i] = sc.box[i];
          for(int i=boxid; i<MAXBOX; i++) savebox[i] = 0, sc.box[i] = 0;

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
      sscanf(buf, "%70s%10s%d%d%d%d%d%d%d",
        buf1, ver, &tid, &land, &score, &tc, &t, &ts, &cert);
      
      for(int xc=0; xc<MODECODES; xc++)
        if(tid == tactic::id && (anticheat::check(cert, ver, dnameof(eLand(land)), tc, t, ts, xc*999+tid + 256 * score))) {
          if(score != 0 && !(land == laOcean && ver < string("8.0f")))
            tactic::record(eLand(land), score, xc);
          anticheat::nextid(tactic::id, ver, cert);
          break;
          }
      }

    if(buf[0] == 'Y' && buf[1] == 'E' && buf[2] == 'N') {
      char buf1[80], ver[10];
      int cid, oy, won, tc, t, ts, cert=0;
      sscanf(buf, "%70s%10s%d%d%d%d%d%d%d",
        buf1, ver, &cid, &oy, &won, &tc, &t, &ts, &cert);
            
      if(won) for(int xc=0; xc<MODECODES; xc++)
      if(anticheat::check(cert, ver, won ? "WON" : "LOST", tc, t, ts, xc*999 + cid + 256 * oy)) {
        if(xc == 19 && cid == 25) xc = 0;
        if(cid > 0 && cid < YENDORLEVELS) 
        if(!(ver < string("8.0f") && oy > 1 && cid == 15)) 
        if(!(ver < string("9.3b") && oy > 1 && (cid == 27 || cid == 28))) 
          {
          yendor::bestscore[xc][cid] = max(yendor::bestscore[xc][cid], oy);
          }
        break;
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
    }
  }
#endif

namespace gamestack {

  struct gamedata {
    hrmap *hmap;
    cellwalker cwt;
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
    gdn.hepta = purehepta;
    gd.push_back(gdn);
    }
    
  void pop() {
    gamedata& gdn = gd[size(gd)-1];
    currentmap = gdn.hmap;
    cwt = gdn.cwt;
    viewctr = gdn.viewctr;
    View = gdn.View;
    geometry = gdn.geometry;
    purehepta = gdn.hepta;
    if(shmup::on) shmup::clearMonsters();
    shmup::on = gdn.shmup;
    resetGeometry();
    gd.pop_back();
    bfs();
    }
  
  };

void restartGame(char switchWhat, bool push) {
  DEBB(DF_INIT, (debugfile,"restartGame\n"));
  
  if(push)
    gamestack::push();
  else if(gamestack::pushed()) {
    gamestack::pop();
    return;
    }
  else {
    achievement_final(true);
  #ifndef NOSAVE
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
  
  #ifndef NOSAVE
    anticheat::tampered = false; 
  #endif
    achievementsReceived.clear();
    princess::saved = false;
    princess::reviveAt = 0;
    princess::forceVizier = false;
    princess::forceMouse = false;
    knighted = 0;
    // items[itGreenStone] = 100;
    cellcount = 0;
    clearMemory();
    }
  if(switchWhat == 'C') {
    geometry = gNormal;
    yendor::on = tactic::on = princess::challenge = false;
    resetGeometry();
    chaosmode = !chaosmode;
    }
#ifdef TOUR
  if(switchWhat == 'T') {
    geometry = gNormal;
    yendor::on = tactic::on = princess::challenge = false;
    chaosmode = purehepta = randomPatternsMode = false;
    shmup::on = false;
    resetGeometry();    
    tour::on = !tour::on;
    }
#endif
  if(switchWhat == '7') {
    if(euclid) geometry = gNormal;
    purehepta = !purehepta;
    resetGeometry();
    }
  if(switchWhat == 'g') {
    if(geometry == targetgeometry) geometry = gNormal;
    else geometry = targetgeometry;
    if(chaosmode && geometry != gNormal) chaosmode = false;
    if(purehepta && euclid) purehepta = false;
    resetGeometry();
    }
  if(switchWhat == 'y') {
    yendor::on = !yendor::on;
    tactic::on = false;
    princess::challenge = false;
    randomPatternsMode = false;
    chaosmode = false;
    if(!yendor::on) firstland = laIce;
    }
  if(switchWhat == 't') {
    tactic::on = !tactic::on;
    yendor::on = false;
    randomPatternsMode = false;
    princess::challenge = false;
    chaosmode = false;
    if(!tactic::on) firstland = laIce;
    }
  if(switchWhat == 's') {
    shmup::on = !shmup::on;
    princess::challenge = false;
    }
  if(switchWhat == 'r') {
    randomPatternsMode = !randomPatternsMode;
    tactic::on = false;
    yendor::on = false;
    princess::challenge = false;
    }
  if(switchWhat == 'p') {
    princess::challenge = !princess::challenge;
    firstland = princess::challenge ? laPalace : laIce;
    shmup::on = false;
    tactic::on = false;
    yendor::on = false;
    chaosmode = false;
    }
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

void clearGameMemory() {
  #ifdef HASLINEVIEW
  conformal::renderAutoband();
  conformal::on = false;
  #endif
  DEBB(DF_INIT, (debugfile,"clearGameMemory\n"));
  pathq.clear();
  dcal.clear();
  yendor::yii = NOYENDOR; yendor::yi.clear();
  clearshadow();
  offscreen.clear();  
  princess::clear();
  buggycells.clear();
  mirrors.clear();
  clearing::bpdata.clear();
  tortoise::emap.clear();
  tortoise::babymap.clear();
  seenSevenMines = false;
  #ifdef HASLINEVIEW
  conformal::killhistory.clear();
  conformal::findhistory.clear();
  conformal::movehistory.clear();
  conformal::includeHistory = false;
  #endif
  recallCell = NULL;
  prairie::lasttreasure = NULL;
  prairie::enter = NULL;
  prairie::tchoices.clear();
  prairie::beaststogen.clear();
  butterflies.clear();
  #ifdef ROGUEVIZ
  rogueviz::close();
  #endif
  }

static int orbid = 0;

eItem nextOrb() {
  orbid++;
  eItem i = eItem(orbid % ittypes);
  if(itemclass(i) == IC_ORB) return i;
  else return nextOrb();
  }

eItem randomTreasure() {
  eItem i = eItem(hrand(ittypes));
  if(itemclass(i) == IC_TREASURE) return i;
  else return randomTreasure();
  }

eItem randomTreasure2(int cv) {
  int bq = 60000, cq = 0;
  eItem best = itDiamond;
  eItem lt = localTreasureType();
  for(int a=1; a<ittypes; a++) {
    eItem i = eItem(a);
    if(itemclass(i) != IC_TREASURE) continue;
    int q = 2*items[i];
    if(a == lt) q -= (2*cv-1);
    if(a == itEmerald && bearsCamelot(cwt.c->land)) q -= 8;
    if(a == itElixir && isCrossroads(cwt.c->land)) q -= 7;
    if(a == itIvory && isCrossroads(cwt.c->land)) q -= 6;
    if(a == itPalace && isCrossroads(cwt.c->land)) q -= 5;
    if(a == itIvory && cwt.c->land == laJungle) q -= 5;
    if(a == itIvory && cwt.c->land == laPalace) q -= 5;
    if(q < bq) bq = q, cq = 0;
    if(q == bq) { cq++; if(hrand(cq) == 0) best = i; }
    }
  return best;
  }

bool isTechnicalLand(eLand l) {
  return l == laNone || l == laOceanWall || l == laBarrier || l == laCanvas ||
    l == laHauntedWall || l == laHauntedBorder || l == laCA;
  }

eLand cheatdest;

void cheatMoveTo(eLand l) {
  cheatdest = l;
  if(l == laCrossroads5) l = laCrossroads;
  activateSafety(l);
  cheatdest = laNone;
  }

bool applyCheat(char u, cell *c = NULL) {

  if(u == 'M' && cwt.c->type == 6) {
    addMessage(XLAT("You summon some Mirages!"));
    cheater++;
    mirror::createMirrors(cwt.c, cwt.spin, moMirage),
    mirror::createMirages(cwt.c, cwt.spin, moMirage);
    return true;
    }
  if(u == 'G') {
    addMessage(XLAT("You summon a golem!"));
    cheater++;
    int i = cwt.spin;
    if(passable(cwt.c->mov[i], NULL, 0)) 
      cwt.c->mov[i]->monst = hrand(2) ? moGolem : moTameBomberbird;
    return true;
    }
  if(u == 'L') {
    do {
      if(firstland == eLand(landtypes-1))
        firstland = eLand(2);
      else
        firstland = eLand(firstland+1);
      }
    while(isTechnicalLand(firstland) || isCyclic(firstland));
    euclidland = firstland;
    cheater++; addMessage(XLAT("You will now start your games in %1", firstland));
    return true;
    }
  if(u == 'C') {
    cheater++; 
    cheatMoveTo(laCrossroads);
    addMessage(XLAT("Activated the Hyperstone Quest!"));

    for(int t=1; t<ittypes; t++) 
      if(t != itHyperstone && t != itBounty && itemclass(eItem(t)) == IC_TREASURE) {
        items[t] = 10;
        }
    kills[moYeti] = 200;
    kills[moDesertman] = 200;
    kills[moRunDog] = 200;
    kills[moZombie] = 200;
    kills[moMonkey] = 200;
    kills[moCultist] = 200;
    kills[moTroll] = 200;
    return true;
    }
  if(u == 'P') {
    for(int i=0; i<ittypes; i++) 
      if(itemclass(eItem(i)) == IC_ORB) 
        items[i] = 0;
    cheater++; addMessage(XLAT("Orb power depleted!"));
    return true;
    }
  if(u == 'O') {
    cheater++; addMessage(XLAT("Orbs summoned!"));
    for(int i=0; i<cwt.c->type; i++) 
      if(passable(cwt.c->mov[i], NULL, 0)) {
        eItem it = nextOrb();
        cwt.c->mov[i]->item = it;
        }
    return true;
    }
  if(u == 'F') {
    if(hardcore && !canmove) { 
      canmove = true; 
      addMessage(XLAT("Revived!"));
      }
    else {
      items[itOrbFlash] += 1;
      items[itOrbTeleport] += 1;
      items[itOrbLightning] += 1;
      items[itOrbSpeed] += 1;
      items[itOrbShield] += 1;
      cheater++; addMessage(XLAT("Orb power gained!"));
      canmove = true;
      if(cmode == emQuit) cmode = emNormal;
      }
    return true;
    }
  if(u == 'D') {
    items[itGreenStone] += 10;
    cheater++; addMessage(XLAT("Dead orbs gained!"));
    return true;
    }
  if(u == 'R'-64) buildRosemap();
#ifndef NOEDIT
  if(u == 'A') {
    lastexplore = turncount;
    cmode = emMapEditor;
    return true;
    }
  if(u == 'A'-64) {
    mapeditor::drawcell = mouseover ? mouseover : cwt.c;
    cmode = emDraw;
    return true;
    }
#endif
  if(u == 'Y') {
    items[itOrbYendor] ++;
    cheater++; addMessage(XLAT("Orb of Yendor gained!"));
    return true;
    }
  if(u == 'T') {
    items[randomTreasure2(10)] += 10;
    cheater++; addMessage(XLAT("Treasure gained!"));
    return true;
    }
  if(u == 'T'-64) {
    items[randomTreasure2(100)] += 100;
    cheater++; addMessage(XLAT("Lots of treasure gained!"));
    return true;
    }
  if(u == 'I'-64) {
    items[randomTreasure2(10)] += 25;
    cheater++; addMessage(XLAT("Treasure gained!"));
    return true;
    }
  if(u == 'U'-64) {
    items[randomTreasure2(10)] += 50;
    cheater++; addMessage(XLAT("Treasure gained!"));
    return true;
    }
  if(u == 'W') {
    addMessage(XLAT("You summon a sandworm!"));
    cheater++;
    int i = cwt.spin;
    if(passable(cwt.c->mov[i], NULL, 0))
      cwt.c->mov[i]->monst = moWorm,
      cwt.c->mov[i]->mondir = NODIR;
    return true;
    }
  if(u == 'I') {
    addMessage(XLAT("You summon an Ivy!"));
    cheater++;
    int i = cwt.spin;
    int j = cwt.c->spn(i);
    cell* c = cwt.c->mov[i]->mov[(j+3)%cwt.c->mov[i]->type];
    if(passable(c, NULL, 0)) buildIvy(c, 0, 1);
    return true;
    }
  if(u == 'E') {
    addMessage(XLAT("You summon a monster!"));
    cheater++;
    int i = cwt.spin;
    if(cwt.c->mov[i]->wall == waChasm)
      cwt.c->mov[i]->wall = waNone;
    if(passable(cwt.c->mov[i], NULL, P_MONSTER)) {
      eMonster mo[9] = { moEagle, moPyroCultist, moGhost, moTroll, moMiner, moVineBeast, moBug0,
        moBomberbird, moSkeleton };
      cwt.c->mov[i]->monst = mo[hrand(9)];
      cwt.c->mov[i]->stuntime = 3;
      cwt.c->mov[i]->hitpoints = 3;
      }
    return true;
    }
  if(u == 'E'-64) {
    if(geometry) {
      restartGame(0, false);
      }
    else {
      euclidland = cwt.c->land;
      printf("target geometry = %d\n", targetgeometry);
      restartGame('g', true);
      }
    return true;
    }
  if(u == 'H') {
    addMessage(XLAT("You summon some Thumpers!"));
    cheater++;
    for(int i=0; i<cwt.c->type; i++) 
      if(passable(cwt.c->mov[i], NULL, 0)) {
        eWall ws[3] = { waThumperOff, waBigStatue, waBoat };
        cwt.c->mov[i]->wall = ws[hrand(3)];
        }
    return true;
    }
  if(u == 'B') {
    addMessage(XLAT("You summon a bonfire!"));
    cheater++;
    int i = cwt.spin;
    if(passable(cwt.c->mov[i], NULL, 0))
      cwt.c->mov[i]->wall = waBonfireOff;
    return true;
    }
  if(u == 'Z') {
    flipplayer = false;
    mirror::spin(1);
    cwspin(cwt, 1);
    return true;
    }
  if(u == 'J') {
    if(items[localTreasureType()] > 0)
      items[localTreasureType()] = 0;
    else for(int i=1; i<ittypes; i++) 
      if(itemclass(eItem(i)) == IC_TREASURE) 
        items[i] = 0;
    cheater++; addMessage(XLAT("Treasure lost!"));
    return true;
    }
  if(u == 'K') {
    for(int i=0; i<motypes; i++) kills[i] += 10;
    kills[moPlayer] = 0;
    cheater++; addMessage(XLAT("Kills gained!"));
    return true;
    }
  if(u == 'S') {
    canmove = true;
    cheatMoveTo(cwt.c->land);
    items[itOrbSafety] += 3;
    cheater++; addMessage(XLAT("Activated Orb of Safety!"));
    return true;
    }
  if(u == 'U') {
    canmove = true;
    cheatMoveTo(firstland);
    cheater++; addMessage(XLAT("Teleported to %1!", firstland));
    return true;
    }
  if(u == 'W'-64) {
    webdisplay++;
    cheater++; addMessage(XLAT("Cheat-changed the display.", firstland));
    return true;
    }
  if(u == 'G'-64) {
    timerghost = !timerghost;
    cheater++; 
    addMessage(XLAT("turn count = %1 last exploration = %2 ghost timer = %3",
      its(turncount), its(lastexplore), ONOFF(timerghost)));
    return true;
    }
  if(u == 'F'-64) {
    items[itOrbShield] += 30;
    return true;
    }
  if(u == 'Y'-64) {
    int i = cwt.spin;
    cwt.c->mov[i]->item = itOrbYendor;
    return true;
    }
  if(u == 'B'-64) {
    int i = cwt.spin;
    sword::angle[0]++;
    cwt.c->mov[i]->item = hrand(2) ? itOrbSword2 : itOrbSword;
    return true;
    }
  if(u == 'X'-64) {
    items[itOrbNature] += 50;
    cheater++;
    return true;
    }
  if(u == 'V'-64) {
    viewdists = !viewdists;
    return true;
    }
  if(u == 'L'-64) {
    cell *c = mouseover;
    describeCell(c);
    printf("Neighbors:"); for(int i=0; i<c->type; i++) printf("%p ",  c->mov[i]);
    printf("Barrier: dir=%d left=%d right=%d\n",
      c->bardir, c->barleft, c->barright);
    return true;
    }
  if(u == 'C'-64) {
    cblind = !cblind;
    return true;
    }
#ifdef LOCAL
  if(u == 'K'-64) {
    printf("viewctr = %p.%d\n", viewctr.h, viewctr.spin);
    display(View);
    }
  if(u == 'D'-64) {
    cheater = 0; autocheat = 0;
    return true;
    }
#endif
  return false;
  }

