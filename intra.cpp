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

/** portal debugging flags */
EX int debug_portal;

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
    if(bt::in()) {
      h = PIU( deparabolic13(h) );
      h[2] = dec.first - d;
      h[3] = 1;
      }
    else {
      h = T * h;
      h[0] /= h[2];
      h[1] /= h[2];
      h[2] = dec.first - d;
      h[3] = 1;
      }
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
  else if(hyperbolic && bt::in()) {
    h = deparabolic13(h);
    h[3] = 1;
    tie(h[0], h[1], h[2]) = make_tuple(h[1], h[2], h[0]);
    h = T * h;
    h[2] *= exp(h[1]);
    return h;
    }
  else if(sol) {
    h = T * h;
    h[2] *= exp(-h[1]);
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
    if(bt::in()) {
      h[2] = 0;
      return PIU( parabolic13(h) ) * exp(d+xd);
      }
    h[2] = 1;
    auto z = product_decompose(h).first;
    return iT * h * exp(d+xd-z);
    }
  else if(prod && kind == 0) {
    auto h0 = h;
    h[0] = sin_auto(h0[2]) * cos_auto(h0[0]);
    h[1] = sin_auto(h0[0]);
    h[2] = cos_auto(h0[0]) * cos_auto(h0[2]);
    h[3] = 1;
    return iT * h * exp(h0[1]);
    }
  else if(hyperbolic && bt::in()) {
    h[2] *= exp(-h[1]);
    h = iT * h;
    return hr::parabolic13(h[0], h[1]) * xpush0(h[2]);
    }
  else if(sol) {
    h[2] *= exp(h[1]);
    return iT * h;
    }
  else {
    h[3] = 1;
    return normalize(iT * h);
    }
  }

