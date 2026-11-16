/* ru physics: water physics, and possibly other physics will be added later */

namespace rogue_unlike {

array<ld, room_y> scale_down, scale_up;

void room::physics_init() {
  for(int y=0; y<room_y; y++) {
    scale_down[y] = pow(get_scale_at((y + .5) * block_y), -2) / pow(get_scale_at((y + .5 + 1) * block_y), -2);
    scale_up[y] = pow(get_scale_at((y + .5) * block_y), -2) / pow(get_scale_at((y + .5 - 1) * block_y), -2);
    }

  for(int y=0; y<room_y; y++) for(int x=0; x<room_x; x++)
    water_pressure[y][x] = (at(x,y) == wWater) ? 1 : 0;
  }

void room::physics_act() {
  auto watery = [&] (int x, int y) { return among(at(x,y), wWater, wAir); };

  bool flip = (gframeid & 4);

  for(int y=0; y<room_y; y++) for(int x=0; x<room_x; x++) 
    current_y[y][x] = 0;

  for(int y=0; y<room_y; y++) for(int x=0; x<room_x; x++) 
    current_x[y][x] = 0;

  for(int it=0; it<11; it++) {
    auto old = water_pressure;

    for(int y=0; y<room_y; y++) for(int x1=0; x1<room_x; x1++) {
      int x = flip ? room_x-1-x1 : x1;
      auto b = at(x,y);
      if(among(b, wWater, wAir)) {
        if(old[y][x]) {
          auto& w = water_pressure[y][x];
          auto o = old[y][x];
          auto clamp = [&] (ld z) { return min(z, w); };
          auto pressure = o > 1 ? (o-1) / 4.1 : 0;
          auto flow = o / 1000. + 1e-4;
          if(watery(x, y+1)) {
            auto q = clamp(flow + pressure);
            water_pressure[y][x] -= q;
            water_pressure[y+1][x] += q * scale_down[y];
            current_y[y+1][x] += q;
            current_y[y][x] += q;
            }
          if(flip && watery(x+1, y)) {
            auto q = clamp(flow + pressure);
            water_pressure[y][x] -= q;
            water_pressure[y][x+1] += q;
            current_x[y][x+1] += q;
            current_x[y][x] += q;
            }
          if(watery(x-1, y)) {
            auto q = clamp(flow + pressure);
            water_pressure[y][x] -= q;
            water_pressure[y][x-1] += q;
            current_x[y][x-1] += -q;
            current_x[y][x] += -q;
            }
          if(!flip && watery(x+1, y)) {
            auto q = clamp(flow + pressure);
            water_pressure[y][x] -= q;
            water_pressure[y][x+1] += q;
            current_x[y][x+1] += q;
            current_x[y][x] += q;
            }
          if(o > 1 && watery(x, y-1)) {
            auto q = clamp(pressure);
            water_pressure[y][x] -= q;
            water_pressure[y-1][x] += q * scale_up[y];
            current_y[y-1][x] -= q;
            current_y[y][x] -= q;
            }
          }
        }
      }
    }

  for(int y=0; y<room_y; y++) for(int x=0; x<room_x; x++) {
    auto b = at(x,y);
    if(among(b, wWater, wAir)) {
      if(b == wWater && water_pressure[y][x] == 0) replace_block(x, y, wAir);
      if(b == wAir && water_pressure[y][x] > 0) replace_block(x, y, wWater);
      }
    }
  }

  }
