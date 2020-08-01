/* explore the Janko group J1: https://en.wikipedia.org/wiki/Janko_group_J1 */

#include "rogueviz.h"

#if !ISWEB

namespace hr {

eGeometry gJanko1(eGeometry(-1));

struct jmatrix : array<array<int, 7>, 7> {
  jmatrix operator * (const jmatrix other) {
    jmatrix res;
    for(int a=0; a<7; a++)
    for(int b=0; b<7; b++)
      res[a][b] = 0;

    for(int a=0; a<7; a++)
    for(int b=0; b<7; b++)
    for(int c=0; c<7; c++)
      res[a][c] += self[a][b] * other[b][c];

    for(int a=0; a<7; a++)
    for(int b=0; b<7; b++)
      res[a][b] %= 11;

    return res;
    }
  };

vector<jmatrix> jms;
std::unordered_map<jmatrix, int> ids;

jmatrix J, Z, id;

int get_id(const jmatrix& m) {
  if(ids.count(m)) return ids[m];
  ids[m] = isize(jms);
  jms.push_back(m);
  return isize(jms)-1;
  }

void build_group(const jmatrix& m) {
  if(ids.count(m)) return;
  jmatrix m2 = m;
  for(int a=0; a<7; a++) { get_id(m2); m2 = m2 * J; }
  }

void build() {
  for(int a=0; a<7; a++)
  for(int b=0; b<7; b++) J[a][b] = ((a+1-b)%7 == 0) ? 1 : 0;
  
  int v[49] = {
    0,  4,  0,  3,  9,  2,  1, 
    4,  0,  4,  3,  7,  1,  3, 
    0,  4, 10,  6,  9,  8,  1, 
    3,  3,  6,  1,  7,  6,  9, 
    9,  7,  9,  7,  0,  9, 10, 
    2,  1,  8,  6,  9,  2,  3, 
    1,  3,  1,  9, 10,  3,  8, 
/*    -3,+2,-1,-1,-3,-1,-3,
    -2,+1,+1,+3,+1,+3,+3,
    -1,-1,-3,-1,-3,-3,+2,
    -1,-3,-1,-3,-3,+2,-1,
    -3,-1,-3,-3,+2,-1,-1,
    +1,+3,+3,-2,+1,+1,+3,
    +3,+3,-2,+1,+1,+3,+1*/
    };
  
  int a = 0;

  for(auto& row: Z) for(auto& i: row) { i = v[a++]; if(i < 0) i += 11; }
  
  for(int a=0; a<7; a++) for(int b=0; b<7; b++) id[a][b] = a==b ? 1 : 0;
  
  build_group(id);
  for(int k=0; k<isize(jms); k++) {
    build_group(jms[k] * Z);
    }
  
  /*
  B = Z
  J = A * B
  A = J * B * B
  */

  println(hlog, "order = ", isize(jms));
  }

void experiment() {
  for(int ia=1; ia<isize(jms); ia++) if(jms[ia] * jms[ia] == id)
  for(int ib=1; ib<isize(jms); ib++) if(jms[ib] * jms[ib] * jms[ib] == id) {
    jmatrix u = jms[ia] * jms[ib];
    if(u != J) continue;
    if(u == id) continue;
    jmatrix up = u, ubp = u;
    ubp = ubp * ubp; up = up * ubp;
    ubp = ubp * ubp; up = up * ubp;
    if(up != id) continue;
    println(hlog, "found: ", tie(ia, ib));

    for(auto& row: jms[ia]) {
      for(auto& i: row) print(hlog, format("%2d, ", i));
      println(hlog);
      }
    }

  exit(1);
  }

auto h = addHook(hooks_newmap, 0, [] {
  if(geometry == gJanko1) {
    build();
    vector<int> connections;
    connections.clear();
    for(int i=0; i<isize(jms); i++) connections.push_back(get_id(jms[i] * Z));
    println(hlog, connections);
    return (hrmap*) new quotientspace::hrmap_quotient(connections);
    }
  return (hrmap*) nullptr;
  });

void create_janko() {
  if(gJanko1 != eGeometry(-1)) return;
  ginf.push_back(ginf[gNormal]);
  gJanko1 = eGeometry(isize(ginf) - 1);
  // variation = eVariation::pure;
  auto& gi = ginf.back();
  gi.flags = qANYQ | qBOUNDED | qEXPERIMENTAL;
  gi.quotient_name = "Janko";
  gi.shortname = "Janko";
  gi.xcode = 0x31400;
  gi.menu_displayed_name = "Janko group J1";
  }
  
auto shot_hooks = addHook(hooks_args, 100, [] {
  if(arg::argis("-janko1x")) {
    build();
    experiment();
    }
  else return true;
  return false;
  }) + addHook(hooks_initialize, 100, create_janko);

}

#endif
