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

using coord = cell*;
using vect2 = cellwalker;

bool bidirectional;

map<cell*, int> distance_from_board;

extern map<cell*, struct tile> board;
extern set<cell*> just_placed;
extern cell *dist_for;

bool in_board(coord co) {
  if(co->land == laMemory) return false;
  if(disksize || closed_manifold) return true;
  auto& dfb = distance_from_board[co];
  int maximum = 0, qty = 0;
  if(board.count(co) && !just_placed.count(co)) { dfb = 7; return true; }
  forCellCM(c1, co) {
    auto dfb1 =	distance_from_board[c1];
    if(dfb1 > maximum) maximum = dfb1, qty = 1;
    else if(dfb1 == maximum) qty++;
    }
  if(maximum && qty < co->type / 2) maximum--;
  if(dfb != maximum) currently_scrolling = true, dist_for = nullptr;
  dfb = maximum;
  return dfb > 0;
  }

bool euclid_only() {
  return geometry == gEuclidSquare && variation == eVariation::pure && !quotient; /* to do: accept standard tori */
  }

vector<cell*> gigacover(cell *c) {
  if(euclid_only()) {
    /* cannot do the default case because of mirror(...) */
    vector<cell*> res;
    res.push_back(c->cmove(0)->cmove(1));
    res.push_back(res.back()->cmove(2));
    res.push_back(res.back()->cmove(2));
    for(int i=0; i<6; i++)
      res.push_back(res[i]->cmove(3));
    return res;
    }
  auto ac = adj_minefield_cells(c);
  ac.push_back(c);
  return ac;
  }

vector<cell*> orthoneighbors(cell* base) {
  vector<coord> res;
  forCellCM(c1, base) res.push_back(c1);
  return res;
  }

map<cell*, cellwalker> tile_orientation;
void set_orientation(cell *c, cellwalker cw) {
  tile_orientation[c] = cw;
  }

vector<vect2> forward_steps(coord c) {
  if(euclid_only() && !bidirectional)
    return {cellwalker(c, 2), cellwalker(c, 3)};
  else {
    vector<vect2> vs;
    for(int i=0; i<c->type; i++) vs.push_back(cellwalker(c, i));
    return vs;
    }
  }

struct xy { int x; int y; };

xy to_xy(cellwalker c) {
  auto co = euc2_coordinates(c.at);
  auto co1 = euc2_coordinates(c.peek());
  auto co2 = co1 - co;
  return xy{co2.first, co2.second};
  }

cell *dist_for = nullptr;
map<cell*, int> distance_to;

int dist(coord a, coord b) {
  if(euclid_only()) {
    auto co = euc2_coordinates(a);
    auto co1 = euc2_coordinates(b);
    auto co2 = co1 - co;
    return max(abs(co2.first), abs(co2.second));
    }
  if(a->type != b->type) return 999;

  if(a != dist_for) {
    distance_to.clear();
    dist_for = a;
    vector<pair<cell*, int>> v;
    v.emplace_back(a, 0);
    for(int i=0; i<isize(v); i++) {
      auto [c, d] = v[i];
      if(distance_to.count(c)) continue;
      distance_to[c] = d;
      auto ac = adj_minefield_cells(c);
      for(cell *c1: ac) {
        if(c1->land == laMemory) continue;
        if(!disksize && !closed_manifold && !distance_from_board[c1]) continue;
        v.emplace_back(c1, d+1);
        }
      }
    }

  return distance_to[b];
  }

void advance(cell*& c, cellwalker& cw) {
  cw += wstep; cw += rev; c = cw.at;
  }

cell *get_advance(cell* c, cellwalker cw) {
  advance(c, cw); return c;
  }

cell *origin() { return currentmap->gamestart(); }
cell *nocoord() { return nullptr; }

map<cell*, int> list_order;

