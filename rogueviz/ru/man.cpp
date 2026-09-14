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
  for(auto& r: rooms) for(auto& e: r.second.entities) e->on_fountain();
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
  detect_area = 0;
  detect_cross = 0;
  rough_detect = 0;

  heavy_armor = 0;
  spikes = 0;
  max_mageshield = 0;
  stealth_bonus = 0;
  dodge_value = 0;
  hallucinating = false;
  mods.clear();
  on_hit.clear();
  status_strings.clear();

  jump_power = 1;
  gravity_value = 1;
  extra_jumps = 0;
  boost_max = 0;
  }

man::man() {
  id = "Alchemist";
  facing = 1;
  for(auto s: allstats) base_stats[s] = 10;
  next.reset(); current.reset();
  hs(fountain_resetter);
  use_next_turn = nullptr;
  }

void man::hs(stater& s) {
  auto& s1 = s.only_full();
  s1.act("facing", facing, 1)
   .act("on_floor_when", on_floor_when, 0)
   .act("xp", experience, 0)
   .act("last_action", last_action, 0)
   .act("gameseed", gameseed, 0)
   .act("dresstime", dresstime, 0)
   .act("current_mageshield", current_mageshield, 0)
   .act("mageshield_recharge_time", mageshield_recharge_time, 0);
  sact(s1, "hair", hair);
  sact(s1, "eyes", eye);
  string z = unspace(backstory);
  s1.act("backstory", z, "");
  backstory = respace(z);
  int prof = (int) profession; s1.act("profession", prof, -1); profession = (stat) prof;
  for(auto st: allstats) s1.act(statinfos[st].name, base_stats[st], 10);

  s1.act("jumps_used", jumps_used, 0);
  s1.act("boost_left", boost_left, 0);

  auto sdata = [&s1] (statdata& sd, string prefix) {
    for(auto st: allstats) s1.act(prefix + statinfos[st].name, sd.stats[st], 10);
    s1.act(prefix + "jump_control", sd.jump_control, 0);
    s1.act(prefix + "coyote_time", sd.coyote_time, 0);
    s1.act(prefix + "hallucinating", sd.hallucinating, 0);
    s1.act(prefix + "detect_area", sd.detect_area, 0);
    s1.act(prefix + "detect_cross", sd.detect_cross, 0);
    s1.act(prefix + "rough_detect", sd.rough_detect, 0);

    s1.act(prefix + "heavy_armor", sd.heavy_armor, 0);
    s1.act(prefix + "spikes", sd.spikes, 0);
    s1.act(prefix + "max_mageshield", sd.max_mageshield, 0);
    s1.act(prefix + "stealth_bonus", sd.stealth_bonus, 0);
    s1.act(prefix + "dodge_value", sd.dodge_value, 0);

    s1.act(prefix + "jump_power", sd.jump_power, 1);
    s1.act(prefix + "gravity_value", sd.gravity_value, 1);
    s1.act(prefix + "extra_jumps", sd.extra_jumps, 0);
    s1.act(prefix + "boost_max", sd.boost_max, 0);
    };

  sdata(current, "curr.");
  sdata(next, "next.");

  entity::hs(s);
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
  for(auto& md: current.mods) md.wpn->mods.emplace_back(md);
  if(h != max_hp())
    hp = randround(1. * hp * max_hp() / h);
  auto dat = get_dat();

  if(dresstime) {
    dresstime--;
    if(dresstime == 0) { addMessage("You finish redressing."); mageshield_recharge_time = game_fps * 5; }
    }

  if(current_mageshield > current.max_mageshield) current.max_mageshield = 0;

  if(mageshield_recharge_time) mageshield_recharge_time--;
  else if(current_mageshield < current.max_mageshield) current_mageshield++;

  if(on_floor) on_floor_when = gframeid;

  fallthru = false;
   
  handle_powers(dat);

  if(next.hallucinating && !current.hallucinating)
    prepare_hallucination();

  if(on_floor || current.jump_control || wallhug) {
    if(on_ice == 0) {
      vel.x = zero_vel.x + dat.dx * dat.d * dat.modv * 2.5;
      }
    if(on_ice == 1) {
      vel.x = lerp(vel.x, zero_vel.x + dat.dx * dat.d * dat.modv * 2.5, 10. / game_fps);
      }
    }

  if(on_bounce) {
    vel.x += dat.dx * dat.d * dat.modv * 0.02;
    }

  if(!(on_floor && !dat.dx)) last_action = gframeid;

  if(dat.dx) facing = dat.dx;

  current_room->fov_from(where.x / block_x, where.y / block_y);

  check_fountains();
  }

