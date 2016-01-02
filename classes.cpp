
// --- help ---

const char *wormdes = 
  "These huge monsters normally live below the sand, but your movements have "
  "disturbed them. They are too big to be slain with your "
  "weapons, but you can defeat them by making them unable to move. "
  "This also produces some Spice. They move two times slower than you.";

const char *cocytushelp =
   "This frozen lake is a hellish version of the Icy Land. Now, your body heat melts the floor, not the walls.";

const char *tentdes = 
  "The tentacles of Cthulhu are like sandworms, but longer. "
  "They also withdraw one cell at a time, instead of exploding instantly.";

const char *gameboardhelp = 
  "Ever wondered how some boardgame would look on the hyperbolic plane? "
  "I wondered about Go, so I have created this feature. Now you can try yourself!\n"
  "Enter = pick up an item (and score), space = clear an item\n"
  "Other keys place orbs and terrain features of various kinds\n"
  "In the periodic editor, press 0-4 to switch walls in different ways\n"
  ;

const char *ivydes = 
  "A huge plant growing in the Jungle. Each Ivy has many branches, "
  "and one branch grows per each of your moves. Branches grow in a clockwise "
  "order. The root itself is vulnerable.";

const char *slimehelp =
  "The Alchemists produce magical potions from pools of blue and red slime. You "
  "can go through these pools, but you cannot move from a blue pool to a red "
  "pool, or vice versa. Pools containing items count as colorless, and "
  "they change color to the PC's previous color when the item is picked up. "
  "Slime beasts also have to keep to their own color, "
  "but when they are killed, they explode, destroying items and changing "
  "the color of the slime and slime beasts around them.";

const char *gdemonhelp =
    "These creatures are slow, but very powerful... more powerful than you. "
    "You need some more experience in demon fighting before you will be able to defeat them. "
    "Even then, you will be able to slay this one, but more powerful demons will come...\n\n"
    "Each 10 lesser demons you kill, you become powerful enough to kill all the greater "
    "demons on the screen, effectively turning them into lesser demons.";    

const char *ldemonhelp =
    "These creatures are slow, but they often appear in large numbers.";

const char *trollhelp =
  "A big monster from the Living Caves. A dead Troll will be reunited "
  "with the rocks, causing some walls to grow around its body.";

const char *trollhelp2 =
  " Additionally, all items around the killed Troll will be destroyed.";

const char *camelothelp = 
  "The Knights of the Round Table are the greatest warriors of these lands. "
  "They are not very inventive with names though, as they call each of their "
  "castles Camelot. "
  "You are probably worth of joining them, but they will surely give you "
  "some quest to prove yourself...\n\n"
  "Each castle contains a single treasure, the Holy Grail, in the center. "
  "The radius of the Round Table is usually 28, but after you find a Holy Grail "
  "successfully, each new castle (and each Round Table) you find will be bigger.";

const char *templehelp = 
  "The temple of Cthulhu consists of many concentric circles of columns. "
  "You will surely encounter many Cultists there, who believe that a pilgrimage "
  "to the inner circles will bring them closer to Cthulhu himself, and Grimoires "
  "which surely contain many interesting secrets.\n\n"
  "The circles in the temple of Cthulhu are actually horocycles. They are "
  "infinite, and there is an infinite number of them.";

const char *barrierhelp = 
  "Huge, impassable walls which separate various lands.";

const char *cavehelp =
  "This cave contains walls which are somehow living. After each turn, each cell "
  "counts the number of living wall and living floor cells around it, and if it is "
  "currently of a different type than the majority of cells around it, it switches. "
  "Items count as three floor cells, and dead Trolls count as five wall cells. "
  "Some foreign monsters also count as floor or wall cells.\n";

const char *vinehelp = 
  "The Vineyard is filled with vines. A very dense pattern of straight lines here...\n\n" 
  "Vine Beasts and Vine Spirits change vine cells to grass, and vice versa.";

const char *hvinehelp = 
  "A vine is growing here... but only on a half of the cell. How is that even possible?!"
  "Most monsters cannot move from this cell to the cell containing the other half. "
  "Vine spirits can move only to the adjacent cells which are also adjacent to the "
  "other half.";

const char *deadcavehelp = 
  "Somehow, this cave has not received the spark of Life yet.";

const char *foresthelp =
  "This forest is quite dry. Beware the bushfires!\n"
  "Trees catch fire on the next turn. The temperature of the grass cells "
  "rises once per turn for each fire nearby, and becomes fire itself "
  "when its temperature has risen 10 times.\n"
  "You can also cut down the trees. Big trees take two turns to cut down.";

const char *hivehelp =
  "The Hive is filled with Hyperbugs. They are huge insects which look a bit like "
  "ants, a bit like bees, and a bit like roaches. "
  "They live in circular nests, and an army of Hyperbugs will attack any intruder, "
  "including you and enemy Hyperbugs. Will you manage to get to the "
  "heart of such a nest, and get the precious Royal Jelly?";

const char *redrockhelp = 
  "This land contains high rock formations. Most of the valley is at level 0, "
  "while items are found at level 3. It is impossible to gain two or more levels, "
  "or to lose three levels, in a single move, (attacks are possible at any "
  "difference, though). Kill Red Trolls and Rock Snakes to make a cell higher.";

const char *caribbeanhelp = 
  "This dangerous sea contains nasty sharks and pirates. ";

const char *cislandhelp =
  "The islands of Caribbean are infinite and circular, and "
  "the pirates probably have hidden their treasures somewhere far from the coast.";

const char *redsnakedes = 
  "Rock snakes are similar to Sandworms, but they move each turn. However, they "
  "only move on the hexagonal cells. They are also somewhat longer than the Sandworms.";

const char *minedesc = 
  "This minefield has been created by the Bomberbirds to protect their valuable eggs. "
  "Mines explode when they are stepped on, creating fire all around them "
  "(this fire never expires in the Minefield). If you don't step on a mine, you "
  "know the number of mines in cells around you. These numbers are marked with "
  "color codes on the cells you have visited.";

const char *palacedesc =
  "So better just to concentrate on collecting treasure. "
  "Beware the traps, and the guards, who are hard to kill!";

const char *gatedesc =
  "Green pressure plates open all gates in radius 3 around them. "
  "Similarly, red pressure plates close all gates in this radius.";

