// --- help ---

#define MC
#define M

const char *wormdes = 
MC"These huge monsters normally live below the sand, but your movements have "
MC"disturbed them. They are too big to be slain with your "
MC"weapons, but you can defeat them by making them unable to move. "
M "This also produces some Spice. They move two times slower than you.";

const char *cocytushelp =
M  "This frozen lake is a hellish version of the Icy Land. Now, your body heat melts the floor, not the walls.";

const char *tentdes = 
MC"The tentacles of Cthulhu are like sandworms, but longer. "
M "They also withdraw one cell at a time, instead of exploding instantly.";

const char *gameboardhelp = 
MC"Ever wondered how some boardgame would look on the hyperbolic plane? "
MC"I wondered about Go, so I have created this feature. Now you can try yourself!\n"
MC"Enter = pick up an item (and score), space = clear an item\n"
M "Other keys place orbs and terrain features of various kinds\n"
  "In the periodic editor, press 0-4 to switch walls in different ways\n"
  ;

const char *ivydes = 
MC"A huge plant growing in the Jungle. Each Ivy has many branches, "
MC"and one branch grows per each of your moves. Branches grow in a clockwise "
M "order. The root itself is vulnerable.";

const char *slimehelp =
MC"The Alchemists produce magical potions from pools of blue and red slime. You "
MC"can go through these pools, but you cannot move from a blue pool to a red "
MC"pool, or vice versa. Pools containing items count as colorless, and "
MC"they change color to the PC's previous color when the item is picked up. "
MC"Slime beasts also have to keep to their own color, "
MC"but when they are killed, they explode, destroying items and changing "
M "the color of the slime and slime beasts around them.";

const char *gdemonhelp =
MC  "These creatures are slow, but very powerful... more powerful than you. "
MC  "You need some more experience in demon fighting before you will be able to defeat them. "
MC  "Even then, you will be able to slay this one, but more powerful demons will come...\n\n"
MC  "Each 10 lesser demons you kill, you become powerful enough to kill all the greater "
M   "demons on the screen, effectively turning them into lesser demons.";    

const char *ldemonhelp =
M   "These creatures are slow, but they often appear in large numbers.";

const char *trollhelp =
MC"A big monster from the Living Caves. A dead Troll will be reunited "
M "with the rocks, causing some walls to grow around its body.";

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
M "Huge, impassable walls which separate various lands.";

const char *cavehelp =
MC"This cave contains walls which are somehow living. After each turn, each cell "
MC"counts the number of living wall and living floor cells around it, and if it is "
MC"currently of a different type than the majority of cells around it, it switches. "
MC"Items count as three floor cells, and dead Trolls count as five wall cells. "
M "Some foreign monsters also count as floor or wall cells.\n";

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
MC"This forest is quite dry. Beware the bushfires!\n"
MC"Trees catch fire on the next turn. The temperature of the grass cells "
MC"rises once per turn for each fire nearby, and becomes fire itself "
MC"when its temperature has risen 10 times.\n"
M "You can also cut down the trees. Big trees take two turns to cut down.";

const char *hivehelp =
  "The Hive is filled with Hyperbugs. They are huge insects which look a bit like "
  "ants, a bit like bees, and a bit like roaches. "
  "They live in circular nests, and an army of Hyperbugs will attack any intruder, "
  "including you and enemy Hyperbugs. Will you manage to get to the "
  "heart of such a nest, and get the precious Royal Jelly?";

// --- monsters ---

const int motypes = 67;

struct monstertype {
  char  glyph;
  int   color;
  const char *name;
  const char *help;
  };

#define BUGCOLORS 3

