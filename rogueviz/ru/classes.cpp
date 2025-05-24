namespace rogue_unlike {

enum class rev { start, active, stop };

/** a helper structure to mass to powerfun */
struct data {
  int keystate;
  double d;
  ld modv;
  ld moda;
  int dx;
  struct power *p;
  struct randeff *re;
  rev mode;
  };

using powerfun = hr::function<void(data&)>;

struct randeff {
  string name;
  string desc;
  string effect;
  power *which_weapon;
  int qty, a, b, c, d;
  powerfun act;
  randeff (string name, string desc, string effect, powerfun act) : name(name), desc(desc), effect(effect), act(act) {}
  };

enum class mod { burning, freezing };

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
  vector<struct randeff*> randeffs;
  void init();
  vector<pair<mod, int>> mods;
  hr::function<void(data&)> act, paused_act, dead_act;
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
  power& while_dead();
  power& identified_name(string, string);
  power& be_wearable(string wear_effect, string remove_effect, string worn = " (worn)");
  power& be_jewelry(string jtype, string desc);
  power& be_potion();
  };

extern vector<power> powers;
extern power *extra_life;

flagtype IDENTIFIED = Flag(1);
flagtype ACTIVE = Flag(2);
flagtype PARTIAL = Flag(4);
flagtype WEAPON = Flag(8);

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
  int timed_orb_end;
  bool edited;

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
    edited = false;
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
  
  void replace_block_frev(int x, int y, eWall w) {
    auto orig = at(x, y);
    replace_block(x, y, w);
    add_revert(fountain_revert, [this, x, y, orig] { replace_block(x, y, orig); });
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
  bool operator == (xy b) { return x == b.x && y == b.y; }
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
  virtual ~entity() {}
  virtual xy siz() = 0;
  xy where, vel;
  bool existing;

  xy dsiz() { return get_scale() * siz(); }

  xy gwhere, gvel;
  xy zero_vel; /* relative to the platform */

  virtual struct moving_platform* as_platform() { return nullptr; }
  virtual struct shopitem* as_shopitem() { return nullptr; }
  virtual struct trader* as_trader() { return nullptr; }
  virtual struct missile* as_missile() { return nullptr; }

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

  bool on_floor, fallthru, on_ice, wallhug, on_bounce, is_stable;

  bool destroyed;
  void kino();
  void apply_vel();
  void apply_walls();
  void apply_walls_reflect();
  void apply_grav();
  void apply_portal_grav();
  bool stay_on_screen(); /* returns true if flipped */
  virtual void act() { kino(); }
  /* for things which can act while not existing */
  virtual void unact() { }

  double get_scale() { return get_scale_at(where.y); }
  virtual bool freezing() { return false; }
  virtual bool burning() { return false; }
  virtual void hit_wall() {};

  virtual void draw();

  virtual void attacked(int s) {}

  virtual void spiked() {
    reduce_hp(10);
    }

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

  virtual int invinc_time() { return 150; }

  virtual bool reduce_hp(int x) {
    if(hp < 0) return false;
    if(gframeid < invinc_end) return false;
    hp -= x;
    if(hp <= 0) on_kill();
    invinc_end = gframeid + invinc_time();
    return true;
    }

  virtual string get_help() { return "No help about this."; }

  virtual bool hit_by_missile(missile *m) { return false; }
  };

struct statdata {
  statarray<ld> stats;
  int jump_control, coyote_time, hallucinating;
  void reset();
  vector<tuple<power*, mod, int>> mods;
  };

struct man : public entity {
  int facing;
  int attack_facing;
  int attack_when;
  int on_floor_when;

  int last_action;

  int experience;

  statarray<int> base_stats;
  statdata current, next;

  virtual int max_hp() { return 10 * current.stats[stat::con]; }

  man() {
    facing = 1; attack_facing = 1;
    for(auto s: allstats) base_stats[s] = 10;
    next.reset(); current.reset();
    postfix();
    }
  xy siz() override { return {12, 12}; }
  string glyph() override { return "@"; }
  color_t color() override { return 0xFF8080FF; }
  void act() override; 
  void draw() override;
  virtual bool hurt_by_spikes() { return true; }
  string get_name() override { return "alchemist"; }
  string get_help() override { return "This is you."; }

