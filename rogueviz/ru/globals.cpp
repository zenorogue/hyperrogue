namespace rogue_unlike {

using namespace hr;

/* the size of a block, in pixels */
constexpr int block_x = 8;
constexpr int block_y = 8;

/* the size of a room (screen), in blocks */
constexpr int room_x = 80;
constexpr int room_y = 40;

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
int gframeid = 0;
bool bottom = 1;

constexpr auto yctr = (t_margin_at + b_margin_at) / 2.;  
constexpr auto xctr = (l_margin_at + r_margin_at) / 2.;  

double mscale = 100;

bool non_hyperbolic;
bool one_room;

struct ruwall {
  string name;
  string glyph;
  color_t color;
  flagtype flags;
  };

enum eWall { wAir, wWall, wBouncy, wSpike, wWater, wFrozen, wDoor, wSmashedDoor, wFountain0, wFountain1, wBluePortal, wOrangePortal, wPlatform };

flagtype W_BLOCK = 1;
flagtype W_TRANS = 2;
flagtype W_PLATFORM = 4;
flagtype W_STAIRCASE = 8;
flagtype W_PAIN = 16;
flagtype W_BOUNCY = 32;
flagtype W_FROZEN = 64;

constexpr int qwall = 14;

ruwall walls[qwall] = {
  {"air", ".", 0x40404080, W_TRANS},
  {"wall", "#", 0xFFFFFFFF, W_BLOCK},
  {"bouncy wall", "#", 0x80FF80FF, W_BLOCK | W_BOUNCY},
  {"spike", "^", 0xC08080FF, W_TRANS | W_PAIN},
  {"water", "~", 0x0000FFFF, W_BLOCK | W_TRANS},
  {"frozen water", "#", 0xC0C0FFFF, W_BLOCK | W_FROZEN},
  {"door", "+", 0xC06000FF, W_BLOCK},
  {"smashed door", "'", 0xC06000FF, W_TRANS},
  {"magic fountain", "!", 0x8080C0FF, W_TRANS},
  {"magic fountain (active)", "!", 0xA0A0FFFF, W_TRANS},
  {"blue portal", "=", 0x4040C0FF, W_TRANS},
  {"orange portal", "=", 0xC08040FF, W_TRANS},
  {"platform", "-", 0xFFFFFFFF, W_PLATFORM | W_TRANS },
  {"staircase", "-", 0xFFFF80FF, W_PLATFORM | W_TRANS | W_STAIRCASE },
  };

int sel = 1;

bool hallucinating;

template<class T> bool in_range(T val, T minv, T maxv) { return val >= minv && val <= maxv; }

map<cell*, struct room> rooms;

struct room *current_room;

basic_textureinfo roomtinf;
hpcshape roomshape, roomshape_big;

bool is_right(struct room *r);
struct room *get_adjacent(struct room *r, int i);

ld get_scale_at(ld y);
room *get_room_at(cell *c);

// gravity portals

extern bool gravision;

struct location {
  int x, y;
  char ch;
  bool fixed;
  ld potential;
  array<location*, 4> neighbors;
  location& get(int id) { return neighbors[id][0]; }
  };

extern array<array<location, 256>, 256> all_locations;

enum class mapmode { standard, poincare, klein };

enum class mode { editmap, menu, playing, paused, inventory, talking };

mode cmode = mode::playing;

mapmode cmapmode = mapmode::standard;
void switch_mapmode_to(mapmode m);

bool should_apply_fov() { return among(cmode, mode::playing, mode::paused, mode::inventory); }

void enable();

void update_keystate();
bool keyheld(int id);
bool keywasheld(int id);
bool keypressed(int id);

void sync_map();

void render_room_objects(room *r);


}
