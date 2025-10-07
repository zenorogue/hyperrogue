#include "hyper.h"
#include <iostream>
#include <fstream>
#include <thread>

namespace hr {

EX namespace arbiquotient {

int aq_max = 100;
bool running = false;
bool displaying = false;
bool block_selfedges = true;
bool block_cones = true;

map<cell*, cellwalker> aqs;

int next_id;

struct aqdata {
  cell *where;
  bool closed;
  cellwalker parent;
  int size;
  int id;
  aqdata(cell *c) : where(c), parent(c) { id = next_id++; size = 1; closed = false; }
  };

/* the stack of known unifications */
vector<pair<cellwalker, cellwalker>> unifications;

vector<aqdata*> allaq;

map<cell*, aqdata> aq;

struct hr_precision_error : hr_exception { hr_precision_error() : hr_exception("precision error") {} };

struct aq_overflow : hr_exception { aq_overflow(): hr_exception("autoquotient overflow") {} };
struct aq_unsupported : hr_exception { aq_unsupported(): hr_exception("unsupported orbifold") {} };

cellwalker rezero(cellwalker cw, cellwalker base) {
  if(base.mirrored) cw += wmirror; cw -= base.spin;
  return cw;
  }

cellwalker dezero(cellwalker cw, cellwalker base) {
  cw += base.spin; if(base.mirrored) cw += wmirror; 
  return cw;
  }

cellwalker ufind(cellwalker cw) {
  auto& d = aq.at(cw.at);
  if(d.where != cw.at) throw hr_exception("where incorrect");
  if(d.parent == cw.at) return cw;
  auto f = ufind(d.parent);
  if(f.at != d.parent.at)
    d.parent = f;
  return dezero(f, cw);
  }

void enlist(cell *c) {
  aq.emplace(c, c);
  aqdata& ref = aq.at(c);
  allaq.push_back(&ref);
  }

void default_list() {
  cellwalker cw0 = currentmap->gamestart();
  aqs.clear();
  auto cw1 = cw0; for(int i=0; i<10; i++) cw1 = cw1 + wstep + 2;
  auto cw2 = cw0+1; for(int i=0; i<10; i++) cw2 = cw2 + wstep + 2; cw2 = cw2 - 1;
  aqs[cw1.at] = cw1;
  aqs[cw2.at] = cw2;
  }

void clear_all() {
  unifications = {}; next_id = 0; aq.clear(); allaq.clear();
  }

bool apply_uni() {
  while(unifications.size()) {
    auto cwp = unifications.back();
    auto cw1 = cwp.first, cw2 = cwp.second;
    unifications.pop_back();
    cw1 = ufind(cw1);
    cw2 = ufind(cw2);
    if(cw1.at == cw2.at) {
      if(cw1 != cw2) return false;
      continue;
      }
    auto d1 = &aq.at(cw1.at);
    auto d2 = &aq.at(cw2.at);
    if(d1->id > d2->id) { swap(cw1, cw2); swap(d1, d2); }
    d1->size += d2->size;
    d2->parent = rezero(cw1, cw2);
    if(cw2.at->item == itGold) cw2.at->item = itRuby;
    for(int i=0; i<cw1.at->type; i++) {
      auto m1 = cw1 + i + wstep;
      auto m2 = cw2 + i + wstep;
      if(aq.count(m1.at) && aq.count(m2.at))
        unifications.emplace_back(m1, m2);
      }
    }
  return true;
  }

vector<int> quotient_output(int dir) {
  vector<int> output;
  int next_offset = 0;
  map<cell*, pair<int, cellwalker>> offsets;
  vector<cell*> listorder;
  auto assign_id = [&] (cellwalker cw) {
    if(!offsets.count(cw.at)) {
      auto& sh = arb::current.shapes[shvid(cw.at)];
      while(cw.spin % sh.cycle_length) cw--;
      offsets[cw.at] = { next_offset, cw };
      listorder.push_back(cw.at);
      next_offset += cw.at->type;
      }
    // auto [ofs, cw1] = offsets[cw.at];
    auto &tmp = offsets[cw.at];
    auto ofs = tmp.first;
    auto cw1 = tmp.second;
    return ofs + gmod(cw.spin - cw1.spin, cw.at->type);
    };
  assign_id(cellwalker(currentmap->gamestart(), dir));
  for(int i=0; i<isize(listorder); i++) {
    cell *c = listorder[i];
    // auto [ofs, cw] = offsets[c];
    auto cw = offsets[c].second;
    for(int j=0; j<c->type; j++)
      output.push_back(assign_id(ufind(cw + j + wstep)));
    }
  return output;
  }

string statstring() {
  int selfedges = 0;
  int tiles = 0;
  int goodedges = 0;
  map<int, ld> verts;
  for(auto p: allaq) {
    auto ref = ufind(p->where);
    if(ref.at == p->where) {
      tiles++;
      for(int i=0; i<p->where->type; i++) {
        cellwalker cw(p->where, i);
        if(cw == ufind(cw+wstep)) selfedges++;
        else goodedges++;
        if(arb::current.have_valence) {
          auto& sh = arb::current.shapes[shvid(cw.at)];
          int val = sh.vertex_valence[i];
          int steps = 0;
          auto cw1 = cw;
          do {
            println(hlog, "at ", cw1);
            cw1 += wstep; cw1++; steps++;
            cw1 = ufind(cw1);
            }
          while(cw1 != cw);
          println(hlog, "looped back to ", cw);
          if(val % steps) throw hr_exception("divisibility error");
          verts[val / steps] += 1. / steps;
          }
        }
      }
    }
  shstream s;
  println(hlog, verts);
  print(s, "F:", tiles, " ");
  if(goodedges % 2) throw hr_exception("divisibility error III");
  print(s, "E:", goodedges/2, " ");
  if(!block_selfedges) print(s, "e:", selfedges, " ");
  for(auto p: verts) {
    auto v = p.second + 1e-6;
    auto fl = floor(v);
    if(v - fl > 2e-6) throw hr_exception("divisibility error II");
    print(s, p.first, ":", int(fl), " ");
    }
  return s.s;
  }

set<buckethash_t> seen_outputs;

struct qdata {
  string name;
  vector<int> connections;
  };

vector<qdata> all_found;

void create() {
  clear_all();

  cellwalker cw0 = currentmap->gamestart();

  enlist(cw0.at);

  int loop_id = 0;

  while(true) {

    if(!apply_uni()) throw aq_unsupported();

    if(loop_id == isize(allaq)) break;

    cell *c = allaq[loop_id++]->where;
    cellwalker cw(c);
    auto cw1 = ufind(cw);
    if(cw1 != cw) continue;
    c->item = itGold;

    for(int i=0; i<cw.at->type; i++) {
      cellwalker cw2 = cw + i + wstep;
      if(!aq.count(cw2.at)) {
        enlist(cw2.at);
        if(aqs.count(cw2.at)) unifications.emplace_back(cw0, aqs[cw2.at]);
        }
      else {
        auto cw3 = ufind(cw2);
        auto cw4 = cw3 + wstep - i;
        if(aq.count(cw4.at)) unifications.emplace_back(cw4, cw);
        }
      }

    if(next_id > 10000) throw aq_overflow();
    }

  /* vector<cell*> masters;
  for(auto& data: aq) if(data.first == data.second.parent.at) masters.push_back(data.first);

  println(hlog, "masters size = ", isize(masters)); */

  }

set<buckethash_t> seen_hashes;

void recurse() {
  if(!running) return;
  if(displaying) {
    int tick = SDL_GetTicks();
    if(tick > history::lastprogress + history::progress_each) {
      history::lastprogress = tick;
      mainloopiter();
      }
    }
  vector<cell*> active;
  int numopen = 0;
  vector<cellwalker> backup;
  buckethash_t hash = 0;
  for(auto p: allaq) {
    auto ref = ufind(p->where);
    hashmix(hash, aq.at(ref.at).id);
    hashmix(hash, ref.spin);
    backup.push_back(p->parent);
    if(p->parent.at == p->where) {
      active.push_back(p->where);
      if(!p->closed) numopen++;

      if(block_selfedges) for(int i=0; i<p->where->type; i++) {
        cellwalker cw(p->where, i);
        auto cw1 = cw + wstep;
        if(aq.count(cw1.at) && cw == ufind(cw1)) return;
        }

      if(block_cones && arb::current.have_valence && p->closed) for(int i=0; i<p->where->type; i++) {
        cellwalker cw(p->where, i);
        auto& sh = arb::current.shapes[shvid(cw.at)];
        int val = sh.vertex_valence[i];
        int steps = 0;
        auto cw1 = cw;
        do {
          cw1 += wstep; cw1++; steps++;
          if(!aq.count(cw1.at)) goto skip;
          cw1 = ufind(cw1);
          if(steps > val) goto skip; /* may happen sometimes */
          }
        while(cw1 != cw);
        if(val != steps) return;
        skip: ;
        }
      }
    }
  if(seen_hashes.count(hash)) return;
  seen_hashes.insert(hash);
  
  cellwalker cw0 = currentmap->gamestart();

  println(hlog, "active ", isize(active), " numopen = ", numopen, " actives = ", active);
  if(numopen == 0) {
    println(hlog, "closed found!");
    int shid = shvid(cw0.at);
    vector<int> bqo = {999};
    int cl = arb::current.shapes[shid].cycle_length;
    for(int i=0; i<cw0.at->type; i+=cl) {
      auto qo = quotient_output(i);
      if(qo < bqo) bqo = qo;
      }

    buckethash_t vhash = 0;
    for(auto i: bqo) hashmix(vhash, i);
    if(seen_outputs.count(vhash)) return;
    seen_outputs.insert(vhash);
    println(hlog, "[", isize(all_found), "] ", bqo);
    all_found.push_back(qdata{statstring() + format("%016lX", (long) vhash), bqo});
    if(!(cgflags & qCLOSED)) return;
    }
  indenter ind(2);

  int shid = shvid(cw0.at);
  int cl = arb::current.shapes[shid].cycle_length;

  for(auto li: active) if(li != cw0.at) {
    if(shvid(li) != shid) continue;
    for(int ro=0; ro<cw0.at->type; ro+=cl) {
      unifications.emplace_back(cw0, cellwalker(li, ro));
      bool b = apply_uni();
      if(b) recurse();
      else unifications.clear();
      }
    auto backup_iterator = backup.begin();
    for(auto p: allaq) p->parent = *(backup_iterator++);
    }
  }

void auto_create(int num) {
  clear_all();
  seen_hashes.clear();
  seen_outputs.clear();

  cellwalker cw0 = currentmap->gamestart();
  enlist(cw0.at);

  int id = 0;
  while(id < num) {
    auto aqd = allaq[id++];
    cell *c = aqd->where;
    forCellCM(c1, c) if(!aq.count(c1)) enlist(c1);
    aqd->closed = true;
    }

  println(hlog, "calling recurse");
  indenter ind(2);
  recurse();

  println(hlog, "solutions found: ", isize(all_found));
  }

vector<int> quotient_data;

void enable_by_id(int id) {
  if(id >= isize(all_found)) throw hr_exception("not that many AQ quotients known");
  stop_game();
  cgflags |= qANYQ | qCLOSED;
  quotient_data = all_found[id].connections;
  start_game();
  }

struct hrmap_autoquotient : hrmap {

  vector<pair<heptagon*, int>> connections;

  pair<heptagon*, int> get_conn(int id, int shape) {
    if(id >= isize(connections)) {
      int t = arb::current.shapes[shape].size();
      heptagon *h = init_heptagon(t);
      h->zebraval = shape;
      for(int i=0; i<t; i++) connections.emplace_back(h, i);
      h->c7 = newCell(t, h);
      }
    if(id >= isize(connections)) throw hr_exception("illegal values in autoquotient");
    return connections[id];
    }

  hrmap_autoquotient() {
    get_conn(0, 0);
    for(int i=0; i<isize(connections); i++) {
      auto co = connections[i];
      auto co2 = get_conn(quotient_data[i], arb::current.shapes[co.first->zebraval].connections[co.second].sid);
      println(hlog, "connectng ", co, " to ", co2);
      co.first->c.connect(co.second, co2.first, co2.second, 0);
      }
    println(hlog, "connections created");
    }

  heptagon *getOrigin() override { 
    return connections[0].first;
    }

  transmatrix adj(heptagon *h, int dir) override {
    return arb::get_adj(arb::current_or_slided(), h->zebraval, dir, h->move(dir)->zebraval, h->c.spin(dir), false);
    }

  int shvid(cell *c) override {
    return c->master->zebraval;
    }

  hyperpoint get_corner(cell *c, int cid, ld cf) override {
    auto& sh = arb::current_or_slided().shapes[c->master->zebraval];
    int id = gmod(cid, c->type);
    if(sh.angles[gmod(id-1, c->type)] <= 0) return sh.vertices[id];
    return normalize(C0 + (sh.vertices[id] - C0) * 3 / cf);
    }
  };

void show_auto_dialog() {
  cmode = sm::SIDE | sm::DIALOG_STRICT_X;
  gamescreen();
  dialog::init(XLAT("auto-generate quotients"));
  add_edit(aq_max);
  add_edit(block_selfedges);
  add_edit(block_cones);
  dialog::addBoolItem(XLAT("running"), running, 'r');
  dialog::add_action([] { 
    println(hlog, "action");
    if(!running) {
      running = true;
      displaying = true;
      all_found.clear();
      auto_create(aq_max);
      }
    running = false;
    });
  dialog::addSelItem(XLAT("quotients found"), its(isize(all_found)), 'n');
  if(running) dialog::addBreak(100);
  else dialog::addBack();
  dialog::display();
  // todo actually prevent backing
  }

EX void show_dialog() {
  cmode = sm::SIDE | sm::DIALOG_STRICT_X;
  gamescreen();
  dialog::init(XLAT("arbitrary quotients"));

  dialog::start_list(2000, 2000, 'A');
  
  for(auto& q: all_found) {
    dialog::addBoolItem(q.name, q.connections == quotient_data, dialog::list_fake_key++);
    dialog::add_action([q] {
      stop_game();
      cgflags |= qANYQ | qCLOSED;
      quotient_data = q.connections;
      ginf[geometry].quotient_name = q.name;
      start_game();
      });
    }
  dialog::end_list();

  dialog::addItem("auto-generate", 'g');
  dialog::add_action_push(show_auto_dialog);
  if(cgflags & qANYQ) {
    dialog::addItem("disable", 'd');
    dialog::add_action([] {
      stop_game();
      cgflags &=~ qANYQ;
      if(!sphere) cgflags &=~ qCLOSED;
      quotient_data = {};
      start_game();
      });
    }
  else dialog::addBreak(100);
  dialog::addBack();
  dialog::display();
  }

auto aqhook =
  arg::add3("-aq-test", [] { try { default_list(); create(); } catch(aq_overflow&) { println(hlog, "AQ overflow"); } })
+ arg::add3("-aq-auto", [] { displaying = false; running = true; arg::shift(); auto_create(arg::argi()); })
+ arg::add3("-aq-enable", [] { arg::shift(); enable_by_id(arg::argi()); })
+ arg::add3("-d:aq", [] { arg::launch_dialog(show_dialog); })
+ addHook(hooks_configfile, 100, [] {
    param_i(aq_max, "aq_max")
    -> editable(1, 500, 10, "limit on the quotient size", "", 'm');
    param_b(block_cones, "aq_block_cones")
    -> editable("block cone points", 'c');
    param_b(block_selfedges, "aq_block_selfedges")
    -> editable("block self-edges", 'e');
    })
+ addHook(hooks_newmap, 0, [] {
    if(geometry == gArbitrary && quotient) {
      return (hrmap*) new hrmap_autoquotient;
      }
    return (hrmap*) nullptr;
    });


}

}