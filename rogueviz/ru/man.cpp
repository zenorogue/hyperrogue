namespace rogue_unlike {

void handle_powers(data& d);

void man::act() {
  kino();

  data dat;
  dat.d = get_scale();
  dat.modv = 60. / game_fps;
  dat.moda = dat.modv * dat.modv;
  dat.dx = 0;  

  fallthru = false;
   
  handle_powers(dat);

  if(on_floor && !on_ice) {
    vel_x = dat.dx * dat.d * dat.modv * 2.5;
    }
  else {
    vel_x += dat.dx * dat.d * .05 * dat.moda;
    }

  if(dat.dx) facing = dat.dx;

  current_room->fov_from(where_x / block_x, where_y / block_y);
  }

}
