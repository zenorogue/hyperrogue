// Hyperbolic Rogue -- achievements
// Copyright (C) 2011-2018 Zeno Rogue, see 'hyper.cpp' for details

namespace hr {

#define NUMLEADER 78

bool offlineMode = false;

const char* leadernames[NUMLEADER] = {
  "Score", "Diamonds", "Gold", "Spice", "Rubies", "Elixirs",
  "Shards100", "Totems", "Daisies", "Statues", "Feathers", "Sapphires",
  "Hyperstones", "Time to Win-71", "Turns to Win-71",
  "Time to 10 Hyperstones-103", "Turns to 10 Hyperstones-103", "Orbs of Yendor",
  "Fern Flowers", 
  "Royal Jellies", "Powerstones", "Silver", "Wine", "Emeralds", "Grimoires",
  "Holy Grails", "Red Gems", "Pirate Treasures",
  "Shmup Score", "Shmup Time to Win", "Shmup Knife to Win",
  "Bomberbird Eggs", // 31
  "Ambers", // 32
  "Pearls", // 33
  "Hypersian Rugs", // 34
  "Garnets", // 35
  "Princess Challenge", // 36
  "Ivory Figurines", // 37
  "Elemental Gems", // 38
  "Onyxes", // 39
  "Yendor Challenge", // 40
  "Pure Tactics Mode", // 41
  "Mutant Saplings", // 42
  "Fulgurites", // 43
  "Shmup Score 2p", // 44
  "Coop Shmup Time to Win", // 45
  "Black Lotuses", // 46
  "Mutant Fruits", // 47
  "White Dove Feathers", // 48
  "Pure Tactics Mode (shmup)", // 49
  "Pure Tactics Mode (2p)", // 50
  "Corals", // 51
  "Thornless Roses", // 52
  "Chaos Mode", // 53
  "Tortoise points", // 54
  "Dragon Scales", // 55
  "Apples", // 56
  "Heptagonal Mode", // 57
  "Sunken Treasures", // 58
  "Ancient Jewelry", // 59
  "Golden Eggs", // 60
  "Multiplayer Score", // 61
  "Statistics", // 62
  "Halloween", // 63
  "Amethysts", // 64
  "Slime Molds", // 65
  "Dodecahedra", // 66
  "Green Grass", // 67
  "Spinel", // 68
  "Orb Strategy Score", // 69
  "Real time to Win-OS", // 70
  "Turns to Win-OS", // 71
  "Ancient Weapons", // 72
  "Forgotten Relics", // 73
  "Lava Lilies", // 74
  "Turquoises", // 75
  "Chrysoberyls", // 76
  "Tasty Jellies" // 77
  };

#define LB_STATISTICS 62
#define LB_HALLOWEEN  63

void upload_score(int id, int v);

string achievementMessage[3];
int achievementTimer;
vector<string> achievementsReceived;

bool wrongMode(char flags) {
  if(cheater) return true;
  if(flags == rg::global) return false;
  if(nonbitrunc != (flags == rg::bitrunc)) return true;
  if(gp::on) return true;
  if(irr::on) return true;

  if((geometry != gNormal) != (flags == rg::geometry)) return true;
  
  if(shmup::on != (flags == rg::shmup)) return true;
#if CAP_DAILY
  if(daily::on != (flags == rg::daily)) return true;
#endif
  if(randomPatternsMode) return true;
  if(yendor::on) return true;
  if(peace::on) return true;
  if(tactic::on) return true;
#if CAP_TOUR
  if(tour::on) return true;
#endif
  if(chaosmode != (flags == rg::chaos)) return true;
  if((numplayers() > 1) != (flags == rg::multi)) return true;
  return false;
  }

void achievement_log(const char* s, char flags) {

#ifdef PRINT_ACHIEVEMENTS
  printf("achievement = %s [%d]\n", s, wrongMode(flags));
#endif
  
  if(wrongMode(flags)) return;
  
  for(int i=0; i<isize(achievementsReceived); i++)
    if(achievementsReceived[i] == s) return;
  achievementsReceived.push_back(s);
  
#if CAP_SAVE
  remove_emergency_save();

  FILE *f = fopen(scorefile, "at");
  if(!f) return;
  
  int t = (int) (time(NULL) - timerstart);

  time_t timer = time(NULL);
  char buf[128]; strftime(buf, 128, "%c", localtime(&timer));

  fprintf(f, "ACHIEVEMENT %s turns: %d time: %d at: %d ver: %s c: %d date: %s\n",
    s, turncount, t, int(timerstart), VER, anticheat::certify(s, turncount, t, (int) timerstart, 0), buf);

  fclose(f);
#endif
  }

#if ISSTEAM
void improveItemScores();
#include "private/hypersteam.cpp"
#elif !ISANDROID && !ISIOS
void achievement_init() {}
void achievement_close() {}
void achievement_gain(const char* s, char flags) {
  achievement_log(s, flags);
  }
#endif

void achievement_collection(eItem it, int prevgold, int newgold) {
  if(cheater) return;
  if(randomPatternsMode) return;
  int q = items[it];
  
  if(it == itTreat && q == 50 && (geometry == gSphere || geometry == gElliptic)) 
    achievement_gain("HALLOWEEN1", rg::geometry);

  if(it == itTreat && q == 100 && (geometry == gSphere || geometry == gElliptic)) 
    achievement_gain("HALLOWEEN2", rg::geometry);

  if(q == 1) {
    if(it == itDiamond) achievement_gain("DIAMOND1");
    if(it == itRuby) achievement_gain("RUBY1");
    if(it == itHyperstone) achievement_gain("HYPER1");
    if(it == itGold) achievement_gain("GOLD1");
    if(it == itStatue) achievement_gain("STATUE1");
    if(it == itShard) achievement_gain("MIRROR1");
    if(it == itBone) achievement_gain("TOTEM1");
    if(it == itSpice) achievement_gain("SPICE1");
    if(it == itElixir) achievement_gain("ELIXIR1");
    if(it == itHell) achievement_gain("DAISY1");
    if(it == itFeather) achievement_gain("FEATHER1");
    if(it == itSapphire) achievement_gain("SAPPHIRE1");
    if(it == itFernFlower) achievement_gain("FERN1");
    if(it == itRoyalJelly) achievement_gain("JELLY1");
    if(it == itWine) achievement_gain("WINE1");
    if(it == itPower) achievement_gain("POWER1");
    if(it == itEmerald) achievement_gain("EMERALD1");
    if(it == itSilver) achievement_gain("SILVER1");
    if(it == itGrimoire) achievement_gain("GRIMOIRE1");
    if(it == itRedGem) achievement_gain("REDGEM1");
    if(it == itPirate) achievement_gain("PIRATE1");
    if(it == itCoast) achievement_gain("COAST1");
    // if(it == itWhirlpool) achievement_gain("WHIRL1"); // requires escape
    if(it == itBombEgg) achievement_gain("MINE1");
    if(it == itPalace) achievement_gain("RUG1");
    if(it == itFjord) achievement_gain("GARNET1");

    if(it == itIvory) achievement_gain("TOWER1");
    if(it == itElemental) achievement_gain("ELEMENT1");
    if(it == itZebra) achievement_gain("ZEBRA1");

    if(it == itMutant) achievement_gain("MUTANT1");
    if(it == itFulgurite) achievement_gain("FULGUR1");

    if(it == itMutant2) achievement_gain("FRUIT1");
    if(it == itWindstone) achievement_gain("DOVE1");
    if(it == itCoral) achievement_gain("CORAL1");
    if(it == itRose) achievement_gain("ROSE1");
    
    if(it == itBabyTortoise) achievement_gain("TORTOISE1");
    if(it == itDragon) achievement_gain("DRAGON1");
    if(it == itApple) achievement_gain("APPLE1");

    if(it == itKraken) achievement_gain("KRAKEN1");
    if(it == itBarrow) achievement_gain("BARROW1");
    if(it == itTrollEgg) achievement_gain("TROLL1");

    if(it == itAmethyst) achievement_gain("MOUNT1");
    if(it == itSlime) achievement_gain("DUNG1");
    if(it == itDodeca) achievement_gain("DOD1");

    if(it == itGreenGrass) achievement_gain("PRAIR1");
    if(it == itBull) achievement_gain("BULL1");
    
    if(it == itHunting) achievement_gain("TURQ1");
    if(it == itBlizzard) achievement_gain("BLIZZ1");
    if(it == itLavaLily) achievement_gain("LILY1");
    if(it == itTerra) achievement_gain("TERRAC1");
    
    if(it == itRuins) achievement_gain("RUIN1");
    if(it == itSwitch) achievement_gain("JELLZ1");
    }

  // 32
  if(it == itHolyGrail) {
    if(q == 1) achievement_gain("GRAIL2"), achievement_gain("GRAILH", rg::bitrunc);
    if(q == 3) achievement_gain("GRAIL3");
    if(q == 8) achievement_gain("GRAIL4");
    }
  
  if(q == (inv::on ? 25 : 10)) {
    if(it == itDiamond) achievement_gain("DIAMOND2");
    if(it == itRuby) achievement_gain("RUBY2");
    if(it == itHyperstone) achievement_gain("HYPER2");
    if(it == itGold) achievement_gain("GOLD2");
    if(it == itStatue) achievement_gain("STATUE2");
    if(it == itShard) achievement_gain("MIRROR2");
    if(it == itBone) achievement_gain("TOTEM2");
    if(it == itSpice) achievement_gain("SPICE2");
    if(it == itElixir) achievement_gain("ELIXIR2");
    if(it == itHell) achievement_gain("DAISY2");
    if(it == itFeather) achievement_gain("FEATHER2");
    if(it == itSapphire) achievement_gain("SAPPHIRE2");
    if(it == itFernFlower) achievement_gain("FERN2");
    if(it == itRoyalJelly) achievement_gain("JELLY2");
    if(it == itWine) achievement_gain("WINE2");
    if(it == itPower) achievement_gain("POWER2");
    if(it == itEmerald) achievement_gain("EMERALD2");
    if(it == itSilver) achievement_gain("SILVER2");
    if(it == itGrimoire) achievement_gain("GRIMOIRE2");
    if(it == itRedGem) achievement_gain("REDGEM2");
    if(it == itPirate) achievement_gain("PIRATE2");
    if(it == itCoast) achievement_gain("COAST2");
    if(it == itWhirlpool) achievement_gain("WHIRL2");
    if(it == itBombEgg) achievement_gain("MINE2");
    if(it == itPalace) achievement_gain("RUG2");
    if(it == itFjord) achievement_gain("GARNET2");

    if(it == itIvory) achievement_gain("TOWER2");
    if(it == itElemental) achievement_gain("ELEMENT2");
    if(it == itZebra) achievement_gain("ZEBRA2");

    if(it == itMutant) achievement_gain("MUTANT2");
    if(it == itFulgurite) achievement_gain("FULGUR2");

    if(it == itMutant2) achievement_gain("FRUIT2");
    if(it == itWindstone) achievement_gain("DOVE2");
    if(it == itCoral) achievement_gain("CORAL2");
    if(it == itRose) achievement_gain("ROSE2");

    if(it == itBabyTortoise) achievement_gain("TORTOISE2");
    if(it == itDragon) achievement_gain("DRAGON2");
    if(it == itApple) achievement_gain("APPLE2");

    if(it == itKraken) achievement_gain("KRAKEN2");
    if(it == itBarrow) achievement_gain("BARROW2");
    if(it == itTrollEgg) achievement_gain("TROLL2");

    if(it == itAmethyst) achievement_gain("MOUNT2");
    if(it == itSlime) achievement_gain("DUNG2");
    if(it == itDodeca) achievement_gain("DOD2");

    if(it == itGreenGrass) achievement_gain("PRAIR2");
    if(it == itBull) achievement_gain("BULL2");
    
    if(it == itHunting) achievement_gain("TURQ2");
    if(it == itBlizzard) achievement_gain("BLIZZ2");
    if(it == itLavaLily) achievement_gain("LILY2");
    if(it == itTerra) achievement_gain("TERRAC2");

    if(it == itRuins) achievement_gain("RUIN2");
    if(it == itSwitch) achievement_gain("JELLZ2");
    }

  if(q == (inv::on ? 50 : 25)) {
    if(it == itDiamond) achievement_gain("DIAMOND3");
    if(it == itRuby) achievement_gain("RUBY3");
    if(it == itHyperstone) achievement_gain("HYPER3");
    if(it == itGold) achievement_gain("GOLD3");
    if(it == itStatue) achievement_gain("STATUE3");
    if(it == itShard) achievement_gain("MIRROR3");
    if(it == itBone) achievement_gain("TOTEM3");
    if(it == itSpice) achievement_gain("SPICE3");
    if(it == itElixir) achievement_gain("ELIXIR3");
    if(it == itHell) achievement_gain("DAISY3");
    if(it == itFeather) achievement_gain("FEATHER3");
    if(it == itSapphire) achievement_gain("SAPPHIRE3");
    if(it == itFernFlower) achievement_gain("FERN3");
    if(it == itRoyalJelly) achievement_gain("JELLY3");
    if(it == itWine) achievement_gain("WINE3");
    if(it == itPower) achievement_gain("POWER3");
    if(it == itEmerald) achievement_gain("EMERALD3");
    if(it == itSilver) achievement_gain("SILVER3");
    if(it == itGrimoire) achievement_gain("GRIMOIRE3");
    if(it == itRedGem) achievement_gain("REDGEM3");
    if(it == itPirate) achievement_gain("PIRATE3");
    if(it == itCoast) achievement_gain("COAST3");
    if(it == itWhirlpool) achievement_gain("WHIRL3");
    if(it == itBombEgg) achievement_gain("MINE3");
    if(it == itPalace) achievement_gain("RUG3");
    if(it == itFjord) achievement_gain("GARNET3");

    if(it == itIvory) achievement_gain("TOWER3");
    if(it == itElemental) achievement_gain("ELEMENT3");
    if(it == itZebra) achievement_gain("ZEBRA3");

    if(it == itMutant) achievement_gain("MUTANT3");
    if(it == itFulgurite) achievement_gain("FULGUR3");
    
    if(it == itMutant2) achievement_gain("FRUIT3");
    if(it == itWindstone) achievement_gain("DOVE3");
    if(it == itCoral) achievement_gain("CORAL3");
    if(it == itRose) achievement_gain("ROSE3");

    if(it == itFulgurite && pureHardcore() && elec::lightningfast == 0)
      achievement_gain("HARDMETAL");

    if(it == itBabyTortoise) achievement_gain("TORTOISE3");
    if(it == itDragon) achievement_gain("DRAGON3");
    if(it == itApple) achievement_gain("APPLE3");

    if(it == itKraken) achievement_gain("KRAKEN3");
    if(it == itBarrow) achievement_gain("BARROW3");
    if(it == itTrollEgg) achievement_gain("TROLL3");

    if(it == itAmethyst) achievement_gain("MOUNT3");
    if(it == itSlime) achievement_gain("DUNG3");
    if(it == itDodeca) achievement_gain("DOD3");

    if(it == itGreenGrass) achievement_gain("PRAIR3");
    if(it == itBull) achievement_gain("BULL3");
    
    if(it == itHunting) achievement_gain("TURQ3");
    if(it == itBlizzard) achievement_gain("BLIZZ3");
    if(it == itLavaLily) achievement_gain("LILY3");
    if(it == itTerra) achievement_gain("TERRAC3");

    if(it == itRuins) achievement_gain("RUIN3");
    if(it == itSwitch) achievement_gain("JELLZ3");
    }

  if(q == 50 && !inv::on) {
    if(it == itDiamond) achievement_gain("DIAMOND4");
    if(it == itRuby) achievement_gain("RUBY4");
    if(it == itHyperstone) achievement_gain("HYPER4");
    if(it == itGold) achievement_gain("GOLD4");
    if(it == itStatue) achievement_gain("STATUE4");
    if(it == itShard) achievement_gain("MIRROR4");
    if(it == itBone) achievement_gain("TOTEM4");
    if(it == itSpice) achievement_gain("SPICE4");
    if(it == itElixir) achievement_gain("ELIXIR4");
    if(it == itHell) achievement_gain("DAISY4");
    if(it == itFeather) achievement_gain("FEATHER4");
    if(it == itSapphire) achievement_gain("SAPPHIRE4");
    if(it == itFernFlower) achievement_gain("FERN4");
    if(it == itRoyalJelly) achievement_gain("JELLY4");
    if(it == itWine) achievement_gain("WINE4");
    if(it == itPower) achievement_gain("POWER4");
    if(it == itEmerald) achievement_gain("EMERALD4");
    if(it == itSilver) achievement_gain("SILVER4");
    if(it == itGrimoire) achievement_gain("GRIMOIRE4");
    if(it == itRedGem) achievement_gain("REDGEM4");
    if(it == itPirate) achievement_gain("PIRATE4");
    if(it == itCoast) achievement_gain("COAST4");
    if(it == itWhirlpool) achievement_gain("WHIRL4");
    if(it == itBombEgg) achievement_gain("MINE4");
    if(it == itPalace) achievement_gain("RUG4");
    if(it == itFjord) achievement_gain("GARNET4");

    if(it == itIvory) achievement_gain("TOWER4");
    if(it == itElemental) achievement_gain("ELEMENT4");
    if(it == itZebra) achievement_gain("ZEBRA4");

    if(it == itMutant) achievement_gain("MUTANT4");
    if(it == itFulgurite) achievement_gain("FULGUR4");

    if(it == itMutant2) achievement_gain("FRUIT4");
    if(it == itWindstone) achievement_gain("DOVE4");
    if(it == itCoral) achievement_gain("CORAL4");
    if(it == itRose) achievement_gain("ROSE4");

    if(it == itBabyTortoise) achievement_gain("TORTOISE4");
    if(it == itDragon) achievement_gain("DRAGON4");
    if(it == itApple) achievement_gain("APPLE4");

    if(it == itKraken) achievement_gain("KRAKEN4");
    if(it == itBarrow) achievement_gain("BARROW4");
    if(it == itTrollEgg) achievement_gain("TROLL4");

    if(it == itAmethyst) achievement_gain("MOUNT4");
    if(it == itSlime) achievement_gain("DUNG4");
    if(it == itDodeca) achievement_gain("DOD4");

    if(it == itGreenGrass) achievement_gain("PRAIR4");
    if(it == itBull) achievement_gain("BULL4");
    
    if(it == itHunting) achievement_gain("TURQ4");
    if(it == itBlizzard) achievement_gain("BLIZZ4");
    if(it == itLavaLily) achievement_gain("LILY4");
    if(it == itTerra) achievement_gain("TERRAC4");

    if(it == itRuins) achievement_gain("RUIN4");
    if(it == itSwitch) achievement_gain("JELLZ4");
    }
  
  if(it == itOrbYendor) {
    achievement_gain("YENDOR2");
    if(pureHardcore()) achievement_gain("HARDCORE");
    if(shmup::on) achievement_gain("SHMUP", rg::shmup);
    }
  }

void achievement_count(const string& s, int current, int prev) {
  if(cheater) return;
  if(shmup::on) return;
  if(randomPatternsMode) return;
  if(s == "GOLEM" && current >= 5)
    achievement_gain("GOLEM2");
  if(s == "GOLEM" && current >= 10)
    achievement_gain("GOLEM3");
  if(s == "STAB" && current >= 1)
    achievement_gain("STABBER1");
  if(s == "STAB" && current >= 2)
    achievement_gain("STABBER2");
  if(s == "SLASH" && current >= 2)
    achievement_gain("SLASH2");
  if(s == "STAB" && current >= 4)
    achievement_gain("STABBER3");
  if(s == "MIRRORKILL" && current-prev >= 1)
    achievement_gain("MIRRORKILL1");
  if(s == "MIRRORKILL" && current-prev >= 2)
    achievement_gain("MIRRORKILL2");
  if(s == "MIRRORKILL" && current-prev >= 3)
    achievement_gain("MIRRORKILL3");
  if(s == "FLASH" && current-prev >= 1)
    achievement_gain("FLASH1");
  if(s == "FLASH" && current-prev >= 5)
    achievement_gain("FLASH2");
  if(s == "FLASH" && current-prev >= 10)
    achievement_gain("FLASH3");
  if(s == "LIGHTNING" && current-prev >= 1)
    achievement_gain("LIGHTNING1");
  if(s == "LIGHTNING" && current-prev >= 5)
    achievement_gain("LIGHTNING2");
  if(s == "LIGHTNING" && current-prev >= 10)
    achievement_gain("LIGHTNING3");
  if(s == "MIRAGE" && current >= 35 && geometry == gEuclid)
    achievement_gain("MIRAGE", rg::geometry);
  if(s == "ORB" && current >= 10)
    achievement_gain("ORB3");
  if(s == "BUG" && current >= 1000)
    achievement_gain("BUG3");
  if(s == "ELEC" && current >= 10)
    achievement_gain("ELEC3");
  }

int specific_improved = 0;
int specific_what = 0;

void improve_score(int i, eItem what) {
  if(offlineMode) return;
#ifdef HAVE_ACHIEVEMENTS
  if(haveLeaderboard(i)) updateHi(what, get_currentscore(i));
  if(items[what] && haveLeaderboard(i)) {
    if(items[what] > get_currentscore(i) && score_loaded(i)) {
      specific_improved++; specific_what = what;
      }
    upload_score(i, items[what]);
    }
#endif
  }

void achievement_score(int cat, int number) {
  if(offlineMode) return;
#ifdef HAVE_ACHIEVEMENTS
  if(cheater) return;
  if(cat == LB_HALLOWEEN) {
    if(geometry != gSphere && geometry != gElliptic)
      return;
    }
  else if(geometry) return;
  if(nonbitrunc) return;
  if(randomPatternsMode) return;
  if(shmup::on && cat != LB_PURE_TACTICS_SHMUP && cat != LB_PURE_TACTICS_COOP) return;
  if(yendor::on && cat != LB_YENDOR_CHALLENGE) return;
  if(tactic::on && cat != LB_PURE_TACTICS && cat != LB_PURE_TACTICS_SHMUP && cat != LB_PURE_TACTICS_COOP) 
    return;
  upload_score(cat, number);
#endif
  }

void improveItemScores() {
  for(int i=1; i<=12; i++) improve_score(i, eItem(i));
  improve_score(17, itOrbYendor);
  improve_score(18, itFernFlower);
  improve_score(19, itRoyalJelly);
  improve_score(20, itPower);
  improve_score(21, itSilver);
  improve_score(22, itWine);
  improve_score(23, itEmerald);
  improve_score(24, itGrimoire);
  improve_score(25, itHolyGrail);
  improve_score(26, itRedGem);
  improve_score(27, itPirate);
  improve_score(31, itBombEgg);
  improve_score(32, itCoast);
  improve_score(33, itWhirlpool);
  improve_score(34, itPalace);
  improve_score(35, itFjord);
  
  improve_score(37, itIvory);
  improve_score(38, itElemental);
  improve_score(39, itZebra);

  improve_score(42, itMutant);
  improve_score(43, itFulgurite);

  if(!isHaunted(cwt.at->land)) improve_score(46, itLotus);
  improve_score(47, itMutant2);
  improve_score(48, itWindstone);
  
  improve_score(51, itCoral);
  improve_score(52, itRose);

  improve_score(54, itBabyTortoise);
  improve_score(55, itDragon);
  improve_score(56, itApple);

  improve_score(58, itKraken);
  improve_score(59, itBarrow);
  improve_score(60, itTrollEgg);
  
  improve_score(64, itAmethyst);
  improve_score(65, itSlime);
  improve_score(66, itDodeca);

  improve_score(67, itGreenGrass);
  improve_score(68, itBull);

  improve_score(72, itTerra);
  improve_score(73, itBlizzard);
  improve_score(74, itLavaLily);
  improve_score(75, itHunting);
  
  improve_score(76, itRuins);
  improve_score(77, itSwitch);
  }

int next_stat_tick;

void achievement_final(bool really_final) {
  if(offlineMode) return;

#ifdef HAVE_ACHIEVEMENTS
  if(ticks > next_stat_tick) {
    upload_score(LB_STATISTICS, time(NULL));
    next_stat_tick = ticks + 600000;
    }
  if(cheater) return;

#if CAP_TOUR
  if(tour::on) return;
#endif
  
  if(randomPatternsMode) return;
  if(peace::on) return;
  if(yendor::on) return;

  if(tactic::on) {
    tactic::record();
    tactic::unrecord();
    tactic::uploadScore();
    return;
    }

#if CAP_DAILY
  if(daily::on) {
    daily::uploadscore(really_final);
    return;
    }  
#endif
  
  int specialcode = 0;
  if(shmup::on) specialcode++;
  if(chaosmode) specialcode+=2;
  if(nonbitrunc) specialcode+=4;
  if(numplayers() > 1) specialcode+=8;
  if(inv::on) specialcode+=16;
  
  if(sphere && specialland == laHalloween) {
    if(specialcode) return;
    achievement_score(LB_HALLOWEEN, items[itTreat]);
    return;
    }

  if(geometry) return;
  if(gp::on) return;
  if(irr::on) return;
  
  // determine the correct leaderboard ID for 'total score'
  // or return if no leaderboard for the current mode
  int sid;
  switch(specialcode) {
    case 0:  sid = 0; break;
    case 1:  sid = 28; break;
    case 2:  sid = 53; break;
    case 4:  sid = 57; break;
    case 8:  sid = 61; break;
    case 9:  sid = 44; break;
    case 16: sid = 69; break;
    default: return;
    }
      
  int total_improved = 0;
  specific_improved = 0;
  specific_what = 0;
  
  if(specialcode == 0) improveItemScores(); 
  
  int tg = gold();
  if(tg && haveLeaderboard(sid)) {
    if(tg > get_currentscore(sid) && score_loaded(sid)) {
      if(get_currentscore(sid) <= 0) total_improved += 2;
      total_improved++; // currentscore[sid] = tg;
      }
    upload_score(sid, tg);
    }

  if(total_improved >= 2) {
#if !ISANDROID  
    addMessage(XLAT("Your total treasure has been recorded in the " LEADERFULL "."));
    addMessage(XLAT("Congratulations!"));
#endif
    }
  else if(total_improved && specific_improved >= 2)
    addMessage(XLAT("You have improved your total high score and %1 specific high scores!", its(specific_improved)));
  else if(total_improved && specific_improved)
    addMessage(XLAT("You have improved your total and '%1' high score!", iinf[specific_what].name));
  else if(total_improved) {
#if !ISANDROID  
    addMessage(XLAT("You have improved your total high score on " LEADER ". Congratulations!"));
#endif
    }
  else if(specific_improved >= 2)
    addMessage(XLAT("You have improved %1 of your specific high scores!", its(specific_improved)));
  else if(specific_improved) {
#if !ISANDROID  
    addMessage(XLAT("You have improved your '%1' high score on " LEADER "!", iinf[specific_what].name));
#endif
    }
#endif
  }

bool hadtotalvictory;

void check_total_victory() {
  if(!inv::on) return;
  if(hadtotalvictory) return;
  if(!items[itOrbYendor]) return;
  if(!items[itHolyGrail]) return;
  if(items[itHyperstone] < 50) return;
  if(!princess::reviveAt) return;
  hadtotalvictory = true;
  achievement_gain("TOTALVICTORY");
  }
  
void achievement_victory(bool hyper) {
  DEBB(DF_STEAM, (debugfile,"achievement_victory\n"))
  if(offlineMode) return;
#ifdef HAVE_ACHIEVEMENTS
  if(cheater) return;
  if(geometry) return;
  if(nonbitrunc) return;
  if(randomPatternsMode) return;
  if(hyper && shmup::on) return;
  if(yendor::on) return;
  if(peace::on) return;
  if(tactic::on) return;
  if(chaosmode) return;
  DEBB(DF_STEAM, (debugfile,"after checks\n"))

  int t = getgametime();
  
  if(hyper && shmup::on) return;
  if(hyper && inv::on) return;
  
  int ih1 = hyper ? 15 : inv::on ? 70 : shmup::on ? (numplayers() > 1 ? 45 : 29) : 13;
  int ih2 = hyper ? 16 : inv::on ? 71 : shmup::on ? 30 : 14;
  
  int improved = 0;
  if(score_loaded(ih1) && score_loaded(ih2)) {
    if(get_currentscore(ih1) == score_default(ih1) || get_currentscore(ih2) == score_default(ih2))
      improved += 4;
    
    if(get_currentscore(ih1) > t) {
      improved++;
      }
  
    if(get_currentscore(ih2) > turncount) {
      improved+=2;
      }
    }

  if(hyper)
    addMessage(XLAT("You have collected 10 treasures of each type."));

  if(improved) {
    if(improved >= 4) {
      if(!hyper) addMessage(XLAT("This is your first victory!"));
#if !ISANDROID
      addMessage(XLAT("This has been recorded in the " LEADERFULL "."));
#endif
      addMessage(XLAT("The faster you get here, the better you are!"));
      }
    else if(improved >= 3) {
      if(shmup::on)
        addMessage(XLAT("You have improved both your real time and knife count. Congratulations!"));
      else 
        addMessage(XLAT("You have improved both your real time and turn count. Congratulations!"));
      }
    else if(improved == 1)
      addMessage(XLAT("You have used less real time than ever before. Congratulations!"));
    else if(improved == 2) {
      if(shmup::on)
        addMessage(XLAT("You have used less knives than ever before. Congratulations!"));
      else
        addMessage(XLAT("You have used less turns than ever before. Congratulations!"));
      }
    }
  
  DEBB(DF_STEAM, (debugfile, "uploading scores\n"))
  upload_score(ih1, t);
  upload_score(ih2, turncount);
#endif
  }

void achievement_pump();

#ifndef HAVE_ACHIEVEMENTS
void achievement_pump() {}
#endif

void achievement_display() {
  #ifdef HAVE_ACHIEVEMENTS
  if(achievementTimer) {
    int col = (ticks - achievementTimer);
    if(col > 5000) { achievementTimer = 0; return; }
    if(col > 2500) col = 5000 - col;
    col /= 10; col *= 0x10101;
    displayfr(vid.xres/2, vid.yres/4, 2, vid.fsize * 2, achievementMessage[0], col & 0xFFFF00, 8);
    int w = 2 * vid.fsize;
#if ISMOBILE==0
    while(w>3 && textwidth(w, achievementMessage[1]) > vid.xres) w--;
#endif
    displayfr(vid.xres/2, vid.yres/4 + vid.fsize*2, 2, w, achievementMessage[1], col, 8);
    w = vid.fsize;
#if ISMOBILE==0
    while(w>3 && textwidth(w, achievementMessage[2]) > vid.xres) w--;
#endif
    displayfr(vid.xres/2, vid.yres/4 + vid.fsize*4, 2, w, achievementMessage[2], col, 8);
    }
  #endif
  }

bool isAscending(int i) { 
  return i == 13 || i == 14 || i == 15 || i == 16 || i == 29 || i == 30 || i == 45;
  };

int score_default(int i) {
  if(isAscending(i)) return 1999999999;
  else return 0;
  }

#ifndef HAVE_ACHIEVEMENTS
int get_sync_status() { return 0; }
void set_priority_board(int) { }
#endif

}
