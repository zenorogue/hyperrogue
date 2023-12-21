// Hyperbolic Rogue
// Copyright (C) 2011-2019 Zeno Rogue

// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

/** \file crossbow.cpp
 *  \brief implementation of the crossbow mode
 */

#include "hyper.h"

namespace hr {

EX namespace bow {

#if HDR
enum eWeapon { wBlade, wCrossbow };
enum eCrossbowStyle { cbBull, cbGeodesic, cbGeometric };
#endif

EX eWeapon weapon;
EX eCrossbowStyle style;
EX bool bump_to_shoot = true;

EX bool crossbow_mode() { return weapon == wCrossbow; }

#if HDR
struct bowpoint {
  cellwalker prev, next;
  flagtype flags;
  bowpoint() {}
  };

struct bowscore {
  int total;
  cellwalker last;
  int turns;
  };
#endif

#if HDR
static constexpr flagtype bpFIRST = 1;
static constexpr flagtype bpLAST = 2;
static constexpr flagtype bpCOPIED = 4;
#endif

EX vector<bowpoint> bowpath;

EX map<cell*, vector<bowpoint>> bowpath_map;

EX map<int, cell*> target_at;

EX int loading_time() {
  return style == cbBull ? 3 : 4;
  }

EX bool blocks(cell *c) {
  if(items[itOrbAether]) return false;
  if(isWall(c) && !among(c->wall, waMirrorWall, waMirror, waCloud) && !isNonblock(c->wall)) return true;
  // if(c->monst && isMultitile(c->monst)) return true;
  return false;
  }

EX int qadd(cellwalker a, cellwalker b) {
  hassert(a.at == b.at);
  for(int i=0; i<a.at->type; i++) if(a+i == b) return i;
  return NODIR;
  }

int best_score_res;

EX int bolt_score(cellwalker cw2) {
  int d = cw2.at->cpdist;
  int ntotal = 2;
  if(inmirror(cw2.at)) cw2 = mirror::reflect(cw2);
  if(blocks(cw2.cpeek())) return -1;
  if(thruVine(cw2.at, cw2.cpeek())) return -1;
  if(nonAdjacent(cw2.at, cw2.cpeek())) return -1;

  if(cw2.at->monst) {
    flagtype attackflags = AF_BOW;
    if(items[itOrbSpeed]&1) attackflags |= AF_FAST;
    if(items[itOrbSlaying]) attackflags |= AF_CRUSH;
    if(items[itCurseWeakness]) attackflags |= AF_WEAK;
    if(canAttack(cw2.cpeek(), moPlayer, cw2.at, cw2.at->monst, attackflags)) {
      ntotal += 10000; ntotal += 1280 >> d;
      }
    }

  for(int t=0; t<cw2.at->type; t++) {
    cell *c1 = cw2.at->cmove(t);
    if(!logical_adjacent(cw2.cpeek(), moPlayer, c1)) continue;
    if(canAttack(cw2.cpeek(),moPlayer,c1,c1->monst,AF_STAB)) {
      ntotal += 10000; ntotal += 1280 >> d;
      }
    }

  return ntotal;
  }

EX vector<int> create_dirseq() {
  map<cell*, bowscore> scores;
  scores[cwt.at].total = 0;

  int best_score = -1; cell* best_score_at = cwt.at;

  for(cell *c: dcal) {
    cell *c1 = target_at[c->cpdist];
    if(c1 && c != c1) continue;
    if(c == c1) { best_score = -1; }
    bowscore best;
    best.total = -1;
    forCellIdEx(c1, i, c) if(c1->cpdist < c->cpdist && scores.count(c1)) {
      auto& last = scores[c1];
      auto ocw2 = cellwalker(c, i);
      int bonus = bolt_score(ocw2);
      if(bonus < 0) continue;
      int ntotal = last.total + bonus;

      int dir = 0;
      if(c->cpdist > 1) {
        dir = qadd(last.last, ocw2+wstep);
        int d = abs(szgmod(dir, c1->type));
        if(d != c1->type / 2) {
          if(style == cbGeodesic) ntotal--;
          if(style == cbBull) continue;
          }
        }
      else {
        dir = qadd(cwt, ocw2+wstep);
        }
      if(ntotal > best.total) {
        best.total = ntotal;
        best.last = ocw2;
        best.turns = dir;
        }
      best.total = max(best.total, ntotal);
      }
    if(best.total > best_score) { best_score = best.total; best_score_at = c; }
    if(best.total > -1) scores[c] = best;
    }

  if(best_score == -1) return {};

  vector<int> dirseq = { NODIR };
  while(best_score_at != cwt.at) { 
    auto& at = scores[best_score_at];
    dirseq.push_back(at.turns);
    best_score_at = at.last.cpeek();
    }
  reverse(dirseq.begin(), dirseq.end());

  best_score_res = best_score;
  return dirseq;
  }

EX vector<int> create_dirseq_geometric() {
  cell *tgt = nullptr;
  for(auto t: target_at) tgt = t.second;
  if(!tgt) return {};
  hyperpoint h = tC0(currentmap->relative_matrix(tgt, cwt.at, C0));
  transmatrix T = spintox(h);

  best_score_res = 0;
  cellwalker at = cwt;
  vector<int> dirseq;
  while(true) {
    int best_i = -1;
    ld best_y = HUGE_VAL;
    for(int i=0; i<at.at->type; i++) {
      cellwalker at1 = at + i;
      if(at1.cpeek()->cpdist != at.at->cpdist + 1) continue;
      transmatrix U = T * currentmap->adj(at.at, at1.spin);
      hyperpoint U0 = tC0(U);
      hyperpoint T0 = tC0(T);
      if(U0[0] < T0[0] + 1e-6) continue;
      ld y;
      if(GDIM == 3) y = hypot(U0[1], U0[2]); else y = abs(U0[1]) + (U0[1] > 0 ? 1e-6 : 0);
      if(y < best_y) { best_y = y; best_i = i; }
      }
    if(best_i < 0) break;
    at = at + best_i;
    int bonus = bolt_score(at + wstep);
    if(bonus < 0) break;
    best_score_res += bonus;
    dirseq.push_back(best_i);
    T = T * currentmap->adj(at.at, at.spin);
    at = at + wstep;
    }

  return dirseq;
  }

EX int create_path() {
  auto dirseq = style == cbGeometric ? create_dirseq_geometric() : create_dirseq();
  if(dirseq.empty()) return -1;

  struct bolt {
    cellwalker at;
    flagtype flags;
    bolt(cellwalker cw, flagtype f) { at = cw; flags = f; }
    };

  bowpath.clear();
  vector<bolt> bolts = { bolt(cwt, 0) };
  for(auto m: mirror::mirrors) bolts.emplace_back(m.second, bpCOPIED);

  set<cell*> broken_mirrors;

  for(auto d: dirseq) {
    bool first = bowpath.empty();
    vector<bolt> nbolts;
    set<cell*> next_broken_mirrors = broken_mirrors;
    for(auto bolt: bolts) {
      bowpath.emplace_back();
      auto& p = bowpath.back();
      p.prev = bolt.at;
      p.flags = bolt.flags;
      if(first) p.flags |= bpFIRST;
      if(d == NODIR || blocks(bolt.at.at)) { p.next = bolt.at; p.flags |= bpLAST; }
      else {
        if(inmirror(bolt.at.at) || (bolt.at.at->wall == waMirrorWall && inmirror((bolt.at+d).cpeek())))
          bolt.at = mirror::reflect(bolt.at);
        bolt.at += d;
        p.next = bolt.at;
        bolt.at += wstep;
        if(among(bolt.at.at->wall, waCloud, waMirror) && !broken_mirrors.count(bolt.at.at)) {
          auto &mir = mirror::mirrors;
          vector<pair<int, cellwalker>> bmir;
          swap(mir, bmir);
          mirror::createHere(bolt.at, 0);
          swap(mir, bmir);
          for(auto b: bmir) nbolts.emplace_back(b.second, bolt.flags);
          next_broken_mirrors.insert(bolt.at.at);
          }
        nbolts.push_back(bolt);
        }
      }
    bolts = nbolts;
    broken_mirrors = next_broken_mirrors;
    }

  return best_score_res;
  }

EX void clear_bowpath() {
  bowpath_map.clear();
  }

EX void gen_bowpath_map() {
  bowpath_map = {};
  for(auto& b: bowpath) bowpath_map[b.next.at].push_back(b);
  }

EX bool auto_path() {
  target_at = {};
  target_at[1] = cwt.cpeek();
  return create_path() >= 10000;
  }

EX bool fire_mode;

EX void switch_fire_mode() {
  if(!crossbow_mode()) { addMessage(XLAT("You fire an angry glance at your enemies.")); return; }
  if(!fire_mode) {
    addMessage(items[itCrossbow] ? XLAT("Note: cannot fire again yet. Turns to reload: %1.", its(items[itCrossbow])) : XLAT("Fire crossbow! Click to aim, click again to confirm."));
    fire_mode = true;
    clear_bowpath();
    target_at = {};
    }
  else if(fire_mode) {
    addMessage(items[itCrossbow] ? XLAT("Fire mode disabled.") : XLAT("Firing cancelled."));
    fire_mode = false;
    clear_bowpath();
    }
  }

EX void add_fire(cell *c) {
  bool emp = target_at.empty();
  auto& t = target_at[c->cpdist];
  if(t == c && !bow::bowpath.empty()) {
    if(items[itCrossbow]) {
      clear_bowpath();
      addMessage(XLAT("Cannot fire again yet. Turns to reload: %1.", its(items[itCrossbow])));
      fire_mode = false;
      return;
      }
    clear_bowpath();
    checked_move_issue = miVALID;
    pcmove pcm;
    pcm.checkonly = false;
    changes.init(false);
    addMessage(XLAT("Fire!"));
    bool b = pcm.try_shooting(false);
    if(!b) changes.rollback();
    fire_mode = false;
    }
  else {
    t = c;
    if(style == cbGeometric) { target_at = {}; target_at[c->cpdist] = c; }
    int res = create_path();
    if(res == -1) {
      if(!emp) {
        target_at = {};
        add_fire(c);
        }
      else {
        addMessage(XLAT("No way to hit this place."));
        bow::bowpath = {};
        }
      }
    gen_bowpath_map();
    }
  }

#if HDR
enum eMouseFireMode { mfmNone, mfmPriority, mfmAlways };
#endif

EX eMouseFireMode mouse_fire_mode = mfmPriority;

EX bool fire_on_mouse(cell *c) {
  if(!crossbow_mode()) return false;
  if(mouse_fire_mode == mfmNone) return false;
  if(!mouseover) return false;
  if(!mouseover->monst) return false;
  if(shmup::on) return false;
  if(items[itCrossbow]) {
    if(mouse_fire_mode == mfmAlways) {
      addMessage(XLAT("Cannot fire again yet. Turns to reload: %1.", its(items[itCrossbow])));
      return true;
      }
    return false;
    }
  target_at = {};
  target_at[mouseover->cpdist] = mouseover;
  int res = create_path();
  if(res <= 0) {
    if(mouse_fire_mode == mfmAlways) {
      addMessage(XLAT("Shooting impossible."));
      return true;
      }
    return false;
    }
  gen_bowpath_map();
  checked_move_issue = miVALID;
  pcmove pcm;
  pcm.checkonly = false;
  changes.init(false);
  addMessage(XLAT("Fire!"));
  bool b = pcm.try_shooting(false);
  if(!b) changes.rollback();
  if(mouse_fire_mode == mfmAlways) return true;
  return b;
  }

EX void shoot() {
  flagtype attackflags = AF_BOW;
  if(items[itOrbSpeed]&1) attackflags |= AF_FAST;
  if(items[itOrbSlaying]) attackflags |= AF_CRUSH;
  if(items[itCurseWeakness]) attackflags |= AF_WEAK;

  vector<bowpoint> pushes;

  // for achievements
  set<eMonster> kills;
  vector<pair<cell*, int>> healthy_dragons;
  map<cell*, pair<int, int>> kraken_hits;
  int dragon_hits = 0;

  // for achievements
  for(auto& mov: bowpath) {
    cell *c = mov.prev.at;
    if(c->monst == moDragonHead) {
      bool healthy = true;
      cell *c1 = c;
      int qty = 0;
      for(int i=0; i<iteration_limit; i++) {
        if(!isDragon(c1)) break;
        if(!c1->hitpoints) { healthy = false; break; }
        if(c1->mondir == NODIR) break;
        c1 = c1->move(c1->mondir);
        qty++;
        }
      if(healthy) healthy_dragons.emplace_back(c, qty);
      }
    if(c->monst == moKrakenT && c->hitpoints) {
      kraken_hits[kraken::head(c)].first++;
      }
    }

  for(auto& mov: bowpath) {
    cell *c = mov.prev.at;
    cell *cf = mov.prev.cpeek();
    if(!c) continue;
    eMonster who = (mov.flags & bpCOPIED) ? moMimic : moPlayer;

    if(c != cf) for(int t=0; t<cf->type; t++) {
      cell *c1 = cf->cmove(t);
      if(!c) continue;

      bool stabthere = false;
      if(logical_adjacent(c, moPlayer, c1)) stabthere = true;

      if(stabthere && canAttack(cf,who,c1,c1->monst,AF_STAB)) {
        hit_anything = true;
        changes.ccell(c1);
        eMonster m = c->monst;
        if(attackMonster(c1, AF_STAB | AF_MSG, who))  {
          spread_plague(c1, cf, t, moPlayer);
          produceGhost(c, m, moPlayer);
          }
        }
      }

    mirror::breakMirror(mov.next, -1);
    eMonster m = c->monst;
    if(!m || isMimic(m)) continue;
    if(m == moKrakenH) continue;

    if(!canAttack(cf, who, c, m, attackflags)) {
      if(among(m, moSleepBull, moHerdBull)) {
        addMessage(XLAT("%The1 is enraged!", m));
        c->monst = moRagingBull;
        hit_anything = true;
        continue;
        }
      else {
        pcmove pcm; pcm.mi = movei(mov.prev).rev();
        pcm.tell_why_cannot_attack();
        continue;
        }
      }
    changes.ccell(c);

    bool push = (items[itCurseWeakness] || (isStunnable(c->monst) && c->hitpoints > 1));
    push = push && (!(mov.flags & bpLAST) && monsterPushable(c));

    // for achievements
    if(isDragon(m)) dragon_hits++;
    if(m == moKrakenT && c->hitpoints) kraken_hits[kraken::head(c)].second++;

    if(m && attackMonster(c, attackflags | AF_MSG, who)) hit_anything = true;

    if(!c->monst || isAnyIvy(m)) {
      spread_plague(cf, c, movei(mov.prev).rev().d, moPlayer);
      produceGhost(c, m, moPlayer);
      kills.insert(m);
      }

    if(push) pushes.push_back(mov);
    }

  while(!pushes.empty()) {
    auto& mov = pushes.back();
    cell *c = mov.prev.at;
    cell *ct = mov.next.cpeek();
    bool can_push = passable(ct, c, P_BLOW);
    if(can_push) {
      hit_anything = true;
      changes.ccell(c);
      changes.ccell(ct);
      pushMonster(mov.next);
      }
    pushes.pop_back();
    }

  reverse(bowpath.begin(), bowpath.end());

  // three achievements:
  achievement_count("BOWVARIETY", kills.size(), 0);

  for(auto p: healthy_dragons) {
    cell *c = p.first;
    if(c->monst != moDragonHead && dragon_hits >= p.second)
      achievement_gain_once("BOWDRAGON");
    }

  for(auto kh: kraken_hits) {
    if(kh.second.first == 3 && kh.second.second == 3) {
      if(kraken::half_killed[kh.first]) achievement_gain_once("BOWKRAKEN");
      else kraken::half_killed[kh.first] = true;
      }
    }

  gen_bowpath_map();
  }

EX bool have_bow_target() {
  dynamicval<decltype(bowpath)> bp(bowpath);
  dynamicval<decltype(bowpath_map)> bpm(bowpath_map);

  for(int i=0; i<isize(dcal); i++) {
    cell *c = dcal[i];
    target_at = {};
    target_at[c->cpdist] = {c};

    int res = create_path();
    if(res == -1) continue;

    checked_move_issue = miVALID;
    pcmove pcm;
    pcm.checkonly = true;
    changes.init(true);
    bool b = pcm.try_shooting(false);
    changes.rollback();

    if(b) return true;
    }
  return false;
  }

EX void showMenu() {
  cmode = sm::SIDE | sm::MAYDARK;
  gamescreen();
  dialog::init(XLAT("weapon selection"));
  add_edit(weapon);
  if(crossbow_mode()) {
    add_edit(style);
    add_edit(bump_to_shoot);
    add_edit(bow::mouse_fire_mode);
    }
  else dialog::addBreak(200);
  dialog::addBack();
  dialog::display();
  }

EX }

}