const char *princessdesc =
  "A mouse squeaks at you. It seems that it wants you to go somewhere. "
  "That's interesting, what could you find here?\n\n"
  "Note: in the part of the Palace that you are exploring during this "
  "quest, the distribution of monsters, pressure plates, and items has been "
  "changed somewhat, to make it a bit more suitable for less "
  "experienced players. The number of monsters does not depend on the "
  "number of Hypersian Rugs you have collected, and there are more "
  "opening plates.\n\n"
  "However, even with the normal distribution, it appears that it should be "
  "always possible to reach your goal. If you want this challenge, "
  "select it from the special game modes menu.";
  
const char *elemdesc = 
  "The Elemental Planes are divided into four subzones: Planes of Fire, Water, Air, and Earth. "
  "You need to collect a Shard from each Plane to construct an Elemental Gem. "
  "It is dangerous to collect too many Shards of the same type without constructing a Gem.";

const char *NODESC = "No description yet.";
// --- monsters ---

const int motypes = 102;

struct monstertype {
  char  glyph;
  int   color;
  const char *name;
  const char *help;
  };

#define BUGCOLORS 3

const char* tamebomberdesc = 
  "This bomberbird will follow you at some distance, and attack your enemies. "
  "You can kill it to place a mine.";

const char *gargdesc = 
    "A being made of stone, who likes high buildings. It becomes normal stone when "
    "killed, but only if next to something stable -- otherwise it falls.";
  
