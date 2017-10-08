#define ORBLINES 56

struct orbinfo {
  eLand l;
  int lchance;
  int gchance;
  eItem orb;
  };

const orbinfo orbinfos[ORBLINES] = {
  {laGraveyard, 200, 200,itGreenStone}, // must be first so that it does not reduce 
  // chance of other orbs
  {laJungle, 1200, 1500,itOrbLightning},
  {laIce, 2000, 1500,itOrbFlash},
  {laCaves, 1800, 2000,itOrbLife},
  {laAlchemist, 800, 800,itOrbSpeed},
  {laDesert, 2500, 1500,itOrbShield},
  {laHell, 2000, 1000,itOrbYendor},
  {laRlyeh, 1500, 1500,itOrbTeleport},
  {laMotion, 2000, 700, itOrbSafety},    
  {laIce, 1500, 0, itOrbWinter},
  {laDragon, 2500, 0, itOrbWinter},
  {laDryForest, 2500, 0, itOrbWinter}, 
  {laCocytus, 1500, 1500, itOrbWinter}, 
  {laCaves, 1200, 0, itOrbDigging},
  {laDryForest, 500, 4500, itOrbThorns},
  {laDeadCaves, 1800, 0, itGreenStone},
  {laDeadCaves, 1800, 1500, itOrbDigging},
  {laEmerald, 1500, 3500, itOrbPsi},
  {laWineyard, 900, 1200, itOrbAether},
  {laHive, 800, 1200, itOrbInvis},
  {laPower, 0, 3000, itOrbFire},
  {laMinefield, 0, 3500, itOrbFriend},
  {laTemple, 0, 3000, itOrbDragon},
  {laCaribbean, 0, 3500, itOrbTime},
  {laRedRock, 0, 2500, itOrbSpace},
  {laCamelot, 1000, 1500, itOrbIllusion},
  {laOcean, 0, 3000, itOrbEmpathy},
  {laOcean, 0, 0, itOrbAir},
  {laPalace, 0, 4000, itOrbDiscord},
  {laPalace, 0, 0, itOrbFrog},
  {laZebra, 500, 2100, itOrbFrog},
  {laLivefjord, 0, 1800, itOrbFish},
  {laPrincessQuest, 0, 200, itOrbLove},
  {laIvoryTower, 500, 4000, itOrbMatter},
  {laElementalWall, 1500, 4000, itOrbSummon},
  {laStorms, 1000, 2500, itOrbStunning},
  {laOvergrown, 1000, 800, itOrbLuck},
  {laWhirlwind, 1250, 3000, itOrbAir},
  {laHaunted, 1000, 5000, itOrbUndeath},
  {laClearing, 5000, 5000, itOrbFreedom},
  {laRose, 2000, 8000, itOrbBeauty},
  {laWarpCoast, 2000, 8000, itOrb37},
  {laDragon, 500, 5000, itOrbDomination},
  {laTortoise, 2500, 1500, itOrbShell},
  {laEndorian, 150, 2500, itOrbEnergy},
  {laEndorian, 450, 0, itOrbTeleport},
  {laKraken, 500, 2500, itOrbSword},
  {laBurial, 500, 2500, itOrbSword2},
  {laTrollheim, 750, 1800, itOrbStone},
  {laMountain, 400, 3500, itOrbNature},
  {laDungeon, 120, 2500, itOrbRecall},
  {laReptile, 500, 2100, itOrbDash},
  {laBull, 720, 3000, itOrbHorns},
  {laPrairie, 0, 3500, itOrbBull},
  {laWhirlpool, 0, 0, itOrbSafety},
  {laWhirlpool, 0, 2000, itOrbWater}, // must be last because it generates a boat
  };

