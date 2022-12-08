#include "hyper.h"

namespace hr {

EX namespace intra {

EX bool in;

#if CAP_RAY && MAXMDIM >= 4
#if HDR
/** information per every space connected with intra-portals */
struct intra_data {
  gamedata gd;
  geometryinfo gi;
  int wallindex;  
  };
#endif

EX vector<intra_data> data;

/** tells gamedata store that we are just storing one world */
EX bool switching;

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
  if(mproduct && kind == 1) {
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
  else if(mproduct && kind == 0) {
    h = T * h;
    ld z = product_decompose(h).first;
    h /= exp(z);
    auto h1 = h;
    h[2] = asin_auto_clamp(h1[0]);
    h[1] = z;
    h[0] = asin_auto_clamp(h1[1] / cos_auto(h[2]));
    h[3] = 1;
    return h;
    }
  #if CAP_BT
  else if(hyperbolic && bt::in()) {
    h = deparabolic13(h);
    h[3] = 1;
    tie(h[0], h[1], h[2]) = make_tuple(h[1], h[2], h[0]);
    h = T * h;
    h[2] *= exp(h[1]);
    return h;
    }
  #endif
  else if(sol) {
    h = T * h;
    h[2] *= exp(-h[1]);
    return h;
    }
  else {
    h = T * h;
    h /= h[3];
    if(sphere)
      h[2] /= sqrt(1+h[0]*h[0]+h[1]*h[1]);
    if(hyperbolic)
      h[2] /= sqrt(1-h[0]*h[0]-h[1]*h[1]);
    return h;
    }
  }

hyperpoint portal_data::from_poco(hyperpoint h) const {
  if(mproduct && kind == 1) {
    ld xd = h[2];
    if(d<0) xd = -xd, h[0] = -h[0];
    #if CAP_BT
    if(bt::in()) {
      h[2] = 0;
      return PIU( parabolic13(h) ) * exp(d+xd);
      }
    #endif
    h[2] = 1;
    auto z = product_decompose(h).first;
    return iT * h * exp(d+xd-z);
    }
  else if(mproduct && kind == 0) {
    auto h0 = h;
    h[0] = sin_auto(h0[2]);
    h[1] = sin_auto(h0[0]) * cos_auto(h0[2]);
    h[2] = cos_auto(h0[0]) * cos_auto(h0[2]);
    h[3] = 1;
    return iT * h * exp(h0[1]);
    }
  #if CAP_BT
  else if(hyperbolic && bt::in()) {
    h[2] *= exp(-h[1]);
    h = iT * h;
    return hr::parabolic13(h[0], h[1]) * xpush0(h[2]);
    }
  else if(sol) {
    h[2] *= exp(h[1]);
    return iT * h;
    }
  #endif
  else {
    h[3] = 1;
    if(sphere)
      h[2] *= sqrt(1+h[0]*h[0]+h[1]*h[1]);
    if(hyperbolic)
      h[2] *= sqrt(1-h[0]*h[0]-h[1]*h[1]);
    return normalize(iT * h);
    }
  }

EX portal_data make_portal(cellwalker cw, int spin) {
  if(debug_portal & 289)
    println(hlog, "working in: ", full_geometry_name(), " wall no ", cw.spin, "/", cw.at->type);
  auto& ss = currentmap->get_cellshape(cw.at);
  auto fac = ss.faces_local[cw.spin];
  portal_data id;
  id.scale = 1;
  id.T = Id;
  auto gg = geometry;
  if(mproduct && cw.spin >= cw.at->type - 2) {
    id.kind = 1;
    id.d = product_decompose(fac[0]).first;
    id.v0 = C0 * exp(id.d);
    #if CAP_BT
    if(bt::in()) {
      for(auto h: fac)
        println(hlog, PIU(deparabolic13(normalize_flat(h))));
      if(cw.spin == cw.at->type - 2)
        fac.pop_back();
      else
        fac.erase(fac.begin() + 1);
      id.scale = log(2)/2;
      }
    #else
    if(false) {}
    #endif
    else {
      hyperpoint ctr = Hypc;
      for(auto p: fac) ctr += product_decompose(p).second;
      ctr = normalize_flat(ctr);
      id.T = gpushxto0(ctr);
      }
    }
  else if(mproduct) {
    id.kind = 0;
    id.v0 = Hypc;
    id.scale = cgi.plevel;
    for(auto p: fac) id.v0 += p;
    id.v0 = normalize_flat(id.v0);
    hyperpoint h = normalize_flat(fac[0]);
    id.T = cspin90(1, 0) * spintox(gpushxto0(id.v0) * h) * gpushxto0(id.v0);
    if((id.T * C0)[0] > 0) id.T = spin180() * id.T;
    for(int i=0; i<3; i++) id.T[3][i] = id.T[i][3] = i==3;
    if(debug_portal & 128)
    for(int a=0; a<4; a++) {
      hyperpoint h = fac[a];
      println(hlog, kz(h), " -> ", kz(spintox(id.v0)*h), " -> ", kz(cpush(0, -hdist0(id.v0))) * kz(spintox(id.v0) * h), " -> ", kz(id.to_poco(h)));
      }
    }
  #if CAP_BT
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
        id.T = cspin90(2, 0) * id.T;
      if(abs((id.T * p)[2]) > 1e-3 && abs((id.T * p)[1]) < 1e-3)
        id.T = cspin90(2, 1) * id.T;
      }
    if((id.T * C03)[2] > 0) id.T = cspin180(2, 0) * id.T;
    if(abs((id.T * removed)[0]) > 1e-2) id.T = cspin90(0, 1) * id.T;
    if((id.T * removed)[1] < -1e-2) id.T = cspin180(0, 1) * id.T;
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
  #endif
  else {
    id.kind = 0;
    id.v0 = project_on_triangle(fac[0], fac[1], fac[2]);
    id.T = cpush(2, -hdist0(id.v0)) * cspin90(2, 0) * spintox(id.v0);
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

  if(debug_portal & 256) {
    println(hlog, "portal scale = ", id.scale);
    auto res = [&] (ld x, ld y, ld z) {
      hyperpoint h = hyperpoint(x, y, z, 1);
      return id.from_poco(h);
      };
    for(int x=0; x<5; x++) {
      println(hlog, "horizontal ", x, " = ", hdist(res(x*.1,0,0), res(x*.1+.001,0,0)));
      println(hlog, "vertical   ", x, " = ", hdist(res(x*.1,0,0), res(x*.1,0.001,0)));
      println(hlog, "deep       ", x, " = ", hdist(res(x*.1,0,0), res(x*.1,0,0.001)));
      }
    hyperpoint a = hyperpoint(.4, .2, .1, 1);
    println(hlog, "a = ", kz(a));
    println(hlog, "b = ", kz(id.from_poco(a)));
    println(hlog, "c = ", kz(id.to_poco(id.from_poco(a))));
    }

  if(debug_portal & 1) {
    for(auto p: fac) {
      auto p2 = id.to_poco(p);
      auto p3 = id.from_poco(p2);
      println(hlog, kz(p), " > ", kz(p2), " > ", kz(p3));
      }
    println(hlog, kz(C0), " > ", kz(id.to_poco(C0)), " > ", kz(id.from_poco(id.to_poco(C0))));
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
  dynamicval<bool> is(switching, true);
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
      println(hlog, "mapping [", p.source_world, "]", get_column(T1, i), " to [", p.target_world, "] ", get_column(T2, i),
        " dists = ", hypot_d(2, get_column(T1,i)), ",", hypot_d(2, get_column(T2,i)));
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

EX void generate_sample_list_for_current() {
  auto v = hybrid::gen_sample_list();
  int gi = 0;
  if(full_sample_list.size()) {
    gi = full_sample_list.back().first;
    full_sample_list.pop_back();
    }
  data[current].wallindex = gi;
  for(auto x: v)
    full_sample_list.emplace_back(x.first + gi, x.second);
  println(hlog, "added ", isize(v)-1, " samples, wallindex = ", data[current].wallindex);
  }

EX void regenerate_full_sample_list() {
  resetter ir;
  full_sample_list.clear();
  for(int i=0; i<isize(data); i++) {
    switch_to(i);
    generate_sample_list_for_current();
    }
  println(hlog, isize(full_sample_list), " samples known");
  }

/** make currentmap into one of the spaces in intra */
EX void become() {
  dynamicval<bool> is(switching, true);
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
  generate_sample_list_for_current();
  sightranges[geometry] = 10;
  }

/** after called become() on some spaces, actually start intra */
EX void start(int id IS(0)) {
  in = true;
  current = id;
  dynamicval<bool> is(switching, true);
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
    if(mproduct) ds[i][2]--;
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
  shift_view_portal(H * (1 - maxv));
  }

EX const connection_data* through_portal() {
  transmatrix iView = view_inverse(View);
  int nei = through_wall(centerover, iView * C0);
  if(nei == -1) return nullptr;
  auto cw1 = cellwalker(centerover, nei);
  return at_or_null(connections, cw1);
  }

EX void check_portal_movement() {
  auto p = through_portal();

  if(p) {
    ld eps = 1e-5;
    ld ss = pow(eps, -2);

    array<hyperpoint, 4> ds; /* camera, forward, upward */
    ds[0] = inverse(View) * C0;
    ds[1] = inverse(get_shift_view_of(ctangent(2, -eps), View)) * C0;
    ds[2] = inverse(get_shift_view_of(ctangent(1, +eps), View)) * C0;
    ds[3] = inverse(get_shift_view_of(ctangent(0, +eps), View)) * C0;
    if(debug_portal & 8) {
      #if CAP_BT
      println(hlog, "at = ", ds[0], " det = ", dsdet(ds), " bt = ", bt::minkowski_to_bt(ds[0]));
      #endif
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
      #if CAP_BT
      println(hlog, "goal: at = ", xds[0], " det = ", dsdet(xds), " bt = ", bt::minkowski_to_bt(xds[0]));
      #endif
      }

    ld scale = p->id2.scale / p->id1.scale;

    camera_speed *= scale;
    anims::cycle_length *= scale;
    #if CAP_VR
    vrhr::absolute_unit_in_meters *= scale;
    #endif
    if(walking::eye_level != -1) walking::eye_level *= scale;

    walking::floor_dir = -1;
    walking::on_floor_of = nullptr;
    }
  }

vector<cellwalker> unconnected;

void erase_unconnected(cellwalker cw) {
  for(int i=0; i<isize(unconnected); i++)
    if(unconnected[i] == cw)
      unconnected.erase(unconnected.begin() + i);
  }

EX string portal_help =
  "In portal maps, you can create portals on walls. Such portals can be entered to end up in another world. "
  "The portals are 'immersive', that is, you can see through the portals. In the current implementation, "
  "the immersive view works only in the raycasting mode, for rendering walls -- so any other objects will not "
  "be rendered. (You can turn raycasting off to see the world without any portals.) As a consequence, "
  "there is no HyperRogue game in a portal map -- you can only move the camera around the scene. Pick "
  "'set recommended settings' in the manage portals menu to optimize settings for this.\n\n"
  "To create portals between different geometries, you need to create a map in the other geometry and save it "
  "(using the map editor), then in your portal map such a world can be loaded to be added to the scene "
  "(in the manage portals menu). Such portals work correctly when their intrinsic shapes are equal (including the "
  "curvature), their extrinsic curvatures match, and are actually implemented; see the video 'Portals to "
  "Non-Euclidean Geometries' for some portals that work.\n\n"
  ;

EX void become_menu() {
  cmode = sm::SIDE | sm::MAYDARK;
  gamescreen();
  dialog::init(XLAT("Become a portal map"));
  dialog::addHelp(XLAT(portal_help));
  dialog::addItem(XLAT("yes, that's what I want"), 'y');
  dialog::add_action([] {
    intra::become();
    intra::start();
    game_keys_scroll = true;
    mapeditor::drawplayer = false;
    popScreen();
    pushScreen(show_portals);
    });

  dialog::addBack();
  dialog::display();
  }

int edit_spin;

EX void world_list() {
  cmode = sm::SIDE | sm::MAYDARK;
  gamescreen();
  dialog::init(XLAT("world list"));
  dialog::start_list(900, 900, '1');
  int c = current;
  for(int i=0; i<isize(data); i++) {
    switch_to(i);
    dialog::addBoolItem(full_geometry_name(), i == c, dialog::list_fake_key++);
    dialog::add_action([i] {
      int ic = i;
      switch_to(ic);
      });
    }
  switch_to(c);
  dialog::end_list();
  dialog::addBreak(100);
  dialog::addItem("add a saved world to the scene", 'a');
  dialog::add_action([] {
    dialog::openFileDialog(levelfile, XLAT("level to load:"), ".lev", [] () {
      intra::become();
      if(mapstream::loadMap(levelfile.c_str())) {
        addMessage(XLAT("Map loaded from %1", levelfile));
        intra::become();
        intra::start();
        mapeditor::map_version++;
        ray::reset_raycaster();
        return true;
        }
      else {
        addMessage(XLAT("Failed to load map from %1", levelfile));
        intra::start();
        return false;
        }
      });
    });
  dialog::addBack();
  dialog::display();
  }

EX void show_portals() {
  cmode = sm::SIDE | sm::MAYDARK | sm::PANNING;
  if(mapeditor::building_mode) cmode |= sm::EDIT_INSIDE_WALLS;
  gamescreen();

  dialog::init(XLAT("manage portals"));

  cellwalker cw(mouseover2, point_direction);
  bool valid = point_direction >= 0 && point_direction < mouseover2->type;

  if(valid) {
    initquickqueue();
    for (const shiftmatrix& V : hr::span_at(current_display->all_drawn_copies, mouseover2)) {
      dynamicval<color_t> p(poly_outline, 0xFFFFFF);
      int ofs = currentmap->wall_offset(mouseover2);
      queuepolyat(V, cgi.shWireframe3D[ofs + point_direction], 0, PPR::SUPERLINE);
      }
    quickqueue();
    }

  dialog::addItem(XLAT("view another world"), 'm');
  dialog::add_action_push(world_list);

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
      mapeditor::map_version++;
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
    dialog::start_list(500, 500, '1');
    for(auto p: unconnected) {
      dialog::addItem(XLAT("connect " + lalign(0, p)), dialog::list_fake_key++);
      dialog::add_action([p, cw] {
        connect_portal(cw, p, edit_spin);
        mapeditor::map_version++;
        erase_unconnected(p);
        });
      }
    dialog::end_list();
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

  if(!game_keys_scroll || mapeditor::drawplayer || !ray::fixed_map || vid.cells_drawn_limit > 100) {
    dialog::addItem(XLAT("set recommended settings"), 'A');
    dialog::add_action([] {
      game_keys_scroll = true;
      mapeditor::drawplayer = false;
      ray::fixed_map = true;
      vid.cells_drawn_limit = 100;
      });
    }
  else {
    dialog::addItem(XLAT("configure raycasting"), 'A');
    dialog::add_action_push(ray::configure);
    }

  if(mproduct && point_direction < mouseover2->type - 2) {
    ld r = get_ratio_edge(mouseover2, point_direction);
    dialog::addSelItem(XLAT("height-to-width ratio"), fts(r), 'r');
    dialog::add_action([] {
      be_ratio_edge(mouseover2, point_direction);
      mapeditor::map_version++;
      ray::reset_raycaster();
      });
    }
  else dialog::addBreak(100);

  walking::add_options();

  dialog::addHelp();
  dialog::add_action([] { gotoHelp(portal_help); });

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

EX void be_ratio(ld v IS(1)) {
  check_cgi();
  cgi.require_basics();
  PIU( vid.plevel_factor = v * cgi.edgelen / cgi.scalefactor );
  check_cgi();
  cgi.require_basics();
  }

EX ld get_edge_length(cell *c, int i) {
  auto c1 = hybrid::get_where(c).first;
  return PIU( hdist(currentmap->get_corner(c1, i), currentmap->get_corner(c1, (i+c1->type-1)%c1->type)) );
  }

EX ld get_ratio_edge(cell *c, int i) {
  ld len = get_edge_length(c, i);
  return vid.plevel_factor * cgi.scalefactor / len;
  }

EX void be_ratio_edge(cell *c, int i, ld v IS(1)) {
  ld len = get_edge_length(c, i);
  vid.plevel_factor = v * len / cgi.scalefactor;
  check_cgi();
  cgi.require_basics();
  }

EX void be_ratio_edge(int i, ld v IS(1)) {
  start_game();
  be_ratio_edge(cwt.at, i, v);
  }

/** Remove the space with the given id. Turns off intra */
EX void kill(int id) {
  if(in) become();
  data.erase(data.begin()+id);
  vector<cellwalker> to_remove;
  for(auto& p: connections) if(intra_id[p.second.scw.at] == id || intra_id[p.second.tcw.at] == id)
    to_remove.push_back(p.first);
    else {
      if(p.second.source_world >= id) p.second.source_world--;
      if(p.second.target_world >= id) p.second.target_world--;
      }
  for(auto r: to_remove) connections.erase(r);
  vector<cell*> to_erase_cell;
  for(auto& p: intra_id)
    if(p.second == id) 
      to_erase_cell.push_back(p.first);
    else if(p.second > id)
      p.second--;
  for(auto c: to_erase_cell) 
    intra_id.erase(c);
  println(hlog, isize(to_remove), " connections and ", isize(to_erase_cell), " cells erased");
  }

EX void erase_all_maps() {
  println(hlog, "erase_all_maps called");
  dynamicval<bool> is(switching, true);
  data[current].gd.storegame();
  in = false;
  for(int i=0; i<isize(data); i++) {
    current = i;
    ginf[gProduct] = data[i].gi;
    data[i].gd.restoregame();
    clearCellMemory();
    }
  intra_id.clear();
  connections.clear();
  data.clear();
  full_sample_list.clear();
  }

EX set<cell*> need_to_save;

EX void prepare_need_to_save() {
  need_to_save.clear();
  map<cell*, cell*> parent;
  vector<cell*> q;
  cell *s = mapstream::save_start();
  parent[s] = s;
  q = {s};
  for(int i=0; i<isize(q); i++) {
    cell *c = q[i];
    forCellEx(c2, c)
      if(!parent.count(c2)) { parent[c2] = c; q.push_back(c2); }
    }
  for(int i=isize(q)-1; i>=0; i--) {
    cell *c = q[i];
    if(c == cwt.at) need_to_save.insert(c);
    for(auto& p: connections) if(p.first.at == c) need_to_save.insert(c);
    if(need_to_save.count(c)) need_to_save.insert(parent[c]);
    }
  println(hlog, "need to save ", isize(need_to_save), " out of ", isize(q), " cells");
  }

auto hooks1 =
  addHook(hooks_o_key, 90, [] (o_funcs& v) {
    if(intra::in) v.push_back(named_dialog(XLAT("manage portals"), show_portals));
    })
  + arg::add3("-intra-add", [] { start_game(); become(); })
  + arg::add3("-intra-start", [] { start_game(); become(); start(0); })
  + arg::add3("-intra-kill", [] { arg::shift(); kill(arg::argi()); start(0); regenerate_full_sample_list(); })
  + arg::add3("-be-square", [] { be_ratio(); })
  + arg::add3("-be-square-edge", [] {
      arg::shift(); int i = arg::argi(); be_ratio_edge(i);
      })
  + arg::add3("-debug-portal", [] { arg::shift(); debug_portal = arg::argi(); });
#endif
EX }

EX namespace walking {

EX bool on;

EX bool auto_eyelevel;

EX int floor_dir = -1;
EX cell *on_floor_of = nullptr;
EX ld eye_level = 0.2174492;
EX ld eye_angle = 0;
EX ld eye_angle_scale = 1;

EX int ticks_end, ticks_last;

EX set<color_t> colors_of_floors;

EX bool isFloor(cell *c) {
  if(!isWall(c)) return false;
  if(colors_of_floors.empty()) return true;
  if(c->wall != waWaxWall) return false;
  return colors_of_floors.count(c->landparam);
  }

EX void handle() {
  if(playermoved || !on) return;

  if(floor_dir == -1 || on_floor_of != centerover) {
    vector<int> choices;
    for(int i=0; i<centerover->type; i++)
      if(isFloor(centerover->cmove(i)))
        choices.push_back(i);

    if(sol && isize(choices) == 2) choices.pop_back();

    if(isize(choices) == 1) {
      on_floor_of = centerover;
      floor_dir = choices[0];
      }
    else if(colors_of_floors.empty() && sn::in()) {
      on_floor_of = centerover;
      auto z = inverse(View) * C0;
      switch(geometry) {
         case gSol:
           floor_dir = (z[2] > 0) ? 2 : 6;
           return;
         case gNIH:
           floor_dir = (z[2] > 0) ? 5 : 4;
           return;
         case gSolN:
           floor_dir = (z[2] > 0) ? 4 : 6;
           return;
         default: throw hr_exception("not solnihv");
         }
      }
    #if CAP_BT
    else if(colors_of_floors.empty() && hyperbolic && bt::in()) {
      auto z = bt::minkowski_to_bt(inverse(View) * C0);
      on_floor_of = centerover;
      floor_dir = z[2] > 0 ? bt::updir() : 0;
      println(hlog, "set floor_dir to ", floor_dir);
      }
    #endif
    else {
      println(hlog, "there are ", isize(choices), " choices for floor_dir");
      if(!on_floor_of) return;
      }
    }

  struct face {
    hyperpoint h0, hx, hy;
    };

  transmatrix ToOld = currentmap->relative_matrix(on_floor_of, centerover, C0);
  auto& csh = currentmap->get_cellshape(on_floor_of);
  face f;
  f.h0 = ToOld * csh.faces_local[floor_dir][0];
  f.hx = ToOld * csh.faces_local[floor_dir][1];
  f.hy = ToOld * csh.faces_local[floor_dir][2];

  auto find_nearest = [&] (const face& fac, hyperpoint at) {
    #if CAP_BT
    if(sol) { at[2] = fac.h0[2]; return at; }
    else if(hyperbolic && bt::in()) {
      auto z = bt::minkowski_to_bt(at);
      z[2] = bt::minkowski_to_bt(fac.h0)[2];
      return bt::bt_to_minkowski(z);
      }
    else if(mproduct && bt::in()) {
      auto dec = product_decompose(at);
      hyperpoint dep = PIU( deparabolic13(dec.second) );
      hyperpoint h = product_decompose(fac.h0).second;
      h = PIU( deparabolic13(h) );
      dep[0] = h[0];
      return orthogonal_move(PIU(parabolic13(dep)), dec.first);
      }
    #else
    if(false) {}
    #endif
    #if CAP_RAY
    else {
      transmatrix M = ray::mirrorize(currentmap->ray_iadj(on_floor_of, floor_dir));
      M = ToOld * M * inverse(ToOld);
      return mid(at, M * at);
      }
    #else
    else return at;
    #endif
    };

  hyperpoint at = tC0(inverse(View));
  if(invalid_point(at)) {
    println(hlog, "at is invalid!");
    on = false;
    return;
    }

  auto wallpt = find_nearest(f, at);

  ld view_eps = 1e-5;

  transmatrix spin_T;
  bool use_T = false;

  if(eye_angle) use_T = true, spin_T = cspin(1, 2, -eye_angle * degree);
  #if CAP_VR
  if(vrhr::active() && !vrhr::first && vrhr::hsm != vrhr::eHeadset::none) {
    use_T = true;
    spin_T = vrhr::hmd_ref_at;
    // print(hlog, "HMD seems to be at altitude ", spin_T[1][3], " depth ", spin_T[2][3], " zeros are ", spin_T[3][1], " and ", spin_T[3][2]);
    dynamicval<eGeometry> g(geometry, gCubeTiling);
    spin_T = vrhr::sm * inverse(spin_T);
    eye_level = -spin_T[1][3] / vrhr::absolute_unit_in_meters;
    if(eye_level < .001) eye_level = 0.001;
    vrhr::be_33(spin_T);
    }
  #endif

  if(use_T) rotate_view(spin_T);
  hyperpoint front = inverse(get_shift_view_of(ctangent(2, -view_eps), View)) * C0;
  hyperpoint up = inverse(get_shift_view_of(ctangent(1, +view_eps), View)) * C0;

  auto fwallpt = find_nearest(f, front);

  transmatrix T = nonisotropic ? nisot::translate(wallpt, -1) : gpushxto0(wallpt);
  hyperpoint dx = inverse_exp(shiftless(T * at));

  transmatrix Tf = nonisotropic ? nisot::translate(fwallpt, -1) : gpushxto0(fwallpt);
  hyperpoint dxf = inverse_exp(shiftless(Tf * front));

  if(eye_level == -1) eye_level = hypot_d(3, dx);

  auto smooth = [&] (hyperpoint h1, hyperpoint h2) {
    if(ticks < ticks_end) {
      ld last_t = ilerp(ticks_end-1000, ticks_end, ticks_last);
      ld curr_t = ilerp(ticks_end-1000, ticks_end, ticks);
      last_t = last_t * last_t * (3-2*last_t);
      curr_t = curr_t * curr_t * (3-2*curr_t);
      ld t = ilerp(last_t, 1, curr_t);
      return lerp(h1, h2, t);
      }
    return h2;
    };

  auto oView = View;
  set_view(
    smooth(at, inverse(T) * direct_exp(dx / hypot_d(3, dx) * eye_level)),
    smooth(front, inverse(Tf) * direct_exp(dxf / hypot_d(3, dxf) * eye_level)),
    smooth(up, inverse(T) * direct_exp(dx / hypot_d(3, dx) * (eye_level + view_eps)))
    );
  if(use_T) rotate_view(inverse(spin_T));

  auto nat = tC0(inverse(View));
  if(invalid_point(nat)) {
    println(hlog, "at is invalid after fixing!");
    View = oView;
    return;
    }

  ticks_last = ticks;
  }

EX void switch_walking() {
  on = !on;
  if(on && auto_eyelevel) eye_level = -1;
  floor_dir = -1;
  on_floor_of = nullptr;
  ticks_last = ticks;
  ticks_end = ticks + 1000;
  }

EX void add_options() {
  dialog::addBoolItem("walking mode", on, 'w');
  dialog::add_action(switch_walking);
  add_edit(eye_level);
  add_edit(eye_angle);
  if(point_direction >= 0 && point_direction < centerover->type) {
    cell *c = centerover->move(point_direction);
    if(c && c->wall == waWaxWall) {
      color_t col = c->landparam;
      dialog::addBoolItem("we are facing floor (color " + format("%06X", col) + ")", colors_of_floors.count(col), 'n');
      dialog::add_action([col] {
        if(colors_of_floors.count(col)) colors_of_floors.erase(col);
        else colors_of_floors.insert(col);
        });
      }
    }
  }

auto a = addHook(hooks_configfile, 100, [] {
  param_b(auto_eyelevel, "auto_eyelevel")
      -> editable("keep eye level when walking enabled", 'L');
  param_f(eye_level, "eye_level")
      -> editable(0, 5, .1, "walking eye level",
      "Distance from the floor to the eye in the walking mode, in absolute units. In VR this is adjusted automatically.",
      'e')
      ->set_extra([] { add_edit(auto_eyelevel); });
  param_f(eye_angle, "eye_angle")
      -> editable(-90, 90, 15, "walking eye angle",
      "0 = looking forward, 90 = looking upward. In VR this is adjusted automatically.",
      'k')
      ->set_extra([] { add_edit(eye_angle_scale); });
  param_f(eye_angle_scale, "eye_angle_scale")
      -> editable(-2, 0, 2, "eye angle scale",
      "1 = the angle can be changed with keyboard or mouse movements, 0 = the angle is fixed",
      'k');
  })
 + addHook(hooks_clearmemory, 40, [] { on_floor_of = nullptr; floor_dir = -1; })
 + arg::add3("-walk-on", [] {
    on = true;
    if(auto_eyelevel) eye_level = -1;
    floor_dir = -1;
    on_floor_of = nullptr;
    ticks_last = ticks_end = ticks;
    });

EX }
}
