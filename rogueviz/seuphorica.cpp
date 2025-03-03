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
void draw_board() {
  minx=15, miny=15, maxx=0, maxy=0;
  for(auto& b: board) if(!just_placed.count(b.first)) minx = min(minx, b.first.x), maxx = max(maxx, b.first.x), miny = min(miny, b.first.y), maxy = max(maxy, b.first.y);
  miny -= 6; minx -= 6; maxx += 7; maxy += 7;
  compute_score();
  }

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

int hold_mode; /* 1 = from board, 2 = from set, 3 = drag size, 4 = spell drag */
tile *tile_moved;
vector<tile>* box_moved;
int tile_boxid;
cell *tile_moved_from;
int *drag_what;

bool tiles3 = true;

struct wider {
  dynamicval<ld> lw;
  wider(ld x) : lw(vid.linewidth, vid.linewidth * x) {}
  };

/** for tiles on the map, only (V,t,c) are defined; for tiles in boxes, (V,t,origbox,boxid) are defined */
void render_tile(const shiftmatrix& V, tile& t, cell *c, vector<tile>* origbox, int boxid) {

  auto pt0 = [&] (int id, ld r) {
    if(c) return currentmap->get_corner(c, id+1, r);
    return spin(-90._deg * id) * eupoint(-3/r, -3/r);
    };

  auto pt = [&] (ld id, ld r) {
    int id0 = int(id);
    if(id == id0) return pt0(id0, r);
    auto p1 = pt0(id0, r), p2 = pt0(id0+1, r);
    return lerp(p1, p2, id-id0);
    };

  int corners = c ? c->type : 4;

  color_t back = darkena(gsp(t).background, 0, 0xFF);

  auto V1 = V;

  color_t lines = 0x000000FF;
  int wide = 1;
  if(t.rarity == 2) lines = 0xFF4040FF, wide = 2;
  if(t.rarity == 3) lines = 0xFFFF80FF, wide = 2;
  if(t.rarity >= 4) lines = 0x40FF80FF, wide = 2;
  if(t.special >= sp::first_artifact) lines = 0xFFD500FF, wide = 2;
  dynamicval<color_t> co(poly_outline, lines);

  if(c && tiles3) {
    wider w(wide);
    set_floor(cgi.shFullFloor);
    for(int i=0; i<c->type; i++)
      if(!board.count(from(c->move(i)))) placeSidewall(c, i, SIDE_SLEV, V, back);
    V1 = orthogonal_move_fol(V, cgi.SLEV[1]);
    draw_qfi(c, V1, back, PPR::WALL3A);
    }
  else {
    wider w(wide);
    for(int i=0; i<=corners; i++) curvepoint(pt(i, 3));
    queuecurve(V, lines, back, PPR::ITEM);
    }

  const ld nearco = 4;

  if(has_power(t, sp::bending)) {
    wider w(3);
    queueline(V1 * pt0(0, nearco), V1 * pt0(corners/2, nearco), 0xC0C0FFFF, 2);
    }

  if(has_power(t, sp::portal)) {
    wider w(3);
    queueline(V1 * pt0(1, nearco), V1 * pt0(1+corners/2, nearco), 0xFF8000FF, 2);
    }

  if(has_power(t, sp::horizontal)) {
    wider w(3);
    queueline(V1 * pt0(0, nearco), V1 * pt0(1, nearco), 0xFFFFFFFF, 2);
    queueline(V1 * pt0(3, nearco), V1 * pt0(2, nearco), 0xFFFFFFFF, 2);
    }

  if(has_power(t, sp::vertical)) {
    wider w(3);
    queueline(V1 * pt0(0, nearco), V1 * pt0(3, nearco), 0xFFFFFFFF, 2);
    queueline(V1 * pt0(1, nearco), V1 * pt0(2, nearco), 0xFFFFFFFF, 2);
    }

  if(has_power(t, sp::initial)) {
    wider w(3);
    queueline(V1 * pt0(0, nearco), V1 * pt0(1, nearco), 0x000000FF, 2);
    queueline(V1 * pt0(0, nearco), V1 * pt0(corners-1, nearco), 0x000000FF, 2);
    }

  if(has_power(t, sp::final)) {
    int ch = corners/2;
    wider w(3);
    queueline(V1 * pt(ch-0.3, nearco), V1 * pt0(ch-1, nearco), 0x000000FF, 2);
    queueline(V1 * pt(ch+0.3, nearco), V1 * pt0(ch+1, nearco), 0x000000FF, 2);
    }

  if(has_power(t, sp::symmetric)) {
    wider w(2);
    for(int i=0; i<corners; i++)
      queueline(V1 * pt(i+0.3, nearco), V1 * pt(i+0.7, nearco), 0xC0C0C0FF, 2);
    }

  write_in_space(V1, 72, c ? 1 : 3, t.letter, darkena(gsp(t).text_color, 0, 0xFF), 0, 8);
  write_in_space(V1 * eupush(pt0(2, 4.5)), 72, c ? 0.4 : 1.2, its(t.value), darkena(gsp(t).text_color, 0, 0xFF), 0, 8);

  if(!c) {
    auto h1 = inverse_shift_any(atscreenpos(0, 0), V * eupoint(-1, -1));
    auto h2 = inverse_shift_any(atscreenpos(0, 0), V * eupoint(+1, +1));
    if(mousex >= h1[0] && mousex <= h2[0] && mousey >= h1[1] && mousey <= h2[1] && !holdmouse) {
      mouseovers = fix(tile_desc(t));
      getcstat = dialog::list_fake_key++;
      dialog::add_key_action(getcstat, [&t, origbox, boxid] {
        holdmouse = true; hold_mode = 2;
        tile_moved = &t; box_moved = origbox; tile_boxid = boxid;
        });
      }
    if(mousex >= h1[0] && mousex <= h2[0] && mousey >= h1[1] && mousey <= h2[1] && holdmouse && hold_mode == 4) {
      if(origbox == &drawn && hold_mode == 4 && holdmouse) {
        mouseovers = "cast " + spell_desc(tile_boxid, -1) + " on: " + fix(tile_desc(t));
        tile_moved = &t;
        }
      else mouseovers = "can cast spells only on tiles in hand";
      }
    }
  }

