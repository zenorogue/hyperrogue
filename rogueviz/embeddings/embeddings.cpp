#include "../rogueviz.h"
#include "embeddings.h"

namespace rogueviz {

namespace embeddings {

std::shared_ptr<embedding> current;

vector<vector<int> > directed_edges;

rogueviz::edgetype *any;

struct rv_embedding : public tiled_embedding {
  pair<cell*, hyperpoint> as_location(int id) override {
    return { vdata[id].m->base, vdata[id].m->at * C0 };
    }
  };

void read_edgelist(const string& fname) {

  rogueviz::init(RV_GRAPH);

  rv_hook(hooks_clearmemory, 100, [] {
    directed_edges.clear();
    current = nullptr;
    });

  any = rogueviz::add_edgetype("embedded edges");

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

  if(rogueviz::rv_quality >= 0) {
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

void reenable_embedding() {
  if(rogueviz::rv_quality >= 0)
    for(auto& v: vdata) {
      auto p = current->as_location(v.id);
      v.be(p.first, rgpushxto0(p.second));
      }
  }

void enable_embedding(std::shared_ptr<embedding> pe) {
  current = std::move(pe);
  reenable_embedding();
  }

void esave(string fname) {
  fhstream f(arg::args(), "wt");
  if(!f.f) file_error(fname);
  current->save(f);
  }

void store_gamedata(struct hr::gamedata* gd) { gd->store(current); }

int a = arg::add3("-edgelist", [] { arg::shift(); read_edgelist(arg::args()); })
  + addHook(hooks_gamedata, 230, store_gamedata)
  + arg::add3("-write-edges", [] { arg::shift(); write_edgelist(arg::args()); })
  + arg::add3("-esaveas", [] { arg::shift(); esave(arg::args()); });

}

}

#include "polar.cpp"
#include "loglik.cpp"
#include "routing.cpp"