monstertype minf[motypes] = {
  { 0,   0,        "none"       , NULL},
  { 'Y', 0x4040FF, "Yeti"       ,
    "A big and quite intelligent monster living in the Icy Land."
  },
  { 'W', 0xD08040, "Icewolf"    ,
    "A nasty predator from the Icy Land. Contrary to other monsters, "
    "it tracks its prey by their heat."
  },
  { 'W', 0xD08040, "Icewolf"    , ""},
  { 'R', 0xFF8000, "Ranger"     ,
    "Rangers take care of the magic mirrors in the Land of Mirrors. "
    "They know that rogues like to break these mirrors... so "
    "they will attack you!"
    },
  { 'T', 0xD0D0D0, "Rock Troll", trollhelp},
  { 'G', 0x20D020, "Goblin",     
    "A nasty creature native to the Living Caves. They don't like you "
    "for some reason."
    },
  { 'S', 0xE0E040, "Sand Worm", wormdes },
  { 's', 0x808000, "Sand Worm Tail", wormdes },
  { 'S', 0x808000, "Sand Worm W", wormdes },
  { 'H', 0x80FF00, "Hedgehog Warrior", 
    "These warriors of the Forest wield exotic weapons called hedgehog blades. "
    "These blades protect them from a frontal attack, but they still can be 'stabbed' "
    "easily by moving from one place next to them to another."
    },
  { 'M', 0x806050, "Desert Man",
    "A tribe of men native to the Desert. They have even tamed the huge Sandworms, who won't attack them."},
  { 'C', 0x00FFFF, "Ivy Root", ivydes},
  { 'C', 0xFFFF00, "Active Ivy", ivydes},
  { 'C', 0x40FF00, "Ivy Branch", ivydes},
  { 'C', 0x006030, "Dormant Ivy", ivydes},
  { 'C', 0x804000, "Dead Ivy", ivydes},
  { 'C', 0x800000, "Dead Ivy", ivydes},
  { 'M', 0x804000, "Giant Ape",
    "This giant ape thinks that you are an enemy."},
  { 'B', 0x909000, "Slime Beast", slimehelp},
  { '@', 0x8080FF, "Mirror Image",
    "A magical being which copies your movements."
    },
  { '@', 0xFF8080, "Mirage",
    "A magical being which copies your movements. "
    "You feel that it would be much more useful in an Euclidean space."
    },
  { '@', 0x509050, "Golem",
    "You can summon these friendly constructs with a magical process."
    },
  { '@', 0x509050, "Golem",
    "You can summon these friendly constructs with a magical process."
    },
  { 'E', 0xD09050, "Eagle",
    "A majestic bird, who is able to fly very fast."
    },
  { 'S', 0xFF8080, "Seep",
    "A monster who is able to live inside the living cave wall."
    },
  { 'Z', 0x804000, "Zombie",
    "A typical Graveyard monster."
    },
  { 'G', 0xFFFFFF, "Ghost",
    "A typical monster from the Graveyard, who moves through walls.\n\n"
    "There are also wandering Ghosts. They will appear "
    "if you do not explore any new places for a long time (about 100 turns). "
    "They can appear anywhere in the game."
    },
  { 'N', 0x404040, "Necromancer",
    "Necromancers can raise ghosts and zombies from fresh graves."
    },
  { 'S', 0x404040, "Shadow",
    "A creepy monster who follows you everywhere in the Graveyard."
    },
  { 'T', 0x40E040, "Tentacle", tentdes },
  { 't', 0x008000, "Tentacle Tail", tentdes },
  { 'T', 0x008000, "Tentacle W", tentdes },
  { 'z', 0xC00000, "Tentacle (withdrawing)", tentdes },
  { 'P', 0xFF8000, "Cultist", 
    "People worshipping Cthulhu. They are very dangerous." 
    },
  { 'P', 0xFFFF00, "Fire Cultist", 
    "People worshipping Cthulhu. This one is especially dangerous, "
    "as he is armed with a weapon which launches fire from afar."
    },
  { 'D', 0xFF0000, "Greater Demon", gdemonhelp},
  { 'D', 0x800000, "Greater Demon", gdemonhelp},
  { 'd', 0xFF2020, "Lesser Demon", ldemonhelp},
  { 'd', 0x802020, "Lesser Demon", ldemonhelp},
  { 'S', 0x2070C0, "Ice Shark", 
    "This dangerous predator has killed many people, and has been sent to Cocytus."
    },
  { 'W', 0xFFFFFF, "Running Dog", 
    "This white dog is able to run all the time. It is the only creature "
    "able to survive and breed in the Land of Eternal Motion."
    },
  { 'S', 0xC00040, "Demon Shark", 
    "Demons of Hell do not drown when they fall into the lake in Cocytus. "
    "They turn into demonic sharks, enveloped in a cloud of steam."
    },
  { 'S', 0xC00040, "Fire Fairy", 
    "These fairies would rather burn the forest, than let you get some Fern Flowers. "
    "The forest is infinite, after all...\n\n"
    "Fire Fairies transform into fires when they die."
    },
  { 'C', 0x4000C0, "Crystal Sage", 
    "This being radiates an aura of wisdom. "
    "It is made of a beautiful crystal, you would love to take it home. "
    "But how is it going to defend itself? Better not to think of it, "
    "thinking causes your brain to go hot...\n\n"
    "Crystal Sages melt at -30 °C, and they can rise the temperature around you from afar."
    },
  { 'P', 0x4040C0, "Pikeman", 
    "When Pikemen move, they attack all cells which are now adjacent to them. "
    "Luckily, they can be killed in the same way.\n\n"
    "They never move if this would attack their friends."
    },
  { 'F', 0xC04040, "Flail Guard", 
    "This guard of the Emerald Mine is wielding a huge flail. "
    "You cannot attack him directly, as the flail would still hit you then. "
    "Luckily, you have learned a trick: if you step away from him, "
    "he will hit himself with the flail!"
    },
  { 'M', 0x404040, "Miner", 
    "Miners have special tools for dealing with the Living Cave. "
    "When they die, these tools activate, destroying the living cave "
    "around them."
    },
  { 'V', 0x421C52, "Vine Beast", 
    "A beast made of vines!\n\n"
    "Vine Beasts turn into vines when they die."
    },
  { 'V', 0xFFC0C0, "Vine Spirit", 
    "A spirit living in the vines!\n\n"
    "Vine Spirits destroy the vines when they die."
    },
  { 'T', 0x803030, "Dark Troll", 
    "A Troll without the power of Life."
    },
  { 'E', 0xFFFF40, "Earth Elemental", 
    "A rare unliving construct from the Dead Caves. "
    "It instantly destroys cave walls next to its path, and also leaves "
    "an impassable wall behind it. You suppose that this impassable wall helps it to "
    "escape from some threats. You hope you won't meet these threats..."
    },
  { 'B', 0xC04040, "Red Hyperbug", hivehelp},
  { 'B', 0x40C040, "Green Hyperbug", hivehelp},
  { 'B', 0x4040C0, "Blue Hyperbug", hivehelp},
  { 'W', 0x404040, "Witch Apprentice", 
    "A Witch without any special powers. But watch out! She will "
    "pick up any basic Orbs on her path, and use their powers."
    },
  { 'W', 0xFF4040, "Speed Witch", 
    "A Witch with a Speed spell. She moves twice as fast as you. Unless you "
    "have an Orb of Speed too, of course!"
    },
  { 'W', 0xFFFFFF, "Flash Witch", 
    "A Witch with a Flash spell. Very dangerous!\n\n"
    "Luckily, she never uses the spell if it would kill her friends. "
    "She could destroy an Evil Golem, though."
    },
  { 'W', 0xFF8000, "Fire Witch", 
    "A Witch with a Fire spell. She will leave a trail of fire behind her."
    },
  { 'W', 0x8080FF, "Winter Witch", 
    "A Witch with a Winter spell. She is able to move through fire."
    },
  { 'W', 0x808080, "Aether Witch", 
    "A Witch with an Aether spell. She is able to move through fire and walls."
    },
  { '@', 0x905050, "Evil Golem",
    "Somebody has summoned these evil constructs with a magical process."
    },
  { '@', 0x8080FF, "Knight", camelothelp },
  { 'P', 0xD10000, "Cult Leader", 
    "These Cultists can push the statues, just like you."
    },
  { 'B', 0x909000, "Slime Beast", slimehelp},
  { '@', 0x8080FF, "Knight", camelothelp }, // knight moved
  { '@', 0x8B4513, "Illusion", 
    "Illusions are targetted "
    "by most monsters, just like yourself, Thumpers, and your friends."
    },
  { 'P', 0xD00000, "Pirate", 
    "Just a typical hyperbolic pirate." },
  { 'S', 0x8080C0, "Shark", "Just a nasty shark."},
  { 'P', 0x0000FF, "Parrot", "Parrots feel safe in the forests of Caribbean, so they "
    "never leave them. But they will help the Pirates by attacking the intruders."},
  { 'S', 0xE09000, "Rock Snake", redsnakedes },
  { 's', 0xE09000, "Rock Snake Tail", redsnakedes },
  { 'T', 0xC02020, "Red Troll", "A kind of Troll native to the Red Rock Valley."},
  { 'B', 0xA00000, "Bomberbird", 
    "Dark red birds who have created the minefield.\n\n"
    "They create a mine on the spot where they are killed, provided "
    "that the terrain is suitable. Also note that mines are triggered "
    "by dead birds falling on them."
    },
  { 'A', 0xFFFFFF, "Albatross", 
    "Those large seabirds attack you for some reason. At least they are not "
    "as fast as Eagles..."
    },
  { 'B', 0x40C000, "Tame Bomberbird", tamebomberdesc},
  { 'B', 0x40C000, "Tame Bomberbird", tamebomberdesc}, // bomberbird moved
  { 'G', 0xFF0000, "Palace Guard", 
     "You have to hit Palace Guards several times to kill them. After each hit, they "
     "are pushed away and stunned for some time."
     },
  { 'G', 0xC06000, "Fat Guard", "Fat guards are too heavy to be pushed away by your hits."},
  { 'G', 0xFFFFFF, "Skeleton", 
    "Skeletons work similar to Palace Guards, but they won't die no matter how many "
    "times you hit them. Well, you have to be more creative...\n\n"
    "Skeletons attacked outside of their native land, Palace, are stunned for a longer time."
    },
  { 'G', 0xC000C0, "Vizier", 
    "Viziers are neither pushed away nor stunned. However, you attack them automatically when "
    "escaping from them."},
  { 'V', 0xC0C0C0, "Viking", "Mighty warriors from the Fjord, who can use boats."},
  { 'T', 0x00FFFF, "Fjord Troll", 
    "Fjord Trolls leave a wall when they die, causing the living fjord to rise around it. "
    "Contrary to Rock Trolls, items around are not destroyed."
    },
  { 'E', 0x0000FF, "Water Elemental", 
    "Wherever this powerful being goes, the living fjord "
    "sinks below water, non-magical boats are destroyed, and fires are extinguished.\n\n"
    "As a special case, you can attack the Water Elemental from the water, without drowning immediately."
    },
  { 'M', 0xD0D0D0, "Mouse", princessdesc},
  { 'M', 0xD0D0D0, "Mouse", princessdesc},
  { 'P', 0xFF80FF, "Prince", princessdesc},
  { 'P', 0xFF80FF, "Princess", princessdesc},
  { 'P', 0xFF80FF, "Prince", princessdesc},
  { 'P', 0xFF80FF, "Princess", princessdesc},
  { 'S', 0xC0C0C0, "Servant", "A simple servant of the master of the Ivory Tower."},
  { 'B', 0x707070, "Gargoyle", gargdesc},
  { 'E', 0xFF0000, "Fire Elemental", 
    "This monster leaves a trail of fire behind."},
  { 'E', 0xC0C0FF, "Air Elemental", "An Air Elemental looks like a live tornado. Once you are three (or less) cells from it, "
  "it is impossible to move closer, due to strong winds. You can stand and wait, though. "
  "This also affects most monsters."},
  { 'D', 0xC06000, "Striped Dog", "A predator native to the Zebra."},
  { 'G', 0xFFFFFF, "Tentacle+Ghost", tentdes },
  // shmup specials
  { '@', 0xC0C0C0, "Rogue", "In the Shoot'em Up mode, you are armed with thrown Knives."},
  { '*', 0xC0C0C0, "Knife", "A simple, but effective, missile, used by rogues."},
  { '*', 0xFF0000, "Flail", "This attack is likely to hit the attacker."},
  { '*', 0xFFFF00, "Fireball", "This magical missile burns whatever it hits."},
  { '*', 0xFFFF00, "Tongue", "Some monsters have long tongues, which allow them to attack enemies in nearby cells."},
  { '*', 0xFFFFFF, "Airball", "This magical missile pushes back whatever it hits."}
  };