bool man::reduce_hp(int x, entity *attacker, flagtype flags) {
  if(m.current.dodge_value > 0 && rand() % 100 < m.current.dodge_value && !(flags & NON_DODGEABLE)) { x = 1; }
  if(m.current_mageshield > 0) { int q = min(m.current_mageshield, x); x -= q; m.current_mageshield -= q; m.mageshield_recharge_time = game_fps * 5; }
  if(m.current.heavy_armor > 0) { x = max((x+4)/5, x - m.current.heavy_armor); }
  if(attacker && m.current.spikes > 0 && !(flags & SPIKES)) attacker->reduce_hp(m.current.spikes, this, SPIKES);
  if(gframeid >= invinc_end)
    for(auto& f: m.current.on_hit)
      f(x);
  return entity::reduce_hp(x, attacker, flags);
  }

bool man::can_see(entity& e) {
  if(m.current.detect_area) {
    ld d = hdist(to_hyper(m.where), to_hyper(e.where));
    if(d < inverse_wvolarea_auto(m.current.detect_area)) return true;
    }
  if(m.current.detect_cross) {
    array<int, 4> ar;
    transmatrix T = iso_inverse(eupush(to_hyper(m.where)));
    auto bb = e.get_pixel_bbox();
    for(int u=0; u<4; u++) {
      xy vertex = { ld((u&1) ? bb.minx : bb.maxx), ld((u&2) ? bb.miny : bb.maxy) };
      hyperpoint h = T * to_hyper(vertex);
      ar[u] = (h[0] > 0 ? 1 : 0) + (h[1] > 0 ? 2 : 0);
      if(hdist0(h) > m.next.detect_cross) ar[u] = 4;
      }
    if(ar[0] != ar[1] || ar[0] != ar[2] || ar[0] != ar[3]) return true;
    }
  return false;
  }

void man::on_kill() {
  entity::on_kill();
  if(extra_life->flags & ACTIVE)
    addMessage(parse_markup("You die... Press [key:Extra Life] to revive."));
  else
    addMessage(parse_markup("You die... permanently. You will have to create a new character. Or just press [key:Extra Life] for a narrative cheat."));
  }

void add_revert(revert_stack& s, const revert_type& what) {
  s.push_back(what);
  }

void revert_all(revert_stack& s) {
  while(!s.empty()) { revert(s.back()); s.pop_back(); }
  }

void man::launch_attack(power *p, int fac, boxfun f) {
  effects.emplace_back();
  auto& e = effects.back();
  e.p = p;
  e.facing = fac;
  e.when = gframeid;
  e.f = f;
  e.length = 50;
  e.cf = [&] (color_t col, int t) {
    auto& alpha = part(col, 0);
    alpha = max<int> (0, alpha - 5 * t);
    };

  auto pb = f(0);
  auto bb = pixel_to_block(pb);
  for(auto& e: current_room->entities)
    if(e->existing && intersect(e->get_pixel_bbox(), pb)) {
      int sav = e->invinc_end;
      int dam = (m.current.stats[stat::str] + 1) * 3 / 2;
      e->attacked(dam, p);
      for(auto& md: p->mods) md.action(&*e, dam, sav);
      }
  for(int y=bb.miny; y<bb.maxy; y++)
  for(int x=bb.minx; x<bb.maxx; x++) {
    int b = current_room->at(x, y);
    if(b == wWeakWall && (p->flags & WEAPON_AXE)) {
      current_room->replace_block_frev(x, y, wSmashedDoor);
      addMessage("You smash the wall!");
      }
    if(b == wDoor && (p->flags & WEAPON_AXE)) {
      current_room->replace_block_frev(x, y, wSmashedDoor);
      addMessage("You SMASH the door!");
      }
    if(b == wDoor) {
      current_room->replace_block_frev(x, y, wSmashedDoor);
      addMessage("You smash the door!");
      }
    for(auto& md: p->mods) md.map_action(x, y);
    }
  }

void man::handle_morph(entity *m) {
  auto pre_size = get_pixel_bbox();
  swap(m, morphed);
  auto at = where;
  if(on_floor) {
    do {
      where.y += 1;
      }
    while(get_pixel_bbox().maxy < pre_size.maxy);
    do {
      where.y -= 1;
      }
    while(get_pixel_bbox().maxy > pre_size.maxy);
    }
  println(hlog, "where moved from ", at.y, " to ", where.y);
  auto bb = pixel_to_block(get_pixel_bbox());
  println(hlog, "bblock is ", bb);
  bool ok = true;
  for(int x = bb.minx; x < bb.maxx; x++) for(int y = bb.miny; y < bb.maxy; y++) {
    eWall b = current_room->at(x, y);
    if(walls[b].flags & W_BLOCK) ok = false;
    }
  for(auto& e: current_room->entities) if(auto p = e->as_platform()) {
    auto pb = p->get_pixel_bbox();
    if(intersect(pb, bb)) ok = false;
    }
  if(ok) {
    if(morphed) {
      string adj = morphed->id == "capy" ? "lovely " : "";
      addMessage("You morph into a " + adj + morphed->get_name() + "!");
      }
    else
      addMessage("You morph back into a human.");
    }
  else {
    swap(m, morphed); where = at;
    addMessage("Not enough space here to morph!");
    }
  if(m) delete m;
  }

}
