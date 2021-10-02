#include "hyper.h"

namespace hr {

EX namespace intra {

EX bool in;

#if HDR
/** information per every space connected with intra-portals */
struct intra_data {
  gamedata gd;
  geometryinfo gi;
  int wallindex;  
  };
#endif

EX vector<intra_data> data;

/** index of the space we are currently in */
EX int current;

/** map cells to their intra spaces */
EX map<cell*, int> intra_id;

#if HDR
/** information about portal (one side) */
struct portal_data {
  int kind;
  hyperpoint v0;
  ld d;
  transmatrix T;
  transmatrix iT;
  hyperpoint co0;
  hyperpoint co1;
  ld scale;
  /* convert h to portal coordinates ('poco') to usual coordinates */
  hyperpoint to_poco(hyperpoint h) const;
  /* convert h from portal coordinates ('poco') to usual coordinates */
  hyperpoint from_poco(hyperpoint h) const;
  };
#endif

hyperpoint portal_data::to_poco(hyperpoint h) const {
  if(prod && kind == 1) {
    auto dec = product_decompose(h);
    h = dec.second;
    h[0] /= h[2];
    h[1] /= h[2];
    h[2] = dec.first - d;
    h[3] = 1;
    if(d<0) h[2] = -h[2], h[0] = -h[0];
    return h;
    }
  else if(prod && kind == 0) {
    h = T * h;
    ld z = product_decompose(h).first;
    h /= exp(z);
    auto h1 = h;
    h[0] = asin_auto(h1[1]);
    h[1] = z;
    h[2] = asin_auto_clamp(h1[0] / cos_auto(h[0]));
    h[3] = 1;
    return h;
    }
  else {
    h = T * h;
    h /= h[3];
    return h;
    }
  }

hyperpoint portal_data::from_poco(hyperpoint h) const {
  if(prod && kind == 1) {
    ld xd = h[2];
    if(d<0) xd = -xd, h[0] = -h[0];
    h[2] = 1;
    auto z = product_decompose(h).first;
    return h * exp(d+xd-z);
    }
  else if(prod && kind == 0) {
    auto h0 = h;
    h[0] = sin_auto(h0[2]) * cos_auto(h0[0]);
    h[1] = sin_auto(h0[0]);
    h[2] = cos_auto(h0[0]) * cos_auto(h0[2]);
    h[3] = 1;
    return iT * h * exp(h0[1]);
    }
  else {
    h[3] = 1;
    return normalize(iT * h);
    }
  }

EX portal_data make_portal(cellwalker cw, int spin) {
  auto& ss = currentmap->get_cellshape(cw.at);
  auto& fac = ss.faces[cw.spin];
  portal_data id;
  id.scale = 1;
  if(prod && cw.spin == cw.at->type - 1) {
    id.kind = 1;
    id.d = product_decompose(fac[0]).first;
    id.v0 = C0 * exp(id.d);
    }
  else if(prod && cw.spin == cw.at->type - 2) {
    id.kind = 1;
    id.d = product_decompose(fac[0]).first;
    id.v0 = C0 * exp(id.d);
    }
  else if(prod) {
    id.kind = 0;
    id.v0 = Hypc;
    id.scale = cgi.plevel;
    for(auto p: fac) id.v0 += p;
    id.v0 /= sqrt(abs(intval(id.v0, Hypc)));
    id.T = cpush(0, -hdist0(id.v0)) * spintox(id.v0);
    for(int i=0; i<3; i++) id.T[3][i] = id.T[i][3] = i==3;
    if(debugflags & DF_GEOM)
    for(int a=0; a<4; a++) {
      hyperpoint h = fac[a];
      println(hlog, kz(h), " -> ", kz(spintox(id.v0)*h), " -> ", kz(cpush(0, -hdist0(id.v0))) * kz(spintox(id.v0) * h), " -> ", kz(id.to_poco(h)));
      }
    }
  else {
    id.kind = 0;
    id.v0 = Hypc;
    for(auto p: fac) id.v0 += p;
    id.v0 = normalize(id.v0);
    id.T = cpush(2, -hdist0(id.v0)) * cspin(2, 0, 90*degree) * spintox(id.v0);
    }
  id.iT = inverse(id.T);
  if(MDIM == 3) for(int i=0; i<4; i++) id.iT[3][i] = id.iT[i][3] = i==3;
  int first = spin;
  int second = spin + 1;
  first = gmod(first, isize(fac));
  second = gmod(second, isize(fac));
  id.co0 = id.to_poco(fac[first]);
  id.co1 = id.to_poco(fac[second]);
  return id;
  }

#if HDR
/** information about connection (portal-to-portal) */
struct connection_data {
  int source_world;
  int target_world;
  cellwalker scw, tcw;
  portal_data id1;
  portal_data id2;
  transmatrix T;
  int spin_value;
  bool mirrored; /* not implemented */
  };
#endif

EX map<cellwalker, connection_data> connections;

EX connection_data* find_connection(int a, int b) {
  for(auto& p: connections)
    if(intra_id[p.first.at] == a && p.second.target_world == b)
      return &p.second;
  return nullptr;
  }

EX void switch_to(int id) {
  if(current == id) return;
  data[current].gd.storegame();
  current = id;
  ginf[gProduct] = data[current].gi;
  data[current].gd.restoregame();
  }

void connect_portal_1(cellwalker cw1, cellwalker cw2, int spin) {
  auto& p = connections[cw1];
  p.source_world = intra_id[cw1.at];
  p.target_world = intra_id[cw2.at];
  p.scw = cw1;
  p.tcw = cw2;
  switch_to(intra_id[cw1.at]);
  p.id1 = make_portal(cw1, 0);
  switch_to(intra_id[cw2.at]);
  p.id2 = make_portal(cw2, spin);

  if(1) {
    dynamicval<eGeometry> g(geometry, gCubeTiling);
    transmatrix T1;
    set_column(T1, 0, p.id1.co0);
    set_column(T1, 1, p.id1.co1);
    set_column(T1, 2, hyperpoint(0,0,p.id1.scale,0));
    set_column(T1, 3, C03);
    transmatrix T2;
    set_column(T2, 0, p.id2.co0);
    set_column(T2, 1, p.id2.co1);
    set_column(T2, 2, hyperpoint(0,0,-p.id2.scale,0));
    set_column(T2, 3, C03);
    if(debugflags & DF_GEOM) for(int i=0; i<4; i++)
      println(hlog, "mapping ", get_column(T1, i), " to ", get_column(T2, i));
    p.T = T2 * inverse(T1);

    if(det(p.T) < 0) {
      set_column(T2, 0, p.id2.co1);
      set_column(T2, 1, p.id2.co0);
      p.T = T2 * inverse(T1);
      }
    }
  }

EX vector<pair<int, cell*>> full_sample_list;

EX void connect_portal(cellwalker cw1, cellwalker cw2, int spin) {
  connect_portal_1(cw1, cw2, spin);
  connect_portal_1(cw2, cw1, -spin);
  }

/** make currentmap into one of the spaces in intra */
EX void become() {
  check_cgi();
  cgi.require_shapes();
  auto& ac = currentmap->allcells();
  current = isize(data);
  for(cell *c: ac)
    intra_id[c] = current;
  for(cell *c: ac)
    currentmap->wall_offset(c);
  for(cell *c: ac) c->item = itNone;
  data.emplace_back();
  data.back().gd.storegame();
  data.back().gi = ginf[gProduct];

  auto v = hybrid::gen_sample_list();
  int gi = 0;
  if(full_sample_list.size()) {
    gi = full_sample_list.back().first;
    full_sample_list.pop_back();
    }
  data.back().wallindex = gi;
  for(auto x: v)
    full_sample_list.emplace_back(x.first + gi, x.second);
  sightranges[geometry] = 10;
  }

/** after called become() on some spaces, actually start intra */
EX void start(int id IS(0)) {
  in = true;
  current = id;
  data[current].gd.restoregame();
  ginf[gProduct] = data[current].gi;
  }

#if HDR
/** a convenience struct to switch back after a temporary switch_to */
struct resetter {
  int ic;
  resetter() { ic = current; }
  ~resetter() { if(in) switch_to(ic); }
  };
#endif

EX void may_switch_to(cell *c) {
  if(in) switch_to(intra_id[c]);
  }

EX int full_wall_offset(cell *c) {
  int wo = currentmap->wall_offset(c);
  if(in) wo += data[intra_id[c]].wallindex;
  return wo;
  }

EX void check_portal_movement() {
  transmatrix iView = view_inverse(View);
  ld dist = hdist0(iView * C0);
  int nei = -1;
  for(int i=0; i<centerover->type; i++) {
    ld dist1 = hdist0(currentmap->ray_iadj(centerover, i) * iView * C0);
    if(dist1 < dist) nei = i, dist = dist1;
    }

  auto cw1 = cellwalker(centerover, nei);
  ld c = camera_speed;
  auto p = at_or_null(connections, cw1);
  if(p) {
    ld eps = 1e-3;
    c /= p->id1.scale;
    anims::cycle_length /= p->id1.scale;

    array<hyperpoint, 3> ds; /* camera, forward, upward */
    ds[0] = inverse(View) * C0;
    ds[1] = inverse(get_shift_view_of(ctangent(2, -eps), View)) * C0;
    ds[2] = inverse(get_shift_view_of(ctangent(1, +eps), View)) * C0;

    for(auto& h: ds) h = p->id1.to_poco(h);

    /* reset the original */
    View = Id; NLP = Id;

    switch_to(p->target_world);
    centerover = p->tcw.at;

    if(1) {
      dynamicval<eGeometry> g(geometry, gCubeTiling);
      for(auto& h: ds) h = p->T * h;
      }

    for(auto& h: ds) h = p->id2.from_poco(h);

    set_view(ds[0], ds[1], ds[2]);
    
    c *= p->id2.scale;
    anims::cycle_length *= p->id2.scale;
    camera_speed = c;
    }
  }

vector<cellwalker> unconnected;

void erase_unconnected(cellwalker cw) {
  for(int i=0; i<isize(unconnected); i++)
    if(unconnected[i] == cw)
      unconnected.erase(unconnected.begin() + i);
  }

int edit_spin;

void show_portals() {
  gamescreen(1);

  dialog::init(XLAT("manage portals"));

  cellwalker cw(centerover, point_direction);
  bool valid = point_direction >= 0 && point_direction < centerover->type;

  dialog::addItem(XLAT("move to the next space"), 'm');
  dialog::add_action([] {
    int ic = (current + 1) % isize(data);
    switch_to(ic);
    });

  bool in_list = false; for(cellwalker x: unconnected) if(x == cw) in_list = true;

  if(!valid) ;
  else if(connections.count(cw)) {
    dialog::addItem(XLAT("disconnect this portal"), 'd');
    dialog::add_action([cw] {
      auto tcw = connections[cw].tcw;
      unconnected.push_back(tcw);
      connections.erase(cw);
      connections.erase(tcw);
      });
    }
  else if(in_list) {
    dialog::addItem(XLAT("remove %1 from the list", lalign(0, cw)), 'r');
    dialog::add_action([cw] {
      erase_unconnected(cw);
      });
    }
  else {
    dialog::addItem(XLAT("add to list"), 'a');
    dialog::add_action([cw] { unconnected.push_back(cw); });
    for(auto p: unconnected) {
      dialog::addItem(XLAT("connect " + lalign(0, p)), '1');
      dialog::add_action([p, cw] {
        connect_portal(cw, p, edit_spin);
        erase_unconnected(p);
        });
      }
    dialog::addSelItem(XLAT("portal orientation"), its(edit_spin), 'o');
    dialog::add_action([] { edit_spin = edit_spin + 1; });
    }

  dialog::display();
  }

#if HDR
struct portal_to_save {
  cellwalker cw1;
  cellwalker cw2;
  int spin;
  bool mirrored;
  };
#endif

EX vector<portal_to_save> portals_to_save;

EX void prepare_to_save() {
  portals_to_save.clear();
  for(auto c: connections) if(c.second.scw < c.second.tcw) {
    portals_to_save.emplace_back(portal_to_save{c.second.scw, c.second.tcw, c.second.spin_value, false});
    }
  }

EX void load_saved_portals() {
  for(const auto& p: portals_to_save) connect_portal(p.cw1, p.cw2, p.spin);
  }

auto hooks1 =
  addHook(hooks_o_key, 90, [] (o_funcs& v) {
    if(intra::in) v.push_back(named_dialog(XLAT("manage portals"), show_portals));
    })
  + arg::add3("-intra-add", [] { start_game(); become(); })
  + arg::add3("-intra-start", [] { start_game(); become(); start(0); })
  + arg::add3("-be-square", [] { 
      check_cgi();
      cgi.require_basics();
      vid.plevel_factor = cgi.edgelen / cgi.scalefactor;
      check_cgi();
      cgi.require_basics();
      });


EX }

}