enum eMonster { 
  moNone, 
  moYeti, moWolf, moWolfMoved, 
  moRanger, 
  moTroll, moGoblin, 
  moWorm, moWormtail, moWormwait, moHedge, 
  moDesertman, 
  moIvyRoot, moIvyHead, moIvyBranch, moIvyWait, moIvyNext, moIvyDead,
  moMonkey,
  moSlime,
  moMirror, moMirage, moGolem, moGolemMoved,
  moEagle, moSeep,
  moZombie, moGhost, moNecromancer, moShadow,
  moTentacle, moTentacletail, moTentaclewait, moTentacleEscaping, 
  moCultist, moPyroCultist,
  moGreater, moGreaterM, moLesser, moLesserM,
  moShark, moRunDog, moGreaterShark, moFireFairy,
  moCrystalSage, moLancer, moFlailer, moMiner,
  moVineBeast, moVineSpirit, moDarkTroll, moEarthElemental,
  moBug0, moBug1, moBug2, 
  moWitch, moWitchSpeed, moWitchFlash, moWitchFire, moWitchWinter, moWitchGhost,
  moEvilGolem, moKnight, moCultistLeader, moSlimeNextTurn, moKnightMoved,
  moIllusion,
  moPirate, moCShark, moParrot,
  moHexSnake, moHexSnakeTail, moRedTroll, moBomberbird, moAlbatross,
  moTameBomberbird, moTameBomberbirdMoved,
  moPalace, moFatGuard, moSkeleton, moVizier,
  moViking, moFjordTroll, moWaterElemental,
  moMouse, moMouseMoved, 
  moPrincess, moPrincessMoved, 
  moPrincessArmed, moPrincessArmedMoved,
  moEdgeMonkey, moGargoyle, moFireElemental, moAirElemental,
  moOrangeDog, moTentacleGhost,
  // shmup specials
  moPlayer, moBullet, moFlailBullet, moFireball, moTongue, moAirball,
  // temporary
  moDeadBug, moLightningBolt, moDeadBird
  };

// --- items ----

const int ittypes = 67;

struct itemtype {
  char  glyph;
  int   color;
  const char *name;
  const char *help;
  };

