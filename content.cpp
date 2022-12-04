// Hyperbolic Rogue -- items, monsters, walls, and lands
// Copyright (C) 2011-2018 Zeno Rogue, see 'hyper.cpp' for details

/** \file content.cpp
 *  \brief X-macros for items, monsters, walls, and lands
 *
 *  Add new content at 'add new content here' so that constants do not change.
 *  Old things are sorted according to type, but this is not necessary for new content.
 */

#ifndef MONSTER
#define MONSTER(a,b,c,d,e,f,g,h)
#endif

#ifndef WALL
#define WALL(a,b,c,d,e,f,g,h,i)
#endif

#ifndef ITEM
#define ITEM(a,b,c,d,e,f,g,h,i)
#endif

#ifndef LAND
#define LAND(a,b,c,d,e,f,g)
#endif

#ifndef NATIVE
#define NATIVE(x)
#endif

#ifndef REQ
#define REQ(x)
#define REQAS(x,y)
#endif

MONSTER( 0,   0,        "no monster"       , moNone, ZERO | CF_NOGHOST | CF_NOBLOW, RESERVED, moNone, NODESC)
MONSTER( 'Y', 0x4040FF, "Yeti"       , moYeti, CF_FACE_UP, RESERVED, moYeti,
    "A big and quite intelligent monster living in the Icy Land."
  )
MONSTER( 'W', 0xD08040, "Icewolf"    , moWolf, CF_FACE_SIDE, RESERVED, moYeti,
    "A nasty predator from the Icy Land. Contrary to other monsters, "
    "it tracks its prey by their heat."
  )
MONSTER( 'W', 0xD08040, "Icewolf"    , moWolfMoved, CF_MOVED | CF_FACE_SIDE, RESERVED, moNone, "")
MONSTER( 'R', 0xFF8000, "Ranger"     , moRanger, CF_FACE_UP, RESERVED, moYeti,
    "Rangers take care of the magic mirrors in the Land of Mirrors. "
    "They know that rogues like to break these mirrors... so "
    "they will attack you!"
    )
MONSTER( 'T', 0xD0D0D0, "Rock Troll", moTroll, CF_FACE_UP | CF_TROLL | CF_HIGH_THREAT, RESERVED, moYeti, trollhelp)
MONSTER( 'G', 0x20D020, "Goblin", moGoblin, CF_FACE_UP, RESERVED, moYeti,     
    "A nasty creature native to the Living Caves. They don't like you "
    "for some reason."
    )

MONSTER( 'S', 0xE0E040, "Sand Worm", moWorm, ZERO | CF_NOGHOST | CF_NOBLOW | CF_MOUNTABLE | CF_MULTITILE | CF_WORM | CF_HIGH_THREAT, RESERVED, moNone, wormdes )
MONSTER( 's', 0x808000, "Sand Worm Tail", moWormtail, ZERO | CF_NOGHOST | CF_NOBLOW | CF_MOUNTABLE | CF_MULTITILE | CF_WORM | CF_INACTIVE | CF_SPAM, RESERVED, moNone, wormdes )
MONSTER( 'S', 0x808000, "Sand Worm W", moWormwait, ZERO | CF_NOGHOST | CF_NOBLOW | CF_MOUNTABLE | CF_MULTITILE | CF_WORM | CF_INACTIVE | CF_SPAM, RESERVED, moNone, wormdes )

MONSTER( 'H', 0x80FF00, "Hedgehog Warrior", moHedge, CF_FACE_UP, RESERVED, moYeti, 
    "These warriors of the Forest wield exotic weapons called hedgehog blades. "
    "These blades protect them from a frontal attack, but they still can be 'stabbed' "
    "easily by moving from one place next to them to another."
    )
MONSTER( 'M', 0x806050, "Desert Man", moDesertman, CF_FACE_UP, RESERVED, moYeti,
    "A tribe of men native to the Desert. They have even tamed the huge Sandworms, who won't attack them.")

MONSTER( 'C', 0x00FFFF, "Ivy Root", moIvyRoot, ZERO | CF_NOGHOST | CF_NOBLOW | CF_IGNORE_SMELL | CF_MULTITILE | CF_ANYIVY | CF_IVY | CF_INACTIVE | CF_SPAM, RESERVED, moNone, ivydes)
MONSTER( 'C', 0xFFFF00, "Active Ivy", moIvyHead, ZERO | CF_NOGHOST | CF_NOBLOW | CF_PART | CF_IGNORE_SMELL | CF_MULTITILE | CF_ANYIVY | CF_IVY | CF_SPAM, RESERVED, moNone, ivydes)
MONSTER( 'C', 0x40FF00, "Ivy Branch", moIvyBranch, ZERO | CF_NOGHOST | CF_NOBLOW | CF_PART | CF_IGNORE_SMELL | CF_MULTITILE | CF_ANYIVY | CF_IVY | CF_SPAM, RESERVED, moNone, ivydes)
MONSTER( 'C', 0x006030, "Dormant Ivy", moIvyWait, ZERO | CF_NOGHOST | CF_NOBLOW | CF_PART | CF_NOHIGHLIGHT | CF_IGNORE_SMELL | CF_MULTITILE | CF_ANYIVY | CF_IVY | CF_INACTIVE | CF_SPAM, RESERVED, moNone, ivydes)
MONSTER( 'C', 0x804000, "Dead Ivy", moIvyNext, ZERO | CF_NOGHOST | CF_NOBLOW | CF_PART | CF_NOHIGHLIGHT | CF_IGNORE_SMELL | CF_MULTITILE | CF_ANYIVY | CF_IVY | CF_INACTIVE | CF_SPAM, RESERVED, moNone, ivydes)
MONSTER( 'C', 0x800000, "Dead Ivy", moIvyDead, ZERO | CF_NOGHOST | CF_NOBLOW | CF_PART | CF_NOHIGHLIGHT | CF_IGNORE_SMELL | CF_MULTITILE | CF_ANYIVY | CF_IVY | CF_INACTIVE | CF_SPAM, RESERVED, moNone, ivydes)

MONSTER( 'M', 0x804000, "Giant Ape", moMonkey, CF_FACE_UP, RESERVED, moYeti,
    "This giant ape thinks that you are an enemy.")
MONSTER( 'B', 0x909000, "Slime Beast", moSlime, CF_FACE_UP | CF_NOBLOW | CF_SLIME | CF_IGNORE_SMELL, RESERVED, moSlime, slimehelp)
MONSTER( '@', 0xFF80FF, "Mimic", moMimic, CF_FACE_UP | CF_NOGHOST | CF_NOBLOW | CF_MIMIC | CF_NONLIVING | CF_FRIENDLY | CF_FACING, RESERVED, moNone,
    "A magical being which copies your movements. "
    "You feel that it would be much more useful in an Euclidean space."
    )
MONSTER( '@', 0xFF8080, "Mirage (REMOVED)", moREMOVED, ZERO, RESERVED, moNone,
    "A magical being which copies your movements. "
    )
MONSTER( '@', 0x509050, "Golem", moGolem, CF_FACE_UP | CF_NOGHOST | CF_GOK | CF_NONLIVING | CF_FRIENDLY | CF_FACING, RESERVED, moNone,
    "You can summon these friendly constructs with a magical process."
    )
MONSTER( '@', 0x509050, "Golem", moGolemMoved, CF_FACE_UP | CF_NOGHOST | CF_GOK | CF_NONLIVING | CF_FRIENDLY | CF_FACING | CF_MOVED, RESERVED, moNone,
    "You can summon these friendly constructs with a magical process."
    )
MONSTER( 'E', 0xD09050, "Eagle", moEagle, CF_FACE_SIDE | CF_BIRD | CF_FLYING | CF_IGNORE_PLATE | CF_HIGH_THREAT, RESERVED, moEagle,
    "A majestic bird, who is able to fly very fast."
    )
MONSTER( 'S', 0xFF8080, "Seep", moSeep, CF_FACE_SIDE | CF_NOBLOW | CF_SLIME | CF_IGNORE_SMELL, RESERVED, moSlime,
    "A monster who is able to live inside the living cave wall."
    )
MONSTER( 'Z', 0x804000, "Zombie", moZombie, CF_FACE_UP | CF_NONLIVING, RESERVED, moYeti,
    "A typical Graveyard monster."
    )
MONSTER( 'G', 0xFFFFFF, "Ghost", moGhost, CF_FACE_SIDE | CF_NOBLOW | CF_NONLIVING | CF_GHOST | CF_FLYING | CF_ATTACK_THRU_VINE | CF_ATTACK_NONADJACENT | CF_IGNORE_PLATE | CF_IGNORE_SMELL | CF_GHOSTMOVER, RESERVED, moGhost,
    "A typical monster from the Graveyard, who moves through walls.\n\n"
    "There are also wandering Ghosts. They will appear "
    "if you do not explore any new places for a long time (about 100 turns). "
    "They can appear anywhere in the game."
    )
MONSTER( 'N', 0x404040, "Necromancer", moNecromancer, CF_FACE_UP | CF_HIGH_THREAT, RESERVED, moYeti,
    "Necromancers can raise ghosts and zombies from fresh graves."
    )
MONSTER( 'S', 0x404040, "Shadow", moShadow, CF_FACE_UP | CF_NOBLOW | CF_NONLIVING | CF_SPAM, RESERVED, moNone,
    "A creepy monster who follows you everywhere in the Graveyard and the Cursed Canyon."
    )
MONSTER( 'T', 0x40E040, "Tentacle", moTentacle, ZERO | CF_NOGHOST | CF_NOBLOW | CF_MOUNTABLE | CF_MULTITILE | CF_WORM, RESERVED, moNone, tentdes )
MONSTER( 't', 0x008000, "Tentacle Tail", moTentacletail, ZERO | CF_NOGHOST | CF_NOBLOW | CF_MOUNTABLE | CF_MULTITILE | CF_WORM | CF_INACTIVE | CF_SPAM, RESERVED, moNone, tentdes )
MONSTER( 'T', 0x008000, "Tentacle W", moTentaclewait, ZERO | CF_NOGHOST | CF_NOBLOW | CF_MOUNTABLE | CF_MULTITILE | CF_WORM | CF_INACTIVE | CF_SPAM, RESERVED, moNone, tentdes )
MONSTER( 'z', 0xC00000, "Tentacle (withdrawing)", moTentacleEscaping, ZERO | CF_NOGHOST | CF_NOBLOW | CF_MOUNTABLE | CF_MULTITILE | CF_WORM | CF_SPAM, RESERVED, moNone, tentdes )

MONSTER( 'P', 0xFF8000, "Cultist", moCultist, CF_FACE_UP, RESERVED, moYeti, 
    "People worshipping Cthulhu. They are very dangerous." 
    )
MONSTER( 'P', 0xFFFF00, "Fire Cultist", moPyroCultist, CF_FACE_UP | CF_HIGH_THREAT, RESERVED, moYeti, 
    "People worshipping Cthulhu. This one is especially dangerous, "
    "as he is armed with a weapon which launches fire from afar."
    )
MONSTER( 'D', 0xFF0000, "Greater Demon", moGreater, CF_FACE_UP | CF_DEMON | CF_SLOWMOVER | CF_SPAM, RESERVED, moYeti, gdemonhelp)
MONSTER( 'D', 0x800000, "Greater Demon", moGreaterM, CF_FACE_UP | CF_DEMON | CF_INACTIVE | CF_SPAM, RESERVED, moLesser, gdemonhelp)
MONSTER( 'd', 0xFF2020, "Lesser Demon", moLesser, CF_FACE_UP | CF_DEMON | CF_SLOWMOVER | CF_SPAM, RESERVED, moYeti, ldemonhelp)
MONSTER( 'd', 0x802020, "Lesser Demon", moLesserM, CF_FACE_UP | CF_DEMON | CF_INACTIVE | CF_SPAM, RESERVED, moLesser, ldemonhelp)
MONSTER( 'S', 0x2070C0, "Ice Shark", moShark, CF_FACE_SIDE | CF_NOBLOW | CF_SHARK, RESERVED, moShark, 
    "This dangerous predator has killed many people, and has been sent to Cocytus."
    )
MONSTER( 'W', 0xFFFFFF, "Running Dog", moRunDog, CF_FACE_SIDE, RESERVED, moYeti, 
    "This white dog is able to run all the time. It is the only creature "
    "able to survive and breed in the Land of Eternal Motion."
    )
MONSTER( 'S', 0xC00040, "Demon Shark", moGreaterShark, CF_FACE_SIDE | CF_NOBLOW | CF_SHARK | CF_IGNORE_SMELL | CF_GHOSTMOVER, RESERVED, moGreaterShark, 
    "Demons of Hell do not drown when they fall into the lake in Cocytus. "
    "They turn into demonic sharks, enveloped in a cloud of steam."
    )
MONSTER( 'S', 0xC00040, "Fire Fairy", moFireFairy, CF_FACE_UP | CF_HIGH_THREAT, RESERVED, moYeti, 
    "These fairies would rather burn the forest, than let you get some Fern Flowers. "
    "The forest is infinite, after all...\n\n"
    "Fire Fairies transform into fires when they die."
    )
MONSTER( 'C', 0x4000C0, "Crystal Sage", moCrystalSage, CF_FACE_UP | CF_UNARMED | CF_HIGH_THREAT, RESERVED, moYeti, 
    "This being radiates an aura of wisdom. "
    "It is made of a beautiful crystal, you would love to take it home. "
    "But how is it going to defend itself? Better not to think of it, "
    "thinking causes your brain to go hot...\n\n"
    "Crystal Sages melt at -30 °C, and they can raise the temperature around you from afar."
    )
MONSTER( 'P', 0x4040C0, "Pikeman", moLancer, CF_FACE_UP | CF_HIGH_THREAT, RESERVED, moYeti, 
    "When Pikemen move, they attack all cells which are now adjacent to them. "
    "Luckily, they can be killed in the same way.\n\n"
    "They never move if this would attack their friends."
    )
MONSTER( 'F', 0xC04040, "Flail Guard", moFlailer, CF_FACE_UP, RESERVED, moYeti, 
    "This guard of the Emerald Mine is wielding a huge flail. "
    "You cannot attack him directly, as the flail would still hit you then. "
    "Luckily, you have learned a trick: if you step away from him, "
    "he will hit himself with the flail!"
    )
MONSTER( 'M', 0x404040, "Miner", moMiner, CF_FACE_UP, RESERVED, moYeti, 
    "Miners have special tools for dealing with the Living Cave. "
    "When they die, these tools activate, destroying the living cave "
    "around them."
    )
MONSTER( 'V', 0x421C52, "Vine Beast", moVineBeast, CF_FACE_SIDE, RESERVED, moYeti, 
    "A beast made of vines!\n\n"
    "Vine Beasts turn into vines when they die."
    )
MONSTER( 'V', 0xFFC0C0, "Vine Spirit", moVineSpirit, CF_FACE_SIDE | CF_NOBLOW | CF_SLIME | CF_ATTACK_THRU_VINE | CF_IGNORE_SMELL, RESERVED, moSlime, 
    "A spirit living in the vines!\n\n"
    "Vine Spirits destroy the vines when they die."
    )
MONSTER( 'T', 0x803030, "Dark Troll", moDarkTroll, CF_FACE_UP | CF_TROLL, RESERVED, moYeti, 
    "A Troll without the power of Life."
    )
MONSTER( 'E', 0xFFFF40, "Earth Elemental", moEarthElemental, CF_FACE_UP | CF_NONLIVING | CF_HIGH_THREAT, RESERVED, moEarthElemental, 
    "A rare unliving construct from the Dead Caves. "
    "It instantly destroys cave walls next to its path, and also leaves "
    "an impassable wall behind it. You suppose that this impassable wall helps it to "
    "escape from some threats. You hope you won't meet these threats..."
    )
MONSTER( 'B', 0xC04040, "Red Hyperbug", moBug0, CF_FACE_SIDE | CF_BUG | CF_FACING | CF_SPAM, RESERVED, moBug0, hivehelp)
MONSTER( 'B', 0x40C040, "Green Hyperbug", moBug1, CF_FACE_SIDE | CF_BUG | CF_FACING | CF_SPAM, RESERVED, moBug1, hivehelp)
MONSTER( 'B', 0x4040C0, "Blue Hyperbug", moBug2, CF_FACE_SIDE | CF_BUG | CF_FACING | CF_SPAM, RESERVED, moBug2, hivehelp)
MONSTER( 'W', 0x404040, "Witch Apprentice", moWitch, CF_FACE_UP | CF_WITCH | CF_POWER, RESERVED, moYeti, 
    "A Witch without any special powers. But watch out! She will "
    "pick up any basic Orbs on her path, and use their powers."
    )
MONSTER( 'W', 0xFF4040, "Speed Witch", moWitchSpeed, CF_FACE_UP | CF_WITCH | CF_POWER, RESERVED, moYeti, 
    "A Witch with a Speed spell. She moves twice as fast as you. Unless you "
    "have an Orb of Speed too, of course!"
    )
MONSTER( 'W', 0xFFFFFF, "Flash Witch", moWitchFlash, CF_FACE_UP | CF_WITCH | CF_POWER, RESERVED, moYeti, 
    "A Witch with a Flash spell. Very dangerous!\n\n"
    "Luckily, she never uses the spell if it would kill her friends. "
    "She could destroy an Evil Golem, though."
    )
MONSTER( 'W', 0xFF8000, "Fire Witch", moWitchFire, CF_FACE_UP | CF_WITCH | CF_POWER, RESERVED, moYeti, 
    "A Witch with a Fire spell. She will leave a trail of fire behind her."
    )
MONSTER( 'W', 0x8080FF, "Winter Witch", moWitchWinter, CF_FACE_UP | CF_WITCH | CF_IGNORE_SMELL | CF_GHOSTMOVER | CF_POWER, RESERVED, moWitchWinter, 
    "A Witch with a Winter spell. She is able to move through fire."
    )
MONSTER( 'W', 0x808080, "Aether Witch", moWitchGhost, CF_FACE_UP | CF_NOBLOW | CF_WITCH | CF_IGNORE_SMELL | CF_GHOSTMOVER | CF_POWER, RESERVED, moWitchGhost, 
    "A Witch with an Aether spell. She is able to move through fire and walls."
    )
MONSTER( '@', 0x905050, "Evil Golem", moEvilGolem, CF_FACE_UP | CF_NONLIVING | CF_POWER, RESERVED, moYeti,
    "Somebody has summoned these evil constructs with a magical process."
    )
