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
  string id;
  string name;
  string desc;
  string effect;
  power *which_weapon;
  int qty, a, b, c, d;
  powerfun act;
  randeff (string name, string desc, string effect, powerfun act);
  void hs(struct stater& s);
  };

enum class mod { burning, freezing, disarming };

struct power {
  int key;
  string id;
  string name;
  string desc;
  string glyph;
  flavor fl = default_flavor;
  color_t color;
  powerfun pf;
  int id_status;
  int qty_filled;
  int qty_owned;
  int flags;
  int random_value;
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
  power& gain(int qf, int qo) { qty_filled += qf; qty_owned += qo; return self; }
  void hs(struct stater& s);
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

struct intxy {
  int x, y;
  intxy() {}
  intxy(int x, int y) : x(x), y(y) {}
  };

struct room {
  // texture::texture_data *room_texture;
  string id, roomname;
  renderbuffer *rbuf;
  cell *where;
  array<array<short, room_x>, room_y> block_at, orig_block_at;
  bool fov[room_y][room_x];
  bool which_map_rendered;

  bool infile; /* loaded from the map file, or edited */
  bool edited; /* edited, so it should be saved when saving map */
  bool save_to_save; /* will be saved to save files */
  bool need_rerender;

  int timed_orb_end;

  vector<unique_ptr<struct entity>> entities;

  eWall at(int x, int y) {
    return eWall(block_at[y][x] >> 3);
    }

  eWall at(intxy xy) { return at(xy.x, xy.y); }
  
  void clear() {
    for(int y=0; y<room_y; y++) for(int x=0; x<room_x; x++) block_at[y][x] = 0;
    }

  void clearfov() {
    for(int y=0; y<room_y; y++) for(int x=0; x<room_x; x++) fov[y][x] = false;
    }
  
  void initial() {
    edited = false;
    save_to_save = false;
    infile = false;
    int ylev = where->master->distance;
    if(ylev <= 0)
      for(int y=room_y-6; y<room_y; y++)
      for(int x=0; x<room_x; x++)
        block_at[y][x] = 8 + 4 + (x&1) + (y&1)*2;
    if(ylev < 0)
      for(int y=0; y<room_y-6; y++)
      for(int x=0; x<room_x; x++)
        block_at[y][x] = 8;

    id = roomname = "UNNAMED-";
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
    add_revert(fountain_revert, {"BLOCK", id, its(x), its(y), its(orig)});
    }

  void replace_block_frev(intxy xy, eWall w) { replace_block_frev(xy.x, xy.y, w); }

  void generate();

  void reveal(int cx, int cy);
  void reveal_around(int cx, int cy);
  void fov_from(int sx, int sy);

  void create_texture();

  using bfs_progress = hr::function<bool(intxy)>;
  vector<intxy> bfs(intxy start, const bfs_progress& f);
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

constexpr int qstat = 4;
constexpr array<stat, qstat> allstats = { stat::str, stat::con, stat::wis, stat::dex };

template<class T> struct statarray : array<T, qstat> {
  statarray() {};
  T& operator [] (stat s) { return array<T, qstat>::operator[] ((int) s); };
  const T& operator [] (stat s) const { return array<T, qstat>::operator[] ((int) s); };
  };

struct statinfo {
  char key;
  string name;
  string desc;
  };

extern statarray<statinfo> statinfos;

struct stater {
  virtual stater& act(const string& s, int& i, int _i) = 0;
  virtual stater& act(const string& s, bool& b, bool _b) = 0;
  virtual stater& act(const string& s, ld& d, ld _d) = 0;
  virtual stater& act(const string& s, string& w, const string& _w) = 0;
  virtual stater& act(const string& s, color_t& c, color_t _c) = 0;
  virtual stater& act(const string& s, intxy& xy, intxy _xy) { act(s+".x", xy.x, _xy.x); act(s+".y", xy.y, _xy.y); return self; }
  virtual stater& act(const string& s, xy& p, xy _p) { act(s+".x", p.x, _p.x); act(s+".y", p.y, _p.y); return self; }
  virtual stater& only_full() { return self; }
  };

struct entity {
  string id;
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

  virtual bool is_disarmer() { return false; }

  int hp;
  int invinc_end;

  entity *hallucinated = nullptr;
  virtual bool can_be_hallucinated() { return true; }

  virtual int max_hp() { return 100; }

  virtual bool visible(room *r);

  virtual xy default_where() { return {screen_x/2., screen_y/2.}; }
  virtual xy default_vel() { return {0, 0}; }
  virtual bool default_existing() { return true; }

  virtual void hs(stater& s) {
    s.act("where", where, default_where())
     .act("vel", vel, default_vel())
     .act("existing", existing, default_existing())
     .act("hp", hp, max_hp())
     .act("invinc_end", invinc_end, -1)
     .act("destroyed", destroyed, false);
    gwhere = where;
    gvel = vel;
    }

  data get_dat();

  virtual bool hidden() { return false; }

  struct bbox get_pixel_bbox_at(xy, ld scalex = 1, ld scaley = 1);
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
  void kill_off_screen(); /* returns true if flipped */
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