itemtype iinf[ittypes] = {
  { 0,   0,        "none",       NULL},
  { '*', 0xFFFFFF, "Ice Diamond", 
    "Cold white gems, found in the Icy Land."
    },
  { '$', 0xFFD700, "Gold",
    "An expensive metal from the Living Caves. For some reason "
    "gold prevents the living walls from growing close to it."
    },
  { ';', 0xFF4000, "Spice",      
    "A rare and expensive substance found in the Desert. "
    "It is believed to extend life and raise special psychic powers."
    },
  { '*', 0xC00000, "Ruby",
    "A beautiful gem from the Jungle."
    },
  { '!', 0xFFFF00, "Elixir of Life",
    "A wonderful beverage, apparently obtained by mixing red and blue slime. You definitely feel more "
    "healthy after drinking it, but you still fell that one hit of a monster is enough to kill you."},
  { '%', 0xFF00FF, "Shard",
    "A piece of a magic mirror, or a mirage cloud, that can be used for magical purposes. Only mirrors and clouds "
    "in the Land of Mirrors leave these."},
  { '/', 0xFF8000, "Necromancer's Totem",
    "These sinister totems contain valuable gems."},
  { '%', 0x00D000, "Demon Daisy",
    "These star-shaped flowers native to Hell are a valuable alchemical component."},
  { '/', 0x00FF00, "Statue of Cthulhu",
    "This statue is made of materials which cannot be found in your world."},
  { '*', 0xFF8000, "Phoenix Feather", 
    "One of few things that does not cause the floor in the Land of Eternal Motion to collapse. Obviously they are quite valuable."
    },
  { '*', 0x8080FF, "Ice Sapphire", 
    "Cold blue gems, found in the Cocytus."
    },
  { '*', 0xEEFF20, "Hyperstone", 
    "These bright yellow gems can be found only by those who have mastered the Crossroads."
    },
  { '[', 0x8080FF, "Key", 
    "That's all you need to unlock the Orb of Yendor! Well... as long as you are able to return to the Orb that this key unlocks...\n\n"
    "Each key unlocks only the Orb of Yendor which led you to it."
    },
  { 'o', 0x306030, "Dead Orb",
    "These orbs can be found in the Graveyard. You think that they were once powerful magical orbs, but by now, their "
    "power is long gone. No way to use them, you could as well simply drop them...\n\n"
    },
  { 'o', 0xFF20FF, "Orb of Yendor",
    "This wonderful Orb can only be collected by those who have truly mastered this hyperbolic universe, "
    "as you need the right key to unlock it. Luckily, your psychic abilities will let you know "
    "where the key is after you touch the Orb." },
  { 'o', 0xFFFF00, "Orb of Storms",
    "This orb can be used to invoke the lightning spell, which causes lightning bolts to shoot from you in all directions."},
  { 'o', 0xFFFFFF, "Orb of Flash",
    "This orb can be used to invoke a flash spell, which destroys almost everything in radius of 2."},
  { 'o', 0x8080FF, "Orb of Winter",
    "This orb can be used to invoke a wall of ice. It also protects you from fires."},
  { 'o', 0xFF6060, "Orb of Speed",
    "This orb can be used to move faster for some time."},
  { 'o', 0x90B090, "Orb of Life",
    "This orb can be used to summon friendly golems. It is used instantly when you pick it up."},
  { 'o', 0x60D760, "Orb of Shielding",
    "This orb can protect you from damage."},
  { 'o', 0x606060, "Orb of Earth",
    "This orb lets you go through living walls. It also has powers in some of the other lands."},
  { 'o', 0x20FFFF, "Orb of Teleport",
    "This orb lets you instantly move to another location on the map. Just click a location which "
    "is not next to you to teleport there. "
    },
  { 'o', 0xA0FF40, "Orb of Safety",
    "This orb lets you instantly move to a safe faraway location. Knowing the nature of this strange world, you doubt "
    "that you will ever find the way back...\n\n"
    "Your game will be saved if you quit the game while the Orb of Safety is still powered.\n\n"
    "Technical note: as it is virtually impossible to return, this Orb recycles memory used for the world so far (even if you do not use it to save the game). "
    },
  { 'o', 0x40C000, "Orb of Thorns",
    "This orb allows attacking Hedgehog Warriors directly, as well as stabbing other monsters.\n"
    },
  { '%', 0x0000FF, "Fern Flower",
    "This flower brings fortune to the person who finds it.\n"
    },
  { '!', 0x900000, "Wine",
    "Wine grown under hyperbolic sun would be extremely prized in your home location."
    },
  { 'o', 0x706070, "Orb of Aether",
    "This orb allows one to pass through all kinds of walls and chasms."
    },
  { '$', 0xFFFFC0, "Silver",
    "A precious metal from the Dead Caves." 
    },
  { 'o', 0x005050, "Orb of the Mind",
    "This orb allows you to instantly kill a non-adjacent enemy by clicking it. "
    "Each use drains 30 charges."
    },
  { '!', 0xE2B227, "Royal Jelly",
    "This is what Hyperbug Queens eat. Very tasty and healthy." 
    },
  { '*', 0x60C060, "Emerald",
    "A precious green gem from the Emerald Mines." 
    },
  { 'o', 0x421C52, "Orb of Invisibility",
    "When you have this Orb, most monsters won't see you, unless "
    "you are standing still, attacking, or picking up items."
    },
  { '*', 0xFFFF00, "Powerstone",
    "A Stone from the Land of Power. You are not sure what it is exactly, but "
    "as the Powerstones are kept in crystal cabinets, they are surely valuable."
    },
  { 'o', 0xFF4000, "Orb of Fire",
    "When you have this Orb, you will leave a trail of fire behind you."
    },
  { '!', 0xFFFF00, "Holy Grail", camelothelp },
  { '?', 0x00FF00, "Grimoire",
    "The Grimoires contain many secrets of the Great Old Ones. "
    "Each new inner circle in the Temple of Cthulhu contains new Grimoires, with new secrets. "
    "You hope to read them when you return home, and to learn many things. "
    "The knowledge is valuable to you, but it is rather pointless to try to get "
    "several copies of the same Grimoire..."
    },
  { 'o', 0xFF8000, "Orb of the Dragon",
    "This Orb allows you to throw fire, just like the Fire Cultists.\n\n"
    "Each fire drains 5 charges. You are not allowed to throw fire into adjacent cells."
    },
  { 'o', 0x8B4513, "Orb of Trickery",
    "This Orb allows you to create illusions of yourself. Illusions are targetted "
    "by most monsters, just like yourself, Thumpers, and your friends.\n\n"
    "Each illusion takes 5 charges to create, and one extra charge "
    "per turn. You can also click your illusion to take it away, restoring 4 charges.\n\n"
    "If you have both Orb of Teleport and Orb of Trickery, Illusion is cast "
    "first -- you can then teleport on your Illusion to switch places with it."
    },
  { 'x', 0xFF0000, "Pirate Treasure", "Ye wonder where did th' Pirates find all these riches..."},
  { '?', 0xFFFFFF, "Compass", 
    "The hyperbolic pirates have no use for treasure maps. However, they have found "
    "out that a compass points to the center of the island. So they just go as "
    "far towards the center as they can, and hide their treasure there."
    },
  { '*', 0xFF8080, "Red Gem", "A gem from the Red Rock Valley."},
  { 'o', 0x6060FF, "Orb of Time", 
    "Normally, the power of most Orbs slowly fades away, even when "
    "you are not actively using them. This Orb prevents this.\n\n"
    
    "When you have the Orb of Time, Orbs which are not active won't lose their power. "
    "Orbs are considered active if they have a continuous power which has actually "
    "affected something in the last turn.\n\n"
    
    "Orbs of Shielding remain active after being activated (even if you are no longer "
    "attacked), and Orbs of Time have a bigger cap inside their native Caribbean than "
    "outside."
    },
  { 'o', 0x40C0C0, "Orb of Space", 
    "This Orb is able to bring faraway items to your location, even if there are "
    "monsters or barriers on the way. The cost of "
    "bringing an item (in charges) equals the square of its distance to you. Contrary "
    "to some other Orbs, usage is not allowed if you have not enough power left.",
    },
  { '!', 0xFF8080, "Bomberbird Egg", 
    "Bomberbird eggs are big and tasty, and thus valuable. "
    "They can hatch when left alone for some time (but this will never happen "
    "if you are watching)."
    },
  { '*', 0xFFBF00, "Amber", 
    "When the tide is away, beautiful ambers can be found on the hyperbolic beaches. "
    "Perhaps there used to be a forest here?"
    },
  { '$', 0xFFFFFF, "Pearl", 
    "You do not know exactly why, but there are valuable pearls on many boats "
    "in the whirlpool."
    },
  { 'o', 0x306000, "Orb of Friendship", 
    "This Orb summons a friendly Bomberbird."
    },
  { 'o', 0x000060, "Orb of Water", 
   "This Orb allows your boat to go against the current, "
   "and also to go into the land, creating water on the way."
    },
  { 'o', 0xC00060, "Orb of Air", 
    "This Orb allows you to blow your enemies away.\n\n"
    "Click a monster to blow it one cell away. It cannot be used against mimics, ghosts, sharks and other monsters restricted to a specific terrain, and multi-tile monsters."
    },
  { '?', 0x802000, "Hypersian Rug", 
    "Nice, a hyperbolic rug! It will not fit very well on your flat Euclidean floor, but who cares?"},
  { 'o', 0x60A060, "Orb of the Frog", 
    "This Orb lets you jump to a place which is two cell away from you, in a single turn. "
    "You can jump over water, chasms and fire, but not over walls."},
  { '*', 0x800000, "Garnet", "Vikings believe that garnets improve their strength."}, 
  { 'o', 0x0070C0, "Orb of the Fish", 
    "This Orb lets you dive into the water. While diving, you are able to see and collect underwater treasures, "
    "while being invisible to monsters yourself. You are afraid of jumping into the water straight from a boat, so usually you won't do it."
    },
  { 'o', 0xC00000, "Orb of Discord", 
    "Causes most monsters to attack other monsters, not only you and your friends."},
  { 'P', 0x00FF00, "SAVE", princessdesc},
  { 'o', 0xFF208F, "Orb of Love", 
    "Love takes time, but it heals all wounds, and transcends time and space.\n\n"
    "The Orb of Love is worth 30$$$, if you end the game with it.\n"
    },
  { '/', 0xFFFFE0, "Ivory Figurine", 
    "A beautiful figurine, made of ivory. Figurines close to the base of the Tower "
    "tend do disappear after you have collected many of them."
    },
  { '*', 0x606060, "Onyx", "A black gem with white stripes. It is beautiful."},
  { '%', 0xFF8000, "Fire Shard", elemdesc},
  { '%', 0x8080C0, "Air Shard", elemdesc},
  { '%', 0x80C080, "Earth Shard", elemdesc},
  { '%', 0x0000C0, "Water Shard", elemdesc},
  { '*', 0xF0F0F0, "Elemental Gem", elemdesc},
  { 'o', 0x309060, "Orb of Summoning",
    "This orb allows you to summon monsters. Usually, they are either Elementals or "
    "native creatures. While the monsters do not like being summoned, and will "
    "attack you once they recover from summoning sickness, such summoning "
    "often has its uses."
    },
  { 'o', 0x306090, "Orb of Matter", 
    "This Orb allows to temporarily create impassable matter, either to block paths or "
    "to build bridges across chasms and waters."}
  };

