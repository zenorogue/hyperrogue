namespace rogue_unlike {

void handle_powers(data& d);

bool on_fountain;
room *fountain_room;

void check_fountains() {
  bool next_on_fountain = false;
  auto bb = pixel_to_block(m.get_pixel_bbox());
  for(int x = bb.minx; x < bb.maxx; x++) for(int y = bb.miny; y < bb.maxy; y++) {
    eWall b = current_room->at(x, y);
    println(hlog, tuple(x, y, int(b)));
    if(b == wFountain) next_on_fountain = true;
    }
  if(next_on_fountain && !on_fountain) {
    fountain_room = current_room;
    addMessage("A magic fountain! You feel safe and refill your potions.");
    m.hp = m.max_hp();
    for(auto& p: powers) p.refill();
    for(auto& r: rooms) for(auto& e: r.second.entities) e->regenerate();
    current_target = nullptr;
    }
  swap(on_fountain, next_on_fountain);
  }

void man::act() {
  kino();

  current_stats = next_stats;
  next_stats = base_stats;
  auto dat = get_dat();

  coyote_time = next_coyote_time; next_coyote_time = 0;
  jump_control = next_jump_control; next_jump_control = 0;

  if(on_floor) on_floor_when = gframeid;

  fallthru = false;
   
  handle_powers(dat);

  if((on_floor || jump_control || wallhug) && !on_ice) {
    vel.x = zero_vel.x + dat.dx * dat.d * dat.modv * 2.5;
    }

  if(on_bounce) {
    vel.x += dat.dx * dat.d * dat.modv * 0.02;
    }

  if(!(on_floor && !dat.dx)) last_action = gframeid;

  if(dat.dx) facing = dat.dx;

  current_room->fov_from(where.x / block_x, where.y / block_y);

  check_fountains();
  }

}
