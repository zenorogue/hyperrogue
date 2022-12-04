// Hyperbolic Rogue -- help routines
// Copyright (C) 2011-2018 Zeno Rogue, see 'hyper.cpp' for details

/** \file help.cpp
 *  \brief Building and displaying help text
 */

#include "hyper.h"
namespace hr {

EX string help;

EX function<void()> help_delegate;

#if HDR
struct help_extension {
  int key;
  string text;
  string subtext;
  color_t color;
  reaction_t action;
  help_extension() { color = forecolor; }
  help_extension(char k, string t, reaction_t a) : key(k), text(t), action(a) { color = forecolor; }
  };
#endif

EX vector<help_extension> help_extensions;

vector<string> quick_keys = {
  "1 = orthogonal/Gans model/FPP",
  "2 = small Poincare model/stereographic projection/SPP",
  "3 = big Poincare model/stereographic projection/TPP",
  "4 = Klein model/gnomonic projection",
  "5 = change wall display mode",
  "6 = change grid",
  "7 = change heptagon marking",
  "8 = monster display mode"
  };

vector<string> normal_keys = {
  "qweasdzxc, hjklyubn, numpad = move/skip turn",
  "g = drop a Dead Orb",
  "t = use a ranged Orb (target center of the screen)"
  };

vector<string> extra_keys = {
  "o = world overview (or another meaning in special modes)",
  "v = menu",
  "F1 = help",
  "F5 = restart game",
  "F10 = quit game",
  "Esc = quest status",
  "Alt+Enter = full screen",
  "Alt = highlight interesting stuff",
  "click left mouse button = move/skip",
  "[shift+]click left mouse button = use ranged Orb (depending on mouse settings)",
  "click right mouse button = context help",
  "mousewheel up = panning",
  "hold middle mouse button = panning",
  "lctrl + hold middle button = move the screen",
  "mousewheel down = move/skip",
  "rshift + mousewheel = change projection",
  "lshift + mousewheel = change zoom (lctrl to keep center)",
  "lctrl + mousewheel = reset the map center",
  "shift + F2 = disable the HUD",
  "shift + F3 = disable the FPS",
  "shift + F4 = disable the map",
  "space = recenter",
  "ctrl + <key> = more precision"
  };

vector<string> extra_keys_2d = {
  "arrows = panning",
  "PageUp/Down = rotate the screen",
  };

vector<string> extra_keys_3d = {
  "arrows = rotate the camera",
  "rshift+arrows = strafe",
  "lshift+arrows = rotate the model (in rug mode)",
  "end = move camera forward",
  "home = move camera backward",
  "shift+Home/End = zoom",
  "PageUp/Down = rotate the screen",
  "move mouse = rotate camera (in rug, only with lctrl)",
  };

EX void buildHelpText() {
  DEBBI(DF_GRAPH, ("buildHelpText"));

  help = XLAT("Welcome to HyperRogue");
#if ISANDROID  
  help += XLAT(" for Android");
#endif
#if ISIOS
  help += XLAT(" for iOS");
#endif
  help += XLAT("! (version %1)\n\n", VER);
  
  if(!game_keys_scroll) help += XLAT(
    "You have been trapped in a strange, non-Euclidean world. Collect as much treasure as possible "
    "before being caught by monsters. The more treasure you collect, the more "
    "monsters come to hunt you, as long as you are in the same land type. The "
    "Orbs of Yendor are the ultimate treasure; get at least one of them to win the game!"
    );
  if(!game_keys_scroll) help += XLAT(" (press ESC for some hints about it).");
  if(!game_keys_scroll) help += "\n\n";
  
  if(!shmup::on && !hardcore && !game_keys_scroll)
    help += XLAT(
      "You can fight most monsters by moving into their location. "
      "The monster could also kill you by moving into your location, but the game "
      "automatically cancels all moves which result in that.\n\n"
      );
      
  if(shmup::on && !game_keys_scroll) {
    help += XLAT(
      "Shmup (shoot'em up) mode: You can play a hyperbolic shoot'em up game. The game is based "
      "on the usual turn-based grid-based HyperRogue, but there are some changes. You fight by "
      "throwing knives, and you have three extra lives. There are no allies, so all Orbs "
      "related to allies give you extra lives instead (up to 5). Some other rules have been "
      "adapted too.\n\n");  
    }
  
  if(shmup::on && multi::players > 1 && !game_keys_scroll) {
    help += XLAT(
      "Multiplayer: Play cooperatively (locally); treasures, kills, and deaths are calculated "
      "for each player too, for more competitive play. Orbs and treasures are shared, orbs drain "
      "faster, knives recharge slower, and player characters are not allowed to separate.\n\n");
    }
  
  if(multi::players > 1 && !shmup::on && !game_keys_scroll) {
    help += XLAT(
      "Turn-based multiplayer: Turns are executed in parallel. A player can leave the game "
      "by pressing a designated key (useful when about to get killed or lost). The following "
      "Orbs work to bring such players back: ");

    help += XLATN(iinf[itOrbLife].name); help += ", ";
    help += XLATN(iinf[itOrbFriend].name); help += ", ";
    help += XLATN(iinf[itOrbUndeath].name); help += ", ";
    help += XLATN(iinf[itOrbTeleport].name); help += ", ";
    help += XLATN(iinf[itOrbSafety].name); help += "\n\n";      
    }

#if CAP_INV    
  if(inv::on && !game_keys_scroll)
  help += XLAT(
    inv::helptext
    );
  else if(!game_keys_scroll)
#endif
  help += XLAT(
    "There are many lands in HyperRogue. Collect 10 treasure "
    "in the given land type to complete it; this enables you to "
    "find the magical Orbs of this land, and in some cases "
    "get access to new lands. At 25 treasures "
    "this type of Orbs starts appearing in other lands as well. Press 'o' to "
    "get the details of all the Lands.\n\n");
  if(!game_keys_scroll) help += "\n\n";
    
#if ISMOBILE
  help += XLAT(
    "Usually, you move by touching somewhere on the map; you can also touch one "
    "of the four buttons on the map corners to change this (to scroll the map "
    "or get information about map objects). You can also touch the "
    "numbers displayed to get their meanings.\n"
    );
#else
  if(DEFAULTCONTROL && !game_keys_scroll)
    help += XLAT(
      "Move with mouse, num pad, qweadzxc, or hjklyubn. Wait by pressing 's' or '.'. Spin the world with arrows, PageUp/Down, and Space. "
      "To save the game you need an Orb of Safety. Press 'v' for the main menu (configuration, special modes, etc.), ESC for the quest status.\n\n"
      );
  else if(DEFAULTCONTROL && WDIM == 2)
    help += XLAT(
      "You are currently in a visualization. Press wasd to scroll, qe to rotate. You can also use the arrow keys. ESC for menu.\n\n");
  else if(DEFAULTCONTROL && WDIM == 3)
    help += XLAT(
      "You are currently in a visualization. Press wasdqe to rotate the camera, ijklyh to move. You can also use the arrow keys and Home/End and PgUp/PgDn. ESC for menu.\n\n");
  help += XLAT(
    "You can right click any element to get more information about it.\n\n"
    );
#if ISMAC
  help += XLAT("(You can also use right Shift)\n\n");
#endif
#endif
  help += XLAT("See more on the website: ") 
    + "https://roguetemple.com/z/hyper/\n\n";
  
#if CAP_TOUR
  if(!tour::on)
  help += XLAT("Try the Guided Tour to help with understanding the "
    "geometry of HyperRogue (menu -> special modes).\n\n");
#endif
  
  help += XLAT("Still confused? Read the FAQ on the HyperRogue website!\n\n");
  
  help_extensions.clear();
  
  help_extensions.push_back(help_extension{'c', XLAT("credits"), [] () { buildCredits(); }});
#if ISMOBILE == 0
  help_extensions.push_back(help_extension{'k', XLAT("advanced keyboard shortcuts"), [] () { 
    help = "";
    for(string s: normal_keys) help += s, help += "\n";
    for(string s: extra_keys) help += s, help += "\n";
    help += "\n\nQuick keys:\n";
    for(string s: quick_keys) help += s, help += "\n";
    if(GDIM == 3 || rug::rugged) {
      help += "\n\nIn 3D modes:\n";
      for(string s: extra_keys_3d) help += s, help += "\n";
      }
    else {
      help += "\n\nIn 2D modes:\n";
      for(string s: extra_keys_2d) help += s, help += "\n";
      }
    }});
#endif
  }

EX string standard_help() {
  return XLAT("Press F1 or right click for help");
  }

EX void buildCredits() {
  help = "";
  help += XLAT("game design, programming, texts and graphics by Zeno Rogue <zeno@attnam.com>\n\n");
  if(lang() != 0)
    help += XLAT("add credits for your translation here");
#if !NOLICENSE
  help += XLAT(
    "released under GNU General Public License version 2 and thus "
    "comes with absolutely no warranty; see COPYING for details\n\n"
    );
#endif
  help += XLAT(
    "special thanks to the following people for their bug reports, feature requests, porting, and other help:\n\n%1\n\n",
    "Konstantin Stupnik, ortoslon, chrysn, Adam Borowski, Damyan Ivanov, Ryan Farnsley, mcobit, Darren Grey, tricosahedron, Maciej Chojecki, Marek Čtrnáct, "
    "wonderfullizardofoz, Piotr Migdał, tehora, Michael Heerdegen, Sprite Guard, zelda0x181e, Vipul, snowyowl0, Patashu, phenomist, Alan Malloy, Tom Fryers, Sinquetica, _monad, CtrlAltDestroy, jruderman, "
    "Kojiguchi Kazuki, baconcow, Alan, SurelyYouJest, hotdogPi, DivisionByZero, xXxWeedGokuxXx, jpystynen, Dmitry Marakasov, Alexandre Moine, Arthur O'Dwyer, "
    "Triple_Agent_AAA, bluetailedgnat, Allalinor, Shitford, KittyTac, Christopher King, KosGD, TravelDemon, Bubbles, rdococ, frozenlake, MagmaMcFry, "
    "Snakebird Priestess, roaringdragon2, Stopping Dog, bengineer8, Sir Light IJIJ, ShadeBlade, Saplou, shnourok, Ralith, madasa, 6% remaining, Chimera245, Remik Pi, alien foxcat thing, "
    "Piotr Grochowski, Ann, still-flow, tyzone, Paradoxica, LottieRatWorld, aismallard, albatross, EncodedSpirit, Jacob Mandelson, CrashTuvai, cvoight"
    );
#ifdef EXTRALICENSE
  help += EXTRALICENSE;
#endif
#if !ISMOBILE
  help += XLAT(
    "\n\nSee sounds/credits.txt for credits for sound effects"
    );
  #endif
  if(musiclicense != "") help += musiclicense;
  }

string pushtext(stringpar p) {
  string s = XLAT(
    "\n\nNote: when pushing %the1 off a heptagonal cell, you can control the pushing direction "
    "by clicking left or right half of the heptagon.", p);
#if !ISMOBILE
  s += XLAT(" With the keyboard, you can rotate the view for a similar effect (Page Up/Down).");
#endif
  return s;
  }

string princedesc() {
  if(princessgender() == GEN_M)
    return XLAT("Apparently a prince is kept locked somewhere, but you won't ever find him in this hyperbolic palace. ");
  else
    return XLAT("Apparently a princess is kept locked somewhere, but you won't ever find her in this hyperbolic palace. ");
  }

EX string helptitle(string s, color_t col) {
  return "@" + its(col) + "\t" + s + "\n";
  }

string princessReviveHelp() {
  if(inv::on) return "";
  string h = "\n\n" +
    XLAT("Killed %1 can be revived with an Orb of Love, after you collect 20 more $$$.", moPrincess);
  if(princess::reviveAt)
    h += "\n\n" +
    XLAT("%The1 will be revivable at %2 $$$", moPrincess, its(princess::reviveAt));
  return h;
  }

void describeOrb(string& help, const orbinfo& oi) {
  if(inv::on) return;
  eOrbLandRelation olr = getOLR(oi.orb, getPrizeLand());
  eItem tr = treasureType(oi.l);
  eItem tt = treasureTypeUnlock(cwt.at->land, oi.orb);
  if(olr == olrGuest) {
    for(auto& oi1: orbinfos) 
      if((oi1.flags & orbgenflags::NATIVE) && oi1.orb == oi.orb)
        tr = treasureType(oi1.l);
    }
  help += "\n\n" + XLAT(olrDescriptions[olr], cwt.at->land, tr, tt);
  int t = items[tr] * landMultiplier(oi.l);
  if(t >= 25)
  if(olr == olrPrize25 || olr == olrPrize3 || olr == olrGuest || olr == olrMonster || olr == olrAlways) {
    for(auto& oi1: orbinfos) 
      if((oi1.flags & orbgenflags::NATIVE) && oi1.orb == oi.orb) {
        help += XLAT("\nSpawn rate (as prize Orb): %1%/%2\n", 
          its(int(.5 + 100 * orbprizefun(t))),
          its(oi1.gchance));
        }
    }
  if(t >= 10)
  if(olr == olrHub) {
    help += XLAT("\nSpawn rate (in Hubs): %1%/%2\n", 
      its(int(.5 + 100 * orbcrossfun(t))),
      its(oi.gchance));
    }
  }

string other_geometry() {
  return XLAT("Note: the rules above correspond to the standard geometry; actual rules in other geometries may be different. ");
  }

string other_land() {
  return XLAT("Note: the rules refer to colors which are not visible in other lands. ");
  }

string other_geometry_land() {
  if(S7 != 7 || !BITRUNCATED) return other_geometry();
  else return other_land();
  }

string forbidden_marked() {
  return XLAT("When the 'mark heptagons' option (hotkey '7') is on, forbidden moves are marked.");
  }

string forbidden_unmarked() {
  return XLAT("When the 'mark heptagons' option (hotkey '7') is on, moves between unmarked cells are forbidden.");
  }

string hyperstone_optional = "Completing the quest in this land is not necessary for the Hyperstone Quest.";

string power_help = 
  "The amount of Orbs obtained by using Orbs of Mirroring is "
  "multiplied by sqrt(1+p/20), where p is the number of Powerstones "
  "collected. This also affects the mirrorings which happened before "
  "collecting the Powerstones.";

EX string generateHelpForItem(eItem it) {

   string help = helptitle(XLATN(iinf[it].name), iinf[it].color);

   if(shmup::on && isShmupLifeOrb(it)) {
     int cnt = 0;
     help += XLAT(
       "The following Orbs act an extra lives in the shmup mode:");
     for(int i=0; i<ittypes; i++) {
       eItem it2 = eItem(i);
       if(isShmupLifeOrb(it2)) help += cnt++ ? XLAT(", %1", it2) : XLAT(" %1", it2);
       }
     }
   
   else
   #if CAP_CRYSTAL
   if(it == itCompass && cryst)
     help += crystal::compass_help();
   else
   #endif
     help += XLAT(iinf[it].help);
   
   if(it == itSavedPrincess || it == itOrbLove) if(!inv::on)
     help += princessReviveHelp();
     
   if(it == itTrollEgg)
     help += XLAT("\n\nAfter the Trolls leave, you have 750 turns to collect %the1, or it gets stolen.", it);
   
   if(it == itIvory || it == itAmethyst || it == itLotus || it == itMutant) {
     help += XLAT(
       "\n\nEasy %1 might disappear when you collect more of its kind.", it);
     if(it != itMutant) help += XLAT(
       " You need to go deep to collect lots of them.");
     }
     
#if ISMOBILE
   if(it == itOrbSafety)
     help += XLAT("This might be very useful for devices with limited memory.");
#else
   if(it == itOrbSafety)
     help += XLAT("Thus, it is potentially useful for extremely long games, which would eat all the memory on your system otherwise.\n");
#endif

   if(isRangedOrb(it)) {
     help += XLAT("\nThis is a ranged Orb. ");
#if ISMOBILE   
     if(vid.shifttarget&2)
       help += XLAT("\nRanged Orbs can be targeted by long touching the desired location.");
     else
       help += XLAT("\nRanged Orbs can be targeted by touching the desired location.");
#else
     if(vid.shifttarget&1)
       help += XLAT("\nRanged Orbs can be targeted by shift-clicking the desired location. ");
     else
       help += XLAT("\nRanged Orbs can be targeted by clicking the desired location. ");
     help += XLAT("You can also scroll to the desired location and then press 't'.");
#endif
     help += XLAT("\nYou can never target cells which are adjacent to the player character, or ones out of the sight range.");
     }

#if ISMOBILE
   if(it == itGreenStone)
     help += XLAT("You can touch the Dead Orb in your inventory to drop it.");
#else
   if(it == itGreenStone)
     help += XLAT("You can press 'g' or click them in the list to drop a Dead Orb.");
#endif
   if(it == itOrbLightning || it == itOrbFlash)
     help += XLAT("\n\nThis Orb is triggered on your first attack or illegal move.");
   if(it == itOrbShield)
     help += XLAT("\n\nThis Orb protects you from attacks, scents, and insulates you "
       "from electricity. It does not let you go through deadly terrain, but "
       "if you are attacked with fire, it lets you stay in place in it.");

  if(it == itOrbWinter) 
    help += XLAT("\n\nThis orb also allows you to collect items encased in ice.");
  
  if(it == itOrbIntensity && inv::on)
    help += XLAT("\n\nIn the Orb Strategy Mode, the effect is increased to +100%.");

  if(it == itOrbEmpathy) {
    int cnt = 0;
    for(int i=0; i<ittypes; i++) {
      eItem it2 = eItem(i);
      if(isEmpathyOrb(it2)) {
        help += cnt ? XLAT(", %1", it2) : XLAT(" %1", it2);
        cnt++;
        }
      }
    help += XLAT("\n\nAdditionally, your allies are protected from your indirect attacks.");
    }

#if CAP_INV  
  if(inv::on) {
    if(it == itOrbYendor || it == itHell) {
      help += XLAT(
        "\n\nIn the Orb Strategy Mode, Orbs of Yendor appear in Hell after "
        "you collect 25 Demon Daisies in Hell, in Crossroads/Ocean after you collect 50, "
        "and everywhere after you collect 100.");  
      }
    
/*    if(it == itBone || it == itGreenStone) {
      help += XLAT(
        "\n\nIn the Orb Strategy Mode, dead orbs are available once you collect "
        "10 Necromancer Totems in the Graveyard."
        );
      } */
    
    if(it == itFeather || it == itOrbSafety) {
      help += XLAT(
        "\n\nIn the Orb Strategy Mode, Orbs of Safety can be gained by "
        "collecting Phoenix Feathers in the Land of Eternal Motion. "
        "You can also find unlimited Orbs of Safety in the Crossroads "
        "and the Ocean (after collecting 25 Phoenix Feathers) "
        "and in the Prairie."
        );
      }
  
    if(it == itOrbYendor || it == itHolyGrail)
      help += XLAT(
        "\n\nCollect %the1 to gain an extra Orb of the Mirror. "
        "You can gain further Orbs of the Mirror by collecting 2, 4, 8...",
        it
        );  
    
    if(it == itPower)
      help += "\n\n" + XLAT(power_help);
    
    if(it == itOrbLuck)
      help += XLAT(
        "\n\nIn the Orb Strategy Mode, the Orb of Luck also "
        "significantly increases the frequency of Great Walls, Crossroads IV, "
        "and sub-lands."
        );

    if(it == itBone)
      help += XLAT(
        "\n\nIn the Orb Strategy Mode, each 25 Necromancer's Totems "
        "you are given a random offensive Orb."
        );
    
    if(inv::remaining[it] || inv::usedup[it]) help += "\n\n" + inv::osminfo(it);
    inv::whichorbinfo = it;  
    inv::compute();
    if(inv::orbinfoline != "") help += "\n\n" + inv::orbinfoline;
    if(inv::extra != "") help += "\n\nExtras:" + inv::extra;
    }
#endif
  
  if(it == itOrbLuck) {
    help += XLAT("\n\nAdditionally, the probabilities of generating terrain features are subtly changed in the following lands:");
    
    int cnt = 0;
    for(int i=0; i<landtypes; i++) {
      eLand land = eLand(i);
      if(isLuckyLand(land)) {
        help += cnt ? XLAT(", %1", land) : XLAT(" %1", land);
        cnt++;
        }
      }
    }

  if(itemclass(it) == IC_ORB || it == itGreenStone || it == itOrbYendor) {
    for(auto& oi: orbinfos) {
      if(oi.orb == it && oi.is_native()) describeOrb(help, oi);
      }
    }
  
  if(itemclass(it) == IC_TREASURE) {
    for(auto& oi: orbinfos) {
      if(treasureType(oi.l) == it) {
        if(oi.gchance > 0) {
          help += "\n\n";
          help += XLAT("\n\nOrb unlocked: %1", oi.orb);
          describeOrb(help, oi);
          }
        else if(oi.l == cwt.at->land || inv::on) {
          help += "\n\n";
          help += XLAT("Secondary orb: %1", oi.orb);
          describeOrb(help, oi);
          }
        }
      }
    }
  
  if(it == itOrb37 && (S7 != 7 || !BITRUNCATED))
    help += "\n\n" + other_geometry() + forbidden_unmarked();

  if(it == itOrbLava && (S7 != 7 || !BITRUNCATED))
    help += "\n\n" + other_geometry() + forbidden_unmarked();
  
  if(among(it, itOrbSide2, itOrbSide3) && !among(S7, 6, 7))
    help += "\n\n" + other_geometry() + XLAT("This orb lets you attack adjacent cells %1 steps from the primary target.", its(it - itOrbSide1 + 1));

#if CAP_INV
  if(inv::on && it == itInventory)
    help += "\n\n" + XLAT(inv::helptext);
#endif

  if(in_full_game() && !required_for_hyperstones(it) && it != itHyperstone)
    help += "\n\n" + XLAT(hyperstone_optional);
    
#if CAP_DAILY
  if(daily::on && it == itOrbLove)
    help += "\n\n" + XLAT("The Orb of Love gives no bonus score in the Strange Challenge.");
#endif

  return help;
  }

void addMinefieldExplanation(string& s) {

  s += XLAT(
    "\n\nOnce you collect a Bomberbird Egg, "
    "stepping on a cell with no adjacent mines also reveals the adjacent cells. "
    "Collecting even more Eggs will increase the radius."
    );

  s += "\n\n";
#if !ISMOBILE
  s += XLAT("Known mines may be marked by pressing 'm'. Your allies won't step on marked mines.");
#else
  s += XLAT("Known mines may be marked by touching while in drag mode. Your allies won't step on marked mines.");
#endif

  help_extensions.push_back(help_extension{'n', XLAT("toggle numerical display"), [] () { numerical_minefield = !numerical_minefield; }});
  }

EX string generateHelpForWall(eWall w) {

  string s = helptitle(XLATN(winf[w].name), winf[w].color);
   
  s += XLAT(winf[w].help);
  if(w == waMineMine || w == waMineUnknown || w == waMineOpen)
    addMinefieldExplanation(s);
  if(isThumper(w)) s += pushtext(w);
  if((w == waClosePlate || w == waOpenPlate) && PURE) 
    s += "\n\n(For the heptagonal mode, the radius has been reduced to 2 for closing plates.)";
  return s;
  }

void buteol(string& s, int current, int req) {
  int siz = isize(s);
  if(s[siz-1] == '\n') s.resize(siz-1);
  char buf[100]; sprintf(buf, " (%d/%d)", current, req);
  s += buf; s += "\n";
  }

EX string generateHelpForMonster(eMonster m) {
  string s = helptitle(XLATN(minf[m].name), minf[m].color);
  
  if(m == moPlayer) {
#if CAP_TOUR
    if(tour::on || peace::on)
      return s+XLAT(
        "A tourist from another world. They mutter something about the 'tutorial', "
        "and claim that they are here just to learn, and to leave without any treasures. "
        "Do not kill them!"
        );
#endif

    s += XLAT(
        "This monster has come from another world, presumably to steal our treasures. "
        "Not as fast as an Eagle, not as resilient as the guards from the Palace, "
        "and not as huge as the Mutant Ivy from the Clearing; however, "
        "they are very dangerous because of their intelligence, "
        "and access to magical powers.\n\n");
    
    if(cheater)
      s += XLAT("Actually, their powers appear god-like...\n\n");
    
    else if(!hardcore)
      s += XLAT(
       "Rogues will never make moves which result in their immediate death. "
       "Even when cornered, they are able to instantly teleport back to their "
       "home world at any moment, taking the treasures forever... but "
       "at least they will not steal anything further!\n\n"
       );
    
    if(!euclid)
      s += XLAT(
        "Despite this intelligence, Rogues appear extremely surprised "
        "by the most basic facts about geometry. They must come from "
        "some really strange world.\n\n"
        );
    
    if(shmup::on)
      s += XLAT("In the Shoot'em Up mode, you are armed with thrown Knives.");
    
    return s;
    }

  s += XLAT(minf[m].help);      
  if(m == moPalace || m == moSkeleton)
    s += pushtext(m);  
  if(m == moTroll) s += XLAT(trollhelp2);  

  if(isMonsterPart(m))
    s += XLAT("\n\nThis is a part of a monster. It does not count for your total kills.", m);

  if(isFriendly(m))
    s += XLAT("\n\nThis is a friendly being. It does not count for your total kills.", m);

  if(m == moTortoise)
    s += XLAT("\n\nTortoises are not monsters! They are just annoyed. They do not count for your total kills.", m);
  
  if(isGhost(m))
    s += XLAT("\n\nA Ghost never moves to a cell which is adjacent to another Ghost of the same kind.", m);
    
  if(m == moMutant) {
    using namespace clearing;
    if(direct)
      s += XLAT("\n\nLeaves cut directly: %1", its(direct));
    if(kills[moMutant])
      s += XLAT("\n\nLeaves cut onscreen: %1", its(kills[moMutant]));
    if(imputed.nonzero())
      s += XLAT("\n\nLeaves cut offscreen (approximately): %1", imputed.get_str(10000));
    }
  
  eItem it = frog_power(m);
  if(it)
    s += XLAT("\n\nThis Frog uses the power of %the1. You get 5 charges yourself for killing it.", it);
    
  if(m == moBat || m == moEagle)
    s += XLAT("\n\nFast flying creatures may attack or go against gravity only in their first move.", m);
  
  if(m == moAltDemon)
    s += "\n\n" + other_geometry_land() + forbidden_unmarked();

  if(among(m, moHexDemon, moHexSnake, moHexSnakeTail))
    s += "\n\n" + other_geometry_land() + forbidden_marked();

  if(among(m, moKrakenT, moKrakenH) && (S7 != 7 || !BITRUNCATED))
    s += "\n\n" + other_geometry() + XLAT("Forbidden cells are marked with a different color.");

  return s;
  }

void add_reqs(eLand l, string& s) {
  back:
  
  switch(l) {
    #define LAND(a,b,c,d,e,f,g) case c:
    #define REQ(x) x return;
    #define REQAS(x,y) y l = x; goto back;
    #define GOLD(x) NUMBER(gold(), x, XLAT("Treasure required: %1 $$$.\n", its(x)))
    #define KILL(who, where) NUMBER(kills[who], 1, XLAT("Kills required: %1 (%2).\n", who, where))
    #define ITEMS(kind, number) NUMBER(items[kind], number, XLAT("Treasure required: %1 x %2.\n", its(number), kind))
    #define NEVER ;
    #define ALWAYS s += XLAT("Always available.\n");
    #define KILLS(x) NUMBER(tkills(), x, XLAT("Kills required: %1.\n", its(x)))
    #define AKILL(who, where) s += XLAT("Alternatively: kill a %1 in %the2.\n", who, where); buteol(s, kills[who], 1);
    #define ORD(a, b) b a
    #define ALT(x) // if([&] { x return true; } ()) return true;
    #define NUMBER(val, required, description) s += description; buteol(s, val, required);
    #define COND(x,y) s += (y);
    #define ITEMS_TOTAL(list, z) \
      { int now = 0; string t = "("; for(eItem i: list) { if(t!="(") t += " | "; t += XLATN(iinf[i].name); now += items[i]; } t += ")"; s += XLAT("Treasure required: %1 x %2.\n", its(z), t); buteol(s, now, z); }
    #define ACCONLY(z) s += XLAT("Accessible only from %the1.\n", z);
    #define ACCONLY2(z,x) s += XLAT("Accessible only from %the1 or %the2.\n", z, x);
    #define ACCONLY3(z,y,x) s += XLAT("Accessible only from %the1, %2, or %3.\n", z, y, x);
    #define ACCONLYF(z) s += XLAT("Accessible only from %the1 (until finished).\n", z);
    #include "content.cpp"

    case landtypes: return;
    }

  }

EX string generateHelpForLand(eLand l) {
  string s = helptitle(XLATN(linf[l].name), linf[l].color);
  
  if(l == laPalace) s += princedesc();

  s += XLAT(linf[l].help);

  if(l == laMinefield) addMinefieldExplanation(s);

  s += "\n\n";
  
  add_reqs(l, s);

  if(l == laPower && inv::on)
    help += XLAT(power_help) + "\n\n";
  
  if(isCoastal(l)) 
    s += XLAT("Coastal region -- connects inland and aquatic regions.\n");
    
  if(isPureSealand(l)) 
    s += XLAT("Aquatic region -- accessible only from coastal regions and other aquatic regions.\n");
    
  if(in_full_game() && !required_for_hyperstones(treasureType(l)) && !isCrossroads(l))
    s += XLAT(hyperstone_optional);

  int rl = isRandland(l);
  if(rl == 2)
    s += XLAT("Variants of %the1 are always available in the Random Pattern Mode.", l);
  else if(rl == 1)
    s += XLAT(
      "Variants of %the1 are available in the Random Pattern Mode after "
      "getting a highscore of at least 10 %2.", l, treasureType(l));

  if(l == laPrincessQuest) {
    s += XLAT("Unavailable in the shmup mode.\n");
    s += XLAT("Unavailable in the multiplayer mode.\n");
    }
  
  /* if(noChaos(l))
    s += XLAT("Unavailable in the Chaos mode.\n"); */
  
  if(l == laWildWest)
    s += XLAT("Bonus land, available only in some special modes.\n");
  
  if(l == laWhirlpool)
    s += XLAT("Orbs of Safety always appear here, and may be used to escape.\n");

  /* if(isHaunted(l) || l == laDungeon)
    s += XLAT("You may be unable to leave %the1 if you are not careful!\n", l); */
    
  if(l == laStorms) {
    if(elec::lightningfast == 0) s += XLAT("\nSpecial conduct (still valid)\n");
    else s += XLAT("\nSpecial conduct failed:\n");
    
    s += XLAT(
      "Avoid escaping from a discharge (\"That was close\").");
    }

  if(isHaunted(l)) {
    if(survivalist) s += XLAT("\nSpecial conduct (still valid)\n");
    else s += XLAT("\nSpecial conduct failed:\n");

    s += XLAT(
      "Avoid chopping trees, using Orbs, and non-graveyard monsters in the Haunted Woods."
      );
    }

  #if CAP_CRYSTAL
  if(l == laCamelot && cryst) {
    if(!crystal::used_compass_inside) s += XLAT("\nSpecial conduct (still valid)\n");
    else s += XLAT("\nSpecial conduct failed:\n");
    
    s += XLAT(
      "Do not use compases.\n\n");

    s += XLAT("Crystal Camelot is an octahedron in 'pure' 3D crystal geometry (and a similar polytope in other pure crystals), "
      "and an Euclidean ball in bitruncated/Goldberg crystals.");
    }
  #endif

  auto lv = land_validity(l);
  if(lv.flags & lv::display_in_help)
    s += "\n\n" + XLAT(lv.msg);
  
#if !ISMOBILE
  if(l == laCA) {
    s += XLAT("\n\nOption -mineadj 1 can be added for Moore neighborhoods.");
    s += XLAT("\n\nHint: use 'm' to toggle cells quickly");
    }  
#endif

  return s;
  }

EX bool instat;

string turnstring(int i) {
  if(i == 1) return XLAT("1 turn");
  else return XLAT("%1 turns", its(i));
  }

reaction_t helpgenerator;
EX string bygen(reaction_t h) {
  helpgenerator = h;
  return "HELPGEN";
  }

void gotoHelpFor(eLand l);

void gotoHelpFor(eItem i) {
  help = generateHelpForItem(i);
  }

void gotoHelpFor(eWall w) {
  help = generateHelpForWall(w);
  }

void gotoHelpFor(eMonster m) {
  help = generateHelpForMonster(m);
  }

EX void appendHelp(string s) {
  auto h = helpgenerator;
  if(help == "HELPGEN") 
    bygen([h,s] { h(); help += s; });
  else
    help += s;
  }

unsigned char lastval;
int windtotal;

EX hookset<void(cell*)> hooks_mouseover;

template<class T> void set_help_to(T t) { 
  help = bygen([t] {
    gotoHelpFor(t);
    help_extensions.push_back(help_extension{'h', XLAT("HyperRogue help"), [] () { buildHelpText(); }});
    });
  }

EX void describeMouseover() {
  DEBBI(DF_GRAPH, ("describeMouseover"));

  cell *c = mousing ? mouseover : playermoved ? NULL : centerover;
  string& out = mouseovers;
  if(!c || instat || getcstat != '-') { }
  else if(c->wall != waInvisibleFloor) {
    out = XLAT1(linf[c->land].name);
    set_help_to(c->land);
    
    if(WDIM == 3 && isGravityLand(c->land)) out += " [" + its(gravityLevel(c)) + "]";
    
    if(c->land == laTemple && c->master->alt) {
      int lev = -celldistAlt(c);
      int ts = temple_layer_size();
      out += " (" + its(lev/ts+1) + ":" + its(lev%ts) + ")";
      }
    if(isIcyLand(c)) 
      out += " (" + fts(heat::celsius(c)) + " °C)";
    if(c->land == laBrownian && c->wall == waNone)
      out += XLAT(" (level %1)", its(snakelevel(c)));
    if(c->land == laDryForest && c->landparam) 
      out += " (" + its(c->landparam)+"/10)";
    if(c->land == laOcean && ls::any_chaos())
      out += " (" + its(c->CHAOSPARAM)+"S"+its(c->SEADIST)+"L"+its(c->LANDDIST)+")";
    else if(c->land == laOcean && c->landparam <= 25) {
      if(shmup::on)
        out += " (" + its(c->landparam)+")";
      else {
        bool b = c->landparam >= tide[(turncount-1) % tidalsize];
        int t = 1;
        for(; t < 1000 && b == (c->landparam >= tide[(turncount+t-1) % tidalsize]); t++) ;
        if(b)
          out += " (" + turnstring(t) + XLAT(" to surface") + ")";
        else 
          out += " (" + turnstring(t) + XLAT(" to submerge") + ")";
        }
      }
    #if CAP_FIELD
    else if(c->land == laVolcano) {
      int id = lavatide(c, -1)/4;
      if(id < 96/4)
        out += " (" + turnstring(96/4-id) + XLAT(" to go cold") + ")";
      else
        out += " (" + turnstring(256/4-id) + XLAT(" to submerge") + ")";
      }
    else if(c->land == laBlizzard) {
      int wm = windmap::at(c);
      windtotal += (signed char) (wm-lastval);
      lastval = wm;
      if(c == cwt.at) windtotal = 0;
      out += " [" + its(windtotal) + "]";
      }
    #endif

    if(c->land == laTortoise && tortoise::seek()) out += " " + tortoise::measure(getBits(c));

    // describe the shadow path
    if(among(c->land, laGraveyard, laCursed) && shpos.size()) {
      string shadowtimes;
      vector<cell*> route;
      for(int s=1; s<SHSIZE; s++) {
        bool shadow = false;
        for(int p: player_indices())
          if(shpos[(cshpos+s)%SHSIZE][p] == c) 
            shadow = true;
        if(shadow) {
          if(shadowtimes == "")
            shadowtimes = its(s);
          else
            shadowtimes += " " + its(s);
          }
        }
      if(shadowtimes != "")
        out += XLAT(" (shadow in %1)", shadowtimes);
      }

    if(buggyGeneration) {
      char buf[80]; sprintf(buf, " %p H=%d M=%d", hr::voidp(c), c->landparam, c->mpdist); out += buf;
      }
    
    if(randomPatternsMode)
      out += " " + describeRPM(c->land);
      
    if(cheater && euc::in(2)) {
      auto co = euc2_coordinates(c);
      out += " (" + its(co.first);
      for(int i=1; i<WDIM; i++) out += "," + its(co.second);
      out += ")";
      }

    if(cheater && euc::in(3) && !(cgflags & qPORTALSPACE)) {
      auto co = euc::get_ispacemap()[c->master];
      out += " (" + its(co[0]);
      for(int i=1; i<WDIM; i++) out += "," + its(co[i]);
      out += ")";
      }
    
    #if CAP_CRYSTAL
    if(geometry == gCrystal344 && cheater && crystal::view_coordinates) {
      out += " (";
      auto co = crystal::get_coord(c->master);
      for(int i=0; i<4; i++) {
        if(i) out += ",";
        out += its(co[i]);
        }
      out += ")";
      }
    #endif
      
    if(c->wall && !(c->wall == waChasm && c->land == laDual && ctof(c)) &&
      !(c->land == laMemory) &&
      !((c->wall == waFloorA || c->wall == waFloorB) && c->item)) { 
      out += ", "; out += XLAT1(winf[c->wall].name); 
      
      if(c->wall == waRose) out += " (" + its(7-rosephase) + ")";
      if(c->wall == waTerraWarrior) out += " (" + its(c->wparam) + ")";
      #if CAP_COMPLEX2
      if(isDie(c->wall)) out += " (" + dice::describe(c) + ")";
      #endif
      
      if((c->wall == waBigTree || c->wall == waSmallTree) && c->land != laDryForest)
        help = 
          "Trees in this forest can be chopped down. Big trees take two turns to chop down.";
      else 
        if(c->wall != waSea && c->wall != waPalace && c->wall != waDeadfloor)
        if(!((c->wall == waCavefloor || c->wall == waCavewall) && (c->land == laEmerald || c->land == laCaves)))
        if(!((isAlch(c->wall) && c->land == laAlchemist)))
          set_help_to(c->wall);
      }
    
    if(isActivable(c)) out += XLAT(" (touch to activate)");
    
    if(hasTimeout(c)) out += " [" + turnstring(c->wparam) + "]";
    
    if(isReptile(c->wall))
      out += " [" + turnstring((unsigned char) c->wparam) + "]";
      
    #if CAP_COMPLEX2
    if(c->monst == moKnight)
      out += XLAT(", %1 the Knight", camelot::knight_name(c));
    #else
    if(0) ;
    #endif
  
    else if(c->monst) {
      out += ", "; out += XLAT1(minf[c->monst].name); 
      #if CAP_COMPLEX2
      if(isDie(c->monst))
        out += " (" + dice::describe(c) + ")";
      #endif
      if(hasHitpoints(c->monst))
        out += " (" + its(c->hitpoints)+" HP)";
      if(isMutantIvy(c))
        out += " (" + its((c->stuntime - mutantphase) & 15) + "*)";
      else if(c->stuntime)
        out += " (" + its(c->stuntime) + "*)";

      if(c->monst == moTortoise && tortoise::seek()) 
        out += " " + tortoise::measure(tortoise::getb(c));
        
      set_help_to(c->monst);
      }
  
    if(c->item && !itemHiddenFromSight(c)) {
      out += ", "; 
      out += XLAT1(iinf[c->item].name); 
      if(c->item == itBarrow) out += " (x" + its(c->landparam) + ")";
      #if CAP_COMPLEX2
      if(c->land == laHunting) {      
        int i = ambush::size(c, c->item);
        if(i) out += " (" + XLAT("ambush:") + " " + its(i) + ")";
        }
      #endif
      if(c->item == itBabyTortoise && tortoise::seek()) 
        out += " " + tortoise::measure(tortoise::babymap[c]);
      if(!c->monst) set_help_to(c->item);
      }
    
    if(isPlayerOn(c) && !shmup::on) out += XLAT(", you"), help = generateHelpForMonster(moPlayer);

    shmup::addShmupHelp(out);

    if(rosedist(c) == 1)
      out += ", wave of scent (front)";

    if(rosedist(c) == 2)
      out += ", wave of scent (back)";
    
    if(sword::at(c)) out += ", Energy Sword";
    
    if(rosedist(c) || c->land == laRose || c->wall == waRose)
      appendHelp(string("\n\n") + rosedesc);
    
    if(isWarped(c) && !isWarpedType(c->land))
      out += ", warped";

    if(isWarped(c)) {
      appendHelp(string("\n\n") + XLAT(warpdesc));

      if(S7 != 7 || !BITRUNCATED) if(c->item != itOrb37)
        appendHelp("\n\n" + other_geometry() + forbidden_unmarked());
      }
    
    if(isElectricLand(c) || isElectricLand(cwt.at->land)) {
      using namespace elec;
      eCharge ch = getCharge(c);
      if(ch == ecCharged) appendHelp("\n\nThis cell is charged.");
      if(ch == ecGrounded) appendHelp("\n\nThis cell is grounded.");
      if(ch == ecConductor) appendHelp("\n\nThis cell is currently conductive.");
      if(ch == ecIsolator) appendHelp("\n\nThis cell is currently not conductive.");
      }
    }
  else {
    shmup::addShmupHelp(out);
    }
    
  callhooks(hooks_mouseover, c);
  
  if(mousey < vid.fsize * 3/2 && getcstat == '-' && !instat) getcstat = SDLK_F1;
  #if CAP_TOUR
  if(tour::on && !tour::texts) {
    if(tour::slides[tour::currentslide].flags & tour::NOTITLE)
      mouseovers = "";
    else
      mouseovers = XLAT(tour::slides[tour::currentslide].name);
    }
  #endif
  }

EX void showHelp() {
  cmode = sm::HELP | sm::DOTOUR | sm::DARKEN;
  getcstat = SDLK_ESCAPE;
  if(help == "HELPFUN") {
    help_delegate();
    return;
    }

  gamescreen();
  string help2;
  if(help[0] == '@') {
    int iv = help.find("\t");
    int id = help.find("\n");
    dialog::init(help.substr(iv+1, id-iv-1), atoi(help.c_str()+1), 120, 100);
    dialog::addHelp(help.substr(id+1));
    }
  else {
    dialog::init("help", forecolor, 120, 100);
    dialog::addHelp(help);
    }
  
  bool in_list = false;

  for(auto& he: help_extensions) {
    if(!in_list && he.key == dialog::first_list_fake_key) {
      dialog::start_list(1000, 1000, 'a');
      in_list = true;
      }
    if(in_list && (he.key < dialog::first_list_fake_key || he.key > dialog::first_list_fake_key + 1000)) {
      in_list = false;
      dialog::end_list();
      }

    if(he.subtext != "")
      dialog::addSelItem(he.text, he.subtext, he.key);
    else
      dialog::addItem(he.text, he.key);
    dialog::add_action(he.action);
    dialog::lastItem().color = he.color;
    }
  if(in_list) dialog::end_list();
  
  dialog::display();
  
  keyhandler = [] (int sym, int uni) {
    dialog::handleNavigation(sym, uni);
    
    if(sym == SDLK_F1)  {
      auto i = help;
      buildHelpText();
      if(help == i) popScreen();
      }
    else if(doexiton(sym, uni))
      popScreen();
    };
  }

EX hookset<bool()> hooks_default_help;

EX void gotoHelp(const string& h) {
  help = h;
  help_extensions.clear();
  pushScreen(showHelp);
  if(help == "@") {

    if(callhandlers(false, hooks_default_help)) return;
    
#if CAP_RUG
    if(rug::rugged) {
      help = rug::makehelp();
      
      help += "\n\n";

      for(string s: extra_keys_3d) help += s, help += "\n";

      help += "\n\n";

      help_extensions.push_back(help_extension{'m', XLAT("Hypersian Rug menu"), [] () { popScreen(); rug::select(); }});    
      help_extensions.push_back(help_extension{'h', XLAT("HyperRogue help"), [] () { buildHelpText(); }});    
      return;
      }
#endif
  
    buildHelpText();
    }
  if(help == "HELPGEN") helpgenerator();
  }

EX void subhelp(const string& h) {
  string oldhelp = help;
  auto ext = help_extensions;
  reaction_t back = [oldhelp, ext] () {
    help = oldhelp;
    help_extensions = ext;
    };
  help = h;
  help_extensions.clear();
  if(help == "HELPGEN") helpgenerator();
  help_extensions.push_back(help_extension{'z', XLAT("back"), back});
  }

EX void gotoHelpFor(eLand l) {
  help = generateHelpForLand(l);
  
  int beastcount = 0;
  for(int m0=0; m0<motypes; m0++)
    if(isNative(l, eMonster(m0)) && !nodisplay(eMonster(m0))) beastcount++;
  
  auto listbeasts = [l] () {
    char nextmonster = 'a';
    for(int m0=0; m0<motypes; m0++) {
      const eMonster m = eMonster(m0);
      if(isNative(l, m) && !nodisplay(m)) {
        help_extension hex;
        hex.key = nextmonster++;
        hex.text = XLATN(minf[m].name);
        hex.action = [m] () {
          subhelp(bygen([m] () { gotoHelpFor(m); }));
          };
        help_extensions.push_back(hex);
        }
      }
    };
  
  if(beastcount > 3)       
  help_extensions.push_back(help_extension{'b', XLAT("bestiary of %the1", l), [l, listbeasts] () { 
    subhelp(helptitle(XLAT("bestiary of %the1", l), 0xC00000));
    listbeasts();
     }});
  else listbeasts();  
  
  if(l == laTortoise)
    help_extensions.push_back(help_extension{'s', XLAT("Galápagos shading"), [] () {
      tortoise::shading_enabled = !tortoise::shading_enabled;
      }});

  help_extensions.push_back(help_extension{'w', XLAT("wiki"), [l] () {
    open_wiki(linf[l].name);
    }});
  }
}