string spotname(cell *c) {
  if(euc::in()) {
    auto co = euc2_coordinates(c);
    return lalign(0, "(", -co.first, ",", -co.second, ")");
    }
  else if(disksize || closed_manifold) {
    return "#" + its(list_order[c]);
    }
  else if(currentmap->strict_tree_rules()) {
    return lalign(0, rulegen::canonical_path_to(c->master));
    }
  else {
    hyperpoint h = calc_relative_matrix(c, currentmap->gamestart(), C0) * C0;
    return lalign(0, kz(h));
    }
  }

cellwalker getback(cellwalker& cw) {
  return cw + rev;
  }

void thru_portal(coord& x, vect2& v);
void mirror(coord& at, vect2& prev);

void snapshot();
void from_map(coord co, struct tile& t);
void is_clone(struct tile& orig, struct tile& clone);

bool gok_hv() { return euclid_only(); }
bool gok_gigacombo() { return euclid_only(); }

bool gok_rev() { return euclid_only() || bidirectional; }

}

#define NONJS
#define ALTGEOM
#include "seuphorica/seuphorica.cpp"

namespace seuphorica {

void thru_portal(coord& x, vect2& v) {
  if(get_gigantic(x) != x) {
    auto x1 = get_gigantic(x);
    auto gc = gigacover(x1);
    for(int i=0; i<seuphorica::isize(gc); i++) if(gc[i] == x) {
      x = gigacover(portals.at(x1))[i];
      v.at = x;
      return;
      }
    }
  auto x1 = portals.at(x);
  v.spin -= tile_orientation[x].spin;
  v.spin += tile_orientation[x1].spin;
  v.at = x1;
  x = x1;
  }

void mirror(coord& at, vect2& prev) {
  if(get_gigantic(at) != at) {
    auto at1 = get_gigantic(at);
    auto v = gigacover(at1);
    vector<int> reindex = {0,3,6,1,4,7,2,5,8};
    for(int i=0; i<9; i++) if(v[i] == at) { at = v[reindex[i]]; prev.at = at; break; }
    }
  prev.spin = tile_orientation[at].spin + 1 - prev.spin;
  prev += 0;
  }

void compute_score();

void compute_size() {}

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
     int co = get_color(c);
     gotoHelp(helptitle("map spot", 0xC000C0) + "Map spots have special powers. Try placing a tile here and see the description for the effects.\n\nThis is: " + describe_color(co));
     return;
     }
  string s;
  s = helptitle(short_desc(t), gsp(t).background);
  s = s + fix(tile_desc(t));
  if(c) {
    int co = get_color(c);
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
  c = get_gigantic(c);
  if(c && just_placed.count(c)) {
    for(int i=1; i<c->type; i++)
      help_extensions.push_back(help_extension{char('0'+i), "rotate " + its(i), [c,i] () { tile_orientation[c]+=i; popScreen(); }});
    }
  }

