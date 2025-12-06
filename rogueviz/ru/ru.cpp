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
#include "staters.cpp"
#include "geometry.cpp"
#include "entity.cpp"
#include "man.cpp"
#include "room.cpp"
#include "render.cpp"
#include "portals.cpp"
#include "powers.cpp"
#include "load-world.cpp"
#include "stats.cpp"
#include "randeff.cpp"
#include "reverts.cpp"
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
    current_room->place_block_full(mousepx / block_x, mousepy / block_y, 8*sel), current_room->edited = true;
  if(keyheld('2'))
    current_room->place_block_full(mousepx / block_x, mousepy / block_y, 8*sel+4), current_room->edited = true;
  if(keyheld('3'))
    current_room->place_block_full(mousepx / block_x, mousepy / block_y, 0), current_room->edited = true;
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
  if(keypressed('t')) { m.where = xy(mousepx, mousepy); m.vel = xy(0, 0); current_room->edited = true; m.existing = true; m.hp = m.max_hp(); }
  }

/* new w, velocity multiplier, neighbor id */
tuple<xy, ld, int> get_next_room(xy w, room *r, int which) {
  auto chk = [&] (bool b, int rid) {
    return which == rid || (which == -1 && b);
    };

  if(chk(w.x < l_margin_at, 2)) {
    w.x += actual_screen_x;
    return {w, 1, 2};
    }
  if(chk(w.x > r_margin_at, non_hyperbolic ? 0 : 4)) {
    w.x -= actual_screen_x;
    return {w, 1, non_hyperbolic ? 0 : 4};
    }

  xy w1;
  w1.y = (w.y - t_margin_at) * 2 + b_margin_at;
  w1.x = 2 * (w.x - l_margin_at);

  if(chk(w.y < t_margin_at && !non_hyperbolic && w1.x <= actual_screen_x, 1)) {
    w1.x += l_margin_at;
    return {w1, 2, 1};
    }
  if(chk(w.y < t_margin_at && !non_hyperbolic && w1.x > actual_screen_x, 0)) {
    w1.x -= actual_screen_x;
    w1.x += l_margin_at;
    return {w1, 2, 0};
    }

  if(chk(w.y > b_margin_at && !non_hyperbolic, 3)) {
    w.x -= l_margin_at;
    w.y -= b_margin_at;
    w.y /= 2;
    w.y += t_margin_at;
    if(is_right(r))
      w.x += actual_screen_x;
    w.x /= 2;
    w.x += l_margin_at;
    return {w, .5, 3};
    }

  return {w, 1, -1};
  }

void playing_frame() {
  m.act();

  auto& ents = current_room->entities;

  for(auto& e: ents) if(e->existing) e->act(); else e->unact();

  auto mb = ents.begin();
  for(auto& e: ents) if(!e->destroyed) *(mb++) = std::move(e);
  ents.resize(mb - ents.begin());

  if(one_room) return;

  auto [w1, vmul, cr] = get_next_room(m.where, current_room);

  if(cr != -1) {
    m.where = w1;
    m.vel *= vmul;
    switch_to_adjacent_room(cr);
    }
  }

