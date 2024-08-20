namespace nilrider {

const string ver = "0.2";

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
          println(f, hr::format("%d %d %f", i, g, l->records[i][g]));
          }
      }
    }
  println(f, "*COLORS\n");
  println(f, "*RLE\n");
  for(auto l: all_levels) {
    if(l->flags & nrlUserCreated) {
      println(f, "*FILE");
      println(f, l->filename);
      }
    for(auto& p: l->manual_replays) {
      println(f, "*MANUAL");
      println(f, l->name);
      println(f, p.name);
      fprintf(f.f, "%08x %08x %08x %08x\n", p.cs.wheel1, p.cs.wheel2, p.cs.seat, p.cs.seatpost);
      println(f, isize(p.headings));
      for(auto t: p.headings) println(f, t.first, " ", t.second);
      println(f);
      }
    for(auto& p: l->plan_replays) {
      println(f, "*PLANNING");
      println(f, l->name);
      println(f, p.name);
      fprintf(f.f, "%08x %08x %08x %08x\n", p.cs.wheel1, p.cs.wheel2, p.cs.seat, p.cs.seatpost);
      println(f, isize(p.plan));
      for(auto t: p.plan) println(f, hr::format("%.6f %.6f %.6f %.6f", t.at[0], t.at[1], t.vel[0], t.vel[1]));
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

colorscheme load_colors(fhstream& f, bool have_colors) {
  if(have_colors) {
    colorscheme s(0);
    fscanf(f.f, "%x%x%x%x", &s.wheel1, &s.wheel2, &s.seat, &s.seatpost);
    return s;
    }
  else {
    colorscheme s(2);
    return s;
    }
  }

vector<pair<int, int>> apply_rle(const vector<int>& data) {
  vector<pair<int, int>> rle;
  if(data.empty()) return rle;
  int last = data[0], count = 0;
  for(int v: data) {
    if(v != last) { rle.emplace_back(count, last); count = 0; last = v; }
    count++;
    }
  rle.emplace_back(count, last);
  return rle;
  }

void load() {
  #if CAP_SAVE
  bool have_colors = false;
  bool have_rle = false;
  println(hlog, "load called");
  fhstream f("nilrider.save", "rt");
  if(!f.f) return;
  string ver = scanline_noblank(f);
  while(!feof(f.f)) {
    string s = scanline_noblank(f);
    if(s == "") continue;
    if(s == "*COLORS") { have_colors = true; continue; }
    if(s == "*RLE") { have_rle = true; continue; }
    if(s == "*FILE") {
      string s1 = scanline_noblank(f);
      try { load_level(s1, false); }
      catch(hr_exception& e) { println(hlog, "error: could not load level ", s1, ", reason: ", e.what()); }
      }
    if(s == "*MANUAL") {
      string lev = scanline_noblank(f);
      string name = scanline_noblank(f);
      colorscheme cs = load_colors(f, have_colors);
      vector<pair<int, int>> headings;
      int size = scan<int> (f);
      if(size < 0 || size > 1000000) throw hstream_exception();
      if(have_rle) {
        println(hlog, "reading a RLE replay");
        for(int i=0; i<size; i++) { int rep = scan<int>(f); headings.emplace_back(rep, scan<int>(f)); }
        }
      else {
        vector<int> h;
        for(int i=0; i<size; i++) h.emplace_back(scan<int>(f));
        headings = apply_rle(h);
        println(hlog, "converted ", isize(h), " to ", isize(headings));
        }
      auto l = level_by_name(lev);
      if(l) l->manual_replays.emplace_back(manual_replay{name, cs, std::move(headings)});
      continue;
      }
    if(s == "*PLANNING") {
      string lev = scanline_noblank(f);
      string name = scanline_noblank(f);
      colorscheme cs = load_colors(f, have_colors);
      plan_t plan;
      int size = scan<int> (f);
      if(size < 0 || size > 1000000) throw hstream_exception();
      plan.resize(size, {C0, C0});
      for(int i=0; i<size; i++) scan(f, plan[i].at[0], plan[i].at[1], plan[i].vel[0], plan[i].vel[1]);
      auto l = level_by_name(lev);
      if(l) l->plan_replays.emplace_back(plan_replay{name, cs, std::move(plan)});
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

void level::load_plan_as_ghost(plan_replay& r) {
  vector<timestamp> history_backup;
  swap(history_backup, history);
  swap(r.plan, plan);
  history.clear();
  history.push_back(start);
  while(true) {
    int s = isize(history);
    if(!simulate()) break;
    if(isize(history) == s) break;
    }
  println(hlog, "a history of length ", isize(history), " becomes a ghost");
  ghosts.emplace_back(ghost{r.cs, {}});
  auto& g = ghosts.back();
  g.history = std::move(history);
  swap(history_backup, history);
  swap(r.plan, plan);
  }

vector<timestamp> level::headings_to_history(manual_replay& r) {
  vector<timestamp> history;
  timestamp cur = start;
  for(auto [qty, h]: r.headings) {
    println(hlog, "pair: ", tie(qty, h));
    for(int i=0; i<qty; i++) {
      if(cur.on_surface) cur.heading_angle = int_to_heading(h);
      history.push_back(cur);
      if(!cur.tick(this)) return history;
      }
    }
  return history;
  }

void level::load_manual_as_ghost(manual_replay& r) {
  ghosts.emplace_back(ghost{r.cs, headings_to_history(r) });
  }

void save_manual_replay() {
  vector<int> ang;
  for(auto& h: curlev->history) ang.push_back(h.on_surface ? heading_to_int(h.heading_angle) : 0);
  curlev->manual_replays.emplace_back(manual_replay{new_replay_name(), my_scheme, apply_rle(ang)});
  save();
  }

}
