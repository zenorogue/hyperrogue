namespace rogue_unlike {

/** a helper structure to mass to powerfun */
struct data {
  int keystate;
  double d;
  ld modv;
  ld moda;
  int dx;
  struct power *p;
  };

using powerfun = hr::function<void(data&)>;

struct power {
  int key;
  string name;
  string desc;
  string glyph;
  color_t color;
  powerfun pf;
  int id_status;
  int qty_filled;
  int qty_owned;
  flagtype flags;
  int random_flavor;
  void init();
  hr::function<void(data&)> act, paused_act;
  hr::function<string()> get_name;
  hr::function<string()> get_desc;
  hr::function<string()> get_glyph;
  hr::function<color_t()> get_color;
  hr::function<void(int)> picked_up;
  hr::function<void()> refill;
  hr::function<void()> reshuffle;
  power& is_starting();
  power& be_weapon();
  power& be_resource(string plural);
  power& while_paused();
  power& identified_name(string, string);
  power& be_wearable(string wear_effect, string remove_effect);
  power& be_jewelry(string jtype, string desc);
  power& be_potion();
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
  short block_at[room_y][room_x];
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

struct xy {
  ld x, y;
  xy() {}
  xy(ld x, ld y) : x(x), y(y) {}
  xy operator + (xy b) { return xy(x+b.x, y+b.y); }
  xy operator - (xy b) { return xy(x-b.x, y-b.y); }
  xy operator * (ld s) { return xy(x*s, y*s); }
  xy operator / (ld s) { return xy(x/s, y/s); }
  xy operator * (xy b) { return xy(x*b.x, y*b.y); }
  xy operator / (xy b) { return xy(x/b.x, y/b.y); }
  friend xy operator * (ld s, xy a) { return xy(s*a.x, s*a.y); }
  xy& operator += (xy b) { x += b.x; y += b.y; return self; }
  xy& operator -= (xy b) { x -= b.x; y -= b.y; return self; }
  xy& operator *= (ld s) { x *= s; y *= s; return self; }
  xy& operator *= (xy b) { x *= b.x; y *= b.y; return self; }
  xy& operator /= (ld s) { x /= s; y /= s; return self; }
  };

enum class stat { str, con, wis, dex };

constexpr stat allstats[] =  { stat::str, stat::con, stat::wis, stat::dex };
constexpr int qstat = 4;

template<class T> struct statarray : array<T, qstat> {
  statarray() {};
  T& operator [] (stat s) { return array<T, qstat>::operator[] ((int) s); };
  const T& operator [] (stat s) const { return array<T, qstat>::operator[] ((int) s); };
  };

struct entity {
  virtual xy siz() = 0;
  xy where, vel;
  bool existing;

  xy dsiz() { return get_scale() * siz(); }

  xy gwhere, gvel;
  xy zero_vel; /* relative to the platform */

  virtual struct moving_platform* as_platform() { return nullptr; }

  int hp;
  int invinc_end;

  virtual int max_hp() { return 100; }
  virtual void regenerate() {}

  virtual bool visible(room *r);

  void clearg() {
    gwhere = where;
    gvel = vel;
    }

  entity() {
    where = xy(screen_x / 2., screen_y / 2.);
    vel = xy(0, 0);
    existing = true;
    destroyed = false; invinc_end = -1;
    clearg();
    };

  void postfix() {
    hp = max_hp();
    }

  data get_dat();

  struct bbox get_pixel_bbox_at(xy);
  struct bbox get_pixel_bbox() { return get_pixel_bbox_at(where); }

  virtual double grav() { return 0.1; }  

  bool on_floor, fallthru, on_ice, wallhug, on_bounce;

  bool destroyed;
  void kino();
  void apply_vel();
  void apply_walls();
  void apply_walls_reflect();
  void apply_grav();
  void apply_portal_grav();
  virtual void act() { kino(); }

  double get_scale() { return get_scale_at(where.y); }
  virtual bool freezing() { return false; }
  virtual void hit_wall() {};

  virtual void draw();

  virtual void attacked(int s) {}

  virtual string glyph() = 0;
  virtual color_t color() = 0;

  virtual bool hurt_by_spikes() { return false; }

  bool visible_inv() {
    return (invinc_end < gframeid || (invinc_end - gframeid) % 50 < 25);
    }

  virtual bool have_help() { return true; }
  virtual string get_name() { return "unnamed"; }

  virtual void on_kill() {
    existing = false;
    }

  virtual bool reduce_hp(int x) {
    if(gframeid < invinc_end) return false;
    hp -= x;
    if(hp < 0) on_kill();
    invinc_end = gframeid + 150;
    return true;
    }

  virtual string get_help() { return "No help about this."; }
  };

struct man : public entity {
  int facing;
  int attack_facing;
  int attack_when;

  int on_floor_when;
  int jump_control, next_jump_control;
  int coyote_time, next_coyote_time;

  int last_action;

  int experience;
  statarray<int> base_stats, current_stats, next_stats;

  virtual int max_hp() { return 10 * current_stats[stat::con]; }

  man() {
    facing = 1; attack_facing = 1;
    for(auto s: allstats) base_stats[s] = 10;
    next_stats = base_stats; current_stats = base_stats;
    postfix();
    }
  xy siz() override { return {12, 12}; }
  string glyph() override { return hallucinating ? "f" : "@"; }
  color_t color() override { return hallucinating ? 0x808080FF : 0xFF8080FF; }
  void act() override; 
  void draw() override;
  virtual bool hurt_by_spikes() { return true; }
  string get_name() override { return "alchemist"; }
  string get_help() override { return "This is you."; }
  };

extern man m;

struct moving_platform : public entity {
  xy ctr;
  ld radius, shift;
  xy siz() override { return {36, 12}; }
  string glyph() override { return "#"; }
  color_t color() override { return 0xFFFFFFFF; }
  virtual xy location_at(ld t) = 0;
  void draw() override;
  void act() override;
  virtual moving_platform* as_platform() { return this; }
  string get_name() override { return "moving platform"; }
  string get_help() override { return "Moving platforms move."; }
};

struct ferris_platform : public moving_platform {
  xy location_at(ld t) override;
  string get_name() override { return "Ferris platform"; }
  string get_help() override { return "Ferris wheel platforms, powered by some ancient mechanism. They always go in perfect circles, with constant velocity."; }
  };

struct pendulum_platform : public moving_platform {
  xy a, b;
  ld period, shift;
  xy location_at(ld t) override;
  string get_name() override { return "pendulum platform"; }
  string get_help() override { return "These pendulum platforms go back and forth between two locations, taking the shortest path possible."; }
  };

struct npc : public entity {
  string sglyph, name;
  color_t col;
  string text;
  int talk_on;
  xy siz() override { return {12, 12}; }
  string glyph() override { return sglyph; }
  color_t color() override { return col; }
  void act() override;
  string get_name() override { return name; }
  string get_help() override { return "Stay awhile and listen."; }
  };

struct enemy : public entity {
  xy respawn;
  int num_kills;
  void on_kill() override { entity::on_kill(); num_kills++; }
  enemy() { num_kills = 0; postfix(); }
  void regenerate() override { where = respawn; vel = xy(0, 0); existing = true; hp = max_hp(); }
  };


struct boar : public enemy {
  xy siz() override { return {18, 18}; }
  string glyph() override { return "B"; }
  color_t color() override { return 0x804000FF; }
  void act() override;
  void attacked(int s) override;
  string get_name() override { return "giant boar"; }
  string get_help() override { return "Beware their tusks."; }
  };

struct snake : public enemy {
  int dir, respawn_dir;
  xy siz() override { return {18, 8}; }
  string glyph() override { return "S"; }
  color_t color() override { return 0x20D020FF; }
  void act() override;
  void attacked(int s) override;
  string get_name() override { return "snake"; }
  string get_help() override { return "A nasty dungeon snake."; }
  void regenerate() override { enemy::regenerate(); dir = respawn_dir; }
  };

struct kestrel : public enemy {
  xy respawn_vel;
  xy siz() override { return {10, 10}; }
  string glyph() override { return "K"; }
  color_t color() override { return 0xD0A0A0FF; }
  void act() override;
  void attacked(int s) override;
  string get_name() override { return "kestrel"; }
  string get_help() override { return "A standard dungeon kestrel."; }
  void regenerate() override { enemy::regenerate(); vel = respawn_vel; }
  };

struct bat : public enemy {
  int next_change;
  xy siz() override { return {6, 6}; }
  string glyph() override { return "B"; }
  color_t color() override { return 0xD0A0A0FF; }
  void act() override;
  void attacked(int s) override;
  string get_name() override { return "bat"; }
  string get_help() override { return "A cave bat."; }
  };

struct hint : public entity {
  string hint_text;
  int state;
  xy size;
  xy siz() override { return size; }
  string glyph() override { return " "; }
  color_t color() override { return 0; }
  void act() override;
  bool have_help() { return false; }
  };

struct item : public entity {
  int id, qty;
  string pickup_message;
  xy siz() override { return {12, 12}; }
  string glyph() override { return powers[id].get_glyph(); }
  color_t color() override { return powers[id].get_color(); }
  void act() override {
    kino();
    if(intersect(get_pixel_bbox(), m.get_pixel_bbox())) {
      addMessage(pickup_message);
      powers[id].picked_up(qty);
      existing = false;
      }
    }
  string get_name() override { return powers[id].name; }
  string get_help() override { return powers[id].get_desc(); }
  };

struct missile : public entity {
  missile() { destroyed = false; }
  xy siz() override { return {4, 4}; }
  string glyph() override { return "*"; }
  color_t color() override { return 0x8080FFFF; }
  void act() override; 
  bool freezing() override { return true; }
  void hit_wall() override { destroyed = true; }
  };

}
