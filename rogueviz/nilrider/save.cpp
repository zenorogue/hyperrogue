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
  println(hlog, "save called");
  fhstream f("nilrider.save", "wt");
  println(f, "version ", ver);
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
  }

}
