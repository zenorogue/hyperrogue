namespace rogue_unlike {

string save_name = "rogueviz/ru/testing-save.txt";

void save_revert(hstream& f, const revert_type& r) {
  bool first = true;
  for(auto& s: r) { if(first) first = false; else print(f, " "); print(f, s); }
  println(f);
  }

void save() {
  fhstream f(save_name, "wt");

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

    for(auto& e: r.entities) {
      changeseeker cs;
      e->hs(cs);
      if(cs.changed) {
        println(f, "ENTITY ", e->id);
        saver sav(f);
        e->hs(sav);
        println(f);
        }
      }
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