EX portal_data make_portal(cellwalker cw, int spin) {
  if(debug_portal & 33)
    println(hlog, "working in: ", full_geometry_name());
  auto& ss = currentmap->get_cellshape(cw.at);
  auto fac = ss.faces_local[cw.spin];
  portal_data id;
  id.scale = 1;
  auto gg = geometry;
  if(prod && cw.spin >= cw.at->type - 2) {
    id.kind = 1;
    id.d = product_decompose(fac[0]).first;
    id.v0 = C0 * exp(id.d);
    if(bt::in()) {
      fac.pop_back();
      id.scale = log(2)/2;
      }
    else {
      hyperpoint ctr = Hypc;
      for(auto p: fac) ctr += product_decompose(p).second;
      ctr = normalize_flat(ctr);
      id.T = gpushxto0(ctr);
      }
    }
  else if(prod) {
    id.kind = 0;
    id.v0 = Hypc;
    id.scale = cgi.plevel;
    for(auto p: fac) id.v0 += p;
    id.v0 /= sqrt(abs(intval(id.v0, Hypc)));
    hyperpoint h = fac[0];
    h /=  sqrt(abs(intval(h, Hypc)));
    id.T = cspin(0, 1, -90*degree) * spintox(gpushxto0(id.v0) * h) * gpushxto0(id.v0);
    for(int i=0; i<3; i++) id.T[3][i] = id.T[i][3] = i==3;
    if(debugflags & DF_GEOM)
    for(int a=0; a<4; a++) {
      hyperpoint h = fac[a];
      println(hlog, kz(h), " -> ", kz(spintox(id.v0)*h), " -> ", kz(cpush(0, -hdist0(id.v0))) * kz(spintox(id.v0) * h), " -> ", kz(id.to_poco(h)));
      }
    }
  else if(bt::in()) {
    hyperpoint removed = Hypc;

    auto facmod = fac;
    if(hyperbolic) for(auto& h: facmod) h = deparabolic13(h);

    for(int i=0; i<isize(facmod); i++) {
      int i1 = i+1; if(i1 >= isize(facmod)) i1 = 0;
      int i2 = i1+1; if(i2 >= isize(facmod)) i2 = 0;
      if(hypot_d(3, 2*facmod[i1] - facmod[i] - facmod[i2]) < 1e-3) {
        removed = fac[i1];
        facmod.erase(facmod.begin()+i1);
        fac.erase(fac.begin()+i1);
        }
      }
    id.kind = 0;
    id.v0 = Hypc;
    id.T = Id;

    auto fac1 = fac;
    auto to_coords = [] (hyperpoint& p) {
      if(hyperbolic) {
        p = deparabolic13(p);
        p = hyperpoint(p[1], p[2], p[0], 1);
        }
      };
    for(auto& p: fac1)
      to_coords(p);
    to_coords(removed);

    for(auto p: fac1) id.v0 += p;
    id.v0 /= isize(fac);
    dynamicval<eGeometry> g(geometry, gCubeTiling);
    id.T = gpushxto0(id.v0);
    for(auto p: fac1) {
      if(abs((id.T * p)[2]) > 1e-3 && abs((id.T * p)[0]) < 1e-3)
        id.T = cspin(2, 0, 90*degree) * id.T;
      if(abs((id.T * p)[2]) > 1e-3 && abs((id.T * p)[1]) < 1e-3)
        id.T = cspin(2, 1, 90*degree) * id.T;
      }
    if((id.T * C03)[2] > 0) id.T = cspin(2, 0, 180*degree) * id.T;
    if(abs((id.T * removed)[0]) > 1e-2) id.T = cspin(0, 1, 90*degree) * id.T;
    if((id.T * removed)[1] < -1e-2) id.T = cspin(0, 1, 180*degree) * id.T;
    vector<hyperpoint> v;
    geometry = gg;
    for(auto f: fac) v.push_back(id.to_poco(f));
    geometry = gCubeTiling;
    ld sca = 1;
    for(int i=0; i<isize(v); i++)
      sca *= sqhypot_d(3, v[i] - v[(1+i) % isize(v)]);
    sca = pow(sca, .5 / isize(v));
    id.scale = sca / 2;
    }
  else {
    id.kind = 0;
    id.v0 = project_on_triangle(fac[0], fac[1], fac[2]);
    id.T = cpush(2, -hdist0(id.v0)) * cspin(2, 0, 90*degree) * spintox(id.v0);
    hyperpoint ctr = Hypc;
    for(auto p: fac) ctr += id.T*p;
    ctr = normalize(ctr);
    id.T = gpushxto0(ctr) * id.T;
    }
  if(MDIM == 3) for(int i=0; i<4; i++) id.T[3][i] = id.T[i][3] = i==3;
  id.iT = inverse(id.T);
  if(MDIM == 3) for(int i=0; i<4; i++) id.iT[3][i] = id.iT[i][3] = i==3;
  int first = spin;
  int second = spin + 1;
  first = gmod(first, isize(fac));
  second = gmod(second, isize(fac));
  id.co0 = id.to_poco(fac[first]);
  id.co1 = id.to_poco(fac[second]);

  if(debug_portal & 32) {
    for(int i=0; i<isize(fac); i++)
      println(hlog, "edge ", i, " length is ", hdist(fac[i], fac[(i+1)%isize(fac)]));
    println(hlog, "chosen edge is ", first, "--", second);
    }

  if(debug_portal & 1) for(auto p: fac) {
    auto p2 = id.to_poco(p);
    auto p3 = id.from_poco(p2);
    println(hlog, kz(p), " > ", kz(p2), " > ", kz(p3));
    }

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
    if(intra_id.at(p.first.at) == a && p.second.target_world == b)
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
  p.source_world = intra_id.at(cw1.at);
  p.target_world = intra_id.at(cw2.at);
  p.scw = cw1;
  p.tcw = cw2;
  switch_to(intra_id.at(cw1.at));
  int pspin = 0, nspin = 0;
  if(spin > 0) pspin = spin; else nspin = -spin;
  p.id1 = make_portal(cw1, nspin);
  switch_to(intra_id.at(cw2.at));
  p.id2 = make_portal(cw2, pspin);
  p.spin_value = spin;

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
    if(debug_portal & 2) for(int i=0; i<4; i++)
      println(hlog, "mapping [", p.source_world, "]", get_column(T1, i), " to [", p.target_world, "] ", get_column(T2, i));
    p.T = T2 * inverse(T1);

    if(debug_portal & 2)
      println(hlog, "det = ", det(p.T));

    if(det(p.T) < 0) {
      set_column(T2, 0, p.id2.co1);
      set_column(T2, 1, p.id2.co0);
      p.T = T2 * inverse(T1);
      }
    }

  if(debug_portal & 2)
    println(hlog, "got scale = ", tie(p.id1.scale, p.id2.scale));

  if(debug_portal & 4) for(int i=0; i<5; i++) {
    hyperpoint h = C03;
    if(i == 1) h[0] += .2;
    if(i == 2) h[0] -= .2;
    if(i == 3) h[1] += .2;
    if(i == 4) h[1] -= .2;
    array<hyperpoint, 4> hl;
    ld eps = 1e-5; ld ss = pow(eps, -2);
    hl[0] = h;
    hl[1] = h + point3(eps, 0, 0);
    hl[2] = h + point3(0, eps, 0);
    hl[3] = h + point3(0, 0, eps);
    auto hl1 = hl;
    may_switch_to(cw1.at);
    println(hlog, "checking ", h, " -> L ", kz(p.id1.from_poco(h)), " g: ", full_geometry_name());
    for(auto& hx: hl1) hx = p.id1.from_poco(hx);
    print(hlog, "L side: "); analyze_orthonormal(hl1, ss);
    may_switch_to(cw2.at);
    println(hlog, "checking ", h, " -> R ", kz(p.id2.from_poco(p.T * h)), " g: ", full_geometry_name());
    auto hl2 = hl;
    for(auto& hx: hl2) hx = p.id2.from_poco(p.T * hx);
    print(hlog, "R side: "); analyze_orthonormal(hl2, ss);
    }

  }