  void on_kill() override;

  bool hit_by_missile(missile *m) override { return true; }

  virtual void spiked() {
    entity::spiked();
    addMessage("OUCH! These spikes hurt!");
    }

  };

extern man m;

struct moving_platform : public entity {
  xy ctr;
  ld radius, shift;
  virtual int width() { return 3; }
  virtual eWall platform_type() { return wWall; }
  xy siz() override { return {12.*width(), 12}; }
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

struct rope_platform : public moving_platform {
  ld period, shift, dist, max_swing;
  xy location_at(ld t) override;
  int width() override { return 1; }
  eWall platform_type() override { return wStaircase; }
  string glyph() override { return "-"; }
  string get_name() override { return "Swinging rope"; }
  string get_help() override { return "A part of a swinging rope."; }
  };

struct timed_orb : public entity {
  int duration;
  xy siz() override { return {18, 18}; }
  string glyph() override { return "O"; }
  color_t color() override {
    println(hlog, tie(gframeid, current_room->timed_orb_end));
    if(gframeid > current_room->timed_orb_end) return 0x8080FFFF;
    return gradient(0x404080FF, 0x8080FFFF, -1, cos((gframeid - current_room->timed_orb_end) * TAU * 5 / game_fps), 1);
    }
  void act() override;
  string get_name() override { return "time orb"; }
  string get_help() override { return "These orbs activate mechanisms for a limited time."; }
  };

struct npc_or_trader : public entity {
  string text;
  string name;
  int talk_on;
  xy siz() override { return {12, 12}; }
  void act() override;
  string get_name() override { return name; }
  };

struct npc : public npc_or_trader {
  string sglyph;
  color_t col;
  xy siz() override { return {12, 12}; }
  string glyph() override { return sglyph; }
  color_t color() override { return col; }
  string get_help() override { return "Stay awhile and listen."; }
  };

struct trader : public npc_or_trader {
  xy siz() override { return {18, 18}; }
  string glyph() override { return "@"; }
  color_t color() override { return 0x2020D0FF; }
  void act() override;
  string get_help() override { return "Stay awhile and listen. Or use gold to pay."; }
  virtual struct trader* as_trader() { return this; }
  };

struct enemy : public entity {
  xy respawn;
  int num_kills;
  void on_kill() override {
    entity::on_kill();
    num_kills++;
    m.experience += (base_xp() * 25 + 24) / (4 + num_kills) / (4 + num_kills);
    }
  enemy() { num_kills = 0; postfix(); }
  void attacked(int s) override;
  void regenerate() override { where = respawn; vel = xy(0, 0); existing = true; hp = max_hp(); }
  virtual int base_xp() { return 0; }
  bool hit_by_missile(missile *m) override { return true; }
  };

struct vtrap : public entity {
  xy siz() override { return {6, 18}; }
  string glyph() override { return "^"; }
  color_t color() override { return 0xD00000FF; }
  void act() override;
  string get_name() override { return "moving trap"; }
  string get_help() override { return "A deadly but invisible trap."; }
  };

struct boar : public enemy {
  xy siz() override { return {18, 18}; }
  string glyph() override { return "B"; }
  color_t color() override { return 0x804000FF; }
  void act() override;
  void attacked(int s) override;
  string get_name() override { return "giant boar"; }
  string get_help() override { return "Beware their tusks."; }
  int base_xp() { return 60; }
  int max_hp() { return 60; }
  };

struct frog : public enemy {
  int jphase, jump_at;
  xy siz() override { return {10, 10}; }
  string glyph() override { return "F"; }
  virtual ld maxvel() { return 3; }
  ld maxrange() { return 50; }
  color_t color() override { return 0x208020FF; }
  void act() override;
  void attacked(int s) override;
  string get_name() override { return "frog"; }
  string get_help() override { return "What a nice frog."; }
  int base_xp() { return 30; }
  int max_hp() { return 30; }
  };

struct giantfrog : public frog {
  int jphase, jump_at;
  xy siz() override { return {36, 36}; }
  string get_name() override { return "giant frog"; }
  string get_help() override { return "Beware their jumps."; }
  int base_xp() { return 200; }
  int max_hp() { return 200; }
  ld maxvel() { return 5; }
  };

struct ghost : public enemy {
  int xp, hp;
  bool flipped;
  xy siz() override { return {12, 12}; }
  string glyph() override { return "g"; }
  color_t color() override { return 0x4040A0FF; }
  void act() override;
  string get_name() override { return "ghost"; }
  string get_help() override { return "This apparition looks strangely like you..."; }
  int base_xp() { return hp; }
  int max_hp() { return xp; }
  void regenerate() override {}
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
  int base_xp() { return 10; }
  int max_hp() { return 30; }
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
  int base_xp() { return 30; }
  int max_hp() { return 30; }
  };

struct gridbug : public enemy {
  int next_move;
  xy siz() override { return {10, 10}; }
  string glyph() override { return "x"; }
  color_t color() override { return 0xD000D0FF; }
  void act() override;
  string get_name() override { return "grid bug"; }
  string get_help() override { return "You are not sure whether this is some kind of insect or some glitch in the fabric of the reality."; }
  int base_xp() { return 10; }
  int max_hp() { return 10; }
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
  int base_xp() { return 10; }
  int max_hp() { return 10; }
  };

struct hint : public entity {
  string hint_text;
  int state;
  xy size;
  xy siz() override { return size; }
  hint() { state = 0; }
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
    stay_on_screen();
    kino();
    if(intersect(get_pixel_bbox(), m.get_pixel_bbox())) {
      addMessage(pickup_message);
      int q0 = powers[id].qty_filled;
      int q1 = powers[id].qty_owned;
      add_revert(death_revert, [this, q0, q1] { existing = true; powers[id].qty_filled = q0; powers[id].qty_owned = q1; });
      powers[id].picked_up(qty);
      existing = false;
      }
    }
  string get_name() override { return powers[id].name; }
  string get_help() override { return powers[id].get_desc(); }
  };