MONSTER( '@', 0x8080FF, "Knight", moKnight, CF_FACE_UP | CF_NOGHOST | CF_GOK | CF_FRIENDLY | CF_FACING, RESERVED, moNone, camelothelp )
MONSTER( 'P', 0xD10000, "Cult Leader", moCultistLeader, CF_FACE_UP | CF_LEADER | CF_HIGH_THREAT, RESERVED, moPirate, 
    "These Cultists can push the statues, just like you."
    )
MONSTER( 'B', 0x909000, "Slime Beast", moSlimeNextTurn, CF_FACE_UP, RESERVED, moNone, slimehelp)
MONSTER( '@', 0x8080FF, "Knight", moKnightMoved, CF_FACE_SIDE | CF_NOGHOST | CF_GOK | CF_FRIENDLY | CF_FACING | CF_MOVED, RESERVED, moNone, camelothelp )
MONSTER( '@', 0x8B4513, "Illusion", moIllusion, CF_FACE_SIDE | CF_NOGHOST | CF_NONLIVING | CF_FRIENDLY | CF_FACING | CF_IGNORE_PLATE, RESERVED, moNone, 
    "Illusions are targeted "
    "by most monsters, just like yourself, Thumpers, and your friends."
    )
MONSTER( 'P', 0xD00000, "Pirate", moPirate, CF_FACE_UP | CF_LEADER, RESERVED, moPirate, 
    "Just a typical hyperbolic pirate." )
MONSTER( 'S', 0x8080C0, "Shark", moCShark, CF_FACE_SIDE | CF_NOBLOW | CF_SHARK, RESERVED, moShark, "Just a nasty shark.")
MONSTER( 'P', 0x0000FF, "Parrot", moParrot, CF_FACE_SIDE | CF_NOBLOW | CF_SLIME | CF_IGNORE_SMELL, RESERVED, moSlime, "Parrots feel safe in the forests of Caribbean, so they "
    "never leave them. But they will help the Pirates by attacking the intruders.")
MONSTER( 'S', 0xE09000, "Rock Snake", moHexSnake, ZERO | CF_NOGHOST | CF_NOBLOW | CF_MOUNTABLE | CF_IGNORE_SMELL | CF_MULTITILE | CF_WORM | CF_HIGH_THREAT, RESERVED, moNone, redsnakedes )
MONSTER( 's', 0xE09000, "Rock Snake Tail", moHexSnakeTail, ZERO | CF_NOGHOST | CF_NOBLOW | CF_MOUNTABLE | CF_MULTITILE | CF_WORM | CF_INACTIVE | CF_SPAM, RESERVED, moNone, redsnakedes )
MONSTER( 'T', 0xC02020, "Red Troll", moRedTroll, CF_FACE_UP | CF_TROLL, RESERVED, moYeti, "A kind of Troll native to the Red Rock Valley.")
MONSTER( 'B', 0xA00000, "Bomberbird", moBomberbird, CF_FACE_SIDE | CF_BIRD | CF_FLYING | CF_IGNORE_PLATE, RESERVED, moEagle, 
    "Dark red birds who have created the minefield.\n\n"
    "They create a mine on the spot where they are killed, provided "
    "that the terrain is suitable. Also note that mines are triggered "
    "by dead birds falling on them."
    )
MONSTER( 'A', 0xFFFFFF, "Albatross", moAlbatross, CF_FACE_SIDE | CF_BIRD | CF_FLYING | CF_IGNORE_PLATE, RESERVED, moEagle, 
    "Those large seabirds attack you for some reason. At least they are not "
    "as fast as Eagles..."
    )
MONSTER( 'B', 0x40C000, "Tame Bomberbird", moTameBomberbird, CF_FACE_SIDE | CF_NOGHOST | CF_GOK | CF_FRIENDLY | CF_FACING | CF_BIRD | CF_FLYING | CF_IGNORE_PLATE, RESERVED, moTameBomberbird, tamebomberdesc)
MONSTER( 'B', 0x40C000, "Tame Bomberbird", moTameBomberbirdMoved, CF_FACE_SIDE | CF_NOGHOST | CF_GOK | CF_FRIENDLY | CF_FACING | CF_BIRD | CF_FLYING | CF_IGNORE_PLATE | CF_MOVED, RESERVED, moTameBomberbird, tamebomberdesc)
MONSTER( 'G', 0xFF0000, "Palace Guard", moPalace, CF_FACE_UP | CF_STUNNABLE | CF_HP, RESERVED, moYeti, 
     "You have to hit Palace Guards several times to kill them. After each hit, they "
     "are pushed away and stunned for some time."
     )
MONSTER( 'G', 0xC06000, "Fat Guard", moFatGuard, CF_FACE_UP | CF_STUNNABLE | CF_HP, RESERVED, moYeti, "Fat guards are too heavy to be pushed away by your hits.")
MONSTER( 'G', 0xFFFFFF, "Skeleton", moSkeleton, CF_FACE_UP | CF_NONLIVING | CF_STUNNABLE | CF_HIGH_THREAT, RESERVED, moYeti, 
    "Skeletons work similar to Palace Guards, but they won't die no matter how many "
    "times you hit them. Well, you have to be more creative...\n\n"
    "Skeletons attacked outside of their native land, Palace, are stunned for a longer time."
    )
MONSTER( 'G', 0xC000C0, "Vizier", moVizier, CF_FACE_UP | CF_STUNNABLE | CF_HP | CF_HIGH_THREAT, RESERVED, moYeti, 
    "Viziers are neither pushed away nor stunned. However, you attack them automatically when "
    "escaping from them.")
MONSTER( 'V', 0xC0C0C0, "Viking", moViking, CF_FACE_UP | CF_LEADER, RESERVED, moPirate, "Mighty warriors from the Fjord, who can use boats.")
MONSTER( 'T', 0x00FFFF, "Fjord Troll", moFjordTroll, CF_FACE_UP | CF_TROLL, RESERVED, moYeti, 
    "Fjord Trolls leave a wall when they die, causing the living fjord to rise around it. "
    "Contrary to Rock Trolls, items around are not destroyed."
    )
MONSTER( 'E', 0x0000FF, "Water Elemental", moWaterElemental, CF_FACE_UP | CF_NOBLOW | CF_NONLIVING | CF_HIGH_THREAT, RESERVED, moWaterElemental, 
    "Wherever this powerful being goes, the living fjord "
    "sinks below water, non-magical boats are destroyed, and fires are extinguished.\n\n"
    "As a special case, you can attack the Water Elemental from the water, without drowning immediately."
    )
MONSTER( 'M', 0xD0D0D0, "Mouse", moMouse, CF_FACE_SIDE | CF_NOGHOST | CF_GOK | CF_FRIENDLY | CF_FACING | CF_UNARMED | CF_IGNORE_PLATE, RESERVED, moNone, princessdesc)
MONSTER( 'M', 0xD0D0D0, "Mouse", moMouseMoved, CF_FACE_SIDE | CF_NOGHOST | CF_GOK | CF_FRIENDLY | CF_FACING | CF_UNARMED | CF_MOVED, RESERVED, moNone, princessdesc)
MONSTER( 'P', 0xFF80FF, "Prince", moPrincess, CF_FACE_UP | CF_NOGHOST | CF_PRINCESS | CF_GOK | CF_STUNNABLE | CF_HP | CF_FRIENDLY | CF_FACING | CF_UNARMED, RESERVED, moNone, princessdesc)
MONSTER( 'P', 0xFF80FF, "Princess", moPrincessMoved, CF_FACE_UP | CF_NOGHOST | CF_PRINCESS | CF_GOK | CF_STUNNABLE | CF_HP | CF_FRIENDLY | CF_FACING | CF_UNARMED | CF_MOVED, RESERVED, moNone, princessdesc)
MONSTER( 'P', 0xFF80FF, "Prince", moPrincessArmed, CF_FACE_UP | CF_NOGHOST | CF_PRINCESS | CF_GOK | CF_STUNNABLE | CF_HP | CF_FRIENDLY | CF_FACING, RESERVED, moNone, princessdesc)
MONSTER( 'P', 0xFF80FF, "Princess", moPrincessArmedMoved, CF_FACE_UP | CF_NOGHOST | CF_PRINCESS | CF_GOK | CF_STUNNABLE | CF_HP | CF_FRIENDLY | CF_FACING | CF_MOVED, RESERVED, moNone, princessdesc)
MONSTER( 'F', 0xD03000, "Familiar", moFamiliar, CF_FACE_SIDE, RESERVED, moYeti, "A simple servant of the master of the Ivory Tower.")
MONSTER( 'B', 0x707070, "Gargoyle", moGargoyle, CF_FACE_SIDE | CF_BIRD | CF_FLYING | CF_IGNORE_PLATE, RESERVED, moEagle, gargdesc)
MONSTER( 'E', 0xFF0000, "Fire Elemental", moFireElemental, CF_FACE_UP, RESERVED, moYeti, 
    "This monster leaves a trail of fire behind.")
MONSTER( 'E', 0xC0C0FF, "Air Elemental", moAirElemental, CF_FACE_UP | CF_FLYING | CF_IGNORE_PLATE | CF_HIGH_THREAT, RESERVED, moAirElemental, "An Air Elemental looks like a live tornado. Once you are three (or less) cells from it, "
  "it is impossible to move closer, due to strong winds. You can stand and wait, though. "
  "This also affects most monsters.")
MONSTER( 'D', 0xC06000, "Striped Dog", moOrangeDog, CF_FACE_SIDE, RESERVED, moYeti, "A predator native to the Zebra.")
MONSTER( 'G', 0xFFFFFF, "Tentacle+Ghost", moTentacleGhost, CF_FACE_SIDE | CF_NOGHOST | CF_NOBLOW | CF_GHOST | CF_FLYING | CF_ATTACK_THRU_VINE | CF_ATTACK_NONADJACENT | CF_IGNORE_PLATE | CF_IGNORE_SMELL | CF_MULTITILE | CF_WORM | CF_GHOSTMOVER, RESERVED, moNone, tentdes )
MONSTER( 'B', 0x8080C0, "Metal Beast", moMetalBeast, CF_FACE_SIDE | CF_METAL | CF_STUNNABLE | CF_SLOWMOVER, RESERVED, moYeti, elecdesc )
MONSTER( 'B', 0xC060C0, "Rich Metal Beast", moMetalBeast2, CF_FACE_SIDE | CF_METAL | CF_STUNNABLE | CF_SLOWMOVER, RESERVED, moYeti, elecdesc )
MONSTER( 'O', 0xA06020, "Outlaw", moOutlaw, CF_FACE_UP | CF_HIGH_THREAT, RESERVED, moYeti, wildwestdesc )
MONSTER( 'C', 0xC0C060, "Mutant Ivy", moMutant, CF_NOGHOST | CF_NOBLOW | CF_PART | CF_IGNORE_SMELL | CF_MULTITILE | CF_ANYIVY | CF_MUTANTIVY | CF_SPAM, RESERVED, moNone, overdesc )
MONSTER( 'T', 0x0080FF, "Storm Troll", moStormTroll, CF_FACE_UP | CF_TROLL, RESERVED, moYeti, elecdesc )
MONSTER( 'T', 0x00C080, "Forest Troll", moForestTroll, CF_FACE_UP | CF_TROLL, RESERVED, moYeti, 
    "Forest Trolls create an impassable wall when they die."
    )
MONSTER( 'F', 0xC35817, "Giant Fox", moRedFox, CF_FACE_SIDE | CF_HIGH_THREAT, RESERVED, moYeti, 
    "What is freedom for you? A situation when you can walk wherever you want? "
    "Or a situation when you do not have to work, since you have as much tasty food "
    "as you want?\n\n"
    "Well, this creature has chosen the second option. It won't be happy "
    "if you destroy its prison.\n"
    )
MONSTER( 'C', 0x8080FF, "Wind Crow", moWindCrow, CF_FACE_SIDE | CF_BIRD | CF_FLYING | CF_IGNORE_PLATE | CF_HIGH_THREAT, RESERVED, moEagle,
    "A large bird who likes strong winds. Just as you, it can fly quickly in the wind."
    )
MONSTER( 'G', 0xC0FFC0, "Friendly Ghost", moFriendlyGhost, CF_FACE_SIDE | CF_NOGHOST | CF_GOK | CF_FRIENDLY | CF_FACING | CF_GHOST | CF_FLYING | CF_ATTACK_THRU_VINE | CF_ATTACK_NONADJACENT | CF_IGNORE_PLATE, RESERVED, moFriendlyGhost, 
    "Friendly ghosts are friendly beings who can go through any obstacles. However, "
    "unlike most friends, they tend to fly away from you."
    )
MONSTER( 'R', 0x906030, "Ratling", moRatling, CF_FACE_UP | CF_LEADER | CF_RATLING, RESERVED, moPirate, 
    "These warped humanoids are skilled warriors and sailors, and they "
    "feel at home at the Warped Coast. Their battle experience has taught them "
    "that enemies who wait without moving or attacking anything are the most deadly. "
    "If they see such an enemy, they become extremely suspicious, and they also wait."
    )
MONSTER( 'F', 0xC00000, "False Princess", moFalsePrincess, CF_FACE_UP, RESERVED, moYeti, GENDERSWITCH )
MONSTER( 'R', 0x500050, "Rose Lady", moRoseLady, CF_FACE_UP | CF_IGNORE_SMELL, RESERVED, moYeti, GENDERSWITCH )
MONSTER( 'R', 0xF0A0D0, "Rose Beauty", moRoseBeauty, CF_FACE_UP | CF_HIGH_THREAT, RESERVED, moYeti, GENDERSWITCH )
MONSTER( 'R', 0x806040, "Ratling Avenger", moRatlingAvenger, CF_FACE_UP | CF_LEADER | CF_RATLING, RESERVED, moPirate, 
    "So, you have killed a Ratling on the unwarped sea? You will be punished for this! "
    "Luckily, if you run away from the Warped Sea quickly, the Ratling Avengers will lose track of you."
    )
MONSTER( 'T', 0x487830, "Tortoise", moTortoise, CF_FACE_SIDE | CF_STUNNABLE | CF_SLOWMOVER | CF_SPAM, RESERVED, moYeti, tortoisedesc)
MONSTER( 'D', 0xC03000, "Dragon", moDragonHead, ZERO | CF_NOGHOST | CF_NOBLOW | CF_STUNNABLE | CF_MOUNTABLE | CF_FLYING | CF_IGNORE_PLATE | CF_MULTITILE | CF_DRAGON | CF_WORM, RESERVED, moDragonHead, dragondesc)
MONSTER( 'd', 0xC03000, "Dragon", moDragonTail, ZERO | CF_NOGHOST | CF_NOBLOW | CF_STUNNABLE | CF_MOUNTABLE | CF_PART | CF_FLYING | CF_IGNORE_PLATE | CF_MULTITILE | CF_DRAGON | CF_WORM | CF_SPAM, RESERVED, moDragonHead, dragondesc)
MONSTER( 'F', 0x909090, "Gadfly", moGadfly, CF_FACE_SIDE | CF_BIRD | CF_FLYING | CF_IGNORE_PLATE, RESERVED, moEagle, "Annoying insects. They can awaken Sleeping Bulls.")
MONSTER( 'Y', 0xFF8000, "Yendorian Researcher", moResearcher, ZERO, RESERVED, moYeti, 
    "These people study gravity and infinite trees. "
    "They have no special features, other than wearing a strange hat."
    )
MONSTER( 'K', 0xA8A8A8, "Sparrowhawk", moSparrowhawk, CF_FACE_SIDE | CF_BIRD | CF_FLYING | CF_IGNORE_PLATE, RESERVED, moEagle, 
    "A bird who hunts in the treetops of the Yendorian Forest."
    )
MONSTER( 'K', 0xD0A0A0, "Kraken", moKrakenH, ZERO | CF_NOGHOST | CF_NOBLOW | CF_MULTITILE | CF_KRAKEN | CF_FACING | CF_HIGH_THREAT, RESERVED, moNone, krakendesc)
MONSTER( 'K', 0xC07070, "Kraken Tentacle", moKrakenT, ZERO | CF_NOGHOST | CF_NOBLOW | CF_PART | CF_MULTITILE | CF_KRAKEN | CF_SPAM, RESERVED, moNone, krakendesc)
MONSTER( 'D', 0xF09090, "Draugr", moDraugr, CF_FACE_UP | CF_NONLIVING | CF_SLOWMOVER, RESERVED, moYeti,
    "Animated corpses of ancient Viking warriors. They are immune to mundane weapons, "
    "but they can be destroyed by your Orb of the Sword."
    )
MONSTER( 'C', 0xC08000, "Friendly Ivy", moFriendlyIvy, ZERO | CF_NOGHOST | CF_NOBLOW | CF_MULTITILE | CF_FRIENDLY | CF_FACING | CF_ANYIVY, RESERVED, moNone, naturedesc )
MONSTER( 'V', 0xC000C0, "Vampire Bat", moVampire, CF_FACE_SIDE | CF_BIRD | CF_FLYING | CF_UNARMED | CF_IGNORE_PLATE, RESERVED, moEagle, 
    "Vampire Bats don't attack normally, but they drain your magical powers if "
    "they are at distance at most 2 from you."
    )
MONSTER( 'B', 0x404040, "Bat", moBat, CF_FACE_SIDE | CF_BIRD | CF_FLYING | CF_UNARMED | CF_IGNORE_PLATE | CF_SPAM, RESERVED, moEagle, 
    "Someone has told you that one can get battle experience safely by "
    "killing tons of essentially harmless creatures, such as Bats. But "
    "does this make any sense?...\n\n"  
    "It does not. Bats cannot hurt you, but may block your movement, or "
    "toggle switches if they fall on them." )
MONSTER( 'R', 0x8080C0, "Reptile", moReptile, CF_FACE_SIDE | CF_STUNNABLE, RESERVED, moReptile, reptiledesc )
MONSTER( 'B', 0x606020, "Herd Bull", moHerdBull, CF_FACE_SIDE | CF_BULL | CF_FACING | CF_SPAM, RESERVED, moRagingBull, 
    "Herds of these Bulls are running long distances for some reason. They become Raging Bulls if something stops them." )
MONSTER( 'B', 0xA03000, "Raging Bull", moRagingBull, CF_FACE_SIDE | CF_BULL | CF_FACING, RESERVED, moYeti, 
    "Raging Bulls charge in a straight line: on heptagons, when they can choose one of two possible directions, "
    "they choose one closer to your current location. In the case of a tie, the cell where more neighbors is "
    "closer to your current location is chosen; if still a tie, past locations are considered. "
    "They can attack you in any direction, and monsters on their way are attacked even if friendly. "
    "When they crash into something, the obstacle is usually destroyed, and they are stunned for three turns, after "
    "which they charge at you again (in any direction). "
    "Raging Bulls cannot be killed or stunned conventionally."
    )