/** for tiles on the map, only (V,t,c) are defined; for tiles in boxes, (V,t,origbox,boxid) are defined */
void render_tile(shiftmatrix V, tile& t, cell *c, vector<tile>* origbox, int boxid) {

  cellwalker cw;
  if(c) cw = tile_orientation[c];

  bool gig = has_power(t, sp::gigantic);

  auto pt0 = [&] (int id, ld r) {
    if(gig) r /= 3;
    if(c) return currentmap->get_corner(c, cw.spin+id+1+c->type/2, r);
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
    ensure_floorshape_generated(shvid(c), c);
    for(int i=0; i<c->type; i++)
      if(!board.count(c->move(i))) placeSidewall(c, i, SIDE_SLEV, V, back);
    V1 = orthogonal_move_fol(V, cgi.SLEV[1]);
    if(!gig || !euclid_only()) draw_qfi(c, V1, back, PPR::WALL3A);
    }
  else {
    wider w(wide);
    for(int i=0; i<=corners; i++) curvepoint(pt(i, 3));
    queuecurve(V, lines, back, PPR::ITEM);
    }

  if(c && gig) {
    if(gigants.at(c) != c) return;
    if(euclid_only())
      draw_qfi(c, V1 * euscalexx(3), back, PPR::WALL3A);
    }

  const ld nearco = 4;

  auto semitile = [&] (ld a, ld b, color_t col) {
    a *= corners / 4.; b *= corners / 4.;
    curvepoint(pt(a, 3));
    a = floor(a) + 1;
    while(a < b) curvepoint(pt(a++, 3));
    curvepoint(pt(b, 3));
    queuecurve(V1, 0, darkena(col, 0, 0xFF), PPR::WALL3A);
    };

  if(has_power(t, sp::polski)) {
    semitile(1.5, 3.5, 0xDD143C);
    semitile(3.5, 5.5, 0xFFFFFF);
    }

  if(has_power(t, sp::deutsch)) {
    semitile(4-1/3., 4+4/3., 0x000000);
    semitile(5/3., 10/3., 0xFFCC00);
    }

  if(has_power(t, sp::francais)) {
    semitile(2/3., 7/3., 0xE1000F);
    semitile(8/3., 13/3., 0x000091);
    }

  if(has_power(t, sp::espanol)) {
    semitile(4-1/4., 4+5/4., 0xAA151B);
    semitile(7/4., 13/4., 0xAA151B);
    }

  if(has_power(t, sp::english)) {
    vid.linewidth *= 3;
    for(int i=0; i<corners; i++) queueline(V1 * pt(i/2., 3), V1 * pt((i+corners)/2., 3), darkena(0xFFFFFF, 0, 0xFF), 2, PPR::WALL3A);
    vid.linewidth /= 3;
    // for simplicity, and less confusion with the letter, we do not add the red cross on top
    }

  if(has_power(t, sp::portugues_br)) {
    for(int i=0; i<=corners; i++) curvepoint(pt(i+.5, 3));
    queuecurve(V1, 0, darkena(0xFEDD00, 0, 0xFF), PPR::WALL3A);
    }

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

  auto V2 = V1;
  if(c) V2 = V2 * ddspin(c,cw.spin,0);

  write_in_space(V2, 72, gigscale, t.letter, darkena(darkened(gsp(t).text_color), 0, 0xFF), 0, 8);
  write_in_space(V2 * xpush(cgi.scalefactor*.2*gigscale) * ypush(cgi.scalefactor*.2*gigscale), 72, 0.4 * gigscale, its(t.value), darkena(darkened(gsp(t).text_color), 0, 0xFF), 0, 8);

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
  if(c->land == laMemory) return false;
  bool inside = in_board(c);
  if(inside) {
    c->wall = waNone; c->landparam = 0x202020;
    if(placing_portal) {
      int val; has_power(board.at(portal_from), sp::portal, val);
      if(dist(portal_from, c) <= val) c->landparam = 0x0000C0;
      }
    }
  else
    c->wall = waChasm;
  if(board.count(c)) {
    auto& t = board.at(c);
    render_tile(V, t, c, nullptr, -1);
    }
  if(!inside) return false;

  if(portals.count(c)) {
    auto c1 = portals.at(c);
    for(const shiftmatrix& V1: hr::span_at(current_display->all_drawn_copies, c1)) {
      queueline(V * currentmap->get_corner(c, 2, 4), V1 * currentmap->get_corner(c1, 2, 4), 0xFF800080, 5);
      queueline(V * currentmap->get_corner(c, 2+c->type/2, 4), V1 * currentmap->get_corner(c1, 2+c->type/2, 4), 0x0000FF80, 5);

      if(tiles3) {
        auto high_V = orthogonal_move_fol(V, cgi.SLEV[1]);
        auto high_V1 = orthogonal_move_fol(V1, cgi.SLEV[1]);
        queueline(high_V * currentmap->get_corner(c, 2, 4), high_V1 * currentmap->get_corner(c1, 2, 4), 0xFF800080, 5);
        queueline(high_V * currentmap->get_corner(c, 2+c->type/2, 4), high_V1 * currentmap->get_corner(c1, 2+c->type/2, 4), 0x0000FF80, 5);
        }
      }
    }

  int co = get_color(c);
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

void animate_sort(vector<tile>& which) {
  snapshot();
  for(auto& t: which) where_is_tile.erase(t.id);
  snapshot();
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
      if(max(abs(d[0]), abs(d[1])) < get_space() + (has_power(t1, sp::gigantic) ? 30:0) + (has_power(t, sp::gigantic) ? 30:0)) return false;
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
      write_in_space(ASP * eupush(*x2 - 10, *y1 + 20), 72, 50/3, title, darkena(col, 0, 0xFF), 16, 16);
      if(isize(*ptset))
        write_in_space(ASP * eupush(*x2 - 10, *y1 + 40), 72, 25/3, its(isize(*ptset)), darkena(col, 0, 0xFF), 16, 16);
      if(ptset == &shop)
        write_in_space(ASP * eupush(*x2 - 10, *y2 - 10), 72, 50/3, its(cash) + "$", darkena(col, 0, 0xFF), 16, 16);
      }

    if(hr::isize(snapshots) || (ptset == &deck && scry_active)) return;

    int idx = 0;
    for(auto& t: *ptset) {
      auto lt = locate_tile(t);
      if(&t == tile_moved && holdmouse) { idx++; continue; }

      if(ptset == &drawn && idx < ev.retain_count) {
        auto T = ASP * eupush(lt) * euscalexx(20) * spin(45._deg);
        for(int i=0; i<=4; i++) curvepoint(spin(90._deg*i) * eupoint(1,1));
        queuecurve(T, darkena(col, 0, 0xFF), darkena(col, 0, 0x80), PPR::ZERO);
        }

      render_tile(ASP * eupush(lt) * euscalexx(20), t, nullptr, ptset, idx++);
      }

    int cx = 0;
    auto sortbutton = [&] (string title, string desc, auto action) {
      if(displayfr(*x1 + 12 + vid.fsize * (++cx)/2 - vid.fsize/4, *y2 - 12 - vid.fsize/2 + vid.fsize/4, 1, vid.fsize/2, title, col, 8)) {
        getcstat = 'X'; mouseovers = desc;
        dialog::add_key_action('X', [this, action] {
          action();
          animate_sort(*ptset);
          });
        }
      };
    auto rar = [] (const tile& t) { return t.rarity + (t.special >= sp::first_artifact ? 100 : 0); };
    auto spc = [] (const tile& t) {
      int i = int(t.special);
      if(t.special == sp::gigantic) i -= 100;
      if(among(t.special, sp::initial, sp::final, sp::symmetric)) i += 100;
      if(among(t.special, sp::trasher, sp::multitrasher, sp::duplicator)) i += 300;
      if(among(t.special, sp::reversing)) i += 400;
      if(among(t.special, sp::bending, sp::portal)) i += 500;
      if(get_language(t)) i += 600;
      return i;
      };
    sortbutton("A", "sort alphabetically", [this] { stable_sort(ptset->begin(), ptset->end(), [] (const tile& a, const tile& b) { return a.letter < b.letter; }); });
    sortbutton("1", "sort by value", [this] { stable_sort(ptset->begin(), ptset->end(), [] (const tile& a, const tile& b) { return a.value > b.value; }); });
    sortbutton("X", "sort by power", [&] { stable_sort(ptset->begin(), ptset->end(), [&] (const tile& a, const tile& b) { return spc(a) < spc(b); }); });
    sortbutton("R", "sort by rarity", [&] { stable_sort(ptset->begin(), ptset->end(), [&] (const tile& a, const tile& b) { return rar(a) > rar(b); }); });
    sortbutton("←", "reverse order", [this] { reverse(ptset->begin(), ptset->end()); });
    sortbutton("?", "shuffle", [this] { for(int i=1; i<isize(*ptset); i++) swap((*ptset)[i], (*ptset)[rand() % (i+1)]); });
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

void from_map(coord co, struct tile& t) {
  shiftpoint h = ggmatrix(co) * tile_center();
  hyperpoint h1;
  applymodel(h, h1);
  h1 = toscrcoord(h1);
  snapshots.back().emplace(t.id, snaptile{t, eupoint(h1[0], h1[1])});
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

void centermap() {
  centerover = currentmap->gamestart();
  View = iddspin(centerover,tile_orientation[centerover].spin,0);
  }

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

  stillscreen = !anims::any_on() && !holdmouse && !hr::isize(snapshots);

  if(placing_portal) mouseovers = "Click another tile to connect a portal";

  if(mouseover) {
    auto at = mouseover;
    auto co = get_color(at);
    if(board.count(at)) {
      mouseovers = fix(tile_desc(board.at(at)));
      if(co) mouseovers = mouseovers + ", " + describe_color(co);
      }
    else if(co) mouseovers = describe_color(co);
    else mouseovers = spotname(at);
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
    dynamicval<ld> gs(cgi.scalefactor, 3);

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
    displayButton(lerp(ui.x0, ui.x2, 1/8.), vid.yres - vid.fsize, just_placed.empty() ? str_skip_turn : str_play, SDLK_RETURN, 8);
    dialog::add_key_action(SDLK_RETURN, play);
    }

  displayButton(lerp(ui.x0, ui.x2, 3/8.), vid.yres - vid.fsize, "center", ' ', 8);
  dialog::add_key_action(' ', centermap);

  displayButton(lerp(ui.x0, ui.x2, 5/8.), vid.yres - vid.fsize, str_view_help, SDLK_F1, 8);
  if(getcstat == SDLK_F1)
    dialog::add_key_action(SDLK_F1, [] { gotoHelp(fix(seuphorica::rules)); });

  displayButton(lerp(ui.x0, ui.x2, 7/8.), vid.yres - vid.fsize, "menu", 'v', 8);
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
      if(box_moved == &drawn && current_box == nullptr && mouseover) {
        auto at = mouseover;
        if(!board.count(at)) {
          swap(drawn[tile_boxid], drawn[0]);
          drop_hand_on(at);
          sort_hand();
          }
        }
      if(box_moved == &shop && current_box == nullptr && mouseover && tile_moved->price <= cash) {
        auto at = mouseover;
        if(!board.count(at)) {
          buy(tile_boxid);
          drop_hand_on(at);
          }
        }
      }
    if(uni == '-' && mouseover && !current_box && !holdmouse && mouseover) {
      auto at = mouseover;
      if(board.count(at)) {
        back_from_board(at); hold_mode = 1; tile_moved_from = mouseover;
        holdmouse = true; tile_moved = &(drawn[0]); tile_boxid = 0; box_moved = &drawn;
        }
      else
        drop_hand_on(at);
      }
    };
  }

