#include "../rogueviz.h"

namespace rogueviz {

unique_ptr<embedding> current_embedding;

namespace embeddings {

vector<vector<int> > directed_edges;

rogueviz::edgetype *any;

struct rv_embedding : public embedding {
  hyperpoint as_hyperpoint(int id) override {
    return currentmap->relative_matrix(vdata[id].m->base, currentmap->gamestart(), C0) * (vdata[id].m->at * C0);
    }
  };

void read_edgelist(string fname) {

  rogueviz::init(RV_GRAPH);

  rv_hook(hooks_clearmemory, 100, [] {
    directed_edges.clear();
    current_embedding = nullptr;
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
    current_embedding = std::make_unique<rv_embedding> ();
    }
  }

int a = arg::add3("-edgelist", [] { arg::shift(); read_edgelist(arg::args()); });

}

}