MONSTER( 'B', 0xB07000, "Sleeping Bull", moSleepBull, CF_FACE_SIDE | CF_BULL | CF_FACING | CF_SPAM, RESERVED, moRagingBull, 
    "Sleeping bulls wake up when you get into distance of two cells from them."
     )
MONSTER( 'S', 0xFFD500, "Butterfly", moButterfly, CF_FACE_SIDE | CF_BIRD | CF_FLYING | CF_IGNORE_PLATE | CF_FACING | CF_SPAM, RESERVED, moButterfly, bulldashdesc)
MONSTER( 'N', 0xFFFF80, "Narcissist", moNarciss, CF_FACE_UP, RESERVED, moYeti, 
    "This person loves to look at their own reflection in the mirror. "
    "He believes himself to be one of the most important creatures in this world, "
    "and hates those who do not admire him."
    )
MONSTER( 'M', 0xFFC0FF, "Mirror Spirit", moMirrorSpirit, CF_FACE_UP | CF_HIGH_THREAT, RESERVED, moYeti, 
    "A long time ago a mighty warrior was guarding the mirrors from being broken. "
    "While this warrior is no longer alive, his reflections have gained life of "
    "their own, and will punish the intruders.\n\n"
    "If you attack a Mirror Spirit physically, it is delayed, but not destroyed -- "
    "more reflections will come out of the mirror. Use Mimics to destroy them."
    )
MONSTER( 'W', 0x202020, "Hunting Dog", moHunterDog, CF_FACE_SIDE, RESERVED, moYeti, huntingdesc)
MONSTER( 'T', 0xE2725B, "Terracotta Warrior", moTerraWarrior, CF_FACE_UP | CF_NONLIVING | CF_STUNNABLE | CF_HP, RESERVED, moYeti, terradesc)
MONSTER( 'J', 0x50A030, "Jiangshi", moJiangshi, CF_FACE_UP | CF_NONLIVING, RESERVED, moYeti, 
    "You think this was one of the people who have constructed the Terracotta Warriors and the arrow traps. "
    "They have been locked inside, so that they will never divulge the secrets of the mausoleum. They would like to return their homes and families, though."
    )
MONSTER( 'B', 0xA00000, "Void Beast", moVoidBeast, CF_FACE_UP | CF_NONLIVING, RESERVED, moVoidBeast, 
    "Are creatures of Void actual monsters, or just monster-shaped holes in the air?\n\nVoid Beasts move simply by letting the air move into their current location -- "
    "the hole will move to the place where the air came from! Void Beasts are great at moving against the wind, but they have problems "
    "moving with the wind.")
MONSTER( 'W', 0xA00000, "Lava Wolf", moLavaWolf, CF_FACE_SIDE, RESERVED, moYeti, 
    "While Ice Wolves love heat, their instincts usually will not let them leave the Icy Lands -- "
    "they are afraid that they will be unable to get back home, and that they will lose track of their prey. "
    "However, they find the Volcanic Wasteland so hot and attractive that they abandon their natural instincts... "
    "and try to track their prey using their other senses and intelligence."
     )
MONSTER( 'W', 0x202020, "Hunting Dog (guarding)", moHunterGuard, CF_FACE_SIDE | CF_SPAM, RESERVED, moYeti, huntingdesc)
MONSTER( 'G', 0xC0C0FF, "Ice Golem", moIceGolem, CF_FACE_UP | CF_NONLIVING, RESERVED, moYeti, 
    "The Ice Golems are powered by intense magical coldness. When destroyed in the Blizzard or another icy land, they become "
    "ice walls, and freeze the land around them.")
MONSTER( 'B', 0xC0C0FF, "Sand Bird", moSandBird, CF_FACE_SIDE, RESERVED, moNone, NODESC)
MONSTER( 'S', 0xA00000, "Salamander", moSalamander, CF_FACE_SIDE | CF_STUNNABLE, RESERVED, moYeti, 
    "Salamanders are tough lizard-like creatures. Their tough skin protects them "
    "from both physical attacks and heat. Salamanders "
    "are stunned for a longer time if you push them into lava, fire, or a solid obstacle.")
MONSTER( 'W', 0x202020, "Hunting Dog (regrouping)", moHunterChanging, CF_SPAM, RESERVED, moYeti, 
    "When your plan has clearly failed, it is better to abandon it and go to a safe place, to have a chance of succeeding next time. This dog clearly knows this.")
MONSTER( 'B', 0xC00000, "North Pole", moNorthPole, ZERO | CF_MAGNETIC | CF_FACING, RESERVED, moYeti, NODESCYET)
MONSTER( 'B', 0x0000C0, "South Pole", moSouthPole, ZERO | CF_MAGNETIC | CF_FACING, RESERVED, moYeti, NODESCYET)
MONSTER( 'P', 0xC03000, "Red Raider", moPair, CF_FACE_UP | CF_RAIDER | CF_FACING, RESERVED, moYeti, "Red Raiders travel in pairs. They have promised to always watch each other's backs. They are able to destroy walls on their way.")
MONSTER( 'H', 0xC0C0C0, "Gray Raider", moHexDemon, CF_FACE_UP | CF_RAIDER, RESERVED, moHexDemon, "Gray Raiders never step on gray cells.")
MONSTER( 'A', 0x80B080, "Green Raider", moAltDemon, CF_FACE_UP | CF_RAIDER, RESERVED, moAltDemon, "Green Raiders never step from one green cell to another.")
MONSTER( 'M', 0x904000, "Brown Raider", moMonk, CF_FACE_UP | CF_RAIDER, RESERVED, moMonk, "Brown Raiders never move adjacent to an item.")
MONSTER( 'C', 0x0060E0, "Blue Raider", moCrusher, CF_FACE_UP | CF_RAIDER, RESERVED, moYeti, "Blue Raiders have a powerful attack which takes two turns to complete, and also makes the Blue Raider stunned "
  "for a long time. This attack can destroy other Raiders if it hits them.")
MONSTER( '@', 0xC00000, "Red Jelly", moSwitch1, CF_FACE_SIDE | CF_SWITCH | CF_FACING, RESERVED, moYeti, jellydesc)
MONSTER( '@', 0x0000C0, "Blue Jelly", moSwitch2, CF_FACE_SIDE | CF_SWITCH | CF_FACING, RESERVED, moYeti, jellydesc)
MONSTER( 'B', 0xE07000, "Bronze Beast", moBrownBug, CF_FACE_SIDE | CF_STUNNABLE, RESERVED, moYeti, 
    "A large bug native to the Brown Islands. Cannot be killed easily due to their tough armor; still, they can be killed by pushing them into water or from great heights. "
    "Bronze Beasts are very bad at descending slopes -- it takes them extra time to get down, and they are stunned for a longer time if you push them down."
    )
MONSTER( 'B', 0xE07060, "Acid Gull", moAcidBird, CF_FACE_SIDE | CF_BIRD | CF_FLYING | CF_IGNORE_PLATE | CF_HIGH_THREAT, RESERVED, moEagle, 
    "Where did this strange bird come from?...\n\n"
    "Acid Gulls dissolve the land on which they fall when they die. "
    )
MONSTER( 'W', 0xA04060, "Mutant", moVariantWarrior, CF_FACE_UP, RESERVED, moYeti, 
    "These guys look a bit strange, but they have no special properties."
    )
MONSTER( 'W', 0x707080, "Falling Dog", moFallingDog, CF_FACE_SIDE, RESERVED, moYeti, 
    "Distant relatives of the Running Dogs.")
MONSTER( 'B', 0xC0C040, "Western Hawk", moWestHawk, CF_FACE_SIDE | CF_BIRD | CF_FLYING | CF_IGNORE_PLATE, RESERVED, moEagle, 
    "Some readers misinterpreted the early maps of Free Fall, thinking that it is a land to the west from some wall. "
    "The name Western Hawks remained." )

ITEM( 0,   0,        "no item", itNone, -1, ZERO | IF_FIREPROOF, RESERVED, osNone,       NODESC)
ITEM( '*', 0xFFFFFF, "Ice Diamond", itDiamond, IC_TREASURE, ZERO, RESERVED, osNone, 
    "Cold white gems, found in the Icy Land."
    )
ITEM( '$', 0xFFD700, "Gold", itGold, IC_TREASURE, ZERO, RESERVED, osNone,
    "An expensive metal from the Living Caves. For some reason "
    "gold prevents the living walls from growing close to it."
    )
ITEM( ';', 0xFF4000, "Spice", itSpice, IC_TREASURE, ZERO, RESERVED, osNone,      
    "A rare and expensive substance found in the Desert. "
    "It is believed to extend life and raise special psychic powers."
    )
ITEM( '*', 0xC00000, "Ruby", itRuby, IC_TREASURE, ZERO, RESERVED, osNone,
    "A beautiful gem from the Jungle."
    )
ITEM( '!', 0xFFFF00, "Elixir of Life", itElixir, IC_TREASURE, ZERO, RESERVED, osNone,
    "A wonderful beverage, apparently obtained by mixing red and blue slime. You definitely feel more "
    "healthy after drinking it, but you still feel that one hit of a monster is enough to kill you.")
ITEM( '%', 0xFF00FF, "Shard", itShard, IC_TREASURE, ZERO, RESERVED, osNone,
    "A piece of a magic mirror, or a mirage cloud, that can be used for magical purposes. Only mirrors and clouds "
    "in the Land of Mirrors leave these.")
ITEM( '/', 0xFF8000, "Necromancer's Totem", itBone, IC_TREASURE, ZERO, RESERVED, osNone,
    "These sinister totems contain valuable gems.")
ITEM( '%', 0x00D000, "Demon Daisy", itHell, IC_TREASURE, ZERO, RESERVED, osNone,
    "These star-shaped flowers native to Hell are a valuable alchemical component.")
ITEM( '/', 0x00FF00, "Statue of Cthulhu", itStatue, IC_TREASURE, ZERO, RESERVED, osNone,
    "This statue is made of materials which cannot be found in your world.")
ITEM( '*', 0xFF8000, "Phoenix Feather", itFeather, IC_TREASURE, ZERO, RESERVED, osNone, 
    "One of few things that does not cause the floor in the Land of Eternal Motion to collapse. Obviously they are quite valuable."
    )
ITEM( '*', 0x8080FF, "Ice Sapphire", itSapphire, IC_TREASURE, ZERO, RESERVED, osNone, 
    "Cold blue gems, found in the Cocytus."
    )
ITEM( '*', 0xEEFF20, "Hyperstone", itHyperstone, IC_TREASURE, ZERO, RESERVED, osNone, 
    "These bright yellow gems can be found only by those who have mastered the Crossroads."
    )
ITEM( '[', 0x8080FF, "Key", itKey, IC_OTHER, ZERO, RESERVED, osNone, 
    "That's all you need to unlock the Orb of Yendor! Well... as long as you are able to return to the Orb that this key unlocks...\n\n"
    "Each key unlocks only the Orb of Yendor which led you to it."
    )
ITEM( 'o', 0x306030, "Dead Orb", itGreenStone, IC_OTHER, ZERO, RESERVED, osNone,
    "These orbs can be found in the Graveyard. You think that they were once powerful magical orbs, but by now, their "
    "power is long gone. No way to use them, you could as well simply drop them...\n\n"
    )
ITEM( 'o', 0xFF20FF, "Orb of Yendor", itOrbYendor, IC_OTHER, ZERO, RESERVED, osNone,
    "This wonderful Orb can only be collected by those who have truly mastered this hyperbolic universe, "
    "as you need the right key to unlock it. Luckily, your psychic abilities will let you know "
    "where the key is after you touch the Orb." )
ITEM( 'o', 0xFFFF00, "Orb of Storms", itOrbLightning, IC_ORB, ZERO, RESERVED, osOffensive,
    "This orb can be used to invoke the lightning spell, which causes lightning bolts to shoot from you in all directions.")
ITEM( 'o', 0xFFFFFF, "Orb of Flash", itOrbFlash, IC_ORB, ZERO, RESERVED, osOffensive,
    "This orb can be used to invoke a flash spell, which destroys almost everything in radius of 2.")
ITEM( 'o', 0x8080FF, "Orb of Winter", itOrbWinter, IC_ORB, ZERO | IF_FIREPROOF | IF_PROTECTION | IF_EMPATHY, RESERVED, osProtective,
    "This orb can be used to invoke a wall of ice. It also protects you from fires.")
ITEM( 'o', 0xFF6060, "Orb of Speed", itOrbSpeed, IC_ORB, ZERO | IF_EMPATHY, RESERVED, osMovement,
    "This orb can be used to move faster for some time.")
ITEM( 'o', 0x90B090, "Orb of Life", itOrbLife, IC_ORB, ZERO | IF_SHMUPLIFE | IF_REVIVAL, RESERVED, osFriend,
    "This orb can be used to summon friendly golems. It is used instantly when you pick it up.")
ITEM( 'o', 0x60D760, "Orb of Shielding", itOrbShield, IC_ORB, ZERO | IF_PROTECTION | IF_EMPATHY, RESERVED, osProtective,
    "This orb can protect you from damage.")
ITEM( 'o', 0x606060, "Orb of Earth", itOrbDigging, IC_ORB, ZERO | IF_EMPATHY, RESERVED, osTerraform,
    "This orb lets you go through living walls. It also has powers in some of the other lands.")
ITEM( 'o', 0x20FFFF, "Orb of Teleport", itOrbTeleport, IC_ORB, ZERO | IF_RANGED, RESERVED, osRanged,
    "This orb lets you instantly move to another location on the map. Just click a location which "
    "is not next to you to teleport there. "
    )
ITEM( 'o', 0xA0FF40, "Orb of Safety", itOrbSafety, IC_ORB, ZERO, RESERVED, osUtility,
    "This orb lets you instantly move to a safe faraway location. Knowing the nature of this strange world, you doubt "
    "that you will ever find the way back...\n\n"
    "Your game will be saved if you quit the game while the Orb of Safety is still powered.\n\n"
    "Technical note: as it is virtually impossible to return, this Orb recycles memory used for the world so far (even if you do not use it to save the game). "
    )
ITEM( 'o', 0x40C000, "Orb of Thorns", itOrbThorns, IC_ORB, ZERO | IF_EMPATHY, RESERVED, osOffensive,
    "This orb allows attacking Hedgehog Warriors directly, as well as stabbing other monsters.\n"
    )
ITEM( '%', 0x0000FF, "Fern Flower", itFernFlower, IC_TREASURE, ZERO, RESERVED, osNone,
    "This flower brings fortune to the person who finds it.\n"
    )
ITEM( '!', 0x900000, "Wine", itWine, IC_TREASURE, ZERO, RESERVED, osNone,
    "Wine grown under hyperbolic sun would be extremely prized in your home location."
    )
ITEM( 'o', 0x706070, "Orb of Aether", itOrbAether, IC_ORB, ZERO | IF_EMPATHY, RESERVED, osMovement,
    "This orb allows one to pass through all kinds of walls and chasms."
    )
ITEM( '$', 0xFFFFC0, "Silver", itSilver, IC_TREASURE, ZERO, RESERVED, osNone,
    "A precious metal from the Dead Caves." 
    )
ITEM( 'o', 0x005050, "Orb of the Mind", itOrbPsi, IC_ORB, ZERO | IF_RANGED, RESERVED, osRanged,
    "This orb allows you to instantly kill a non-adjacent enemy by clicking it. "
    "Each use drains 30 charges."
    )
ITEM( '!', 0xE2B227, "Royal Jelly", itRoyalJelly, IC_TREASURE, ZERO, RESERVED, osNone,
    "This is what Hyperbug Queens eat. Very tasty and healthy." 
    )
ITEM( '*', 0x60C060, "Emerald", itEmerald, IC_TREASURE, ZERO, RESERVED, osNone,
    "A precious green gem from the Emerald Mines." 
    )
ITEM( 'o', 0x421C52, "Orb of Invisibility", itOrbInvis, IC_ORB, ZERO | IF_PROTECTION | IF_EMPATHY, RESERVED, osProtective,
    "When you have this Orb, most monsters won't see you, unless "
    "you are standing still, attacking, or picking up items."
    )
ITEM( '*', 0xFFFF00, "Powerstone", itPower, IC_TREASURE, ZERO, RESERVED, osNone,
    "A Stone from the Land of Power. You are not sure what it is exactly, but "
    "as the Powerstones are kept in crystal cabinets, they are surely valuable."
    )
ITEM( 'o', 0xFF4000, "Orb of Fire", itOrbFire, IC_ORB, ZERO | IF_FIREPROOF | IF_EMPATHY, RESERVED, osTerraform,
    "When you have this Orb, you will leave a trail of fire behind you."
    )
ITEM( '!', 0xFFFF00, "Holy Grail", itHolyGrail, IC_OTHER, ZERO, RESERVED, osNone, camelothelp )
ITEM( '?', 0x00FF00, "Grimoire", itGrimoire, IC_TREASURE, ZERO, RESERVED, osNone,
    "The Grimoires contain many secrets of the Great Old Ones. "
    "Each new inner circle in the Temple of Cthulhu contains new Grimoires, with new secrets. "
    "You hope to read them when you return home, and to learn many things. "
    "The knowledge is valuable to you, but it is rather pointless to try to get "
    "several copies of the same Grimoire..."
    )
ITEM( 'o', 0xFF8000, "Orb of the Dragon", itOrbDragon, IC_ORB, ZERO | IF_FIREPROOF | IF_RANGED, RESERVED, osRanged,
    "This Orb allows you to throw fire, just like the Fire Cultists.\n\n"
    "Each fire drains 5 charges. You are not allowed to throw fire into adjacent cells."
    )
ITEM( 'o', 0x8B4513, "Orb of Trickery", itOrbIllusion, IC_ORB, ZERO | IF_RANGED, RESERVED, osRanged,
    "This Orb allows you to create illusions of yourself. Illusions are targeted "
    "by most monsters, just like yourself, Thumpers, and your friends.\n\n"
    "Each illusion takes 5 charges to create, and one extra charge "
    "per turn. You can also click your illusion to take it away, restoring 4 charges.\n\n"
    "If you have both Orb of Teleport and Orb of Trickery, Illusion is cast "
    "first -- you can then teleport on your Illusion to switch places with it."
    )