bool draw(cell *c, const shiftmatrix& V) {
  auto sco = from(c);
  bool inside = (sco.x >= minx && sco.x <= maxx && sco.y >= miny && sco.y <= maxy);
  if(inside) {
    c->wall = waNone; c->landparam = 0x202020;
    }
  else
    c->wall = waChasm;
  if(board.count(sco)) {
    auto& t = board.at(sco);
    render_tile(V, t, c, nullptr, -1);
    }
  if(!inside) return false;

  int co = get_color(sco);
  if(co == beRed || co == beBlue)
    queuepoly(V, cgi.shTriangle, co == beRed ? 0xFF0000FF : 0x0000FFFF);
  if(co == bePower)
    queuepoly(V, cgi.shStar, 0x408040FF);
  if(co == beStay)
    queuepoly(V, cgi.shPirateX, 0x303030FF);
  if(co >= beSpell) {
    spell& sp = spells[co - beSpell];
    write_in_space(V, 72, 1, sp.greek, darkena(sp.color_value, 0, 0xFF), 0, 8, PPR::ITEM);
    }

  return false;
  }

map<int, hyperpoint> where_is_tile;

vector<tile>* current_box;

string describe_color(int co) {
  if(co == beRed) return "Red spot";
  if(co == beBlue) return "Blue spot";
  if(co == beStay) return "Stay spot";
  if(co == bePower) return "Power spot";
  if(co >= beSpell) return spell_desc(co - beSpell);
  return "?";
  }

struct uicoords {
  int x0, x1, x2;
  int y0, y1, y2, y3;
  } ui;