enum eItem { itNone, itDiamond, itGold, itSpice, itRuby, itElixir, itShard, itBone, itHell, itStatue,
  itFeather, itSapphire, itHyperstone, itKey,
  itGreenStone, itOrbYendor,
  itOrbLightning, itOrbFlash, itOrbWinter, itOrbSpeed, itOrbLife, itOrbShield, itOrbDigging,
  itOrbTeleport, itOrbSafety,
  itOrbThorns, itFernFlower,
  itWine, itOrbGhost, itSilver, itOrbPsi,
  itRoyalJelly, itEmerald, itOrbInvis, itPower, itOrbFire,
  itHolyGrail, itGrimoire,
  itOrbDragon, itOrbIllusion,
  itPirate, itCompass,
  itRedGem, itOrbPreserve, itOrbTelekinesis,
  itBombEgg, itCoast, itWhirlpool,
  itOrbFriend, itOrbWater, itOrbAir,
  itPalace, itOrbFrog,
  itFjord, itOrbFish,
  itOrbDiscord,
  itSavedPrincess, itOrbLove,
  itEdge, itZebra, 
  itFireShard, itAirShard, itEarthShard, itWaterShard,
  itElemental, itOrbSummon, itOrbMatter
  };

// --- wall types ---

const int walltypes = 69;

struct walltype {
  char  glyph;
  int   color;
  const char *name;
  const char *help;
  };

const char *lakeDesc = "Hell has these lakes everywhere... They are shaped like evil stars, and filled with burning sulphur.";

const char *thumpdesc = "A device that attracts sandworms and other enemies. You need to activate it.";

const char *twdesc = "This structure will disappear after some time.";