ITEM( 'x', 0xFF0000, "Pirate Treasure", itPirate, IC_TREASURE, ZERO, RESERVED, osNone, "Ye wonder where did th' Pirates find all these riches...")
ITEM( '?', 0xFFFFFF, "Compass", itCompass, IC_OTHER, ZERO, RESERVED, osNone, 
    "The hyperbolic pirates have no use for treasure maps. However, they have found "
    "out that a compass points to the center of the island. So they just go as "
    "far towards the center as they can, and hide their treasure there."
    )
ITEM( '*', 0xFF8080, "Red Gem", itRedGem, IC_TREASURE, ZERO, RESERVED, osNone, "A gem from the Red Rock Valley.")
ITEM( 'o', 0x6060FF, "Orb of Time", itOrbTime, IC_ORB, ZERO, RESERVED, osPowerUtility, 
    "Normally, the power of most Orbs slowly fades away, even when "
    "you are not actively using them. This Orb prevents this.\n\n"
    
    "When you have the Orb of Time, Orbs which are not active won't lose their power. "
    "Orbs are considered active if they have a continuous power which has actually "
    "affected something in the last turn.\n\n"
    
    "Orbs of Shielding remain active after being activated (even if you are no longer "
    "attacked), and Orbs of Time have a bigger cap inside their native Caribbean than "
    "outside."
    )
ITEM( 'o', 0x40C0C0, "Orb of Space", itOrbSpace, IC_ORB, ZERO | IF_RANGED, RESERVED, osRanged, 
    "This Orb is able to bring faraway items to your location, even if there are "
    "monsters or barriers on the way. The cost of "
    "bringing an item (in charges) equals the square of its distance to you. Contrary "
    "to some other Orbs, usage is not allowed if you have not enough power left."
    )
ITEM( '!', 0xFF8080, "Bomberbird Egg", itBombEgg, IC_TREASURE, ZERO, RESERVED, osNone, 
    "Bomberbird eggs are big and tasty, and thus valuable. "
    "They can hatch when left alone for some time (but this will never happen "
    "if you are watching)."
    )
ITEM( '*', 0xFFBF00, "Amber", itCoast, IC_TREASURE, ZERO, RESERVED, osNone, 
    "When the tide is away, beautiful ambers can be found on the hyperbolic beaches. "
    "Perhaps there used to be a forest here?"
    )
ITEM( '$', 0xFFFFFF, "Pearl", itWhirlpool, IC_TREASURE, ZERO, RESERVED, osNone, 
    "You do not know exactly why, but there are valuable pearls on many boats "
    "in the whirlpool."
    )
ITEM( 'o', 0x306000, "Orb of Friendship", itOrbFriend, IC_ORB, ZERO | IF_SHMUPLIFE | IF_REVIVAL, RESERVED, osFriend, 
    "This Orb summons a friendly Bomberbird."
    )
ITEM( 'o', 0x000060, "Orb of Water", itOrbWater, IC_ORB, ZERO | IF_EMPATHY, RESERVED, osTerraform, 
   "This Orb allows your boat to go against the current, "
   "and also to go into the land, creating water on the way."
    )
ITEM( 'o', 0xC0C0FF, "Orb of Air", itOrbAir, IC_ORB, ZERO | IF_RANGED, RESERVED, osRanged, 
    "This Orb allows you to blow your enemies away.\n\n"
    "Click a monster to blow it one cell away. It cannot be used against mimics, ghosts, sharks and other monsters restricted to a specific terrain, and multi-tile monsters."
    )
ITEM( '?', 0x802000, "Hypersian Rug", itPalace, IC_TREASURE, ZERO, RESERVED, osNone, 
    "Nice, a hyperbolic rug! It will not fit very well on your flat Euclidean floor, but who cares?")
ITEM( 'o', 0x60A060, "Orb of the Frog", itOrbFrog, IC_ORB, ZERO | IF_RANGED, RESERVED, osFrog, 
    "This Orb lets you jump to a place which is two cell away from you, in a single turn. "
    "You can jump over water, chasms and fire, but not over walls.")
ITEM( '*', 0x800000, "Garnet", itFjord, IC_TREASURE, ZERO, RESERVED, osNone, "Vikings believe that garnets improve their strength.")
ITEM( 'o', 0x0070C0, "Orb of the Fish", itOrbFish, IC_ORB, ZERO | IF_EMPATHY, RESERVED, osNone, 
    "This Orb lets you dive into the water. While diving, you are able to see and collect underwater treasures, "
    "while being invisible to monsters yourself. You are afraid of jumping into the water straight from a boat, so usually you won't do it."
    )
ITEM( 'o', 0xC00000, "Orb of Discord", itOrbDiscord, IC_ORB, ZERO, RESERVED, osFriend, 
    "Causes most monsters to attack other monsters, not only you and your friends.")
ITEM( 'P', 0x00FF00, "SAVE", itSavedPrincess, IC_NAI, ZERO, RESERVED, osNone, princessdesc)
ITEM( 'o', 0xFF208F, "Orb of Love", itOrbLove, IC_ORB, ZERO | IF_SHMUPLIFE, RESERVED, osLove, 
    "Love takes time, but it heals all wounds, and transcends time and space.\n\n"
    "The Orb of Love is worth 30$$$, if you end the game with it.\n"
    )
ITEM( '/', 0xFFFFE0, "Ivory Figurine", itIvory, IC_TREASURE, ZERO, RESERVED, osNone, 
    "A beautiful figurine, made of ivory. Figurines close to the base of the Tower "
    "tend to disappear after you have collected many of them."
    )
ITEM( '*', 0x606060, "Onyx", itZebra, IC_TREASURE, ZERO, RESERVED, osNone, "A black gem with white stripes. It is beautiful.")
ITEM( '%', 0xFF8000, "Fire Shard", itFireShard, IC_OTHER, ZERO | IF_SHARD, RESERVED, osNone, elemdesc)
ITEM( '%', 0x8080C0, "Air Shard", itAirShard, IC_OTHER, ZERO | IF_SHARD, RESERVED, osNone, elemdesc)
ITEM( '%', 0x80C080, "Earth Shard", itEarthShard, IC_OTHER, ZERO | IF_SHARD, RESERVED, osNone, elemdesc)
ITEM( '%', 0x0000C0, "Water Shard", itWaterShard, IC_OTHER, ZERO | IF_SHARD, RESERVED, osNone, elemdesc)
ITEM( '*', 0xF0F0F0, "Elemental Gem", itElemental, IC_TREASURE, ZERO, RESERVED, osNone, elemdesc)
ITEM( 'o', 0x309060, "Orb of Summoning", itOrbSummon, IC_ORB, ZERO | IF_RANGED, RESERVED, osRanged,
    "This orb allows you to summon monsters. Usually, they are either Elementals or "
    "native creatures. While the monsters do not like being summoned, and will "
    "attack you once they recover from summoning sickness, such summoning "
    "often has its uses."
    )
ITEM( 'o', 0x306090, "Orb of Matter", itOrbMatter, IC_ORB, ZERO | IF_RANGED, RESERVED, osRanged, 
    "This Orb allows one to temporarily create impassable matter, either to block paths or "
    "to build bridges across chasms and waters.")
ITEM( '*', 0xF0F000, "Bounty", itBounty, IC_TREASURE, ZERO, RESERVED, osNone, wildwestdesc)
ITEM( '[', 0xC0C0C0, "Revolver", itRevolver, IC_OTHER, ZERO | IF_RANGED, RESERVED, osRanged, wildwestdesc)
ITEM( '*', 0xF0F080, "Fulgurite", itFulgurite, IC_TREASURE, ZERO, RESERVED, osNone, elecdesc)
ITEM( '%', 0xFFFFFF, "Mutant Sapling", itMutant, IC_TREASURE, ZERO, RESERVED, osNone, overdesc)
ITEM( 'o', 0xA08000, "Orb of Stunning", itOrbStunning, IC_ORB, ZERO | IF_RANGED, RESERVED, osRanged, 
    "This Orb allows you to target monsters to stun them. "
    "10 charges are used to stun for 5 turns. Does not "
    "work against multi-tile monsters.")
ITEM( 'o', 0xC00000, "Orb of Luck", itOrbLuck, IC_ORB, ZERO, RESERVED, osUtility, 
    "This Orb allows you to find new lands more easily. "
    "Lands where you have already collected less treasure, "
    "and especially the Crossroads, are more likely to "
    "spawn while you have this. Additionally, Orbs of Safety "
    "are more likely to spawn in the Whirlpool."
    )
ITEM( '%', 0xD03030, "Mutant Fruit", itMutant2, IC_TREASURE, ZERO, RESERVED, osNone, cleardesc)
ITEM( 'o', 0xC00000, "Orb of Freedom", itOrbFreedom, IC_ORB, ZERO, RESERVED, osOffensive, 
    "This orb is activated if you are unable to escape (radius 4) "
    "without making illegal moves or "
    "going through cells which are currently adjacent to enemy monsters. "
    "Most game over situations are covered by this, but generally, "
    "this orb is oversensitive...\n\n"
    "When activated, it creates a Flash effect of radius 5."
    )
ITEM( '%', 0x606060, "Black Lotus", itLotus, IC_TREASURE, ZERO, RESERVED, osNone, 
    "This beautiful flower is greatly prized by wizards, as it allows them to cast powerful magical spells "
    "without preparation.\n"
    )
ITEM( 'o', 0x505050, "Orb of Undeath", itOrbUndeath, IC_ORB, ZERO | IF_EMPATHY | IF_SHMUPLIFE | IF_REVIVAL, RESERVED, osFriend, 
    "Monsters slain by you in melee are turned into friendly ghosts. "
    "Does not affect plants and friends."
    )
ITEM( '*', 0x8080FF, "White Dove Feather", itWindstone, IC_TREASURE, ZERO, RESERVED, osNone, 
    "This feather is truly beautiful and strong."
    )
ITEM( 'o', 0xC00060, "Orb of Empathy", itOrbEmpathy, IC_ORB, ZERO | IF_SHMUPLIFE, RESERVED, osFriend, 
    "This Orb lets your allies share your Orb powers.\n\n"
    "The following Orbs are affected:"
    )
ITEM( '>', 0x0000FF, "strong wind", itStrongWind, IC_NAI, ZERO, RESERVED, osNone, 
    "In the Windy Plains, you can let the wind carry you, "
    "causing you to move two cells with the wind in a single turn. "
    "This cannot be done if you are standing at distance at most 2 "
    "from the Air Elemental, or if any of the three cells on the way "
    "has two wind directions.\n\n"
    "Press 't' or click the destination to activate."
    )
ITEM( 'x', 0xFF00FF, "buggy item", itBuggy, IC_NAI, ZERO, RESERVED, osNone, 
    "Please report this as a bug."
    )
ITEM( 'x', 0xFFFF00, "buggy item", itBuggy2, IC_NAI, ZERO, RESERVED, osNone, 
    "Please report this as a bug."
    )
ITEM( '%', 0x744c7c / 4 + 0x800000, "Thornless Rose", itRose, IC_TREASURE, ZERO, RESERVED, osNone, 
    "A big, beautiful, magical flower."
    )
ITEM( '*', 0xFF40A0, "Coral", itCoral, IC_TREASURE, ZERO, RESERVED, osNone, 
    "Corals have a somewhat hyperbolic structure even in your home world, "
    "but natural corals from the Warped Sea have truly beautiful shapes. "
    "Ratlings know the value of corals, and thus keep them in boats for safety."
    )
ITEM( 'o', 0x764e7c*2, "Orb of Beauty", itOrbBeauty, IC_ORB, ZERO, RESERVED, osProtective, 
    "This Orb makes you stunningly beautiful. "
    "Monsters which come next to you will be stunned for one turn. "
    "Multi-tile monsters are not affected. Additionally, it makes you immune to "
    "beauty."
    )
ITEM( 'o', 0xFFFF80, "Orb of the Warp", itOrb37, IC_ORB, ZERO, RESERVED, osWarping, 
    "This Orb creates a warped zone of radius 5 around you, "
    "and also allows you to move diagonally in warped zones."
    )
ITEM( 'o', 0xFFFF80, "Orb of Energy", itOrbEnergy, IC_ORB, ZERO, RESERVED, osPowerUtility, 
    "This Orb halves the power usage of orbs which cost some "
    "charges with each activation. It even affects the "
    "one-shot orbs such as Flash or Teleport. If such an activation "
    "normally costs x charges, it costs only x/2 (rounded up) "
    "if you have an Orb of Energy."
    )
ITEM( 't', 0x487830, "Baby Tortoise", itBabyTortoise, IC_TREASURE, ZERO, RESERVED, osNone, tortoisedesc)
ITEM( 'o', 0x487830, "Orb of the Shell", itOrbShell, IC_ORB, ZERO | IF_PROTECTION, RESERVED, osProtective, 
    "This Orb protects you from physical attacks. "
    "It lasts for more turns than the Orb of Shielding, but "
    "10 charges are lost whenever you are attacked. "
    "It also does not protect you from fires, scents, and being eaten."
    )
ITEM( '!', 0xc00000, "Apple", itApple, IC_TREASURE, ZERO, RESERVED, osNone, "A fruit from the Yendorian Forest.")
ITEM( '!', 0xFF6000, "Dragon Scale", itDragon, IC_TREASURE, ZERO | IF_FIREPROOF, RESERVED, osNone, 
    "Dragon Scales are a prized material for armors. "
    "They are also prized by collectors, who would like to boast "
    "about how they have killed a Dragon.\n\n"
    "Dragon Scales disappear after 500 turns."
    )
ITEM( 'o', 0x900000, "Orb of Domination", itOrbDomination, IC_ORB, ZERO | IF_RANGED | IF_SHMUPLIFE, RESERVED, osRanged, 
    "This Orb lets you ride Dragons and other worm-like creatures. "
   "Simply move onto such a creature to ride them; while riding, you are protected from dangerous terrains "
   "and partially from attacks (they cause you to lose half of your Domination power), "
   "but you cannot collect items.\n\n"
   /*"When only one charge is left, "
   "you have to dismount this turn -- be very careful to make this possible, "
   "as your mount could attack you immediately!\n\n" */
   "While riding, "
   "click on a location to order your mount to move or attack there."
    )
ITEM( 'o', 0xFFFF80, "Orb of the Sword", itOrbSword, IC_ORB, ZERO, RESERVED, osDirectional, 
    "This Orb gives you a weapon made of pure magical energy. You do not hold "
    "it, it simply floats in the air next to you. When you go, the energy sword moves "
    "with you, pointing at the same relative angle it pointed before -- you cannot "
    "move or rotate it otherwise. Most monsters can be killed by moving the sword into them, "
    "and won't move into the spot with the sword."
    )
ITEM( 'x', 0x4040FF, "Sunken Treasure", itKraken, IC_TREASURE, ZERO, RESERVED, osNone, 
    "Cargo of a ship which was once destroyed by a Kraken." )
ITEM( 'o', 0xFF8040, "Orb of the Sword II", itOrbSword2, IC_ORB, ZERO, RESERVED, osDirectional, 
    "An alternative version of Orb of the Sword. If you have both of them, "
    "you have two energy swords, facing in opposite directions."
    )
ITEM( '*', 0xFFFF80, "Ancient Jewelry", itBarrow, IC_TREASURE, ZERO, RESERVED, osNone, 
    "Precious belongings of ancient Viking heroes. Your Orb of the Sword can be "
    "used to dig these treasures out of the barrows."
    )
ITEM( '!', 0xFFD700, "Golden Egg", itTrollEgg, IC_TREASURE, ZERO, RESERVED, osNone, 
    "Trolls of Trollheim are descendants of a bridge Troll, who collected "
    "payments from people crossing the bridge. One of them paid with "
    "golden eggs. The bridge Troll found the eggs beautiful, but he quickly lost them. "
    "Golden eggs are still revered by Trolls, and you can find them in their "
    "caves."
    )
ITEM( '!', 0xFF0000, "Warning", itWarning, IC_NAI, ZERO, RESERVED, osNone, warningdesc
    )
ITEM( 'o', 0x808080, "Orb of the Stone", itOrbStone, IC_ORB, ZERO | IF_EMPATHY, RESERVED, osTerraform, 
    "Trolls turn into stone walls when they die. When you have this Orb, "
    "this happens to every monster you defeat. Statues created from this Orb "
    "have slightly different properties than Trolls who petrify naturally."
    )
ITEM( 'o', 0xC08000, "Orb of Nature", itOrbNature, IC_ORB, ZERO | IF_RANGED | IF_SHMUPLIFE, RESERVED, osRanged, naturedesc )
ITEM( '%', 0x800080, "Treat", itTreat, IC_TREASURE, ZERO | IF_FIREPROOF, RESERVED, osNone, halloweendesc )
ITEM( '%', 0x30A030, "Slime Mold", itSlime, IC_TREASURE, ZERO, RESERVED, osNone, 
    "A very interesting species of slime mold."
    )
ITEM( '*', 0xFF00FF, "Amethyst", itAmethyst, IC_TREASURE, ZERO, RESERVED, osNone, "A beatiful purple gem from the Lost Mountain." )
ITEM( 'o', 0xC00040, "Orb of Recall", itOrbRecall, IC_ORB, ZERO, RESERVED, osUtility, 
    "When the charges on this Orb expire, "
    "you will be automatically returned to the place where you have found it. "
    "Extra Orbs of Recall delay this without changing the recall location. "
    "Picking up an Orb of Safety causes an immediate recall.")
ITEM( ']', 0x8080FF, "Dodecahedron", itDodeca, IC_TREASURE, ZERO, RESERVED, osNone, 
    "These dodecahedra made of a mysterious material are the Reptiles' favorite toy."
    )
ITEM( 'o', 0x8080FF, "Orb of Vaulting", itOrbDash, IC_ORB, ZERO | IF_RANGED, RESERVED, osFrog, 
    "This Orb allows you to jump over an adjacent monster, killing or stunning it. "
    "You can only vault in a roughly straight line. "
    "Target a cell on the other side to use it."
    )
ITEM( '$', 0x80FF80, "Green Grass", itGreenGrass, IC_TREASURE, ZERO, RESERVED, osNone, prairiedesc )
ITEM( 'o', 0xC09090, "Orb of Horns", itOrbHorns, IC_ORB, ZERO, RESERVED, osDirectional, 
    "After you move while having this Orb, you immediately attack the next cell in the straight line "
    "(or two cells, when moving on a heptagon). This attack is slightly stronger than your normal "
    "attack: it can stun some of the monsters which cannot be killed or stunned normally."
    )
ITEM( 'o', 0xA05020, "Orb of the Bull", itOrbBull, IC_ORB, ZERO, RESERVED, osDirectional, 
    "You get the powers of Shield, Horns, and Thorns after you move two moves in a straight line "
    "with this Orb." )
