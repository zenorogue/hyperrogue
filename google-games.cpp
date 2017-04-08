// PRIVATE

void viewAchievements();
void viewLeaderboard(string id);
void switchGoogleConnection();

namespace leader { 

bool currentlyConnecting();
bool currentlyConnected();

string leaderboardIDs[NUMLEADER] = {
  "CgkIspzRq_8NEAIQAg",
  "CgkIspzRq_8NEAIQKg",
  "CgkIspzRq_8NEAIQLA",
  "CgkIspzRq_8NEAIQKw",
  "CgkIspzRq_8NEAIQLQ",

  "CgkIspzRq_8NEAIQMA",
  "CgkIspzRq_8NEAIQLg",
  "CgkIspzRq_8NEAIQNQ",
  "CgkIspzRq_8NEAIQNg",
  "CgkIspzRq_8NEAIQMg",

  "CgkIspzRq_8NEAIQLw",
  "CgkIspzRq_8NEAIQOA",
  "CgkIspzRq_8NEAIQOg",
  "CgkIspzRq_8NEAIQPA",
  "CgkIspzRq_8NEAIQOw",

  "CgkIspzRq_8NEAIQPg", // 15
  "CgkIspzRq_8NEAIQPQ",
  "CgkIspzRq_8NEAIQUA",
  "CgkIspzRq_8NEAIQMw",
  "CgkIspzRq_8NEAIQUQ", 

  "CgkIspzRq_8NEAIQVA", // 20
  "CgkIspzRq_8NEAIQUw", 
  "CgkIspzRq_8NEAIQVQ", 
  "CgkIspzRq_8NEAIQUg",
  "CgkIspzRq_8NEAIQcQ", // Grimories
  "CgkIspzRq_8NEAIQcg", // 25 Holy Grails

  "CgkIspzRq_8NEAIQdA", // 26 Red Gems
  "CgkIspzRq_8NEAIQcw", // 27 Pirate Treasures

  "CgkIspzRq_8NEAIQ4QE", // 28 Shmup mode

  "CgkIspzRq_8NEAIQ4gE", // 29 Shmup time to win
  "CgkIspzRq_8NEAIQ4wE",  // 30 Shmup knives to win

  "CgkIspzRq_8NEAIQigE", // 31 bombeggs
  "CgkIspzRq_8NEAIQiwE", // 32 ambers
  "CgkIspzRq_8NEAIQjAE", // 33 pearls
  "CgkIspzRq_8NEAIQjQE", // 34 hyperrugs
  "CgkIspzRq_8NEAIQjgE", // 35 garnets

  "CgkIspzRq_8NEAIQ5gE", // 36 princess challenge

  "CgkIspzRq_8NEAIQnAE", // 37 Ivory Figurines
  "CgkIspzRq_8NEAIQmgE", // 38 Elemental Gems
  "CgkIspzRq_8NEAIQmwE", // 39 Onyxes


  "CgkIspzRq_8NEAIQwAE", // 40 Yendor Challenge
  "CgkIspzRq_8NEAIQ5AE", // 41 Pure Tactics Mode

  "CgkIspzRq_8NEAIQpgE", // 42 Mutant Saplings
  "CgkIspzRq_8NEAIQpQE", // 43 Fulgurites

  "", // 44
  "", // 45
  "CgkIspzRq_8NEAIQswE", // 46 Black Lotuses
  "CgkIspzRq_8NEAIQsgE", // 47 Mutant Fruits
  "CgkIspzRq_8NEAIQsQE", // 48 White Dove

  "CgkIspzRq_8NEAIQ5QE", // 49 PTM shmup
  "", // 50


  "CgkIspzRq_8NEAIQtQE", // 51 Corals
  "CgkIspzRq_8NEAIQtgE", // 52 Thornless Roses
  "CgkIspzRq_8NEAIQtwE",  // 53 Chaos Mode

  "CgkIspzRq_8NEAIQwwE", // 54 Tortoise Points
  "CgkIspzRq_8NEAIQwgE", // 55 Dragon Scales
  "CgkIspzRq_8NEAIQwQE", // 56 Apples
  "CgkIspzRq_8NEAIQ4AE", // 57 Heptagonal mode
  "CgkIspzRq_8NEAIQ3gE", // 58 Sunken treasures
  "CgkIspzRq_8NEAIQ3QE", // 59 Ancient Jewelry
  "CgkIspzRq_8NEAIQ3wE", // 60 Golden Eggs
  
  "", // 61 multiplayer
  "", // 62 statistics

  "CgkIspzRq_8NEAIQ6gE", // 63 Halloween
    
  "CgkIspzRq_8NEAIQ9AE", // 64 Amethyst
  "CgkIspzRq_8NEAIQ9QE", // 65 Slime Molds
  "CgkIspzRq_8NEAIQ9gE", // 66 Dodecahedra

  "CgkIspzRq_8NEAIQ_gE", // 67 Green Grass
  "CgkIspzRq_8NEAIQ_QE", // 68 Spinels
    };

const char* achievementIDs[] = {
         "DIAMOND2", "CgkIspzRq_8NEAIQAw",
         "SPICE2", "CgkIspzRq_8NEAIQBA",
         "GOLD2", "CgkIspzRq_8NEAIQBQ",
         "RUBY2", "CgkIspzRq_8NEAIQCQ",
         "MIRROR2", "CgkIspzRq_8NEAIQBw",
         "FEATHER2", "CgkIspzRq_8NEAIQDA",
         "ELIXIR2", "CgkIspzRq_8NEAIQDQ",
         "STATUE2", "CgkIspzRq_8NEAIQBg",
         "FERN2", "CgkIspzRq_8NEAIQDg",
         "TOTEM2", "CgkIspzRq_8NEAIQCA",
         "DAISY2", "CgkIspzRq_8NEAIQCw",
         "SAPPHIRE2", "CgkIspzRq_8NEAIQDw",
         "HYPER2", "CgkIspzRq_8NEAIQCg",
         "DIAMOND3", "CgkIspzRq_8NEAIQEA",
         "SPICE3", "CgkIspzRq_8NEAIQEQ",
         "GOLD3", "CgkIspzRq_8NEAIQEg",
         "RUBY3", "CgkIspzRq_8NEAIQEw",
         "MIRROR3", "CgkIspzRq_8NEAIQFA",
         "FEATHER3", "CgkIspzRq_8NEAIQFQ",
         "ELIXIR3", "CgkIspzRq_8NEAIQFg",
         "STATUE3", "CgkIspzRq_8NEAIQFw",
         "FERN3", "CgkIspzRq_8NEAIQGA",
         "TOTEM3", "CgkIspzRq_8NEAIQGQ",
         "DAISY3", "CgkIspzRq_8NEAIQGg",
         "SAPPHIRE3", "CgkIspzRq_8NEAIQGw",
         "HYPER3", "CgkIspzRq_8NEAIQHA",
         "DIAMOND4", "CgkIspzRq_8NEAIQHQ",
         "SPICE4", "CgkIspzRq_8NEAIQHg",
         "GOLD4", "CgkIspzRq_8NEAIQHw",
         "RUBY4", "CgkIspzRq_8NEAIQIA",
         "MIRROR4", "CgkIspzRq_8NEAIQIQ",
         "FEATHER4", "CgkIspzRq_8NEAIQIg",
         "ELIXIR4", "CgkIspzRq_8NEAIQIw",
         "STATUE4", "CgkIspzRq_8NEAIQJA",
         "FERN4", "CgkIspzRq_8NEAIQJQ",
         "TOTEM4", "CgkIspzRq_8NEAIQJg",
         "DAISY4", "CgkIspzRq_8NEAIQJw",
         "SAPPHIRE4", "CgkIspzRq_8NEAIQKA",
         "HYPER4", "CgkIspzRq_8NEAIQKQ",
         "STABBER2", "CgkIspzRq_8NEAIQPw",
         "STABBER3", "CgkIspzRq_8NEAIQQA",
         "FLASH2", "CgkIspzRq_8NEAIQQQ",
         "FLASH3", "CgkIspzRq_8NEAIQQg",
         "LIGHTNING2", "CgkIspzRq_8NEAIQQw",
         "LIGHTNING3", "CgkIspzRq_8NEAIQRA",
         "FALLDEATH1", "CgkIspzRq_8NEAIQRQ",
         "GOLEM2", "CgkIspzRq_8NEAIQRg",
         "GOLEM3", "CgkIspzRq_8NEAIQRw",
         "MIRAGE", "CgkIspzRq_8NEAIQSA",
         "MIRRORKILL2", "CgkIspzRq_8NEAIQSQ",
         "MIRRORKILL3", "CgkIspzRq_8NEAIQSg",
         "IVYSLAYER", "CgkIspzRq_8NEAIQSw",
         "DEMONSLAYER", "CgkIspzRq_8NEAIQTA",
         "YENDOR1", "CgkIspzRq_8NEAIQTQ",
         "YENDOR2", "CgkIspzRq_8NEAIQTg",

         "WINE2", "CgkIspzRq_8NEAIQVg",
         "EMERALD2", "CgkIspzRq_8NEAIQVw",
         "SILVER2", "CgkIspzRq_8NEAIQWA",
         "JELLY2", "CgkIspzRq_8NEAIQWQ",
         "POWER2", "CgkIspzRq_8NEAIQWg",
                  
         "WINE3", "CgkIspzRq_8NEAIQWw",
         "EMERALD3", "CgkIspzRq_8NEAIQXQ",
         "SILVER3", "CgkIspzRq_8NEAIQXA",
         "JELLY3", "CgkIspzRq_8NEAIQXg",
         "POWER3 ", "CgkIspzRq_8NEAIQXw",
         
         "WINE4", "CgkIspzRq_8NEAIQYA",
         "EMERALD4", "CgkIspzRq_8NEAIQYQ",
         "SILVER4", "CgkIspzRq_8NEAIQYg",
         "JELLY4", "CgkIspzRq_8NEAIQYw",
         "POWER4", "CgkIspzRq_8NEAIQZA",
                  
         "ORB3", "CgkIspzRq_8NEAIQZQ",
         "BUG3", "CgkIspzRq_8NEAIQZg",
         "LIFEBRINGER", "CgkIspzRq_8NEAIQZw",
         "GARDENER ", "CgkIspzRq_8NEAIQaA",
         "MELEE3", "CgkIspzRq_8NEAIQaQ",
         "MELEE5", "CgkIspzRq_8NEAIQag",

         "GRIMOIRE2", "CgkIspzRq_8NEAIQaw",
         "GRIMOIRE3", "CgkIspzRq_8NEAIQbA",
         "GRIMOIRE4", "CgkIspzRq_8NEAIQbQ",

         "GRAIL2", "CgkIspzRq_8NEAIQbg",
         "GRAIL3", "CgkIspzRq_8NEAIQbw",
         "GRAIL4", "CgkIspzRq_8NEAIQcA",

         "PIRATE2", "CgkIspzRq_8NEAIQdQ",
         "PIRATE3", "CgkIspzRq_8NEAIQdg",
         "PIRATE4", "CgkIspzRq_8NEAIQdw",

         "REDGEM2", "CgkIspzRq_8NEAIQeA",
         "REDGEM3", "CgkIspzRq_8NEAIQeQ",
         "REDGEM4", "CgkIspzRq_8NEAIQeg",

         "COAST2", "CgkIspzRq_8NEAIQew",
         "COAST3", "CgkIspzRq_8NEAIQfA",
         "COAST4", "CgkIspzRq_8NEAIQfQ",
         
         "WHIRL2", "CgkIspzRq_8NEAIQfg",
         "WHIRL3", "CgkIspzRq_8NEAIQfw",
         "WHIRL4", "CgkIspzRq_8NEAIQgAE",
         
         "MINE2", "CgkIspzRq_8NEAIQgQE",
         "MINE3", "CgkIspzRq_8NEAIQggE",
         "MINE4", "CgkIspzRq_8NEAIQgwE",
         
         "RUG2", "CgkIspzRq_8NEAIQhAE",
         "RUG3", "CgkIspzRq_8NEAIQhQE",
         "RUG4", "CgkIspzRq_8NEAIQhgE",

         "GARNET2", "CgkIspzRq_8NEAIQhwE",
         "GARNET3", "CgkIspzRq_8NEAIQiAE",
         "GARNET4", "CgkIspzRq_8NEAIQiQE",

         "ZEBRA2", "CgkIspzRq_8NEAIQkQE",
         "ZEBRA3", "CgkIspzRq_8NEAIQkgE",
         "ZEBRA4", "CgkIspzRq_8NEAIQkwE",
         "ELEMENT2", "CgkIspzRq_8NEAIQlAE",
         "ELEMENT3", "CgkIspzRq_8NEAIQlQE",
         "ELEMENT4", "CgkIspzRq_8NEAIQlgE",
         "TOWER2", "CgkIspzRq_8NEAIQlwE",
         "TOWER3", "CgkIspzRq_8NEAIQmAE",
         "TOWER4", "CgkIspzRq_8NEAIQmQE",

         "PRINCESS1", "CgkIspzRq_8NEAIQjwE",
         "PRINCESS_PACIFIST", "CgkIspzRq_8NEAIQkAE",
         // Achievements.unlock(mGoogleApiClient, "CgkIspzRq_8NEAIQAw"),

         "FULGUR2", "CgkIspzRq_8NEAIQoAE",
         "FULGUR3", "CgkIspzRq_8NEAIQoQE",
         "FULGUR4", "CgkIspzRq_8NEAIQogE",
         "HARDMETAL", "CgkIspzRq_8NEAIQpAE",
         "ELEC3", "CgkIspzRq_8NEAIQowE",
         "MUTANT2", "CgkIspzRq_8NEAIQnQE",
         "MUTANT3", "CgkIspzRq_8NEAIQngE",
         "MUTANT4", "CgkIspzRq_8NEAIQnwE",

         "DOVE2", "CgkIspzRq_8NEAIQqwE",
         "DOVE3", "CgkIspzRq_8NEAIQrAE",
         "DOVE4", "CgkIspzRq_8NEAIQrQE",
         "LOTUS2", "CgkIspzRq_8NEAIQpwE",
         "LOTUS3", "CgkIspzRq_8NEAIQqAE",
         "LOTUS4", "CgkIspzRq_8NEAIQqQE",
         "FRUIT2", "CgkIspzRq_8NEAIQrgE",
         "FRUIT3", "CgkIspzRq_8NEAIQrwE",
         "FRUIT4", "CgkIspzRq_8NEAIQsAE",
         "COMEBACK", "CgkIspzRq_8NEAIQtAE",
         "SURVIVAL", "CgkIspzRq_8NEAIQqgE",

         "CORAL2", "CgkIspzRq_8NEAIQuAE",
         "CORAL3", "CgkIspzRq_8NEAIQuQE",
         "CORAL4", "CgkIspzRq_8NEAIQugE",
         "ROSE2", "CgkIspzRq_8NEAIQuwE",
         "ROSE3", "CgkIspzRq_8NEAIQvAE",
         "ROSE4", "CgkIspzRq_8NEAIQvQE",
         "CR4", "CgkIspzRq_8NEAIQvgE",
         "CHAOS", "CgkIspzRq_8NEAIQvwE",

         "APPLE2",  "CgkIspzRq_8NEAIQxAE", 
         "APPLE3",  "CgkIspzRq_8NEAIQxQE",
         "APPLE4",  "CgkIspzRq_8NEAIQxgE",
         "DRAGON2", "CgkIspzRq_8NEAIQxwE",
         "DRAGON3", "CgkIspzRq_8NEAIQyAE",
         "DRAGON4", "CgkIspzRq_8NEAIQyQE",
         "TORTOISE2", "CgkIspzRq_8NEAIQygE",
         "TORTOISE3", "CgkIspzRq_8NEAIQywE",
         "TORTOISE4", "CgkIspzRq_8NEAIQzAE",     

          "KRAKEN2", "CgkIspzRq_8NEAIQ0QE",
          "KRAKEN3", "CgkIspzRq_8NEAIQ0gE",
          "KRAKEN4", "CgkIspzRq_8NEAIQ0wE",
          "BARROW2", "CgkIspzRq_8NEAIQ1AE",
          "BARROW3", "CgkIspzRq_8NEAIQ1QE",
          "BARROW4", "CgkIspzRq_8NEAIQ1gE",
          "TROLL2", "CgkIspzRq_8NEAIQ1wE",
          "TROLL3", "CgkIspzRq_8NEAIQ2AE",
          "TROLL4", "CgkIspzRq_8NEAIQ2QE",
          "GRAILH", "CgkIspzRq_8NEAIQ2gE",
          "HARDCORE", "CgkIspzRq_8NEAIQ3AE",
          "SHMUP", "CgkIspzRq_8NEAIQ2wE",
          "SLASH2", "CgkIspzRq_8NEAIQzwE",
          "GOSWORD", "CgkIspzRq_8NEAIQ0AE",
          
          "HALLOWEEN1", "CgkIspzRq_8NEAIQ5wE",
          "HALLOWEEN2", "CgkIspzRq_8NEAIQ6AE",
          "GOBLINSWORD", "CgkIspzRq_8NEAIQ6QE",
          
          "DOD2", "CgkIspzRq_8NEAIQ6wE",
          "DOD3", "CgkIspzRq_8NEAIQ7AE",
          "DOD4", "CgkIspzRq_8NEAIQ7QE",
          "DUNG2", "CgkIspzRq_8NEAIQ7gE",
          "DUNG3", "CgkIspzRq_8NEAIQ7wE",
          "DUNG4", "CgkIspzRq_8NEAIQ8AE",
          "MOUNT2", "CgkIspzRq_8NEAIQ8QE",
          "MOUNT3", "CgkIspzRq_8NEAIQ8gE",
          "MOUNT4", "CgkIspzRq_8NEAIQ8wE",
          
          "PRAIR2", "CgkIspzRq_8NEAIQ9wE",
          "PRAIR3", "CgkIspzRq_8NEAIQ-AE",
          "PRAIR4", "CgkIspzRq_8NEAIQ-QE",
          "BULL2", "CgkIspzRq_8NEAIQ-gE",
          "BULL3", "CgkIspzRq_8NEAIQ-wE",
          "BULL4", "CgkIspzRq_8NEAIQ_AE",
           NULL
  };

const char *getGoogleAchievementID(const string& s) {
  const char** a = achievementIDs;
  while(*a && **a) {
    if(a[0] == s) return a[1];
    a++;
    }
  return NULL;
  }

int qleader, leaders[NUMLEADER];

int pshift;

void showMenu() {
  dialog::init();
  dialog::addItem(XLAT("return to the game"), ' ');
  if(!currentlyConnecting())
    dialog::addBoolItem(XLAT("connected to Google Games"), currentlyConnected(), '1');
  else
    dialog::addBreak(100);
  dialog::addItem(XLAT("view your achievements"), '2');
  dialog::addItem(XLAT("next page"), '3');
  
  qleader = 0;
  for(int i=0; i<NUMLEADER; i++) if(leaderboardIDs[i] != "")
    leaders[qleader++] = i;
  
  dialog::addBreak(50);
  
  for(int i=0; i<10; i++) {
    int ip = i + pshift;
    if(ip < qleader) {
      int id = leaders[ip];
      int sco = currentscore[id];
      string ss = sco >= 0 ? its(sco) : SCORE_UNKNOWN ? "?" : "-";
      dialog::addSelItem(leadernames[id], ss, 'a' + i);
      }
    }
  dialog::display();
  }

void handleKey(int sym, int uni) {
  dialog::handleNavigation(sym, uni);
  if(uni == '1')
    switchGoogleConnection();
  else if(uni == '2')
    viewAchievements();
  else if(uni == '3') {
    pshift += 10;
    if(pshift >= qleader) pshift = 0;
    }
  else if(uni >= 'a' && uni <= 'a' + 10) {
    int id = (uni - 'a') + pshift;
    if(id >= qleader) return;
    viewLeaderboard(leaderboardIDs[leaders[id]]);
    }
  else
    cmode = emNormal;
  }

bool ccon = false;
bool incon = false;

bool currentlyConnected() { return ccon; }
bool currentlyConnecting() { return incon; }

int scale(int i) { if(i == 13 || i == 15 || i == 29 || i == 45) return 1000; else return 1; }

bool isDescending(int i) { return !isAscending(i); }
  
}
