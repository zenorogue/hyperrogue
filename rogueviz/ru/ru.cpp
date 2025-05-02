#include "../../rogueviz/rogueviz.h"

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

#include "globals.cpp"
#include "classes.cpp"
#include "geometry.cpp"
#include "entity.cpp"
#include "man.cpp"
#include "room.cpp"
#include "render.cpp"
#include "portals.cpp"
#include "powers.cpp"
#include "save.cpp"

namespace rogue_unlike {

static double gtime = 0;

bool last_mkey = false;

extern int mousepx, mousepy;

int lev = 0;
void floodfill(int x, int y, int src, int tgt) {
  dynamicval<int> d(lev, lev+1);
  println(hlog, lev, ": ", tie(x, y));
  if(x < 0 || y < 0 || x >= room_x || y >= room_y) return;
  if(y > room_y - b_margin && tgt) return;
  if(current_room->block_at[y][x] != src) return;
  current_room->place_block_full(x, y, tgt);
  floodfill(x+1, y, src, tgt);
  floodfill(x-1, y, src, tgt);
  floodfill(x, y+1, src, tgt);
  floodfill(x, y-1, src, tgt);
  }

void floodfill_pick(int x, int y) {
  if(x < 0 || y < 0 || x >= room_x || y >= room_y) return;
  auto cur = current_room->block_at[y][x];
  if(cur != 8*sel) floodfill(x, y, cur, 8 * sel);
  else floodfill(x, y, 8 * sel, 0);
  }

bool last_keystate[KEYSTATES], cur_keystate[KEYSTATES];

bool keyheld(int id) { return cur_keystate[id]; }
bool keywasheld(int id) { return last_keystate[id]; }
bool keypressed(int id) { return cur_keystate[id] && !last_keystate[id]; }

void update_keystate() {
  const Uint8 *keystate = SDL12_GetKeyState(NULL);
  for(int i=0; i<KEYSTATES; i++) last_keystate[i] = cur_keystate[i], cur_keystate[i] = keystate[i];
  }

void editmap_frame() {
  if(keyheld('1'))
    current_room->place_block_full(mousepx / block_x, mousepy / block_y, 8*sel);
  if(keyheld('2'))
    current_room->place_block_full(mousepx / block_x, mousepy / block_y, 8*sel+4);
  if(keyheld('3'))
    current_room->place_block_full(mousepx / block_x, mousepy / block_y, 0);
  if(keypressed('4')) pushScreen([] {
    dialog::init();
    dialog::addTitle("what to add", 0x4040C0, 150);
    for(int i=1; i<qwall; i++) {
      dialog::addItem(walls[i].name, 'a'+i);
      dialog::add_action([i] { sel = i; popScreen(); });
      }
    dialog::display();
    });
  if(keypressed('f')) floodfill_pick(mousepx / block_x, mousepy / block_y);
  if(keypressed('t')) { m.where_x = mousepx; m.where_y = mousepy; m.vel_x = 0; m.vel_y = 0; }
  }

void playing_frame() {
  m.act();

  auto& ents = current_room->entities;

  for(auto& e: ents) e->act();

  auto mb = ents.begin();
  for(auto& e: ents) if(!e->destroyed) *(mb++) = std::move(e);
  ents.resize(mb - ents.begin());
  
  auto& nonh = non_hyperbolic;
  if(one_room) return;
  
  if(m.where_x < l_margin_at) {
    m.where_x += actual_screen_x;
    switch_to_adjacent_room(2);
    m.clearg();
    }
  if(m.where_x > r_margin_at) {
    m.where_x -= actual_screen_x;
    switch_to_adjacent_room(nonh ? 0 : 4);
    m.clearg();
    }
  
  if(m.where_y < t_margin_at && !nonh) {
    m.where_y = (m.where_y - t_margin_at) * 2 + b_margin_at;
    m.where_x -= l_margin_at;
    m.where_x = 2 * m.where_x;
    if(m.where_x > actual_screen_x) {
      m.where_x -= actual_screen_x;
      switch_to_adjacent_room(0);
      }
    else
      switch_to_adjacent_room(1);
    m.where_x += l_margin_at;
    m.vel_x *= 2; m.vel_y *= 2;
    m.clearg();
    }
  if(m.where_y > b_margin_at && !nonh) {
    m.where_x -= l_margin_at;
    m.where_y -= b_margin_at;
    m.where_y /= 2;
    m.where_y += t_margin_at;
    if(is_right(current_room))
      m.where_x += actual_screen_x;
    switch_to_adjacent_room(3);
    m.where_x /= 2;
    m.where_x += l_margin_at;
    m.vel_x /= 2; m.vel_y /= 2;
    m.clearg();
    }
  }

void sync_map() {
  for(auto& p: rooms)
    if(&p.second == current_room)
      centerover = p.first;
  View = cspin90(1, 0);
  // if(cmode == mode::playing) View = View * inverse(parabolic13_at(to_hyper(m.where_x, m.where_y)));
  if(cmode == mode::playing) {
    hyperpoint p = to_hyper(m.where_x, m.where_y);
    transmatrix T = iso_inverse(parabolic13_at(deparabolic13(p)));
    View = View * T;
    }
  }

void switch_mapmode_to(mapmode m) {
  cmapmode = m;
  switch(cmapmode) {
    case mapmode::standard:
      nomap = true;
      break;
    case mapmode::poincare:
    case mapmode::klein:
      sync_map();
      pconf.scale = 0.95;
      pconf.alpha = cmapmode == mapmode::poincare ? 1 : 0;
      pmodel = mdDisk;
      nomap = false;
      break;
    }
  }

void render_the_map() {
  switch(cmapmode) {
    case mapmode::standard:
      emptyscreen();
      render_room(current_room);
      draw_room();
      drawmessages();
      nomsg = false;
      if(1) {
        compute_scrm();
        ld& scale = scrm.T[0][0];
        mousepx = (mousex - current_display->xcenter) * 2 / scale / current_display->radius + screen_x/2;
        mousepy = (mousey - current_display->ycenter) * 2 / scale / current_display->radius + screen_y/2;
        }
      dialog::add_key_action('v', [] { cmode = mode::menu; });
      break;
    case mapmode::poincare:
    case mapmode::klein:
      gamescreen();
      if(!mouseout() && mouseover && rooms.count(mouseover) && anyshiftclick) {
        current_room = &rooms[mouseover];
        }
      if(!mouseout()) {
        auto h = inverse_shift(ggmatrix(current_room->where), mouseh);
        tie(mousepx, mousepy) = from_hyper(h);
        }
      if(cmode == mode::editmap) {
        getcstat = '-';
        dialog::add_key_action('-', [] { if(!mouseover) return; current_room = &rooms[mouseover]; switch_mapmode_to(mapmode::standard); });
        }
      break;
    }
  }

hyperpoint pentagon_center = Hypc;
cell *pentagon_at = nullptr;
ld pentagon_spin = 0;

void draw_pentagon() {

  if(cmapmode == mapmode::standard) return;
  dialog::add_key_action('P', [] {
    pentagon_at = mouseover;
    pentagon_center = inverse_shift(ggmatrix(mouseover), mouseh);
    });
  dialog::add_key_action('I', [] { pentagon_spin += 0.01; });
  dialog::add_key_action('O', [] { pentagon_spin -= 0.01; });

  if(!pentagon_at) return;

  int q = 5;
  ld e = edge_of_triangle_with_angles(90._deg, M_PI/q, 45._deg);
  initquickqueue();
  for(int i=0; i<6; i++) {
    auto S = ggmatrix(pentagon_at) * rgpushxto0(pentagon_center) * spin(pentagon_spin);
    vid.linewidth *= 5;
    queueline(S * xspinpush0(TAU*i/q, e), S * xspinpush0(TAU*(i+1)/q, e), 0xFFD50080, 4);
    vid.linewidth /= 5;
    queueline(S * xspinpush0(TAU*i/q, e), S * xspinpush0(TAU*(i+1)/q, e), 0xFFD500FF, 4);
    }
  quickqueue();
  }

void run() {
  // clearMessages();
  dialog::init();

  switch(cmode) {
    case mode::editmap:
    case mode::playing:
    case mode::paused:
      if(cmode == mode::playing) sync_map();
      render_the_map();
      if(cmode == mode::editmap) dialog::add_key_action('p', [] {
        if(cmapmode == mapmode::poincare) switch_mapmode_to(mapmode::standard);
        else switch_mapmode_to(mapmode::poincare);
        });
      if(cmode == mode::editmap) mouseovers = format("coordinates: %d %d (%.2lf)", mousepx, mousepy, double(get_scale_at(mousepy)));
      draw_pentagon();
      break;

    case mode::inventory:
      render_the_map();
      draw_inventory();
      dialog::add_key_action('v', [] { cmode = mode::menu; });
      break;

    case mode::talking:
      break;

    case mode::menu:
      nomap = true;
      emptyscreen();
      dialog::init();
      dialog::addTitle("Fountains of Alchemy", 0x4040C0, 150);
      dialog::addItem("return to game", 'v');
      dialog::add_action([] { cmode = mode::playing; });

      dialog::addItem("inventory", 'i');
      dialog::add_action([] { cmode = mode::inventory; });

      dialog::addItem("map editor", 'e');
      dialog::add_action([] { cmode = mode::editmap; });

      dialog::addItem("screenshot", 'z');
      dialog::add_action([] { pushScreen(shot::menu); });

      dialog::addItem("save to editmap.ru", 's');
      dialog::add_key_action('s', [] {
        save_map("rogueviz/ru/editmap.ru");
        });
      
      dialog::display();
      break;
    }
  
  keyhandler = [] (int sym, int uni) {
    if(among(cmode, mode::paused, mode::editmap) && cmapmode != mapmode::standard)
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
  init_scales();
  gen_powers();
  
  hyperpoint aleft = deparabolic13(to_hyper(l_margin_at, yctr));
  hyperpoint aright = deparabolic13(to_hyper(r_margin_at, yctr));
  
  vid.binary_width = abs(aright[1] - aleft[1]) / log(2);
  
  start_game();
  cgi.require_shapes();

  current_room = get_room_at(cwt.at);
  
  prepare_tinf();
  
  add_platf_hooks();
  load_map("rogueviz/ru/map.ru");
  switch_mapmode_to(mapmode::standard);
  }

void add_platf_hooks() {
 
  rogueviz::rv_hook(hooks_prestats, 90, [=] {
  
    if(nomap)
      draw_room();
    else
      render_room(current_room);
    
    return true;
    });

  rogueviz::rv_hook(hooks_resetGL, 90, [=] {
    for(auto& [w, r]: rooms) if(r.rbuf) { delete r.rbuf; r.rbuf = nullptr; }
    });

  rogueviz::rv_hook(shmup::hooks_turn, 90, [=] (int d) {
    gtime += d;
    while(gtime > 1000. / game_fps) {
      gtime -= 1000. / game_fps;

      update_keystate();

      switch(cmode) {
        case mode::editmap:
          editmap_frame();
          break;

        case mode::playing:
          gframeid++;
          playing_frame();
          break;

        case mode::paused:
          data d;
          handle_powers(d);
          break;

        default: ;
        }
      }
    return true;
    });

  rogueviz::rv_hook(hooks_drawcell, 90, draw_room_on_map);

  rogueviz::rv_hook(mapstream::hooks_savemap, 100, [] (hstream& f) {
    f.write<int>(67);
    for(auto& p: rooms) {
      f.write(mapstream::cellids[p.first]);
      for(int y=0; y<room_y; y++)
      for(int x=0; x<room_x; x++)
        f.write(p.second.block_at[y][x]);
      f.write<int>(0);
      }
    f.write<int>(-1);
    f.write(mapstream::cellids[current_room->where]);
    f.write(m.where_x);
    f.write(m.where_y);
    f.write(m.vel_x);
    f.write(m.vel_y);
    });

  pushScreen(run);  
  }

auto chk = arg::add3("-ru", enable)
  + arg::add3("-ru-cheat", [] { arg::shift(); load_cheat(arg::args()); })
  + addHook(mapstream::hooks_loadmap, 100, [] (hstream& f, int id) {
    if(id == 67) {
      println(hlog, "loading platformer");
      while(true) {
        int i = f.get<int>();
        if(i == -1) break;
        auto r = get_room_at(mapstream::cellbyid[i]);
        for(int y=0; y<room_y; y++)
        for(int x=0; x<room_x; x++)
          f.read(r->block_at[y][x]);
        r->clearfov();
        r->infile = true;
        f.get<int>();
        }
      int id = f.get<int>();
      current_room = get_room_at(mapstream::cellbyid[id]);
      f.read(m.where_x);
      f.read(m.where_y);
      f.read(m.vel_x);
      f.read(m.vel_y);
      add_platf_hooks();
      println(hlog, "done");
      set_sval();

      cgi.prepare_shapes();
      prepare_tinf();
      init_scales();

      cmode = mode::playing;
      println(hlog, "set mode to playing");
      }
    });

}