ITEM( '$', 0xC060C0, "Spinel", itBull, IC_TREASURE, ZERO, RESERVED, osNone, bulldashdesc )
ITEM( 'o', 0xC0C0FF, "Orb of the Mirror", itOrbMirror, IC_ORB, ZERO, RESERVED, osNone, 
    "Use Orb of the Mirror to gain copies of one of your Orbs; "
    "mirroring weaker Orbs usually yields more copies. "
    "It can only be used once per Orb type, "
    "and only when you are next to a mirror."
     )
ITEM( 'O', 0xF0F0F0, "your orbs", itInventory, IC_OTHER, ZERO, RESERVED, osNone, 
    "Click this to see your orbs.")
ITEM( '%', 0xE0E000, "Lava Lily", itLavaLily, IC_TREASURE, ZERO | IF_FIREPROOF, RESERVED, osNone, 
    "This plant, able to survive in the extreme conditions of the Volcanic Wasteland, "
    "is a valuable alchemical ingredient."
    )
ITEM( '*', 0x40E0D0, "Turquoise", itHunting, IC_TREASURE, ZERO, RESERVED, osNone, 
    "Hunters believe that wearing a Turquoise amulet will improve their accuracy. "
    "This one has been lost, but the hunting dogs are guarding it until the owner returns.")
ITEM( '*', 0x009090, "Forgotten Relic", itBlizzard, IC_TREASURE, ZERO, RESERVED, osNone, blizzarddesc)
ITEM( '(', 0xFFE080, "Ancient Weapon", itTerra, IC_TREASURE, ZERO, RESERVED, osNone, 
    "This ancient weapon is beautifully encrusted with precious gems, but you prefer your own -- it is much lighter.")
ITEM( 'o', 0x307080, "Orb of Slashing", itOrbSide1, IC_ORB, ZERO | IF_EMPATHY, RESERVED, osOffensive, 
    "Whenever you attack with this Orb, you also hit the monsters adjacent both to you and the monster you originally attacked.")
ITEM( 'o', 0x30A080, "Orb of the Triangle", itOrbSide2, IC_ORB, ZERO | IF_EMPATHY, RESERVED, osOffensive, 
    "Whenever you attack with this Orb, you also hit the monsters adjacent to you which are neither adjacent nor opposite to the monster "
    "you originally attacked.")
ITEM( 'o', 0x30D080, "Orb of Ferocity", itOrbSide3, IC_ORB, ZERO | IF_EMPATHY, RESERVED, osOffensive, 
    "Whenever you attack with this Orb, you also hit the monsters adjacent to you and opposite to the monster you originally attacked.")
ITEM( 'o', 0xD08030, "Orb of Lava", itOrbLava, IC_ORB, ZERO | IF_FIREPROOF, RESERVED, osTerraform, 
    "This Orb summons a minor volcanic activity around you. "
    "All the heptagonal cells adjacent to enemies in distance at most 5 to you "
    "will be set on fire. Does not affect the cell you are on, and enemies resistant to fire.")
ITEM( 'o', 0x3080D0, "Orb of Change", itOrbMorph, IC_ORB, ZERO | IF_RANGED, RESERVED, osRanged, 
    "This ranged Orb will transform the target monster into one without any special powers. It also stuns them for one turn. "
    "Does not affect multi-tile monsters.")
ITEM( '!', 0x80FF00, "Glowing Crystal", itGlowCrystal, IC_TREASURE, ZERO, RESERVED, osNone, crystaldesc)
ITEM( '!', 0x80FF80, "Snake Oil", itSnake, IC_TREASURE, ZERO, RESERVED, osNone, "Made of actual snakes!")
ITEM( '!', 0x80FF80, "Sea Glass", itDock, IC_TREASURE, ZERO, RESERVED, osNone, NODESCYET)
ITEM( '*', 0xBBCC99, "Chrysoberyl", itRuins, IC_TREASURE, ZERO, RESERVED, osNone, "Fragment of the past glory.")
ITEM( '*', 0x80FF80, "Monopole", itMagnet, IC_TREASURE, ZERO, RESERVED, osNone, NODESCYET)
ITEM( '!', 0xFF00FF, "Tasty Jelly", itSwitch, IC_TREASURE, ZERO, RESERVED, osNone, "A tasty byproduct of the Jelly Revolution.")
ITEM( 'o', 0xFFFF80, "Orb of Phasing", itOrbPhasing, IC_ORB, ZERO, RESERVED, osFrog, 
    "This orb lets you pass through walls (one cell wide), and also through monsters, as long as they will not attack you in transit.")
ITEM( 'o', 0xFFFF80, "Orb of Magnetism", itOrbMagnetism, IC_ORB, ZERO, RESERVED, osUtility, 
    "This orb makes you immediately pickup all the items on adjacent cells.")
ITEM( 'o', 0x202020, "Orb of Slaying", itOrbSlaying, IC_ORB, ZERO | IF_EMPATHY, RESERVED, osOffensive,
    "This Orb lets you defeat Raiders and other tough single-cell monsters in melee."
    )
ITEM( '*', 0xFFA860, "Tiger's Eye", itBrownian, IC_TREASURE, ZERO, RESERVED, osNone,
    "A brown gem."
    )
ITEM( '$', 0xF0C0C0, "Meteorite", itWest, IC_TREASURE, ZERO, RESERVED, osNone, 
    "These rocks falling from the sky have been captured to fall forever in the artificial gravity. Meteorite iron is believed to be a valuable material for magical weapons.")
ITEM( '*', 0x30FF30, "Torbernite", itVarTreasure, IC_TREASURE, ZERO, RESERVED, osNone, 
    "Crystals emitting magical radiation.")
ITEM( 'o', 0x703800, "Orb of Intensity", itOrbIntensity, IC_ORB, ZERO, RESERVED, osPowerUtility, 
    "When you have this, initial and maximal charge amounts of all Orbs are increased by 20%."
    )
ITEM( 'o', 0x80D080, "Orb of Gravity", itOrbGravity, IC_ORB, ZERO | IF_SHMUPLIFE, RESERVED, osWarping, 
    "This Orb lets you magically control gravity around you. In lands with unusual gravity, the options are: usual gravity (no change, except that it may disrupt birds), "
    "anti-gravity (causes the direction of gravity to be reversed), levitation (you can move in directions which are neither up nor down, "
    "or between cells if one of them has a wall in a 'neutral' direction). In lands with standard gravity, levitation lets creatures to avoid traps, chasms, "
    "and water, while anti-gravity makes it possible to move only when next to a wall (movement between cells without adjacent walls is not allowed). "
    "For each move, the gravity is based on what you do, and all enemies in the effective range are affected (for example, if you move next to a wall in a non-gravity land, "
    "anti-gravity kicks in and monsters will not be able to move in open space)."
    )
ITEM( 'o', 0xD08080, "Orb of Choice", itOrbChoice, IC_ORB, ZERO, RESERVED, osNone, 
    "Did you know that it is possible to break a magical sphere into finitely many parts, and then, out of the parts obtained, to make two magical spheres, just like the original? "
    "Hyperbolic geometry makes this even easier!\n\n"
    "When you pick up another Orb, it will not disappear from the ground -- Orb of Choice will be drained instead."
    )
//ITEM( '*', 0x26619C, "Lapis Lazuli", itLapisLazuli, IC_TREASURE, ZERO, RESERVED, osNone, NODESCYET)

WALL( '.', 0xFF00FF, "no wall", waNone, ZERO | WF_HEATCOLOR, RESERVED, 0, sgNone,        NODESC)
WALL( '#', 0x8080FF, "ice wall", waIcewall, WF_WALL | WF_HIGHWALL | WF_HEATCOLOR, RESERVED, 0, sgNone,    
    "Ice Walls melt after some time has passed."
    )
WALL( '#', 0xC06000, "great wall", waBarrier, WF_WALL | WF_HIGHWALL, RESERVED, 0, sgNone,  barrierhelp)
WALL( '+', 0x900030, "red slime", waFloorA, ZERO | WF_ALCHEMY, RESERVED, 0, sgFloorA,  slimehelp )
WALL( '+', 0x300090, "blue slime", waFloorB, ZERO | WF_ALCHEMY, RESERVED, 0, sgFloorB,   slimehelp )
WALL( '#', 0xA0D0A0, "living wall", waCavewall, WF_WALL | WF_HIGHWALL, RESERVED, 0, sgCave, cavehelp)
WALL( '.', 0x306060, "living floor", waCavefloor, ZERO, RESERVED, 0, sgNone,cavehelp)
WALL( '#', 0xD03030, "dead rock troll", waDeadTroll, WF_WALL | WF_HIGHWALL, RESERVED, 0, sgNone,  trollhelp)
WALL( '#', 0xCDA98F, "sand dune", waDune, WF_WALL | WF_HIGHWALL | WF_CONE, RESERVED, 0, sgNone,   
  "A natural terrain feature of the Desert."
  )
WALL( '%', 0xC0C0FF, "Magic Mirror", waMirror, WF_WALL, RESERVED, 0, sgNone,
  "You can go inside the Magic Mirror, and produce some mirror images to help you."
  )
WALL( '%', 0xFFC0C0, "Cloud of Mirage", waCloud, WF_WALL, RESERVED, 0, sgNone,
   "Tiny droplets of magical water. You see images of yourself inside them. "
   "Go inside the cloud, to make these images help you.")
WALL( '^', 0x8D694F, "Thumper", waThumperOff, WF_WALL | WF_ACTIVABLE | WF_THUMPER, RESERVED, 0, sgNone, thumpdesc)
WALL( '^', 0x804000, "Fire", waFire, WF_FIRE | WF_TIMEOUT, RESERVED, 0, sgNone,
    "This cell is on fire. Most beings and items cannot survive."
    )
WALL( '+', 0xC0C0C0, "ancient grave", waAncientGrave, WF_WALL | WF_HIGHWALL | WF_GRAVE, RESERVED, 0, sgNone, 
  "An ancient grave." 
    )
WALL( '+', 0xD0D080, "fresh grave", waFreshGrave, WF_WALL | WF_HIGHWALL | WF_GRAVE, RESERVED, 0, sgNone,
  "A fresh grave. Necromancers like those." 
    )
WALL( '#', 0x00FFFF, "column", waColumn, WF_WALL | WF_HIGHWALL, RESERVED, 0, sgNone,
  "A piece of architecture typical to R'Lyeh." 
   )
WALL( '=', 0xFFFF00, "lake of sulphur", waSulphurC, ZERO | WF_CHASM | WF_SULPHURIC, RESERVED, 0, sgNone, lakeDesc )
WALL( '=', 0xFFFF00, "lake of sulphur", waSulphur, ZERO | WF_CHASM | WF_SULPHURIC, RESERVED, 0, sgNone, lakeDesc )
WALL( '=', 0x000080, "lake", waLake, WF_WATER | WF_HEATCOLOR, RESERVED, 0, sgWater, 
  "An impassable lake in Cocytus."
    )
WALL( '_', 0x000080, "frozen lake", waFrozenLake, ZERO | WF_HEATCOLOR, RESERVED, 0, sgNone, cocytushelp )
WALL( '>', 0x101010, "chasm", waChasm, ZERO | WF_CHASM, RESERVED, 0, sgNone,
  "It was a floor... until something walked on it." 
    )
WALL( '>', 0x101010, "chasmD", waChasmD, WF_WALL, RESERVED, 0, sgNone,
  "It was a floor... until something walked on it." 
    )
WALL( '#', 0x60C000, "big tree", waBigTree, WF_WALL | WF_HIGHWALL | WF_STDTREE | WF_CONE, RESERVED, 0, sgNone, foresthelp)
WALL( '#', 0x006000, "tree", waSmallTree, WF_WALL | WF_HIGHWALL | WF_STDTREE | WF_CONE, RESERVED, 0, sgNone, foresthelp)
WALL( '#', 0x421C52*2, "vine", waVinePlant, WF_WALL | WF_HIGHWALL, RESERVED, 0, sgVine, vinehelp)
WALL( ':', 0x006000, "vine", waVineHalfA, ZERO | WF_NOFLIGHT | WF_HALFVINE, RESERVED, 0, sgVine, hvinehelp)
WALL( ';', 0x006000, "vine", waVineHalfB, ZERO | WF_NOFLIGHT | WF_HALFVINE, RESERVED, 0, sgVine, hvinehelp)
WALL( '^', 0x804000, "partial fire", waPartialFire, WF_FIRE | WF_TIMEOUT, RESERVED, 0, sgNone, "This cell is partially on fire.")
WALL( '#', 0xA07070, "dead wall", waDeadwall, WF_WALL | WF_HIGHWALL, RESERVED, 0, sgCave, deadcavehelp)
WALL( '.', 0x401010, "dead floor", waDeadfloor, ZERO, RESERVED, 0, sgNone,deadcavehelp)
WALL( '.', 0x905050, "rubble", waDeadfloor2, ZERO, RESERVED, 1, sgNone, "Dead floor, with some rubble.")
WALL( '#', 0xD0D010, "weird rock", waWaxWall, WF_WALL | WF_HIGHWALL, RESERVED, 0, sgNone, 
    "A weirdly colored rock. Hyperentomologists claim that the "
    "Hyperbug armies use these rocks to navigate back home after a victorious battle."
    )
WALL( '#', 0x8080C0, "crystal cabinet", waGlass, WF_WALL, RESERVED, 0, sgNone, 
    "Witches use these crystal cabinets to protect Powerstones, as well as the more "
    "expensive Orbs. They are partially protected from thieves: they are too strong "
    "to be smashed by conventional attacks, and if you try to steal the item "
    "using an Orb of Aether, your Aether power will be completely drained."
    )
WALL( '#', 0xC0C0C0, "wall of Camelot", waCamelot, WF_WALL | WF_HIGHWALL, RESERVED, 0, sgNone, camelothelp )
WALL( '+', 0xA06000, "Round Table", waRoundTable, WF_WALL, RESERVED, 1, sgNone, camelothelp )
WALL( '=', 0x0000A0, "moat of Camelot", waCamelotMoat, WF_WATER, RESERVED, 0, sgWater, camelothelp)
WALL( '+', 0x606060, "big statue of Cthulhu", waBigStatue, WF_WALL, RESERVED, 0, sgNone, 
    "These statues of Cthulhu are too large to carry, and they don't look too "
    "valuable anyway. Most monsters will never go through them... they probably have "
    "their reasons. But you can go! When you move into the cell containing "
    "a statue, you push the statue to the cell you left.\n"
    )
WALL( '=', 0x0000A0, "sea", waSea, WF_WATER, RESERVED, 0, sgWater, caribbeanhelp)
WALL( '+', 0x0000A0, "boat", waBoat, ZERO | WF_BOAT | WF_NOFLIGHT, RESERVED, 0, sgNone, 
    "Hyperbolic pirates do not need huge ships, since so many lands to conquest "
    "are so close. These small boats are enough for them.\n\n"
    "Boats allow you to go through water. If you are in a boat, you can move into "
    "a water cell (and the boat will come with you)."
    )
WALL( '.', 0x00FF00, "island", waCIsland, ZERO | WF_CISLAND, RESERVED, 0, sgNone, cislandhelp)
WALL( '.', 0x80C060, "island", waCIsland2, ZERO | WF_CISLAND, RESERVED, 0, sgNone, cislandhelp)
WALL( '#', 0x006000, "tree", waCTree, WF_WALL | WF_HIGHWALL | WF_CONE | WF_CISLAND, RESERVED, 0, sgTree, 
    "The forests of Caribbean are too dense to be traversed by humans, "
    "and they are hard to burn. Many colorful parrots can be found there."
    )
WALL( ',', 0x800000, "rock I", waRed1, ZERO | WF_RED, RESERVED, 1, sgNone, redrockhelp)
WALL( ':', 0xC00000, "rock II", waRed2, ZERO | WF_RED, RESERVED, 2, sgNone, redrockhelp)
WALL( ';', 0xFF0000, "rock III", waRed3, ZERO | WF_RED, RESERVED, 3, sgNone, redrockhelp)
WALL( '.', 0xD0D0D0, "minefield", waMineUnknown, ZERO, RESERVED, 0, sgNone, minedesc)
WALL( '.', 0xD0D0D0, "minefield", waMineMine, ZERO, RESERVED, 0, sgNone, minedesc)
WALL( '.', 0x909090, "cell without mine", waMineOpen, ZERO, RESERVED, 0, sgNone, minedesc)
WALL( '+', 0x808000, "stranded boat", waStrandedBoat, ZERO | WF_BOAT | WF_NOFLIGHT, RESERVED, 0, sgNone, 
    "This boat cannot go through the sand. But if you sit inside and "
    "wait for the tide, you will be able to use it to travel through the Ocean."
    )
WALL( '#', 0xFFD500, "palace wall", waPalace, WF_WALL | WF_HIGHWALL, RESERVED, 0, sgNone, palacedesc )
WALL( '+', 0xFFFFFF, "closed gate", waClosedGate, WF_WALL | WF_HIGHWALL, RESERVED, 0, sgNone, gatedesc )
WALL( '-', 0x404040, "open gate", waOpenGate, ZERO, RESERVED, 0, sgNone, gatedesc )
WALL( '_', 0xC00000, "closing plate", waClosePlate, ZERO, RESERVED, 0, sgNone, gatedesc )
WALL( '_', 0x00C050, "opening plate", waOpenPlate, ZERO, RESERVED, 0, sgNone, gatedesc )
WALL( '_', 0x202020, "trapdoor", waTrapdoor, ZERO, RESERVED, 0, sgNone, "This floor will fall after someone goes there. Go quickly!" )
WALL( '+', 0xFF0000, "giant rug", waGiantRug, ZERO, RESERVED, 0, sgNone, 
    "This is the biggest Hypersian Rug you have ever seen! "
    "Unfortunately, it is too large to take it as a trophy." )
WALL( '#', 0xfffff0, "platform", waPlatform, WF_WALL | WF_HIGHWALL, RESERVED, 0, sgNone, "You can stand here.")
WALL( '#', 0x909090, "stone gargoyle", waGargoyle, WF_WALL | WF_HIGHWALL, RESERVED, 0, sgNone, gargdesc)
WALL( '.', 0xB0B0B0, "stone gargoyle floor", waGargoyleFloor, ZERO, RESERVED, 1, sgNone, gargdesc)
WALL( '.', 0x909090, "rubble", waRubble, ZERO, RESERVED, 1, sgNone, "Some rubble.")
WALL( '+', 0x804000, "ladder", waLadder, ZERO, RESERVED, 0, sgNone, 
    "You can use this ladder to climb the Tower."
    )