EX vector<pair<int, cell*>> full_sample_list;

EX void connect_portal(cellwalker cw1, cellwalker cw2, int spin) {
  connect_portal_1(cw1, cw2, spin);
  connect_portal_1(cw2, cw1, -spin);
  }

/** make currentmap into one of the spaces in intra */
EX void become() {
  if(intra::in) {
    /* let them add more spaces in this case */
    data[current].gd.storegame();
    intra::in = false;
    return;
    }

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

  again:
  int missing = 0;
  for(auto p: intra_id)
    for(int i=0; i<p.first->type; i++) {
      cell *c1 = p.first->move(i);
      if(!c1) continue;
      if(intra_id.count(c1) == 0) {
        intra_id[c1] = p.second;
        missing++;
        }
      }
  if(debug_portal & 64) println(hlog, "missing = ", missing);
  if(missing) goto again;
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
  if(in) switch_to(intra_id.at(c));
  }

EX int full_wall_offset(cell *c) {
  int wo = currentmap->wall_offset(c);
  if(in) wo += data[intra_id.at(c)].wallindex;
  return wo;
  }

ld dsdet(array<hyperpoint, 4> ds) {
  transmatrix T;
  set_column(T, 0, ds[1]-ds[0]);
  set_column(T, 1, ds[2]-ds[0]);
  set_column(T, 2, ds[3]-ds[0]);
  return det3(T);
  }

EX void analyze_orthonormal(array<hyperpoint, 4> ds, ld sca) {
  transmatrix T = gpushxto0(ds[0]);
  vector<ld> orths;
  for(int i: {1,2,3}) {
    ds[i] = T * ds[i];
    if(prod) ds[i][2]--;
    }
  for(int i=0; i<3; i++)
  for(int j=0; j<3; j++)
    orths.push_back(dot_d(3, ds[i+1], ds[j+1]) * sca);
  println(hlog, "orths = ", kz(orths));
  }

EX void shift_view_portal(hyperpoint H) {
  shift_view(H);
  if(!through_portal()) return;
  shift_view(-H);
  ld minv = 0, maxv = 1;
  for(int i=0; i<30; i++) {
    ld t = (minv + maxv) / 2;
    shift_view(H * t);
    bool b = through_portal();
    if(b) maxv = t; else minv = t;
    shift_view(H * -t);
    }
  println(hlog, "maxv = ", maxv);
  shift_view(H * maxv);
  check_portal_movement();
  shift_view(H * (1 - maxv));
  }

