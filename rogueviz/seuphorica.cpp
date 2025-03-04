// Seuphorica frontend
// Copyright (C) 2025 Zeno Rogue, see 'hyper.cpp' for details
// the Seuphorica repo (seuphorica.cpp and dictionary files) needs to be placed in the 'seuphorica' subdirectory

#include "rogueviz.h"
#include <fstream>

namespace hr {

namespace seuphorica {
void read_dictionary(struct language& l);
void read_naughty_dictionary(language& l);
void activate_scry();
using std::stringstream;
using std::to_string;
using std::ostream;

void snapshot();
void from_map(struct coord co, struct tile& t);
void is_clone(struct tile& orig, struct tile& clone);
}

#define NONJS
#include "seuphorica/seuphorica.cpp"

namespace seuphorica {

void compute_score();

void draw_board() {
  compute_size();
  compute_score();
  turncount++; // to clean messages
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
  return coord(7-co.first, 7-co.second);
  }

cell* to(coord co) {
  auto& m = euc::get_spacemap();
  return m[euc::coord(7-co.x, 7-co.y, 0)]->c7;
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

string describe_color(int co) {
  if(co == beRed) return "Red spot";
  if(co == beBlue) return "Blue spot";
  if(co == beStay) return "Stay spot";
  if(co == bePower) return "Power spot";
  if(co >= beSpell) return spell_desc(co - beSpell);
  return "?";
  }

void push_tile_info_screen(tile &t, cell *c, vector<tile>* origbox, int boxid) {
  if(&t == &empty_tile) {
     int co = get_color(from(c));
     gotoHelp(helptitle("map spot", 0xC000C0) + "Map spots have special powers. Try placing a tile here and see the description for the effects.\n\nThis is: " + describe_color(co));
     return;
     }
  string s;
  s = helptitle(short_desc(t), gsp(t).background);
  s = s + fix(tile_desc(t));
  if(c) {
    int co = get_color(from(c));
    if(co) s += "\n\nMap spot: " + describe_color(co);
    }
  gotoHelp(s);
  if(has_power(t, sp::wild) && origbox == &drawn) {
    auto lang = get_language(t);
    if(!lang) lang = current;
    char next_letter = 'A';
    for(auto letter: lang->alphabet) {
      char this_letter = 0;
      if(letter.size() == 1 && letter[0] >= 'A' && letter[0] <= 'Z') this_letter = letter[0] + 32;
      else this_letter = next_letter++;
      help_extensions.push_back(help_extension{this_letter, "become " + letter, [letter, boxid] () { wild_become(boxid, letter.c_str()); popScreen(); }});
      }
    }
  }

/** for tiles on the map, only (V,t,c) are defined; for tiles in boxes, (V,t,origbox,boxid) are defined */
void render_tile(shiftmatrix V, tile& t, cell *c, vector<tile>* origbox, int boxid) {

  bool gig = has_power(t, sp::gigantic);

  auto pt0 = [&] (int id, ld r) {
    if(gig) r /= 3;
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
    if(!gig) draw_qfi(c, V1, back, PPR::WALL3A);
    }
  else {
    wider w(wide);
    for(int i=0; i<=corners; i++) curvepoint(pt(i, 3));
    queuecurve(V, lines, back, PPR::ITEM);
    }

  if(c && gig) {
    if(gigants.at(from(c)) != from(c) - coord{1,1}) return;
    draw_qfi(c, V1 * euscalexx(3), back, PPR::WALL3A);
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

  int gigscale = gig ? 3 : 1;

  write_in_space(V1, 72, (c ? 1 : 3) * gigscale, t.letter, darkena(gsp(t).text_color, 0, 0xFF), 0, 8);
  write_in_space(V1 * eupush(pt0(2, 4.5)), 72, (c ? 0.4 : 1.2) * gigscale, its(t.value), darkena(gsp(t).text_color, 0, 0xFF), 0, 8);

  if(!c && origbox) {
    auto h1 = inverse_shift_any(atscreenpos(0, 0), V * eupoint(-gigscale, -gigscale));
    auto h2 = inverse_shift_any(atscreenpos(0, 0), V * eupoint(+gigscale, +gigscale));
    if(mousex >= h1[0] && mousex <= h2[0] && mousey >= h1[1] && mousey <= h2[1] && !holdmouse) {
      mouseovers = fix(tile_desc(t));
      getcstat = dialog::list_fake_key++;
      dialog::add_key_action(getcstat, [&t, origbox, boxid] {
        holdmouse = true; hold_mode = 2;
        tile_moved = &t; box_moved = origbox; tile_boxid = boxid;
        });
      dialog::add_key_action(SDLK_F1, [&t, c, origbox, boxid] {
        push_tile_info_screen(t, c, origbox, boxid);
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

  if(portals.count(sco)) {
    auto sco1 = portals.at(sco);
    auto c1 = to(sco1);
    for(const shiftmatrix& V1: hr::span_at(current_display->all_drawn_copies, c1)) {
      queueline(V * currentmap->get_corner(c, 2, 4), V1 * currentmap->get_corner(c1, 2, 4), 0xFF800080);
      queueline(V * currentmap->get_corner(c, 2+c->type/2, 4), V1 * currentmap->get_corner(c1, 2+c->type/2, 4), 0x0000FF80);

      if(tiles3) {
        auto high_V = orthogonal_move_fol(V, cgi.SLEV[1]);
        auto high_V1 = orthogonal_move_fol(V1, cgi.SLEV[1]);
        queueline(high_V * currentmap->get_corner(c, 2, 4), high_V1 * currentmap->get_corner(c1, 2, 4), 0xFF800080);
        queueline(high_V * currentmap->get_corner(c, 2+c->type/2, 4), high_V1 * currentmap->get_corner(c1, 2+c->type/2, 4), 0x0000FF80);
        }
      }
    }

  int co = get_color(sco);
  if(co == beRed || co == beBlue)
    queuepoly(V, cgi.shTriangle, darkena(co == beRed ? 0xFF0000 : 0x0000FF, 0, 0xFF));
  if(co == bePower)
    queuepoly(V, cgi.shStar, darkena(0x408040, 0, 0xFF));
  if(co == beStay)
    queuepoly(V, cgi.shPirateX, darkena(0x303030, 0, 0xFF));
  if(co >= beSpell) {
    spell& sp = spells[co - beSpell];
    write_in_space(V, 72, 1, sp.greek, darkena(darkened(sp.color_value), 0, 0xFF), 0, 8, PPR::ITEM);
    }

  return false;
  }

map<int, hyperpoint> where_is_tile;

void activate_scry() {
  for(auto& t: deck) where_is_tile.erase(t.id);
  }

vector<tile>* current_box;

struct uicoords {
  int x0, x1, x2;
  int y0, y1, y2, y3;
  } ui;

using snaptile = pair<tile, hyperpoint>;

vector<map<int, snaptile>> snapshots;

struct tilebox {
  int *x1, *y1, *x2, *y2;
  vector<tile> *ptset;
  color_t col;
  tilebox(int& x1, int& y1, int& x2, int& y2, vector<tile>& tset, color_t col) : x1(&x1), y1(&y1), x2(&x2), y2(&y2), ptset(&tset), col(col) {}

  int get_margin() { return 40; }
  int get_space() { return 50; }
  int get_margin_for(tile &t) { return 40 + (has_power(t, sp::gigantic) ? 50 : 0); }

  bool in_bounds(hyperpoint h, tile &t1, bool req_down) {
    int ex = get_margin_for(t1);
    return h[0] >= *x1 + ex && h[0] <= *x2 - ex && h[1] >= *y1 + ex && (!req_down || h[1] <= *y2 + ex);
    }

  bool good_location(hyperpoint h, tile &t1) {
    for(auto& t: *ptset) if(where_is_tile.count(t.id)) {
      auto d = h - where_is_tile[t.id];
      if(max(abs(d[0]), abs(d[1])) < get_space() + (has_power(t1, sp::gigantic) ? 20:0) + (has_power(t, sp::gigantic) ? 20:0)) return false;
      }
    return true;
    }

  hyperpoint locate_tile(tile& t) {
    if(where_is_tile.count(t.id)) return where_is_tile[t.id];
    int margin = get_margin_for(t), space = get_space();
    int px = *x1 + margin, py = *y1 + margin;
    while(true) {
      hyperpoint h = eupoint(px, py);
      if(good_location(h, t)) {
        if(h[1] > *y2 - margin) *y2 = h[1] + margin;
        return where_is_tile[t.id] = h;
        }
      px += space;
      if(px > *x2 - margin) { px = *x1 + margin; py += space; }
      }
    }

  void locate_all() {
    for(auto& t: *ptset) {
      auto lt = locate_tile(t);
      if(!in_bounds(lt, t, true)) {
        where_is_tile.erase(t.id);
        lt = locate_tile(t);
        }
      }
    }

  void render(const string& title) {
    locate_all();

    shiftmatrix ASP = atscreenpos(0, 0);
    curvepoint(eupoint(*x1+10, *y1+10));
    curvepoint(eupoint(*x1+10, *y2-10));
    curvepoint(eupoint(*x2-10, *y2-10));
    curvepoint(eupoint(*x2-10, *y1+10));
    curvepoint(eupoint(*x1+10, *y1+10));

    auto h1 = inverse_shift_any(atscreenpos(0, 0), ASP * eupoint(*x1+10, *y1+10));
    auto h2 = inverse_shift_any(atscreenpos(0, 0), ASP * eupoint(*x2-10, *y2-10));
    if(mousex >= h1[0] + 10 && mousex <= h2[0] - 10 && mousey >= h1[1] + 10 && mousey <= h2[1] + 10) {
      current_box = ptset;
      mouseovers = "";
      dialog::add_key_action(SDLK_F1, [this] {
        if(ptset == &drawn) gotoHelp("This is your hand. You can play these tiles.");
        if(ptset == &deck) gotoHelp("This is your bag of tiles. Every turn, you drawn a number of random tiles from the bag.");
        if(ptset == &discard) gotoHelp("This is your discard pile. After every turn, all tiles (used or unused) go to the discard pile. They return to your bag when the bag is empty.");
        if(ptset == &shop) gotoHelp("This is the shop. You can buy tiles for your cash, by dragging them to the board or your hand.\n\nYou cannot sell tiles, except to cancel your purchase on the same turn.");
        });
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

    if(hr::isize(snapshots)) return;

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

void snapshot() {
  auto& b = snapshots.emplace_back();
  for(auto tb: {&tb_deck, &tb_shop, &tb_discard, &tb_hand}) {
    tb->locate_all();
    for(auto& t: *(tb->ptset)) b.emplace(t.id, snaptile{t, tb->locate_tile(t)});
    }
  }

void from_map(struct coord co, struct tile& t) {
  snapshots.back().emplace(t.id, snaptile{t, eupoint(vid.xres/2, vid.yres/2)});
  }

void is_clone(struct tile& orig, struct tile& clone) {
  snapshots.back().emplace(clone.id, snaptile{orig, snapshots.back().at(orig.id).second});
  }

void sort_hand() {
  sort(drawn.begin(), drawn.end(), [] (tile &t1, tile &t2) {
    auto h1 = tb_hand.locate_tile(t1);
    auto h2 = tb_hand.locate_tile(t2);
    if(h1[1] != h2[1]) return h1[1] < h2[1];
    return h1[0] < h2[0];
    });
  }

void seuphorica_menu();

int start_tick;
int frametime = 500;

void seuphorica_screen() {

  if(snapshots.empty())
    start_tick = 0;

  if(start_tick && ticks - start_tick > frametime) {
    println(hlog, "frame ended");
    start_tick = 0;
    snapshots.erase(snapshots.begin());
    }

  if(snapshots.size() == 1)
    snapshots.clear();

  if(snapshots.size() && !start_tick) {
    println(hlog, "frame started");
    start_tick = ticks;
    }

  if(last_spell_effect != "") {
    addMessage(last_spell_effect);
    last_spell_effect = "";
    }

  getcstat = '-';
  cmode = sm::SIDE | sm::DIALOG_STRICT_X | sm::MAYDARK;

  gamescreen();
  dialog::init();
  dialog::display();
  dialog::add_key_action(SDLK_F1, [] { gotoHelp(fix(seuphorica::rules)); });

  if(placing_portal) mouseovers = "Click another tile to connect a portal";

  if(mouseover) {
    auto at = from(mouseover);
    auto co = get_color(at);
    if(board.count(at)) {
      mouseovers = fix(tile_desc(board.at(at)));
      if(co) mouseovers = mouseovers + ", " + describe_color(co);
      }
    else if(co) mouseovers = describe_color(co);
    if(board.count(at) || co) {
      dialog::add_key_action(SDLK_F1, [at] {
        push_tile_info_screen(board.count(at) ? board.at(at) : empty_tile, mouseover, nullptr, -1);
        });
     }
    }

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

    if(start_tick) {
      ld t = (ticks - start_tick) * 1. / frametime;
      t = t * t * (3 - 2 * t);
      auto& fst = snapshots[0];
      auto& snd = snapshots[1];
      for(auto& [id, p]: fst) if(snd.count(id) == 0) {
        snd.emplace(id, snaptile{p.first, eupoint(lerp(ui.x0, ui.x2, randd()), vid.yres * 2)});
        }
      for(auto& [id, p]: snd) if(fst.count(id) == 0) {
        fst.emplace(id, snaptile{p.first, eupoint(vid.xres * 2, lerp(ui.y0, ui.y2, randd()))});
        }
      for(auto& [id, p]: fst) {
        auto& p1 = snd.at(id);
        hyperpoint at = p.second + (p1.second - p.second) * t;
        render_tile(atscreenpos(at[0], at[1]) * euscalexx(20), t < .5 ? p.first : p1.first, nullptr, nullptr, 0);
        }
      }

    quickqueue();
    }

  int in_row = (isize(spells) + 1) / 2;
  for(int id=0; id<isize(spells); id++) {
    auto& sp = spells[id];
    if((sp.identified && enabled_id) || sp.inventory) {
      if(displayfr(lerp(ui.x0, ui.x2, (id % in_row+1.)/(1+in_row)), ui.y3 + vid.fsize/2 + (id/in_row * vid.fsize), 1, vid.fsize, sp.greek + ": " + its(sp.inventory), sp.color_value, 8)) {
        mouseovers = spell_desc(id, sp.inventory);
        getcstat = 'C';
        dialog::add_key_action('C', [=] {
          holdmouse = true; hold_mode = 4; tile_boxid = id;
          });
        dialog::add_key_action(SDLK_F1, [id] { gotoHelp(
          helptitle("spellcasting", 0xC000C0) +
            "This is a spell. You can cast spells by dragging them to one of the tiles in your hand.\n\n"
            "(Some spells affect tiles, some do not; usually spells need to be identified before you know what they actually do. You need to drag all spells, just in case.)\n\n"
            "This spell is:\n\n" + fix(spell_desc(id))
          ); });
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

  displayButton(lerp(ui.x0, ui.x2, 3/6.), vid.yres - vid.fsize, str_view_help, SDLK_F1, 8);
  if(getcstat == SDLK_F1)
    dialog::add_key_action(SDLK_F1, [] { gotoHelp(fix(seuphorica::rules)); });

  displayButton(lerp(ui.x0, ui.x2, 5/6.), vid.yres - vid.fsize, "menu", 'v', 8);
  dialog::add_key_action('v', [] { pushScreen(seuphorica_menu); });

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
    if(uni == '-' && mouseover && !current_box && !holdmouse) {
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

string logfile = "seuphorica-log.txt";

void seuphorica_dictionary() {
  cmode = sm::DARKEN;
  gamescreen();
  dialog::init("Seuphorica dictionary", 0xFFFF80);
  dialog::addBack();
  if(dialog::infix != "") mouseovers = dialog::infix;

  dialog::start_list(900, 900, '1');
  if(dictionary_checked != dialog::infix) {
    dictionary_checked = dialog::infix;
    for(char& ch: dictionary_checked) if(ch >= 'a' && ch <= 'z') ch -= 32;
    find_words(dictionary_checked);
    }
  for(auto& wo: words_found) dialog::addItem(wo, dialog::list_fake_key++);
  dialog::end_list();

  if(utf8_length(dictionary_checked) < 2) dialog::addInfo(str_least2);
  else if(words_found.empty()) dialog::addInfo(str_no_matching);
  else dialog::addInfo("(" + its(isize(words_found)) + str_matching->get() + ")");

  dialog::addBreak(100);
  dialog::addHelp(fix(str_dict_help));

  keyhandler = [] (int sym, int uni) {
    if(among(uni, '$', '?')) { dialog::infix += uni; return; }
    dialog::handleNavigation(sym, uni);
    if(dialog::editInfix(uni)) dialog::list_skip = 0;
    else if(doexiton(sym, uni)) popScreen();
    };

  dialog::display();
  }

language **lang_to_edit;

void pick_language() {
  dialog::init("pick language", 0xFFFF80);
  char key = 'a';
  for(auto l: languages) {
    dialog::addItem(l->name, key++);
    dialog::add_action([l] { *lang_to_edit = l; popScreen(); });
    }
  dialog::addBack();
  dialog::display();
  }

enum class ss { technical, disabled, random, enabled };

vector<ss> special_setting;

int pick_qty = 8;

void init_special_setting() {
  int qty = int(sp::first_artifact);
  special_setting.resize(qty);
  for(int i=0; i<qty; i++)
    special_setting[i] =
      (i < 2) ? ss::technical :
      (sp(i) == sp::naughty) ? ss::disabled :
      (i > int(sp::naughty)) ? ss::disabled :
      ss::random;
  }

bool want_spells = true, want_stay = true, want_power = true, want_id = true;
int want_seed;

void seuphorica_newgame() {
  cmode = sm::DARKEN;
  gamescreen();
  dialog::init("Seuphorica: new game", 0xFFFF80);
  dialog::addSelItem("language", next_language->name, 'l');
  lang_to_edit = &next_language; dialog::add_action_push(pick_language);
  dialog::addItem("start new standard game", 's');
  dialog::add_action([] {
    View = Id; where_is_tile.clear(); current = next_language;
    restart("", "", "");
    popScreen(); popScreen();
    });
  if(!is_daily) {
    check_daily_time();
    dialog::addSelItem("start new daily game", its(daily), 'd');
    dialog::add_action([] {
      View = Id; where_is_tile.clear(); current = next_language;
      restart((its(daily) + "9").c_str(), "D", "8");
      popScreen(); popScreen();
      });
    }
  dialog::addBreak(100);
  dialog::start_list(900, 900, 'A');
  int randoms = 0;
  for(int i=0; i<isize(special_setting); i++) {
    if(special_setting[i] == ss::technical) continue;
    if(get_language(sp(i)) == next_language) continue;
    dialog::addBoolItem(specials[i].caption, special_setting[i] != ss::disabled, dialog::list_fake_key++);
    dialog::add_action([i] {
      auto& v = special_setting[i];
      if(v == ss::disabled) v = ss::random;
      else if(v == ss::random) v = ss::enabled;
      else v = ss::disabled;
      });
    if(special_setting[i] == ss::random) dialog::lastItem().value = "?", randoms++;
    }
  dialog::end_list();
  dialog::addSelItem("random powers to pick", its(pick_qty) + "/" + its(randoms), '/');
  dialog::add_action([randoms] {
    dialog::editNumber(pick_qty, 0, randoms, 1, 8, "random powers to pick", "");
    });
  dialog::addBoolItem_action("spell spots", want_spells, 's');
  dialog::addBoolItem_action("identification", want_id, 'i');
  dialog::addBoolItem_action("power spots", want_power, 'p');
  dialog::addBoolItem_action("stay spots", want_stay, 't');
  dialog::addSelItem("seed", want_seed ? its(want_seed) : "?", 'z');
  dialog::add_action([] {
    dialog::editNumber(want_seed, 0, 999999, 1, rand() % 1000000, "seed", "set to 0 for random");
    });
  dialog::addItem("start custom game", 'c');
  dialog::add_action([] {
    if(!want_seed) gameseed = time(NULL);
    else gameseed = want_seed;
    enabled_spells = want_spells;
    enabled_id = want_id;
    enabled_power = want_power;
    enabled_stay = want_stay;
    println(hlog, "gameseed is ", gameseed);
    std::mt19937 restrict_rng(gameseed);

    auto sc = special_setting;
    int qty = isize(sc);

    for(int i=0; i<qty; i++) {
      auto lang = get_language(sp(i));
      if(lang == next_language) sc[i] = ss::disabled;
      }

    for(int i=0; i < pick_qty; i++) {
      for(int j=0; j<1000; j++) {
        int r = hrand((int) sp::first_artifact, restrict_rng);
        println(hlog, r, tie(i, j));
        if(sc[r] == ss::random) { sc[r] = ss::enabled; break; }
        }
      }

    for(int i=0; i < qty; i++)
      special_allowed[i] = sc[i] == ss::enabled;

    polyglot_languages.clear();
    for(int i=0; i<qty; i++) if(special_allowed[i]) {
      auto lang = get_language(sp(i));
      if(!lang) continue;
      polyglot_languages.insert(lang);
      }

    View = Id; where_is_tile.clear(); current = next_language;
    is_daily = false; game_restricted = false;
    for(int i=0; i<qty; i++) if(!special_allowed[i]) game_restricted = true;
    new_game();
    popScreen(); popScreen();
    });

  dialog::addBack();
  dialog::display();
  }

void seuphorica_menu() {
  cmode = sm::DARKEN;
  dictionary_checked = "";

  gamescreen();
  dialog::init("Seuphorica", 0xFFFF80);
  dialog::addItem("dictionary", 'd');
  dialog::add_action_push(seuphorica_dictionary);
  dialog::addItem("new game", 'n');
  dialog::add_action_push(seuphorica_newgame);
  dialog::addItem("save log", 'l');
  dialog::add_action([] {
    dialog::openFileDialog(logfile, XLAT("where to save the log:"), ".txt",
      [] {
        fhstream f(logfile, "wt");
        if(!f.f) { addMessage("failed to open file"); return false; }
        print(f, fix(game_log.str()));
        return true;
        });
    });
  dialog::addItem("quit", 'q');
  dialog::add_action([] { quitmainloop = true; });
  dialog::addItem("cheat", 'c');
  dialog::add_action(cheat);
  dialog::addBack();
  dialog::display();
  }

void launch() {  
  stop_game();
  enable_canvas();
  ccolor::set_plain_nowall(0x202020);
  start_game();
  View = Id;
  init_special_setting();
  restart("", "", "");
  menu_darkening = 3; /* needs more darkening than HyperRogue due to higher contrast */

  showstartmenu = false;
  mapeditor::drawplayer = false;
  color_descs = false;

  rogueviz::rv_hook(hooks_build_help, 100, [] { help = fix(seuphorica::rules); return true; });

  clearMessages();
  addMessage("Welcome to Seuphorica!");
  }

void enable() {
  rogueviz::rv_hook(hooks_drawcell, 100, draw);
  }

auto seuphorica_hook = arg::add3("-seuphorica", [] { launch(); enable(); pushScreen(seuphorica_screen); });

}
}
