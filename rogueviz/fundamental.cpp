// show the fundamental domain for quotient spaces
// Copyright (C) 2018 Zeno and Tehora Rogue, see 'hyper.cpp' for details

#include "rogueviz.h"

namespace hr {

namespace fundamental {

transmatrix current_position, last_view;

void analyze_view_pre() {
  current_position = current_position * last_view * inverse(View);
  }

void analyze_view_post() {
  last_view = View;
  }

struct settings {
  int funmode;
  bool single_edges;
  bool fill_faces;

  ld widthfactor;
  color_t color_other;
  color_t color_main;
  int lq;

  color_t color_mirage;
  color_t color_mirror;
  ld label_dist;
  ld label_scale;

  int alpha;

  void show_options();
  } sett;

cell *starter;

struct shapedata {
  map<cell*, set<int>> same;
  map<cell*, shiftmatrix> gm;
  int corners, corners0;
  vector<hyperpoint> abs_cornerpos;
  vector<cell*> cells;
  vector<cellwalker> cornerlist;
  map<cellwalker, int> corner_id;
  cell *current_starter;

  vector<int> connections;
  vector<bool> mirrored;

  bool is_connected(cellwalker cw);
  void be_connected(cellwalker cw);
  int group_count(cellwalker cw);

  void auto_corners();
  void find_corners();
  shiftpoint cwcorner(cellwalker cw);
  void compute_shape();
  void render();
  } sd;

bool shapedata::is_connected(cellwalker cw) {
  return same[cw.at].count(cw.spin);
  }

void shapedata::be_connected(cellwalker cw) {
  same[cw.at].insert(cw.spin);
  cw += wstep;
  same[cw.at].insert(cw.spin);
  }


shiftpoint shapedata::cwcorner(cellwalker cw) {
  shiftmatrix T = gm[cw.at];

  if(sett.funmode == 2 && BITRUNCATED) {
    while(cw.at->type != S7) { 
      cw++; 
      T = T * currentmap->adj(cw.at, cw.spin);
      cw += wstep; 
      }
    return T * C0;
    }

  return gm[cw.at] * get_corner_position(cw.at, cw.spin+(cw.mirrored?0:1), 3);
  }

void shapedata::auto_corners() {
  cellwalker cw;

  corners = 0;

  for(int k=0; k<isize(cells); k++) {
    cell *c = cells[k];
    for(int i=0; i<c->type; i++) {
      cellwalker cw0(c, i);
      if(sett.single_edges) {
        if(!is_connected(cw0)) corners++, cw = cw0;
        }
      else {
        if(group_count(cw0) >= 3) corners++, cw = cw0;
        }
      }
    }

  if(!corners) return;

  cornerlist.clear();
  corner_id.clear();

  for(int ci=0; ci<corners; ci++) {
    corner_id[cw] = cornerlist.size();
    cornerlist.push_back(cw);

    while(true) {
      cw++;
      while(is_connected(cw)) {
        cw += wstep;
        cw++;
        }
      if(sett.single_edges || group_count(cw) >= 3) break;
      }
    }
  corners0 = corners;
  corners = isize(cornerlist);
  cornerlist.push_back(cw);

  if(corners0 != corners) println(hlog, "corners=", tie(corners0, corners));

  abs_cornerpos.clear();
  for(auto c: cornerlist) {
    auto co = inverse_shift(gm[starter], cwcorner(c));
    if(isize(abs_cornerpos) && (hdist(co, abs_cornerpos[0]) < 1e-3 || hdist(co, abs_cornerpos.back()) < 1e-3)) continue;
    abs_cornerpos.push_back(co);
    }
  corners = isize(abs_cornerpos); abs_cornerpos.push_back(abs_cornerpos[0]);
  }

void shapedata::find_corners() {

  abs_cornerpos.clear();

  auto build = [&] (int sides, ld a, ld a1, ld shift) {
    transmatrix T = Id;
    if(variation == eVariation::untruncated)
      T = T * rgpushxto0(get_corner_position(starter, 0, 3));
    for(int i=0; i<=sides; i++) {
      abs_cornerpos.push_back(T * xspinpush0(-i * TAU/sides + shift, (i&1) ? a : a1));
      }
    corners = sides;
    };

  if(sett.funmode == 3) switch(geometry) {
    case gKleinQuartic: {
      ld a = edge_of_triangle_with_angles(90._deg, M_PI/14, M_PI*2/14);
      return build(14, a, a, 0);
      }
    case gSchmutzM2: {
      ld a = edge_of_triangle_with_angles(90._deg, M_PI/12, M_PI/6);
      ld a1 = edge_of_triangle_with_angles(45._deg, M_PI/12, 60._deg);
      return build(24, a1, a, 0);
      }
    case gSchmutzM3: {
      ld a = edge_of_triangle_with_angles(60._deg, M_PI/12, M_PI/12);
      ld a1 = edge_of_triangle_with_angles(M_PI/12, 60._deg, M_PI/12);
      return build(24, a, a1, 0);
      }
    case gBolza: {
      ld a = edge_of_triangle_with_angles(90._deg, M_PI/8, M_PI/8);
      return build(8, a, a, 22.5_deg);
      }
    case gBolza2: {
      ld a = edge_of_triangle_with_angles(90._deg, M_PI/8, M_PI/8);
      hyperpoint h1 = xspinpush0(0, a);
      hyperpoint h2 = xspinpush0(45._deg, a);
      hyperpoint hm = mid(h1, h2);
      ld a1 = hdist0(hm) * 2;
      return build(16, a, a1, 0);
      }
    default: break;
    }

  return auto_corners();
  }

transmatrix rel(cellwalker cw) {
  return currentmap->adj(cw.at, cw.spin);
  }

shiftmatrix labelpos(shiftpoint h1, shiftpoint h2) {
  shiftpoint h = mid(h1, h2);
  shiftmatrix T = rgpushxto0(h);
  hyperpoint hx = inverse_shift(T, h2);
  ld alpha = atan2(-hx[1], hx[0]);
  return T * xspinpush(alpha + 90._deg, sett.label_dist);
  }
 
int shapedata::group_count(cellwalker cw) {
  if(is_connected(cw)) return 0;
  auto cw1 = cw;
  int groups = 0;
  do {
    if(!is_connected(cw1)) groups++;
    cw1 = cw1 + wstep - 1;
    }
  while(cw1 != cw);
  return groups;
  }

map<unsigned, color_t> bucket_color;

color_t *current_domain = nullptr;

void shapedata::compute_shape() {
  if(current_starter == starter) return;
  current_starter = starter;
  same.clear();
  gm.clear();
  
  gm[starter] = ggmatrix(starter);
  
  cells = {starter};
  
  int tree_edges = 0;
  int face_edges = 0;

  bool first_zebra_phase = geometry == gZebraQuotient && sett.funmode == 3 && PURE;
  bool second_zebra_phase = false;

  again:

  for(int k=0; k<isize(cells); k++) {
    cell *c = cells[k];
    for(int i=0; i<c->type; i++) {
      cellwalker cw(c, i);
      cell *c2 = cw.cpeek();
      if(gm.count(c2)) continue;
      if(first_zebra_phase && (zebra40(c) % 4) != (zebra40(c2) % 4))
        continue;
      if(second_zebra_phase) {
        int j = 0;
        for(; j<c->type; j++) if(zebra40(c->move(j)) == 4 + zebra40(c) % 12) break;
        int v = gmod(i-j+2, 7); if(v < 5) continue;
        }
      gm[c2] = gm[c] * rel(cw);
      be_connected(cw);
      tree_edges++;
      cells.push_back(c2);
      }
    }

  if(first_zebra_phase) {
    first_zebra_phase = false;
    second_zebra_phase = true;
    goto again;
    }
  
  while(sett.fill_faces) {
    int f = face_edges;
    for(int k=0; k<isize(cells); k++) {
      cell *c = cells[k];
      for(int i=0; i<c->type; i++) {
        cellwalker cw(c, i);
        if(group_count(cw) == 1) {
          face_edges++;
          be_connected(cw);
          }
        }
      }
    if(f == face_edges) break;
    }

  find_corners();
  connections.clear(); connections.resize(corners, -1);
  mirrored.clear(); mirrored.resize(corners, false);
  }

void shapedata::render() {
  if(!sd.corners) return;

  set<unsigned> buckets_used;

  for(int i=0; i<corners; i++) curvepoint_pretty(sd.abs_cornerpos[i], sd.abs_cornerpos[i+1], sett.lq);
  curvepoint_first();

  auto pos = current_position * last_view * inverse(View);

  map<unsigned, int> midedge_id;
  auto T = ggmatrix(starter);
  unsigned central_bucket = bucketer(unshift(T*C0));
  for(int i=0; i<corners; i++)
    midedge_id[bucketer(unshift(T*mid(abs_cornerpos[i], abs_cornerpos[i+1])))] = i;

  int next_connection_id = 0;

  dynamicval<ld> lw(vid.linewidth, vid.linewidth * sett.widthfactor);

  for(auto c: cells)
  for(const shiftmatrix& V : hr::span_at(current_display->all_drawn_copies, c)) {
    auto V1 = V * inverse_shift(gm[c], gm[starter]);
    auto bu = bucketer(pos * unshift(V1*C0));
    if(buckets_used.count(bu)) continue;
    buckets_used.insert(bu);

    if(sett.alpha && !bucket_color.count(bu)) {
      if(bucket_color.empty()) bucket_color[bu] = sett.alpha;
      else bucket_color[bu] = (hrand(0x1000000) << 8) | sett.alpha;
      }

    if(c == cwt.at && sett.alpha && !current_domain) current_domain = &bucket_color[bu];
    queuecurve_reuse(V1, sett.color_other, sett.alpha ? bucket_color[bu] : 0, PPR::LINE);

    if(bu != central_bucket) {
      for(int i=0; i<corners; i++) {
        unsigned val = bucketer(unshift(V1*mid(abs_cornerpos[i], abs_cornerpos[i+1])));
        auto p = at_or_null(midedge_id, val);
        if(p && connections[*p] == -1) {
          connections[*p] = connections[i] = next_connection_id++;
          mirrored[*p] = mirrored[i] = det(V1.T) * det(T.T) < 0;
          }
        }
      }
    }
  
  queuecurve(T, sett.color_main, 0, PPR::LINE);

  string conlabels = "123456789ABCDEFGHIJKLMNOPQRSTUVWYXZabcdefghijklmnopqrstuvwxyz$%@&+#";

  for(int ci=0; ci<corners; ci++) {
    int mc = mirrored[ci] ? sett.color_mirror : sett.color_mirage;
    int id = connections[ci];
    if(id == -1) continue;
    string lab = s0 + conlabels[id % isize(conlabels)];
    for(int i=0; i<id / isize(conlabels); i++) lab += "'";
    queuestr(labelpos(T * abs_cornerpos[ci], T * abs_cornerpos[ci+1]), sett.label_scale/cgi.scalefactor, lab, mc);
    }
  }

void fundamental_marker() {
  current_domain = nullptr;
  if(!sett.funmode || !quotient || !closed_manifold || GDIM == 3) return;
  sd.compute_shape();
  sd.render();
  }

void clear_data() {
  sd = {};
  bucket_color.clear();
  }

void settings::show_options() {
  dialog::init(XLAT("display fundamental domains"), 0xFFFFFFFF, 150, 0);
  add_edit(funmode);
  add_edit(single_edges);
  add_edit(fill_faces);
  add_edit(alpha);

  dialog::addBreak(50);
  add_edit(widthfactor);
  add_edit(color_main);
  add_edit(color_other);
  add_edit(lq);

  dialog::addBreak(50);
  add_edit(label_dist);
  add_edit(label_scale);
  add_edit(color_mirage);
  add_edit(color_mirror);
  }

EX void showMenu() {
  cmode = sm::SIDE | sm::MAYDARK;
  gamescreen();
  sett.show_options();
  dialog::addBreak(100);
  dialog::addSelItem("set the central tile to current position", its(celldistance(starter, cwt.at)), 's');
  dialog::add_action([] () {
    starter = cwt.at;
    });
  if(sett.alpha) {
    dialog::addItem("reshuffle all fill colors", 'r');
    dialog::add_action([] { bucket_color.clear(); });
    if(current_domain) {
      dialog::addColorItem("current central domain", *current_domain, 'y');
      dialog::add_action([] () {
        dialog::openColorDialog(*current_domain, NULL);
        dialog::get_di().dialogflags |= sm::MAYDARK | sm::SIDE;
        });
      }
    }

  dialog::addBack();
  dialog::display();
  }

int enable_fundamental() {
  addHook(hooks_post_initgame, 100, [] {
    starter = cwt.at; current_position = Id; last_view = View;
    });
  addHook(hooks_frame, 100, fundamental_marker);
  addHook(hooks_clearmemory, 100, clear_data);
  addHook(hooks_o_key, 80, [] (o_funcs& v) {
    if(sett.funmode) v.push_back(named_dialog("fundamental", showMenu));
    });
  addHook(hooks_preoptimize, 75, analyze_view_pre);
  addHook(hooks_postoptimize, 75, analyze_view_post);
  addHook(hooks_configfile, 100, [] {
    param_enum(sett.funmode, "funmode", 0)
    ->editable({{"disabled", "do not construct or draw the fundamental domains"}, {"corners", "find the shape automatically"}, {"centers", "may produce less corners"}, {"special", "nice domains for specific quotient shapes"}}, "fundamental domain method", 'm')
    ->set_reaction(clear_data);
    param_color(sett.color_main, "fundamental_color_main", true, 0xFFFFFFFF)
    ->editable("boundary of the central domain", "", 'c');
    param_color(sett.color_other, "fundamental_color_other", true, 0xFFFFFF40)
    ->editable("boundary of the other domains", "", 'o');
    param_color(sett.color_mirage, "fundamental_color_mirage", false, winf[waCloud].color)
    ->editable("gluing label color", "", 'd');
    param_color(sett.color_mirror, "fundamental_color_mirror", false, winf[waMirror].color)
    ->editable("mirrored gluing label color", "", 'b');
    param_b(sett.single_edges, "fundamental_single_edges", false)
    ->editable("fundamental: single edges", '1')
    ->set_reaction(clear_data);
    param_b(sett.fill_faces, "fundamental_fill_faces", true)
    ->editable("fundamental: fill faces", '2')
    ->set_reaction(clear_data);
    param_f(sett.label_dist, "fundamental_label_dist", .3)
    ->editable(0, 10, .05, "gluing label distance", "", 'd');
    param_f(sett.widthfactor, "fundamental_width", 5)
    ->editable(0, 5, 1, "domain boundary width factor", "", 'w');
    param_f(sett.label_scale, "fundamental_label_scale", 1)
    ->editable(0, 10, 0.2, "gluing label scale", "", 's');
    param_i(sett.lq, "fundamental_lq", 3)
    ->editable(0, 5, 1, "domain line quality", "", 'w');
    param_i(sett.alpha, "fundamental_alpha", 32)
    ->editable(0, 5, 8, "opacity of domain fill colors", "", 'a');
    });
  return 0;
  }

auto fundamentalhook = enable_fundamental();
}
}