EX const connection_data* through_portal() {
  transmatrix iView = view_inverse(View);
  ld dist = hdist0(iView * C0);
  int nei = -1;
  for(int i=0; i<centerover->type; i++) {
    ld dist1 = hdist0(currentmap->ray_iadj(centerover, i) * iView * C0);
    if(dist1 < dist) nei = i, dist = dist1;
    }

  auto cw1 = cellwalker(centerover, nei);
  return at_or_null(connections, cw1);
  }

EX void check_portal_movement() {
  auto p = through_portal();
  ld c = camera_speed;
  if(p) {
    ld eps = 1e-5;
    c /= p->id1.scale;
    anims::cycle_length /= p->id1.scale;
    ld ss = pow(eps, -2);

    array<hyperpoint, 4> ds; /* camera, forward, upward */
    ds[0] = inverse(View) * C0;
    ds[1] = inverse(get_shift_view_of(ctangent(2, -eps), View)) * C0;
    ds[2] = inverse(get_shift_view_of(ctangent(1, +eps), View)) * C0;
    ds[3] = inverse(get_shift_view_of(ctangent(0, +eps), View)) * C0;
    if(debug_portal & 8) {
      println(hlog, "at = ", ds[0], " det = ", dsdet(ds), " bt = ", bt::minkowski_to_bt(ds[0]));
      analyze_orthonormal(ds, ss);
      }

    for(auto& h: ds) h = p->id1.to_poco(h);

    if(debug_portal & 8) {
      println(hlog, "poco: at = ", ds[0], " det = ", dsdet(ds));
      if(debug_portal & 16) {
        dynamicval<eGeometry> g(geometry, gCubeTiling);
        analyze_orthonormal(ds, ss);
        }
      }

    /* reset the original */
    View = Id; NLP = Id;

    switch_to(p->target_world);
    centerover = p->tcw.at;

    if(1) {
      dynamicval<eGeometry> g(geometry, gCubeTiling);
      for(auto& h: ds) h = p->T * h;
      }

    if(debug_portal & 8) {
      println(hlog, "poco2: at = ", ds[0], " det = ", dsdet(ds));
      if(debug_portal & 16) {
        dynamicval<eGeometry> g(geometry, gCubeTiling);
        analyze_orthonormal(ds, ss);
        }
      }

    for(auto& h: ds) h = p->id2.from_poco(h);

    if(debug_portal & 8) {
      println(hlog, "goal: at = ", ds[0], " det = ", dsdet(ds));
      analyze_orthonormal(ds, ss);
      }

    set_view(ds[0], ds[1], ds[2]);

    if(debug_portal & 8) {
      array<hyperpoint, 4> xds; /* camera, forward, upward */
      xds[0] = inverse(View) * C0;
      xds[1] = inverse(get_shift_view_of(ctangent(2, -eps), View)) * C0;
      xds[2] = inverse(get_shift_view_of(ctangent(1, +eps), View)) * C0;
      xds[3] = inverse(get_shift_view_of(ctangent(0, +eps), View)) * C0;
      println(hlog, "goal: at = ", xds[0], " det = ", dsdet(xds), " bt = ", bt::minkowski_to_bt(xds[0]));
      }

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

  if(debug_portal) {
    dialog::addItem(XLAT("debug"), 'd');
    dialog::add_action([] {
      ld eps = 1e-5;
      array<hyperpoint, 4> ds; /* camera, forward, upward */
      ds[0] = inverse(View) * C0;
      ds[1] = inverse(get_shift_view_of(ctangent(2, -eps), View)) * C0;
      ds[2] = inverse(get_shift_view_of(ctangent(1, +eps), View)) * C0;
      ds[3] = inverse(get_shift_view_of(ctangent(0, +eps), View)) * C0;
      set_view(ds[0], ds[1], ds[2]);
      });
    }

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
    if(debug_portal) {
      dialog::addItem(XLAT("mirror connection"), 'm');
      dialog::add_action([cw] { connect_portal(cw, cw, edit_spin); });
      }
    if(debug_portal) {
      dialog::addItem(XLAT("test portal here"), 't');
      dialog::add_action([cw] { make_portal(cw, 0); });
      }
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
      PIU( vid.plevel_factor = cgi.edgelen / cgi.scalefactor );
      check_cgi();
      cgi.require_basics();
      })
  + arg::add3("-debug-portal", [] { arg::shift(); debug_portal = arg::argi(); });


EX }

}