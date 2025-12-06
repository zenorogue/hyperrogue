namespace rogue_unlike {

room fake_room;

room *find_room_by_id(string s) {
  for(auto& [c,r]: rooms) if(r.id == s) return &r;
  throw hr_name_error("find_room");
  }

room *find_room_by_name(string s) {
  for(auto& [c,r]: rooms) if(r.roomname == s) return &r;
  throw hr_name_error("find_room");
  }

entity* find_entity_by_id(string s) {
  auto e = entity_by_id[s];
  if(!e) throw hr_name_error("find_entity");
  return e;
  }

template<class T> T force_exist(const T& c) { if(!c) throw hr_name_error("check"); return c; }

void revert(const revert_type& r) {
  // auto rf = [] (string s) { return atof(s.c_str()); };
  auto ri = [] (string s) { return atoi(s.c_str()); };

  try {

  if(r[0] == "BLOCK") find_room_by_id(r[1])->replace_block(ri(r[2]), ri(r[3]), (eWall) ri(r[4]));
  else if(r[0] == "ITEM") {
    auto& p = find_power(r[1]);
    p.qty_filled = ri(r[2]);
    p.qty_owned = ri(r[3]);
    }
  else if(r[0] == "EXIST") {
    auto e = find_entity_by_id(r[1]);
    if(e) e->existing = true;
    }
  else if(r[0] == "UNEXIST") {
    auto e = find_entity_by_id(r[1]);
    if(e) e->existing = false;
    }
  else if(r[0] == "UNBOUGHT") {
    auto e = find_entity_by_id(r[1]);
    if(e) e->existing = false;
    force_exist(e->as_shopitem())->bought = false;
    }

  } catch(hr_name_error& e) {
    println(hlog, "a name error occured while reverting: ", e.what());
    }
  }

}
