namespace rogue_unlike {

void handle_powers(data& d);

bool on_fountain;
room *fountain_room;
xy fountain_where;

room *stable_room;
xy stable_where;

void regenerate_all() {
  m.hp = m.max_hp();
  for(auto& p: powers) p.refill();
  for(auto& r: rooms) for(auto& e: r.second.entities) e->regenerate();
  revert_all(fountain_revert);
  current_target = nullptr;
  shuffle_all();
  }

void check_fountains() {
  bool next_on_fountain = false;
  auto bb = pixel_to_block(m.get_pixel_bbox());
  for(int x = bb.minx; x < bb.maxx; x++) for(int y = bb.miny; y < bb.maxy; y++) {
    eWall b = current_room->at(x, y);
    if(b == wFountain) next_on_fountain = true;
    }
  if(next_on_fountain && !on_fountain) {
    if(extra_life->flags & ACTIVE) {
      fountain_room = current_room;
      fountain_where = m.where;
      death_revert.clear();
      }
    addMessage("A magic fountain! You feel safe and refill your potions.");
    regenerate_all();
    }
  swap(on_fountain, next_on_fountain);
  }

void statdata::reset() {
  for(auto i: allstats) stats[i] = m.base_stats[i];
  coyote_time = 0;
  jump_control = 0;
  hallucinating = false;
  mods.clear();
  }

void man::act() {
  kino();

  if(is_stable) {
    stable_room = current_room;
    stable_where = where;
    }

  auto h = max_hp();
  current = next;
  next.reset();
  for(auto& po: powers) po.mods.clear();
  for(auto& [po, type, val]: current.mods) po->mods.emplace_back(type, val);
  if(h != max_hp())
    hp = randround(1. * hp * max_hp() / h);
  auto dat = get_dat();

  if(on_floor) on_floor_when = gframeid;

  fallthru = false;
   
  handle_powers(dat);

  if((on_floor || current.jump_control || wallhug) && !on_ice) {
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

void man::on_kill() {
  entity::on_kill();
  if(extra_life->flags & ACTIVE)
    addMessage("You die... Press [key:Extra Life] to revive.");
  else
    addMessage("You die... permanently. You will have to create a new character. Or just press [key:Extra Life] for a narrative cheat.");
  }

void add_revert(revert_stack& s, const reaction_t& what) {
  s.push_back(what);
  }

void revert_all(revert_stack& s) {
  while(!s.empty()) { s.back()(); s.pop_back(); }
  }

void man::launch_attack(power *p, int fac, boxfun f) {
  effects.emplace_back();
  auto& e = effects.back();
  e.p = p;
  e.attack_facing = fac;
  e.attack_when = gframeid;
  e.f = f;
  auto pb = f(0);
  auto bb = pixel_to_block(pb);
  for(auto& e: current_room->entities)
    if(e->existing && intersect(e->get_pixel_bbox(), pb)) {
      int sav = e->invinc_end;
      e->attacked((m.current.stats[stat::str] + 1) * 3 / 2);
      for(auto& [m, qty]: p->mods) {
        if(m == mod::burning) { e->invinc_end = sav; e->attacked(qty); }
        if(m == mod::freezing) { e->invinc_end = sav; e->attacked(qty); }
        }
      }
  for(int y=bb.miny; y<bb.maxy; y++)
  for(int x=bb.minx; x<bb.maxx; x++) {
    int b = current_room->at(x, y);
    if(b == wDoor) {
      current_room->replace_block_frev(x, y, wSmashedDoor);
      addMessage("You smash the door!");
      }
    for(auto& [m, qty]: p->mods) {
      if(m == mod::burning && b == wWoodWall) {
        current_room->replace_block_frev(x, y, wAir);
        addMessage("You burn the wall!");
        }
      if(m == mod::freezing && b == wWater) {
        current_room->replace_block_frev(x, y, wFrozen);
        addMessage("You freeze the water!");
        }
      }
    }
  }

}