  virtual void on_fountain();

  virtual void on_reset_all() {}

  entity *hal();
  };

struct statdata {
  statarray<ld> stats;
  int jump_control, coyote_time, hallucinating;
  ld detect_area, detect_cross, rough_detect;
  void reset();
  vector<tuple<power*, mod, int>> mods;
  };

using boxfun = hr::function<bbox(int)>;

struct effect {
  power *p;
  int attack_facing;
  int attack_when;
  boxfun f;
  };

struct man : public entity {
  int facing;
  int attack_facing;
  int attack_when;
  int on_floor_when;
  entity *morphed = nullptr;
  vector<effect> effects;

  int last_action;

  int experience;
  stat profession;
  string backstory;
  flavor hair = default_flavor, eye = default_flavor;

  statarray<int> base_stats;
  statdata current, next;

  virtual int max_hp() { return 10 * current.stats[stat::con]; }

  bool can_see(entity& e);
  man();
  xy siz() override {
    if(morphed) return morphed->siz();
    return {12, 12};
    }
  string glyph() override {
    if(morphed) return morphed->glyph();
    return "@";
    }
  color_t color() override {
    if(morphed) return morphed->color();
    return 0xFF8080FF;
    }
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

  void launch_attack(power *p, int fac, boxfun f);

  virtual void hs(stater& s);
  };

extern man m;

struct moving_platform : public entity {
  xy ctr;
  ld radius, shift;
  ld cur_t;
  virtual int width() { return 3; }
  virtual eWall platform_type() { return wWall; }
  xy siz() override { return {12.*width(), 12}; }
  string glyph() override { return "#"; }
  color_t color() override { return 0xFFFFFFFF; }
  virtual xy location_at(ld t) = 0;
  virtual xy default_where() override { return location_at(cur_t); }
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
  void draw() override;
  };

/* entities with a defined 'respawn' location */
struct located_entity : public entity {
  xy respawn;
  xy default_where() override { return respawn; }
  };

struct timed_orb : public located_entity {
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

struct npc_or_trader : public located_entity {
  string text, name;
  int talk_on;
  xy siz() override { return {12, 12}; }
  void act() override;
  string get_name() override { return name; }
  virtual void hs(stater& s) override { entity::hs(s); s.act("talk_on", talk_on, 0); }
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

struct enemy : public located_entity {
  int num_kills;
  void on_kill() override {
    entity::on_kill();
    num_kills++;
    m.experience += (base_xp() * 25 + 24) / (4 + num_kills) / (4 + num_kills);
    }
  virtual void hs(stater& s) override { located_entity::hs(s); s.only_full().act("kills", num_kills, 0); }
  void attacked(int s) override;
  virtual int base_xp() { return 0; }
  bool hit_by_missile(missile *m) override { return true; }
  };

struct vtrap : public located_entity {
  xy siz() override { return {6, 18}; }
  string glyph() override { return "^"; }
  color_t color() override { return 0xD00000FF; }
  bool hidden() override { return true; }
  void act() override;
  string get_name() override { return "moving trap"; }
  string get_help() override { return "A deadly but invisible trap."; }
  };

void sact(stater& s, string t, flavor co) { s.act(t+".name", co.name, default_flavor.name).act(t+".col", co.col, default_flavor.col); }

struct cat : public enemy {
  flavor col = default_flavor;
  cat();
  xy siz() override { return {5, 5}; }
  string glyph() override { return "f"; }
  color_t color() override { return col.col; }
  string get_name() override { return col.name + " cat"; }
  string get_help() override { return "Do not fight cats!"; }
  virtual void hs(stater& s) override { enemy::hs(s); sact(s, "color", col); }
  int base_xp() { return 10; }
  int max_hp() { return 20; }
  };

struct capybara : public enemy {
  xy siz() override { return {6, 6}; }
  string glyph() override { return "C"; }
  color_t color() override { return 0xC08040FF; }
  string get_name() override { return "capybara"; }
  string get_help() override { return "Do not fight capybaras!"; }
  int base_xp() { return 15; }
  int max_hp() { return 40; }
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
  virtual void hs(stater& s) override { enemy::hs(s); s.act("jphase", jphase, 0).act("jump_at", jump_at, 0); }
  };

struct giantfrog : public frog {
  xy siz() override { return {36, 36}; }
  string get_name() override { return "giant frog"; }
  string get_help() override { return "Beware their jumps."; }
  int base_xp() { return 200; }
  int max_hp() { return 200; }
  ld maxvel() { return 5; }
  };

struct ghost : public enemy {
  int ghost_xp, ghost_hp, extra_invinc;
  bool flipped;
  xy siz() override { return {12, 12}; }
  string glyph() override { return "g"; }
  color_t color() override { return 0x4040A0FF; }
  void act() override;
  string get_name() override { return "ghost"; }
  string get_help() override { return "This apparition looks strangely like you..."; }
  int base_xp() { return ghost_hp; }
  int max_hp() { return ghost_xp; }
  xy default_where() override { return where; } // do not reset on fountains
  virtual void hs(stater& s) override {
    enemy::hs(s);
    s.act("extra_invinc", extra_invinc, 2 * game_fps).
    only_full().act("flipped", flipped, false).act("xp", ghost_xp, 50).act("hp", ghost_hp, 100).act("where", where, {0, 0});
    }
  void on_reset_all() override { destroyed = true; }
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
  int base_xp() override { return 10; }
  int max_hp() override { return 30; }
  virtual int bite() { return 25; }
  virtual void hs(stater& s) override { enemy::hs(s); s.act("dir", dir, respawn_dir); }
  };

struct disnake : public snake {
  color_t color() override { return m.hair.col; }
  void act() override;
  bool is_disarmer() override { return true; }
  string get_name() override { return "hairsnake"; }
  string get_help() override { return "A magically animated hair."; }
  int base_xp() override { return 0; }
  int max_hp() override { return 1; }
  void unact() override { destroyed = true; }
  int bite() override { return 5; }
  void on_fountain() override { destroyed = true; }
  void on_reset_all() override { destroyed = true; }
  virtual void hs(stater& s) override { snake::hs(s); s.act("respawn", respawn, {0, 0}); }
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
  int base_xp() { return 30; }
  int max_hp() { return 30; }
  xy default_vel() override { return respawn_vel; }
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
  virtual void hs(stater& s) override { enemy::hs(s); s.act("nextmove", next_move, 0); }
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
  virtual void hs(stater& s) override { enemy::hs(s); s.act("nextchange", next_change, 0); }
  };

struct guineapig : public enemy {
  int ca;
  bool falling;
  int spindir, respawn_spindir;
  ld pigvel;
  xy siz() override { return {10, 10}; }
  string glyph() override { return "G"; }
  color_t color() override { return 0xD0A0A0FF; }
  void act() override;
  void attacked(int s) override;
  string get_name() override { return "guinea pig"; }
  string get_help() override { return "A standard dungeon guinea pig."; }
  int base_xp() { return 30; }
  int max_hp() { return 30; }
  virtual void hs(stater& s) override { enemy::hs(s); s.act("falling", falling, true).act("spindir", spindir, respawn_spindir).act("ca", ca, 0); }
  };

struct icicle : public enemy {
  int state;
  vector<xy> fallframes;
  xy siz() override { return {10, 10}; }
  string glyph() override { return "|"; }
  color_t color() override { return 0xA0A0F0FF; }
  xy default_vel() override { auto dat = get_dat(); return { 0, 150 * 0.005 * dat.modv * dat.d }; }
  void act() override;
  string get_name() override { return "icicle"; }
  string get_help() override { return "A dangerous looking icicle."; }
  int base_xp() { return 1; }
  int max_hp() { return 1; }
  virtual void hs(stater& s) override { enemy::hs(s); s.act("state", state, 0); }
  };

struct hint : public located_entity {
  string hint_text;
  int state;
  xy size;
  xy siz() override { return size; }
  hint() { state = 0; }
  string glyph() override { return " "; }
  color_t color() override { return 0; }
  void act() override;
  bool have_help() { return false; }
  string get_name() override { return "<hint>"; }
  bool can_be_hallucinated() override { return false; }
  };

struct item : public located_entity {
  power *p;
  int qty;
  string pickup_message;
  xy siz() override { return {12, 12}; }
  string glyph() override { return p->get_glyph(); }
  color_t color() override { return p->get_color(); }
  void act() override {
    stay_on_screen();
    kino();
    if(intersect(get_pixel_bbox(), m.get_pixel_bbox())) {
      addMessage(pickup_message);
      add_revert(death_revert, {"ITEM", p->id, its(p->qty_filled), its(p->qty_owned)});
      add_revert(death_revert, {"EXIST", id});
      p->picked_up(qty);
      existing = false;
      }
    }
  string get_name() override { return p->name; }
  string get_help() override { return p->get_desc(); }
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
  virtual void hs(stater& s) override { item::hs(s); s.act("bought", bought, false).act("last_intersect", last_intersect, 0); }
  };

struct loot : public item {
  entity *owner;
  bool dropped;
  virtual bool default_existing() { return dropped; }
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
  virtual void hs(stater& s) override { s.act("dropped", dropped, false); item::hs(s); }
  };

struct ghost_item : public item {
  virtual void hs(stater& s) override { item::hs(s); s.act("respawn", respawn, {0, 0}).act("qty", qty, 0); }
  void on_reset_all() override { destroyed = true; }
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
  virtual void hs(stater& s) override { entity::hs(s); s.act("power", power, 0).act("where", where, {0, 0}); }
  void on_fountain() override { destroyed = true; }
  void on_reset_all() override { destroyed = true; }
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

struct vision : public entity {
  string text, name;
  string sglyph;
  color_t col;
  string glyph() override { return sglyph; }
  color_t color() override { return col; }
  string get_name() override { return name; }
  string get_help() override { return text; }
  xy siz() { return {1, 1}; }
  };

vector<unique_ptr<vision>> visions;

}
