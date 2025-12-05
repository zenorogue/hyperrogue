#include "dhrg.h"

namespace dhrg {

string legacy_dhrg_name;

int dhrg_legacy_args() {
  using namespace arg;
           
  if(argis("-graph") || argis("-dhrg")) {
    PHASE(3); shift();
    legacy_dhrg_name = args();
    rogueviz::embeddings::read_edgelist(legacy_dhrg_name + "-links.txt");
    rogueviz::embeddings::read_polar(legacy_dhrg_name + "-coordinates.txt");
    dhrg_init(); graph_from_rv();
    next_timestamp++;
    ts_rogueviz = next_timestamp;
    ts_vertices = next_timestamp;
    }

  else if(argis("-esave")) {
    fhstream f(legacy_dhrg_name + "-dhrg.txt", "wt");
    if(!f.f) { file_error(legacy_dhrg_name); return 0; }
    rogueviz::embeddings::current->save(f);
    }

  else if(argis("-eload")) {
    PHASE(3); shift();
    legacy_dhrg_name = args();
    rogueviz::embeddings::read_edgelist(legacy_dhrg_name + "-links.txt");
    dhrg_init(); load_embedded(legacy_dhrg_name + "-dhrg.txt");
    next_timestamp++;
    ts_rogueviz = next_timestamp;
    ts_vertices = next_timestamp;
    }

  else return 1;

  return 0;
  }

auto legacy_hook = 
    addHook(hooks_args, 50, dhrg_legacy_args);

}
