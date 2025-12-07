namespace rogue_unlike {

string save_name = "rogueviz/ru/testing-save.txt";

void save_revert(hstream& f, const revert_type& r) {
  bool first = true;
  for(auto& s: r) { if(first) first = false; else print(f, " "); print(f, s); }
  println(f);
  }

void power::hs(stater& s) {
  sact(s, "flavor", fl);
  s.act("id", id_status, 0).act("qf", qty_filled, 0).act("qo", qty_owned, 0).act("flags", flags, 0).act("rv", random_value, 0);
  }

void randeff::hs(stater& s) {
  string str = which_weapon ? which_weapon->id : "NONE";
  s.act("wpn", str, "NONE");
  try {
    which_weapon = (str != "NONE") ? &find_power_by_id(str) : nullptr;
    }
  catch(hr_name_error& e) { which_weapon = &find_power_by_id("dagger"); }
  }

template<class T> void save_via_stater(fhstream& f, T& t, string cat, bool always = false) {
  changeseeker cs;
  t.hs(cs);
  if(cs.changed || always) {
    println(f, cat, " ", t.id);
    saver sav(f);
    t.hs(sav);
    println(f);
    }
  }

void save() {
  fhstream f(save_name, "wt");

  save_via_stater(f, m, "MAN", false);

  // list current powers
  for(auto& p: powers) {
    save_via_stater(f, p, "POWER", isize(p.randeffs));
    for(auto& r: p.randeffs) save_via_stater(f, *r, "EFFECT", true);
    }

  for(auto& [c,r]: rooms) {
    if(!r.save_to_save) continue;
    int seen_count = 0;
    for(int y=0; y<room_y; y++)
    for(int x=0; x<room_y; x++) if(r.fov[y][x]) seen_count ++;
    for(auto& e: r.entities) { changeseeker cs; e->hs(cs); if(cs.changed) seen_count++; }

    if(!seen_count) continue;

    println(f, "ROOM ", r.roomname);
    shstream ss;
    for(int y=0; y<room_y; y++)
    for(int x=0; x<room_y; x++) println(ss, r.fov[y][x] ? '1' : '0');
    println(f, as_hexstring(compress_string(ss.s)));

    for(auto& e: r.entities) save_via_stater(f, *e, "ENTITY");
    println(f);
    }

  if(fountain_revert.size()) {
    println(f, "FOUNTAINS");
    for(auto r: fountain_revert) save_revert(f, r);
    println(f);
    }

  if(death_revert.size()) {
    println(f, "DEATH");
    for(auto r: death_revert) save_revert(f, r);
    println(f);
    }
  }

}
