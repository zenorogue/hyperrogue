namespace rogue_unlike {

hyperpoint to_hyper(ld x, ld y) {
  if(non_hyperbolic) return hyperpoint(x, y, 1, 1);
  y -= t_margin_at;
  y += actual_screen_y;
  x -= xctr;

  y /= mscale;
  x /= mscale;

  y += .5;
  ld d = x*x + y*y;
  x /= d;
  y /= d;
  y -= 1;
  hyperpoint h;
  h[0] = -x; h[1] = y; h[2] = 1;
  h = spin270() * h;
  return perspective_to_space(h, 1, gcHyperbolic);
  }

pair<ld, ld> from_hyper(hyperpoint h) {
  if(non_hyperbolic) return {h[0], h[1]};
  h = spin90() * h; h[0] = -h[0];
  h[2] += 1;
  h /= h[2];

  h[1] += 1;
  ld d = h[0]*h[0] + h[1]*h[1];
  h /= d;
  h[1] -= .5;
  
  double x = h[0], y = h[1];

  y *= mscale;
  x *= mscale;

  y -= actual_screen_y;
  y += t_margin_at;
  x += xctr;

  return {x, y};
  }

// room connections

void switch_to_room(room *r) {
  current_room->need_rerender = true;
  current_room = r;
  current_room->need_rerender = true;
  }

bool is_right(room *r) {
  cell *c = r->where;
  c->cmove(3);
  return c->c.spin(3) == 0;
  }

room *get_adjacent(room *r, int i) {
  cell *c = r->where;
  c->cmove(i);
  c = c->cmove(i);
  return get_room_at(c);
  }

void switch_to_adjacent_room(int i) {
  switch_to_room(get_adjacent(current_room, i));
  }

ld get_scale_at(ld y) {
  if(non_hyperbolic) return 1;
  return (actual_screen_y + (y - t_margin_at)) / (actual_screen_y * 3/2.);
  }

bbox pixel_to_block(bbox b) {
  b.minx /= block_x;
  b.miny /= block_y;
  b.maxx = (b.maxx + block_x-1) / block_x;
  b.maxy = (b.maxy + block_y-1) / block_y;
  if(b.minx < 0) b.minx = 0;
  if(b.miny < 0) b.miny = 0;
  if(b.maxx >= room_x) b.maxx = room_x;
  if(b.maxy >= room_y) b.maxy = room_y;
  return b;
  }

bbox join(bbox a, bbox b) {
  bbox r;
  r.minx = min(a.minx, b.minx);
  r.miny = min(a.miny, b.miny);
  r.maxx = max(a.maxx, b.maxx);
  r.maxy = max(a.maxy, b.maxy);
  return r;
  }

bbox room_bb{0, 0, room_x, room_y};
bbox screen_bb{0, 0, screen_x, screen_y};

bbox extend(bbox a, int l, int r, int u, int d) {
  a.minx -= l;
  a.maxx += r;
  a.miny -= u;
  a.maxy += d;
  return a;
  }

bbox extend_all(bbox a, int x) { return extend(a, x, x, x, x); }

bbox get_intersect(bbox a, bbox b) {
  bbox r;
  r.minx = max(a.minx, b.minx);
  r.miny = max(a.miny, b.miny);
  r.maxx = min(a.maxx, b.maxx);
  r.maxy = min(a.maxy, b.maxy);
  return r;
  }

bool intersect(bbox a, bbox b) {
  auto g = get_intersect(a, b);
  return g.minx < g.maxx && g.miny < g.maxy;
  }

void print(hstream& hs, const bbox& b) {
  print(hs, "[", b.minx,",",b.miny, ",",b.maxx,",",b.maxy,"]");
  }

hyperpoint rupf(ld x, ld y) {
  return hyperpoint(x, y, 1, 0);
  }

shiftmatrix asp(ld x, ld y, ld scale, ld scalex, ld scaley) {
  transmatrix V = Id;
  
  V[0][2] = (x - screen_x/2) * scale;
  V[1][2] = (y - screen_y/2) * scale;
  V[0][0] = 2 * cgi.hcrossf / cgi.crossf * scale * scalex;
  V[1][1] = 2 * cgi.hcrossf / cgi.crossf * scale * scaley;
  V[2][2] = 1;

  return shiftless(V);
  }

  }