struct tilebox {
  int *x1, *y1, *x2, *y2;
  vector<tile> *ptset;
  color_t col;
  tilebox(int& x1, int& y1, int& x2, int& y2, vector<tile>& tset, color_t col) : x1(&x1), y1(&y1), x2(&x2), y2(&y2), ptset(&tset), col(col) {}

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
    int px = *x1 + margin, py = *y1 + margin;
    while(true) {
      hyperpoint h = eupoint(px, py);
      if(good_location(h)) {
        if(h[1] > *y2 - get_margin()) *y2 = h[1] + get_margin();
        return where_is_tile[t.id] = h;
        }
      px += space;
      if(px > *x2 - margin) { px = *x1 + margin; py += space; }
      }
    }

  void render(const string& title) {
    shiftmatrix ASP = atscreenpos(0, 0);

    for(auto& t: *ptset) {
      auto lt = locate_tile(t);
      if(lt[0] < *x1 + get_margin() || lt[0] > *x2 - get_margin() || lt[1] < *y1 + get_margin() || lt[1] > *y2 - get_margin()) {
        where_is_tile.erase(t.id);
        lt = locate_tile(t);
        }
      }

    curvepoint(eupoint(*x1+10, *y1+10));
    curvepoint(eupoint(*x1+10, *y2-10));
    curvepoint(eupoint(*x2-10, *y2-10));
    curvepoint(eupoint(*x2-10, *y1+10));
    curvepoint(eupoint(*x1+10, *y1+10));

    auto h1 = inverse_shift_any(atscreenpos(0, 0), ASP * eupoint(*x1+10, *y1+10));
    auto h2 = inverse_shift_any(atscreenpos(0, 0), ASP * eupoint(*x2-10, *y2-10));
    if(mousex >= h1[0] + 10 && mousex <= h2[0] - 10 && mousey >= h1[1] + 10 && mousey <= h2[1] + 10) {
      current_box = ptset;
      }

    if(1) {
      wider wid(5);
      queuecurve(ASP, darkena(col, 0, 0xFF), darkena(col, 0, 0x80), PPR::ZERO);
      write_in_space(ASP * eupush(*x2 - 10, *y1 + 20), 72, 50, title, darkena(col, 0, 0xFF), 16, 16);
      if(isize(*ptset))
        write_in_space(ASP * eupush(*x2 - 10, *y1 + 40), 72, 25, its(isize(*ptset)), darkena(col, 0, 0xFF), 16, 16);
      if(ptset == &shop)
        write_in_space(ASP * eupush(*x2 - 10, *y2 - 10), 72, 50, its(cash) + "$", darkena(col, 0, 0xFF), 16, 16);
      }

    int idx = 0;
    for(auto& t: *ptset) {
      dynamicval<ld> cs(cgi.scalefactor, 1);
      auto lt = locate_tile(t);
      if(&t == tile_moved && holdmouse) { idx++; continue; }

      if(ptset == &drawn && idx < ev.retain_count) {
        auto T = ASP * eupush(lt) * euscalexx(20) * spin(45._deg);
        for(int i=0; i<=4; i++) curvepoint(spin(90._deg*i) * eupoint(1,1));
        queuecurve(T, darkena(col, 0, 0xFF), darkena(col, 0, 0x80), PPR::ZERO);
        }

      render_tile(ASP * eupush(lt) * euscalexx(20), t, nullptr, ptset, idx++);
      }
    }
  };

tilebox tb_deck(ui.x0, ui.y0, ui.x1, ui.y1, deck, 0xFF8000);
tilebox tb_discard(ui.x1, ui.y0, ui.x2, ui.y1, discard, 0xFF0000);
tilebox tb_hand(ui.x0, ui.y1, ui.x2, ui.y2, drawn, 0x00FF00);
tilebox tb_shop(ui.x0, ui.y2, ui.x2, ui.y3, shop, 0xFFD500);

void sort_hand() {
  sort(drawn.begin(), drawn.end(), [] (tile &t1, tile &t2) {
    auto h1 = tb_hand.locate_tile(t1);
    auto h2 = tb_hand.locate_tile(t2);
    if(h1[1] != h2[1]) return h1[1] < h2[1];
    return h1[0] < h2[0];
    });
  }