eItem orbType(eLand l) {
  if(isElemental(l)) l = laElementalWall;
  if(inv::on && (l == laMirror || l == laMirrorOld || isCrossroads(l)))
    return itOrbMirror;
  if(l == laMirror || l == laMirrorOld) return itShard;
  for(int i=0; i<ORBLINES; i++) 
    if(orbinfos[i].l == l && orbinfos[i].gchance)
      return orbinfos[i].orb;
  return itNone;
  }

const orbinfo& getOrbInfo(eItem orb) {
  for(int i=0; i<ORBLINES; i++) 
    if(orbinfos[i].orb == orb && orbinfos[i].gchance)
      return orbinfos[i];
  static orbinfo oi; 
  oi.l = laMirror;
  return oi;
  }

enum eOrbLandRelation { 
  olrForbidden, // never appears: forbidden
  olrDangerous, // never appears: would be dangerous
  olrUseless,   // never appears: useless here
  olrNoPrizes,  // no prizes in this land
  olrNoPrizeOrb,// orb not allowed as a prize
  olrPrize25,   // prize for collecting 25
  olrPrize3,    // prize for collecting 3
  olrNative,    // native orb in this land
  olrNative1,    // native orb in this land (1)
  olrGuest,     // extra orb in this land
  olrPNative,   // Land of Power: native
  olrPBasic,    // Land of Power: basic orbs
  olrPPrized,   // Land of Power: prized orbs
  olrPNever,    // Land of Power: foreign orbs
  olrHub,       // hub lands
  olrMonster,   // available from a monster
  olrAlways     // always available
  };

string olrDescriptions[] = {
  "forbidden to find in %the1",
  "too dangerous to use in %the1",
  "useless in %the1",
  "only native Orbs allowed in %the1",
  "this Orb is never unlocked globally (only hubs)",
  "collect 25 %2 to unlock it in %the1",
  "collect 3 %2 to unlock it in %the1",
  "native to %the1 (collect 10 %2)",
  "native to %the1 (collect 1 %2)",
  "secondary in %the1 (collect 10 %3, or 25 %2)",
  "the native Orb of %the1",
  "this Orb appears on floors and is used by witches",
  "a prized Orb, it appears only in cabinets",
  "this Orb never appears in %the1",
  "Hub Land: orbs appear here if unlocked in their native land",
  "kill a monster, or collect 25 %2",
  "always available"
  };

