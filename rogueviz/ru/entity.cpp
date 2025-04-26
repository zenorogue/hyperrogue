namespace rogue_unlike {

int gframeid = 0;

man m;

bbox entity::get_pixel_bbox_at(double x, double y) {
  bbox b;
  double d = get_scale_at(y);
  double man_x = sx();
  double man_y = sy();
  b.minx = x - man_x * d / 2;
  b.maxx = x + man_x * d / 2 + 1;
  b.miny = y - man_y * d / 2;
  b.maxy = y + man_y * d / 2 + 1;
  return b;
  }

bool entity::visible(room *r) {
  auto bb = get_intersect(pixel_to_block(get_pixel_bbox()), room_bb);
  for(int y = bb.miny; y < bb.maxy; y++) for(int x = bb.minx; x < bb.maxx; x++) if(r->fov[y][x]) return true;
  return false;
  }

void entity::apply_grav() {

  if(non_hyperbolic) return apply_portal_grav();

  ld modv = 80. / game_fps;
  ld moda = modv * modv;
  auto d = get_scale();

  vel_y += d * grav() * moda;
  }

void entity::kino() {
  on_floor = false;
  on_ice = false;
  wallhug = false;
  on_bounce = false;

  // ld modv = 60. / game_fps;

  apply_grav();
  again:

  auto obb = pixel_to_block(get_pixel_bbox());
  auto nbb = pixel_to_block(get_pixel_bbox_at(where_x + vel_x, where_y + vel_y));
  auto jbb = join(obb, nbb);

  flagtype blocking = (vel_y < 0 || fallthru) ? W_BLOCK : (W_BLOCK | W_PLATFORM);

  for(int x = obb.minx; x < obb.maxx; x++) for(int y = obb.maxy; y < jbb.maxy; y++) {
    eWall b = current_room->at(x, y);
    if(walls[b].flags & blocking) {
      if(walls[b].flags & W_BOUNCY) { vel_y = -vel_y; on_bounce = true; goto again; }
      on_floor = true;
      if(walls[b].flags & W_FROZEN) on_ice = true;
      vel_y /= 2;
      if(abs(vel_y) < 1e-6) vel_y = 0;
      if(freezing()) {
        if(b == wWater) current_room->replace_block(x, y, wFrozen);
        else if(b != wFrozen) hit_wall();
        }
      if(pixel_to_block(get_pixel_bbox_at(where_x + vel_x, where_y + vel_y)).maxy <= y) where_y += vel_y; 
      goto again;
      }
    }

  for(int x = obb.minx; x < obb.maxx; x++) for(int y = jbb.miny; y < obb.miny; y++) {
    eWall b = current_room->at(x, y);
    if(walls[b].flags & W_BLOCK) {
      vel_y /= 2;
      if(abs(vel_y) < 1e-6) vel_y = 0;
      if(pixel_to_block(get_pixel_bbox_at(where_x + vel_x, where_y + vel_y)).miny > y) where_y += vel_y; 
      goto again;
      }
    }

  if(!fallthru) for(int x = nbb.minx; x < nbb.maxx; x++) for(int y = jbb.maxy-1; y < jbb.maxy; y++) {
    eWall b = current_room->at(x, y);
    if(walls[b].flags & W_STAIRCASE) {
      on_floor = true;
      if(vel_y > 0) { vel_y = 0; goto again; }
      }
    }

  for(int x = obb.maxx; x < jbb.maxx; x++) for(int y = jbb.miny; y < jbb.maxy; y++) {
    eWall b = current_room->at(x, y);
    if(b == wFountain0) {
      current_room->replace_block(x, y, wFountain1);
      addMessage("A magic fountain! You feel safe and refill your potions.");
      }
    if(walls[b].flags & W_BLOCK) {
      if(freezing()) { hit_wall(); }
      vel_x = (vel_x - max<ld>(vel_y, 0)/10) / 2;
      wallhug = true;
      goto again;
      }
   }
  
  for(int x = jbb.minx; x < obb.minx; x++) for(int y = jbb.miny; y < jbb.maxy; y++) {
    eWall b = current_room->at(x, y);
    if(b == wFountain0) {
      current_room->replace_block(x, y, wFountain1);
      addMessage("A magic fountain! You feel safe and refill your potions.");
      }
    if(walls[b].flags & W_BLOCK) {
      if(freezing()) { hit_wall(); }
      vel_x = (vel_x + max<ld>(vel_y, 0)/10) / 2;
      wallhug = true;
      goto again;
      }
    }
  
  int bx0 = floor(where_x / block_x);
  int by0 = floor(where_y / block_y);
  where_x += vel_x;
  where_y += vel_y;

  int bx1 = floor(where_x / block_x);
  int by1 = floor(where_y / block_y);

  if(non_hyperbolic) {
    auto& loc = all_locations[by0][bx0];
    if(bx1 > bx0) {
      auto& loc1 = loc.get(0);
      where_x += block_x * (loc1.x - (loc.x + 1));
      where_y += block_y * (loc1.y - loc.y);
      }
    if(bx1 < bx0) {
      auto& loc1 = loc.get(2);
      where_x += block_x * (loc1.x - (loc.x - 1));
      where_y += block_x * (loc1.y - loc.y);
      }
    if(by1 < by0) {
      auto& loc1 = loc.get(1);
      where_x += block_x * (loc1.x - loc.x);
      where_y += block_x * (loc1.y - (loc.y - 1));
      }
    if(by1 > by0) {
      auto& loc1 = loc.get(3);
      where_x += block_x * (loc1.x - loc.x);
      where_y += block_x * (loc1.y - (loc.y + 1));
      }
    }

  // ld test_x, test_y;
  // tie(test_x, test_y) = from_hyper(h_at);
  
  /*println(hlog, tie(where_x, where_y), " TO ", h_at, " TO ", tie(test_x, test_y));
  exit(1); */

  if(true || !non_hyperbolic) {  
    hyperpoint h_at = to_hyper(where_x, where_y);
    hyperpoint h_was = to_hyper(where_x - vel_x, where_y - vel_y);
    hyperpoint h_willbe = rgpushxto0(h_at) * MirrorX * MirrorY * gpushxto0(h_at) * h_was;
    ld next_x, next_y;
    tie(next_x, next_y) = from_hyper(h_willbe);
    vel_x = next_x - where_x;
    vel_y = next_y - where_y;
    }

  apply_grav();

  gwhere_x += gvel_x;
  gwhere_y += gvel_y;

  ld delta = 1/60.;
  if((where_x - gwhere_x) * (gvel_x - vel_x) + (where_y - gwhere_y) * (gvel_y - vel_y) < 0) delta *= 2;
  ld ndelta = 1-delta;
  gvel_x = ndelta * gvel_x + delta * (where_x - gwhere_x);
  gvel_y = ndelta * gvel_y + delta * (where_y - gwhere_y);
  }

void missile::act() {
  kino();
  if(where_x > screen_x || where_x < 0 || where_y < 0 || where_y > screen_y) destroyed = true;
  }

void npc::act() {
  kino();
  if(gframeid > m.last_action + 300 && intersect(get_pixel_bbox(), m.get_pixel_bbox()) && talk_on != m.last_action) {
    talk_on = m.last_action = gframeid;
    cmode = mode::talking;
    pushScreen([&] { cmode = mode::playing; popScreen(); });
    pushScreen([&] {
      dialog::init(name, col >> 8);
      dialog::addInfo(text);
      dialog::addBreak(100);
      dialog::addBack();
      dialog::display();
      });
    }
  }

}
