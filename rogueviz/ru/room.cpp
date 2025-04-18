namespace rogue_unlike {

void room::place_block_full(int x, int y, int b) {
  need_rerender = true;
  indenter ind(2);
  infile = true;

  if(x < 0 || y < 0 || x >= room_x || y >= room_y) return;
  if(b < 8) b = 0;
  if((b & 4) && (x&1) != (b&1)) x--;
  if((b & 4) && (2*(y&1)) != (b&2)) y--;
  if(y < t_margin && (b&4)) return;
  if(y > room_y - b_margin && !(b&4) && b) return;
  if(block_at[y][x] == b) return;
  
  block_at[y][x] = b;
  if(b & 4) {
    place_block_full(x^1, y, b^1);
    place_block_full(x, y^1, b^2);
    }
  else {
    if(block_at[y][x^1] & 4)
      place_block_full(x^1, y, 0);
    if(block_at[y^1][x] & 4)
      place_block_full(x, y^1, 0);
    }
  
  auto& nonh = non_hyperbolic;
  if(x < lr_margin)
    get_adjacent(this, nonh ? 2 : 2)->place_block_full(x + room_x - lr_margin, y, b);
  if(x >= room_x - lr_margin)
    get_adjacent(this, nonh ? 0 : 4)->place_block_full(x - room_x + lr_margin, y, b);
  if(y < t_margin) {
    if(nonh) {
      get_adjacent(this, 1)->place_block_full(x, y - room_y + t_margin, b);
      }
    else if(x < room_x / 2) {
      get_adjacent(this, 1)->place_block_full((x - lr_margin/2) * 2 + lr_margin/2, room_y - b_margin + 2*y, b^4);
      }
    else {
      get_adjacent(this, 0)->place_block_full((x - room_x/2) * 2 + lr_margin/2, room_y - b_margin + 2*y, b^4);
      }
    }
  
  if(y >= room_y - b_margin && !nonh) {
    bool r = is_right(this);
    x -= lr_margin/2;
    x /= 2;
    x += (r ? room_x/2 : lr_margin/2);
    y = y - room_y + b_margin;
    y /= 2;
    b &= ~3;
    get_adjacent(this, 3)->place_block_full(x, y, b ^ 4);
    }

  if(y >= room_y - t_margin && nonh) {
    get_adjacent(this, 1)->place_block_full(x, y + room_y - t_margin, b);
    }
  }

void room::generate() {
  clear();
  place_small(0, 0);
  place_small(room_x/2-1, 0);
  place_small(room_x/2, 0);
  place_small(room_x-1, 0);
  place_big(0, room_y-2);
  place_big(room_x-2, room_y-2);
  
  /*
  for(int x=0; x<room_x; x+=2)
    if(hrand(100) < 10)
      place_big(x, room_y-2);

  for(int x=0; x<room_x; x++)
    if(hrand(100) < 10)
      place_small(x, 0); */

  for(int y=2; y<room_y-2; y+=2)
  for(int x=2; x<room_x-2; x+=2)
    if(hrand(room_y * 8) < y)
      place_big(x, y);

  for(int y=2; y<room_y-2; y++)
  for(int x=2; x<room_x-2; x++)
    if(block_at[y][x] == 0)
    if(hrand((room_y) * 8) < y)
      place_small(x, y);
    
  for(int y=room_y/2-2; y<room_y/2+2; y++)
  for(int x=room_x/2-2; x<room_x/2+2; x++)
    block_at[y][x] = 0;

  infile = false;
  }

void room::reveal(int cx, int cy) {
  if(cx < 0 || cy < 0 || cx >= room_x || cy >= room_y) return;
  fov[cy][cx] = 1;
  auto b = block_at[cy][cx];
  if(b & 4) {
    if(b&1) cx--;
    if(b&2) cy--;
    fov[cy][cx+1] = fov[cy+1][cx] = fov[cy+1][cx+1] = fov[cy][cx] = true;
    }   
  }

void room::reveal_around(int cx, int cy) {
  for(int dx: {-1, 0, 1}) for(int dy: {-1, 0, 1}) reveal(cx+dx, cy+dy);
  }

void room::fov_from(int sx, int sy) {
  for(int y=0; y<room_y; y++)
  for(int x=0; x<room_x; x++) 
  if(x == 0 || y == 0 || x == room_x-1 || y == room_y-1) {
    int len = max(abs(x-sx), abs(y - sy)) * 2;
    auto sh = to_hyper((sx+0.5)*block_x, (sy+0.5)*block_y);
    auto th = to_hyper((x+0.5)*block_x, (y+0.5)*block_y);
    for(int l=0; l<len; l++) {
      auto h = lerp(sh, th, l * 1. / len);
      auto xy = from_hyper(h);
      int cx = int(xy.first / block_x);
      int cy = int(xy.second / block_y);
      if(cx < 0 || cy < 0 || cx >= room_x || cy >= room_y) break;
      if(!(walls[at(cx, cy)].flags & W_TRANS)) break;
      reveal_around(cx, cy);
      }
    }
  }
    
room *get_room_at(cell *c) {
  bool create = !rooms.count(c);
  auto& r = rooms[c];
  if(create) {
    r.where = c;
    r.clear();
    r.initial();
    c->wall = waNone;
    c->land = laCanvas;
    }
  return &r;
  }

  }
