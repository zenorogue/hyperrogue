#include "../rogueviz.h"
#include "embeddings.h"

namespace rogueviz {

namespace embeddings {

std::shared_ptr<embedding> current;

vector<vector<int> > directed_edges;

// keep this correct
// note: isize(vdata) may be not correct because of the extra 'legend' nodes
int get_n() {
  return isize(directed_edges);
  }

rogueviz::edgetype *any;

edgetype *ensure_edge() {
  if(!any) any = add_edgetype("embedded edge");
  return any;
  }

vector<ld> importance;

struct rv_embedding : public tiled_embedding {
  virtual string name() override { return "RogueViz internal representation"; }
  pair<cell*, hyperpoint> as_location(int id) override {
    return { vdata[id].m->base, vdata[id].m->at * C0 };
    }
  };

void symmetrize() {
  int n = get_n();
  directed_edges.clear();
  directed_edges.resize(n);
  for(auto& e: edgeinfos) {
    directed_edges[e->i].push_back(e->j);
    directed_edges[e->j].push_back(e->i);
    }
  }

void read_edgelist(const string& fname) {

  rogueviz::init(RV_GRAPH);

  rv_hook(hooks_clearmemory, 100, [] {
    directed_edges.clear();
    current = nullptr;
    any = nullptr;
    });

  ensure_edge();
  indenter_finish ind(true, "Reading edgelist...");

  fhstream f(fname, "rt");
  if(!f.f) return file_error(fname);

  set<pair<int, int>> edgesets;

  while(true) {
    int i = rogueviz::readLabel(f), j = rogueviz::readLabel(f);
    if(i == -1 || j == -1) break;

    sizeto(directed_edges, i);
    sizeto(directed_edges, j);

    if(edgesets.count({i, j}) == 0 && edgesets.count({j, i}) == 0)
      addedge(i, j, 1, any);

    if(edgesets.count({i, j}) == 0) {
      edgesets.emplace(i, j);
      directed_edges[i].push_back(j);
      }
    }

  int id = 0;
  auto all = currentmap->allcells();

  if(rogueviz::rv_quality >= 1) {
    for(auto& v: vdata) 
      v.be(all[gmod(id++, isize(all))], Id);
    current = std::make_shared<rv_embedding> ();
    }
  }

void write_edgelist(const string &fname) {
  FILE *f = fopen(fname.c_str(), "wt");
  if(!f) return file_error(fname);
  for(auto& e: edgeinfos) {
    if(vizflags & RV_HAVE_WEIGHT)
      fprintf(f, "%s;%s;%.17e\n", vdata[e->i].name.c_str(), vdata[e->j].name.c_str(), e->weight);
    else
      fprintf(f, "%s %s\n", vdata[e->i].name.c_str(), vdata[e->j].name.c_str());
    }
  fclose(f);
  }

void force_rvgraph() {
  for(auto& v: vdata) if(v.id < get_n()) {
    auto p = current->as_location(v.id);
    if(get_node_rendered(v.id))
      v.be(p.first, rgpushxto0(p.second));
    else
      v.be_nowhere();
    }
  }

vector<ld> node_importance;
vector<bool> node_rendered;
ld render_radius;

void recompute_rendered() {
  if(render_radius > 0) {
    int N = get_n();
    vector<int> order;
    for(int i=0; i<N; i++) order.push_back(i);
    sort(order.begin(), order.end(), [] (int i, int j) { return pair(get_node_importance(i), i) < pair(get_node_importance(j), j); });
    vector<hyperpoint> sofar;
    node_rendered.resize(N);
    for(auto i: order) {
      bool ok = true;
      auto h1 = current->as_hyperpoint(i);
      for(auto h0: sofar) if(hdist(h0, h1) < render_radius) { ok = false; break; }
      node_rendered[i] = ok;
      if(ok) sofar.push_back(h1);
      }
    }
  }

void reenable_embedding() {
  eval.current = false;
  recompute_rendered();
  if(rogueviz::rv_quality >= 1 && among(current->get_dimension(), 2, 3) && current->get_dimension() <= WDIM) force_rvgraph();
  }

void enable_embedding(std::shared_ptr<embedding> pe) {
  current = std::move(pe);
  eval.current = false;
  reenable_embedding();
  }

void esave(string fname) {
  fhstream f(arg::args(), "wt");
  if(!f.f) file_error(fname);
  current->save(f);
  }

void store_gamedata(struct hr::gamedata* gd) { gd->store(current); }

string embedding::get_space() {
  shstream ss;
  ss.write(ss.get_vernum());
  mapstream::save_geometry(ss);
  return ss.s;
  }

void set_missing_edges(ld x) {
  missing_edges = x;
  log_missing_edges = log(x);
  remaining_edges = 1 - x;
  log_remaining_edges = log(1 - x);
  }

int a = arg::add3("-edgelist", [] { arg::shift(); read_edgelist(arg::args()); })
  + addHook(hooks_gamedata, 230, store_gamedata)
  + arg::add3("-write-edges", [] { arg::shift(); write_edgelist(arg::args()); })
  + arg::add3("-esaveas", [] { arg::shift(); esave(arg::args()); })
  + arg::add3("-missing-edges", [] { arg::shift(); set_missing_edges(arg::argf()); })
  + arg::add3("-missing-edges-env", [] { auto env = std::getenv("MISSING_EDGES"); if(env) set_missing_edges(atof(env)); })
  + arg::add3("-el-rv", [] { if(rogueviz::rv_quality == 0) force_rvgraph(); current = std::make_shared<rv_embedding> (); })
  + arg::add3("-render-radius", [] { arg::shift_arg_formula(render_radius); })
  + arg::add3("-edge-arrow", [] {
    arg::shift(); any->arrow_scale = arg::argf();
    });

}

}

#include "polar.cpp"
#include "coords.cpp"
#include "loglik.cpp"
#include "routing.cpp"

#include "maprank.cpp"
#include "mdl.cpp"
#include "compare.cpp"
#include "visualize.cpp"
