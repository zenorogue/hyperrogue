#include "hyper.h"
#include <iostream>
#include <fstream>
#include <thread>

namespace hr {

EX namespace arbiquotient {

int aq_max = 10;
bool running = false;
bool displaying = false;
bool block_selfedges = false;
bool block_cones = false;
bool block_mirrors = false;
int min_distance = 1;

bool dedup_rotation = true;
bool dedup_mirror = true;
bool dedup_focus = true;

bool allow_nonorientable = false;

set<cellwalker> preunify;

int next_id;

struct aqdata {
  cell *where;
  bool closed;
  cellwalker parent;
  int size;
  int id;
  bool unified_to_start;
  aqdata(cell *c) : where(c), parent(c) { id = next_id++; size = 1; closed = false; unified_to_start = false; }
  };

/* the stack of known unifications */
vector<pair<cellwalker, cellwalker>> unifications;

vector<aqdata*> allaq;

map<cell*, aqdata> aq;

struct hr_precision_error : hr_exception { hr_precision_error() : hr_exception("precision error") {} };

struct aq_overflow : hr_exception { aq_overflow(): hr_exception("autoquotient overflow") {} };
struct aq_unsupported : hr_exception { aq_unsupported(): hr_exception("unsupported orbifold") {} };

cellwalker ufind(cellwalker cw) {
  auto& d = aq.at(cw.at);
  if(d.where != cw.at) throw hr_exception("where incorrect");
  if(d.parent == cw.at) return cw;
  auto f = ufind(d.parent);
  if(f.at != d.parent.at) d.parent = f;
  return cw_add_diff(f, cw, cw.at);
  }

void enlist(cell *c) {
  aq.emplace(c, c);
  aqdata& ref = aq.at(c);
  allaq.push_back(&ref);
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
    d2->parent = cw_add_diff(cw1, cw2.at, cw2);
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

vector<int> quotient_output(int dir, bool mirrored) {
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
    return ofs + cwdiff_fixed(cw, cw1) + (cw.mirrored != cw1.mirrored ? quotientspace::symmask : 0);
    };
  assign_id(cellwalker(currentmap->gamestart(), dir, mirrored));
  for(int i=0; i<isize(listorder); i++) {
    cell *c = listorder[i];
    // auto [ofs, cw] = offsets[c];
    auto cw = offsets[c].second;
    for(int j=0; j<c->type; j++)
      output.push_back(assign_id(ufind(cw + j + wstep)));
    }
  return output;
  }

/** \brief return the statistical information about the current orbifold
 *  in case of incorrect identification, may also return ERROR
 */
string statstring() {
  int selfedges = 0;
  int mirrors = 0;
  int tiles = 0;
  int goodedges = 0;
  map<int, ld> verts;
  map<int, ld> mverts;
  for(auto p: allaq) {
    auto ref = ufind(p->where);
    if(ref.at == p->where) {
      tiles++;
      for(int i=0; i<p->where->type; i++) {
        cellwalker cw(p->where, i);
        println(hlog, cw, " vs ", ufind(cw+wstep));
        if(cw == ufind(cw+wstep)) selfedges++;
        else if(cw == ufind(cw+wstep+wmirror)) mirrors++;
        else goodedges++;
        if(arb::current.have_valence) for(bool mirr: {false, true}) {
          auto& sh = arb::current.shapes[shvid(cw.at)];
          int val = sh.vertex_valence[mirr ? gmod(i-1, cw.at->type) : i];
          int steps = 0;
          bool has_mirror = false;
          auto cwc = cw; if(mirr) cwc += wmirror;
          auto cw1 = cwc;
          do {
            println(hlog, "at ", cw1);
            cw1 += wstep;  cw1 = ufind(cw1);
            println(hlog, "   ", cw1);
            if(cw1 == cwc+wmirror) has_mirror = true;
            cw1++; steps++;
            cw1 = ufind(cw1);
            if(steps > 100) { return "ERROR"; }
            }
          while(cw1 != cwc);
          println(hlog, "looped back to ", cwc, has_mirror ? " (m)" : "");
          if(val % steps) return "ERROR";
          if(has_mirror) mverts[val / steps] += 1. / steps;
          else verts[val / steps] += 0.5 / steps;
          }
        }
      }
    }
  shstream s;
  println(hlog, tie(goodedges, selfedges, mirrors));
  println(hlog, "verts = ", verts, " mverts = ", mverts);
  print(s, "F:", tiles);
  if(goodedges % 2) return "ERROR";
  print(s, " E:", goodedges/2);
  if(!block_selfedges) print(s, " e:", selfedges);
  if(!block_mirrors && allow_nonorientable) print(s, " M:", mirrors);
  for(bool star: {false, true}) {
    bool first = true;
    for(auto p: (star ? mverts : verts)) {
      auto v = p.second + 1e-6;
      auto fl = int(floor(v));
      if(v - fl > 2e-6) return "ERROR";
      if(first) {
        print(s, star ? " *" : " ");
        first = false;
        }
      if(fl <= 3) for(int i=0; i<fl; i++) print(s, p.first);
      else print(s, p.first, "[", fl, "]");
      }
    }
  return s.s;
  }

set<buckethash_t> seen_outputs;

auto& all_found = arb::current.quotients;

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
    hashmix(hash, ref.mirrored);
    backup.push_back(p->parent);
    if(p->parent.at == p->where) {
      active.push_back(p->where);
      if(!p->closed) numopen++;

      if(block_selfedges) for(int i=0; i<p->where->type; i++) {
        cellwalker cw(p->where, i);
        auto cw1 = cw + wstep;
        if(aq.count(cw1.at) && cw == ufind(cw1)) return;
        }

      if(block_mirrors) for(int i=0; i<p->where->type; i++) {
        cellwalker cw(p->where, i);
        auto cw1 = cw + wstep + wmirror;
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

    else if(min_distance > 1 && celldistance(p->parent.at, p->where) < min_distance) return;
    }
  if(seen_hashes.count(hash)) return;
  seen_hashes.insert(hash);
  
  cellwalker cw0 = currentmap->gamestart();

  println(hlog, "active ", isize(active), " numopen = ", numopen, " actives = ", active);

  int shid = shvid(cw0.at);
  auto& sh = arb::current.shapes[shid];
  int cl = sh.cycle_length;

  if(numopen == 0) {
    println(hlog, "closed found!");
    vector<int> bqo = {999 | quotientspace::symmask};

    for(auto li: active) {
      auto oshvid = shvid(li);
      bool mirr = arb::current.shapes[oshvid].is_mirrored;
      oshvid = arb::current.shapes[oshvid].orig_id;
      if(oshvid != shid) continue;
      if(!mirr) {
         for(int i=0; i<cw0.at->type; i+=cl) {
          auto qo = quotient_output(i, false);
          if(qo < bqo) bqo = qo;
          if(sh.symmetric_value && dedup_mirror) {
            auto qo = quotient_output(sh.reflect(i), true);
            if(qo < bqo) bqo = qo;
            }
          if(!dedup_rotation) break;
          }
        if(!dedup_focus) break;
        }
      if(mirr && dedup_mirror) {
        for(int i=0; i<cw0.at->type; i+=cl) {
          auto qo = quotient_output(cw0.at->type-i-1, true);
          if(qo < bqo) bqo = qo;
          if(!dedup_rotation) break;
          }
        }
      }

    buckethash_t vhash = 0;
    for(auto i: bqo) hashmix(vhash, i);
    if(seen_outputs.count(vhash)) return;
    seen_outputs.insert(vhash);
    println(hlog, "[", isize(all_found), "] ", bqo);
    auto ss = statstring();
    if(ss == "ERROR") return;
    all_found.push_back(arb::quotient_data{ss + format(" %016lX", (long) vhash), bqo});
    if(!(cgflags & qCLOSED)) return;
    }
  indenter ind(2);

  for(auto li: active) if(li != cw0.at) {

    if(min_distance > 1 && celldistance(li, cw0.at) < min_distance) continue;

    auto tryout = [&] (cellwalker cw1) {
      aq.at(cw1.at).unified_to_start = true;
      unifications.emplace_back(cw0, cw1);
      bool b = apply_uni();
      if(b) recurse();
      else unifications.clear();
      auto backup_iterator = backup.begin();
      for(auto p: allaq) p->parent = *(backup_iterator++);
      aq.at(cw1.at).unified_to_start = false;
      };

    auto oshvid = shvid(li);
    bool mirr = arb::current.shapes[oshvid].is_mirrored;
    oshvid = arb::current.shapes[oshvid].orig_id;
    if(oshvid != shid) continue;

    if(allow_nonorientable && mirr) {
      for(int ro=0; ro<cw0.at->type; ro+=cl) tryout(cellwalker(li, cw0.at->type-1-ro, true));
      }
    if(!mirr) for(int ro=0; ro<cw0.at->type; ro+=cl) {
      tryout(cellwalker(li, ro));
      if(allow_nonorientable && sh.symmetric_value) {
        tryout(cellwalker(li, sh.reflect(ro), true));
        }
      }
    }
  }

void auto_create(int num) {
  clear_all();
  seen_hashes.clear();
  seen_outputs.clear();

  cellwalker cw0 = currentmap->gamestart();
  enlist(cw0.at);

  int id = 0;
  while(id < num && id < isize(allaq)) {
    auto aqd = allaq[id++];
    cell *c = aqd->where;
    forCellCM(c1, c) if(!aq.count(c1)) enlist(c1);
    aqd->closed = true;
    }

  for(auto p: preunify) {
    if(!aq.count(p.at)) {
      unifications.clear();
      addMessage("preunify outside of the searched region");
      return;
      }

    unifications.emplace_back(cw0, p);
    aq.at(p.at).unified_to_start = true;
    }
  if(!apply_uni()) {
    unifications.clear();
    addMessage("invalid preunify");
    return;
    }

  println(hlog, "calling recurse");
  indenter ind(2);
  recurse();

  println(hlog, "solutions found: ", isize(all_found));
  }

EX vector<int> quotient_data;

EX void enable_by_id(int id) {
  if(id >= isize(all_found)) throw hr_exception("not that many AQ quotients known");
  enable_quotient_data(all_found[id]);
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
      auto co2 = get_conn(quotient_data[i] &~ quotientspace::symmask, arb::current.shapes[co.first->zebraval].connections[co.second].sid);
      co.first->c.connect(co.second, co2.first, co2.second, (quotient_data[i] & quotientspace::symmask));
      }
    println(hlog, "connections created");
    }

