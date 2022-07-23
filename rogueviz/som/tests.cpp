// define all our manifolds, and perform tests on them
// Copyright (C) 2011-2022 Tehora and Zeno Rogue, see 'hyper.cpp' for details

#include "kohonen.h"
#include <unordered_map>

namespace rogueviz { 

transmatrix& memo_relative_matrix(cell *c1, cell *c2);

namespace kohonen_test {

using namespace kohonen;

void equal_weights() {
  alloc(weights);
  for(auto& w: weights) w = 1;
  }

void show_all() {
  samples_to_show.clear();
  for(int i=0; i<samples; i++)
    samples_to_show.push_back(i);
  }

bool check(bool deb);

vector<pair<int, int> > voronoi_edges;

bool nmap;

vector<cell*> where;
int max_distance = 999;

bool using_subdata;
vector<sample> orig_data;
vector<int> sub_indices;
vector<int> inv_sub_indices;

void ideal() {
  for(int i=0; i<isize(net); i++)
    for(auto& v: net[i].net) v = 1000;    
  
  for(int i=0; i<isize(net); i++)
  for(int j=0; j<isize(where); j++) 
    if(where[j] == net[i].where)
      net[i].net = (using_subdata ? orig_data : data)[j].val;
    
  println(hlog, make_pair(isize(net), isize(where)));
  }

bool sphere_data = false;

bool edge_data;

vector<bool> is_special;
vector<int> ctrdist;
int ctrdist_max;

voronoi::manifold data_manifold;

using measures::manidata;
manidata test_emb, test_orig;

void create_manidata(manidata& mdata) {
  auto ac = gen_neuron_cells();
  auto& edges = mdata.edges;
  edges.clear();
  mdata.size = isize(ac);
  mdata.distances.clear();
  for(int i=0; i<mdata.size; i++) {
    for(int j=0; j<i; j++)
      if(isNeighbor(ac[i], ac[j]))
        edges.emplace_back(i, j);
    }
  }

void create_data() {
  if(sphere_data) return;
  sphere_data = true;

  initialize_rv();
    
  cell *c0 = currentmap->gamestart();
  where.clear();
  is_special.clear();
  ctrdist.clear();
  ctrdist_max = 0;
  edge_data = false;
  
  drawthemap();
  data.clear(); samples_to_show.clear();
  auto ac = gen_neuron_cells();

  for(auto c: ac) {
    if(celldistance(c, c0) > max_distance) continue;
    where.push_back(c);
    sample s;
    embeddings::get_coordinates(s.val, c, c0);
    data.push_back(std::move(s));
    }
  samples = isize(data);
  test_orig.size = samples;
  colnames.resize(columns);
  for(int i=0; i<columns; i++) colnames[i] = "Coordinate " + its(i);
  equal_weights();
  show_all();
  data_manifold = voronoi::build_manifold(ac);

  for(int i=0; i<samples; i++) {
    bool sign = false;
    using embeddings::signposts;
    for(cell *c: signposts) if(c == where[i]) sign = true;
    if(signposts.empty()) sign = where[i]->type != (S3 == 3 ? 6 : 4);
    is_special.push_back(sign);
    ctrdist.push_back(celldist(where[i]));
    ctrdist_max = max(ctrdist_max, ctrdist.back());
    }
  
  create_manidata(test_orig);
  }

void create_subdata(int qty) {
  if(!using_subdata)
    orig_data = data;
  using_subdata = true;
  int N = isize(orig_data);
  sub_indices.resize(N);
  for(int i=0; i<N; i++) sub_indices[i] = i;
  hrandom_shuffle(sub_indices);
  sub_indices.resize(qty);
  data.clear();

  for(int i=0; i<isize(vdata); i++)
    if(vdata[i].m) vdata[i].m->dead = true;  
  vdata.clear();
  sample_vdata_id.clear();
  state &= ~KS_SAMPLES;

  for(int idx: sub_indices) data.push_back(orig_data[idx]);
  samples = isize(data);

  inv_sub_indices.clear();
  inv_sub_indices.resize(N, -1);
  for(int i=0; i<samples; i++) inv_sub_indices[sub_indices[i]] = i;
  
  show_all();
  edge_data = false;
  }

bool colorless = false;

void create_edgedata() {
  if(edge_data) return;
  edge_data = true;
  create_data();

  if(!colorless) for(int i=0; i<samples; i++) {
    if(is_special[i])    
      vdata[i].cp.color1 = gradient(0xC0C000FF, 0xC00000FF, 0, ctrdist[i], ctrdist_max);
    else
      vdata[i].cp.color1 = gradient(0x0000FFFF, 0x101010FF, 0, ctrdist[i], ctrdist_max);
    }
    
  auto& edges = test_orig.edges;

  for(int i=0; i<samples; i++) {
    vector<int> ids;
    for(auto e: edges) {
      if(e.first == i) ids.push_back(e.second);
      if(e.second == i) ids.push_back(e.first);
      }
    vdata[i].name = lalign(0, "#", i, " ", ids);
    }

  auto any = add_edgetype("adjacent");
  
  if(!using_subdata) {
    for(auto e: edges) 
      addedge(e.first, e.second, 1, false, any);
    }
  else {
    vector<int> nearest(isize(orig_data), -1);
    set<pair<int, int>> subedges;
    for(int i=0; i<samples; i++)
      nearest[sub_indices[i]] = i;
    while(true) {
      vector<pair<int, int> > changes;
      for(auto e: edges) {
        if(nearest[e.first] == -1 && nearest[e.second] >= 0) changes.emplace_back(e.first, nearest[e.second]);
        if(nearest[e.second] == -1 && nearest[e.first] >= 0) changes.emplace_back(e.second, nearest[e.first]);
        }
      if(changes.empty()) break;
      // hrandom_shuffle(changes);
      for(auto ch: changes) nearest[ch.first] = ch.second;
      }
    for(auto e: edges) 
      if(nearest[e.first] != nearest[e.second])
        subedges.emplace(nearest[e.first], nearest[e.second]);
    // for(auto se: subedges) println(hlog, "subedges = ", se);
    for(auto sube: subedges)
      addedge(sube.first, sube.second, 1, false, any);
    }
  
  println(hlog, "edgedata created, ", using_subdata);
  }

void sphere_test() {
  
  create_data();

  initialize_dispersion();
  initialize_neurons_initial();

  analyze();
  
  create_edgedata();

  ideal();
  analyze();
  }

void sphere_test_no_disp() {
  
  create_data();

  initialize_neurons_initial();
  analyze();  
  create_edgedata();
  ideal();
  analyze();
  }

void check_energy() {
  vector<int> dlist;

  vector<cell*> win_cells(samples);
  for(int i=0; i<samples; i++)
    win_cells[i] = winner(i).where;

  vector<int> distlist;
  for(int i=0; i<samples; i++)
    for(int j=0; j<i; j++)
      distlist.push_back(celldistance(where[i], where[j]));
  
  for(int i=0; i<samples; i++) {
    shiftmatrix M = ggmatrix(where[i]);
    println(hlog, i, ": ", M * C0);
    }
  // println(hlog, distlist);

  for(auto e: test_orig.edges) {
    cell *w1 = win_cells[e.first];
    cell *w2 = win_cells[e.second];
    int d = celldistance(w1, w2);
    dlist.push_back(d);
    }
  println(hlog, dlist);
  }

void evaluate() {
  create_manidata(test_emb);
  test_orig.distances = measures::build_distance_matrix(test_orig.size, test_orig.edges);
  test_emb.distances = measures::build_distance_matrix(test_emb.size, test_emb.edges);
  vector<int> mapp(test_orig.size, 0);
  map<cell*, int> id;
  for(int i=0; i<isize(net); i++) id[net[i].where] = i;
  for(int i=0; i<samples; i++) mapp[i] = id[winner(i).where];
  vector<pair<int, int>> edo_recreated = measures::recreate_topology(mapp, test_orig.edges);
  for(int k=0; k<measures::MCOUNT; k++) {
    print(hlog, measures::catnames[k], " = ", measures::evaluate_measure(test_emb, test_orig, mapp, voronoi_edges, edo_recreated, k), " ");
    }
  println(hlog);
  }

bool kst_key(int sym, int uni) {
  if((cmode & sm::NORMAL) && uni == 'l') {
    set_neuron_initial();
    t = tmax;
    analyze();
    return true;
    }
  else if((cmode & sm::NORMAL) && uni == 'd') {
    for(int i=0; i<samples; i++)
      println(hlog, i, ": ", data[i].val);
    return true;
    }
  else if((cmode & sm::NORMAL) && uni == 'v') {
    voronoi_edges = voronoi::compute_voronoi_edges(data_manifold);
    println(hlog, "voronoi edges computed");
    evaluate();
    return true;
    }
  else if((cmode & sm::NORMAL) && uni == 'r') {
    set_neuron_initial();
    t = tmax;
    dynamicval ks(qpct, 0);
    while(!finished()) kohonen::step();
    println(hlog, "check result = ", check(true));
    check_energy();
    return true;
    }
  if((cmode & sm::NORMAL) && uni == 't') {
    tmax /= 2;
    println(hlog, "tmax = ", tmax);
    return true;
    }
  if((cmode & sm::NORMAL) && uni == 'x') {
    int t = clock();
    println(hlog, "weights = ", weights, " w0 = ", weights[0]);
    check(true);
    println(hlog, "time = ", int(clock() - t), " power = ", kohonen::ttpower, " dea = ", kohonen::dispersion_end_at, " dm = ", kohonen::distmul);
    return true;
    }
  return false;
  }

int ks_empty, ks_nonadj, ks_distant;

int qenergy = 0;
double tot_energy = 0;

bool check(bool deb) {
  dynamicval dd(debugflags, 0);
  set_neuron_initial();
  t = tmax;
  dynamicval dp(qpct, 0);
  while(!finished()) kohonen::step();
  analyze();
  int empty = 0, nonadj = 0, distant = 0;
  for(int i=0; i<cells; i++)
    if(net[i].csample == 0)
      empty++;

  vector<cell*> win_cells(samples);
  for(int i=0; i<samples; i++)
    win_cells[i] = winner(i).where;
  
  int energy = 0;
    
  for(auto e: test_orig.edges) {
    cell *w1 = win_cells[e.first];
    cell *w2 = win_cells[e.second];
    int rho = celldistance(w1, w2);
    energy += rho * rho;
    if(!isNeighbor(w1, w2)) {
      nonadj++;
      bool dist2 = false;
      forCellEx(w3, w1) 
        if(isNeighbor(w3, w2)) dist2 = true;
      if(!dist2) distant++;
      }
    }
  
  tot_energy += energy;
  qenergy++;

  if(deb) println(hlog, "empty = ", empty, " nonadj = ", nonadj, " distant = ", distant, " energy = ", energy, " avg ", tot_energy / qenergy);
  bool res = empty <= ks_empty && distant <= ks_distant && nonadj <= ks_nonadj;
  return res;
  }

vector<ld> get_parameters() {
  return vector<ld> { ttpower, learning_factor, gaussian ? distmul :  dispersion_end_at-1 };
  }

void set_parameters(const vector<ld>& v) {
  ttpower = v[0];
  learning_factor = v[1];
  if(gaussian) distmul = v[2];
  else dispersion_end_at = v[2] + 1;
  }

void som_table() {
  sphere_test();
  
  map<array<int, 3>, pair<int, int> > tries;

  map<array<int, 3>, string> sucorder;
  
  auto bttpower = ttpower;
  auto blearning = learning_factor;
  auto bdist = distmul;
  auto bdispe = dispersion_end_at - 1;
  
  ld last_distmul = -1;
  
  auto set_parameters = [&] (array<int, 3>& u) {
    distmul = bdist * exp(u[0] / 5.);
    dispersion_end_at = 1 + bdispe * exp(u[0] / 5.);
    ttpower = bttpower * exp(u[2] / 5.);
    learning_factor = blearning * exp(u[1] / 5.);

    if(last_distmul != distmul) {
      last_distmul = distmul, state &=~ KS_DISPERSION;
      }
    };
  
  array<int, 3> best = {0, 0, 0};
  
  int maxtry = 20;
  
  while(true) {
    
    array<int, 3> best_at;
    ld bestval = -1;
    
    vector<ld> vals;
    
    for(int k=0; k<27; k++) {
      array<int, 3> cnt;
      int k1 = k;
      for(int i=0; i<3; i++) cnt[2-i] = best[2-i] + (k1%3-1), k1 /= 3;
      set_parameters(cnt);
      do {
        tries[cnt].second++;
        dynamicval dd(debugflags, 0); 
        bool chk = check(false);
        if(chk)
          tries[cnt].first++;
        sucorder[cnt] += (chk ? 'y' : 'n');
        }
      while(tries[cnt].second < maxtry);
      ld val_here = tries[cnt].first * 1. / tries[cnt].second;
      if(val_here > bestval) bestval = val_here, best_at = cnt;
      vals.push_back(val_here);
      }
    
    sort(vals.begin(), vals.end());
    
    best = best_at;
    set_parameters(best);
    println(hlog, "score ", bestval, " at ", best_at, " : ", tie(distmul, dispersion_end_at, learning_factor, ttpower), " x", tries[best].second, " s=", vals[vals.size()-2]);
    if(tries[best].second > maxtry)
      maxtry = tries[best].second;
      
    if(tries[best].second >= 1000) {
      println(hlog, "suc ", best, " :\n", sucorder[best]);

      for(int vv=10; vv>=0; vv--) {
        dynamicval ks(qpct, 0);
        t = vv ? tmax * vv / 10 : 1;
        step();
        println(hlog, "t=", t);
        println(hlog, "dispersion_count = ", dispersion_count);
        }
  
      return;
      }
    }
  }

vector<string> shapelist;
map<string, reaction_t> shapes;
map<string, reaction_t> embeddings;

int data_scale = 1;
int embed_scale = 1;
int current_scale = 1;

void set_gp(int a, int b) {
  a *= current_scale; b *= current_scale;
  if(a == 1 && b == 1) 
    set_variation(eVariation::bitruncated);
  else if(a == 1 && b ==  0)
    set_variation(eVariation::pure);
  else {
    set_variation(eVariation::goldberg);
    gp::param = gp::loc(a, b);
    }
  }

void set_restrict() {
  kohonen::kqty = 5000;
  kohonen::kohrestrict = 520 * current_scale * current_scale;
  }

void set_torus2(int a, int b, int c, int d, int e) {
  using namespace euc;
  auto& T0 = eu_input.user_axes;
  T0[0][0] = a;
  T0[0][1] = b;
  T0[1][0] = c;
  T0[1][1] = d;
  eu_input.twisted = e;
  build_torus3();
  }

int dim = 10;

string emb;

void add(string name, reaction_t embed, reaction_t set) {
  shapelist.push_back(name);
  shapes[name] = set;
  embeddings[name] = embed;
  }

void set_euclid3(int x, int y, int z, int twist) {
  using namespace euc;
  auto& T0 = eu_input.user_axes;
  for(int i=0; i<3; i++)
  for(int j=0; j<3; j++) T0[i][j] = 0;
  
  T0[0][0] = x;
  T0[1][1] = y;
  T0[2][2] = z;

  eu_input.twisted = twist;
  build_torus3();
  }

void klein_signposts() {
  embeddings::etype = embeddings::eSignpost;
  println(hlog, "marking klein signposts");
  embeddings::signposts.clear();
  
  for(cell *c: currentmap->allcells()) setdist(c, 7, nullptr);
  
  for(int x=0; x<4; x++)
  for(int y=0; y<13; y++) {
    cellwalker cw = cellwalker(currentmap->gamestart(), 0);
    for(int i=0; i<x*5*current_scale; i++) {
      cw += wstep;
      cw += 3;
      }
    cw+=2;
    for(int j=0; j<y*current_scale; j++)  {
      cw += wstep;
      cw += 2;
      cw += wstep;
      cw -= 2;
      }
    embeddings::signposts.push_back(cw.at);
    }
  println(hlog, embeddings::signposts);
  }

flagtype flags;

const static flagtype m_symmetric = Flag(0);

int landscape_dim = 60;

void init_shapes() {
  
  auto signpost = [] { emb = "signpost"; embeddings::mark_signposts(false, gen_neuron_cells()); };
  auto signpost_subg = [] (int a, int b) { return [a,b] { emb = "signpost" + its(a) + its(b); embeddings::mark_signposts_subg(a, b, gen_neuron_cells()); }; };
  // auto signpost_full = [] { emb = "signpost-full"; embeddings::mark_signposts(true, gen_neuron_cells()); };
  auto signpost_klein = [] { emb = "signpost-klein"; klein_signposts(); };
  auto landscape = [] {
    emb = "landscape"; 
    if(landscape_dim)
      embeddings::init_landscape(landscape_dim);
    else
      embeddings::init_landscape_det(gen_neuron_cells());
    };
  auto natural = [] { emb = "natural"; embeddings::etype = embeddings::eNatural; };
  
  /* disks */

  add("disk10", landscape, [] {
    set_geometry(gNormal);
    set_gp(1,0);
    set_restrict();
    flags = 0;
    });

  add("disk11", landscape, [] {
    set_geometry(gNormal);
    set_gp(1,1);
    set_restrict();
    flags = 0;
    });

  add("disk20", landscape, [] {
    set_geometry(gNormal);
    set_gp(2,0);
    set_restrict();
    flags = 0;
    });

  add("disk21", landscape, [] {
    set_geometry(gNormal);
    set_gp(2,1);
    set_restrict();
    flags = 0;
    });

  add("disk40", landscape, [] {
    set_geometry(gNormal);
    set_gp(4,0);
    set_restrict();
    flags = 0;
    });

  add("disk43", landscape, [] {
    set_geometry(gNormal);
    set_gp(4,3);
    set_restrict();
    flags = 0;
    });

  add("disk-euclid", landscape, [] {
    set_geometry(gEuclid);
    set_gp(1,0);
    set_restrict();
    set_torus2(0,0,0,0,0);
    flags = 0;
    });

  /* spheres */
  
  add("elliptic", signpost, [] {
    set_geometry(gElliptic);
    set_gp(6,6);
    flags = m_symmetric;
    });
  
  add("sphere", natural, [] {
    set_geometry(gSphere);
    set_gp(6,2);
    flags = m_symmetric;
    });

  add("sphere4", natural, [] {
    set_geometry(gSmallSphere);
    set_gp(7,6);
    flags = m_symmetric;
    });

  /* tori */

  add("torus-hex", natural, [] {
    set_geometry(gEuclid);
    set_gp(1,0);
    dim = 23;
    set_torus2(dim, 0, 0, dim, 0);
    flags = m_symmetric;
    });

  add("torus-sq", natural, [] {
    set_geometry(gEuclid);
    set_gp(1,0);
    dim = 13;
    set_torus2(dim*1.6,0,-dim,dim*2,0);
    });

  add("torus-rec", natural, [] {
    set_geometry(gEuclid);
    set_gp(1,0);
    dim = 9;
    set_torus2(dim*3+2,0,-dim,dim*2,0);
    flags = 0;
    });

  if(0) add("torus-sq-sq", natural, [] {
    set_geometry(gEuclidSquare);
    set_gp(1,0);
    set_torus2(23,0,0,23,0);
    flags = m_symmetric;
    });

  add("klein-sq", signpost_klein, [] {
    set_geometry(gEuclid);
    set_gp(1,0);
    dim = 13;
    set_torus2(dim*1.6,0,-dim,dim*2,8);
    flags = 0;
    });

  /* hyperbolic 8 */

  add("Bolza", signpost_subg(1, 1), [] {
    set_geometry(gBolza);
    set_gp(6,3);
    flags = m_symmetric;
    });

  add("Bolza2", signpost, [] {
    set_geometry(gBolza2);
    set_gp(5,1);
    flags = m_symmetric;
    });
  
  /* hyperbolic 7 */

  add("minimal", signpost, [] {
    set_geometry(gMinimal);
    set_gp(5,5);
    flags = 0;
    });
  
  add("Zebra", signpost, [] {
    set_geometry(gZebraQuotient);
    set_gp(4,3);
    flags = 0;
    });

  add("KQ", signpost, [] {
    set_geometry(gKleinQuartic);
    set_gp(3,2);
    flags = m_symmetric;
    });

  add("Macbeath", signpost, [] {
    set_geometry(gMacbeath);
    set_gp(2,1);
    flags = m_symmetric;
    });
  
  add("triplet1", signpost, [] {
    field_quotient_2d(0, 1, 0);
    set_gp(1, 1);
    flags = m_symmetric;
    });

  add("triplet2", signpost, [] {
    field_quotient_2d(0, 1, 1);
    set_gp(1, 1);
    flags = m_symmetric;
    });

  add("triplet3", signpost, [] {
    field_quotient_2d(0, 1, 2);
    set_gp(1, 1);
    flags = m_symmetric;
    });
  
  }

void shot_settings() {
  View = Id;

  brm_limit = GDIM == 2 ? 1000 : 0;
  
  if(GDIM == 3)
    View = cspin(0, 2, 30 * degree) * cspin(1, 2, 30*degree) * View;
  
  shift_view(ctangent(2, -0.5));

  vid.creature_scale = GDIM == 2 ? 4.5 : 1;
  if(geometry == gCell600) vid.creature_scale = 0.5;
  pconf.alpha = 1;
  pmodel = mdDisk;
  
  if(GDIM == 3 && hyperbolic) {
    sightranges[geometry] = 8;
    vid.creature_scale = 2;
    }

  shot::shotformat = -1;
  shot::shotx = 1000;
  shot::shoty = 1000;
  shot::transparent = GDIM == 2;
  pconf.scale = 0.9;
  
  modelcolor = 0xFF;
  
  drawthemap();

  if(sphere) pconf.scale = 0.6;
  
  if(euclid) {
    ld maxs = 0;
    auto& cd = current_display;
    for(auto n: net) {
      auto w = n.where;
      shiftmatrix g = ggmatrix(w);
      for(int i=0; i<w->type; i++) {
        shiftpoint h = tC0(g * currentmap->adj(w, i));
        hyperpoint onscreen;
        applymodel(h, onscreen);
        maxs = max(maxs, onscreen[0] / cd->xsize);
        maxs = max(maxs, onscreen[1] / cd->ysize);
        }
      }
    pconf.alpha = 1;
    pconf.scale = pconf.scale / 2 / maxs / cd->radius;
    pconf.scale /= 1.2;
    if(closed_manifold) pconf.scale = WDIM == 3 ? 0.2 : 0.07;
    }
  
  if(GDIM == 3) pmodel = mdPerspective;
  if(nil || sol) pmodel = mdGeodesic;

  vid.use_smart_range = 2;
  vid.smart_range_detail = 7;
  vid.cells_generated_limit = 999999;
  vid.cells_drawn_limit = 200000;  
  }

void shot_settings_png() {
  vid.use_smart_range = 2;
  vid.smart_range_detail = 0.5;

  shot::shotx = 500;
  shot::shoty = 500;
  }

bool more = true;

void create_index() {

  hr::ignore(system(("mkdir " + som_test_dir).c_str()));

  fhstream f(som_test_dir + "index-" + its(current_scale) + ".html", "wt");

  fhstream csv(som_test_dir + "manifold-data-" + its(current_scale) + ".csv", "wt");

  fhstream tex(som_test_dir + "manifold-data-" + its(current_scale) + ".tex", "wt");
  println(f, "<html><body>");

  // fhstream distf(som_test_dir + "distlists-" + its(current_scale) + ".txt", "wt");
  
  bool add_header = true;
  
  for(auto s: shapelist) {

    sphere_data = false;
    println(hlog, "building ", s);
    kohonen::kqty = kohonen::krad = 0;
    kohonen::kohrestrict = 999999999;
    stop_game();
    shapes[s]();
    // if(!euclid) continue;
    start_game();
    
    initialize_rv();
    embeddings[s]();

    println(hlog, "create_data");
    create_data();
    
    println(hlog, "sphere_test");
    sphere_test_no_disp();

    println(hlog, "building disttable");
    
    vector<int> disttable(100, 0);
    int pairs = 0;

    test_orig.distances = measures::build_distance_matrix(test_orig.size, test_orig.edges);
    int N = test_orig.size;
    for(int i=0; i<N; i++)
    for(int j=0; j<i; j++) {
      disttable[test_orig.distances[i][j]]++;
      pairs++;
      }

    println(hlog, "render");
    shot_settings();
    shot_settings_png();
    shot::take(som_test_dir + s + "-" + its(current_scale) + ".png");
    
    println(f, "<img src=\"" + s + "-" + its(current_scale) + ".png\"/><br/>");
    
    println(f, "shape ", s, " : ", samples, " items, ", isize(test_orig.edges), " edges, dim ", columns, " (", emb, "), ", full_geometry_name());
    
    println(f, "<hr/>");
    fflush(f.f);
    
    again:
    if(add_header) print(csv, "name"); else print(csv, s);
    if(add_header) print(tex, "name"); else print(tex, s);
    #define Out(title,value) if(add_header) { print(csv, ";", title); print(tex, "&", title); } else { print(csv, ";", value); print(tex, "&", value); }
    
    double avgdist = 0, avgdist2 = 0, sqsum = 0;
    int maxdist = 0;
    for(int i=0; i<100; i++) {
      if(disttable[i] > 0) maxdist = i;
      avgdist += i * disttable[i];
      avgdist2 += i * i * disttable[i];
      sqsum += disttable[i] * (disttable[i]-1.);
      }
    disttable.resize(maxdist + 1);
    if(more) println(hlog, disttable, " pairs = ", pairs);

    avgdist /= pairs;
    avgdist2 /= pairs;
    double kmax = 1 - sqsum / (pairs * (pairs-1.));
      
    Out("samples", samples);
    Out("edges", isize(test_orig.edges));
    Out("columns", columns);
    Out("embtype", emb);
    Out("gpx", gp::univ_param().first);
    Out("gpy", gp::univ_param().second);
    Out("orientable", nonorientable ? 0 : 1);
    Out("symmetric", (flags & m_symmetric) ? 1 : 0);
    Out("closed", closed_manifold ? 1 : 0);
    Out("quotient", quotient ? 1 : 0);
    Out("dim", WDIM);
    Out("valence", S3);
    Out("tile", S7);

    println(hlog, "gen neuron cells");
    auto ac = gen_neuron_cells();
    int sum = 0;
    for(cell *c: ac) sum += c->type;
    ld curvature = (S3 == 3 ? 6 : S3 >= OINF ? 2 : 4) - sum * 1. / isize(ac);
    if(GDIM == 3) curvature = hyperbolic ? -1 : sphere ? 1 : 0;
    Out("curvature", curvature);
    
    println(hlog, "compute geometry data");
    auto gd = compute_geometry_data();
    Out("euler", gd.euler);
    Out("area", gd.area);
    
    Out("geometry",
      hyperbolic ? "hyperbolic" :
      euclid ? "euclidean" :
      sphere ? "spherical" :
      "other");

    if(more) {
      Out("maxdist", maxdist);
      Out("avgdist", avgdist);
      Out("avgdist2", avgdist2);
      Out("kmax", kmax);
      }
    
    println(csv); println(tex, "\\\\");
    fflush(csv.f); fflush(f.f); fflush(tex.f);
    
    if(add_header) { add_header = false; goto again; }
    
    println(hlog, "geom = ", s, " delta = ", isize(embeddings::delta_at));
    }

  println(f, "</body></html>");
  }

unsigned hash(string s) {
  unsigned res = 0;
  for(char c: s) res = res * 37 + c;
  return res;
  }

int subdata_value;

bool only_landscape;

string cg() {
  string s = "";
  if(kohonen::gaussian == 1) s += "-cg";
  if(kohonen::gaussian == 2) s += "-gg";
  if(kohonen::dispersion_long) s += "-dl";
  if(ttpower != 1) s += "-tt" + lalign(0, ttpower);
  if(subdata_value) s += "-s" + its(subdata_value);
  if(landscape_dim) s += "-l" + its(landscape_dim);
  if(data_scale) s += "-d" + its(data_scale);
  if(embed_scale) s += "-e" + its(embed_scale);  
  return s;
  }

vector<vector<sample> > saved_data;

void all_pairs(bool one) {

  string dir = som_test_dir + "pairs" + cg();

  hr::ignore(system(("mkdir -p " + dir + "/img").c_str()));

  int sid = 0;
  for(auto s1: shapelist) {
    for(auto s2: shapelist) {
      sid++;
      
      if(kohonen::gaussian == 2 && s2.substr(0, 4) != "disk" && s2.substr(0, 6) != "sphere") continue;
      if(only_landscape && s1.substr(0, 4) != "disk") continue;

      string fname_vor = dir + "/" + s1 + "-" + s2 + ".vor";
      string fname = dir + "/" + s1 + "-" + s2 + ".txt";
      if(file_exists(fname)) continue;
      
      fhstream f(fname, "wt");
      fhstream fvor(fname_vor, "wt");
      
      println(hlog, "mapping ", s1, " to " ,s2);
      
      shrand(hash(s1) ^ hash(s2));

      sphere_data = false; using_subdata = false;
      kohonen::kqty = kohonen::krad = 0;
      kohonen::kohrestrict = 999999999;
      current_scale = data_scale;
      stop_game();
      shapes[s1]();
      start_game();

      initialize_rv();
      
      if(landscape_dim) {
        saved_data.clear();
        for(int i=0; i<100; i++) {
          sphere_data = false;
          data.clear();
          embeddings[s1]();
          create_data();
          saved_data.push_back(data);
          if(i < 5)
          for(int j=0; j<20; j++)
            println(hlog, "saved data ", i, ":", j, ": ", saved_data[i][j].val);
          }
        }
      else {
        embeddings[s1]();
        create_data();
        }
      
      stop_game();

      kohonen::kqty = kohonen::krad = 0;
      kohonen::kohrestrict = 999999999;

      current_scale = embed_scale;
      shapes[s2]();

      initialize_dispersion();
      initialize_neurons_initial();
    
      analyze();
      
      create_edgedata();
      
      int orig_samples = samples;
      
      for(int i=0; i<100; i++) {
        println(hlog, "iteration ", lalign(3, i), " of ", fname);
        
        if(landscape_dim) { data = orig_data = saved_data[i]; }
        
        if(subdata_value) create_subdata(subdata_value);

        set_neuron_initial();
        
        if(0) {
          println(hlog, "initial neuron values:"); indenter ind(2);
          for(auto& n: net) println(hlog, n.net);
          }

        t = tmax;
        dynamicval ks(qpct, 0);
        while(!finished()) kohonen::step();

        for(int i=0; i<orig_samples; i++) {
          if(i) print(f, " ");
          int j = i;
          if(using_subdata) {
            j = inv_sub_indices[i];
            if(j == -1) { print(f, "-1"); continue; }
            }
          auto& w = winner(j);
          print(f, int((&w) - &net[0]));
          }
        
        print(f, "\n");
        fflush(f.f);
        
        voronoi::debug_str = lalign(0, fname_vor, " iteration ", i);
        
        auto ve = voronoi::compute_voronoi_edges(data_manifold);        
        println(fvor, isize(ve));
        for(auto e: ve) print(fvor, e.first, " ", e.second, " ");
        println(fvor);
        fflush(fvor.f);
        
        if(i < 10) {
          analyze();
          if(i == 0) {
            shot_settings();
            shot_settings_png();
            shot::shotx = 200;
            shot::shoty = 200;
            }
          if(using_subdata) {
            create_edgedata();
            }
          shot::take(dir + "/img/" + s1 + "-" + s2 + "-" + its(i) + ".png");
          }
        if(i == 10) brm_structure.clear();
        fflush(stdout);
        }
      
      if(1) {
        fhstream rndcheck(som_test_dir + "rndcheck" + cg(), "at");
        vector<int> rnd;
        for(int i=0; i<10; i++) rnd.push_back(hrand(1000));
        println(hlog, "finished ", s1, "-", s2, " rnd = ", rnd);
        println(rndcheck, "finished ", s1, "-", s2, " rnd = ", rnd);
        }
      
      if(one) exit(0);
      }
    }

  hr::ignore(system("touch done"));
  }

bool verify_distlists = false;

void create_edgelists() {
  fhstream f("results/edgelists-" + its(current_scale) + ".txt", "wt");
  for(auto s: shapelist) {
    
    sphere_data = false;
    kohonen::kqty = kohonen::krad = 0;
    kohonen::kohrestrict = 999999999;
    stop_game();
    shapes[s]();
    // if(!euclid) continue;
    start_game();
    
    initialize_rv();
    embeddings[s]();

    println(hlog, "create_data");
    create_data();
    
    println(hlog, "sphere_test");
    sphere_test_no_disp();

    auto ac = gen_neuron_cells();
    
    int N = isize(ac);
    int M = isize(test_orig.edges);
    print(f, s, "\n", N, " ", M);
    for(auto e: test_orig.edges) print(f, " ", e.first, " ", e.second);
    
    if(verify_distlists) {
      test_orig.distances = measures::build_distance_matrix(test_orig.size, test_orig.edges);
      for(int i=0; i<N; i++)
      for(int j=0; j<i; j++)
        if(celldistance(ac[i], ac[j]) != test_orig.distances[i][j])
          println(hlog, "failure on ", tie(i, j));
      }

    println(f);
    fflush(f.f);
    }
  }

#if CAP_COMMANDLINE
int readArgs() {
  using namespace arg;
  
  if(0) ;
  
  /* choose the embedding */
  
  else if(argis("-som-landscape")) {
    PHASE(3);
    start_game();
    initialize_rv();
    shift(); embeddings::init_landscape(argi());
    println(hlog, "landscape init, ", argi());
    }

  else if(argis("-som-landscape-det")) {
    PHASE(3);
    start_game();
    initialize_rv();
    embeddings::init_landscape_det(gen_neuron_cells());
    println(hlog, "landscape init, ", argi());
    }

  else if(argis("-som-signposts")) {
    PHASE(3);
    start_game();
    initialize_rv();
    embeddings::mark_signposts(false, gen_neuron_cells());
    }

  else if(argis("-som-signposts-full")) {
    PHASE(3);
    start_game();
    initialize_rv();
    embeddings::mark_signposts(true, gen_neuron_cells());
    }

  else if(argis("-som-signposts-klein")) {
    PHASE(3);
    start_game();
    klein_signposts();
    }

  else if(argis("-som-signposts-subg")) {
    PHASE(3);
    shift(); int a = argi();
    shift(); int b = argi();
    start_game();
    initialize_rv();
    embeddings::mark_signposts_subg(a, b, gen_neuron_cells());
    }

  else if(argis("-som-signposts-draw")) {
    for(cell *c: embeddings::signposts) c->wall = waPlatform;
    }

  else if(argis("-som-rug")) {
    PHASE(3);
    start_game();
    initialize_rv();
    shift(); embeddings::generate_rug(argi(), true);
    }

  else if(argis("-som-rug-show")) {
    PHASE(3);
    start_game();
    initialize_rv();
    shift(); embeddings::generate_rug(argi(), false);
    }

  else if(argis("-som-proj")) {
    PHASE(3); embeddings::etype = embeddings::eProjection;
    }

  else if(argis("-som-emb")) {
    PHASE(3); embeddings::etype = embeddings::eNatural;
    }
  
  /* other stuff */

  else if(argis("-som-test")) {
    PHASE(3);
    start_game();
    sphere_test();
    println(hlog, "data = ", isize(data), " neurons = ", isize(net));
    }

  else if(argis("-som-cdata")) {
    PHASE(3);
    start_game();
    create_data();
    }

  else if(argis("-subdata")) {
    shift();
    create_subdata(argi());
    }

  else if(argis("-subdata-val")) {
    shift();
    subdata_value = argi();
    }

  else if(argis("-landscape-dim")) {
    shift(); landscape_dim = argi();
    }

  else if(argis("-som-optimize")) {
    PHASE(3);
    start_game();
    // som_optimize();
    }

  else if(argis("-som-table")) {
    PHASE(3);
    start_game();
    som_table();
    }

  else if(argis("-som-scale-data")) {
    PHASE(3);
    shift(); current_scale = data_scale = argi();
    }
  
  else if(argis("-som-scale-embed")) {
    PHASE(3);
    shift(); current_scale = embed_scale = argi();
    }
  
  else if(argis("-by-name")) {
    PHASE(3);
    init_shapes();
    land_structure = lsSingle;
    shift(); string s = args();
    
    if(shapes.count(s)) {
      kohonen::kqty = kohonen::krad = 0;
      kohonen::kohrestrict = 999999999;
      stop_game();
      shapes[s]();
      start_game();
      initialize_rv();
      embeddings[s]();
      println(hlog, "embedding used: ", emb, " for: ", s);
      }
    else {
      println(hlog, "shape unknown: ", s);
      }
    }

  else if(argis("-only-landscape")) {
    only_landscape = true;
    }

  else if(argis("-som-experiment")) {
    PHASE(3);
    init_shapes();   
    land_structure = lsSingle;
    all_pairs(false);
    }

  else if(argis("-som-experiment1")) {
    PHASE(3);
    init_shapes();   
    land_structure = lsSingle;
    all_pairs(true);
    }

  else if(argis("-som-experiment-index")) {
    PHASE(3);
    init_shapes();   
    land_structure = lsSingle;
    create_index();
    }

  else if(argis("-edgecheck")) {
    PHASE(3);
    unsigned x = 1;
    for(auto e: test_orig.edges) {
      x*= 7;
      x += e.first;
      x += 123 * e.second;
      }
    println(hlog, "x = ", x, " edges = ", isize(test_orig.edges));
    }

  else if(argis("-som-experiment-tables")) {
    PHASE(3);
    init_shapes();    
    land_structure = lsSingle;
    create_edgelists();
    }

  else if(argis("-ex")) exit(0);

  else return 1;
  return 0;
  }

auto hooks3 = addHook(hooks_args, 100, readArgs);
#endif

auto khook = arg::add3("-kst-keys", [] { rv_hook(hooks_handleKey, 150, kst_key); })
  + addHook(hooks_configfile, 100, [] {
    param_i(ks_empty, "ks_empty", 0);
    param_i(ks_distant, "ks_distant", 0);
    param_i(ks_nonadj, "ks_nonadj", 0);
    param_i(max_distance, "ks_max");
    })
  + arg::add3("-kst-colorless", [] { colorless = true; })
  + addHook(hooks_markers, 100, [] () {
    int N = isize(net);
    bool multidraw = quotient;
    bool use_brm = closed_manifold && isize(currentmap->allcells()) <= brm_limit;
    vid.linewidth *= 3;
    for(auto e: voronoi_edges)
      if(e.first < N && e.second < N)
        for(const shiftmatrix& V1 : hr::span_at(current_display->all_drawn_copies, net[e.first].where)) {
          if(use_brm) {
            auto V2 = brm_get(net[e.first].where, C0, net[e.second].where, C0);
            queueline(V1*C0, V1*V2*C0, 0xC010C0FF, vid.linequality + 3);
            }
          else if(multidraw || elliptic) {
            auto V2 = memo_relative_matrix(net[e.second].where, net[e.first].where);
            queueline(V1*C0, V1*V2*C0, 0xC010C0FF, vid.linequality + 3);
            }
          else
            for(const shiftmatrix& V2 : hr::span_at(current_display->all_drawn_copies, net[e.second].where))
              queueline(V1*C0, V2*C0, 0xC010C0FF, vid.linequality + 3);
          }
    vid.linewidth /= 3;
    })
  + arg::add3("-kst-animate", [] { rv_hook(anims::hooks_record_anim, 100, [] (int i, int noframes) {
    bool steps = false;
    ld nf = noframes;
    while(t * nf > (nf - i) * tmax)
      step(), steps = true;
    if(steps) analyze();
    }); });

}}

