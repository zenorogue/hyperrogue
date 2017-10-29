static const int motypes = 152;

struct monstertype {
  char  glyph;
  int   color;
  const char *name;
  const char *help;
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
  moMimic, moREMOVED, moGolem, moGolemMoved,
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
  moFamiliar, moGargoyle, moFireElemental, moAirElemental,
  moOrangeDog, moTentacleGhost,
  moMetalBeast, moMetalBeast2, moOutlaw, moMutant, 
  moStormTroll, moForestTroll, 
  moRedFox, moWindCrow, moFriendlyGhost, moRatling, moFalsePrincess, moRoseLady,
  moRoseBeauty, moRatlingAvenger,
  moTortoise, moDragonHead, moDragonTail,
  moGadfly, moResearcher, moSparrowhawk,
  moKrakenH, moKrakenT, moDraugr, moFriendlyIvy,
  moVampire, moBat, moReptile, 
  moHerdBull, moRagingBull, moSleepBull,
  moButterfly, moNarciss, moMirrorSpirit,
  moHunterDog, moTerraWarrior, moJiangshi, moVoidBeast, moLavaWolf, moHunterGuard,
  moIceGolem, moSandBird, moSalamander, moHunterChanging,
  // shmup specials
  moPlayer, moBullet, moFlailBullet, moFireball, moTongue, moAirball,
  // temporary
  moDeadBug, moLightningBolt, moDeadBird, moEnergySword, moWarning, moArrowTrap,
  moRogueviz
  };

struct genderswitch_t {
  int gender;
  eMonster m;
  const char *name;
  const char *desc;
  };

#define NUM_GS 6

static const int ittypes = 121;

struct itemtype {
  char  glyph;
  int   color;
  const char *name;
  const char *help;
  };

enum eItem { 
  itNone, itDiamond, itGold, itSpice, itRuby, itElixir, itShard, itBone, itHell, itStatue,
  itFeather, itSapphire, itHyperstone, itKey,
  itGreenStone, itOrbYendor,
  itOrbLightning, itOrbFlash, itOrbWinter, itOrbSpeed, itOrbLife, itOrbShield, itOrbDigging,
  itOrbTeleport, itOrbSafety,
  itOrbThorns, itFernFlower,
  itWine, itOrbAether, itSilver, itOrbPsi,
  itRoyalJelly, itEmerald, itOrbInvis, itPower, itOrbFire,
  itHolyGrail, itGrimoire,
  itOrbDragon, itOrbIllusion,
  itPirate, itCompass,
  itRedGem, itOrbTime, itOrbSpace,
  itBombEgg, itCoast, itWhirlpool,
  itOrbFriend, itOrbWater, itOrbAir,
  itPalace, itOrbFrog,
  itFjord, itOrbFish,
  itOrbDiscord,
  itSavedPrincess, itOrbLove,
  itIvory, itZebra, 
  itFireShard, itAirShard, itEarthShard, itWaterShard,
  itElemental, itOrbSummon, itOrbMatter,
  itBounty, itRevolver, itFulgurite, itMutant,
  itOrbStunning, itOrbLuck, 
  itMutant2, itOrbFreedom, itLotus, itOrbUndeath,
  itWindstone, itOrbEmpathy, itStrongWind, itBuggy, itBuggy2,
  itRose, itCoral, itOrbBeauty, itOrb37, itOrbEnergy,
  itBabyTortoise, itOrbShell, itApple, itDragon, itOrbDomination,
  itOrbSword, itKraken, itOrbSword2, itBarrow,
  itTrollEgg, itWarning, itOrbStone, itOrbNature, itTreat,
  itSlime, itAmethyst, itOrbRecall, itDodeca, itOrbDash, itGreenGrass, itOrbHorns,
  itOrbBull, itBull, itOrbMirror,
  itInventory,
  itLavaLily, itHunting, itBlizzard, itTerra,
  itOrbSide1, itOrbSide2, itOrbSide3,
  itOrbLava, itOrbMorph,
  };

static const int walltypes = 105;

struct walltype {
  char  glyph;
  int   color;
  const char *name;
  const char *help;
  };

enum eWall { waNone, waIcewall, waBarrier, waFloorA, waFloorB, waCavewall, waCavefloor, waDeadTroll, waDune,
  waMirror, waCloud, waThumperOff, waFire, waAncientGrave, waFreshGrave, waColumn, waSulphurC, waSulphur,
  waLake, waFrozenLake, waChasm, waChasmD, waBigTree, waSmallTree, 
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
  waCharged, waGrounded, waSandstone, waSaloon, waMetal,
  waDeadTroll2, waFan,
  waTemporary, waEarthD, waElementalTmp, waElementalD,
  waSlime1, waSlime2, waRose, waWarpGate,
  waTrunk, waSolidBranch, waWeakBranch, waCanopy,
  waBarrowWall, waBarrowDig,
  waPetrified, waTower,
  waBigBush, waSmallBush,
  waReptile, waReptileBridge,
  waInvisibleFloor,
  waMirrorWall,
  waPetrifiedBridge,
  waTempBridgeBlocked,
  waTerraWarrior, waBubble,
  waArrowTrap, waMercury, waMagma
  };

static const int landtypes = 77;

struct landtype {
  int color;
  const char *name;
  const char *help;
  };

enum eLand { laNone, laBarrier, laCrossroads, laDesert, laIce, laCaves, laJungle, laAlchemist, laMirror, laGraveyard,
  laRlyeh, laHell, laCocytus, laMotion, laDryForest, laEmerald, laWineyard, laDeadCaves, 
  laHive, laPower, laCamelot, laTemple, 
  laCrossroads2, laCaribbean, laRedRock, laMinefield, laOcean, laWhirlpool,
  laPalace, laLivefjord, 
  laIvoryTower, laZebra, laEFire, laEAir, laEEarth, laEWater, laCrossroads3,
  laOceanWall, laElementalWall, 
  laCanvas, laPrincessQuest,
  laWildWest, laStorms, laOvergrown, laClearing, 
  laHaunted, laHauntedWall, laHauntedBorder,
  laWhirlwind, laRose, laWarpCoast, laWarpSea, laCrossroads4,
  laEndorian, laTortoise, laDragon,
  laKraken, laBurial, laTrollheim,
  laHalloween, laDungeon, laMountain, laReptile,
  laPrairie, laBull, laCrossroads5, laCA,
  laMirrorWall, laMirrored, laMirrorWall2, laMirrored2,
  laMirrorOld,
  laVolcano, laBlizzard, laHunting, laTerracotta, laMercuryRiver
  };

enum eGeometry {gNormal, gEuclid, gSphere, gElliptic, gQuotient, gQuotient2, gTorus, gOctagon, g45, g46, g47, gSmallSphere, gTinySphere, gGUARD};

struct geometryinfo {
  const char* name;
  const char* shortname;
  int sides;
  int vertex;
  int quotientstyle;
  int cclass; // 0-hyperbolic, 1-Euclidean, 2-spherical
  int distlimit[2]; // truncated, non-truncated
  };

static const int qZEBRA = 1;
static const int qFIELD = 2;
static const int qELLIP = 4;
static const int qTORUS = 8;

// note: dnext assumes that x&7 equals 7
static const int SEE_ALL = 15;
static const int FORBIDDEN = -1;

extern eGeometry geometry;

extern geometryinfo ginf[gGUARD];
