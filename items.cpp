// Hyperbolic Rogue - items
// Copyright (C) 2011-2019 Zeno Rogue, see 'hyper.cpp' for details

/** \file item.cpp
 *  \brief routines related to items
 */

#include "hyper.h"

namespace hr {

EX int currentLocalTreasure;

/** for treasures, the number collected; for orbs, the number of charges */
EX array<int, ittypes> items;

EX map<modecode_t, array<int, ittypes> > hiitems;

EX bool cannotPickupItem(cell *c, bool telekinesis) {
  return itemHidden(c) && !telekinesis && !(isWatery(c) && markOrb(itOrbFish));
  }

EX bool canPickupItemWithMagnetism(cell *c, cell *from) {
  if(!c->item || c->item == itOrbYendor || isWall(c) || cannotPickupItem(c, false))
    return false;
  if(c->item == itCompass && from->item) 
    return false;
  return true;
  }

EX bool doPickupItemsWithMagnetism(cell *c) {
  /* repulsion works first -- Magnetism will collect the item if not repulsed */
  if(items[itCurseRepulsion])
    forCellIdEx(c3, i, c) if(c3->item) {
      cellwalker cw(c, i);
      cw += wstep;
      for(int j=1; j<c3->type; j++) {
        cell *c4 = (cw+j).peek();
        if(!isNeighbor(c, c4) && c3->item && !c4->item && passable(c4, c3, ZERO)) {
          changes.ccell(c3);
          changes.ccell(c4);
          c4->item = c3->item;
          moveEffect(movei(c3, c4, (cw+j).spin), moDeadBird);
          c3->item = itNone;
          markOrb(itCurseRepulsion);
          }
        }
      }
  cell *csaf = NULL;
  if(items[itOrbMagnetism])
    forCellEx(c3, c) if(canPickupItemWithMagnetism(c3, c)) {      
      changes.ccell(c3);
      changes.ccell(c);
      if(c3->item == itCompass) {
        if(!c->item)
          moveItem(c3, c, false);
        }
      else if(c3->item == itOrbSafety || c3->item == itBuggy || c3->item == itBuggy2)
        csaf = c3;
      else if(markOrb(itOrbMagnetism))
        collectItem(c3, c3, false);
      }
  if(csaf)
    return collectItem(csaf, csaf, false);
  return false;
  }

EX void pickupMovedItems(cell *c, cell *from) {
  if(!c->item) return;
  if(c->item == itOrbSafety) return;
  if(isPlayerOn(c)) collectItem(c, from, true);
  if(items[itOrbMagnetism])
    forCellEx(c2, c)
      if(isPlayerOn(c2) && canPickupItemWithMagnetism(c, c2)) {
        changes.ccell(c2);
        changes.ccell(c);
        collectItem(c, c2, true);
        }
  }

EX bool in_lovasz() {
  return specialland == laMotion && closed_or_bounded && ls::single() && !daily::on;
  }

EX int threshold_met(int i) {
  for(int a: {500, 250, 100, 50, 25, 10, 5})
    if(i >= a) return a;
  return 0;
  }

EX bool collectItem(cell *c2, cell *last, bool telekinesis IS(false)) {

  bool dopickup = true;
  bool had_choice = false;
  
  if(cannotPickupItem(c2, telekinesis))
    return false;

  if(items[itCurseGluttony] && c2->item) {
    addMessage(XLAT("%The1 is consumed!", c2->item));
    playSound(c2, "apple");
    items[itCurseGluttony] = 0;
    c2->item = itNone;
    return false;
    }
  
  /* if(c2->item == itHolyGrail && telekinesis)
    return false; */

  if(c2->item) {
    invismove = false;
    if(shmup::on) shmup::visibleFor(2000);
    string s0 = "";
    
    if(c2->item == itPalace && items[c2->item] == 12)
      changes.value_set(princess::forceVizier, true);
    
    if(!cantGetGrimoire(c2, false)) collectMessage(c2, c2->item);
    if(c2->item == itDodeca && peace::on) peace::simon::extend();
    }

  #if CAP_COMPLEX2
  if(c2->land == laHunting && c2->item && !inv::activating) {
    ambush::ambush(c2, ambush::size(c2, c2->item));
    }
  #endif
  
  if(isRevivalOrb(c2->item) && multi::revive_queue.size()) {
    multiRevival(last, c2);
    }
  else if(isShmupLifeOrb(c2->item) && shmup::on) {
    playSound(c2, "pickup-orb"); // TODO summon
    gainLife();
    }
  else if(orbcharges(c2->item)) {
    eItem it = c2->item;
    if(it == itOrbRecall && !dual::state) {
      cellwalker cw2 = cwt;
      cw2++;
      cw2.at = c2;
      saveRecall(cw2);
      }
    if(it == itOrbFire) playSound(c2, "fire");
    else if(it == itOrbFire) playSound(c2, "fire");
    else if(it == itOrbWinter) playSound(c2, "pickup-winter");
    else if(it == itOrbSpeed) playSound(c2, "pickup-speed");
    else if(it == itRevolver) playSound(c2, "pickup-key");
    else playSound(c2, "pickup-orb");
    had_choice = items[itOrbChoice];
    
    if(it == itOrbPurity) {
      bool no_curses = true;
      for(cell *c: dcal) if(c->land == laCursed) no_curses = false;
      if(no_curses) {
        items[itOrbSpeed] += 5;
        items[itOrbWinter] += 5;
        items[itOrbSlaying] += 5;
        items[itOrbTime] += 5;
        items[itOrbMagnetism] += 5;
        items[itOrbChoice] += 5;
        }
      }

    if(had_choice) items[itOrbChoice] = 0;

    int oc = orbcharges(it);
    if(dual::state && among(it, itOrbTeleport, itOrbFrog, itOrbPhasing, itOrbDash, itOrbRecall)) {
      oc = 10;
      it = itOrbSpeed;
      }
    if(c2->land == laAsteroids) oc = 10;
    if(markOrb(itOrbIntensity)) oc = intensify(oc);
    if(!items[it]) items[it]++;
    items[it] += oc;
    }
  else if(c2->item == itOrbLife) {
    playSound(c2, "pickup-orb"); // TODO summon
    placeGolem(last, c2, moGolem);
    if(cwt.at->monst == moGolem) cwt.at->stuntime = 0;
    }
  else if(c2->item == itOrbFriend) {
    playSound(c2, "pickup-orb"); // TODO summon
    placeGolem(last, c2, moTameBomberbird);
    if(cwt.at->monst == moTameBomberbird) cwt.at->stuntime = 0;
    }
#if CAP_TOUR
  else if(tour::on && (c2->item == itOrbSafety || c2->item == itOrbRecall)) {
    addMessage(XLAT("This Orb is not compatible with the Tutorial."));
    if(changes.on) changes.rollback();
    return true;
    }
#endif
  else if(c2->item == itOrbSafety) {

    if(changes.on) {
      if(changes.checking) {
        changes.rollback();
        return true;
        }
      changes.commit();
      }

    playSound(c2, "pickup-orb"); // TODO safety
    if(!dual::state) items[c2->item] = 7;
    if(shmup::on || multi::players > 1) {
      shmup::delayed_safety = true;
      shmup::delayed_safety_land = c2->land;
      c2->item = itNone;
      }
    else 
      activateSafety(c2->land);
    return true;
    }
  else if(c2->item == itBabyTortoise) {
    using namespace tortoise;
    int bnew = babymap[c2];
    changes.map_value(babymap, c2);
    babymap.erase(c2);
    int bold = seekbits;
    changes.value_set(tortoise::seekbits, bnew);
    changes.value_set(tortoise::last, seekbits);
    if(seek()) {
      cell *c = passable(last, NULL, 0) ? last : c2;
      changes.ccell(c);
      c->item = itBabyTortoise;
      if(c == c2) dopickup = false;
      changes.map_value(babymap, c);
      babymap[c] = bold;
      }
    else items[itBabyTortoise]++;
    }
  else if(c2->item == itOrbYendor && peace::on) {
    if(!items[itDodeca]) {
      addMessage(XLAT("Collect as many Dodecahedra as you can, then return here!"));
      }
    else {
      addMessage(XLAT("Your score: %1", its(items[itDodeca])));
      peace::simon::restore();
      }
    dopickup = false;
    }
  else if(c2->item == itOrbYendor && yendor::state(c2) != yendor::ysUnlocked) {
    dopickup = false;
    }
  else if(c2->item == itOrbYendor) 
    yendor::collected(c2);    
  else if(c2->item == itHolyGrail) {
    playSound(c2, "tada");
    int v = newRoundTableRadius() + 12;
    items[itOrbTeleport] += v;
    items[itOrbSpeed] += v;
    items[itHolyGrail]++;
    addMessage(XLAT("Congratulations! You have found the Holy Grail!"));
    if(!eubinary) changes.value_keep(c2->master->alt->emeraldval);
    if(!eubinary) c2->master->alt->emeraldval |= GRAIL_FOUND;
    achievement_collection(c2->item);
    }
  else if(c2->item == itKey) {
    playSound(c2, "pickup-key");
    for(int i=0; i<isize(yendor::yi); i++) if(yendor::yi[i].actual_key() == c2)
      changes.value_set(yendor::yi[i].found, true);
    items[itKey]++;
    }
  else if(!telekinesis && cantGetGrimoire(c2)) {
    // telekinesis checks the condition earlier
    dopickup = false;
    }
  else if(c2->item == itCompass) {
    dopickup = false;
    }
  else if(c2->item == itBuggy || c2->item == itBuggy2) {

    if(changes.on) {
      if(changes.checking) {
        changes.rollback();
        return true;
        }
      changes.commit();
      }

    items[itOrbSafety] += 7;
    if(shmup::on)
      shmup::delayed_safety = true;
    else {
      buggyGeneration = false;
      activateSafety(laCrossroads);
      }
    return true;
    }
  else if(c2->item == itTreat) {
    playSound(c2, "pickup-scroll");
    halloween::getTreat(c2);
    }   
  else {
    int q = items[c2->item];
    int q_el = items[itElemental];

    if(c2->item == itBarrow) 
      for(int i=0; i<c2->landparam; i++) gainItem(c2->item);
    else if(c2->item) gainItem(c2->item);

    if(c2->item && items[c2->item] > q && (vid.bubbles_all || (threshold_met(items[c2->item]) > threshold_met(q) && vid.bubbles_threshold))) {
      drawBubble(c2, iinf[c2->item].color, its(items[c2->item]), 0.5);
      }
    
    if(c2->item && items[itElemental] > q_el && (vid.bubbles_all || (threshold_met(items[itElemental]) > threshold_met(q_el) && vid.bubbles_threshold))) {
      drawBubble(c2, iinf[itElemental].color, its(items[itElemental]), 0.5);
      }

    if(c2->item) {
      char ch = iinf[c2->item].glyph;
      if(ch == '*') playSound(c2, "pickup-gem");
      else if(ch == '$' || ch == 'x') playSound(c2, "pickup-gold");
      else if(ch == '%' || ch == ';') playSound(c2, "pickup-potion");
      else if(c2->item == itApple) playSound(c2, "apple");
      else playSound(c2, "pickup-scroll");
      }
    }
  
  if(dopickup && c2->item) {
#if CAP_HISTORY
    // temporary variable to avoid the "cannot bind bitfield" problem in C++11
    eItem dummy = c2->item;
    history::findhistory.emplace_back(c2, dummy);
#endif

    if(c2->item == itBombEgg && c2->land == laMinefield) {
      c2->landparam |= 2;
      c2->landparam &= ~1;
      }

    if(!had_choice)
      c2->item = itNone;
    }
//    if(c2->land == laHive)
//      c2->heat = 1;

  int numOrb = 0;
  for(int i=0; i<ittypes; i++)
    if(itemclass(eItem(i)) == IC_ORB && items[i])
      numOrb++;
  if(numOrb) achievement_count("ORB", numOrb, 0);

  if(princess::reviveAt && gold(NO_LOVE) >= princess::reviveAt && !inv::on) {
    changes.value_set(princess::reviveAt, 0);
    items[itSavedPrincess] = 1;
    addMessage("You have enough treasure now to revive the Princess!");
    }

  return false;
  }

EX void glance_message() {
  if(gold() >= 300)
    addMessage(XLAT("You feel great, like a true treasure hunter."));
  else if(gold() >= 200)
    addMessage(XLAT("Your eyes shine like gems."));
  else if(gold() >= 100)
    addMessage(XLAT("Your eyes shine as you glance at your precious treasures."));
  else if(gold() >= 50)
    addMessage(XLAT("You glance at your great treasures."));
  else if(gold() >= 10)
    addMessage(XLAT("You glance at your precious treasures."));
  else if(gold() > 0)
    addMessage(XLAT("You glance at your precious treasure."));
  else
    addMessage(XLAT("Your inventory is empty."));
  }

EX void dropGreenStone(cell *c) {
  if(items[itGreenStone] && !passable(c, NULL, P_MONSTER)) {
    // NOTE: PL/CZ translations assume that itGreenStone is dropped to avoid extra forms!
    addMessage(XLAT("Cannot drop %the1 here!", itGreenStone));
    return;
    }
  if(items[itGreenStone] && c->item == itNone) {
    changes.ccell(c);
    changes.value_keep(items[itGreenStone]);
    items[itGreenStone]--;
    if(false) {
      c->item = itNone;
      spill(c, eWall(c->wall ^ waFloorA ^ waFloorB), 3);
      addMessage(XLAT("The slime reacts with %the1!", itGreenStone));
      }
    else {
      c->item = itGreenStone;
      addMessage(XLAT("You drop %the1.", itGreenStone));
      if(isHaunted(cwt.at->land)) 
        fail_survivalist();
      }
    }
  else {
    if(items[itGreenStone] && c->item == itGreenStone)
      addMessage(XLAT("You juggle the Dead Orbs."));
    else if(items[itGreenStone] && c->item)
      addMessage(XLAT("You give %the1 a grim look.", c->item));
    else if(items[itGreenStone]) {
      addMessage(XLAT("Cannot drop %the1 here!", itGreenStone));
      return;
      }
    else glance_message();
    }
  }

EX void moveItem1(cell *from, cell *to, bool activateYendor) {
  if(from->item == itOrbYendor) {
    bool xnew = true;
    for(int i=0; i<isize(yendor::yi); i++) 
      if(yendor::yi[i].path[0] == from) xnew = false;
    if(xnew && activateYendor) yendor::check(from);
    for(int i=0; i<isize(yendor::yi); i++) 
      if(yendor::yi[i].path[0] == from) 
        changes.value_set(yendor::yi[i].path[0], to);
    }

  if(from->item == itKey) {
    for(int i=0; i<isize(yendor::yi); i++) if(yendor::yi[i].path[YDIST-1] == from) {
      changes.value_set(yendor::yi[i].path[YDIST-1], to);
      }
    for(int i=0; i<isize(yendor::yi); i++) if(yendor::yi[i].actualKey == from)
      changes.value_set(yendor::yi[i].actualKey, to);
    }
  
  if(from->item == itBabyTortoise || to->item == itBabyTortoise) {
    tortoise::move_baby(from, to);
    }  

  eItem i = to->item;
  to->item = from->item;
  from->item = i;
  }

EX void moveItem (cell *from, cell *to, bool activateYendor) {
  static cell dummy;
  dummy.item = itNone;
  moveItem1(from, &dummy, activateYendor);
  moveItem1(to, from, activateYendor);
  moveItem1(&dummy, to, activateYendor);
  }

EX bool itemHidden(cell *c) {
  return isWatery(c) && !(shmup::on && shmup::boatAt(c));
  }

EX eItem localTreasureType() {
  lastland = singlepos()->land;
  return treasureType(lastland);
  }

EX void countLocalTreasure() {
  eItem i = localTreasureType();
  currentLocalTreasure = i ? items[i] : 0;
  if(i != itHyperstone) for(int i=0; i<isize(dcal); i++) {
    cell *c2 = dcal[i];
    if(c2->cpdist > (WDIM == 3 ? 1 : 3)) break;
    eItem i2 = treasureType(c2->land);
    if(i2 && items[i2] < currentLocalTreasure)
      currentLocalTreasure = items[i2];
    }
  }

#if HDR
static const int NO_TREASURE = 1;
static const int NO_YENDOR = 2;
static const int NO_GRAIL = 4;
static const int NO_LOVE = 8;
#endif

EX int gold(int no IS(0)) {
  int i = 0;
  if(!(no & NO_YENDOR)) i += items[itOrbYendor] * 50;
  if(!(no & NO_GRAIL)) i += items[itHolyGrail] * 10;
  if(!(no & NO_LOVE)) {
    bool love = items[itOrbLove];
#if CAP_INV
    if(inv::on && inv::remaining[itOrbLove])
      love = true;
#endif
#if CAP_DAILY
    if(daily::on) love = false;
#endif
    if(love) i += 30;
    }
  
  if(!(no & NO_TREASURE)) 
    for(int t=0; t<ittypes; t++) 
      if(itemclass(eItem(t)) == IC_TREASURE)
        i += items[t];
  return i;
  }

EX int maxgold() {
  int mg = 0;
  for(int i=0; i<ittypes; i++) 
    if(itemclass(eItem(i)) == IC_TREASURE && items[i] > mg) 
      mg = items[i];
  return mg;
  }

EX void updateHi_for_code(eItem it, int v, modecode_t xcode) {
  if(!yendor::on)
    if(v > hiitems[xcode][it]) 
      changes.value_set(hiitems[xcode][it], v);
  }

EX void updateHi(eItem it, int v) {
  updateHi_for_code(it, v, modecode());
  }

EX void gainItem(eItem it) {
  int g = gold();
  bool lhu = landUnlocked(laHell);
  items[it]++; if(it != itLotus) updateHi(it, items[it]);
  if(it == itRevolver && items[it] > 6) items[it] = 6;
  achievement_collection(it);
  multi::treasures[multi::cpid]++;
#if CAP_DAILY
    if(daily::on) achievement_final(false);
#endif

  int g2 = gold();
  if(items[itFireShard] && items[itAirShard] && items[itWaterShard] && items[itEarthShard]) {
    items[itFireShard]--;
    items[itAirShard]--;
    items[itWaterShard]--;
    items[itEarthShard]--;
    gainItem(itElemental);
    gainItem(itElemental);
    gainItem(itElemental);
    gainItem(itElemental);
    addMessage(XLAT("You construct some Elemental Gems!", it) + itemcounter(items[itElemental]));
    playSound(cwt.at, "elementalgem");
    }          

  if(it == itBounty) 
    items[itRevolver] = 6;
      
  if(it == itHyperstone && items[itHyperstone] == 10)
    achievement_victory(true);

  if(ls::any_chaos() && gold() >= 300)
    achievement_gain_once("CHAOS", rg::chaos);

#if ISMOBILE
  if(g < lastsafety + R30*3/2 && g2 >= lastsafety + R30*3/2)
    addMessage(XLAT("The Orb of Safety from the Land of Eternal Motion might save you."));
#endif
  
#define IF(x) if(g < (x) && g2 >= x && !peace::on)

  if(in_full_game()) {
    IF(R60/4) 
      addMessage(XLAT("Collect treasure to access more different lands..."));
    IF(R30)
      addMessage(XLAT("You feel that you have enough treasure to access new lands!"));
    IF(R30*3/2)
      addMessage(XLAT("Collect more treasures, there are still more lands waiting..."));
    IF(R60)
      addMessage(XLAT("You feel that the stars are right, and you can access R'Lyeh!"));
    IF(R30*5/2)
      addMessage(XLAT("Kill monsters and collect treasures, and you may get access to Hell..."));
    IF(R10 * 9) 
      addMessage(XLAT("To access Hell, collect %1 treasures each of %2 kinds...", its(R10), its(lands_for_hell())));
    if(landUnlocked(laHell) && !lhu) {
      addMessage(XLAT("Abandon all hope, the gates of Hell are opened!"));
      addMessage(XLAT("And the Orbs of Yendor await!"));
      }
    }
  }

EX string itemcounter(int qty) {
  string s = ""; s += " ("; s += its(qty); s += ")"; return s;
  }

EX void gainShard(cell *c2, const char *msg) {
  invismove = false;
  string s = XLAT(msg);
  if(is_mirrorland(c2) && !peace::on) {
     collectMessage(c2, itShard);
     gainItem(itShard);
     if(vid.bubbles_all || (threshold_met(items[itShard]) == items[itShard] && vid.bubbles_threshold)) {
       drawBubble(c2, iinf[itShard].color, its(items[itShard]), 0.5);
       }
     s += itemcounter(items[itShard]);
     }
  addMessage(s);
  c2->wall = waNone;
  invismove = false;
  }

EX void placeItems(int qty, eItem it) {
  int dcs = isize(dcal);
  for(int i=1; qty && i<dcs; i++) {
    cell *c = dcal[i];
    if(!c->monst && !c->item && passable(c, NULL, 0))
      c->item = it, qty--;
    }
  }

EX bool cantGetGrimoire(cell *c2, bool verbose IS(true)) {
  if(ls::any_chaos()) return false;
  if(!eubinary && !c2->master->alt) return false;
  if(c2->item == itGrimoire && items[itGrimoire] > celldistAlt(c2)/-temple_layer_size()) {
    if(verbose)
      addMessage(XLAT("You already have this Grimoire! Seek new tomes in the inner circles."));
    return true;
    }
  return false;
  }

EX void gainLife() {
  items[itOrbLife] ++;
  if(items[itOrbLife] > 5 && !inv::on) items[itOrbLife] = 5;
  }

EX void collectMessage(cell *c2, eItem which) {
  bool specialmode = yendor::on || princess::challenge || cheater || !in_full_game();
  
  if(which == itDodeca && peace::on) return;  
  if(which == itTreat) ;    
  else if(isElementalShard(which)) {
    int tsh = 
      items[itFireShard] + items[itAirShard] + items[itWaterShard] + items[itEarthShard] +
      items[itElemental];
    if(tsh == 0) {
      addMessage(XLAT("Collect four different Elemental Shards!"));
      addMessage(XLAT("Unbalanced shards in your inventory are dangerous."));
      }
    else {
      string t = XLAT("You collect %the1. (%2)", which, its(items[which]+1));
      addMessage(t);
      }
    }
  else if(which == itKey) {
    addMessage(XLAT("You have found the Key! Now unlock this Orb of Yendor!"));
    }
  else if(which == itGreenStone && !items[itGreenStone])
    addMessage(XLAT("This orb is dead..."));
  else if(which == itGreenStone)
    addMessage(XLAT("Another Dead Orb."));
  else if(itemclass(which) != IC_TREASURE) {
    if(!inv::activating)
      addMessage(XLAT("You have found %the1!", which));
    }
  else if(which == itBabyTortoise) {
    playSound(c2, playergender() ? "speak-princess" : "speak-prince");
    addMessage(XLAT("Aww, poor %1... where is your family?", which));
    }
  else if(gold() == 0 && !specialmode)
    addMessage(XLAT("Wow! %1! This trip should be worth it!", which));
  else if(gold() == 1 && !specialmode)
    addMessage(XLAT("For now, collect as much treasure as possible..."));
  else if(gold() == 2 && !specialmode)
    addMessage(XLAT("Prove yourself here, then find new lands, with new quests..."));
  else if(!items[which] && itemclass(which) == IC_TREASURE)
    addMessage(XLAT("You collect your first %1!", which));
  else if(items[which] == 4 && maxgold() == U5-1 && !specialmode) {
    addMessage(XLAT("You feel that %the2 become%s2 more dangerous.", which, c2->land));
    addMessage(XLAT("With each %1 you collect...", which, c2->land));
    }
  else if(items[which] == 9 && maxgold() == 9 && !specialmode) {
    if(inv::on) {
      addMessage(XLAT("The treasure gives your magical powers!", c2->land));
      if(!ISMOBILE) 
        addMessage(XLAT("Press 'i' to access your magical powers.", c2->land));
      }
    else
      addMessage(XLAT("Are there any magical orbs in %the1?...", c2->land));
    }
  else if(items[which] == R10 && maxgold() == R10 && !specialmode && !inv::on) {
    addMessage(XLAT("You feel that %the1 slowly become%s1 dangerous...", c2->land));
    addMessage(XLAT("Better find some other place."));
    }
  else if(which == itHunting && items[itHunting] == 4 && !specialmode && !ISMOBWEB)
    addMessage(XLAT("Hint: hold Alt to highlight enemies and other important features."));
  else if(which == itSpice && items[itSpice] == U10*7/10 && !specialmode && isLandIngame(laHell))
    addMessage(XLAT("You have a vision of the future, fighting demons in Hell..."));
  else if(which == itSpice && items[itSpice] == U10-1 && !specialmode && isLandIngame(laRedRock))
    addMessage(XLAT("You will be fighting red rock snakes, too..."));
  else if(which == itKraken && items[itKraken] == U10-1 && !specialmode)
    addMessage(XLAT("You feel that a magical weapon is waiting for you..."));
//  else if(which == itFeather && items[itFeather] == 10)
//    addMessage(XLAT("There should be a Palace somewhere nearby..."));
  else if(which == itElixir && items[itElixir] == U5-1 && !specialmode)
    addMessage(XLAT("With this Elixir, your life should be long and prosperous..."));
  else if(which == itRuby && items[itRuby] == U5-1 && !specialmode && isLandIngame(laMountain)) {
    addMessage(XLAT("You feel something strange about gravity here..."));
    }
  else if(which == itPalace && items[itPalace] == U5-1 && !specialmode && isLandIngame(laDungeon)) {
    addMessage(XLAT("The rug depicts a man in a deep dungeon, unable to leave."));
    }
  else if(which == itFeather && items[itFeather] == 25-1 && !specialmode && inv::on && !ls::any_chaos())
    addMessage(XLAT("You feel the presence of free saves on the Crossroads."));
  else if(which == itHell && items[itHell] == 25-1 && !specialmode && inv::on && !ls::any_chaos())
    addMessage(XLAT("You feel the Orbs of Yendor nearby..."));
  else if(which == itHell && items[itHell] == 50-1 && !specialmode && inv::on && !ls::any_chaos())
    addMessage(XLAT("You feel the Orbs of Yendor in the Crossroads..."));
  else if(which == itHell && items[itHell] == 100-1 && !specialmode && inv::on && !ls::any_chaos())
    addMessage(XLAT("You feel the Orbs of Yendor everywhere..."));
  else if(which == itBone && items[itBone] % 25 == 24 && !specialmode && inv::on)
    addMessage(XLAT("You have gained an offensive power!"));
  else if(which == itHell && items[itHell] >= 100 && items[itHell] % 25 == 24 && !specialmode && inv::on)
    addMessage(XLAT("A small reward for braving the Hell."));
  else if(which == itIvory && items[itIvory] == U5-1 && !specialmode && (isLandIngame(laMountain) || isLandIngame(laDungeon))) {
    addMessage(XLAT("You feel attuned to gravity, ready to face mountains and dungeons."));
    }
  else if(which == itBone && items[itBone] == U5+1 && !specialmode && isLandIngame(laHell))
    addMessage(XLAT("The Necromancer's Totem contains hellish incantations..."));
  else if(which == itStatue && items[itStatue] == U5+1 && !specialmode)
    addMessage(XLAT("The inscriptions on the Statue of Cthulhu point you toward your destiny..."));
  else if(which == itStatue && items[itStatue] == U5-1 && isLandIngame(laTemple))
    addMessage(XLAT("There must be some temples of Cthulhu in R'Lyeh..."));
  else if(which == itDiamond && items[itDiamond] == U10-2 && !specialmode)
    addMessage(XLAT("Still, even greater treasures lie ahead..."));
  else if(which == itFernFlower && items[itFernFlower] == U5-1 && isLandIngame(laEmerald))
    addMessage(XLAT("You overheard Hedgehog Warriors talking about emeralds..."));
  else if(which == itEmerald && items[itEmerald] == U5-1 && !specialmode && isLandIngame(laCamelot))
    addMessage(XLAT("You overhear miners talking about a castle..."));
  else if(which == itEmerald && items[itEmerald] == U5 && !specialmode && isLandIngame(laCamelot))
    addMessage(XLAT("A castle in the Crossroads..."));
  else if(which == itShard) ;
  else {
    int qty = (which == itBarrow) ? c2->landparam : 1;
    string t;
    if(which == itBarrow && items[which] < 25 && items[which] + qty >= 25)
      t = XLAT("Your energy swords get stronger!");
    else if(maxgold() < 25 && items[which] + qty >= 25)
      t = XLAT("You feel even more attuned to the magic of this land!");
    else t = XLAT("You collect %the1. (%2)", which, its(items[which]+qty));
    addMessage(t);
    }
  }

EX bool itemHiddenFromSight(cell *c) {
  return isWatery(c) && !items[itOrbInvis] && !(items[itOrbFish] && playerInWater())
    && !(shmup::on && shmup::boatAt(c));
  }

}
