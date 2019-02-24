namespace hr {

typedef unsigned color_t;

typedef unsigned long long flagtype;
#define Flag(i) (flagtype(1ull<<i))

static inline void set_flag(flagtype& f, flagtype which, bool b) {
  if(b) f |= which;
  else f &= ~which;
  }

static const int motypes = 167;

struct monstertype {
  char  glyph;
  color_t color;
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
  moNorthPole, moSouthPole,
  moPair, moHexDemon, moAltDemon, moMonk, moCrusher,
  moSwitch1, moSwitch2,
  moBrownBug, moAcidBird, moVariantWarrior,
  moFallingDog, moWestHawk, 
  // shmup specials
  moPlayer, moBullet, moFlailBullet, moFireball, moTongue, moAirball, moCrushball,
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

static const int ittypes = 136;

struct itemtype {
  char  glyph;
  color_t color;
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
  itOrbLava, itOrbMorph, itGlowCrystal, itSnake,
  itDock, itRuins, itMagnet, itSwitch,
  itOrbPhasing, itOrbMagnetism, itOrbSlaying,
  itBrownian, itWest, itVarTreasure,
  itOrbIntensity, itOrbGravity, itOrbChoice
  };

static const int walltypes = 111;

struct walltype {
  char  glyph;
  color_t color;
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
  waArrowTrap, waMercury, waMagma,
  waDock, waBurningDock, waRuinWall, waBrownian,
  waFireTrap, waExplosiveBarrel
  };

static const int landtypes = 87;

struct landtype {
  color_t color;
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
  laVolcano, laBlizzard, laHunting, laTerracotta, laMercuryRiver,
  laDual, laSnakeNest, laDocks, laRuins, laMagnetic,
  laSwitch, laMemory, laBrownian, laWestWall, laVariant
  };

enum eGeometry {
  gNormal, gEuclid, gSphere, gElliptic, gZebraQuotient, gFieldQuotient, gTorus, gOctagon, g45, g46, g47, gSmallSphere, gTinySphere, gEuclidSquare, gSmallElliptic, 
  gKleinQuartic, gBolza, gBolza2, gMinimal, gBinaryTiling, gArchimedean, 
  gMacbeath, gBring, gSchmutzM2, gSchmutzM3, gCrystal, gOctahedron, gBinary3, gCubeTiling,
  gGUARD};

enum eGeometryClass { gcHyperbolic, gcEuclid, gcSphere };

enum class eVariation { bitruncated, pure, goldberg, irregular, dual };  

struct geometryinfo {
  const char* tiling_name;
  const char* quotient_name;
  const char* menu_displayed_name;
  const char* shortname;
  int sides;
  int vertex;
  flagtype flags;
  eGeometryClass cclass;
  int xcode;
  std::array<int,2> distlimit; // bitrunc, non-bitrunc
  eVariation default_variation;
  };

static const flagtype qBOUNDED         = 1;
static const flagtype qANYQ            = 2;
static const flagtype qNONORIENTABLE   = 4;
static const flagtype qSMALL           = 8;

static const flagtype qFIELD           = 16;
static const flagtype qDOCKS           = 32;
static const flagtype qZEBRA           = 64;

// note: dnext assumes that x&7 equals 7
static const int SEE_ALL = 50;
static const int FORBIDDEN = -1;

extern eGeometry geometry;
extern eVariation variation;

extern std::vector<geometryinfo> ginf;

extern monstertype minf[motypes];
extern itemtype iinf[ittypes];

extern const landtype linf[landtypes];
extern color_t floorcolors[landtypes];

enum cpatterntype { 
  cpFootball, cpThree, cpChess, cpSingle, cpSingleSym, cpOddEven, cpLarge, cpZebra, cpUnknown
  };
  
struct landtacinfo { eLand l; int tries, multiplier; };

enum eModel {
  mdDisk, mdHalfplane, mdBand, mdPolygonal, mdFormula,
  mdEquidistant, mdEquiarea, mdBall, mdHyperboloid, 
  mdHemisphere, mdBandEquidistant, mdBandEquiarea, mdSinusoidal, mdTwoPoint, 
  mdFisheye, mdJoukowsky, mdJoukowskyInverted,
  mdRotatedHyperboles, mdSpiral,
  mdGUARD, mdUnchanged, mdHyperboloidFlat, mdPolynomial
  };

typedef unsigned long long flagtype;

namespace mf {
  static const flagtype azimuthal = 1;
  static const flagtype band = 2 + 512;
  static const flagtype equiarea = 4;
  static const flagtype equidistant = 8;
  static const flagtype conformal = 16;
  static const flagtype euc_boring = 32;
  static const flagtype space = 64;
  static const flagtype hyper_only = 128;
  static const flagtype hyper_or_torus = 256;
  static const flagtype quasiband = 512;
  };
  
struct modelinfo {
  const char *name_hyperbolic;
  const char *name_euclidean;
  const char *name_spherical;  
  
  flagtype flags;

  int is_azimuthal;
  int is_band;
  int is_equiarea;
  int is_equidistant;
  int is_conformal;
  const char* name;
  };

extern const modelinfo models[int(mdPolynomial)+1];

}
