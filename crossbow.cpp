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
  int total;
  movei last, next, lastun;
  cell *con;
  bowpoint() : last(nullptr), next(nullptr), lastun(nullptr) { total = 0; }
  };
#endif

EX vector<bowpoint> bowpath;

EX vector<bowpoint> last_bowpath;

EX map<int, cell*> target_at;

EX int loading_time() {
  return style == cbBull ? 3 : 4;
  }

EX bool blocks(cell *c) {
  if(isWall(c) && c->wall != waMirrorWall) return true;
  return false;
  }

EX int create_path() {
  map<cell*, bowpoint> scores; 
  scores[cwt.at] = bowpoint();
  scores[cwt.at].last = movei(cwt.at, STAY);

  int best_score = -1; cell* best_score_at = cwt.at;

  for(cell *c: dcal) {
    cell *c1 = target_at[c->cpdist];
    if(c1 && c != c1) continue;
    if(c == c1) { best_score = -1; }
    bowpoint best;
    best.total = -1;
    forCellIdEx(c1, i, c) if(c1->cpdist < c->cpdist && scores.count(c1)) {
      auto last = scores[c1];
      int ntotal = last.total;
      auto cw2 = cellwalker(c, i);
      if(inmirror(c)) cw2 = mirror::reflect(cw2);
      if(blocks(cw2.peek())) continue;
      if(cw2.at->monst) { ntotal += 10000; ntotal += 1280 >> c->cpdist; }
      ntotal += 2;
      if(c->cpdist > 1) {
        int d = abs(szgmod(last.lastun.d - c->c.spin(i), c1->type));
        if(d != c1->type / 2) {
          if(style == cbGeodesic) ntotal--;
          if(style == cbBull) continue;
          }
        }
      if(ntotal > best.total) { best.total = ntotal; best.lastun = movei(c, i); best.last = movei(cw2.at, cw2.spin); best.con = c1; best.next = movei(cw2.at, STAY); }
      best.total = max(best.total, ntotal);
      }
    if(best.total > best_score) { best_score = best.total; best_score_at = c; }
    if(best.total > -1) scores[c] = best;
    }

  bowpath.clear();
  if(best_score == -1) return best_score;
  while(best_score_at != cwt.at) { 
    bowpath.push_back(scores[best_score_at]); best_score_at = bowpath.back().con; scores[best_score_at].next = bowpath.back().last.rev();
    }
  bowpath.push_back(scores[best_score_at]); 

  reverse(bowpath.begin(), bowpath.end());
  return best_score;
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
    last_bowpath = {};
    targets = {};
    }
  else if(fire_mode) {
    addMessage(XLAT("Firing cancelled."));
    fire_mode = false;
    last_bowpath = {};
    }
  }

EX void add_fire(cell *c) {
  bool emp = target_at.empty();
  auto& t = target_at[c->cpdist];
  if(t == c) {
    println(hlog, "done");
    bow::last_bowpath.clear();
    checked_move_issue = miVALID;
    pcmove pcm;
    pcm.checkonly = false;
    changes.init(false);
    bool b = pcm.try_shooting(false);
    println(hlog, "b = ", b);
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
      else addMessage(XLAT("No way to hit this place."));
      }
    bow::last_bowpath = bow::bowpath;
    }
  }

EX void shoot() {
  flagtype attackflags = AF_NORMAL;
  if(items[itOrbSpeed]&1) attackflags |= AF_FAST;
  if(items[itOrbSlaying]) attackflags |= AF_CRUSH;
  if(items[itCurseWeakness]) attackflags |= AF_WEAK;

  for(auto& m: bowpath) {
    cell *c = m.next.s;
    if(!c) continue;
    changes.ccell(c);
    if(c->monst) attackMonster(c, attackflags | AF_MSG, moPlayer);
    }
  last_bowpath = bowpath;
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

