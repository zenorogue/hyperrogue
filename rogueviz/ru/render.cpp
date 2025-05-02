namespace rogue_unlike {

shiftmatrix scrm;

void prepare_tinf() {
  
  auto add_vertex = [&] (double bx, double by) {
    roomtinf.tvertices.push_back(glhr::makevertex(bx / screen_x, 1 - by / screen_y, 0));
    cgi.hpc.push_back(to_hyper(bx, by));
    };
  
  cgi.bshape(roomshape, PPR::WALL);
  for(int by=t_margin_at; by<b_margin_at; by+=block_y)
  for(int bx=l_margin_at; bx<r_margin_at; bx+=block_x) {
    add_vertex(bx, by);
    add_vertex(bx+block_x, by);
    add_vertex(bx+block_x, by+block_y);
    add_vertex(bx, by);
    add_vertex(bx, by+block_y);
    add_vertex(bx+block_x, by+block_y);
    }
  cgi.last->flags |= POLY_TRIANGLES;
  cgi.last->tinf = &roomtinf;
  cgi.last->texture_offset = 0;

  int q = isize(roomtinf.tvertices);

  cgi.bshape(roomshape_big, PPR::WALL_TOP);
  for(int by=0; by<screen_y; by+=block_y)
  for(int bx=0; bx<screen_x; bx+=block_x) {
    add_vertex(bx, by);
    add_vertex(bx+block_x, by);
    add_vertex(bx+block_x, by+block_y);
    add_vertex(bx, by);
    add_vertex(bx, by+block_y);
    add_vertex(bx+block_x, by+block_y);
    }
  cgi.last->flags |= POLY_TRIANGLES;
  cgi.last->tinf = &roomtinf;
  cgi.last->texture_offset = q;

  cgi.finishshape();
  cgi.extra_vertices();

  println(hlog, "sizes: ", tuple(roomshape_big.e - roomshape_big.s, roomshape.e - roomshape.s));
  }

void room::create_texture() {
  /* if(room_texture) return;
  room_texture = new texture::texture_data;
  
  auto& tex = *room_texture;

  tex.twidth = tex.theight = 256;
  
  tex.tx = screen_x;
  tex.ty = screen_y;
  tex.stretched = false;
  tex.strx = tex.tx;
  tex.stry = tex.ty;
  tex.base_x = 0;
  tex.base_y = (tex.theight - tex.ty) / 2; */

  if(rbuf) return;
  rbuf = new renderbuffer(screen_x, screen_y, true);
  }

struct dqi_poly_tex : dqi_poly {
  int texture_id;
  void draw() override { if(tinf) tinf->texture_id = texture_id; dqi_poly::draw(); }
  };  

basic_textureinfo sprite_vertices;

void render_room(room *r);

bool draw_room_on_map(cell *c, const shiftmatrix& V) {
  hr::addaura(tC0(V), pconf.alpha == 1 ? 0xFF00FF00 : 0xFF00FFFF, 0);
  if(!rooms.count(c)) {
    c->landparam = 0x101010;
    get_room_at(c);
    return false;
    }
  auto& r = rooms[c];
  if(!r.rbuf || &r == current_room || r.which_map_rendered != should_apply_fov() || r.need_rerender) {
    vector<unique_ptr<drawqueueitem>> alt_ptds;
    swap(ptds, alt_ptds);
    render_room(&r);
    swap(ptds, alt_ptds);
    calcparam();
    }
  if(!r.rbuf) return false;

  bool big = (&r == current_room);
  auto& sh = big ? roomshape_big : roomshape;
  auto& p = queuea<dqi_poly_tex> (sh.prio);
  p.V = V;
  p.offset = sh.s;
  p.cnt = sh.e - sh.s;
  p.color = 0xFFFFFFFF;
  p.tab = &cgi.ourshape;
  p.flags = sh.flags;
  p.tinf = &roomtinf;
  p.tinf->texture_id = r.rbuf->renderedTexture;
  p.offset_texture = sh.texture_offset;
  p.texture_id = r.rbuf->renderedTexture;
  
  if(big || cmode == mode::editmap) {
    dynamicval<color_t> po(poly_outline, 0x80FF80FF);
    ensure_floorshape_generated(shvid(c), c);
    queuepolyat(V, cgi.shFullFloor.b[0], 0, PPR::LINE);
    }

  // render_room_objects(&r, render_at);
  return true;
  }

void asciiletter(ld minx, ld miny, ld maxx, ld maxy, const string& ch, color_t col);

void compute_scrm() {
  ld tx = screen_x;
  ld ty = screen_y;
  ld scalex = (vid.xres/2) / (current_display->radius * tx);
  ld scaley = (vid.yres/2) / (current_display->radius * ty);
  ld scale = min(scalex, scaley);
  scale *= 4;
  
  scrm = shiftless(Id);
  scrm.T[0][2] = (- screen_x/2) * scale;
  scrm.T[1][2] = (- screen_y/2) * scale;
  scrm.T[0][0] = scale;
  scrm.T[1][1] = scale;
  scrm.T[2][2] = 1;
  }

void render_room_walls(room *r) {
  initquickqueue();
  bool af = should_apply_fov();
  for(int y=0; y<room_y; y++)
  for(int x=0; x<room_x; x++) {
    if(af && !r->fov[y][x]) continue;
    int c = r->block_at[y][x];

    // ld sx = 1.5;
    // ld sy = 1.3;

    int cc = c >> 3;

    if((c & 7) == 0)
      asciiletter(x*block_x, y*block_y, (x+1)*block_x, (y+1)*block_y, walls[cc].glyph, walls[cc].color);

    if((c & 7) == 4)
      asciiletter(x*block_x, y*block_y, (x+2)*block_x, (y+2)*block_y, walls[cc].glyph, walls[cc].color);

    if(gravision) {
      int minx = x*block_x, maxx = (x+1)*block_x;
      int miny = y*block_y, maxy = (y+1)*block_y;
      for(int a: {0, 1, 3, 2, 0})
        curvepoint(hyperpoint((a&1)?minx:maxx, (a&2)?miny:maxy, 1, 0));
      color_t r = rainbow_color(1, all_locations[y][x].potential * 30);
      r <<= 8;
      queuecurve(scrm, r | 0xFF, r | 0x80, PPR::LINE);
      }
    }
  quickqueue();
  }

void render_room(room *r) {
  r->create_texture();

  resetbuffer rb;
  r->rbuf->enable();

  dynamicval<videopar> v(vid, vid);
  vid.xres = r->rbuf->tx;
  vid.yres = r->rbuf->ty;
  calcparam();

  current_display->set_viewport(0);
  r->rbuf->clear(0xFF000000);

  flat_model_enabler fme;
  compute_scrm();

  render_room_walls(r);
  render_room_objects(r);

  rb.reset();
  r->which_map_rendered = should_apply_fov();
  r->need_rerender = false;
  GLERR("render_room");
  }

transmatrix letterscales[128];

void init_scales() {
  for(int i=0; i<128; i++) letterscales[i] = Id;
  letterscales['#'] = euscale(1.5, 1.2);
  letterscales['*'] = eupush(0, 0.4) * euscale(1.5, 2);
  letterscales['~'] = eupush(0, -0.4) * euscale(1.5, 1.2);
  letterscales[')'] = euscale(2.5, 1);
  letterscales['('] = euscale(2.5, 1);
  }

void asciiletter(ld minx, ld miny, ld maxx, ld maxy, const string& ch, color_t col) {
  ld medx = (minx + maxx) / 2;
  ld medy = (miny + maxy) / 2;
  write_in_space(scrm * eupush(medx, medy) * euscale(maxx-minx, maxy-miny) * letterscales[int(ch[0])],
    max_glfont_size, 2, ch, col, 0, 8, PPR::TEXT, rupf);

  if(anyshiftclick) {
    for(int a: {0, 1, 3, 2, 0})
      curvepoint(hyperpoint((a&1)?minx:maxx, (a&2)?miny:maxy, 1, 0));
    queuecurve(scrm, 0xFF0000FF, 0xFF000080, PPR::LINE);
    }
  }

void entity::draw() {

  double d = get_scale();

  gwhere = where;

  auto si = siz();

  ld minx = min(where.x, gwhere.x) - si.x * d / 2;
  ld miny = min(where.y, gwhere.y) - si.y * d / 2;
  ld maxx = max(where.x, gwhere.x) + si.x * d / 2;
  ld maxy = max(where.y, gwhere.y) + si.y * d / 2;

  asciiletter(minx, miny, maxx, maxy, glyph(), color());
  }

void man::draw() {
  entity::draw();

  ld t = gframeid - attack_when;
  if(t < 50) {
    auto af = attack_facing * (1 - t * 0.01);
    auto ds = dsiz();
    asciiletter(
      where.x + af * ds.x - ds.x/2, where.y - ds.y/2,
      where.x + af * ds.x + ds.x/2, where.y + ds.y/2,
      attack_facing == -1 ? "(" : ")", 0xFFFFFF00 + (255 - t * 5)
      );
    }
  }

void render_room_objects(room *r) {
  initquickqueue();
  if(r == current_room && m.visible_inv()) m.draw();
  for(auto& e: r->entities)
    if(e->visible(r) && e->visible_inv())
      e->draw();
  quickqueue();
  }

int mousepx, mousepy;

void draw_room() {
  flat_model_enabler fme;

  compute_scrm();

  if(false) {
    basic_textureinfo bti;
    bti.texture_id = current_room->rbuf->renderedTexture;

    ld cx[6] = {1,0,0,0,1,1};
    ld cy[6] = {1,1,0,0,0,1};

    for(int i=0; i<6; i++) {
      bti.tvertices.emplace_back(glhr::makevertex(cx[i], 1 - cy[i], 0));
      curvepoint(eupoint(cx[i] * screen_x, cy[i] * screen_y));
      }

    initquickqueue();
    auto& q = queuecurve(scrm, 0xFFFFFFFF, 0xFFFFFFFF, PPR::LINE);
    q.tinf = &bti; q.flags |= POLY_TRIANGLES; q.offset_texture = 0;
    quickqueue();
    }
  if(true) {
    render_room_walls(current_room);
    }
  
  render_room_objects(current_room);
  }

}