eOrbLandRelation getOLR(eItem it, eLand l) {

  if(l == laPower) {
    if(it == itOrbFire) return olrPNative;

    if(
      it == itOrbFlash || it == itOrbSpeed || it == itOrbWinter || it == itOrbAether ||
      it == itOrbLife) return olrPBasic;

    if(
      it == itOrbLightning || it == itOrbThorns || it == itOrbInvis ||
      it == itOrbShield || it == itOrbTeleport || it == itOrbPsi ||
      it == itOrbDragon || it == itOrbIllusion || it == itOrbTime)
        return olrPPrized;
    
    return olrPNever;
    }
  
  if(it == itOrbSafety && l == laCrossroads5) return olrDangerous;
  if(it == itOrbFire && l == laKraken) return olrUseless;
  if(it == itOrbDragon && l == laKraken) return olrUseless;
  if(it == itOrbDigging && l == laKraken) return olrUseless;
  if(it == itOrbIllusion && l == laKraken) return olrUseless;
  
  // if(it == itOrbYendor && l == laWhirlpool) return olrUseless;
  if(it == itOrbYendor && l == laWhirlwind) return olrUseless;
  
  if(it == itOrbLife && (l == laKraken)) return olrUseless;
  
  if(it == itOrbAir && l == laAlchemist) return olrUseless;
  // if(it == itOrbShield && l == laMotion) return olrUseless;

  if(it == itOrbIllusion && l == laCamelot) return olrNative1;
  if(it == itOrbLove) return olrNoPrizeOrb;    
  if(orbType(l) == it) return olrNative;
  if(it == itOrbWinter && (l == laIce || l == laDryForest || l == laDragon))
    return olrGuest;
  if(it == itOrbLuck && l == laIvoryTower)
    return olrUseless;
  if(it == itOrbLuck && l == laEndorian)
    return olrUseless;
  if(it == itOrbLuck && l == laDungeon)
    return olrUseless;
  if(it == itOrbWater && l == laRedRock)
    return olrUseless;
  if(it == itOrbLuck && l == laTortoise)
    return olrUseless;
  if(it == itOrbLuck && l == laMountain)
    return olrUseless;
   if(it == itOrbLuck && l == laCamelot)
    return olrUseless;
   if(it == itOrbLuck && l == laHaunted)
    return olrUseless;
   if(it == itOrbNature && l == laWineyard)
    return olrDangerous;
  if(it == itOrbDigging && l == laCaves)
    return olrGuest;
  if(it == itOrbFrog && (l == laPalace || l == laPrincessQuest))
    return olrGuest;
  if(it == itOrbDragon && l == laRlyeh)
    return olrMonster;
    
  if(it == itOrbSafety && l == laWhirlpool)
    return olrAlways;
  if(it == itOrbSafety && l == laPrairie)
    return olrAlways;
  if(it == itGreenStone && isHaunted(l))
    return olrAlways;
  if(it == itOrbWater && l == laLivefjord)
    return olrMonster;
  if(isCrossroads(l) || l == laOcean)
    return olrHub;
  
  if(l == laCocytus)
    if(it == itOrbDragon || it == itOrbFire || it == itOrbFlash || it == itOrbLightning)
      return olrDangerous;
  
  if(it == itOrbSafety)
    if(l == laCaves || l == laLivefjord || l == laRedRock || l == laCocytus || l == laHell ||
      l == laDesert || l == laAlchemist || l == laDeadCaves || l == laMinefield || isHaunted(l) ||
      l == laDragon) 
      return olrDangerous;
    
  if(it == itOrbMatter)
    if(l == laCaves || l == laEmerald || l == laAlchemist || l == laCaribbean || 
      l == laMinefield || l == laCocytus) return olrUseless;

  if(l == laPrincessQuest)
    if(it == itOrbAether || it == itOrbFlash || it == itOrbTeleport || it == itOrbSummon || it == itOrbFreedom)
      return olrForbidden;
    
  if(l == laTemple)
    return olrNoPrizes;
  
  if(it == itOrbDigging) {
    if(l == laCaves || l == laOcean || l == laLivefjord || l == laEmerald ||
      l == laDesert || l == laDeadCaves || l == laRedRock || l == laCaribbean || l == laGraveyard ||
      l == laMountain)
        return olrPrize25;
    return olrUseless;
    }
  
  if(it == itShard) {
    if(l == laDesert || l == laIce || l == laJungle || l == laGraveyard ||
      l == laRlyeh || l == laHell || l == laDryForest || l == laWineyard ||
      l == laHive || l == laCamelot || l == laRedRock || l == laPalace ||
      l == laLivefjord || l == laZebra || isElemental(l) || l == laPrincessQuest ||
      l == laDragon || l == laTortoise || l == laBurial || l == laTrollheim ||
      l == laOcean || l == laHaunted || l == laWarpCoast || l == laRose)
      return olrPrize25;
    return olrForbidden;
    }

  if(it == itOrbWater) 
    if(l == laMotion || l == laZebra || l == laIvoryTower || l == laEndorian ||
      l == laMountain || l == laReptile || l == laDungeon)
      return olrUseless;
  
  if(it == itOrbWinter && l == laMinefield)
    return olrForbidden;
  
  if(it == itOrbWinter && l != laRlyeh && l != laTemple) 
    return olrUseless;
  
  if(it == itOrbLife && l == laMotion)
    return olrUseless;
  
  if(it == itOrbFish && l == laKraken)
    return olrAlways;
    
  if(it == itOrbSword && l == laBurial)
    return olrAlways;
    
  if(it == itOrbFish && l != laOcean && l != laLivefjord && l != laWhirlpool && l != laCamelot &&
    l != laTortoise)
    return olrUseless;

  if(it == itOrbDomination && l != laOcean && l != laRedRock && l != laDesert &&
    l != laRlyeh && l != laDragon)
    return olrUseless;
  
  if(it == itOrbIllusion) return olrPrize3;
  
  if(l == laTortoise)
    if(it == itOrbFlash || it == itOrbLightning || it == itOrbFreedom ||
      it == itOrbPsi || it == itOrbFriend || it == itOrbDragon)
      return olrForbidden;
    
  if(l == laEndorian)
    if(it == itOrbDragon || it == itOrbFire || it == itOrbLightning)
      return olrDangerous;
    
  if(l == laDungeon) {
    if(it == itOrbSafety || it == itOrbFrog || 
      it == itOrbTeleport || it == itOrbMatter || it == itOrbNature ||
      it == itOrbAether || it == itOrbSummon || it == itOrbStone) 
      return olrForbidden;
    }
  
  return olrPrize25;
  }