WALL( '#', 0xC0C0C0, "limestone wall", waStone, WF_WALL | WF_HIGHWALL, RESERVED, 0, sgNone, "Simply a wall. Mostly.")
WALL( '^', 0x804000, "Bonfire", waBonfireOff, WF_WALL | WF_ACTIVABLE, RESERVED, 0, sgNone,
    "A heap of wood that can be used to start a fire. Everything is already here, you just need to touch it to fire it."
    )
WALL( '^', 0x8D694F, "Thumper", waThumperOn, WF_WALL | WF_TIMEOUT | WF_PUSHABLE | WF_THUMPER, RESERVED, 0, sgNone,
     "A device that attracts sandworms and other enemies. You need to activate it.")
WALL( '^', 0x804000, "Eternal Fire", waEternalFire, WF_FIRE, RESERVED, 0, sgNone,
    "This fire never burns out."
    )
WALL( '.', 0x909090, "stone gargoyle bridge", waGargoyleBridge, ZERO, RESERVED, 1, sgNone, gargdesc)
WALL( '#', 0x309060, "temporary wall", waTempWall, WF_WALL | WF_HIGHWALL | WF_TIMEOUT, RESERVED, 0, sgNone, twdesc)
WALL( '.', 0x309060, "temporary floor", waTempFloor, ZERO | WF_TIMEOUT, RESERVED, 1, sgNone, twdesc)
WALL( '.', 0x309060, "temporary bridge", waTempBridge, ZERO | WF_TIMEOUT, RESERVED, 1, sgNone, twdesc)
WALL( '#', 0x3030FF, "charged wall", waCharged, WF_WALL | WF_HIGHWALL, RESERVED, 0, sgNone, elecdesc)
WALL( '#', 0xFF3030, "grounded wall", waGrounded, WF_WALL | WF_HIGHWALL, RESERVED, 0, sgNone, elecdesc)
WALL( '#', 0xA0A060, "sandstone wall", waSandstone, WF_WALL | WF_HIGHWALL, RESERVED, 0, sgNone, elecdesc)
WALL( '#', 0x704000, "saloon wall", waSaloon, WF_WALL | WF_HIGHWALL, RESERVED, 0, sgNone, wildwestdesc)
WALL( '#', 0x90C0C0, "metal wall", waMetal, WF_WALL | WF_HIGHWALL, RESERVED, 0, sgNone, elecdesc)
WALL( '#', 0x607030, "dead troll", waDeadTroll2, WF_WALL | WF_HIGHWALL, RESERVED, 0, sgNone,  trollhelpX)
WALL( '+', 0xC0C0FF, "fan", waFan, WF_WALL, RESERVED, 0, sgNone,  winddesc)
WALL( '?', 0xFF00FF, "<temporary>", waTemporary, WF_WALL, RESERVED, 0, sgNone,  NODESC)
WALL( '?', 0xFF00FF, "<earth d", waEarthD, WF_WALL, RESERVED, 0, sgNone,  NODESC)
WALL( '?', 0xFF00FF, "<elemental tmp>", waElementalTmp, WF_WALL, RESERVED, 0, sgNone,  NODESC)
WALL( '?', 0xFF00FF, "<elemental d>", waElementalD, WF_WALL, RESERVED, 0, sgNone,  NODESC)
WALL( '+', 0x00F000, "green slime", waSlime1, ZERO, RESERVED, 0, sgNone,  NODESC)
WALL( '+', 0xF0F000, "yellow slime", waSlime2, ZERO, RESERVED, 0, sgNone,  NODESC)
WALL( '#', 0x764e7c, "rosebush", waRose, WF_WALL | WF_HIGHWALL | WF_THORNY, RESERVED, 0, sgNone, roselanddesc)
WALL( '#', 0xC0C000, "warp gate", waWarpGate, WF_WALL | WF_HIGHWALL, RESERVED, 0, sgNone,
    "This gate separates the warped area from the normal land.")
WALL( '+', 0x804000, "trunk", waTrunk, ZERO | WF_NOFLIGHT, RESERVED, 0, sgNone, "The skeleton of a tree.")
WALL( '-', 0x402000, "solid branch", waSolidBranch, ZERO, RESERVED, 0, sgNone, "Branches here could bear your weight easily.")
WALL( ':', 0x804000, "weak branch", waWeakBranch, ZERO, RESERVED, 0, sgNone, 
    "Branches here will bear you weight, but if you use them to move (not fall) to an unstable place, they will break.")
WALL( '+', 0x60C060, "canopy", waCanopy, ZERO, RESERVED, 0, sgNone, 
    "Only thin twigs and leaves here. They may bear fruits, but for you, these cells count "
    "as unstable."
    )
WALL( '#', 0xD0C060, "barrow wall", waBarrowWall, WF_WALL | WF_HIGHWALL, RESERVED, 0, sgNone, "This wall is quite strong. You will need another way in.")
WALL( '#', 0x90A060, "barrow", waBarrowDig, WF_WALL | WF_HIGHWALL, RESERVED, 0, sgNone, "Your Orb of the Sword can be used to dig here.")
WALL( '#', 0xE0E0E0, "stone statue", waPetrified, WF_WALL | WF_HIGHWALL, RESERVED, 0, sgNone, "A petrified creature.")
WALL( '.', 0xE8E8E8, "tower of Camelot", waTower, ZERO, RESERVED, 3, sgNone, camelothelp)
WALL( '-', 0x402000, "big bush", waBigBush, ZERO | WF_NOFLIGHT, RESERVED, 0, sgNone, 
    "You can hold this bush to climb the Lost Mountain. "
    "Bushes block the movement of birds."
    )
WALL( ':', 0x804000, "small bush", waSmallBush, ZERO | WF_NOFLIGHT, RESERVED, 0, sgNone, 
    "You can hold this bush to climb the Lost Mountain, "
    "but it is not very strong -- it will get destroyed "
    "if you climb from it into an unstable location. "
    "Bushes block the movement of birds.")
WALL( '.', 0xFFFF00, "Reptile floor", waReptile, ZERO | WF_REPTILE, RESERVED, 0, sgNone, reptiledesc)
WALL( '.', 0xFFFF00, "Reptile bridge", waReptileBridge, ZERO | WF_REPTILE, RESERVED, 0, sgNone, reptiledesc)
WALL( '.', 0xFFFF00, "invisible floor", waInvisibleFloor, ZERO, RESERVED, 0, sgNone, NODESCYET)
WALL( '#', 0xC0C0FF, "mirror wall", waMirrorWall, WF_WALL, RESERVED, 0, sgNone, mirroreddesc)
WALL( '.', 0xE0E0E0, "stepping stones", waPetrifiedBridge, ZERO, RESERVED, 1, sgNone, "A petrified creature.")
WALL( '#', 0x309060, "temporary wall", waTempBridgeBlocked, WF_WALL | WF_HIGHWALL | WF_TIMEOUT, RESERVED, 0, sgNone, twdesc)
WALL( 'S', 0xB0B0B0, "warrior statue", waTerraWarrior, WF_WALL, RESERVED, 0, sgNone, terradesc)
WALL( '=', 0xB0B0B0, "bubbling slime", waBubble, ZERO | WF_CHASM, RESERVED, 0, sgNone, NODESC)
WALL( '^', 0xD00000, "arrow trap", waArrowTrap, ZERO, RESERVED, 0, sgNone, arrowtrapdesc)
WALL( '=', 0xE2E2E2, "mercury river", waMercury, ZERO | WF_CHASM, RESERVED, 0, sgNone, "A river of mercury.")
WALL( '&', 0xD00000, "lava", waMagma, ZERO, RESERVED, 1, sgNone, lavadesc)
WALL( '=', 0x804000, "dock", waDock, ZERO, RESERVED, 0, sgNone, "A dock.")
WALL( '^', 0xFF8000, "burning dock", waBurningDock, WF_FIRE | WF_TIMEOUT, RESERVED, 0, sgNone, "A burning dock.")
WALL( '#', 0xE04030, "ruin wall", waRuinWall, WF_WALL | WF_HIGHWALL, RESERVED, 0, sgNone, ruindesc)
WALL( '#', 0xA04060, "Brownian generator", waBrownian, WF_WALL | WF_HIGHWALL, RESERVED, 0, sgNone, NODESC)
WALL( '^', 0xC05000, "fire trap", waFireTrap, ZERO, RESERVED, 0, sgNone, 
    "This trap will explode when stepped on, setting all the adjacent cells on fire. However, this happens on the next turn, "
    "so you can safely escape if you are fast enough.")
WALL( '^', 0xFD692F, "Explosive Barrel", waExplosiveBarrel, WF_WALL | WF_PUSHABLE, RESERVED, 0, sgNone, 
    "These barrels can be pushed, and they will explode if next to a fire, or under some attacks."
    )
WALL( '#', 0xC0C0C0, "editable statue", waEditStatue, WF_WALL, RESERVED, 0, sgNone, 
    "A statue."
    )
WALL( '+', 0x804000, "trunk", waTrunk3, WF_WALL, RESERVED, 0, sgNone, "The skeleton of a tree.")


LAND( 0xFF00FF, "???"          , laNone, ZERO | LF_TECHNICAL, itNone, RESERVED, "")
  NATIVE(0)
  REQ(NEVER)

LAND( 0xC06000, "Great Wall"   , laBarrier, ZERO | LF_TECHNICAL, itNone, RESERVED, "")
  NATIVE(0)
  REQ(NEVER)

LAND( 0xFF0000, "Crossroads"   , laCrossroads, ZERO, itHyperstone, RESERVED, 
    "This land is a quick gateway to other lands. It is very easy to find other lands "
    "from the Crossroads. Which means that you find monsters from most other lands here!\n\n"
    "As long as you have found enough treasure in their native lands, you can "
    "find magical items in the Crossroads. Mirror Land brings mirrors and clouds, "
    "and other land types bring magical orbs.\n\n"
    "A special treasure, Hyperstone, can be found on the Crossroads, but only "
    "after you have found 10 of every other treasure."
    )
  NATIVE(0)
  REQ(ALWAYS)

LAND( 0xCDA98F, "Desert", laDesert, ZERO, itSpice, RESERVED,
    "A hot land, full of sand dunes, mysterious Spice, and huge and dangerous sand worms."
    )
  NATIVE((m == moDesertman || m == moWorm) ? 2 : 0)
  REQ(ALWAYS)

LAND( 0x8080FF, "Icy Land", laIce, ZERO | LF_ICY, itDiamond, RESERVED,
    "A very cold land, full of ice walls. Your mere presence will cause these ice walls to "
    "melt, even if you don't want it."
    )
  NATIVE((m == moWolf || m == moWolfMoved || m == moYeti) ? 2 : 0)
  REQ(ALWAYS)

LAND( 0x306060, "Living Cave", laCaves, ZERO | LF_TROLL, itGold, RESERVED, cavehelp)
  NATIVE((m == moGoblin || m == moTroll) ? 2 : m == moSeep ? 1 : 0)
  REQ(ALWAYS)

LAND( 0x00C000, "Jungle", laJungle, ZERO, itRuby, RESERVED,
    "A land filled with huge ivy plants and dangerous animals."
    )
  NATIVE((m == moIvyRoot || m == moMonkey) ? 2 : (isIvy(m) || m == moEagle || m == moMonkey) ? 1 : 0)
  REQ(ALWAYS)

LAND( 0x900090, "Alchemist Lab", laAlchemist, ZERO, itElixir, RESERVED, slimehelp)
  NATIVE((m == moSlime) ? 2 : 0)
  REQ(ALWAYS)

LAND( 0x704070, "Hall of Mirrors", laMirror, ZERO | LF_MIRROR, itShard, RESERVED,
    "A strange land filled with mirrors. "
    "Break magic mirrors and mirage clouds to "
    "gain treasures and helpful Mimics."
    )
  NATIVE((m == moMirrorSpirit || m == moNarciss || m == moMimic) ? 1 : 0)
  REQ(GOLD(R30))

LAND( 0x404070, "Graveyard", laGraveyard, ZERO, itBone, RESERVED,
    "All the monsters you kill are carried to this strange land, and buried. "
    "Careless Rogues are also carried here..."
    )
  NATIVE((m == moZombie || m == moNecromancer) ? 2 : m == moGhost ? 1 : 0)
  REQ(KILLS(R100))

LAND( 0x00FF00, "R'Lyeh", laRlyeh, ZERO, itStatue, RESERVED,
    "An ancient sunken city which can be reached only when the stars are right.\n\n"
    "You can find Temples of Cthulhu in R'Lyeh once you collect five Statues of Cthulhu."
    )
  NATIVE(
        (m == moCultist || m == moTentacle || m == moPyroCultist) ? 2 :
        (m == moCultistLeader || isIvy(m)) ? 1 : 0)
  REQ(ACCONLYF(laOcean) GOLD(R60))

LAND( 0xC00000, "Hell", laHell, ZERO, itHell, RESERVED,
    "A land filled with demons and molten sulphur. Abandon all hope ye who enter here!"
    )
  NATIVE((m == moLesser || m == moGreater) ? 2 : 0)
  REQ(NUMBER(orbsUnlocked(), lands_for_hell(), XLAT("Finished lands required: %1 (collect %2 treasure)\n", "9", its(R10))))

LAND( 0x00FF00, "Cocytus", laCocytus, ZERO | LF_ICY, itSapphire, RESERVED,
    cocytushelp
    )
  NATIVE((m == moShark || m == moGreaterShark || m == moCrystalSage) ? 2 : m == moYeti ? 1 : 0)
  REQ(ITEMS(itHell, U10) ITEMS(itDiamond, U10))

LAND( 0xFFFF00, "Land of Eternal Motion", laMotion, ZERO, itFeather, RESERVED,
    "A land where you cannot stop, because every piece of floor is extremely unstable. Only monsters who "
    "can run forever are able to survive there, and only phoenix feathers are so light that they do not disturb "
    "the floor.\n"
    )
  NATIVE((m == moRunDog) ? 2 : 0)
  REQ(ALWAYS)

LAND( 0x008000, "Dry Forest", laDryForest, ZERO, itFernFlower, RESERVED, foresthelp)
  NATIVE((m == moHedge || m == moFireFairy) ? 2 : 0)
  REQ(GOLD(R60))

LAND( 0x60C060, "Emerald Mine", laEmerald, ZERO, itEmerald, RESERVED, 
     "Evil people are mining for emeralds in this living cave. "
     "It does not grow naturally, but it is dug out in a regular "
     "pattern, which is optimal according to the evil engineers."
     )
  NATIVE((m == moFlailer || m == moLancer || m == moMiner) ? 2 : among(m, moHedge, moSeep) ? 1 : 0)
  REQ( ORD(AKILL(moVizier, laPalace), ITEMS(itFernFlower, U5) ITEMS(itGold, U5)))

LAND( 0x421C52, "Vineyard", laWineyard, ZERO, itWine, RESERVED, vinehelp)
  NATIVE((m == moVineSpirit || m == moVineBeast) ? 2 : 0)
  REQ(GOLD(R60))

LAND( 0x104040, "Dead Cave", laDeadCaves, ZERO | LF_TROLL, itSilver, RESERVED, deadcavehelp)
  NATIVE((m == moEarthElemental || m == moDarkTroll) ? 2 : (m == moGoblin || m == moSeep) ? 1 : 0)
  REQ(GOLD(R60) ITEMS(itGold, U10))

LAND( 0x705020, "Hive", laHive, ZERO, itRoyalJelly, RESERVED, hivehelp)
  NATIVE(isBug(m) ? 1 : 0)
  REQ(KILLS(R100) GOLD(R60))

LAND( 0xFFFF00, "Land of Power", laPower, ZERO, itPower, RESERVED, 
    "The Land of Power is filled with everburning fire, magical Orbs, and guarded by "
    "witches and golems. There are basic orbs lying everywhere, and more prized ones "
    "are kept in crystal cabinets.\n\n"
    "Witches are allowed to use all the powers of the "
    "basic orbs against intruders. These powers never expire, but a Witch "
    "can use only one power at a time (not counting Orbs of Life).\n\n"
    "Witches and Golems don't pursue you into other Lands. Also, most Orb powers "
    "are drained when you leave the Land of Power."
    )
  NATIVE((isWitch(m) || m == moEvilGolem) ? 1 : 0)
  REQ(ITEMS(itHell, U10))

LAND( 0xD0D0D0, "Camelot", laCamelot, ZERO | LF_CYCLIC, itHolyGrail, RESERVED, camelothelp )
  NATIVE((m == moKnight || m == moHedge || m == moFlailer || m == moLancer) ? 1 : 0)
  REQ(ITEMS(itEmerald, U5) ACCONLY3(laCrossroads, laCrossroads3, laCrossroads4))

LAND( 0xD000D0, "Temple of Cthulhu", laTemple, ZERO | LF_CYCLIC, itGrimoire, RESERVED, templehelp )
  NATIVE((m == moTentacle || m == moCultist || m == moPyroCultist || m == moCultistLeader) ? 1 : 0)
  REQ(ACCONLY(laRlyeh) ITEMS(itStatue, U5))

LAND( 0xFF8000, "Crossroads II", laCrossroads2, ZERO, itHyperstone, RESERVED, 
    "An alternate layout of the Crossroads. It is more dense and more regular, "
    "although you won't find the castle of Camelot here."
    )
  NATIVE(0)
  REQ(GOLD(R60))

LAND( 0x0000FF, "Caribbean", laCaribbean, ZERO | LF_SEA | LF_PURESEA, itPirate, RESERVED, caribbeanhelp)
  NATIVE((m == moPirate || m == moParrot || m == moCShark) ? 1 : 0)
  REQAS(laOcean,)

LAND( 0x400000, "Red Rock Valley", laRedRock, ZERO | LF_TROLL, itRedGem, RESERVED, redrockhelp )
  NATIVE((m == moRedTroll || m == moHexSnake) ? 2 : 0)
  REQ(GOLD(R60) ITEMS(itSpice, 10))

LAND( 0xD0D0D0, "Minefield", laMinefield, ZERO, itBombEgg, RESERVED, minedesc)
  NATIVE((m == moBomberbird || m == moTameBomberbird) ? 1 : 0)
  REQ(GOLD(R30))

