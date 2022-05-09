namespace nilrider {

const string ver = "0.1";

string new_replay_name() {
  time_t timer;
  timer = time(NULL);
  char timebuf[128]; 
  strftime(timebuf, 128, "%y%m%d-%H%M%S", localtime(&timer));
  return timebuf;
  }

void save() {
  #if CAP_SAVE
  println(hlog, "save called");
  fhstream f("nilrider.save", "wt");
  println(f, "NilRider version ", ver);
  for(auto l: all_levels) {
    for(int i: {0,1}) {
      int qg = isize(l->records[i]);
      for(int g=0; g<qg; g++)
        if(l->records[i][g]) {
          println(f, "*RECORD");
          println(f, l->name);
          println(f, format("%d %d %f", i, g, l->records[i][g]));
          }
      }
    }
  for(auto l: all_levels) {
    for(auto& p: l->manual_replays) {
      println(f, "*MANUAL");
      println(f, l->name);
      println(f, p.name);
      println(f, isize(p.headings));
      for(auto t: p.headings) println(f, t);
      println(f);
      }
    for(auto& p: l->plan_replays) {
      println(f, "*PLANNING");
      println(f, l->name);
      println(f, p.name);
      println(f, isize(p.plan));
      for(auto t: p.plan) println(f, format("%.6f %.6f %.6f %.6f", t.at[0], t.at[1], t.vel[0], t.vel[1]));
      println(f);
      }
    }
  #endif
  }

level *level_by_name(string s) {
  for(auto l: all_levels) if(l->name == s) return l;
  println(hlog, "error: unknown level ", s);
  return nullptr;
  }

void load() {
  #if CAP_SAVE
  println(hlog, "load called");
  fhstream f("nilrider.save", "rt");
  if(!f.f) return;
  string ver = scanline_noblank(f);
  while(!feof(f.f)) {
    string s = scanline_noblank(f);
    if(s == "") continue;
    if(s == "*MANUAL") {
      string lev = scanline_noblank(f);
      string name = scanline_noblank(f);
      vector<int> headings;
      int size = scan<int> (f);
      if(size < 0 || size > 1000000) throw hstream_exception();
      for(int i=0; i<size; i++) headings.push_back(scan<int>(f));
      auto l = level_by_name(lev);
      if(l) l->manual_replays.emplace_back(manual_replay{name, std::move(headings)});
      continue;
      }
    if(s == "*PLANNING") {
      string lev = scanline_noblank(f);
      string name = scanline_noblank(f);
      plan_t plan;
      int size = scan<int> (f);
      if(size < 0 || size > 1000000) throw hstream_exception();
      plan.resize(size, {C0, C0});
      for(int i=0; i<size; i++) scan(f, plan[i].at[0], plan[i].at[1], plan[i].vel[0], plan[i].vel[1]);
      auto l = level_by_name(lev);
      if(l) l->plan_replays.emplace_back(plan_replay{name, std::move(plan)});
      continue;
      }
    if(s == "*RECORD") {
      string lev = scanline_noblank(f);
      int i = scan<int>(f);
      int g = scan<int>(f);
      ld t = scan<ld>(f);
      auto l = level_by_name(lev);
      if(l) {
        int qg = isize(l->goals);
        if(i < 0 || i > 1) println(hlog, "error: wrong number as i");
        if(g < 0 || g >= qg) println(hlog, "error: wrong goal index");
        l->records[i].resize(qg, 0);
        l->records[i][g] = t;
        }
      continue;
      }
    println(hlog, "error: unknown content ", s);
    }
  #endif
  }

}