int orbsUnlocked() {
  int i = 0;
  for(int t=0; t<ittypes; t++) 
    if(itemclass(eItem(t)) == IC_TREASURE && items[t] >= R10)
      i++;
  return i;
  }

bool hellUnlocked() {
  return orbsUnlocked() >= 9;
  }

ld orbprizefun(int tr) {
  if(tr < 10) return 0;
  return .6 + .4 * log(tr/25.) / log(2);
  }

ld orbcrossfun(int tr) {
  if(tr < 10) return 0;
  if(tr > 25) return 1;
  return (tr*2 + 50) / 100.;
  }

bool buildPrizeMirror(cell *c, int freq) {
  if(inv::on) return false;
  if(c->type == 7 && !purehepta) return false;
  if(items[itShard] < 25) return false;
  if(freq && hrand(freq * 100 / orbprizefun(items[itShard])) >= 100)
    return false;
  c->wall = hrand(2) ? waCloud : waMirror;
  return true;
  }                    

eLand getPrizeLand(cell *c = cwt.c) {
  eLand l = c->land;
  if(isElemental(l)) l = laElementalWall;
  if(l == laPalace && princess::dist(c) < OUT_OF_PRISON)
    l = laPrincessQuest;
  return l;
  }

void placePrizeOrb(cell *c) {
  if(peace::on) return;
  
  eLand l = getPrizeLand(c);

  // these two lands would have too much orbs according to normal rules
  if(l == laPalace && hrand(100) >= 20) return;
  if(l == laPrincessQuest && hrand(100) >= 20) return;
  if(l == laGraveyard && hrand(100) >= 15) return;
  if(l == laBurial && hrand(100) >= 10) return;
  if(l == laLivefjord && hrand(100) >= 35) return;
  if(l == laMinefield && hrand(100) >= 25) return;
  if(l == laElementalWall && hrand(100) >= 25) return;

  for(int i=0; i<ORBLINES; i++) {
    const orbinfo& oi(orbinfos[i]);

    if(inv::on) {
      if(oi.orb == itOrbYendor && items[itHell] >= 100) ;
      else continue;
      }

    eOrbLandRelation olr = getOLR(oi.orb, l);
    if(olr != olrPrize25 && olr != olrPrize3) continue;
    int treas = items[treasureType(oi.l)];
    if(olr == olrPrize3) treas *= 10;
    if(olr == olrPrize25 || olr == olrPrize3 || olr == olrGuest || olr == olrMonster || olr == olrAlways) {
      if(treas < 25) continue;
      } 
    else continue;

    int gch = oi.gchance;
    if(!gch) continue;
    gch = int(gch / orbprizefun(treas));
    if(hrand(gch) >= 60) continue;
    if(oi.orb == itOrbWater && c->land != laOcean && c->land != laKraken) {
      if(cellHalfvine(c)) continue;
      c->item = oi.orb;
      c->wall = waStrandedBoat;
      return;
      }
    c->item = oi.orb;
    }
  
  // printf("land: %s orb: %s\n", dnameof(l), dnameof(c->item));
  }