LAND( 0x2020FF, "Ocean", laOcean, ZERO | LF_EQUI | LF_SEA | LF_COASTAL, itCoast, RESERVED, 
    "You can collect some valuable Ambers on the coast, but beware the tides!\n\n"
    "You can also take one of the boats and venture into the Ocean, "
    "to find other coasts, dangerous whirlpools, and islands inhabited by Pirates."
    )
  NATIVE(m == moAlbatross ? 2 : m == moPirate ? 1 : 0)
  REQ(GOLD(R30))

LAND( 0x0000C0, "Whirlpool", laWhirlpool, ZERO | LF_CYCLIC | LF_SEA, itWhirlpool, RESERVED, 
    "Many lost boats are spinning in this whirlpool. Some of them contain treasures "
    "and Pirates.\n\n"
    "Unmanned boats will go one cell clockwise in each turn. Sharks and manned boats "
    "are only allowed to go with the current, that is, towards the center, or clockwise."
    )
  NATIVE((m == moPirate || m == moCShark) ? 1 : 0)
  REQAS(laOcean, ACCONLY(laOcean))

LAND( 0xFFD500, "Palace", laPalace, ZERO, itPalace, RESERVED, palacedesc )
  NATIVE((m == moPalace || m == moFatGuard || m == moSkeleton || m == moVizier) ? 2 : 
        m == moSkeleton ? 1 : 0)
  REQ(GOLD(R30))

LAND( 0xC0C000, "Living Fjord", laLivefjord, ZERO | LF_SEA | LF_COASTAL | LF_TROLL, itFjord, RESERVED, 
    "A coastal area, from where you can get both to the inland worlds and to the Ocean. "
    "Each turn, each cell will become water or earth, based on the majority of cells around it. "
    "Contrary to the Living Caves, this process is not affected by most items and monsters, "
    "but elementals, dead Trolls, and cells outside of the Living Fjord have "
    "a very powerful effect."
     )
  NATIVE(m == moViking ? 2 : (m == moFjordTroll || m == moWaterElemental) ? 1 : 0)
  REQAS(laOcean,)

LAND( 0xFFFFE0, "Ivory Tower", laIvoryTower, ZERO | LF_GRAVITY | LF_EQUI, itIvory, RESERVED, "Powerful wizards claimed this part of the world, to perform their magical "
  "experiments in peace and solitude. They have changed the direction of gravity, "
  "to make it even harder for intruders to reach them.\n\n"
  "Gravity works as follows: cells are unstable if they are empty, and there is "
  "no cell immediately below them which contains a wall. It is impossible to move "
  "from one unstable cell to another, except if moving down.")
  NATIVE((m == moFamiliar || m == moGargoyle) ? 2 : 0)
  REQ(GOLD(R30))

LAND( 0x606060, "Zebra", laZebra, ZERO, itZebra, RESERVED, "Everything in this Land has black and white stripes.")
  NATIVE((m == moOrangeDog) ? 2 : 0)
  REQ(GOLD(R30) ITEMS(itFeather, U10))

LAND( 0xFFA080, "Plane of Fire", laEFire, ZERO | LF_ELEMENTAL, itElemental, RESERVED, elemdesc)
  NATIVE(m == moFireElemental ? 2 : isNative(laElementalWall, m))
  REQAS(laElementalWall,)

LAND( 0x808080, "Plane of Air", laEAir, ZERO | LF_ELEMENTAL, itElemental, RESERVED, elemdesc)
  NATIVE(m == moAirElemental ? 2 : isNative(laElementalWall, m))
  REQAS(laElementalWall,)

LAND( 0x80C080, "Plane of Earth", laEEarth, ZERO | LF_ELEMENTAL, itElemental, RESERVED, elemdesc)
  NATIVE(m == moEarthElemental ? 2 : isNative(laElementalWall, m))
  REQAS(laElementalWall,)

LAND( 0x8080C0, "Plane of Water", laEWater, ZERO | LF_ELEMENTAL, itElemental, RESERVED, elemdesc)
  NATIVE(m == moWaterElemental ? 2 : isNative(laElementalWall, m))
  REQAS(laElementalWall,)

LAND( 0x4040FF, "Crossroads III", laCrossroads3, ZERO, itHyperstone, RESERVED, 
    "An alternate layout of the Crossroads. Great Walls cross here at right angles."
    )
  NATIVE(0)
  REQ(NUMBER(orbsUnlocked(), lands_for_hell(), XLAT("Finished lands required: %1 (collect %2 treasure)\n", "9", its(R10))))

LAND( 0x4040C0, "Sea Border", laOceanWall, ZERO | LF_TECHNICAL | LF_SEA, itNone, RESERVED, "Border between seas.")
  // REQAS(laOcean,)
  REQ(NEVER)
  NATIVE(0)

LAND( 0x4040C0, "Elemental Planes", laElementalWall, ZERO | LF_ELEMENTAL, itElemental, RESERVED, elemdesc)
  NATIVE((m == moAirElemental || m == moEarthElemental || m == moWaterElemental || m == moFireElemental) ? 1 : 0)
  REQ(KILL(moAirElemental, laWhirlwind) KILL(moWaterElemental, laLivefjord) KILL(moEarthElemental, laDeadCaves) KILL(moFireElemental, laDragon))

LAND( 0xE08020, "Canvas", laCanvas, ZERO | LF_TECHNICAL, itNone, RESERVED, "A fake Land with colored floors.")
  NATIVE(0)
  REQ(NEVER)

LAND( 0x00C000, "Palace Quest", laPrincessQuest, ZERO, itSavedPrincess, RESERVED, princessdesc) // fake
  NATIVE(isNative(laPalace, m))
  REQ(ACCONLY(laPalace) KILL(moVizier, laPalace))

LAND( 0xD0D060, "Wild West", laWildWest, ZERO, itBounty, RESERVED, wildwestdesc)
  NATIVE((m == moOutlaw) ? 2 : 0)
  REQ( NEVER )

LAND( 0x80A080, "Land of Storms", laStorms, ZERO | LF_TROLL | LF_ELECTRIC, itFulgurite, RESERVED, elecdesc)
  NATIVE((m == moMetalBeast || m == moMetalBeast2 || m == moStormTroll) ? 1 : 0)
  REQ( GOLD(R60) )

LAND( 0x20A050, "Overgrown Woods", laOvergrown, ZERO | LF_TROLL, itMutant, RESERVED, overdesc)
  NATIVE((m == moMutant || m == moForestTroll) ? 1 : 0)
  REQ( GOLD(R60) ITEMS(itRuby, U10) )

LAND( 0x20D050, "Clearing", laClearing, ZERO | LF_CYCLIC, itMutant2, RESERVED, cleardesc)
  NATIVE((m == moMutant || m == moRedFox) ? 1 : 0)
  REQ(ACCONLY(laOvergrown) ITEMS(itMutant, U5))

LAND( 0x303030, "Haunted Woods", laHaunted, ZERO | LF_EQUI | LF_HAUNTED, itLotus, RESERVED, hauntdesc)
  NATIVE((m == moGhost || m == moFriendlyGhost) ? 1 : 0)
  REQ(ACCONLY(laGraveyard) ITEMS(itBone, U10))

LAND( 0x303030, "Haunted Woods", laHauntedWall, ZERO | LF_EQUI | LF_TECHNICAL | LF_HAUNTED, itLotus, RESERVED, hauntdesc)
  NATIVE(isNative(laHaunted, m))
  REQAS(laHaunted,)

LAND( 0x303030, "Haunted Woods", laHauntedBorder, ZERO | LF_EQUI | LF_TECHNICAL | LF_HAUNTED, itLotus, RESERVED, hauntdesc)
  NATIVE(isNative(laHaunted, m))
  REQAS(laHaunted,)

LAND( 0xC0C0FF, "Windy Plains", laWhirlwind, ZERO, itWindstone, RESERVED, winddesc)
  NATIVE((m == moAirElemental || m == moWindCrow) ? 2 : 0)
  REQ( GOLD(R60) )

LAND( 0x764e7c*2, "Rose Garden", laRose, ZERO, itRose, RESERVED, roselanddesc)
  NATIVE((m == moFalsePrincess || m == moRoseBeauty || m == moRoseLady) ? 2 : 0)
  REQ( GOLD(R90) )

LAND( 0xFFD580, "Warped Coast", laWarpCoast, ZERO | LF_WARPED | LF_COASTAL, itCoral, RESERVED, warplanddesc)
  NATIVE(m == moRatling ? 2 : m == moRatlingAvenger ? 1 : 0)
  REQAS(laOcean,)

LAND( 0xFFD580, "Warped Sea", laWarpSea, ZERO | LF_WARPED | LF_SEA | LF_COASTAL, itCoral, RESERVED, warplanddesc)
  NATIVE(isNative(laWarpCoast, m))
  REQAS(laWarpCoast,)

LAND( 0xC08080, "Crossroads IV", laCrossroads4, ZERO, itHyperstone, RESERVED, 
    "An alternate layout of the Crossroads, without walls."
    )
  NATIVE(0)
  REQ(GOLD(R200))

LAND( 0xFFD580, "Yendorian Forest", laEndorian, ZERO | LF_GRAVITY | LF_EQUI, itApple, RESERVED, 
    "This forest was planted by one of the wizards from the Ivory Tower "
    "to conduct experiments with gravity."
    )
  NATIVE((m == moResearcher || m == moSparrowhawk) ? 2 : 0)
  REQ(ITEMS(itIvory, U10))

LAND( 0x487830, "Galápagos", laTortoise, ZERO, itBabyTortoise, RESERVED, tortoisedesc)
  NATIVE(m == moTortoise ? 1 : 0)
  REQ(COND(tortoise::seek(), XLAT("Find a %1 in %the2.", itBabyTortoise, laDragon)) ACCONLY(laDragon))

LAND( 0xD04000, "Dragon Chasms", laDragon, ZERO, itDragon, RESERVED, dragondesc)
  NATIVE((isDragon(m) || m == moFireElemental) ? 1 : 0)
  REQ(NUMBER(killtypes(), R20, XLAT("Different kills required: %1.\n", its(R20))))

LAND( 0xD04000, "Kraken Depths", laKraken, ZERO | LF_SEA | LF_PURESEA, itKraken, RESERVED, 
    "A long time ago, this was a trade route. But then, Krakens have risen out of the "
    "depths. Many trading ships sank here. Legend says that you can uncover the secret "
    "of a magical weapon spell somewhere in the depths...\n\n"
    
    "You can find Sunken Treasures here, but they won't appear until you have killed "
    "a Kraken. You will also need Orb of the Fish to get the treasures, luckily you can "
    "steal one from the Viking treasure hunters."
    )
  NATIVE(m == moKrakenH ? 2 : (m == moViking || m == moKrakenT) ? 1 : 0)
  REQ(ITEMS(itFjord, U10))

LAND( 0x804020, "Burial Grounds", laBurial, ZERO, itBarrow, RESERVED, 
    "Ancient Viking heroes were buried here. Their graves have barrows raised over "
    "them, and are guarded by Draugar, animated corpses who are immune to mundane weapons. "
    "You will need to use a magical weapon spell to defeat them, and to rob the "
    "ancient jewelry buried in the graves."
    )
  NATIVE(m == moDraugr ? 1 : 0)
  REQ(ITEMS(itKraken, U10))

LAND( 0x90A548, "Trollheim", laTrollheim, ZERO, itTrollEgg, RESERVED, 
    "Many clans of Trolls spend their lives in this kingdom. You can find many "
    "statues of Trolls here. You suppose that they are not actually statues, but simply "
    "elderly Trolls, who have petrified upon death. Or maybe you have killed "
    "these Trolls yourself?"
    )
  NATIVE(isTroll(m) ? 1 : 0)
  REQ( KILL(moTroll, laCaves) KILL(moDarkTroll, laDeadCaves) KILL(moRedTroll, laRedRock) KILL(moStormTroll, laStorms) KILL(moForestTroll, laOvergrown) KILL(moFjordTroll, laLivefjord) )

LAND( 0xFF7518, "Halloween", laHalloween, ZERO, itTreat, RESERVED, halloweendesc)
  NATIVE((m == moGhost || m == moZombie || m == moWitch ||
        m == moLesser || m == moGreater || 
        m == moVampire || m == moDraugr ||
        m == moSkeleton || m == moWitchFire || 
        m == moFlailer || m == moPyroCultist || 
        m == moFatGuard || m == moHedge || 
        m == moLancer || m == moFireFairy || 
        m == moBomberbird || m == moRatlingAvenger || 
        m == moVineBeast || m == moDragonHead || m == moDragonTail) ? 1 : 0)
  REQ( NEVER )

LAND( 0x605040, "Dungeon", laDungeon, ZERO | LF_GRAVITY | LF_EQUI, itSlime, RESERVED, 
    "The result of a collaboration of the Great Vizier and the Wizard of the Ivory Tower."
    )
  NATIVE(m == moBat ? 2 : m == moSkeleton || m == moGhost ? 1 : 0)
  REQ(ITEMS(itPalace, U5) ITEMS(itIvory, U5))

LAND( 0x603000, "Lost Mountain", laMountain, ZERO | LF_GRAVITY | LF_CYCLIC, itAmethyst, RESERVED, 
    "Gravitational anomalies in the Jungle create mountains "
    "overgrown with ivies and bushes. "
    "Will you dare to climb the ivies to get the amethysts hidden above?\n\n"  
    "Cells adjacent to Ivies count as stable (but Ivies "
    "cannot climb themselves or other Ivies).")
  NATIVE(m == moEagle || m == moMonkey || isIvy(m) || m == moFriendlyIvy ? 1 : 0)
  REQ(ITEMS(itRuby, U5) ITEMS(itIvory, U5) ACCONLY(laJungle))

LAND( 0xFFFF00, "Reptiles", laReptile, ZERO, itDodeca, RESERVED, reptiledesc)
  NATIVE(m == moReptile ? 1 : 0)
  REQ(GOLD(R30) ITEMS(itElixir, U10))

LAND( 0x0000D0, "Prairie", laPrairie, ZERO, itGreenGrass, RESERVED, prairiedesc)
  NATIVE((m == moRagingBull || m == moHerdBull || m == moGadfly) ? 1 : 0)
  REQ(GOLD(R90))

LAND( 0x800080, "Bull Dash", laBull, ZERO, itBull, RESERVED, bulldashdesc)
  NATIVE((m == moSleepBull || m == moRagingBull || m == moButterfly || m == moGadfly) ? 1 : 0)
  REQ(GOLD(R90))

LAND( 0xC000C0, "Crossroads V", laCrossroads5, ZERO, itHyperstone, RESERVED, "Extremely narrow Crossroads layout.\n")
  NATIVE(0)
  REQ(GOLD(R300))

LAND( 0xC0C0C0, "Cellular Automaton", laCA, ZERO | LF_TECHNICAL, itNone, RESERVED, cadesc)
  NATIVE(0)
  REQ(NEVER)

LAND( 0xC0C0FF, "Mirror Wall", laMirrorWall, ZERO | LF_TECHNICAL | LF_INMIRRORORWALL, itShard, RESERVED, mirroreddesc)
  NATIVE(isNative(laMirror, m))

LAND( 0xC8C8FF, "Reflection", laMirrored, ZERO | LF_TECHNICAL | LF_INMIRROR | LF_INMIRRORORWALL, itShard, RESERVED, mirroreddesc)
  NATIVE(isNative(laMirror, m))
  REQAS(laMirror,)

LAND( 0xC0C0FF, "Reflection", laMirrorWall2, ZERO | LF_TECHNICAL | LF_INMIRROR | LF_INMIRRORORWALL, itShard, RESERVED, mirroreddesc)
  NATIVE(isNative(laMirror, m))
  REQAS(laMirror,)

LAND( 0xC8C8FF, "Reflection", laMirrored2, ZERO | LF_TECHNICAL | LF_INMIRROR | LF_INMIRRORORWALL, itShard, RESERVED, mirroreddesc)
  NATIVE(isNative(laMirror, m))
  REQAS(laMirror,)

LAND( 0xC8C8FF, "Mirror Land", laMirrorOld, ZERO | LF_MIRROR, itShard, RESERVED, 
    "A strange land which contains mirrors and mirages, protected by Mirror Rangers.")
  NATIVE((m == moEagle || m == moRanger || m == moMimic) ? 1 : 0)
  REQ(GOLD(R30))

LAND( 0xA06000, "Volcanic Wasteland", laVolcano, ZERO, itLavaLily, RESERVED, lavadesc)
  NATIVE((m == moLavaWolf || m == moSalamander) ? 2 : 0)
  REQ(GOLD(R60) ITEMS(itElixir, U10))

LAND( 0x8080FF, "Blizzard", laBlizzard, ZERO | LF_ICY, itBlizzard, RESERVED, blizzarddesc)
  NATIVE((m == moVoidBeast || m == moIceGolem) ? 2 : 0)
  REQ(ITEMS(itDiamond, U5) ITEMS(itWindstone, U5))

LAND( 0x207068, "Hunting Ground", laHunting, ZERO, itHunting, RESERVED, huntingdesc)
  NATIVE(m == moHunterDog ? 1 : 0)
  REQ(ALWAYS)

LAND( 0xE2725B, "Terracotta Army", laTerracotta, ZERO, itTerra, RESERVED, terraldesc)
  NATIVE(m == moJiangshi ? 2 : m == moTerraWarrior ? 1 : 0)
  REQ(GOLD(R90))

LAND( 0xE2725B, "Terracotta Army", laMercuryRiver, ZERO | LF_TECHNICAL, itTerra, RESERVED, terraldesc)
  NATIVE(isNative(laTerracotta, m))
  REQAS(laTerracotta,)
      
LAND( 0x80FF00, "Crystal World", laDual, ZERO, itGlowCrystal, RESERVED, crystaldesc)
  NATIVE(m == moRatling ? 2 : 0)
  REQ(GOLD(R90))

LAND( 0x306030, "Snake Nest", laSnakeNest, ZERO, itSnake, RESERVED, NODESCYET)
  NATIVE(m == moHexSnake ? 2 : among(m, moRedTroll, moMiner, moSkeleton, moBomberbird) ? 1 : 0)
  REQ(GOLD(R90))

LAND( 0x80FF00, "Docks", laDocks, ZERO | LF_SEA, itDock, RESERVED, NODESCYET)
  NATIVE(among(m, moRatling, moPirate, moCShark, moAlbatross, moFireFairy) ? 2 : 0)
  REQAS(laOcean,)

LAND( 0x306030, "Ruined City", laRuins, ZERO, itRuins, RESERVED, ruindesc)
  NATIVE(among(m, moPair, moHexDemon, moAltDemon, moMonk, moCrusher) ? 2 : m == moSkeleton ? 1 : 0)
  REQ(KILL(moSkeleton, laPalace))