void seuphorica_screen() {

  getcstat = '-';
  cmode = 0;
  cmode = sm::SIDE | sm::DIALOG_STRICT_X | sm::MAYDARK;

  gamescreen();
  if(mouseover) {
    auto at = from(mouseover);
    auto co = get_color(at);
    if(board.count(at)) {
      mouseovers = fix(tile_desc(board.at(at)));
      if(co) mouseovers = mouseovers + ", " + describe_color(co);
      }
    else if(co) mouseovers = describe_color(co);
    }

  dialog::init();
  dialog::display();

  if(holdmouse && hold_mode == 3) *drag_what = mousey;

  if(holdmouse && hold_mode == 4) {
    displaystr(mousex, mousey, 0, vid.fsize, spells[tile_boxid].greek, spells[tile_boxid].color_value, 8);
    tile_moved = nullptr;
    }

  ui.x0 = vid.xres - dialog::dwidth; ui.x2 = vid.xres; ui.x1 = lerp(ui.x0, ui.x2, .5);
  int ymax = vid.yres - 2 * vid.fsize;
  ui.y0 = 2 * vid.fsize;
  if(!ui.y1) {
    ui.y1 = lerp(ui.y0, ymax, .2);
    ui.y2 = lerp(ui.y0, ymax, .4);
    ui.y3 = lerp(ui.y0, ymax, .6);
    }
  auto sep = [&] (int& y1, int& y2) {
    ld err = (ymax - ui.y0) / 10 - (y2-y1);
    if(err < 0) return;
    y1 -= err/2; y2 += err/2;
    };
  sep(ui.y0, ui.y1); sep(ui.y1, ui.y2); sep(ui.y2, ui.y3);

  if(1) {

    flat_model_enabler fme;
    initquickqueue();
    dynamicval<eGeometry> g(geometry, gEuclid);

    current_box = nullptr;

    tb_deck.render("bag");
    tb_discard.render("discard");
    tb_hand.render("hand");
    tb_shop.render("shop");

    for(auto y: {&ui.y1, &ui.y2, &ui.y3}) {
      // println(hlog, mousex >= ui.x0, ", ", abs(mousey - *y));
      if(mousex >= ui.x0 && abs(mousey - *y) < 5) {

        dynamicval<ld> dl(vid.linewidth, vid.linewidth * 3);
        queueline(atscreenpos(ui.x0, *y) * C0, atscreenpos(ui.x2, *y) * C0, 0xFF0000FF, 0);
        getcstat = '0';
        dialog::add_key_action('0', [=] { holdmouse = true; hold_mode = 3; drag_what = y; });
        }
      }

    if(holdmouse && among(hold_mode, 1, 2)) render_tile(atscreenpos(mousex, mousey) * euscalexx(20), *tile_moved, nullptr, nullptr, 0);

    quickqueue();
    }

  int in_row = (isize(spells) + 1) / 2;
  for(int id=0; id<isize(spells); id++) {
    auto& sp = spells[id];
    if((sp.identified && enabled_id) || sp.inventory) {
      if(displayfr(lerp(ui.x0, ui.x2, (id % in_row+1.)/(1+in_row)), ui.y3 + vid.fsize/2 + (id/in_row * vid.fsize), 1, vid.fsize, sp.greek + ": " + its(sp.inventory), sp.color_value, 8)) {
        mouseovers = spell_desc(id, sp.inventory);
        getcstat = 'C';
        dialog::add_key_action('C', [=] { holdmouse = true; hold_mode = 4; tile_boxid = id; });
        }
      }
    }

  stringstream ss;
  seuphorica::gamestats(ss); ss << ev.current_scoring;
  int size = vid.fsize;
  while(true) {
    if(size <= 3) break;
    auto ny = dialog::displayLong(fix(ss.str()), size, ui.y3 + vid.fsize * 3, true);
    if(ny <= ymax) break;
    size = (size * 9) / 10;
    }
  dialog::displayLong(fix(ss.str()), size, ui.y3 + vid.fsize * 3, false);

  if(ev.valid_move) {
    displayButton(lerp(ui.x0, ui.x2, 1/6.), vid.yres - vid.fsize, just_placed.empty() ? str_skip_turn : str_play, SDLK_RETURN, 8);
    dialog::add_key_action(SDLK_RETURN, play);
    }

  displayButton(lerp(ui.x0, ui.x2, 3/6.), vid.yres - vid.fsize, str_view_help, 'H', 8);
  dialog::add_key_action('H', [] { gotoHelp(fix(seuphorica::rules)); });

  displayButton(lerp(ui.x0, ui.x2, 5/6.), vid.yres - vid.fsize, "menu", 'Q', 8);
  dialog::add_key_action('Q', [] { quitmainloop = true; });

  keyhandler = [] (int sym, int uni) {
    handlePanning(sym, uni);
    dialog::handleNavigation(sym, uni);
    if(uni == SDLK_ESCAPE) popScreen();
    if(uni == PSEUDOKEY_RELEASE && hold_mode == 4 && tile_moved) {
      swap(*tile_moved, drawn[0]);
      cast_spell(tile_boxid);
      }
    if(uni == PSEUDOKEY_RELEASE && among(hold_mode, 1, 2)) {
      where_is_tile[tile_moved->id] = eupoint(mousex, mousey);
      if(box_moved == &shop && current_box == &drawn) {
        buy(tile_boxid);
        sort_hand();
        }
      if(box_moved == &drawn && current_box == &drawn) {
        sort_hand();
        return;
        }
      if(box_moved == &drawn && current_box == &shop) {
        swap(drawn[tile_boxid], drawn[0]);
        back_to_shop();
        sort_hand();
        }
      if(box_moved == &drawn && current_box == nullptr && tile_moved_from == mouseover && hold_mode == 1) {
        /* do nothing, it was already removed from the board */
        sort_hand();
        return;
        }
      if(box_moved == &drawn && current_box == nullptr) {
        auto at = from(mouseover);
        if(!board.count(at)) {
          swap(drawn[tile_boxid], drawn[0]);
          drop_hand_on(at.x, at.y);
          sort_hand();
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
        back_from_board(at.x, at.y); hold_mode = 1; tile_moved_from = mouseover;
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
  color_descs = false;
  }

void enable() {
  rogueviz::rv_hook(hooks_drawcell, 100, draw);
  }

auto seuphorica_hook = arg::add3("-seuphorica", [] { launch(); enable(); pushScreen(seuphorica_screen); });

}
}
