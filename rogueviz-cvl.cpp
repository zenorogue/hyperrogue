// Copyright (C) 2018 Zeno and Tehora Rogue, see 'hyper.cpp' for details
// this is a plugin which generates branched tilings for newconformist 
// https://github.com/zenorogue/newconformist (see the option '-cvl')

namespace hr {

struct location {
  transmatrix lView;
  heptspin lviewctr;
  };

struct lineinfo {
  vector<location> locs;
  int plus_matrices;
  int minus_matrices;
  };

map<int, lineinfo> lines;

location loc_multiply(location orig, transmatrix T) {
  dynamicval<transmatrix> dv(View, orig.lView);
  dynamicval<heptspin> dc(viewctr, orig.lviewctr);
  View = inverse(T) * View;
  for(int a=0; a<10; a++) optimizeview();
  return location{View, viewctr};
  }

bool show_map = false;

void cvl_marker() {
  if(show_map) for(auto& l: lines) {
    int id = 0;
    for(auto& loc: l.second.locs) {
      if(gmatrix.count(loc.lviewctr.at->c7)) {
        transmatrix T = gmatrix[loc.lviewctr.at->c7] * inverse(spin(loc.lviewctr.spin*2*M_PI/S7 + master_to_c7_angle())) * inverse(loc.lView);
        queuepoly(T, shAsymmetric, 0xFF00FFFF);
        queuestr(T, 1.0, its(l.first)+"/"+its(id), 0xFFFFFF);
        }
      id++;
      }
    }
  }

int readArgs() {
  using namespace arg;
           
  if(0) ;
  else if(argis("-cvlbuild")) {
    PHASEFROM(3);
    start_game();
    shift(); FILE *f = fopen(argcs(), "rt");
    if(!f) { shift(); printf("failed to open file\n"); return 0; }
    int id;
    lineinfo l0;    
    ignore(fscanf(f, "%d%d%d", &id, &l0.plus_matrices, &l0.minus_matrices));
    l0.locs.push_back(location{View, viewctr});
    for(int i=1; i<l0.plus_matrices; i++)
      l0.locs.push_back(loc_multiply(l0.locs.back(), xpush(1)));
    lines[id] = std::move(l0);
    
    while(true) {
      ignore(fscanf(f, "%d", &id));
      println(hlog, "id=", id, ".");
      if(id < 0) break;
      auto& l1 = lines[id];
      int step;
      ignore(fscanf(f, "%d%d", &id, &step));
      transmatrix T;
      double d;
      for(int a=0; a<9; a++) {
        ignore(fscanf(f, "%lf", &d));
        T[0][a] = d;
        }
      ignore(fscanf(f, "%d%d", &l1.plus_matrices, &l1.minus_matrices));
      auto old = lines[id].locs[step];
      println(hlog, "FROM ", old.lView, old.lviewctr, " id=", id, " step=", step);
      l1.locs.push_back(loc_multiply(old, T));
      println(hlog, "TO ", l1.locs.back().lView, l1.locs.back().lviewctr, "; creating ", l1.plus_matrices);
      for(int i=1; i<l1.plus_matrices; i++)
        l1.locs.push_back(loc_multiply(l1.locs.back(), xpush(1)));
      println(hlog, "LAST ", l1.locs.back().lView, l1.locs.back().lviewctr);
      }
    }
  else if(argis("-cvllist")) {
    for(auto& l: lines)
      for(auto& loc: l.second.locs) {
        println(hlog, l.first, ". ", loc.lviewctr, " (dist=", celldist(loc.lviewctr.at->c7), "), View = ", loc.lView);
        }
    }
  else if(argis("-cvlmap")) {
    show_map = !show_map;
    }
  else if(argis("-cvldraw")) {
    shift(); string s = args();
    for(auto& p: lines) {
      int i = 0;
      for(auto& loc: p.second.locs) {
        dynamicval<transmatrix> dv(View, loc.lView);
        dynamicval<heptspin> dc(viewctr, loc.lviewctr);
        shot::take(format(s.c_str(), p.first, i++));
        }
      }
    }
  else return 1;
  return 0;
  }

auto magichook = addHook(hooks_args, 100, readArgs) + addHook(hooks_frame, 100, cvl_marker);


 
}