  heptagon *getOrigin() override { 
    return connections[0].first;
    }

  transmatrix adj(heptagon *h, int dir) override {
    return arb::get_adj(arb::current_or_slided(), h->zebraval, dir, h->move(dir)->zebraval, h->c.spin(dir), h->c.mirror(dir));
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

bool aq_drawcell(cell *c, const shiftmatrix& V) {
  if(c == currentmap->gamestart()) {
    queuepoly(V * ddspin(c, 0), cgi.shAsymmetric, 0xFFFF00FF);
    }
  if(!running) {
    for(auto cw: preunify) if(cw.at == c)
      queuepoly(V * ddspin(c, cw.spin) * (cw.mirrored?Mirror:Id), cgi.shAsymmetric, 0x0000FFFF);
    return false;
    }
  auto p = at_or_null(aq, c);
  if(!p) return false;
  if(p->unified_to_start) {
    auto cw = cellwalker(c);
    auto cw1 = ufind(cw);
    auto cw2 = cw_add_diff(cw, currentmap->gamestart(), cw1);
    queuepoly(V * ddspin(c, cw2.spin) * (cw2.mirrored?Mirror:Id), cgi.shAsymmetric, 0x0000FFFF);
    }
  if(p->parent.at == c)
    queuepoly(V, cgi.shDisk, 0xFFFFFFF);
   else
    queuepoly(V, cgi.shDisk, 0x202020FF);
  return false;
  }

string export_filename = "tessellations/exported.tes";

EX bool export_tes(string fname) {
  fhstream f(fname, "wt");
  if(!f.f) return false;
  auto& ac = arb::current_or_slided();
  println(f, "## ", ac.name, " (exported)");
  println(f, (cgflags & qAFFINE) ? "a2." : hyperbolic ? "h2." : euclid ? "e2." : "s2.");
  auto& shs = ac.shapes;
  for(int i=0; i<isize(shs); i++) {
    print(f, "tile(");
    for(int j=0; j<shs[i].cycle_length; j++)
      print(f, format("%.15f,%.15f,", shs[i].edges[j], shs[i].angles[j]));
    print(f, "*", shs[i].size() / shs[i].cycle_length);
    if(shs[i].symmetric_value) print(f, ",|", shs[i].symmetric_value);
    println(f, ")");
    }
  for(int i=0; i<isize(shs); i++) {
    for(int j=0; j<shs[i].cycle_length; j++) {
      auto& co = shs[i].connections[j];
      println(f, "c(", i, ",", j, ",", co.sid, ",", co.eid, ",", co.mirror, ")");
      if(shs[i].stretch_shear[j] != make_pair<ld,ld>(1, 0))
        println(f, "stretch_shear(", format("%.15f,%.15f,", shs[i].stretch_shear[j].first, shs[i].stretch_shear[j].second), ",", i, ",", j, ")");
      }
    }
  for(auto& af: all_found) {
    println(f, "#/ ", af.name);
    print(f, "quotient(");
    int ct = 0;
    for(auto v: af.connections) {
      if(ct++) print(f, ",");
      print(f, v &~ quotientspace::symmask);
      if(v & quotientspace::symmask) print(f, "$");
      }
    println(f, ")");
    }
  return true;
  }

void show_auto_dialog() {
  cmode = sm::SIDE | sm::DIALOG_STRICT_X;
  if(running) cmode |= sm::NO_EXIT;
  int p = addHook(hooks_drawcell, 100, aq_drawcell);
  gamescreen();
  delHook(hooks_drawcell, p);
  dialog::init(XLAT("auto-generate quotients"));
  add_edit(aq_max);

  cell *c0 = currentmap->gamestart();
  if(cwt.at != c0) {
    auto& ac = arb::current;
    auto oshvid = shvid(cwt.at);
    bool mirr = ac.shapes[oshvid].is_mirrored;
    oshvid = ac.shapes[oshvid].orig_id;
    auto cl = ac.shapes[0].cycle_length;

    if(oshvid == 0) {

      char key = '1';
      auto add_option = [&] (bool b) {
        cellwalker cw(cwt.at, cwt.spin, b);
        dialog::addBoolItem(b ? XLAT("preunify mirrored start") : XLAT("preunify start"), preunify.count(cw), key++);
        dialog::add_action([cw] {
          if(preunify.count(cw)) preunify.erase(cw); else preunify.insert(cw);
          });
        };

      if(gmod(cwt.spin, cl) == 0) add_option(false);
      if(mirr && gmod(cwt.spin - (c0->type-1), cl) == 0) add_option(true);
      if(ac.shapes[0].symmetric_value && gmod(cwt.spin - ac.shapes[0].reflect(0), cl) == 0) add_option(true);
      }
    }

  add_edit(block_selfedges);
  add_edit(block_cones);
  add_edit(block_mirrors);
  add_edit(min_distance);
  add_edit(allow_nonorientable);
  add_edit(dedup_rotation);
  add_edit(dedup_focus);
  add_edit(dedup_mirror);
  dialog::addBoolItem(XLAT("running"), running, 'r');
  dialog::add_action([] { 
    if(!running) {
      running = true;
      displaying = true;
      all_found.clear();
      auto_create(aq_max);
      }
    running = false;
    });
  dialog::addSelItem(XLAT("export the quotients found"), its(isize(all_found)), 'E');
  dialog::add_action([] {
    dialog::openFileDialog(export_filename, XLAT("export to file:"), ".tes",
      [] () {
        return export_tes(export_filename);
        });
    });
  if(running) dialog::addBreak(100);
  else dialog::addBack();
  dialog::display();
  }

EX void enable_quotient_data(const struct arb::quotient_data& q) {
  stop_game();
  cgflags |= qANYQ | qCLOSED;
  quotient_data = q.connections;
  ginf[geometry].quotient_name = q.name;
  start_game();
  }

EX void disable_quotient_data() {
  stop_game();
  cgflags &=~ qANYQ;
  if(!sphere) cgflags &=~ qCLOSED;
  quotient_data = {};
  start_game();
  }

EX void show_dialog() {
  cmode = sm::SIDE | sm::DIALOG_STRICT_X;
  gamescreen();
  dialog::init(XLAT("arbitrary quotients"));

  dialog::start_list(2000, 2000, 'A');
  
  for(auto& q: all_found) {
    dialog::addBoolItem(q.name, q.connections == quotient_data, dialog::list_fake_key++);
    dialog::add_action([q] { enable_quotient_data(q); });
    }
  dialog::end_list();

  dialog::addBreak(100);
  dialog::addItem("auto-generate", 'g');
  dialog::add_action_push(show_auto_dialog);
  if(cgflags & qANYQ) {
    dialog::addItem("disable", 'd');
    dialog::add_action([] { disable_quotient_data(); });
    }
  else dialog::addBreak(100);
  dialog::addItem("help", 'h');
  dialog::add_action([] { open_url("https://github.com/zenorogue/hyperrogue/blob/master/tessellations/sample/README.md"); });
  dialog::addBack();
  dialog::display();
  }

auto aqhook =
  arg::add3("-aq-auto", [] { displaying = false; running = true; arg::shift(); auto_create(arg::argi()); })
+ arg::add3("-aq-enable", [] { arg::shift(); enable_by_id(arg::argi()); })
+ arg::add3("-aq-export", [] { arg::shift(); export_tes(arg::args()); })
+ arg::add3("-d:aq", [] { arg::launch_dialog(show_dialog); })
+ addHook(hooks_configfile, 100, [] {
    param_i(aq_max, "aq_max")
    -> editable(1, 500, 10, "number of tiles to search in", "", 'l');
    param_i(min_distance, "aq_min_distance", 1)
    -> editable(1, 10, 1, "minimum distance between identified tiles", "", 'd');
    param_b(block_cones, "aq_block_cones")
    -> editable("block cone points on vertices", 'c');
    param_b(block_selfedges, "aq_block_selfedges")
    -> editable("block cone points on edge centers", 'e');
    param_b(block_mirrors, "aq_block_mirrors")
    -> editable("block mirrors on edges", 'm');
    param_b(allow_nonorientable, "aq_allow_nonorientable")
    -> editable("allow nonorientable", 'n');
    param_b(dedup_rotation, "aq_dedup_rotation")
    -> editable("dedup rotation", 's');
    param_b(dedup_focus, "aq_dedup_focus")
    -> editable("dedup focus", 'f');
    param_b(dedup_mirror, "aq_dedup_mirror")
    -> editable("dedup mirror", 'o');
    })
+ addHook(hooks_clearmemory, 0, [] { preunify.clear(); })
+ addHook(hooks_newmap, 0, [] {
    if(geometry == gArbitrary && quotient) {
      return (hrmap*) new hrmap_autoquotient;
      }
    return (hrmap*) nullptr;
    });


}

}