LAND( 0x306030, "Magnetosphere", laMagnetic, ZERO, itMagnet, RESERVED, NODESCYET)
  NATIVE(isMagneticPole(m) ? 2 : 0)
  REQ(NEVER)

LAND( 0x306030, "Jelly Kingdom", laSwitch, ZERO, itSwitch, RESERVED, jellydesc)
  NATIVE(m == moSwitch1 || m == moSwitch2 ? 2 : 0)
  REQ(GOLD(R30) ITEMS(itElixir, U10))

LAND( 0xFF00FF, "Lost Memory", laMemory, ZERO | LF_TECHNICAL, itNone, RESERVED, 
    "Because of the properties of hyperbolic geometry, it is extremely unlikely to randomly "
    "get back to a faraway place you have been to. However, you have managed to get there "
    "somehow. In the meantime, its memory has been cleared, since the 'remove faraway cells from the memory'"
    " option was on."
    )
  NATIVE(0)
  REQ(NEVER)

LAND( 0x804000, "Brown Island", laBrownian, ZERO | LF_PURESEA, itBrownian, RESERVED, 
    "The Brown Islands have been generated by ancient underground creatures, who moved randomly and raised lands in their path... "
    "adults spawned larvae in each move, which also moved randomly and also raised lands in their path, and eventually became adults and spawned their own larvae. "
    "The creatures spawned exponentially, but the space they lived in still grew faster, so there was enough space for the whole spawn...\n\n"
    "That was a long time ago. Now, the mountainous Brown Islands are inhabited by strange monsters. Good luck adventuring!\n\n"
    "Parts of the Brown Islands have different elevations, from 0 to 3. It is impossible to gain two or more levels, "
    "or to lose three levels, in a single move, (attacks are possible at any "
    "difference, though). Killed Bronze Beasts rise the land by one level."
    )
  NATIVE(among(m, moBrownBug, moAcidBird) ? 2 : 0)
  REQAS(laOcean,)

LAND( 0x211F6F, "Free Fall", laWestWall, ZERO | LF_GRAVITY | LF_EQUI, itWest, RESERVED, 
    "What on one side looks to be a normal (well, infinite) horizontal wall, on the other side turns out to be the vertical wall"
    " of an infinitely high tower. Jump from the window, and let the magical gravity carry you..."
    )
  NATIVE(among(m, moWestHawk, moFallingDog) ? 2 : 0)
  REQ(ITEMS(itFeather, U5) ITEMS(itIvory, U5))

LAND( 0x30FF30, "Irradiated Field", laVariant, ZERO, itVarTreasure, RESERVED, 
    "These fields are ravaged with many kinds of magical radiation, which not only make the ground glow nicely in various colors, "
    "but also cause the inhabitants to protect the treasures of their land in various ways. In some areas of the Irradiated Fields, "
    "you will meet powerful enemies; in other areas, you will find arrow traps, fire traps, or pools, which can kill you if you are "
    "careless, or help you if you know how to use them to your advantage. Will you walk through the Irradiated Fields randomly, or "
    "try to find areas where treasures are common but nasty monsters are not, and keep to them? It is your choice!\n\n"
    )
  NATIVE(
        m == moVariantWarrior ? 2 :
        among(m, moMonk, moCrusher, moSkeleton, moHedge, moLancer, moFlailer, moCultist, moPyroCultist, moNecromancer, moGhost, moZombie, moRatling) ? 1 :
        isIvy(m) ? 1 : 0)
  #define LST {itRuins, itEmerald, itBone}
  REQ(ITEMS_TOTAL(LST, variant_unlock_value()))
  #undef LST

// add new content here

LAND( 0x202020, "Space Rocks", laAsteroids, ZERO, itAsteroid, RESERVED, rock_description)
ITEM( '!', 0xFFD0D0, "Fuel", itAsteroid, IC_TREASURE, ZERO, RESERVED, osNone, rock_description)
MONSTER('A', 0x606040, "Space Rock", moAsteroid, ZERO, RESERVED, moAsteroid, rock_description)
  NATIVE(m == moAsteroid ? 2 : 0)
  REQ( NEVER )
  
LAND( 0x00C0C0, "Wetland",    laWet, ZERO, itWet, RESERVED, 
  "Some people have definitely drowned in this treacherous area. Better be careful!"
  )
ITEM( '%', 0xFF6EC7, "Water Lily", itWet, IC_TREASURE, ZERO, RESERVED, osNone, 
  "A beautiful plant from the Wetland.")
WALL( '=', 0x00C0C0, "shallow water", waShallow, ZERO, RESERVED, 0, sgWater, 
  "Shallow water is passable both for land creatures and for water creatures."
  )
WALL( '=', 0x0000A0, "deep water", waDeepWater, WF_WATER, RESERVED, 0, sgWater, 
  "This body of water is calm and beautiful, but too deep to pass.")
MONSTER( 'P', 0xC08080, "Pike", moPike, CF_FACE_SIDE | CF_NOBLOW | CF_SHARK, RESERVED, moShark, 
  "You remember anglers from your hometown showing the impressive pikes they have caught. This one is much larger."
  )
MONSTER( 'T', 0xFF00FF, "World Turtle", moWorldTurtle, CF_FACE_SIDE | CF_STUNNABLE | CF_SLOWMOVER | CF_SPAM, RESERVED, moYeti, 
  "Have you reached the edge of the Poincaré disk? No, it must just be the edge of this world. "
  "Which happens to be floating on the backs of some turtles. Wondering how such not very big turtles could "
  "support such a large world? Well, there are LOTS of them! "
  "This one got so annoyed by the fact that you discovered this that it left its post. However, the world still "
  "stands, because there are so many turtles."
  )
MONSTER( 'R', 0x4040C0, "Rusałka", moRusalka, CF_FACE_SIDE | CF_SHARK | CF_HIGH_THREAT, RESERVED, moShark, GENDERSWITCH)
ITEM( 'o', 0x808080, "Orb of Plague", itOrbPlague, IC_ORB, IF_EMPATHY, RESERVED, osOffensive, 
  "When you kill an enemy, adjacent enemies are also attacked, as long as they are further away from you than from the originally attacked enemy. "
  "These enemies may in turn infect further enemies, up to distance of at most 4 from you."
  )
  NATIVE(among(m, moPike, moRusalka) ? 2 : 0)
  REQ( GOLD(R30) )

LAND( 0x6FA136, "Frog Park",     laFrog, ZERO, itFrog, RESERVED, 
  "This park is a perfect place for frogs to hide from predators. "
  "So perfect that the frogs have grown in size, gained magical abilities, "
  "and become very dangerous themselves."
  )
ITEM( '$', 0xFFD520, "Gold Ball", itFrog, IC_TREASURE, ZERO, RESERVED, osNone, 
  "A toy lost in the Frog Park by a spoiled child.")
ITEM( 'o', 0x808080, "Orb of Impact", itOrbImpact, IC_ORB, ZERO, RESERVED, osUtility, 
  "Whenever you use a ranged Orb to achieve an effect, all the single-tile monsters adjacent to the target "
  "location are stunned."
  )
WALL( '#', 0x00C000, "shrub",  waShrub, WF_WALL | WF_HIGHWALL | WF_STDTREE | WF_CONE, RESERVED, 0, sgNone, 
  "A strange small tree that cannot be attacked with mundane weapons nor phased though. "
  "It can be vaulted over, though."
  )
MONSTER('F', 0x60A060, "Giant Frog", moFrog, CF_HIGH_THREAT, RESERVED, moFrog, 
  "At first, you are shocked by the agility of this frog. Such a large creature "
  "should not be able to jump that quickly!\n\n"
  "Then, you notice the green glow around its legs. This frog must be magical... "
  "it has sacrificed its swimming abilities for superfrog jumping powers."
  )
MONSTER('F', 0xFFFF80, "Yellow Frog", moPhaser, CF_HIGH_THREAT, RESERVED, moPhaser, 
  "A slightly transparent yellow frog. It has mastered the magical power of phasing through solid "
  "obstacles such as rock."
  )
MONSTER('F', 0x8080FF, "Blue Frog", moVaulter, CF_HIGH_THREAT, RESERVED, moVaulter, 
  "This frog is able to vault over the shrubs in the Frog Park, destroying the shrub in the process. "
  "While it can also vault over monsters, it will never hurt the other frogs!"
  )
  
  NATIVE(among(m, moFrog, moPhaser, moVaulter) ? 2 : 0)  
  #define LST {itDodeca, itZebra, itSwitch}
  REQ(ITEMS_TOTAL(LST, variant_unlock_value()*4/5))
  #undef LST

LAND( 0xC0C0FF, "Eclectic City", laEclectic, LF_ICY | LF_ELECTRIC, itEclectic, RESERVED, 
  "Many kinds of creatures are living here, each with its own architectural style."
  )
MONSTER( 'W', 0xA04060, "Mutant2", moVariantWarrior2, CF_FACE_UP, RESERVED, moYeti, 
    "These guys look a bit strange, but they have no special properties."
    )
ITEM( '/', 0x26619c, "Lazurite Figurine", itEclectic, IC_TREASURE, ZERO, RESERVED, osNone, 
    "A beautiful blue figurine. Every figurine is different.")
ITEM( 'o', 0x808080, "Orb of Chaos", itOrbChaos, IC_ORB, ZERO, RESERVED, osTerraform, 
   "When you move, the contents of the two cells adjacent to your both locations are swapped. "
   "If they contain creatures, these creatures will be stunned for some time, before they understand what happened.\n\n"
   "Does not work if one of the cells contains a "
   "multi-tile creature or a barrier-like wall."
  )
  NATIVE(among(m, moRedTroll, moMiner, moTroll, moFireFairy, moMetalBeast, moPalace, moWolf, moIceGolem, moPair) ? 1 : 0)
  #define LST {itDiamond, itFulgurite, itPalace, itSilver}
  REQ(ITEMS_TOTAL(LST, variant_unlock_value()*4/3))
  #undef LST

WALL( '$', 0xFD692F, "Crate", waCrateCrate, WF_WALL | WF_PUSHABLE, RESERVED, 0, sgNone, 
    "These crates can be pushed."
    )

WALL( '.', 0x40FD40, "Target", waCrateTarget, 0, RESERVED, 0, sgNone, 
    "Push all your crates on targets."
    )

WALL( '$', 0x40FD40, "Crate on Target", waCrateOnTarget, WF_WALL | WF_PUSHABLE, RESERVED, 0, sgNone, 
    "A crate already on a target."
    )

ITEM( 'o', 0xF0F0FF, "Orb of Purity", itOrbPurity, IC_ORB, ZERO, RESERVED, osProtective, 
  "Reverses all the curses. When found far away from the Cursed Canyon, you gain low amounts of various reversed curse powers."
  )

ITEM('c', 0x202020, "Curse of Weakness", itCurseWeakness, IC_ORB, IF_CURSE, RESERVED, osOffensive, 
  "This Curse weakens your normal attack, which will only stun monsters. For monsters which are normally only stunned anyway, they don't push the victim away. You are also unable to cut trees or fight Ivy."
  )

ITEM('c', 0x6060FF, "Curse of Draining", itCurseDraining, IC_ORB, IF_CURSE, RESERVED, osPowerUtility, 
  "Drains the powers of all your orbs (and curses) by 2 charges per turn."
  )

ITEM('c', 0x000060, "Curse of Water", itCurseWater, IC_ORB, IF_CURSE, RESERVED, osTerraform, 
  "Makes you leave a trail of shallow water. You also become afraid of water. You are unable to step into water, including shallow water and boats on water. "
  "On the good side, you are protected from fire."
  )

ITEM('c', 0xFF6060, "Curse of Fatigue", itCurseFatigue, IC_ORB, IF_CURSE, RESERVED, osMovement, 
  "With this Curse you cannot move too quickly. Every move increases your fatigue by 1; fatigue cannot go over 10. "
  "Resting reduces your fatigue by 5."
  )

ITEM('c', 0xFFFF80, "Curse of Repulsion", itCurseRepulsion, IC_ORB, IF_CURSE, RESERVED, osUtility, 
  "If you have this Curse, all the items on adjacent cells will try to escape from you.")

ITEM('c', 0xD08080, "Curse of Gluttony", itCurseGluttony, IC_ORB, IF_CURSE, RESERVED, osNone, 
  "The first item you pick up is consumed without any effect."
  )

ITEM('>', 0xFF6060, "fatigue", itFatigue, IC_NAI, ZERO, RESERVED, osNone, 
    "See the Curse of Fatigue."
    )

MONSTER('H', 0x181818, "Canyon Hag", moHexer, CF_FACE_UP, RESERVED, moYeti,
  "A Canyon Hag can curse you from afar, if standing on top of a Curse and close enough.")

LAND(0x500050, "Cursed Canyon", laCursed, 0, itCursed, RESERVED,
  "This canyon is full of curses! Curses are like Orbs, but grant negative effects."
  )
  
NATIVE(m == moHexer ? 2 : 0)  
#define LST {itElixir, itPirate, itRuins, itBrownian, itPower}
REQ(ITEMS_TOTAL(LST, variant_unlock_value()*6/5))
#undef LST

ITEM('$', 0xda5e29, "Capon Stone", itCursed, IC_TREASURE, ZERO, RESERVED, osNone,
  "A lapis alectorius, alectoria or capon stone is a non-precious stone found in the gizzard of capons (young, castrated roosters). In magic it is believed to be an effective amulet, granting the wearer a heightened sense of courage and boldness. These properties are unproven. [Wikipedia]")

ITEM('o', 0x208020, "Orb of the Woods", itOrbWoods, IC_ORB, ZERO, RESERVED, osTerraform,
  "Lets you swap positions with the trees.")

LAND(0xC0FFC0, "Dice Reserve", laDice, 0, itDice, RESERVED,
  "This land is full of sentient dice. Do not disturb them!")
  NATIVE((m == moAnimatedDie || m == moAngryDie) ? 1 : 0)
  REQ(GOLD(R90))

ITEM('/', 0xD0D0D8, "Crystal Die", itDice, IC_TREASURE, ZERO, RESERVED, osNone,
  "A nice souvenir from the Dice Reserve. Make sure to collect the whole set!")

WALL('d', 0x7F6A30, "Unhappy Die", waRichDie, WF_WALL | WF_PUSHABLE, RESERVED, 0, sgNone, 
  "Sentient dice like to be in a position such that their highest number is on top. "
  "Unfortunately, someone has rolled this one into a wrong position, and did not fix this. "
  "It will reward you if you roll it so that the highest number is on top again!")

WALL('d', 0x106040, "Happy Die", waHappyDie, WF_WALL | WF_PUSHABLE, RESERVED, 0, sgNone, 
  "A happy sentient die. Dice are happy when they are in their correct position "
  "(the highest number on the top); happy dice with one roll from their best position are sometimes found too."
  "Other positions are much less convenient for them and expose their "
  "weak spots. You can roll Happy Dice, but it may become angry!"
  )

MONSTER('d', 0x603010, "Animated Die", moAnimatedDie, ZERO, RESERVED, moAnimatedDie, 
  "When sentient dice are too long in an incorrect position, they start to move on their own, "
  "and attack everyone. You can still convince Animated Dice of your good intentions by "
  "rolling them into a position such that the highest number is on top. "
  "If you do, the die will stop moving and (if it happens in the Dice Reserve) you will be rewarded. "
  "Other rolls and attacks are not allowed."
  )

MONSTER('d', 0x901020, "Angry Die", moAngryDie, ZERO, RESERVED, moAnimatedDie, 
  "You have made a die unhappy. Taste the revenge! This one won't forgive you, no matter what you do."
  )

//shmupspecials
MONSTER( '@', 0xC0C0C0, "Rogue", moPlayer, CF_FACE_UP | CF_PLAYER, RESERVED, moNone, "In the Shoot'em Up mode, you are armed with thrown Knives.")
MONSTER( '*', 0xC0C0C0, "Knife", moBullet, ZERO | CF_BULLET, RESERVED, moNone, "A simple, but effective, missile, used by rogues.")
MONSTER( '*', 0xFF0000, "Flail", moFlailBullet, ZERO | CF_BULLET, RESERVED, moNone, "This attack is likely to hit the attacker.")
MONSTER( '*', 0xFFFF00, "Fireball", moFireball, ZERO | CF_BULLET, RESERVED, moNone, "This magical missile burns whatever it hits.")
MONSTER( '*', 0xFFFF00, "Tongue", moTongue, ZERO | CF_BULLET, RESERVED, moNone, "Some monsters have long tongues, which allow them to attack enemies in nearby cells.")
MONSTER( '*', 0xFFFFFF, "Airball", moAirball, ZERO | CF_BULLET, RESERVED, moNone, "This magical missile pushes back whatever it hits.")
MONSTER( '*', 0x0060E0, "Blueball", moCrushball, ZERO | CF_BULLET, RESERVED, moNone, "A powerful missile from a Blue Raider.")

//technical/temporary
MONSTER( '?', 0x00C000, "dead bug", moDeadBug, ZERO | CF_TECHNICAL, RESERVED, moNone, NODESC)
// appears as 'killed by electric discharge'
MONSTER( '?', 0xFFFF00, "electric discharge", moLightningBolt, ZERO | CF_TECHNICAL, RESERVED, moNone, elecdesc)
MONSTER( '?', 0xE06000, "dead bird", moDeadBird, ZERO | CF_TECHNICAL, RESERVED, moNone, NODESC)
MONSTER( '?', 0xE06000, "Energy Sword", moEnergySword, ZERO | CF_TECHNICAL, RESERVED, moNone, NODESC)
MONSTER( '!', 0xFF0000, "Warning", moWarning, ZERO | CF_TECHNICAL, RESERVED, moNone, warningdesc)
MONSTER( '!', 0xFF0000, "arrow trap", moArrowTrap, ZERO | CF_BULLET | CF_TECHNICAL, RESERVED, moNone, arrowtrapdesc)
MONSTER( '*', 0,        "vertex", moRogueviz, ZERO | CF_TECHNICAL, RESERVED, moNone, "A vertex from rogueviz.")

#undef MONSTER
#undef LAND
#undef WALL
#undef ITEM
#undef NATIVE
#undef REQ
#undef REQAS

#undef GOLD
#undef ITEMS
#undef NEVER
#undef ALWAYS
#undef KILLS
#undef KILL
#undef AKILL
#undef ORD
#undef NUMBER
#undef COND
#undef ITEMS_TOTAL
#undef ACCONLY
#undef ACCONLY2
#undef ACCONLY3
#undef ACCONLYF