string logfile = "seuphorica-log.txt";

void seuphorica_dictionary() {
  cmode = sm::DARKEN;
  gamescreen();
  stillscreen = !anims::any_on();
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

void reset_rv() {
  View = Id; where_is_tile.clear(); current = next_language;
  tile_orientation.clear();
  list_order.clear();
  distance_from_board.clear();
  dist_for = nullptr; distance_to.clear();
  if(disksize || closed_manifold) {
    auto v = currentmap->allcells();
    for(int i=0; i<hr::isize(v); i++) list_order[v[i]] = i;
    }
  }

struct seuphgeom {
  string name;
  reaction_t launcher;
  };

int current_seuphgeom = -1;

vector<seuphgeom> seuphgeoms = {
  {"Infinite Board", []{
    set_geometry(gEuclidSquare);
    set_variation(eVariation::pure);
    pconf.scale = 0.2;
    vid.use_smart_range = 2;
    vid.creature_scale = 1;
    auto& T0 = euc::eu_input.user_axes; T0[0][0] = T0[0][1] = T0[1][0] = T0[1][1] = euc::eu_input.twisted = 0;
    euc::build_torus3();
    req_disksize = 0;
    }},

  {"Claustrophobia", []{
    set_geometry(gEuclidSquare);
    set_variation(eVariation::pure);
    pconf.scale = 0.2;
    vid.use_smart_range = 2;
    vid.creature_scale = 1;
    auto& T0 = euc::eu_input.user_axes; T0[0][0] = T0[0][1] = T0[1][0] = T0[1][1] = euc::eu_input.twisted = 0;
    euc::build_torus3();
    req_disksize = 15 * 15;
    diskshape = dshVertices;
    }},

  {"Hyperbolic Board", []{
    set_geometry(g45);
    gp::param = {3, 1};
    set_variation(eVariation::goldberg);
    gp::dual_of_current();
    vid.use_smart_range = 2;
    pconf.scale = 0.9;
    vid.creature_scale = 1.5;
    req_disksize = 0;
    arb::convert::convert();
    rulegen::prepare_rules();
    arb::convert::activate();
    }},

  {"Spherical Board", []{
    set_geometry(gOctahedron);
    gp::param = {6, 1};
    set_variation(eVariation::goldberg);
    gp::dual_of_current();
    pconf.scale = 0.9;
    req_disksize = 0;
    }},

  {"Kite and Dart", []{
    set_geometry(gKiteDart2);
    pconf.scale = 0.5;
    vid.use_smart_range = 2;
    req_disksize = 0;
    }},

  {"Hyperbolic Board II", []{
    set_geometry(g46);
    gp::param = {5, 0};
    set_variation(eVariation::goldberg);
    vid.use_smart_range = 2;
    pconf.scale = 0.9;
    vid.creature_scale = 0.8;
    req_disksize = 0;
    arb::convert::convert();
    rulegen::prepare_rules();
    arb::convert::activate();
    }},

  {"Dodecagons", []{
    set_variation(eVariation::pure);
    arcm::load_symbol("4,6,12", true);
    vid.use_smart_range = 2;
    pconf.scale = 0.25;
    vid.creature_scale = 2;
    req_disksize = 0;
    }},

  {"Torus", []{
    set_geometry(gEuclidSquare);
    set_variation(eVariation::pure);
    pconf.scale = 0.2;
    vid.use_smart_range = 2;
    vid.creature_scale = 1;
    auto& T0 = euc::eu_input.user_axes; T0[0][0] = 20; T0[1][1] = 20; T0[0][1] = 11; T0[1][0] = -11; euc::eu_input.twisted = 0;
    euc::build_torus3();
    req_disksize = 0;
    }},

  {"Bring Surface", []{
    set_geometry(gBring);
    gp::param = {3, 1};
    set_variation(eVariation::goldberg);
    gp::dual_of_current();
    vid.use_smart_range = 2;
    pconf.scale = 0.9;
    vid.creature_scale = 1.5;
    req_disksize = 0;
    }},
  };

void reset_seuphorica_screen() {
  popScreenAll();
  pushScreen(seuphorica_screen);
  centermap();
  clearMessages();
  addMessage("Welcome to Seuphorica!");
  }

void enable();

void seuphorica_setgeom() {
  cmode = sm::DARKEN;
  gamescreen();
  stillscreen = !anims::any_on();
  dialog::init("Seuphorica geometries", 0xFFFF80);
  char let = 'a';
  for(int i=0; i<isize(seuphgeoms); i++) {
    dialog::addBoolItem(seuphgeoms[i].name, i == current_seuphgeom, let++);
    dialog::add_action([i] {
      current_seuphgeom = i;
      stop_game();
      seuphgeoms[i].launcher();
      start_game();
      reset_rv();
      new_game();
      enable();
      reset_seuphorica_screen();
      });
    }
  dialog::addBreak(100);
  dialog::addBoolItem("words both ways", bidirectional, 'w');
  dialog::add_action([] {
    bidirectional = !bidirectional;
    reset_rv(); new_game(); reset_seuphorica_screen();
    });
  dialog::addBack();
  dialog::display();
  }

void seuphorica_newgame() {
  cmode = sm::DARKEN;
  gamescreen();
  stillscreen = !anims::any_on();
  dialog::init("Seuphorica: new game", 0xFFFF80);
  dialog::addSelItem("language", next_language->name, 'l');
  lang_to_edit = &next_language; dialog::add_action_push(pick_language);

  dialog::addSelItem("geometry", current_seuphgeom == -1 ? "custom" : seuphgeoms[current_seuphgeom].name, 'g');
  dialog::add_action_push(seuphorica_setgeom);

  dialog::addItem("start new standard game", 's');
  dialog::add_action([] {
    reset_rv();
    restart("", "", "");
    reset_seuphorica_screen();
    });
  if(!is_daily) {
    check_daily_time();
    dialog::addSelItem("start new daily game", its(daily), 'd');
    dialog::add_action([] {
      reset_rv();
      restart((its(daily) + "9").c_str(), "D", "8");
      reset_seuphorica_screen();
      });
    }
  dialog::addBreak(100);
  dialog::start_list(900, 900, 'A');
  int randoms = 0;
  for(int i=0; i<isize(special_setting); i++) {
    if(special_setting[i] == ss::technical) continue;
    if(!geom_allows(sp(i))) continue;
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
      if(!geom_allows(sp(i))) sc[i] = ss::disabled;
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

    reset_rv();
    is_daily = false; game_restricted = false;
    for(int i=0; i<qty; i++) if(!special_allowed[i]) game_restricted = true;
    new_game();
    reset_seuphorica_screen();
    });

  dialog::addBack();
  dialog::display();
  }