monstertype minf[motypes] = {
  { 0,   0,        "none"       , NULL},
  { 'Y', 0x4040FF, "Yeti"       ,
M   "A big and quite intelligent monster living in the Icy Land."
  },
  { 'W', 0xD08040, "Icewolf"    ,
MC  "A nasty predator from the Icy Land. Contrary to other monsters, "
M   "it tracks its prey by their heat."
  },
  { 'W', 0xD08040, "Icewolf"    , ""},
  { 'R', 0xFF8000, "Ranger"     ,
MC  "Rangers take care of the magic mirrors in the Land of Mirrors. "
MC  "They know that rogues like to break these mirrors... so "
M   "they will attack you!"
    },
  { 'T', 0xD0D0D0, "Rock Troll", trollhelp},
  { 'G', 0x20D020, "Goblin",     
MC  "A nasty creature native to the Living Caves. They don't like you "
M   "for some reason."
    },
  { 'S', 0xE0E040, "Sand Worm", wormdes },
  { 's', 0x808000, "Sand Worm Tail", wormdes },
  { 'S', 0x808000, "Sand Worm W", wormdes },
  { 'H', 0x80FF00, "Hedgehog Warrior", 
MC  "These warriors of the Forest wield exotic weapons called hedgehog blades. "
MC  "These blades protect them from a frontal attack, but they still can be 'stabbed' "
M   "easily by moving from one place next to them to another."
    },
  { 'M', 0x806050, "Desert Man",
M   "A tribe of men native to the Desert. They have even tamed the huge Sandworms, who won't attack them."},
  { 'C', 0x00FFFF, "Ivy Root", ivydes},
  { 'C', 0xFFFF00, "Active Ivy", ivydes},
  { 'C', 0x40FF00, "Ivy Branch", ivydes},
  { 'C', 0x006030, "Dormant Ivy", ivydes},
  { 'C', 0x800000, "Ivy N", ivydes},
  { 'C', 0x800000, "Ivy D", ivydes},
  { 'M', 0x804000, "Giant Ape",
M   "This giant ape thinks that you are an enemy."},
  { 'B', 0x909000, "Slime Beast", slimehelp},
  { '@', 0x8080FF, "Mirror Image",
M   "A magical being which copies your movements."
    },
  { '@', 0xFF8080, "Mirage",
MC  "A magical being which copies your movements. "
M   "You feel that it would be much more useful in an Euclidean space."
    },
  { '@', 0x509050, "Golem",
M   "You can summon these friendly constructs with a magical process."
    },
  { '@', 0x509050, "Golem",
M   "You can summon these friendly constructs with a magical process."
    },
  { 'E', 0xD09050, "Eagle",
M   "A majestic bird, who is able to fly very fast."
    },
  { 'S', 0xFF8080, "Seep",
M   "A monster who is able to live inside the living cave wall."
    },
  { 'Z', 0x804000, "Zombie",
M   "A typical Graveyard monster."
    },
  { 'G', 0xFFFFFF, "Ghost",
M   "A typical monster from the Graveyard, who moves through walls.\n\n"
    "There are also wandering Ghosts. They will appear "
    "if you do not explore any new places for a long time (about 100 turns). "
    "They can appear anywhere in the game."
    },
  { 'N', 0x404040, "Necromancer",
M   "Necromancers can raise ghosts and zombies from fresh graves."
    },
  { 'S', 0x404040, "Shadow",
M   "A creepy monster who follows you everywhere in the Graveyard."
    },
  { 'T', 0x40E040, "Tentacle", tentdes },
  { 't', 0x008000, "Tentacle Tail", tentdes },
  { 'T', 0x008000, "Tentacle W", tentdes },
  { 'z', 0xC00000, "Tentacle (withdrawing)", tentdes },
  { 'P', 0xFF8000, "Cultist", 
M   "People worshipping Cthulhu. They are very dangerous." 
    },
  { 'P', 0xFFFF00, "Fire Cultist", 
MC  "People worshipping Cthulhu. This one is especially dangerous, "
M   "as he is armed with a weapon which launches fire from afar."
    },
  { 'D', 0xFF0000, "Greater Demon", gdemonhelp},
  { 'D', 0x800000, "Greater Demon", gdemonhelp},
  { 'd', 0xFF2020, "Lesser Demon", ldemonhelp},
  { 'd', 0x802020, "Lesser Demon", ldemonhelp},
  { 'S', 0x2070C0, "Ice Shark", 
M   "This dangerous predator has killed many people, and has been sent to Cocytus."
    },
  { 'W', 0xFFFFFF, "Running Dog", 
MC  "This white dog is able to run all the time. It is the only creature "
M   "able to survive and breed in the Land of Eternal Motion."
    },
  { 'S', 0xC00040, "Demon Shark", 
MC  "Demons of Hell do not drown when they fall into the lake in Cocytus. "
M   "They turn into demonic sharks, enveloped in a cloud of steam."
    },
  { 'S', 0xC00040, "Fire Fairy", 
MC  "These fairies would rather burn the forest, than let you get some Fern Flowers. "
MC  "The forest is infinite, after all...\n\n"
M   "Fire Fairies transform into fires when they die."
    },
  { 'C', 0x4000C0, "Crystal Sage", 
MC  "This being radiates an aura of wisdom. "
MC  "It is made of a beautiful crystal, you would love to take it home. "
MC  "But how is it going to defend itself? Better not to think of it, "
MC  "thinking causes your brain to go hot...\n\n"
M   "Crystal Sages melt at -30 °C, and they can rise the temperature around you from afar."
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
  { 'V', 0x003000, "Vine Spirit", 
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
  // temporary
  moDeadBug, moLightningBolt
  };

#define NUMWITCH 7

// --- items ----

const int ittypes = 40;

struct itemtype {
  char  glyph;
  int   color;
  const char *name;
  const char *help;
  };

itemtype iinf[ittypes] = {
  { 0,   0,        "none",       NULL},
  { '*', 0xFFFFFF, "Ice Diamond", 
M   "Cold white gems, found in the Icy Land."
    },
  { '$', 0xFFD700, "Gold",
MC  "An expensive metal from the Living Caves. For some reason "
M   "gold prevents the living walls from growing close to it."
    },
  { ';', 0xFF4000, "Spice",      
MC  "A rare and expensive substance found in the Desert. "
M   "It is believed to extend life and raise special psychic powers."
    },
  { '*', 0xC00000, "Ruby",
M   "A beautiful gem from the Jungle."
    },
  { '!', 0xFFFF00, "Elixir of Life",
MC  "A wonderful beverage, apparently obtained by mixing red and blue slime. You definitely feel more "
M   "healthy after drinking it, but you still fell that one hit of a monster is enough to kill you."},
  { '%', 0xFF00FF, "Shard",
MC  "A piece of a magic mirror, or a mirage cloud, that can be used for magical purposes. Only mirrors and clouds "
M   "in the Land of Mirrors leave these."},
  { '/', 0xFF8000, "Necromancer's Totem",
M   "These sinister totems contain valuable gems."},
  { '%', 0x00D000, "Demon Daisy",
M   "These star-shaped flowers native to Hell are a valuable alchemical component."},
  { '/', 0x00FF00, "Statue of Cthulhu",
M   "This statue is made of materials which cannot be found in your world."},
  { '*', 0xFF8000, "Phoenix Feather", 
M   "One of few things that does not cause the floor in the Land of Eternal Motion to collapse. Obviously they are quite valuable."
    },
  { '*', 0x8080FF, "Ice Sapphire", 
M   "Cold blue gems, found in the Cocytus."
    },
  { '*', 0xEEFF20, "Hyperstone", 
M   "These bright yellow gems can be found only by those who have mastered the Crossroads."
    },
  { '[', 0x8080FF, "Key", 
MC  "That's all you need to unlock the Orb of Yendor! Well... as long as you are able to return to the Orb that this key unlocks...\n\n"
M   "Each key unlocks only the Orb of Yendor which led you to it."
    },
  { 'o', 0x306030, "Dead Orb",
MC  "These orbs can be found in the Graveyard. You think that they were once powerful magical orbs, but by now, their "
M   "power is long gone. No way to use them, you could as well simply drop them...\n\n"
    },
  { 'o', 0xFF20FF, "Orb of Yendor",
MC  "This wonderful Orb can only be collected by those who have truly mastered this hyperbolic universe, "
MC  "as you need the right key to unlock it. Luckily, your psychic abilities will let you know "
M   "where the key is after you touch the Orb." },
  { 'o', 0xFFFF00, "Orb of Storms",
M   "This orb can be used to invoke the lightning spell, which causes lightning bolts to shoot from you in all directions."},
  { 'o', 0xFFFFFF, "Orb of Flash",
M   "This orb can be used to invoke a flash spell, which destroys almost everything in radius of 2."},
  { 'o', 0x8080FF, "Orb of Winter",
M   "This orb can be used to invoke a wall of ice. It also protects you from fires."},
  { 'o', 0xFF6060, "Orb of Speed",
M   "This orb can be used to move faster for some time."},
  { 'o', 0x90B090, "Orb of Life",
M   "This orb can be used to summon friendly golems. It is used instantly when you pick it up."},
  { 'o', 0x60D760, "Orb of Shielding",
M   "This orb can protect you from damage."},
  { 'o', 0x606060, "Orb of Earth",
M   "This orb lets you go through living walls. It also has powers in some of the other lands."},
  { 'o', 0x20FFFF, "Orb of Teleport",
MC  "This orb lets you instantly move to another location on the map. Just click a location which "
M   "is not next to you to teleport there. "
    },
  { 'o', 0xA0FF40, "Orb of Safety",
MC  "This orb lets you instantly move to a safe faraway location. Knowing the nature of this strange world, you doubt "
MC  "that you will ever find the way back...\n\n"
MC  "Your game will be saved if you quit the game while the Orb of Safety is still powered.\n\n"
M   "Technical note: as it is virtually impossible to return, this Orb recycles memory used for the world so far (even if you do not use it to save the game). "
    },
  { 'o', 0x40C000, "Orb of Thorns",
M   "This orb allows attacking Hedgehog Warriors directly, as well as stabbing other monsters.\n"
    },
  { '%', 0x0000FF, "Fern Flower",
M   "This flower brings fortune to the person who finds it.\n"
    },
  { '!', 0x900000, "Wine",
M   "Wine grown under hyperbolic sun would be extremely prized in your home location."
    },
  { 'o', 0x706070, "Orb of Aether",
M   "This orb allows one to pass through all kinds of walls and chasms."
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
  itOrbDragon, itOrbIllusion
  };

// --- wall types ---

const int walltypes = 37;

struct walltype {
  char  glyph;
  int   color;
  const char *name;
  const char *help;
  };

const char *lakeDesc = "Hell has these lakes everywhere... They are shaped like evil stars, and filled with burning sulphur.";

walltype winf[walltypes] = {
  { '.', 0xFF00FF, "none",        NULL},
  { '#', 0x8080FF, "ice wall",    
M   "Ice Walls melt after some time has passed."
    },
  { '#', 0xC06000, "great wall",  barrierhelp},
  { '+', 0x900030, "red slime",  slimehelp },
  { '+', 0x300090, "blue slime",   slimehelp },
  { '#', 0xA0D0A0, "living wall", cavehelp},
  { '.', 0x306060, "living floor",cavehelp},
  { '#', 0xD03030, "dead troll",  trollhelp},
  { '#', 0xCDA98F, "sand dune",   
M "A natural terrain feature of the Desert."
  },
  { '%', 0xC0C0FF, "Magic Mirror",
M "You can go inside the Magic Mirror, and produce some mirror images to help you."
  },
  { '%', 0xFFC0C0, "Cloud of Mirage",
MC "Tiny droplets of magical water. You see images of yourself inside them. "
M  "Go inside the cloud, to make these images help you."},
  { '^', 0x8D694F, "Thumper",
M    "A device that attracts sandworms and other enemies. You need to activate it."},
  { '^', 0x804000, "Bonfire",
M   "A heap of wood that can be used to start a fire. Everything is already here, you just need to touch it to fire it."
    },
  { '+', 0xC0C0C0, "ancient grave", 
M "An ancient grave." 
    },
  { '+', 0xD0D080, "fresh grave",
M "A fresh grave. Necromancers like those." 
    },
  { '#', 0x00FFFF, "column",
M "A piece of architecture typical to R'Lyeh." 
   },
  { '=', 0xFFFF00, "lake of sulphur", lakeDesc },
  { '=', 0xFFFF00, "lake of sulphur", lakeDesc },
  { '=', 0x000080, "lake", 
M "An impassable lake in Cocytus."
    },
  { '_', 0x000080, "frozen lake", cocytushelp },
  { '>', 0x101010, "chasm",
M "It was a floor... until something walked on it." 
    },
  { '>', 0x101010, "chasmD",
M "It was a floor... until something walked on it." 
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
    }
  };

enum eWall { waNone, waIcewall, waBarrier, waFloorA, waFloorB, waCavewall, waCavefloor, waDeadTroll, waDune,
  waMirror, waCloud, waThumper, waBonfire, waAncientGrave, waFreshGrave, waColumn, waSulphurC, waSulphur,
  waLake, waFrozenLake, waChasm, waChasmD, waDryTree, waWetTree, 
  waVinePlant, waVineHalfA, waVineHalfB, waPartialFire, 
  waDeadwall, waDeadfloor, waDeadfloor2, waWaxWall, waGlass, waCamelot, waRoundTable,
  waCamelotMoat,
  waBigStatue,
  // temporary walls for various purposes
  waTemporary, waEarthD
  };

// --- land types ---

const int numLands = 20;
const int landtypes = numLands + 3;

struct landtype {
  int color;
  const char *name;
  const char *help;
  };

landtype linf[landtypes] = {
  { 0xFF00FF, "???"          , ""},
  { 0xC06000, "Great Wall"   , ""},
  { 0xFF0000, "Crossroads"   , 
MC  "This land is a quick gateway to other lands. It is very easy to find other lands "
MC  "from the Crossroads. Which means that you find monsters from most other lands here!\n\n"
MC  "As long as you have found enough treasure in their native lands, you can "
MC  "find magical items in the Crossroads. Mirror Land brings mirrors and clouds, "
MC  "and other land types bring magical orbs.\n\n"
MC  "A special treasure, Hyperstone, can be found on the Crossroads, but only "
MC  "after you have found 10 of every other treasure."
    },
  { 0xCDA98F, "Desert",
M   "A hot land, full of sand dunes, mysterious Spice, and huge and dangerous sand worms."
    },
  { 0x8080FF, "Icy Land",
MC  "A very cold land, full of ice walls. Your mere presence will cause these ice walls to "
M   "melt, even if you don't want it."
    },
  { 0x306060, "Living Cave", cavehelp},
  { 0x00C000, "Jungle",
M   "A land filled with huge ivy plants and dangerous animals."
    },
  { 0x900090, "Alchemist Lab", slimehelp},
  { 0x704070, "Mirror Land",
M   "A strange land which contains mirrors and mirages, protected by Mirror Rangers."},
  { 0x404070, "Graveyard",
MC  "All the monsters you kill are carried to this strange land, and buried. "
M   "Careless Rogues are also carried here..."
    },
  { 0x00FF00, "R'Lyeh",
M   "An ancient sunken city which can be reached only when the stars are right.\n\n"
    "You can find Temples of Cthulhu in R'Lyeh once you collect five Statues of Cthulhu."
    },
  { 0xC00000, "Hell",
M   "A land filled with demons and molten sulphur. Abandon all hope ye who enter here!"
    },
  { 0x00FF00, "Cocytus",
    cocytushelp
    },
  { 0xFFFF00, "Land of Eternal Motion",
MC  "A land where you cannot stop, because every piece of floor is extremely unstable. Only monsters who "
MC  "can run forever are able to survive there, and only phoenix feathers are so light that they do not disturb "
M   "the floor.\n"
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
    "Witches and Golems don't pursue you into other Lands. Also, most Orb powers"
    "are drained when you leave the Land of Power."
    },
  { 0xD0D0D0, "Camelot", camelothelp },
  { 0xD000D0, "Temple of Cthulhu", templehelp },
  { 0xE08020, "Game Board", gameboardhelp}
  };

enum eLand { laNone, laBarrier, laCrossroads, laDesert, laIce, laCaves, laJungle, laAlchemist, laMirror, laGraveyard,
  laRlyeh, laHell, laCocytus, laMotion, laDryForest, laFjord, laWineyard, laDeadCaves, 
  laHive, laPower, laCamelot, laTemple, laGameBoard };

// cell information for the game

struct gcell {
  // 8 bits
  eLand land : 5;
  unsigned mondir : 3;
  // 8 bits
  eMonster monst : 7;
  unsigned ligon : 1;
  // 16 bits
  eWall wall : 6;
  eLand barleft : 5, barright : 5;
  // 16 bits
  eItem item : 6;
  unsigned cpdist : 5, mpdist : 5;
  // 14 bits
  unsigned pathdist : 10, bardir : 4;
  short tmp;
  float heat;
  };

#define NODIR 7
#define NOBARRIERS 8
