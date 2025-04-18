namespace rogue_unlike {

struct data {
  int keystate;
  double d;
  ld modv;
  ld moda;
  int dx;
  };

using powerfun = hr::function<void(data&)>;

struct power {
  int key;
  string name;
  string desc;
  string glyph;
  color_t color;
  flagtype flags;
  powerfun pf;
  power(int key, string name, string desc, string glyph, color_t color, flagtype f, powerfun pf) : key(key), name(name), desc(desc), glyph(glyph), color(color), flags(f), pf(pf) {
    }
  };

extern vector<power> powers;

struct bbox {
  int minx, miny, maxx, maxy;
  bool operator != (bbox& a) { return minx != a.minx || miny != a.miny || maxx != a.maxx || maxy != a.maxy; }
  };

bool intersect(bbox a, bbox b);

struct room {
  // texture::texture_data *room_texture;
  string roomname;
  renderbuffer *rbuf;
  cell *where;
  char block_at[room_y][room_x];
  bool fov[room_y][room_x];
  bool which_map_rendered;
  bool infile, need_rerender;

  vector<unique_ptr<struct entity>> entities;

  eWall at(int x, int y) {
    return eWall(block_at[y][x] >> 3);
    }
  
  void clear() {
    for(int y=0; y<room_y; y++) for(int x=0; x<room_x; x++) block_at[y][x] = 0;
    }

  void clearfov() {
    for(int y=0; y<room_y; y++) for(int x=0; x<room_x; x++) fov[y][x] = false;
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

    roomname = "UNNAMED-";
    for(int i=0; i<8; i++) roomname += char('A' + rand() % 26);
    println(hlog, "generated roomname as ", roomname);
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

  void place_block_full(int x, int y, int b);

  void replace_block(int x, int y, eWall w) {
    int b = 8 * w;
    if(block_at[y][x] & 4) b += 4;
    place_block_full(x, y, b);
    }
  
  void generate();

  void reveal(int cx, int cy);
  void reveal_around(int cx, int cy);
  void fov_from(int sx, int sy);

  void create_texture();
  };

struct entity {
  virtual double sx() = 0;
  virtual double sy() = 0;
  double where_x, where_y;
  double vel_x, vel_y;

  ld dsx() { return get_scale() * sx(); }
  ld dsy() { return get_scale() * sy(); }

  double gwhere_x, gwhere_y;
  double gvel_x, gvel_y;

  void clearg() {
    gwhere_x = where_x;
    gwhere_y = where_y;
    gvel_x = vel_x;
    gvel_y = vel_y;
    }

  entity() {
    where_x = screen_x / 2.;
    where_y = screen_y / 2.;
    vel_x = 0;
    vel_y = 0;
    destroyed = false;
    clearg();
    };

  struct bbox get_pixel_bbox_at(double x, double y);
  struct bbox get_pixel_bbox() { return get_pixel_bbox_at(where_x, where_y); }

  virtual double grav() { return 0.1; }  

  bool on_floor;
  bool fallthru;
  bool on_ice;

  bool destroyed;
  void kino();
  void apply_grav();
  void apply_portal_grav();
  virtual void act() { kino(); }

  double get_scale() { return get_scale_at(where_y); }
  virtual bool freezing() { return false; }
  virtual void hit_wall() {};

  virtual void draw();

  virtual string glyph() = 0;
  virtual color_t color() = 0;
  };

struct man : public entity {
  int facing;
  int attack_facing;
  int attack_when;
  man() { facing = 1; attack_facing = 1; }
  double sx() override { return 12; }
  double sy() override { return 12; }
  string glyph() override { return hallucinating ? "f" : "@"; }
  color_t color() override { return hallucinating ? 0x808080FF : 0xFF8080FF; }
  void act() override; 
  void draw() override;
  };

extern man m;

struct sage : public entity {
  double sx() override { return 12; }
  double sy() override { return 12; }
  string glyph() override { return hallucinating ? "D" : "A"; }
  color_t color() override { return hallucinating ? 0xFF0000FF : 0x90FF90FF; }
  };

struct item : public entity {
  int id;
  double sx() override { return 12; }
  double sy() override { return 12; }
  string glyph() override { return powers[id].glyph; }
  color_t color() override { return powers[id].color; }
  void act() override {
    if(intersect(get_pixel_bbox(), m.get_pixel_bbox())) {
      addMessage("You pick up the " + powers[id].name + ".");
      destroyed = true;
      }
    }
  };

struct missile : public entity {
  missile() { destroyed = false; }
  double sx() override { return 4; }
  double sy() override { return 4; }
  string glyph() override { return "*"; }
  color_t color() override { return 0x8080FFFF; }
  void act() override; 
  bool freezing() override { return true; }
  void hit_wall() override { destroyed = true; }
  };

}
