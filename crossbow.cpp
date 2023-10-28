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
enum eCrossbowStyle { cbBull, cbGeodesic };
#endif

EX eWeapon weapon;
EX eCrossbowStyle style;
EX bool bump_to_shoot = true;

EX bool crossbow_mode() { return weapon == wCrossbow; }

#if HDR
struct bowpoint {
  cellwalker prev, next;
  bool first, last;
  bool copied;
  bool mirrored;
  bowpoint() {}
  };

struct bowscore {
  int total;
  cellwalker last;
  int turns;
  };
#endif

EX vector<bowpoint> bowpath;

EX map<cell*, vector<bowpoint>> bowpath_map;

EX map<int, cell*> target_at;

EX int loading_time() {
  return style == cbBull ? 3 : 4;
  }

EX bool blocks(cell *c) {
  if(isWall(c) && !among(c->wall, waMirrorWall, waMirror, waCloud)) return true;
  // if(c->monst && isMultitile(c->monst)) return true;
  return false;
  }

EX int qadd(cellwalker a, cellwalker b) {
  hassert(a.at == b.at);
  for(int i=0; i<a.at->type; i++) if(a+i == b) return i;
  return NODIR;
  }

EX int create_path() {
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
      int ntotal = last.total;
      auto ocw2 = cellwalker(c, i);
      auto cw2 = ocw2;
      if(inmirror(c)) cw2 = mirror::reflect(cw2);
      if(blocks(cw2.peek())) continue;
      if(cw2.at->monst) { ntotal += 10000; ntotal += 1280 >> c->cpdist; }
      ntotal += 2;
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

  if(best_score == -1) return best_score;

  vector<int> dirseq = { NODIR };
  while(best_score_at != cwt.at) { 
    auto& at = scores[best_score_at];
    dirseq.push_back(at.turns);
    best_score_at = at.last.peek();
    }
  reverse(dirseq.begin(), dirseq.end());

  println(hlog, "dirseq = ", dirseq);

  bowpath.clear();
  vector<cellwalker> bolts = { cwt };
  for(auto m: mirror::mirrors) bolts.push_back(m.second);

  set<cell*> broken_mirrors;

  for(auto d: dirseq) {
    bool first = bowpath.empty();
    vector<cellwalker> nbolts;
    set<cell*> next_broken_mirrors = broken_mirrors;
    for(auto bolt: bolts) {
      bowpath.emplace_back();
      auto& p = bowpath.back();
      p.prev = bolt;
      p.first = first;
      if(d == NODIR || blocks(bolt.at)) { p.next = bolt; p.last = true; }
      else {
        if(inmirror(bolt.at) || bolt.at->wall == waMirrorWall) bolt = mirror::reflect(bolt);
        bolt += d;
        p.next = bolt; p.last = false;
        bolt += wstep;
        if(among(bolt.at->wall, waCloud, waMirror) && !broken_mirrors.count(bolt.at)) {
          auto &mir = mirror::mirrors;
          vector<pair<int, cellwalker>> bmir;
          swap(mir, bmir);
          mirror::createHere(bolt, 0);
          swap(mir, bmir);
          for(auto b: bmir) nbolts.push_back(b.second);
          next_broken_mirrors.insert(bolt.at);
          }
        nbolts.push_back(bolt);
        }
      }
    bolts = nbolts;
    broken_mirrors = next_broken_mirrors;
    }

  return best_score;
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
  if(items[itCrossbow]) { addMessage(XLAT("You need more time to reload your crossbow!")); return; }
  if(!fire_mode) {
    addMessage(XLAT("Double-click tile to fire."));
    fire_mode = true;
    clear_bowpath();
    targets = {};
    }
  else if(fire_mode) {
    addMessage(XLAT("Firing cancelled."));
    fire_mode = false;
    clear_bowpath();
    }
  }

EX void add_fire(cell *c) {
  bool emp = target_at.empty();
  auto& t = target_at[c->cpdist];
  if(t == c && !bow::bowpath.empty()) {
    clear_bowpath();
    checked_move_issue = miVALID;
    pcmove pcm;
    pcm.checkonly = false;
    changes.init(false);
    bool b = pcm.try_shooting(false);
    if(!b) changes.rollback();
    fire_mode = false;
    }
  else {
    t = c;
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

EX void shoot() {
  flagtype attackflags = AF_NORMAL;
  if(items[itOrbSpeed]&1) attackflags |= AF_FAST;
  if(items[itOrbSlaying]) attackflags |= AF_CRUSH;
  if(items[itCurseWeakness]) attackflags |= AF_WEAK;

  reverse(bowpath.begin(), bowpath.end());

  for(auto& mov: bowpath) {
    cell *c = mov.prev.at;
    cell *cf = mov.prev.cpeek();
    if(!c) continue;

    if(c != cf) for(int t=0; t<cf->type; t++) {
      cell *c1 = cf->move(t);
      if(!c) continue;
      
      bool stabthere = false;
      if(logical_adjacent(c, moPlayer, c1)) stabthere = true;

      if(stabthere && canAttack(cf,moPlayer,c1,c1->monst,AF_STAB)) {
        changes.ccell(c1);
        eMonster m = c->monst;
        if(attackMonster(c1, AF_STAB | AF_MSG, moPlayer))  {
          spread_plague(c1, cf, t, moPlayer);
          produceGhost(c, m, moPlayer);
          }
        }
      }

    mirror::breakMirror(mov.next, -1);
    eMonster m = c->monst;
    if(!m || isMimic(m)) continue;
    if(!canAttack(cf, moPlayer, c, m, attackflags)) {
      pcmove pcm; pcm.mi = movei(mov.prev).rev();
      pcm.tell_why_cannot_attack();
      continue;
      }
    changes.ccell(c);
    if(m) attackMonster(c, attackflags | AF_MSG, moPlayer);

    if(!c->monst || isAnyIvy(m)) {
      spread_plague(cf, c, movei(mov.prev).rev().d, moPlayer);
      produceGhost(c, m, moPlayer);
      }

    if(items[itCurseWeakness] || (isStunnable(c->monst) && c->hitpoints > 1)) {
      if(!mov.last && monsterPushable(c)) {
        cell *ct = mov.next.cpeek();
        bool can_push = passable(ct, c, P_BLOW);
        if(can_push) {
          changes.ccell(c);
          changes.ccell(ct);
          pushMonster(mov.next);
          }
        }
      }
    }

  reverse(bowpath.begin(), bowpath.end());

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
    }
  else dialog::addBreak(200);
  dialog::addBack();
  dialog::display();
  }

EX }

}

