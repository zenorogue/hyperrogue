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
  hallucinating = false;
  mods.clear();
  }

man::man() {
  id = "Alchemist";
  facing = 1; attack_facing = 1;
  for(auto s: allstats) base_stats[s] = 10;
  next.reset(); current.reset();
  hs(fountain_resetter);
  }

void man::hs(stater& s) {
  auto& s1 = s.only_full();
  s1.act("facing", facing, 1)
   .act("attack_facing", attack_facing, 1)
   .act("attack_when", attack_when, 0)
   .act("on_floor_when", on_floor_when, 0)
   .act("xp", experience, 0)
   .act("last_action", last_action, 0);
  sact(s1, "hair", hair);
  sact(s1, "eyes", eye);
  string z = unspace(backstory);
  s1.act("backstory", z, "");
  backstory = respace(z);
  int prof = (int) profession; s1.act("profession", prof, -1); profession = (stat) prof;
  for(auto st: allstats) s1.act(statinfos[st].name, base_stats[st], 10);

  auto sdata = [&s1] (statdata& sd, string prefix) {
    for(auto st: allstats) s1.act(prefix + statinfos[st].name, sd.stats[st], 10);
    s1.act(prefix + "jump_control", sd.jump_control, 0);
    s1.act(prefix + "coyote_time", sd.coyote_time, 0);
    s1.act(prefix + "hallucinating", sd.hallucinating, 0);
    s1.act(prefix + "detect_area", sd.detect_area, 0);
    s1.act(prefix + "detect_cross", sd.detect_cross, 0);
    s1.act(prefix + "rough_detect", sd.rough_detect, 0);
    };

  sdata(current, "curr.");
  sdata(next, "next.");

  entity::hs(s);
  s.act("protection", protection, 0);
  s.act("vampire", vampire, 0);
  s.act("healbubble", healbubble, 0);
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

  if(next.hallucinating && !current.hallucinating)
    prepare_hallucination();

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

bool man::reduce_hp(int x) {
  if(protection && gframeid >= invinc_end) {
    ld fraction = 1 - 100 / (protection + 100);
    int take = ceil(x * fraction);
    if(take > protection) take = protection;
    protection -= take;
    x -= protection;
    }
  if(healbubble && gframeid >= invinc_end) {
    int take = x;
    if(take > healbubble) take = healbubble;
    healbubble -= take;
    auto d = m.get_dat();
    auto mi = std::make_unique<healthbubble>();
    mi->id = "HEALBUBBLE";
    ld r = (rand() % 360) * degree;
    mi->hs(fountain_resetter);
    mi->power = take * 2;
    mi->where = m.where;
    mi->vel = { cos(r) * d.modv * 3, sin(r) * d.modv * 3 };
    mi->invinc_end = gframeid + 300;
    new_entities.emplace_back(std::move(mi));
    }
  return entity::reduce_hp(x);
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
  e.attack_facing = fac;
  e.attack_when = gframeid;
  e.f = f;
  auto pb = f(0);
  auto bb = pixel_to_block(pb);
  for(auto& e: current_room->entities)
    if(e->existing && intersect(e->get_pixel_bbox(), pb)) {
      int sav = e->invinc_end;
      int dam = (m.current.stats[stat::str] + 1) * 3 / 2;
      e->attacked(dam);
      for(auto& [md, qty]: p->mods) {
        if(md == mod::burning) { e->invinc_end = sav; e->attacked(qty); }
        if(md == mod::freezing) { e->invinc_end = sav; e->attacked(qty); }
        if(md == mod::vampire) {
          int dam1 = min(dam, m.vampire);
          hp += dam1; m.vampire -= dam1;
          }
        if(md == mod::disarming && e->hidden()) {
          e->existing = false;
          addMessage("You have disarmed a "+e->hal()->get_name()+".");
          }
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
      if(m == mod::disarming && b == wRogueWallHidden) {
        current_room->replace_block_frev(x, y, wRogueWall);
        addMessage("You open a secret passage!");
        }
      }
    }
  }

}
