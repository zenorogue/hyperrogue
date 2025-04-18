// an experiment on potential-correct gravity

namespace rogue_unlike {

bool gravision;

array<array<location, 256>, 256> all_locations;

void entity::apply_portal_grav() {
  ld modv = 60. / game_fps;
  ld moda = modv * modv;
  auto d = get_scale();
  int bx0 = floor(where_x / block_x);
  int by0 = floor(where_y / block_y);
  auto& loc = all_locations[by0][bx0];
  auto px = (loc.get(0).potential - loc.get(2).potential) * 255 / 2;
  auto py = (loc.get(3).potential - loc.get(1).potential) * 255 / 2;
  vel_x += d * grav() * moda * px;
  vel_y += d * grav() * moda * py;  
  }

void load_nonhyperbolic() {
  enable();
  non_hyperbolic = true;
  one_room = true;
  fhstream f("rogueviz/ru/rumap.txt", "rt");
  vector<string> rumap;
  for(int y=0; y<room_y; y++) rumap.push_back(scan<string>(f));
  println(hlog, rumap);
   
  auto& cr = *current_room;
  for(int y=0; y<room_y; y++)
  for(int x=0; x<room_x; x++)
    cr.block_at[y][x] =
      rumap[y][x] == '#' ? 8 :
      rumap[y][x] == '-' ? 12*8 :
      rumap[y][x] == '.' ? 0 :
      rumap[y][x] >= 'a' && rumap[y][x] <= 'z' ? 10*8 :
      rumap[y][x] >= 'A' && rumap[y][x] <= 'Z' ? 11*8 :
      1;

  for(int y=0; y<room_y; y++)
  for(int x=0; x<room_x; x++) cr.fov[y][x] = true;

  actual_view_transform = Id;

  map<char, location*> where_is;
  for(int y=0; y<room_y; y++)
  for(int x=0; x<room_x; x++)
    where_is[rumap[y][x]] = &all_locations[y][x];

  for(int y=0; y<256; y++)
  for(int x=0; x<256; x++) {
    auto& loc = all_locations[y][x];
    loc.x = x;
    loc.y = y;
    loc.fixed = false;
    loc.ch = (y < room_y && x < room_x) ? rumap[y][x] : 0;
    loc.neighbors[0] = &all_locations[y][(x+1)&255];
    loc.neighbors[1] = &all_locations[(y-1)&255][x];
    loc.neighbors[2] = &all_locations[y][(x-1)&255];
    loc.neighbors[3] = &all_locations[(y+1)&255][x];
    }

  for(int y=0; y<256; y++)
  for(int x=0; x<256; x++) {
    auto& loc = all_locations[y][x];
    if(in_range(loc.get(1).ch, 'a', 'z')) {
      auto& up_below = loc;
      auto& up_above = loc.get(1);
      auto& down_below = where_is[loc.get(1).ch ^ 32][0];
      auto& down_above = down_below.get(1);
      swap(up_below.neighbors[1], down_below.neighbors[1]);
      swap(up_above.neighbors[3], down_above.neighbors[3]);
      }
    }

  for(int x=0; x<256; x++) for(int y=0; y<256; y++) {
    int flip_y = (y - room_y/2 + 128) & 255;
    auto& loc = all_locations[y][x];
    loc.potential = flip_y / 255.;
    if(flip_y == 0 || flip_y == 255) loc.fixed = true;
    }

  int it = 0;
  while(true) {
    ld err = 0;
    for(int y=0; y<256; y++) for(int x=0; x<256; x++) {
      auto& loc = all_locations[y][x];
      if(loc.fixed) continue;
      ld pot = (loc.get(0).potential + loc.get(1).potential + loc.get(2).potential + loc.get(3).potential) / 4;
      err += abs(pot - loc.potential);
      loc.potential = pot;
      }
    it++;
    if(err < 1e-2) break;
    }
  }

auto portal_chk = arg::add3("-nonh", load_nonhyperbolic);
}
