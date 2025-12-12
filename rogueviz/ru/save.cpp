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

void save_as(string fname) {
  fhstream f(fname, "wt");

  println(f, "TIME ", gframeid);

  save_via_stater(f, m, "MAN", false);

  if(m.morphed) save_via_stater(f, *m.morphed, "MORPH", true);

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

    println(f, "ROOM ", r.id);
    string code;
    for(int y=0; y<room_y; y++)
    for(int x=0; x<room_y; x++) code += (r.fov[y][x] ? '1' : '0');
    println(f, as_hexstring(compress_string(code)));

    if(&r == current_room) println(f, "HERE");

    for(int y=0; y<room_y; y++)
    for(int x=0; x<room_y; x++) if(r.block_at[y][x] != r.orig_block_at[y][x])
      println(f, "AT ", y, " ", x, " ", r.block_at[y][x]);

    for(auto& e: r.entities) save_via_stater(f, *e, "ENTITY");

    if(r.timed_orb_end >= gframeid) println(f, "TIMER ", r.timed_orb_end);
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

void save() {
  save_as(save_name);
  }

void reset_all() {
  m.hs(resetter);
  for(auto& p: powers) p.hs(resetter);
  for(auto r: *all_effects) r.second->hs(resetter);
  fountain_revert.clear();
  death_revert.clear();

  for(auto& [c,r]: rooms) {
    for(auto& e: r.entities) { e->on_reset_all(); e->hs(resetter); }

    for(int y=0; y<room_y; y++)
    for(int x=0; x<room_x; x++) {
      r.fov[y][x] = false;
      }
    r.block_at = r.orig_block_at;
    }
  }

void load_reverts(fhstream& f, revert_stack& stack) {
  string s;
  while(true) {
    s = scanline_noblank(f);
    if(s == "") return;
    s += " ";
    revert_type rev;
    string cur = "";
    for(char c: s) if(c == ' ') rev.push_back(cur), cur = ""; else cur += c;
    stack.push_back(rev);
    }
  }

template<class T> void load_hs(fhstream &f, T& t) {
  string s;
  while(true) {
    s = scanline_noblank(f);
    if(s == "") return;
    auto pos = s.find("=");
    if(pos == string::npos) { println(hlog, "warning: incorrect HS line: ", s); return; }
    loader l;
    l.name = s.substr(0, pos);
    l.value = s.substr(pos + 1);
    t.hs(l);
    if(!l.loaded) println(hlog, "warning: could not read hs: ", s, " for entity with ID: ", t.id);
    }
  }

void load_from(string fname) {
  reset_all();
  fhstream f(fname, "rt");

  room *current_room = nullptr;
  power *current_power = nullptr;
  entity* current_entity = nullptr;

  string s;
  while(!feof(f.f)) {
    s = scanline_noblank(f);
    if(s == "") continue;
    auto pos = s.find(" ");
    if(pos == string::npos) {
      if(s == "HERE") { if(!current_room) { println(hlog, "warning: no current room"); continue; } rogue_unlike::current_room = current_room; }
      else if(s == "FOUNTAINS") 
        load_reverts(f, fountain_revert);
      else if(s == "DEATH") 
        load_reverts(f, death_revert);
      else
        println(hlog, "warning: ill-formed string in save: ", s); continue;
      }
    string cap = s.substr(0, pos);
    string param = s.substr(pos+1);
    if(cap == "TIME") {
      sscanf(param.c_str(), "%d", &gframeid);
      }
    else if(cap == "ROOM") {
      try {
        current_room = find_room_by_id(param);
        string code = decompress_string(from_hexstring(scanline_noblank(f)));
        int pos = 0;
        if(code.size() == room_y * room_x)
          for(int y=0; y<room_y; y++) for(int x=0; x<room_x; x++) current_room->fov[y][x] = code[pos++] == '1';
        }
      catch(hr_name_error& e) {
        println(hlog, "warning: could not find room: ", param);
        }
      }
    else if(cap == "AT") {
      if(!current_room) { println(hlog, "warning: no current room"); continue; }
      int x, y, c;
      sscanf(param.c_str(), "%d%d%d", &x, &y, &c);
      current_room->block_at[y][x] = c;
      }
    else if(cap == "TIMER") {
      if(!current_room) { println(hlog, "warning: no current room"); continue; }
      current_room->timed_orb_end = atoi(param.c_str());
      }
    else if(cap == "ENTITY") {
      if(!current_room) { println(hlog, "warning: no current room"); continue; }
      auto add = [&] (std::unique_ptr<entity>&& e) { current_room->entities.emplace_back(std::move(e)); current_entity = &*(current_room->entities.back()); };
      if(param == "GHOST") add(std::make_unique<ghost>());
      else if(param == "BONES") add(std::make_unique<ghost_item>());
      else if(param == "FIREMISSILE") add(std::make_unique<fire_missile>());
      else if(param == "DISNAKE") add(std::make_unique<disnake>());
      else try { current_entity = find_entity_by_id(param); } catch(hr_name_error& e) { current_entity = nullptr; continue; }
      load_hs(f, *current_entity);
      }
    else if(cap == "MORPH") {
      if(param == "cat") { m.morphed = new cat; load_hs(f, *m.morphed); }
      else if(param == "capy") { m.morphed = new capybara; load_hs(f, *m.morphed); }
      }
    else if(cap == "MAN") {
      load_hs(f, m);
      }
    else if(cap == "POWER") {
      current_power = nullptr;
      try {
        current_power = &find_power_by_id(param);
        load_hs(f, *current_power);
        current_power->randeffs = {};
        }
      catch(hr_name_error& e) {
        println(hlog, "warning: unknown power: ", param);
        }
      }
    else if(cap == "EFFECT") {
      if(!all_effects->count(param)) {
        println(hlog, "warning: unknown effect: ", param);
        }
      else {
        randeff* current_effect = (*all_effects)[param];
        if(current_power) current_power->randeffs.push_back(current_effect);
        load_hs(f, *current_effect);
        }
      }
    else {
      println(hlog, "warning: could not understand save line: ", s);
      }
    }
  }

}