void sync_map() {
  for(auto& p: rooms)
    if(&p.second == current_room)
      centerover = p.first;
  View = cspin90(1, 0);
  // if(cmode == mode::playing) View = View * inverse(parabolic13_at(to_hyper(m.where_x, m.where_y)));
  if(cmode == mode::playing) {
    hyperpoint p = to_hyper(m.where);
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
      dialog::add_key_action('v', [] { save(); cmode = mode::menu; });
      break;
    case mapmode::poincare:
    case mapmode::klein:
      gamescreen();
      if(!mouseout() && mouseover && rooms.count(mouseover) && anyshiftclick) {
        current_room = &rooms[mouseover];
        }
      if(!mouseout()) {
        auto h = inverse_shift(ggmatrix(current_room->where), mouseh);
        auto fh = from_hyper(h);
        tie(mousepx, mousepy) = pair(fh.x, fh.y);
        }
      if(cmode == mode::editmap) {
        getcstat = '-';
        dialog::add_key_action('-', [] { if(!mouseover || !rooms.count(mouseover)) return; current_room = &rooms[mouseover]; switch_mapmode_to(mapmode::standard); });
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
      if(cmode == mode::editmap) {
        mouseovers = format("coordinates: %d %d (%.2lf) (hdist=%.2lf)", mousepx, mousepy, double(get_scale_at(mousepy)), hdist(to_hyper(m.where), to_hyper(xy(mousepx, mousepy))));
        auto [w, vmul, id] = get_next_room(xy(mousepx, mousepy), current_room);
        if(id != -1) mouseovers += format(" (%.1lf %.1lf %d)", w.x, w.y, id);
        }
      if(cmode == mode::playing) {
        titlecolor = 0xFFFFFF;
        mouseovers = current_room->roomname;
        displayfr(vid.fsize, vid.fsize, 2, vid.fsize, "HP " + its(m.hp) + "/" + its(m.max_hp()), titlecolor, 0);
        if(current_target && current_target->existing)
        displayfr(vid.xres - vid.fsize, vid.fsize, 2, vid.fsize, "HP " + its(current_target->hp) + "/" + its(current_target->max_hp()) + " " + current_target->get_name(), titlecolor, 16);
        }
      if(cmode == mode::paused) {
        xy mousep(mousepx, mousepy);
        string helpstr = "";
        entity* help_entity = nullptr;

        if(contains(m.get_pixel_bbox(), mousep)) {
          help_entity = &m;
          }

        for(auto& e: current_room->entities)
          if(e->existing && e->visible(current_room) && e->visible_inv() && e->have_help())
            if(contains(e->get_pixel_bbox(), mousep))
              help_entity = &*e;

        if(help_entity) {
          mouseovers = help_entity->get_name();
          helpstr = help_entity->get_help();
          }

        int x = mousepx / block_x, y = mousepy / block_y;
        if(!help_entity && x >= 0 && y >= 0 && x < room_x && y < room_y) {
          if(!current_room->fov[y][x]) mouseovers = "invisible", helpstr = "You need to explore to see what is there.";
          else {
            auto&w = walls[current_room->block_at[y][x] >> 3];
            mouseovers = w.name;
            helpstr = w.help;
            }
          }

        dialog::add_key_action(SDLK_F1, [helpstr] { gotoHelp(helpstr); });
        }
      draw_pentagon();
      break;

    case mode::menu:
      render_the_map();
      draw_inventory_frame();
      dialog::init();
      dialog::addTitle("Fountains of Alchemy", 0x4040C0, 150);
      dialog::addItem("return to game", 'v');
      dialog::add_action([] { cmode = mode::playing; });

      dialog::addItem("inventory", 'i');
      dialog::add_action_push(draw_inventory);

      dialog::addItem("character", 'k');
      dialog::add_action_push(draw_stats);

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

string parse_markup(string s) {
  while(s.find("\\n") != string::npos) s.replace(s.find("\\n"), 2, "\n");
  while(s.find("[key:") != string::npos) {
    int kf = s.find("[key:");
    int ke = s.find("]", kf);
    string what = s.substr(kf+5, ke-kf-5);
    bool ok = false;
    for(auto& p: powers) if(p.name == what) {
      s.replace(kf, ke-kf+1, dialog::keyname(p.key));
      ok = true;
      break;
      }
    if(!ok) return s;
    }
  return s;
  }

void enable() {
  
  stop_game();
  
  set_sval();
  init_scales();
  gen_powers();
  shuffle_all();
  
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

    ld tscale = 1;
    if(dexmode->flags & ACTIVE)
      tscale *= 10. / (10 + m.current.stats[stat::dex]);

    gtime += d * tscale;
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
    f.write(m.where.x);
    f.write(m.where.y);
    f.write(m.vel.x);
    f.write(m.vel.y);
    });

  pushScreen(run);  
  }

void start_new_game() {
  enable();
  randomize_stats();
  cmode = mode::menu;
  pushScreen(initial_stats);
  }

auto chk = arg::add3("-ru", enable)
  + arg::add3("-ru-start", start_new_game)
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
      f.read(m.where.x);
      f.read(m.where.y);
      f.read(m.vel.x);
      f.read(m.vel.y);
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
