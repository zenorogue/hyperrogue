// Seuphorica frontend
// Copyright (C) 2025 Zeno Rogue, see 'hyper.cpp' for details
// the Seuphorica repo (seuphorica.cpp and dictionary files) needs to be placed in the 'seuphorica' subdirectory

#include "rogueviz.h"
#include <fstream>

namespace hr {

namespace seuphorica {
void read_dictionary(struct language& l);
void read_naughty_dictionary(language& l);
using std::stringstream;
using std::to_string;
using std::ostream;
}

#define NONJS
#include "seuphorica/seuphorica.cpp"

namespace seuphorica {

void compute_score();
void draw_board() { compute_score(); }

void read_dictionary(language& l) {
  fhstream f(find_file("rogueviz/seuphorica/" + l.fname), "rt");
  if(!f.f) { l.state = language_state::fetch_fail; compute_score(); return; }
  string s;
  while(scan(f, s)) 
    l.dictionary[utf8_length(s)].insert(s);
  l.dictionary[utf8_length(l.gamename)].insert(l.gamename);
  l.state = language_state::fetch_success;
  compute_score();
  }

void read_naughty_dictionary(language& l) {
  fhstream f(find_file("rogueviz/seuphorica/naughty-" + l.fname), "rt");
  if(!f.f) { l.state = language_state::fetch_fail; compute_score(); return; }
  string s;
  while(scan(f, s)) 
    l.naughty.insert(s);
  l.state = language_state::fetch_success;
  compute_score();
  }

coord from(cell *c) {
  auto co = euc2_coordinates(c);
  return coord(-co.first+7, -co.second+7);
  }

string fix(string s) {
  auto rep = [&] (string a, string b) {
    while(true) {
      auto pos = s.find(a);
      if(pos == string::npos) return;
      s.replace(pos, a.size(), b);
      }
    };
  rep("<b>", "");
  rep("</b>", "");
  rep("</b>", "");
  rep("🪙", "$");
  rep("<font color='#FF4040'>", "");
  rep("</font>", "");
  rep("<br/>", "\n");
  rep("<li> ", "  * ");
  rep("<li>", "* ");
  rep("</li>", "\n");
  rep("</ul>", "");
  rep("<ul>", "\n");
  return s;
  }

bool draw(cell *c, const shiftmatrix& V) {
  auto sco = from(c);
  if(!board.count(sco)) { c->wall = waNone; c->landparam = 0x202020; }
  else {
    auto& t = board.at(sco);
    c->wall = waNone; c->landparam = gsp(t).background;
    write_in_space(V /* ** ddspin(c,cdir,0)*/, 72, 1, t.letter, darkena(gsp(t).text_color, 0, 0xFF), 0, 8);
    }

  return false;
  }

const tile *tile_moved;
vector<tile>* box_moved;
int tile_boxid;

void render_tile(const shiftmatrix& V, const tile& t, vector<tile>* origbox, int boxid) {
  curvepoint(eupoint(-1, -1));
  curvepoint(eupoint(-1, +1));
  curvepoint(eupoint(+1, +1));
  curvepoint(eupoint(+1, -1));
  curvepoint(eupoint(-1, -1));
  queuecurve(V, 0xFF, darkena(gsp(t).background, 0, 0xFF), PPR::ZERO);
  write_in_space(V, 72, 2, t.letter, darkena(gsp(t).text_color, 0, 0xFF), 0, 8);
 
  auto h1 = inverse_shift_any(atscreenpos(0, 0), V * eupoint(-1, -1));
  auto h2 = inverse_shift_any(atscreenpos(0, 0), V * eupoint(+1, +1));
  if(mousex >= h1[0] && mousex <= h2[0] && mousey >= h1[1] && mousey <= h2[1] && !holdmouse) {
    mouseovers = fix(tile_desc(t));
    getcstat = dialog::list_fake_key++;
    dialog::add_key_action(getcstat, [&t, origbox, boxid] {
      holdmouse = true;
      tile_moved = &t; box_moved = origbox; tile_boxid = boxid;
      });
    }
  }

map<int, hyperpoint> where_is_tile;

vector<tile>* current_box;

struct tilebox {
  int x1, y1, x2, y2;
  vector<tile> *ptset;
  color_t col;
  tilebox(int x1, int y1, int x2, int y2, vector<tile>& tset, color_t col) : x1(x1), y1(y1), x2(x2), y2(y2), ptset(&tset), col(col) {}

  int get_margin() { return 40; }
  int get_space() { return 50; }

  bool good_location(hyperpoint h) {
    for(auto& t: *ptset) if(where_is_tile.count(t.id)) {
      auto d = h - where_is_tile[t.id];
      if(max(abs(d[0]), abs(d[1])) < get_space()) return false;
      }
    return true;
    }

  hyperpoint locate_tile(tile& t) {
    if(where_is_tile.count(t.id)) return where_is_tile[t.id];
    int margin = 40, space = 50;
    int px = x1 + margin, py = y1 + margin;
    while(true) {
      hyperpoint h = eupoint(px, py);
      if(good_location(h)) {
        println(hlog, tie(x1,y1), " located ", tile_desc(t), " at ", h, " coord = ", tie(x1,y1,x2,y2));
        return where_is_tile[t.id] = h;
        }
      px += space;
      if(px > x2 - margin) { px = x1 + margin; py += space; }
      }
    }

