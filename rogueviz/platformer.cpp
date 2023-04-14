#include "../rogueviz/rogueviz.h"

/** \brief Hyperbolic platformer.
The implementation of the platformer from https://twitter.com/ZenoRogue/status/1467233150380089345 
To play, load platformer.lev (e.g. `-load platformer.lev`)
Or create a new map by running with options:
 `-noscr -canvas B -geo dbin -platformer`
 
Keys:
up/left/right -- move the guy
c -- clear the current room (buggy, does not clear parts of the adjacent rooms)
g -- generate the current room (buggy)
m -- see the map (toggle)
p -- pause (toggle)
z -- screenshot menu
v -- HyperRogue settings
q -- quit
s -- save to platformer.lev
1 -- place a small block under the mouse
2 -- place a big block under the mouse
3 -- delete the block under the mouse

Have fun!
*/

namespace hr {
#if CAP_TEXTURE
namespace platformer {

/* the size of a block, in pixels */
constexpr int block_x = 8;
constexpr int block_y = 8;

/* the size of a room (screen), in blocks */
constexpr int room_x = 32;
constexpr int room_y = 24;

/* the size of a room, in pixels */
constexpr int screen_x = block_x * room_x;
constexpr int screen_y = block_y * room_y;

/* the size of the area shared between right/left blocks */
constexpr int lr_margin = 4;

/* the size of the area shared with the block above */
constexpr int t_margin = 2;

/* the size of the area shared with the block below */
constexpr int b_margin = 2 * t_margin;

/* the position of margins in pixels */
constexpr int l_margin_at = lr_margin * block_x / 2;
constexpr int r_margin_at = screen_x - block_x * lr_margin / 2;
constexpr int t_margin_at = t_margin * block_y / 2;
constexpr int b_margin_at = screen_y - b_margin * block_y / 2;

/* between the margins */
constexpr int actual_screen_x = r_margin_at - l_margin_at;
constexpr int actual_screen_y = b_margin_at - t_margin_at;

int game_fps = 300;
bool bottom = 1;

basic_textureinfo roomtinf;

hpcshape roomshape;

constexpr auto yctr = (t_margin_at + b_margin_at) / 2.;  
constexpr auto xctr = (l_margin_at + r_margin_at) / 2.;  

double mscale = 100;

hyperpoint to_hyper(ld x, ld y) {
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

void prepare_tinf() {
  cgi.bshape(roomshape, PPR::WALL);
  
  auto add_vertex = [&] (double bx, double by) {
    roomtinf.tvertices.push_back(glhr::makevertex(bx / screen_x, (by + (screen_x-screen_y)/2.) / screen_x, 0));
    cgi.hpc.push_back(to_hyper(bx, by));
    };
  
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
  cgi.finishshape();
  cgi.extra_vertices();
  println(hlog, "room generated at ", roomshape.s, " to ", roomshape.e, " tvertices = ", isize(roomtinf.tvertices));
  }

bool is_right(struct room *r);
struct room *get_adjacent(struct room *r, int i);

struct room {
  texture::texture_data *room_texture;
  cell *where;
  char block_at[room_y][room_x];
  
  void clear() {
    for(int y=0; y<room_y; y++)
    for(int x=0; x<room_x; x++)
      block_at[y][x] = 0;
    }
  
  void initial() {
    int ylev = where->master->distance;
    if(ylev <= 0)
      for(int y=room_y-6; y<room_y; y++)
      for(int x=0; x<room_x; x++)
        block_at[y][x] = 8 + 4 + (x&1) + (y&1)*2;
    if(ylev < 0)
      for(int y=0; y<room_y-6; y++)
      for(int x=0; x<room_x; x++)
        block_at[y][x] = 8;
    }
  
  void place_small(int x, int y) {
    block_at[y][x] = 8;
    }

  void place_big(int x, int y) {
    block_at[y][x] = 8+4;
    block_at[y][x+1] = 8+5;
    block_at[y+1][x] = 8+6;
    block_at[y+1][x+1] = 8+7;
    }

  void place_block_full(int x, int y, int b) {
    indenter ind(2);
    println(hlog, "trying ", b, " at ", tie(x, y), " of ", where);
    

    if(x < 0 || y < 0 || x >= room_x || y >= room_y) return;
    if(b < 8) b = 0;
    if((b & 4) && (x&1) != (b&1)) return;
    if((b & 4) && (2*(y&1)) != (b&2)) return;
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
    
    if(x < lr_margin)
      get_adjacent(this, 2)->place_block_full(x + room_x - lr_margin, y, b);
    if(x >= room_x - lr_margin)
      get_adjacent(this, 4)->place_block_full(x - room_x + lr_margin, y, b);
    if(y < t_margin) {
      if(x < room_x / 2) {
        get_adjacent(this, 1)->place_block_full((x - lr_margin/2) * 2 + lr_margin/2, room_y - b_margin + 2*y, b^4);
        }
      else {
        get_adjacent(this, 0)->place_block_full((x - room_x/2) * 2 + lr_margin/2, room_y - b_margin + 2*y, b^4);
        }
      }
    
    if(y >= room_y - b_margin) {
      bool r = is_right(this);
      x -= lr_margin/2;
      x /= 2;
      x += (r ? room_x/2 : lr_margin/2);
      y = y - room_y + b_margin;
      y /= 2;
      b &= ~3;
      get_adjacent(this, 3)->place_block_full(x, y, b ^ 4);
      }
    }
  
  void generate() {
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
    }
  
  void create_texture();
  };

map<cell*, room> rooms;

bool paused;

room *current_room;

void switch_to_room(room *r) {
  current_room = r;
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

string block[8] = {
  "11111111",
  "12444461",
  "13555571",
  "13555571",
  "13555571",
  "13555571",
  "14666671",
  "11111111"
  };

const int man_x = 8, man_y = 12;

string man[man_y] = {
  "..1111..",
  "..1111..",
  "..1111..",
  "11111111",
  "..1111..",
  "..1111..",
  "1..11..1",
  ".111111.",
  "...11...",
  "..1111..",
  ".11..11.",
  "11....11",
  };

double where_x = screen_x / 2.;
double where_y = screen_y / 2.;

double get_scale() {
  return (actual_screen_y + (where_y - t_margin_at)) / (actual_screen_y * 3/2.);
  }

struct bbox {
  int minx, miny, maxx, maxy;
  };

bbox get_pixel_bbox() {
  bbox b;
  double d = get_scale();
  b.minx = where_x - man_x * d / 2;
  b.maxx = where_x + man_x * d / 2 + 1;
  b.miny = where_y - man_y * d / 2;
  b.maxy = where_y + man_y * d / 2 + 1;
  return b;
  }

bbox pixel_to_block(bbox b) {
  b.minx /= block_x;
  b.miny /= block_y;
  b.maxx = (b.maxx + block_x-1) / block_x;
  b.maxy = (b.maxy + block_y-1) / block_y;
  if(b.minx < 0) b.minx = 0;
  if(b.miny < 0) b.miny = 0;
  if(b.maxx >= room_x) b.maxx = 0;
  if(b.maxy >= room_y) b.maxy = 0;
  return b;
  }

bool conflict_at(double x, double y) {
  dynamicval<double> fx(where_x, x);
  dynamicval<double> fy(where_y, y);
  bbox b = pixel_to_block(get_pixel_bbox());
  for(int xx=b.minx; xx<b.maxx; xx++)
  for(int yy=b.miny; yy<b.maxy; yy++)
    if(current_room->block_at[yy][xx]) return true;
  return false;
  }

static double gtime = 0;

double vel_x = 0, vel_y = 0;

static const double grav = 0.1;

bool map_on = false;

bool last_mkey = false;

extern int mousepx, mousepy;

void game_frame() {  
  const Uint8 *keystate = SDL12_GetKeyState(NULL);
  if(keystate['3'])
    current_room->place_block_full(mousepx / block_x, mousepy / block_y, 0);
  if(keystate['1'])
    current_room->place_block_full(mousepx / block_x, mousepy / block_y, 8);
  if(keystate['2'])
    current_room->place_block_full(mousepx / block_x, mousepy / block_y, 12);
  
  if(paused) return;

  double d = get_scale();
  
  bool on_floor = false;
  
  ld modv = 60. / game_fps;
  ld moda = modv * modv;
  
  vel_y += d * grav * moda;
  
  while(conflict_at(where_x, where_y + vel_y)) {
    if(vel_y > 0) on_floor = true;
    vel_y /= 2;
    if(vel_y < 1e-9) { vel_y = 0; break; }
    if(!conflict_at(where_x, where_y + vel_y)) where_y += vel_y;
    }
  where_y += vel_y;

  double avel_x = vel_x;
  while(conflict_at(where_x + vel_x, where_y)) {
    vel_x /= 2;
    if(vel_x < 1e-9) { vel_x = 0; break; }
    if(!conflict_at(where_x + vel_x, where_y)) where_x += vel_x;
    }
  where_x += vel_x;
  vel_x = avel_x;
  
  hyperpoint h_at = to_hyper(where_x, where_y);
  // ld test_x, test_y;
  // tie(test_x, test_y) = from_hyper(h_at);
  
  /*println(hlog, tie(where_x, where_y), " TO ", h_at, " TO ", tie(test_x, test_y));
  exit(1); */
  
  hyperpoint h_was = to_hyper(where_x - vel_x, where_y - vel_y);
  hyperpoint h_willbe = rgpushxto0(h_at) * MirrorX * MirrorY * gpushxto0(h_at) * h_was;
  ld next_x, next_y;
  tie(next_x, next_y) = from_hyper(h_willbe);
  vel_x = next_x - where_x;
  vel_y = next_y - where_y;
  
  if(on_floor) {
    if(keystate[SDLK_LEFT]) vel_x = -d * modv;
    else if(keystate[SDLK_RIGHT]) vel_x = d * modv;
    else vel_x = 0;
    if(keystate[SDLK_UP]) vel_y = -4 * d * modv;
    }
  else {
    if(keystate[SDLK_LEFT]) vel_x += -d * .01 * moda;
    else if(keystate[SDLK_RIGHT]) vel_x += d * .01 * moda;
    vel_y += d * grav * moda;
    }
  
  if(where_x < l_margin_at) {
    where_x += actual_screen_x;
    switch_to_adjacent_room(2);
    }
  if(where_x > r_margin_at) {
    where_x -= actual_screen_x;
    switch_to_adjacent_room(4);
    }
  
  if(where_y < t_margin_at) {
    where_y = (where_y - t_margin_at) * 2 + b_margin_at;
    where_x -= l_margin_at;
    where_x = 2 * where_x;
    if(where_x > actual_screen_x) {
      where_x -= actual_screen_x;
      switch_to_adjacent_room(0);
      }
    else
      switch_to_adjacent_room(1);
    where_x += l_margin_at;
    vel_x *= 2; vel_y *= 2;
    }
  if(where_y > b_margin_at) {
    where_x -= l_margin_at;
    where_y -= b_margin_at;
    where_y /= 2;
    where_y += t_margin_at;
    if(is_right(current_room))
      where_x += actual_screen_x;
    switch_to_adjacent_room(3);
    where_x /= 2;
    where_x += l_margin_at;
    vel_x /= 2; vel_y /= 2;
    }

  }

void room::create_texture() {
  if(room_texture) return;
  room_texture = new texture::texture_data;
  
  auto& tex = *room_texture;

  tex.twidth = tex.theight = 256;
  
  tex.tx = screen_x;
  tex.ty = screen_y;
  tex.stretched = false;
  tex.strx = tex.tx;
  tex.stry = tex.ty;
  tex.base_x = 0;
  tex.base_y = (tex.theight - tex.ty) / 2;
  }

texture::texture_data *sprite_texture;

void create_sprite_texture() {
  if(sprite_texture) return;
  sprite_texture = new texture::texture_data;
  auto& tex = *sprite_texture;
  tex.twidth = tex.theight = 256;
  tex.whitetexture();
  for(int y=0; y<256; y++)
  for(int x=0; x<256; x++)
    tex.get_texture_pixel(x, y) = 0;
  for(int y=0; y<man_y; y++)
  for(int x=0; x<man_x; x++)
    if(man[y][x] == '1')
      tex.get_texture_pixel(x, y) = 0xFFFFFFFF;
  tex.loadTextureGL();  
  }

struct dqi_poly_tex : dqi_poly {
  int texture_id;
  void draw() override { if(tinf) tinf->texture_id = texture_id; dqi_poly::draw(); }
  };  

basic_textureinfo sprite_vertices;

template<class R> void render_room_objects(room *r, R render_at);

void render_room(room *r);

bool draw_room_on_map(cell *c, const shiftmatrix& V) {
  hr::addaura(tC0(V), 0xFF00FF00, 0);
  if(!rooms.count(c)) {
    c->landparam = 0x101010;
    get_room_at(c);
    return false;
    }
  auto& r = rooms[c];
  if(!r.room_texture) render_room(&r);
  if(!r.room_texture) return false;

  auto& p = queuea<dqi_poly_tex> (PPR::WALL);
  p.V = V;
  p.offset = roomshape.s;
  p.cnt = roomshape.e - roomshape.s;
  p.color = 0xFFFFFFFF;
  p.tab = &cgi.ourshape;
  p.flags = roomshape.flags;
  p.tinf = &roomtinf;
  p.offset_texture = 0;
  p.texture_id = r.room_texture->textureid;
  // println(hlog, "offset = ", p.offset, " texture_offset = ", p.offset_texture);
  
  auto render_at = [&] (GLuint texid, double px0, double py0, double px1, double py1,
    double tx0, double ty0, double tx1, double ty1) {

    auto addpoint = [&] (int x, int y) {
      curvepoint(to_hyper(x ? px1 : px0, y ? py1 : py0));
      sprite_vertices.tvertices.emplace_back(glhr::makevertex(x ? tx1 : tx0, y ? ty1 : ty0, 1));
      };
    
    int tcurvestart = isize(sprite_vertices.tvertices);
    
    addpoint(0, 0);
    addpoint(0, 1);
    addpoint(1, 1);
    addpoint(0, 0);
    addpoint(1, 0);
    addpoint(1, 1);
    
    auto& p = queuea<dqi_poly_tex> (PPR::MONSTER_BODY);
    p.V = V;
    p.offset = curvestart;
    p.cnt = isize(curvedata) - curvestart;
    curvestart = isize(curvedata);
    p.color = 0xFFFFFFFF;
    p.tab = &curvedata;
    p.flags = POLY_TRIANGLES;
    p.tinf = &sprite_vertices;
    p.offset_texture = tcurvestart;
    p.texture_id = texid;
    };
  
  render_room_objects(&r, render_at);
  return true;
  }

void render_room(room *r) {
  r->create_texture();
  auto& tex = *(r->room_texture);
  tex.whitetexture();
  
  auto pb = get_pixel_bbox();
  auto bb = pixel_to_block(pb);

  for(int y=0; y<screen_y; y++)
  for(int x=0; x<screen_x; x++)
    tex.get_texture_pixel(x, y) = 0;
  
  for(int y=0; y<screen_y; y++)
  for(int x=0; x<screen_x; x++) {
  
    int lx = x / block_x;
    int ly = y / block_y;
    char c = r->block_at[ly][lx];
    
    if(c == 0) {
      if(false && lx >= bb.minx && lx < bb.maxx && ly >= bb.miny && ly < bb.maxy)
        tex.get_texture_pixel(x, y + tex.base_y) = 0xFF008000;
      else
        tex.get_texture_pixel(x, y + tex.base_y) = 0xFF000000;
      }
    
    else {
      int ax = x % block_x;
      int ay = y % block_y;
      if(c & 4) {
        ax /= 2;
        ay /= 2;
        if(c & 1) ax += (block_x/2);
        if(c & 2) ay += (block_y/2);
        }
      char chr = block[ay][ax];
      tex.get_texture_pixel(x, y + tex.base_y) = 0xFFFFFFFF + 0x202020 * (chr - '7');
      }
    }
  
  tex.loadTextureGL();  
  }

template<class R> void render_room_objects(room *r, R render_at) {
  auto pb = get_pixel_bbox();  
  if(r != current_room) return;
  create_sprite_texture();
  render_at(sprite_texture->textureid, pb.minx, pb.miny, pb.maxx, pb.maxy, 0, 0, man_x/256., man_y/256.);
  }

int mousepx, mousepy;

void draw_room() {
  render_room(current_room);
  create_sprite_texture();
  flat_model_enabler fme;
  auto& tex = *(current_room->room_texture);

  ld tx = tex.tx;
  ld ty = tex.ty;
  ld scalex = (vid.xres/2) / (current_display->radius * tx);
  ld scaley = (vid.yres/2) / (current_display->radius * ty);
  ld scale = min(scalex, scaley);
  scale *= 4;
  
  double low = tex.base_y * 1. / tex.theight;
  
  auto render_at = [&] (GLuint texid, double px0, double py0, double px1, double py1,
    double tx0, double ty0, double tx1, double ty1) {
    static vector<glhr::textured_vertex> rtver(4);  

    ld cx[4] = {1,0,0,1};
    ld cy[4] = {1,1,0,0};
  
    for(int i=0; i<4; i++) {
      rtver[i].texture[0] = cx[i] ? tx1 : tx0;
      rtver[i].texture[1] = cy[i] ? ty1 : ty0;
      rtver[i].coords[0] = ((cx[i] ? px1 : px0) - screen_x/2) * scale;
      rtver[i].coords[1] = ((cy[i] ? py1 : py0) - screen_y/2) * scale;
      rtver[i].coords[2] = 1;
      rtver[i].coords[3] = 1;
      }
    
    glhr::be_textured();
    current_display->set_projection(0, false);
    glBindTexture(GL_TEXTURE_2D, texid);
    glhr::color2(0xFFFFFFFF);
    glhr::id_modelview();
    current_display->set_mask(0);
    glhr::prepare(rtver);
    glhr::set_depthtest(false);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    };
  
  render_at(tex.textureid, 0, 0, screen_x, screen_y, 0, low, 1, 1-low);
  render_room_objects(current_room, render_at);
  
  mousepx = (mousex - current_display->xcenter) * 2 / scale / current_display->radius + screen_x/2;
  mousepy = (mousey - current_display->ycenter) * 2 / scale / current_display->radius + screen_y/2;
  }

void run() {
  clearMessages();
  dialog::init();

  sprite_vertices.tvertices.clear();
  if(map_on) {
    render_room(current_room);
    gamescreen();
    }
  else {
    draw_room();
    }
  
  dialog::add_key_action('c', [] { current_room->clear(); });
  dialog::add_key_action('g', [] { current_room->generate(); });
  dialog::add_key_action('m', [] { 
    map_on = !map_on;
    });
  dialog::add_key_action('v', [] {
    pushScreen(showSettings);
    });
  dialog::add_key_action('p', [] { 
    paused = !paused;
    });
  dialog::add_key_action('z', [] { 
    pushScreen(shot::menu);
    });
  dialog::add_key_action('q', [] {
    if(tour::on) tour::next_slide();
    else exit(0);
    });
  dialog::add_key_action('o', [] {
    if(tour::on) tour::next_slide();
    });
  dialog::add_key_action(SDLK_ESCAPE, [] {
    if(tour::on) tour::next_slide();
    });
  dialog::add_key_action(SDLK_F10, [] {
    if(tour::on) tour::next_slide();
    });
  dialog::add_key_action('s', [] {
    mapstream::saveMap("platformer.lev");
    });
  
  keyhandler = [] (int sym, int uni) {
    if(map_on && paused)
      handlePanning(sym, uni);
    dialog::handleNavigation(sym, uni);
    };
  }

void add_platf_hooks();

void set_sval() {
  
  ld s_min = 10, s_max = 1200;
  
  for(int it=0; it<100; it++) {
    mscale = sqrt(s_min * s_max);
    hyperpoint atop = deparabolic13(to_hyper(0, t_margin_at));
    if(atop[0] < -log(2)/2) s_max = mscale;
    else s_min = mscale;
    }
  }

void enable() {
  
  stop_game();
  
  set_sval();
  
  hyperpoint aleft = deparabolic13(to_hyper(l_margin_at, yctr));
  hyperpoint aright = deparabolic13(to_hyper(r_margin_at, yctr));
  
  vid.binary_width = abs(aright[1] - aleft[1]) / log(2);
  
  start_game();
  cgi.prepare_shapes();

  current_room = get_room_at(cwt.at);
  
  prepare_tinf();
  
  add_platf_hooks();
  }

void add_platf_hooks() {
 
  rogueviz::rv_hook(hooks_prestats, 90, [=] {
  
    if(nomap)
      draw_room();
    else
      render_room(current_room);
    
    return true;
    });

  rogueviz::rv_hook(shmup::hooks_turn, 90, [=] (int d) {
    gtime += d;
    while(gtime > 1000. / game_fps) {
      gtime -= 1000. / game_fps;
      game_frame();
      }
    return true;
    });

  rogueviz::rv_hook(hooks_drawcell, 90, draw_room_on_map);

  rogueviz::rv_hook(mapstream::hooks_savemap, 100, [] (hstream& f) {
    f.write<int>(66);
    for(auto& p: rooms) {
      f.write(mapstream::cellids[p.first]);
      for(int y=0; y<room_y; y++)
      for(int x=0; x<room_x; x++)
        f.write(p.second.block_at[y][x]);
      f.write<int>(0);
      }
    f.write<int>(-1);
    f.write(mapstream::cellids[current_room->where]);
    f.write(where_x);
    f.write(where_y);
    f.write(vel_x);
    f.write(vel_y);
    });

  pushScreen(run);  
  }

auto chk = arg::add3("-platformer", enable)
  + addHook_rvslides(195, [] (string s, vector<tour::slide>& v) {
      if(s != "mixed") return;
      v.push_back(tour::slide{
        "platformer", 10, tour::LEGAL::NONE | tour::QUICKSKIP | tour::QUICKGEO,
        "A non-Euclidean platformer.\n\nPress up/left/right to move the guy.\n\nM to see the map\n\nP to pause\n\nV to change HyperRogue settings.\n\nPress Q when you are done.\n"
        ,
        [] (tour::presmode mode) {
          slide_url(mode, 'y', "non-Euclidean platformer (YouTube)", "https://www.youtube.com/watch?v=eb2DhCcGH7U");
          slide_url(mode, 't', "non-Euclidean platformer (Twitter)", "https://twitter.com/ZenoRogue/status/1467233150380089345");
          slide_url(mode, 'g', "how to edit this", "https://github.com/zenorogue/hyperrogue/blob/master/rogueviz/platformer.cpp");
          setCanvas(mode, '0');
          using namespace tour;
          if(mode == pmStart) {
            mapstream::loadMap("platformer.lev");
            }
          }
        });
      })
+ addHook(mapstream::hooks_loadmap, 100, [] (hstream& f, int id) {
    if(id == 66) {
      println(hlog, "loading platformer");
      while(true) {
        int i = f.get<int>();
        if(i == -1) break;
        auto r = get_room_at(mapstream::cellbyid[i]);
        for(int y=0; y<room_y; y++)
        for(int x=0; x<room_x; x++)
          f.read(r->block_at[y][x]);
        f.get<int>();
        }
      int id = f.get<int>();
      current_room = get_room_at(mapstream::cellbyid[id]);
      f.read(where_x);
      f.read(where_y);
      f.read(vel_x);
      f.read(vel_y);
      add_platf_hooks();
      println(hlog, "done");
      set_sval();

      cgi.prepare_shapes();
      prepare_tinf();
      }
    });

}
#endif
}