walltype winf[walltypes] = {
  { '.', 0xFF00FF, "none",        NULL},
  { '#', 0x8080FF, "ice wall",    
    "Ice Walls melt after some time has passed."
    },
  { '#', 0xC06000, "great wall",  barrierhelp},
  { '+', 0x900030, "red slime",  slimehelp },
  { '+', 0x300090, "blue slime",   slimehelp },
  { '#', 0xA0D0A0, "living wall", cavehelp},
  { '.', 0x306060, "living floor",cavehelp},
  { '#', 0xD03030, "dead troll",  trollhelp},
  { '#', 0xCDA98F, "sand dune",   
  "A natural terrain feature of the Desert."
  },
  { '%', 0xC0C0FF, "Magic Mirror",
  "You can go inside the Magic Mirror, and produce some mirror images to help you."
  },
  { '%', 0xFFC0C0, "Cloud of Mirage",
   "Tiny droplets of magical water. You see images of yourself inside them. "
   "Go inside the cloud, to make these images help you."},
  { '^', 0x8D694F, "Thumper", thumpdesc},
  { '^', 0x804000, "Fire",
    "This cell is on fire. Most beings and items cannot survive."
    },
  { '+', 0xC0C0C0, "ancient grave", 
  "An ancient grave." 
    },
  { '+', 0xD0D080, "fresh grave",
  "A fresh grave. Necromancers like those." 
    },
  { '#', 0x00FFFF, "column",
  "A piece of architecture typical to R'Lyeh." 
   },
  { '=', 0xFFFF00, "lake of sulphur", lakeDesc },
  { '=', 0xFFFF00, "lake of sulphur", lakeDesc },
  { '=', 0x000080, "lake", 
  "An impassable lake in Cocytus."
    },
  { '_', 0x000080, "frozen lake", cocytushelp },
  { '>', 0x101010, "chasm",
  "It was a floor... until something walked on it." 
    },
  { '>', 0x101010, "chasmD",
  "It was a floor... until something walked on it." 
    },
  { '#', 0x60C000, "big tree", foresthelp},
  { '#', 0x006000, "tree", foresthelp},
  { '#', 0x421C52*2, "vine", vinehelp},
  { ':', 0x006000, "vine", hvinehelp},
  { ';', 0x006000, "vine", hvinehelp},
  { '^', 0x804000, "partial fire", "This cell is partially on fire."},
  { '#', 0xA07070, "dead wall", deadcavehelp},
  { '.', 0x401010, "dead floor",deadcavehelp},
  { '.', 0x905050, "rubble", "Dead floor, with some rubble."},
  { '#', 0xD0D010, "weird rock", 
    "A weirdly colored rock. Hyperentomologists claim that the "
    "Hyperbug armies use these rocks to navigate back home after a victorious battle."
    },
  { '#', 0x8080C0, "crystal cabinet", 
    "Witches use these crystal cabinets to protect Powerstones, as well as the more "
    "expensive Orbs. They are partially protected from thieves: they are too strong "
    "to be smashed by conventional attacks, and if you try to steal the item "
    "using an Orb of Aether, your Aether power will be completely drained."
    },
  { '#', 0xC0C0C0, "wall of Camelot", camelothelp },
  { '#', 0xA06000, "Round Table", camelothelp },
  { '=', 0x0000A0, "moat of Camelot", camelothelp},
  { '+', 0x606060, "big statue of Cthulhu", 
    "These statues of Cthulhu are too large to carry, and they don't look too "
    "valuable anyway. Most monsters will never go through them... they probably have "
    "their reasons. But you can go! When you move into the cell containing "
    "a statue, you push the statue to the cell you left.\n"
    },
  { '=', 0x0000A0, "sea", caribbeanhelp},
  { '+', 0x0000A0, "boat", 
    "Hyperbolic pirates do not need huge ships, since so many lands to conquest "
    "are so close. These small boats are enough for them.\n\n"
    "Boats allow you to go through water. If you are in a boat, you can move into "
    "a water cell (and the boat will come with you)."
    },
  { '.', 0x00FF00, "island", cislandhelp},
  { '.', 0x80C000, "island", cislandhelp},
  { '#', 0x006000, "tree", 
    "The forests of Caribbean are too dense to be traversed by humans, "
    "and they are hard to burn. Many colorful parrots can be found there."
    },
  { ',', 0x800000, "rock I", redrockhelp},
  { ':', 0xC00000, "rock II", redrockhelp},
  { ';', 0xFF0000, "rock III", redrockhelp},
  { '.', 0xD0D0D0, "minefield", minedesc},
  { '.', 0xD0D0D0, "minefield", minedesc},
  { '.', 0x909090, "cell without mine", minedesc},
  { '+', 0x808000, "stranded boat", 
    "This boat cannot go through the sand. But if you sit inside and "
    "wait for the tide, you will be able to use it to travel through the Ocean."
    },
  { '#', 0xFFD500, "palace wall", palacedesc },
  { '+', 0xFFFFFF, "closed gate", gatedesc },
  { '-', 0x404040, "open gate", gatedesc },
  { '_', 0xC00000, "closing plate", gatedesc },
  { '_', 0x00C000, "opening plate", gatedesc },
  { '_', 0x202020, "trapdoor", "This floor will fall after someone goes there. Go quickly!" },
  { '+', 0xFF0000, "giant rug", 
    "This is the biggest Hypersian Rug you have ever seen! "
    "Unfortunately, it is too large to take it as a trophy." },
  { '#', 0xfffff0, "platform", "You can stand here."},
  { '#', 0x909090, "stone gargoyle", gargdesc},
  { '.', 0x909090, "stone gargoyle floor", gargdesc},
  { '.', 0x909090, "rubble", "Some rubble."},
  { '+', 0x804000, "ladder", 
    "You can use this ladder to climb the Tower."
    },
  { '#', 0xC0C0C0, "limestone wall", "Simply a wall. Mostly."},
  { '^', 0x804000, "Bonfire",
    "A heap of wood that can be used to start a fire. Everything is already here, you just need to touch it to fire it."
    },
  { '^', 0x8D694F, "Thumper",
     "A device that attracts sandworms and other enemies. You need to activate it."},
  { '^', 0x804000, "Eternal Fire",
    "This fire never burns out."
    },
  { '.', 0x909090, "stone gargoyle bridge", gargdesc},
  { '#', 0x309060, "temporary wall", twdesc},
  { '.', 0x309060, "temporary floor", twdesc},
  { '.', 0x309060, "temporary bridge", twdesc}
  };

enum eWall { waNone, waIcewall, waBarrier, waFloorA, waFloorB, waCavewall, waCavefloor, waDeadTroll, waDune,
  waMirror, waCloud, waThumperOff, waFire, waAncientGrave, waFreshGrave, waColumn, waSulphurC, waSulphur,
  waLake, waFrozenLake, waChasm, waChasmD, waDryTree, waWetTree, 
  waVinePlant, waVineHalfA, waVineHalfB, waPartialFire, 
  waDeadwall, waDeadfloor, waDeadfloor2, waWaxWall, waGlass, waCamelot, waRoundTable,
  waCamelotMoat,
  waBigStatue,
  waSea, waBoat, waCIsland, waCIsland2, waCTree,
  waRed1, waRed2, waRed3,
  waMineUnknown, waMineMine, waMineOpen,
  waStrandedBoat,
  waPalace, waClosedGate, waOpenGate, waClosePlate, waOpenPlate, waTrapdoor,
  waGiantRug,
  waPlatform, waGargoyle, waGargoyleFloor, waRubble, waLadder, waStone,
  waBonfireOff, waThumperOn, waEternalFire,
  waGargoyleBridge,
  waTempWall, waTempFloor, waTempBridge,
  // temporary walls for various purposes
  waTemporary, waEarthD, waElementalTmp, waElementalD
  };

// --- land types ---

const int numLands = 35;
const int landtypes = numLands + 6;

struct landtype {
  int color;
  const char *name;
  const char *help;
  };