struct shopitem : public item {
  int qty1;
  int price;
  bool bought;
  string glyph() override;
  color_t color() override;
  bool last_intersect;
  void act() override {
    kino();
    bool next_intersect = intersect(get_pixel_bbox(), m.get_pixel_bbox());
    if(next_intersect && !last_intersect) {
      addMessage("This costs " + its(price) + " gold.");
      }
    last_intersect = next_intersect;
    }
  string get_name() override { if(bought) return its(price) + " gold"; return item::get_name(); }
  string get_help() override { if(bought) return "You have bought something from this shop. The trader has stored the gold here."; return item::get_help() + "\n\nPrice: " + its(price); }
  shopitem* as_shopitem() override { return this; }
  };

struct loot : public item {
  entity *owner;
  bool dropped;
  void act() {
    item::act();
    if(on_floor) {
      auto dat = get_dat();
      if(vel.x > 0) vel.x = max<ld>(vel.x - dat.d * dat.moda * 0.02, 0);
      if(vel.x < 0) vel.x = min<ld>(vel.x + dat.d * dat.moda * 0.02, 0);
      }
    }
  void unact() override {
    if(!dropped && !owner->existing) {
      where = owner->where, vel = owner->vel;
      dropped = true; existing = true;
      }
    }
  };

struct missile : public entity {
  int power;
  missile() { destroyed = false; }
  xy siz() override { auto p = 2 + sqrt(power); return {p, p}; }
  string glyph() override { return "*"; }
  set<entity*> hit_list;
  void act() override; 
  void hit_wall() override { destroyed = true; }
  struct missile* as_missile() override { return this; }
  };

struct ice_missile : public missile {
  color_t color() override { return 0x8080FFFF; }
  bool freezing() override { return true; }
  bool hit_by_missile(missile *m) override { return m->burning(); }
  };

struct fire_missile : public missile {
  int index;
  color_t color() override { return gradient(0xFFFF00FF, 0xFF0000FF, -1, sin(index+ticks/100), 1); }
  bool burning() override { return true; }
  bool hit_by_missile(missile *m) override { return m->freezing(); }
  };

}