  void render(const string& title) {
    shiftmatrix ASP = atscreenpos(vid.xres - dialog::dwidth, 0);
    curvepoint(eupoint(x1+10, y1+10));
    curvepoint(eupoint(x1+10, y2-10));
    curvepoint(eupoint(x2-10, y2-10));
    curvepoint(eupoint(x2-10, y1+10));
    curvepoint(eupoint(x1+10, y1+10));

    auto h1 = inverse_shift_any(atscreenpos(0, 0), ASP * eupoint(x1+10, y1+10));
    auto h2 = inverse_shift_any(atscreenpos(0, 0), ASP * eupoint(x2-10, y2-10));
    if(mousex >= h1[0] + 10 && mousex <= h2[0] - 10 && mousey >= h1[1] + 10 && mousey <= h2[1] + 10) {
      current_box = ptset;
      }

    if(1) {
      dynamicval lw(vid.linewidth, vid.linewidth * 5);
      queuecurve(ASP, darkena(col, 0, 0xFF), darkena(col, 0, 0x80), PPR::ZERO);
      write_in_space(ASP * eupush(x2 - 10, y1 + 20), 72, 50, title, darkena(col, 0, 0xFF), 16, 16);
      }

    int idx = 0;
    for(auto& t: *ptset) {
      dynamicval lw(vid.linewidth, vid.linewidth * 3);
      dynamicval<ld> cs(cgi.scalefactor, 1);
      auto lt = locate_tile(t);
      if(&t == tile_moved && holdmouse) { idx++; continue; }
      if(lt[0] < x1 + get_margin() || lt[0] > x2 - get_margin() || lt[1] < y1 + get_margin() || lt[1] > y2 - get_margin()) {
        println(hlog, "relocating ", lt, " due to ", tie(x1,y1,x2,y2));
        where_is_tile.erase(t.id);
        lt = locate_tile(t);
        }
      render_tile(ASP * eupush(lt) * euscalexx(20), t, ptset, idx++);
      }
    }
  };

void seuphorica_screen() {

  getcstat = '-';
  cmode = 0;
  cmode = sm::SIDE | sm::DIALOG_STRICT_X | sm::MAYDARK;

  gamescreen();
  if(mouseover) {
    auto at = from(mouseover);
    if(board.count(at))
      mouseovers = fix(tile_desc(board.at(at)));
    }

  dialog::init();
  dialog::display();

  if(1) {
    int x1 = dialog::dwidth;

    flat_model_enabler fme;
    initquickqueue();
    dynamicval<eGeometry> g(geometry, gEuclid);

    current_box = nullptr;
    tilebox(0, 2*vid.fsize, x1/2, vid.yres/5, deck, 0xFF8000).render("bag");
    tilebox(x1/2, 2*vid.fsize, x1, vid.yres/5, discard, 0xFF0000).render("discard");
    tilebox(0, vid.yres/5, x1, vid.yres*2/5, drawn, 0x00FF00).render("hand");
    tilebox(0, vid.yres*2/5, x1, vid.yres*3/5, shop, 0xFFD500).render("shop");

    if(holdmouse) render_tile(atscreenpos(mousex, mousey) * euscalexx(20), *tile_moved, nullptr, 0);

    quickqueue();
    }

  stringstream ss;
  seuphorica::gamestats(ss); ss << ev.current_scoring;
  dialog::displayLong(fix(ss.str()), vid.fsize, vid.yres*3/5 + vid.fsize, false);

  int x0 = vid.xres - dialog::dwidth;
  if(ev.valid_move) {
    displayButton(lerp(x0, vid.xres, 1/6.), vid.yres - vid.fsize, just_placed.empty() ? str_skip_turn : str_play, SDLK_RETURN, 8);
    dialog::add_key_action(SDLK_RETURN, play);
    }

  displayButton(lerp(x0, vid.xres, 3/6.), vid.yres - vid.fsize, str_view_help, 'H', 8);
  dialog::add_key_action('H', [] { gotoHelp(fix(seuphorica::rules)); });

  displayButton(lerp(x0, vid.xres, 5/6.), vid.yres - vid.fsize, "menu", 'Q', 8);
  dialog::add_key_action('Q', [] { quitmainloop = true; });

  keyhandler = [] (int sym, int uni) { 
    handlePanning(sym, uni);
    dialog::handleNavigation(sym, uni);
    if(uni == SDLK_ESCAPE) popScreen();
    if(uni == PSEUDOKEY_RELEASE) {
      where_is_tile[tile_moved->id] = eupoint(mousex, mousey);
      if(box_moved == &shop && current_box == &drawn) {
        buy(tile_boxid);
        }
      if(box_moved == &drawn && current_box == &shop) {
        swap(drawn[tile_boxid], drawn[0]);
        back_to_shop();
        }
      if(box_moved == &drawn && current_box == nullptr) {
        auto at = from(mouseover);
        if(!board.count(at)) {
          swap(drawn[tile_boxid], drawn[0]);
          drop_hand_on(at.x, at.y);
          }
        }
      if(box_moved == &shop && current_box == nullptr && tile_moved->price <= cash) {
        auto at = from(mouseover);
        if(!board.count(at)) {
          buy(tile_boxid);
          drop_hand_on(at.x, at.y);
          }
        }
      }
    if(uni == '-' && mouseover && !holdmouse) {
      auto at = from(mouseover);
      if(board.count(at)) {
        back_from_board(at.x, at.y);
        holdmouse = true; tile_moved = &(drawn[0]); tile_boxid = 0; box_moved = &drawn;
        }
      else
        drop_hand_on(at.x, at.y);
      }
    };
  }

void launch() {  
  stop_game();
  enable_canvas();
  ccolor::set_plain_nowall(0x202020);
  start_game();
  View = Id;
  restart("", "", "");

  showstartmenu = false;
  mapeditor::drawplayer = false;
  }

void enable() {
  rogueviz::rv_hook(hooks_drawcell, 100, draw);
  }

auto seuphorica_hook = arg::add3("-seuphorica", [] { launch(); enable(); pushScreen(seuphorica_screen); });

}
}
