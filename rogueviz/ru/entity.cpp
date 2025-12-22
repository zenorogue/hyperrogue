namespace rogue_unlike {

man m;

bbox entity::get_pixel_bbox_at(xy p, ld scalex, ld scaley) {
  bbox b;
  double d = get_scale_at(p.y);
  ld dx = d * scalex;
  ld dy = d * scaley;
  double man_x = siz().x;
  double man_y = siz().y;
  b.minx = p.x - man_x * dx / 2;
  b.maxx = p.x + man_x * dx / 2 + 1;
  b.miny = p.y - man_y * dy / 2;
  b.maxy = p.y + man_y * dy / 2 + 1;
  if(ldebug) println(hlog, tie(man_x, man_y, d));
  return b;
  }

xy entity::get_precise_bbox_at(xy p, int mx, int my, ld scalex, ld scaley) {
  double d = get_scale_at(p.y);
  ld dx = d * scalex;
  ld dy = d * scaley;
  double man_x = siz().x;
  double man_y = siz().y;
  return xy { p.x + mx * man_x * dx / 2, p.y + my * man_y * dy / 2 };
  }

void entity::on_fountain() { hs(fountain_resetter); }

bool entity::visible(room *r) {
  auto bb = get_intersect(pixel_to_block(get_pixel_bbox()), room_bb);
  for(int y = bb.miny; y < bb.maxy; y++) for(int x = bb.minx; x < bb.maxx; x++) if(r->fov[y][x]) return true;
  return false;
  }

data entity::get_dat() {
  data dat;
  dat.d = get_scale();
  dat.modv = 60. / game_fps;
  dat.moda = dat.modv * dat.modv;
  dat.dx = 0;
  return dat;
  }

void entity::apply_grav() {

  if(non_hyperbolic) return apply_portal_grav();

  auto dat = get_dat();
  vel.y += dat.d * grav() * dat.moda * 16/9.;
  }

void entity::apply_vel() {
  int bx0 = floor(where.x / block_x);
  int by0 = floor(where.y / block_y);
  where += vel;

  int bx1 = floor(where.x / block_x);
  int by1 = floor(where.y / block_y);

  if(non_hyperbolic) {
    auto& loc = all_locations[by0][bx0];
    if(bx1 > bx0) {
      auto& loc1 = loc.get(0);
      where.x += block_x * (loc1.x - (loc.x + 1));
      where.y += block_y * (loc1.y - loc.y);
      }
    if(bx1 < bx0) {
      auto& loc1 = loc.get(2);
      where.x += block_x * (loc1.x - (loc.x - 1));
      where.y += block_x * (loc1.y - loc.y);
      }
    if(by1 < by0) {
      auto& loc1 = loc.get(1);
      where.x += block_x * (loc1.x - loc.x);
      where.y += block_x * (loc1.y - (loc.y - 1));
      }
    if(by1 > by0) {
      auto& loc1 = loc.get(3);
      where.x += block_x * (loc1.x - loc.x);
      where.y += block_x * (loc1.y - (loc.y + 1));
      }
    }

  // ld test_x, test_y;
  // tie(test_x, test_y) = from_hyper(h_at);
  
  /*println(hlog, tie(where_x, where_y), " TO ", h_at, " TO ", tie(test_x, test_y));
  exit(1); */

  if(true || !non_hyperbolic) {
    hyperpoint h_at = to_hyper(where);
    hyperpoint h_was = to_hyper(where - vel);
    hyperpoint h_willbe = rgpushxto0(h_at) * MirrorX * MirrorY * gpushxto0(h_at) * h_was;
    xy next = from_hyper(h_willbe);
    vel = next - where;
    }
  }

void entity::apply_walls() {
  int loopcount = 0;
  again:
  loopcount++;

  auto obb = pixel_to_block(get_pixel_bbox());
  auto nbb = pixel_to_block(get_pixel_bbox_at(where + vel));
  auto jbb = join(obb, nbb);

  flagtype blocking = (vel.y < 0 || fallthru) ? W_BLOCK : (W_BLOCK | W_PLATFORM);

  auto pain_effect = [&] {
    if(!hurt_by_spikes()) return false;
    spiked();
    vel.x = -vel.x; vel.y = -vel.y; apply_grav();
    return true;
    };

  auto allside = [this] (int x, int y, eWall b) {
    if(b == wRogueWallHidden && is_disarmer()) {
      current_room->replace_block_frev(x, y, wRogueWall);
      if(current_room->fov[y][x])
        addMessage("Your " + hal()->get_name() + " exposes a fake wall!");
      }
    };

  for(int x = obb.minx; x < obb.maxx; x++) for(int y = obb.maxy; y < jbb.maxy; y++) {
    eWall b = current_room->at(x, y);
    if(walls[b].flags & W_DOWNWARD) continue;
    allside(x, y, b);
    if(walls[b].flags & blocking) {
      if(walls[b].flags & W_BOUNCY) { vel.y = -vel.y; apply_grav(); apply_grav(); if(vel.y > 0) vel.y = 0; on_bounce = true; goto again; }
      on_floor = true;
      if(walls[b].flags & W_FROZEN) on_ice = true;
      vel.y /= 2;
      if(abs(vel.y) < 1e-6) vel.y = 0;
      if(burning()) {
        if(b == wWoodWall)
          current_room->replace_block_frev(x, y, wAir);
        else hit_wall();
        }
      if(freezing()) {
        if(b == wWater)
          current_room->replace_block_frev(x, y, wFrozen);
        else if(b != wFrozen) hit_wall();
        }
      if(pixel_to_block(get_pixel_bbox_at(where + vel)).maxy <= y) where.y += vel.y; 

      if(walls[b].flags & W_STABLE) is_stable = true;

      goto again;
      }
    if((walls[b].flags & W_PAIN) && pain_effect()) goto again;
    }

  for(int x = obb.minx; x < obb.maxx; x++) for(int y = jbb.miny; y < obb.miny; y++) {
    eWall b = current_room->at(x, y);
    allside(x, y, b);
    if(walls[b].flags & W_BLOCK) {
      if(walls[b].flags & W_BOUNCY) { vel.y = -vel.y; apply_grav(); apply_grav(); on_bounce = true; goto again; }
      vel.y /= 2;
      if(abs(vel.y) < 1e-6) vel.y = 0;
      if(pixel_to_block(get_pixel_bbox_at(where + vel)).miny > y) where.y += vel.y;
      if(burning()) {
        if(b == wWoodWall) current_room->replace_block_frev(x, y, wAir);
        }
      goto again;
      }
    if((walls[b].flags & W_PAIN) && pain_effect()) goto again;
    }

  if(!fallthru) for(int x = nbb.minx; x < nbb.maxx; x++) for(int y = jbb.maxy-1; y < jbb.maxy; y++) {
    eWall b = current_room->at(x, y);
    if(walls[b].flags & W_STAIRCASE) {
      on_floor = true;
      if(vel.y > 0) { vel.y = 0; goto again; }
      }
    }

  for(int x = obb.maxx; x < jbb.maxx; x++) for(int y = jbb.miny; y < jbb.maxy; y++) {
    eWall b = current_room->at(x, y);
    allside(x, y, b);
    if((walls[b].flags & W_DOWNWARD) && b == current_room->at(x-1, y)) continue;
    if(walls[b].flags & W_BLOCK) {
      if(walls[b].flags & W_BOUNCY) { vel.x = -vel.x; on_bounce = true; goto again; }
      if(freezing()) { hit_wall(); }
      if(burning()) {
        if(b == wWoodWall) current_room->replace_block_frev(x, y, wAir);
        else hit_wall();
        }
      vel.x = (vel.x - max<ld>(vel.y, 0)/10) / 2;
      wallhug = true;
      goto again;
      }
   if((walls[b].flags & W_PAIN) && pain_effect()) goto again;
   }
  
  for(int x = jbb.minx; x < obb.minx; x++) for(int y = jbb.miny; y < jbb.maxy; y++) {
    eWall b = current_room->at(x, y);
    allside(x, y, b);
    if((walls[b].flags & W_DOWNWARD) && b == current_room->at(x+1, y)) continue;
    if(walls[b].flags & W_BLOCK) {
      if(walls[b].flags & W_BOUNCY) { vel.x = -vel.x; on_bounce = true; goto again; }
      if(freezing()) { hit_wall(); }
      if(burning()) {
        if(b == wWoodWall) current_room->replace_block_frev(x, y, wAir);
        else hit_wall();
        }
      vel.x = (vel.x + max<ld>(vel.y, 0)/10) / 2;
      wallhug = true;
      goto again;
      }
    if((walls[b].flags & W_PAIN) && pain_effect()) goto again;
    }

  if(loopcount < 100) for(auto& e: current_room->entities) if(auto p = e->as_platform()) {
    auto opw = p->location_at(gframeid-1);
    auto npw = p->location_at(gframeid);

    xy screen_ctr(xctr, yctr);

    auto rmow = (where - opw) / get_scale_at(opw.y) + screen_ctr;
    auto rmnw = (where + vel - npw) / get_scale_at(npw.y) + screen_ctr;
    auto rvel = rmnw - rmow;

    auto eb = get_pixel_bbox_at(rmnw);
    auto pb = p->get_pixel_bbox_at(screen_ctr);

    if(intersect(pb, eb)) {
      auto zvel = (rmow - screen_ctr) * get_scale_at(npw.y) + npw - where;

      bool reset = false;

      if(p->platform_type() == wStaircase) {
        if(!fallthru) { on_floor = true; zero_vel = zvel; if(rvel.y > 0) rvel.y = 0; }
        }

      else if(intersect(pb, get_pixel_bbox_at(rmow))) { /* should not happen */ }

      else if(!intersect(pb, get_pixel_bbox_at(rmow + xy(rvel.x, 0))) && rvel.y > 0) {
        on_floor = true;
        rvel.y /= 2;
        if(abs(rvel.y) < 1e-6) rvel.y = 0;
        reset = true;
        zero_vel = zvel;
        }

      else if(!intersect(pb, get_pixel_bbox_at(rmow + xy(rvel.x, 0))) && rvel.y < 0) {
        rvel.y /= 2;
        if(abs(rvel.y) < 1e-6) rvel.y = 0;
        reset = true;
        zero_vel = zvel;
        }

      else {
        rvel.x = -rvel.x;
        reset = true;
        zero_vel = zvel;
        }

      vel = (rmow + rvel - screen_ctr) * get_scale_at(npw.y) + npw - where;
      if(reset) goto again;
      }
    }

  for(int x = jbb.minx; x < jbb.maxx; x++) for(int y = jbb.miny; y < jbb.maxy; y++) {
    eWall b = current_room->at(x, y);
    if(walls[b].flags & W_SLOPE) {

      bool left = walls[b].glyph[0] == '\\';

      // in big slopes, ignore one of the corners
      if(current_room->subwall(x, y) == (left ? 5 : 4)) continue;

      auto bad = [&] (xy p) {
        if(left)
          return -p.x + p.y >= -block_x * x + block_y * y && p.y >= block_y * y && p.x <= block_x * (x + 1);
        else
          return p.x + p.y >= block_x * x + block_y * y + block_x && p.y >= block_y * y && p.x >= block_x * x;
        };
      auto p = get_precise_bbox_at(where + vel, left ? -1 : 1, 1);

      if(loopcount > 200) {
        println(hlog, "[slope loop error]");
        continue;
        }

      if(bad(p)) {
        ld target_vy;
        target_vy = left ? vel.x : -vel.x;
        if(target_vy > 0) target_vy *= .9;
        if(vel.y > target_vy - 1e-6) vel.y = target_vy;
        else if(vel.y > target_vy + 1e-6) vel.y = (vel.y + target_vy) / 2;
        on_floor = true;
        if(walls[b].flags & W_STABLE) is_stable = true;
        goto again;
        }
      }
    }

  // println(hlog, format("%.20lf %.20lf %.20lf %.20lf L%d -> %.20lf %.20lf", where.x, where.y, vel.x, vel.y, loopcount, (where+vel).x, (where+vel).y));
  }

bool entity::stay_on_screen() {
  bool res = false;
  if(where.x < l_margin_at && vel.x < 0) vel.x = -vel.x, res = true;
  if(where.x > r_margin_at && vel.x > 0) vel.x = -vel.x, res = true;
  if(where.y < t_margin_at && vel.y < 0) vel.y = -vel.y, res = true;
  if(where.y > b_margin_at && vel.y > 0) vel.y = -vel.y, res = true;
  return res;
  }

void entity::kill_off_screen() {
  if(where.x < l_margin_at && vel.x < 0) existing = false;
  if(where.x > r_margin_at && vel.x > 0) existing = false;
  if(where.y < t_margin_at && vel.y < 0) existing = false;
  if(where.y > b_margin_at && vel.y > 0) existing = false;
  }

void entity::kino() {
  on_floor = false;
  on_ice = false;
  wallhug = false;
  on_bounce = false;
  is_stable = false;
  zero_vel = xy(0, 0);

  // ld modv = 60. / game_fps;

  apply_grav();
  apply_walls();
  apply_vel();
  apply_grav();

  gwhere += gvel;

  ld delta = 1/60.;
  auto z = (where - gwhere) * (gvel - vel);
  if(z.x + z.y < 0) delta *= 2;
  ld ndelta = 1-delta;
  gvel = ndelta * gvel + delta * (where - gwhere);
  }

void missile::act() {
  kino();
  if(where.x > screen_x || where.x < 0 || where.y < 0 || where.y > screen_y) destroyed = true;
  for(auto& e: current_room->entities) if(e->hit_by_missile(this) && intersect(get_pixel_bbox(), e->get_pixel_bbox()) && !hit_list.count(&*e)) {
    if(burning() && e->as_missile() && e->freezing()) { destroyed = true; e->destroyed = true; }
    if(freezing() && e->as_missile() && e->burning()) { destroyed = true; e->destroyed = true; }
    hit_list.insert(&*e);
    e->invinc_end = 0;
    e->attacked(power);
    }
  }

void npc_or_trader::act() {
  kino();
  if(gframeid > m.last_action + 300 && intersect(extend_all(get_pixel_bbox(), get_scale()*12), m.get_pixel_bbox()) && talk_on != m.last_action) {
    talk_on = m.last_action = gframeid;
    cmode = mode::menu;
    pushScreen([&] { cmode = mode::playing; popScreen(); });
    pushScreen([&] {
      dialog::init(name, color() >> 8);
      dialog::addHelp(parse_markup(text));
      dialog::addBreak(100);
      dialog::addBack();
      dialog::display();
      });
    }
  }

extern int gold_id;

string shopitem::glyph() { if(bought) return powers[gold_id].get_glyph(); else return item::glyph(); }
color_t shopitem::color() { if(bought) return powers[gold_id].get_color(); else return item::color(); }

void timed_orb::act() {
  if(gframeid > current_room->timed_orb_end) {
    walls[wTimeDoor].glyph = '+';
    walls[wTimeDoor].flags = W_BLOCK | W_BLOCKBIRD;
    }
  else {
    walls[wTimeDoor].glyph = '\'';
    walls[wTimeDoor].flags = W_TRANS;
    }
  if(intersect(get_pixel_bbox(), m.get_pixel_bbox()))
    current_room->timed_orb_end = gframeid + duration;
  }

void trader::act() {
  bool any_purchases = false;
  for(auto& e: current_room->entities) if(auto si = e->as_shopitem()) if(!si->existing) any_purchases = true;
  if(any_purchases) {
    walls[wShopDoor].glyph = '+';
    walls[wShopDoor].flags = W_BLOCK | W_BLOCKBIRD;
    }
  else {
    walls[wShopDoor].glyph = '\'';
    walls[wShopDoor].flags = W_TRANS;
    }

  if(any_purchases) talk_on = m.last_action;
  npc_or_trader::act();
  }

void boar::act() {
  stay_on_screen();
  kino();
  if(intersect(get_pixel_bbox(), m.get_pixel_bbox())) {
    int s = where.x < m.where.x ? -1 : 1;
    if(m.reduce_hp(15)) addMessage("The " + hal()->get_name() + " gores you!");
    auto dat = get_dat();
    auto mdat = m.get_dat();
    if(m.on_floor) m.vel.x = mdat.d * mdat.modv * -s * 1.5, m.vel.y = -mdat.d * mdat.modv * 2;
    if(on_floor) vel.x = dat.d * dat.modv * s * 1.5;
    }
  if(on_floor) {
    auto dat = get_dat();
    if(vel.x > 0) vel.x = max<ld>(vel.x - dat.d * dat.moda * 0.05, 0);
    if(vel.x < 0) vel.x = min<ld>(vel.x + dat.d * dat.moda * 0.05, 0);
    if(gframeid > invinc_end) {
      if(intersect(extend(get_pixel_bbox(), 60 * dat.d, 0, 0, 0), m.get_pixel_bbox())) vel.x -= dat.d * dat.moda * 0.2;
      if(intersect(extend(get_pixel_bbox(), 0, 60 * dat.d, 0, 0), m.get_pixel_bbox())) vel.x += dat.d * dat.moda * 0.2;
      }
    }
  }

void enemy::attacked(int dmg) {
  current_target = this;
  if(reduce_hp(dmg)) {
    if(!existing) addMessage("You kill the " + hal()->get_name() + "."); else addMessage("You hit the " + hal()->get_name() + ".");
    }
  }

void boar::attacked(int dmg) {
  enemy::attacked(dmg);
  auto dat = get_dat();
  int s = where.x < m.where.x ? -1 : 1;
  if(on_floor) vel.x = dat.d * dat.modv * s * 2, vel.y = -dat.d * dat.modv * 2.5;
  }

void frog::act() {
  stay_on_screen();
  kino();
  if(intersect(get_pixel_bbox(), m.get_pixel_bbox())) {
    int s = where.x < m.where.x ? -1 : 1;
    if(m.reduce_hp(20)) addMessage("The giant frog crushes you!");
    auto dat = get_dat();
    auto mdat = m.get_dat();
    if(m.on_floor) m.vel.x = mdat.d * mdat.modv * -s * 1.5, m.vel.y = -mdat.d * mdat.modv * 2;
    if(on_floor) vel.x = dat.d * dat.modv * s * 1.5;
    }
  if(on_floor) {
    vel.x = 0;
    if(gframeid > invinc_end) {
      auto dat = get_dat();
      if(jphase == 0 && gframeid >= jump_at && intersect(extend(get_pixel_bbox(), 100 * dat.d, 100 * dat.d, 20 * dat.d, 20 * dat.d), m.get_pixel_bbox())) {
        vel.y = -2 * dat.modv;
        jphase = 1;
        }
      else if(jphase == 1) {
        auto p = where;
        ld v = maxvel();
        array<xy, 2> fp;
        for(int i=0; i<2; i++) {
          where = p, vel.x = (i ? v : -v) * dat.modv, vel.y = -v * dat.modv;
          int it = 0;
          do { stay_on_screen(); kino(); } while(!on_floor && it++ < 5000);
          fp[i] = where;
          }
        ld dist = max(abs(fp[0].x - p.x), abs(fp[1].x - p.x));
        where = p;
        jphase = 2;
        println(hlog, tuple(fp[0].x, fp[0].y, fp[1].x, fp[1].y));
        vel.y = -v * dat.modv;
        if(dist == 0) return;
        ld il = (m.where.x - where.x) / dist;
        if(il > 1) il = 1 - (rand() % 100) / 1000;
        if(il < -1) il = -1 + (rand() % 100) / 1000;
        vel.x = il * v * dat.modv;
        }
      else if(jphase == 2) {
        jphase = 0; jump_at = gframeid + game_fps * (maxvel() > 4 ? 2 : 0) + rand() % game_fps;
        }
      }
    }
  }

void frog::attacked(int dmg) {
  enemy::attacked(dmg);
  auto dat = get_dat();
  int s = where.x < m.where.x ? -1 : 1;
  if(on_floor) vel.x = dat.d * dat.modv * s * 2, vel.y = -dat.d * dat.modv * 2.5;
  }

void ghost::act() {
  if(gframeid >= invinc_end && extra_invinc) gframeid += extra_invinc, extra_invinc = 0;
  hyperpoint g = to_hyper(where);
  hyperpoint h = to_hyper(m.where);
  ld d = hdist(g, h);
  ld angle = gframeid < invinc_end ? M_PI : d > 0.5 ? 90._deg : d > 0.05 ? 80._deg : 5._deg;
  ld gv = d > 0.04 && gframeid > invinc_end ? 0.2 : 0.1;
  if(flipped) angle = -angle;
  hyperpoint g1 = rgpushxto0(g) * rspintox(gpushxto0(g) * h) * spin(angle) * xpush0(gv / game_fps);
  vel = from_hyper(g1) - where;
  if(stay_on_screen()) flipped = !flipped;
  apply_vel();
  if(intersect(get_pixel_bbox(), m.get_pixel_bbox()) && gframeid > invinc_end) {
    invinc_end = gframeid + 200;
    if(m.reduce_hp(20)) addMessage("The " + get_name() + " passes through you!");
    }
  }

void snake::act() {
  stay_on_screen();
  kino();
  if(on_floor) {
    auto dat = get_dat();
    if(abs(vel.x) < 1e-6) {
      dir = -dir;
      }
    vel.x = zero_vel.x + dat.d * dat.modv * dir;
    }
  if(intersect(get_pixel_bbox(), m.get_pixel_bbox()) && gframeid > invinc_end) {
    if(m.reduce_hp(bite())) addMessage("The " + get_name() + " bites you!");
    }
  }

void snake::attacked(int dmg) {
  enemy::attacked(dmg);
  dir *= -1;
  if(where.x < m.where.x) vel.x = -abs(vel.x);
  if(where.x > m.where.x) vel.x = +abs(vel.x);
  }

void naga_warrior::act() {
  stay_on_screen();
  kino();
  if(on_floor) {
    auto dat = get_dat();
    if(abs(vel.x) < 1e-6) {
      dir = -dir;
      }
    vel.x = zero_vel.x + dat.d * dat.modv * dir;
    }
  if(gmod(gframeid, 100) == 0) {
    bbox b = get_pixel_bbox_at(xy{where.x + dir * dsiz().x, where.y});
    if(intersect(b, m.get_pixel_bbox()) && gframeid > invinc_end) {
      if(m.reduce_hp(chop())) addMessage("The " + get_name() + " chops you!");
      }
    }
  if(intersect(get_pixel_bbox(), m.get_pixel_bbox()) && gframeid > invinc_end) {
    if(m.reduce_hp(bite())) addMessage("The " + get_name() + " bites you!");
    }
  }

void naga_warrior::draw() {
  entity::draw();

  int t = gmod(gframeid, 100);
  if(t <= 50) {
    bbox b = get_pixel_bbox_at(xy{where.x + dir * (1-0.01 * t) * dsiz().x, where.y});
    auto col = 0xFFFFFFFF;
    auto& alpha = part(col, 0);
    alpha = max<int> (0, alpha - 5 * t);
    asciiletter(
      b.minx, b.miny,
      b.maxx, b.maxy,
      dir == -1 ? "(" : ")", col
      );
    }
  }

void naga_warrior::attacked(int dmg) {
  enemy::attacked(dmg); // do not reverse on being attacked!
  }

void hint::act() {
  bool cur = intersect(get_pixel_bbox(), m.get_pixel_bbox());
  if(gframeid < 300) cur = 0;
  if(cur && !state) {
    addMessage(parse_markup(hint_text));
    }
  state = cur;
  }

void avoid::act() {
  bool cur = intersect(get_pixel_bbox(), m.get_pixel_bbox());
  if(gframeid < 300) cur = 0;
  if(whom && cur && !state && !whom->avoided && whom->existing && !done) {
    whom->avoided = true; done = true; whom->score();
    addMessage(parse_markup(hint_text));
    }
  state = cur;
  }

xy ferris_platform::location_at(ld t) {
  return from_hyper(rgpushxto0(to_hyper(ctr)) * xspinpush0(t / game_fps + shift, radius));
  }

xy rope_platform::location_at(ld t) {
  return from_hyper(eupush(to_hyper(ctr)) * xspinpush0(sin(t / game_fps / period * TAU + shift) * max_swing, dist));
  }

void rope_platform::draw() {
  if(cmode == mode::editmap) {
    for(int a=-50; a<=50; a++) {
      auto h = from_hyper(eupush(to_hyper(ctr)) * xspinpush0(a/50. * max_swing, dist));
      curvepoint(eupush(h.x, h.y) * C0);
      }
    color_t col = color_t(dist * 0x41268517) | 0xFF;
    queuecurve(scrm, col, 0, PPR::LINE);
    }

  moving_platform::draw();
  }

xy pendulum_platform::location_at(ld t) {
  auto h1 = to_hyper(a);
  auto h2 = to_hyper(b);
  auto d = hdist(h1, h2);
  auto x = (1 - cos(t / game_fps * TAU / period)) / 2 * d;
  return from_hyper(rgpushxto0(h1) * rspintox(gpushxto0(h1) * h2) * xpush0(x));
  }

void moving_platform::draw() {
  double d = get_scale();
  auto wi = width();
  for(int w=0; w<wi; w++) {
    ld minx = where.x + siz().x * d * (((w+0.) / wi ) - 0.5);
    ld miny = where.y - siz().y * d / 2;
    ld maxx = where.x + siz().x * d * (((w+1.) / wi) - 0.5);
    ld maxy = where.y + siz().y * d / 2;
    asciiletter(minx, miny, maxx, maxy, glyph(), color());
    }
  }

void moving_platform::act() {
  where = location_at(gframeid);
  }

void entity::apply_walls_reflect() {
  int loopcount = 0;
  again:
  loopcount++;

  auto obb = pixel_to_block(get_pixel_bbox());
  auto nbb = pixel_to_block(get_pixel_bbox_at(where + vel));
  auto jbb = join(obb, nbb);

  flagtype blocking = (W_BLOCK | W_BLOCKBIRD);

  if(loopcount >= 100) return;

  for(int x = obb.minx; x < obb.maxx; x++) for(int y = obb.maxy; y < jbb.maxy; y++) {
    eWall b = current_room->at(x, y);
    if(walls[b].flags & blocking) {
      vel.y = -vel.y; goto again;
      }
    }

  for(int x = obb.minx; x < obb.maxx; x++) for(int y = jbb.miny; y < obb.miny; y++) {
    eWall b = current_room->at(x, y);
    if(walls[b].flags & blocking) {
      vel.y = -vel.y; goto again;
      }
    }

  for(int x = nbb.minx; x < nbb.maxx; x++) for(int y = jbb.miny; y < jbb.maxy; y++) {
    eWall b = current_room->at(x, y);
    if(walls[b].flags & blocking) {
      vel.x = -vel.x; goto again;
      }
    }

  for(int x = obb.maxx; x < jbb.maxx; x++) for(int y = jbb.miny; y < jbb.maxy; y++) {
    eWall b = current_room->at(x, y);
    if(walls[b].flags & blocking) {
      vel.x = -vel.x; goto again;
      }
    }
  }

void kestrel::act() {
  stay_on_screen();
  apply_walls_reflect();
  apply_vel();

  if(intersect(get_pixel_bbox(), m.get_pixel_bbox())) {
    if(m.reduce_hp(15)) addMessage("The " + hal()->get_name() + " claws you!");
    }
  }

void gridbug::act() {

  if(intersect(get_pixel_bbox(), m.get_pixel_bbox())) {
    if(m.reduce_hp(15)) addMessage("The " + hal()->get_name() + " zaps you!");
    }

  if(gframeid < next_move || !visible(current_room) || gframeid < invinc_end) return;
  auto gridbox = pixel_to_block(get_pixel_bbox());

  array<array<ld, room_x>, room_y> times;
  for(int y=0; y<room_y; y++)
  for(int x=0; x<room_x; x++) times[y][x] = HUGE_VAL;

  for(auto& e: current_room->entities) if(&*e != this && e->existing) {
    auto obox = pixel_to_block(e->get_pixel_bbox());
    for(int x=obox.minx; x<obox.maxx; x++)
    for(int y=obox.miny; y<obox.maxy; y++) times[y][x] = -10;
    }

  std::priority_queue<pair<ld, pair<int, int>>> q;

  auto visit = [&] (int x, int y, ld t) {
    q.push({-t, {x, y}});
    };

  auto manbox = pixel_to_block(m.get_pixel_bbox());
  for(int x=manbox.minx; x<manbox.maxx; x++)
  for(int y=manbox.miny; y<manbox.maxy; y++) visit(x, y, 0);

  int origx = (gridbox.minx+gridbox.maxx)/2;
  int origy = (gridbox.miny+gridbox.maxy)/2;
  int resx = origx, resy = origy;
  ld rest = HUGE_VAL;
  ld res_move_t = 0.1;

  while(!q.empty()) {
    auto [t, xy] = q.top(); q.pop();
    t = -t; auto [x, y] = xy;
    ld& memt = times[y][x];
    if(t > memt) continue;
    memt = t;

    auto move_to = [&] (int x1, int y1) {
      if(x1 < 0 || y1 < 0 || x1 >= room_x || y1 >= room_y) return;
      auto b = current_room->at(x1, y1);
      flagtype blocking = (W_BLOCK | W_BLOCKBIRD);
      if(walls[b].flags & blocking) return;
      ld d = hdist(to_hyper(block_x*(x+.5), block_y*(y+.5)), to_hyper(block_x*(x1+.5), block_y*(y+1.5))) * 10;
      if(x1 == origx && y1 == origy && rest > t+d) { rest = t+d; resx = x; resy = y; res_move_t = d; }
      visit(x1, y1, t+d);
      };

    move_to(x+1, y);
    move_to(x-1, y);
    move_to(x, y+1);
    move_to(x, y-1);
    }

  next_move = gframeid + game_fps * res_move_t;
  where = xy(block_x * (resx + .5), block_y * (resy + .5));
  }

void bat::act() {
  if(gframeid >= next_change && gframeid > invinc_end + 300) {
    next_change = gframeid + 300 + rand() % 300;
    int angle = rand() % 360;

    auto dat = get_dat();
    ld v = dat.d * dat.modv;

    vel.x = v * cos(angle);
    vel.y = v * sin(angle);
    }

  stay_on_screen();
  apply_walls_reflect();
  apply_vel();

  if(intersect(get_pixel_bbox(), m.get_pixel_bbox())) {
    if(m.reduce_hp(15)) addMessage("The " + hal()->get_name() + " bites you!");
    }
  }

void guineapig::act() {

  if(falling) {
    stay_on_screen();
    kino();
    if(on_floor) { ca = 0; falling = false; }
    return;
    }

  auto nonblocked = [this] (int angle, int mul) {
    auto w1 = from_hyper(eupush(to_hyper(where)) * xspinpush0(angle * 45._deg * spindir, pigvel * mul));

    auto obb = pixel_to_block(get_pixel_bbox());
    auto nbb = pixel_to_block(get_pixel_bbox_at(w1));
    auto jbb = join(obb, nbb);

    flagtype blocking = (W_BLOCK | W_BLOCKBIRD);

    bool ok = true;

    if(w1.x < 0 || w1.x > screen_x || w1.y < 0 || w1.y > screen_y) ok = false;

    for(int x = obb.minx; x < obb.maxx; x++) for(int y = obb.maxy; y < jbb.maxy; y++) {
      eWall b = current_room->at(x, y);
      if(walls[b].flags & blocking) ok = false;
      }

    for(int x = obb.minx; x < obb.maxx; x++) for(int y = jbb.miny; y < obb.miny; y++) {
      eWall b = current_room->at(x, y);
      if(walls[b].flags & blocking) ok = false;
      }

    for(int x = nbb.minx; x < nbb.maxx; x++) for(int y = jbb.miny; y < jbb.maxy; y++) {
      eWall b = current_room->at(x, y);
      if(walls[b].flags & blocking) ok = false;
      }

    for(int x = obb.maxx; x < jbb.maxx; x++) for(int y = jbb.miny; y < jbb.maxy; y++) {
      eWall b = current_room->at(x, y);
      if(walls[b].flags & blocking) ok = false;
      }

    return pair(w1, ok);
    };

  for(int s: {1, 2, 3}) {
    for(int i=0; i<8; i++) {
      if(!nonblocked(ca+i-3, s).second) {
        for(int j=i+1; j<i+8; j++) {
          if(nonblocked(ca+j-3, s).second) {
            where = nonblocked(ca = gmod(ca+j-3, 8), s).first;
            if(intersect(get_pixel_bbox(), m.get_pixel_bbox())) {
              if(m.reduce_hp(15)) {
                addMessage("The " + hal()->get_name() + " bites you!");
                spindir *= -1;
                }
              }
            return;
            }
          }
        }
      }
    }

  falling = true; vel = xy(0, 0);
  }

void guineapig::attacked(int dmg) {
  enemy::attacked(dmg);
  spindir *= -1;
  }

void vtrap::act() {
  auto v = vel;
  stay_on_screen();
  apply_walls_reflect();
  if(v.y * vel.y < 0) {
    auto dat = get_dat();
    vel.y = (rand() % 100 + 100) * (vel.y > 0 ? 1 : -1) * 0.005 * dat.modv * dat.d;
    }
  apply_vel();

  if(intersect(get_pixel_bbox(), m.get_pixel_bbox())) {
    if(m.reduce_hp(200)) addMessage("The " + hal()->get_name() + " zaps you!");
    }
  }

void kestrel::attacked(int dmg) {
  enemy::attacked(dmg);
  if(where.x < m.where.x) vel.x = -abs(vel.x);
  if(where.x > m.where.x) vel.x = +abs(vel.x);
  }

void bat::attacked(int dmg) {
  enemy::attacked(dmg);
  if(where.x < m.where.x) vel.x = -abs(vel.x);
  if(where.x > m.where.x) vel.x = +abs(vel.x);
  if(where.y < m.where.y) vel.y = -abs(vel.y);
  if(where.y > m.where.y) vel.y = +abs(vel.y);
  }

cat::cat() {
  col = hrand_elt(cat_colors);
  }

void disnake::act() {
  snake::act();
  kill_off_screen();
  for(auto& e: current_room->entities)
    if(e->hidden() && e->existing && intersect(e->get_pixel_bbox(), get_pixel_bbox())) {
      addMessage("Your hear a sound of the " + e->hal()->get_name() + " colliding with the " + hal()->get_name() + ".");
      e->existing = false;
      destroyed = true;
      }
  }

void icicle::act() {
  fallthru = true;
  if(fallframes.empty()) {
    while(isize(fallframes) < 9999) {
      fallframes.push_back(where);
      kino();
      if(on_floor) break;
      }
    where = respawn;
    }
  if(state == 0) {
    auto w = m.where;
    for(auto f: fallframes) {
      if(intersect(get_pixel_bbox_at(f), m.get_pixel_bbox_at(w))) state = 1;
      w += m.vel;
      }
    }
  if(state == 1) {
    kino();
    if(on_floor && vel.y > 0) existing = false;
    if(vel.y < 0) state = 2;
    }
  if(state == 2) {
    kino();
    if(vel.y >= 0) state = 0;
    }
  if(state != 0) {
    if(intersect(get_pixel_bbox(), m.get_pixel_bbox())) {
      if(m.reduce_hp(50)) addMessage("The " + hal()->get_name() + " falls on you!");
      }
    }
  }

}
