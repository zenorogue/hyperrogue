namespace rogue_unlike {

void handle_powers(data& d);

void man::act() {
  kino();

  auto dat = get_dat();

  coyote_time = next_coyote_time; next_coyote_time = 0;
  jump_control = next_jump_control; next_jump_control = 0;

  if(on_floor) on_floor_when = gframeid;

  fallthru = false;
   
  handle_powers(dat);

  if((on_floor || jump_control || wallhug) && !on_ice) {
    vel_x = dat.dx * dat.d * dat.modv * 2.5;
    }

  if(on_bounce) {
    vel_x += dat.dx * dat.d * dat.modv * 0.02;
    }

  if(!(on_floor && !dat.dx)) last_action = gframeid;

  if(dat.dx) facing = dat.dx;

  current_room->fov_from(where_x / block_x, where_y / block_y);
  }

}