void seuphorica_menu() {
  cmode = sm::DARKEN;
  dictionary_checked = "";
  stillscreen = !anims::any_on();

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

void enable() {
  rogueviz::rv_hook(hooks_build_help, 100, [] { help = fix(seuphorica::rules); return true; });
  rogueviz::rv_hook(hooks_drawcell, 100, draw);
  rogueviz::rv_change(showstartmenu, false);
  rogueviz::rv_change(mapeditor::drawplayer, false);
  }

local_parameter_set lps_seuphorica("seuphorica:");

void default_config() {
  lps_add(lps_seuphorica, menu_darkening, 3);
  lps_add(lps_seuphorica, mine_adjacency_rule, true);
  }

void launch() {  
  // change the settings from JS Seuphorica
  specials[int(sp::english)].text_color = 0xFFC8102E;
  color_descs = false;
  init_special_setting();

  stop_game();
  enable_canvas();
  ccolor::set_plain_nowall(0x202020);
  lps_enable(&lps_seuphorica);
  start_game();

  reset_rv();
  restart("", "", "");
  enable();
  reset_seuphorica_screen();
  }

auto seuphorica_hook =
  arg::add3("-seuphorica", [] { launch(); pushScreen(seuphorica_screen); })
+ addHook(hooks_configfile, 300, default_config)
+ arg::add3("-seuphorica-geo", [] {
    arg::shift();
    int which = -1;
    string s = arg::args();
    for(int i=0; i<isize(seuphgeoms); i++) if(appears(seuphgeoms[i].name, s)) which = i;
    if(which == -1 && s[0] >= '0' && s[0] <= '9') which = atoi(s.c_str());
    if(which == -1) throw hr_exception("unknown seuphorica-geo geometry");
    current_seuphgeom = which;
    seuphgeoms[which].launcher();
    launch();
    });

}
}