void placeLocalOrbs(cell *c) {
  eLand l = c->land;
  if(l == laZebra && c->wall == waTrapdoor) return;
  if(isGravityLand(l) && cellEdgeUnstable(c)) return;
  if(isElemental(l)) l = laElementalWall;
  if(peace::on) return;
  
  for(int i=0; i<ORBLINES; i++) {
    const orbinfo& oi(orbinfos[i]);
    if(oi.l != l) continue;
    if(inv::on) {
      if(oi.orb != itOrbYendor) continue;
      if(items[itHell] < 25) continue;
      }
    if(yendor::on && (oi.orb == itOrbSafety || oi.orb == itOrbYendor))
      continue;
    if(!oi.lchance) continue;
    int ch = hrand(oi.lchance);
    if(ch == 1 && chaosmode && hrand(2) == 0 && items[treasureType(oi.l)] * landMultiplier(oi.l) >= (11+hrand(15)))
      ch = 0;
    if(tactic::trailer && ch < 5) ch = 0;
    if(ch == 0 && items[treasureType(oi.l)] * landMultiplier(oi.l) >= (chaosmode ? 1+hrand(10) : 10)) {
      // printf("local orb\n");
      c->item = oi.orb;
      if(oi.orb == itOrbWater && c->land != laOcean) c->wall = waStrandedBoat;
      return;
      }
    else if(oi.gchance && (ch >= 1 && ch < 11) && getOLR(itShard, l) == olrPrize25 && l != laRedRock && l != laWhirlwind)
      buildPrizeMirror(c, 10);
    else if(oi.gchance && (ch >= 11 && ch < 11+PRIZEMUL)) 
      placePrizeOrb(c);
    }
  }

void placeCrossroadOrbs(cell *c) {  
  if(peace::on) return;
  for(int i=0; i<ORBLINES; i++) {
    const orbinfo& oi(orbinfos[i]);
    if(!oi.gchance) continue;

    if(inv::on) {
      if(oi.orb == itOrbYendor && items[itHell] >= 50) ;
      else if(oi.orb == itOrbSafety && items[itFeather] >= 25) ;
      else continue;
      }
    int treas = items[treasureType(oi.l)] * landMultiplier(oi.l);
    if(tactic::on && isCrossroads(tactic::lasttactic)) {
      if(oi.orb == itOrbYendor || oi.orb == itOrbSummon || oi.orb == itOrbFish || oi.orb == itOrbDigging || oi.orb == itOrbLove || oi.orb == itOrbLuck)
        continue;
      }
    else {
      if(treas < 10) continue;
      }
    if(oi.orb == itOrbSafety && c->land == laCrossroads5) continue;
    int mul = c->land == laCrossroads5 ? 10 : 1;
    int gch = oi.gchance;
    if(!inv::on) gch /= orbcrossfun(treas); else gch /= 2;
    if(hrand(gch) >= mul) continue;
    if(hrand(50+items[itHyperstone]) >= 50) continue;
    c->item = oi.orb;
    if(oi.orb == itOrbWater && c->land != laOcean) c->wall = waStrandedBoat;
    }
  }

void placeOceanOrbs(cell *c) {  
  if(peace::on) return;
  for(int i=0; i<ORBLINES; i++) {
    const orbinfo& oi(orbinfos[i]);

    if(inv::on) {
      if(oi.orb == itOrbYendor && items[itHell] >= 50) ;
      else if(oi.orb == itOrbSafety && items[itFeather] >= 25) ;
      else continue;
      }

    if(items[treasureType(oi.l)] * landMultiplier(oi.l) < 10) continue;
    if(!oi.gchance) continue;
    if(oi.orb == itOrbLife) continue; // useless
    if(hrand(oi.gchance) >= 20) continue;
    c->item = oi.orb;
    }
  }