landtype linf[landtypes] = {
  { 0xFF00FF, "???"          , ""},
  { 0xC06000, "Great Wall"   , ""},
  { 0xFF0000, "Crossroads"   , 
    "This land is a quick gateway to other lands. It is very easy to find other lands "
    "from the Crossroads. Which means that you find monsters from most other lands here!\n\n"
    "As long as you have found enough treasure in their native lands, you can "
    "find magical items in the Crossroads. Mirror Land brings mirrors and clouds, "
    "and other land types bring magical orbs.\n\n"
    "A special treasure, Hyperstone, can be found on the Crossroads, but only "
    "after you have found 10 of every other treasure."
    },
  { 0xCDA98F, "Desert",
    "A hot land, full of sand dunes, mysterious Spice, and huge and dangerous sand worms."
    },
  { 0x8080FF, "Icy Land",
    "A very cold land, full of ice walls. Your mere presence will cause these ice walls to "
    "melt, even if you don't want it."
    },
  { 0x306060, "Living Cave", cavehelp},
  { 0x00C000, "Jungle",
    "A land filled with huge ivy plants and dangerous animals."
    },
  { 0x900090, "Alchemist Lab", slimehelp},
  { 0x704070, "Mirror Land",
    "A strange land which contains mirrors and mirages, protected by Mirror Rangers."},
  { 0x404070, "Graveyard",
    "All the monsters you kill are carried to this strange land, and buried. "
    "Careless Rogues are also carried here..."
    },
  { 0x00FF00, "R'Lyeh",
    "An ancient sunken city which can be reached only when the stars are right.\n\n"
    "You can find Temples of Cthulhu in R'Lyeh once you collect five Statues of Cthulhu."
    },
  { 0xC00000, "Hell",
    "A land filled with demons and molten sulphur. Abandon all hope ye who enter here!"
    },
  { 0x00FF00, "Cocytus",
    cocytushelp
    },
  { 0xFFFF00, "Land of Eternal Motion",
    "A land where you cannot stop, because every piece of floor is extremely unstable. Only monsters who "
    "can run forever are able to survive there, and only phoenix feathers are so light that they do not disturb "
    "the floor.\n"
    },
  { 0x008000, "Dry Forest", foresthelp},
  { 0x0000C0, "Emerald Mine", 
     "Evil people are mining for emeralds in this living cave. "
     "It does not grow naturally, but it is dug out in a regular "
     "pattern, which is optimal according to the evil engineers."
     },
  { 0x421C52, "Vineyard", foresthelp},
  { 0x104040, "Dead Cave", deadcavehelp},
  { 0x705020, "Hive", hivehelp},
  { 0xFFFF00, "Land of Power", 
    "The Land of Power is filled with everburning fire, magical Orbs, and guarded by "
    "witches and golems. There are basic orbs lying everywhere, and more prized ones "
    "are kept in crystal cabinets.\n\n"
    "Witches are allowed to use all the powers of the "
    "basic orbs against intruders. These powers never expire, but a Witch "
    "can use only one power at a time (not counting Orbs of Life).\n\n"
    "Witches and Golems don't pursue you into other Lands. Also, most Orb powers "
    "are drained when you leave the Land of Power."
    },
  { 0xD0D0D0, "Camelot", camelothelp },
  { 0xD000D0, "Temple of Cthulhu", templehelp },
  { 0xFF8000, "Crossroads II", 
    "An alternate layout of the Crossroads. It is more dense and more regular, "
    "although you won't find the castle of Camelot here."
    },
  { 0x0000FF, "Caribbean", caribbeanhelp},
  { 0x400000, "Red Rock Valley", redrockhelp },
  { 0xD0D0D0, "Minefield", minedesc},
  { 0x2020FF, "Ocean", 
    "You can collect some valuable Ambers on the coast, but beware the tides!\n\n"
    "You can also take one of the boats and venture into the Ocean, "
    "to find other coasts, dangerous whirlpools, and islands inhabited by Pirates."
    },
  { 0x0000C0, "Whirlpool", 
    "Many lost boats are spinning in this whirlpool. Some of them contain treasures "
    "and Pirates.\n\n"
    "Unmanned boats will go one cell clockwise in each turn. Sharks and manned boats "
    "are only allowed to go with the current, that is, towards the center, or clockwise."
    },
  { 0xFFD500, "Palace", palacedesc },
  { 0xC0C000, "Living Fjord", 
    "A coastal area, from where you can get both to the inland worlds and to the Ocean. "
    "Each turn, each cell will become water or earth, based on the majority of cells around it. "
    "Contrary to the Living Caves, this process is not affected by most items and monsters, "
    "but elementals, dead Trolls, and cells outside of the Living Fjord have "
    "a very powerful effect."
     },
  { 0xFFFFE0, "Ivory Tower", "Powerful wizards claimed this part of the world, to perform their magical "
  "experiments in peace and solitude. They have changed the direction of gravity, "
  "to make it even harder for intruders to reach them.\n\n"
  "Gravity works as follows: cells are unstable if they are empty, and there is "
  "no cell immediately below them which contains a wall. It is impossible to move "
  "from one unstable cell to another, except if moving down."},
  { 0x606060, "Zebra", "Everything in this Land has black and white stripes."},
  { 0xC08080, "Plane of Fire", elemdesc},
  { 0x808080, "Plane of Air", elemdesc},
  { 0x80C080, "Plane of Earth", elemdesc},
  { 0x8080C0, "Plane of Water", elemdesc},
  { 0x4040FF, "Crossroads III", 
    "An alternate layout of the Crossroads. Great Walls cross here at right angles."
    },
  { 0x4040C0, "Sea Border", "Border between seas."},
  { 0x4040C0, "Elemental Planes", elemdesc},
  { 0xE08020, "Canvas", "A fake Land with colored floors."},
  { 0x00C000, "Palace Quest", princessdesc}, // this is fake
  };

enum eLand { laNone, laBarrier, laCrossroads, laDesert, laIce, laCaves, laJungle, laAlchemist, laMirror, laGraveyard,
  laRlyeh, laHell, laCocytus, laMotion, laDryForest, laEmerald, laWineyard, laDeadCaves, 
  laHive, laPower, laCamelot, laTemple, 
  laCrossroads2, laCaribbean, laRedRock, laMinefield, laOcean, laWhirlpool,
  laPalace, laLivefjord, 
  laEdge, laZebra, laEFire, laEAir, laEEarth, laEWater, laCrossroads3,
  laOceanWall, laElementalWall, laCanvas, laPrincessQuest };

// cell information for the game

struct gcell {

  // main fields
  eLand land : 8;
  eWall wall : 8;
  eMonster monst : 8;
  eItem item : 8;

  // if this is a barrier, what lands on are on the sides?
  eLand barleft : 8, barright : 8; 

  unsigned ligon : 1;    // is it sparkling with lightning?

  unsigned 
    pathdist : 5,       // player distance wrt usual movement
    cpdist : 5, mpdist : 5; // current/minimum player distance

  unsigned mondir : 3,   // monster direction, for multi-tile monsters and graphics
    bardir : 4,         // barrier direction
    stuntime : 4,       // stun time left (for Palace Guards and Skeletons)
    hitpoints : 3,      // hitpoints left (for Palace Guards)
    landflags : 2;      // extra flags for land
  
  char wparam;          // wall parameter, used for remaining power of Bonfires and Thumpers

  // 'tmp' is used for:
  // pathfinding algorithm used by monsters with atypical movement (which do not use pathdist)
  // bugs' pathfinding algorithm
  short aitmp;

  // 'landparam' is used for: 
  // heat in Icy/Cocytus; 
  // heat in Dry (0..10); 
  // CR2 structure; 
  // hive Weird Rock color / pheromones;
  // Ocean/coast depth
  union { int32_t landpar; float heat; } LHU;
  };

#define landparam LHU.landpar

#define NODIR 7
#define NOBARRIERS 8
