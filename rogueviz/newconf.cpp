// newconformist explorable explanation
// example commandline: -noplayer -rugtsize 4096 -smart 1 -canvas B -ncee
// set CAP_NCONF (and change the path) if you have access to newconformist

#include "rogueviz.h"

#if CAP_NCONF

#ifndef CAP_DRAW
#define CAP_DRAW 0
#endif

#define main nconf_main
#undef self
#include "nconf.cpp"
#undef main
#endif

namespace hr {

namespace nconf2 {

string rfname;

enum class ptype : char { outside, inside, inside_left_up, inside_left_down, top, bottom, left_inf, right_inf, marked };

dqi_line& queueline1(const hyperpoint& H1, const hyperpoint& H2, color_t col, int prf = 0, PPR prio = PPR::LINE) {
  return queueline(shiftless(H1), shiftless(H2), col, prf, prio);
  }

void add_border(vector<string>& v, int cy) {
  int Y = isize(v), X = isize(v[0]);
  char nx = '6';
  for(int y=0; y<Y; y++)
  for(int x=0; x<X; x++)
    if(v[y][x] != '1')
    if((y && v[y-1][x] == '1') || (y<Y-1 && v[y+1][x] == '1') || (x && v[y][x-1] == '1') || (x<X-1 && v[y][x+1] == '1'))
      v[y][x] = y < cy ? '4' : y > cy ? '5' : (nx++);  
  }

vector<string> gensquare(int X, int Y) {
  vector<string> res(Y+4, string (X+4, '0'));
  for(int y=0; y<Y; y++)
  for(int x=0; x<X; x++)
    res[y+2][x+2] = '1';
  add_border(res, 2+Y/2);
  return res;
  }

vector<string> genellipse(int D, ld alpha) {
  vector<string> res(D+4, string (D+4, '0'));
  ld R = (D-1.) / 2;
  for(int y=0; y<D; y++)
  for(int x=0; x<D; x++) {
    ld ax = (x-R);
    ld ay = (y-R);
    ld bx = ax * cos(alpha) - ay * sin(alpha);
    ld by = ay * cos(alpha) + ax * sin(alpha);
    bx /= R;
    by /= R;
    res[y+2][x+2] = bx*bx+by*by*2 < 1 ? '1' : '0';
    }
  add_border(res, 2+R);
  return res;
  }

vector<string> gent(int X, int Y) {
  vector<string> res(Y+X+4, string (Y+X+X+4, '0'));
  for(int y=0; y<Y; y++)
  for(int x=0; x<X+X+Y; x++)
    res[y+2][x+2] = '1';
  for(int y=0; y<Y; y++)
  for(int x=0; x<X; x++)
    res[x+2+Y][y+2+X] = '1';
  add_border(res, 2+Y/2);
  return res;
  }

vector<string> fmap = gensquare(13, 13);

vector<string> snake = {
  "00000000000000000000000000000000000",
  "00444444444444444444444444444444400",
  "04111111111111111111111111111111140",
  "04111111111111111111111111111111140",
  "06111111111111111111111111111111140",
  "05111111111111111111111111111111140",
  "05111111111111111111111111111111140",
  "00555555555555555555555555511111140",
  "00000000000000000000000000051111140",
  "00555555555555555555555555511111140",
  "05111111111111111111111111111111140",
  "05111111111111111111111111111111140",
  "05111111111111111111111111111111140",
  "05111111111111111111111111111111140",
  "05111111111111111111111111111111140",
  "05111111444444444444444444444444400",
  "05111114000000000000000000000000000",
  "05111111444444444444444444444444400",
  "05111111111111111111111111111111140",
  "05111111111111111111111111111111140",
  "05111111111111111111111111111111170",
  "05111111111111111111111111111111150",
  "05111111111111111111111111111111150",
  "00555555555555555555555555555555500",
  "00000000000000000000000000000000000"
  };

vector<string> apple = {
  "0000000000000000000000000000000",
  "0000000000000000440000000000000",
  "0000000000000004114000000000000",
  "0000000000000004140000000000000",
  "0000000000000041140000000000000",
  "0000000000000041140000000000000",
  "0000004444444441144444440000000",
  "0000041111111111111111114400000",
  "0000411111111111111111111140000",
  "0004111111111111111111111114000",
  "0041111111111111111111111111400",
  "0041111111111111111111111111140",
  "0041111111111111111111111111140",
  "0411111111111111111111111111140",
  "0411111111111111111111111111140",
  "0411111111111111111111111111140",
  "0411111111111111111111111111140",
  "0411111111111111111111111111140",
  "0711111111111111111111111111160",
  "0511111111111111111111111111150",
  "0511111111111111111111111111150",
  "0051111111111111111111111111150",
  "0051111111111111111111111111500",
  "0051111111111111111111111111500",
  "0005111111111111111111111111500",
  "0005111111111111111111111115000",
  "0005111111111111111111111115000",
  "0000511111111111111111111150000",
  "0000051111111111111111111500000",
  "0000051111111111111111115000000",
  "0000005111111111111111150000000",
  "0000000551111511111115500000000",
  "0000000005555055555550000000000",
  "0000000000000000000000000000000"
  };

struct coord { 
  int x, y; 
  coord operator + (int d) {
    coord res = *this;
    d &= 3;
    if(d == 0) res.x++;
    if(d == 1) res.y++;
    if(d == 2) res.x--;
    if(d == 3) res.y--;
    return res;
    }
  };

char out = '-';

char& fmap_at(coord c) { return c.x >= 0 && c.x < isize(fmap[0]) && c.y >= 0 && c.y < isize(fmap) ? fmap[c.y][c.x] : out; }

ld vx[256][256], vy[256][256];

void reset_vxy() { for(int y=0; y<256; y++) for(int x=0; x<256; x++) vy[y][x] = vx[y][x] = 0; }

ld cscale;

bool pretty = true;

void iterate() {
  int Y = isize(fmap);
  int X = isize(fmap[0]);
  for(int y=0; y<Y; y++) 
  for(int x=0; x<X; x++) {
    if(fmap[y][x] == '6')
      vy[y][x] = 0, vx[y][x] = -1;
    else if(fmap[y][x] == '7')
      vy[y][x] = 0, vx[y][x] = +1;
    else if(fmap[y][x] == '4')
      vy[y][x] = -1;
    else if(fmap[y][x] == '5')
      vy[y][x] = +1;
    else if(fmap[y][x] == '1')
      vy[y][x] = (vy[y-1][x] + vy[y+1][x] + vy[y][x-1] + vy[y][x+1]) / 4;
    if(among(fmap[y][x], '1', '4', '5')) {
      int qty = 0;
      ld total = 0;
      auto in = [&] (char c) { return pretty ? c > '0' : among(c, '1', '6', '7'); };
      if(y > 0 && in(fmap[y-1][x])) qty++, total += vx[y-1][x];
      if(y < Y-1 && in(fmap[y+1][x])) qty++, total += vx[y+1][x];
      if(x > 0 && in(fmap[y][x-1])) qty++, total += vx[y][x-1];
      if(x < X-1 && in(fmap[y][x+1])) qty++, total += vx[y][x+1];
      vx[y][x] = total / qty;
      }
    }

  vector<ld> xes;

  for(int y=0; y<Y-1; y++) for(int x=0; x<X-1; x++) 
    if(fmap[y][x] == '1' && fmap[y+1][x] == '1' && fmap[y][x+1] == '1') {
    
    hyperpoint here = point2(vx[y][x], vy[y][x]);
    hyperpoint v0   = point2(vx[y][x+1], vy[y][x+1]) - here;
    hyperpoint v1   = point2(vx[y+1][x], vy[y+1][x]) - here;
    
    ld det = (v0 ^ v1)[2];
    if(det == 0) continue;

    hyperpoint ba2 = point2(v1[1], -v0[1]) / det;
    hyperpoint ca2 = point2(-v1[0], v0[0]) / det;
    
    ld d = (ba2|ba2);
    if(d == 0) continue;
  
    ld good = (ca2^ba2)[2] / d;

    xes.push_back(good);
    }
  
  sort(xes.begin(), xes.end());
  if(isize(xes))
    cscale = -xes[isize(xes) / 2];
  
  // println(hlog, "cscale = ", cscale);
  }

#if CAP_NCONF
void nconf_solve() {
  nconf::SY = isize(fmap);
  nconf::SX = isize(fmap[0]);
  nconf::resize_pt();
  for(int y=0; y<nconf::SY; y++) 
  for(int x=0; x<nconf::SX; x++) {
    auto& p = nconf::pts[y][x];
    p.type = (nconf::ptype)(fmap[y][x] - '0');
    p.side = 0;
    }
  nconf::pretty_borders = pretty;
  nconf::draw_progress = false;
  nconf::text_progress = false;
  nconf::computemap(nconf::pts);
  for(int y=0; y<nconf::SY; y++) 
  for(int x=0; x<nconf::SX; x++) { 
    if(fmap[y][x] == '1')
    vx[y][x] = nconf::pts[y][x].x[0] * 2 - 1,
    vy[y][x] = nconf::pts[y][x].x[1] * 2 - 1;
    if(pretty && (fmap[y][x] == '4' || fmap[y][x] == '5'))
      vx[y][x] = nconf::pts[y][x].x[0] * 2 - 1;
    }

  printf("nconf solved\n");
  }
#endif

basic_textureinfo nctinf, nctinf2;
vector<glvertex> vs;

bool viewmap = true;

void fix_border() {
  int Y = isize(fmap);
  int X = isize(fmap[0]);
  for(string& s: fmap) for(char& c: s) if(c == '5') c = '4';

  coord cc;
  for(int y=0; y<Y; y++) for(int x=0; x<X; x++) if(fmap[y][x] == '6') cc = coord{x,y};
  
  int dir = 0;
  while(fmap_at(cc+dir) != '1') dir++;
  
  cc = cc + dir;
  dir += 2;
  
  while(true) {
    if(fmap_at(cc+dir) == '1') cc = cc + dir, dir++;
    else {
      if(fmap_at(cc+dir) == '4') fmap_at(cc+dir) = '5';
      if(fmap_at(cc+dir) == '7') break;
      dir--;
      }
    }
  }

void doublemap() {
  for(string& s: fmap) {
    string res = "";
    for(char c: s) res += c, res += c;
    s = res;
    }
  fmap.resize(isize(fmap) * 2);
  for(int y=isize(fmap)-1; y>=0; y--) fmap[y] = fmap[y/2];

  int Y = isize(fmap);
  int X = isize(fmap[0]);
  
  for(int y=Y-1; y>=0; y--) for(int x=X-1; x>=0; x--)
    vx[y][x] = vx[y/2][x/2], vy[y][x] = vy[y/2][x/2];

  for(int y=0; y<Y; y++) for(int x=0; x<X; x++) {
    coord cc{x,y};
    auto& us = fmap[y][x];
    if(us >= '4') {
      bool live = false;
      for(int k=0; k<4; k++) if(fmap_at(cc+k) == '1') live = true;
      if(!live) us = '0';
      }
    }
  
  bool found6 = false, found7 = false;
  for(int y=0; y<Y; y++) for(int x=0; x<X; x++) {
    if(fmap[y][x] == '6') {
      if(found6) fmap[y][x] = '4'; else found6 = true;
      }
    if(fmap[y][x] == '7') {
      if(found7) fmap[y][x] = '5'; else found7 = true;
      }
    fix_border();
    }  
  }

int pointmode;

bool show_mapping = true;
bool show_mgrid = true;

ld mapping_split = .75;

char paintmode;

void changepoint(int x, int y, bool can_add) {
  int Y = isize(fmap);
  int X = isize(fmap[0]);
  if(x < 0 || y < 0 || x >= X || y >= Y) return;
  if(pointmode) {
    if(fmap[y][x] < '4') return;
    for(string& s: fmap) for(char& c: s) if(c == pointmode) c = '4';
    fmap[y][x] = pointmode;
    fix_border();
    return;
    }
  if(y == 0) {
    paintmode = 0;
    fmap.emplace_back();
    for(int i=Y-1; i>=0; i--) fmap[i+1] = fmap[i];
    for(char& c: fmap[0]) c = '0';
    }
  if(y == Y-1) {
    paintmode = 0;
    fmap.push_back(fmap[0]);
    for(char& c: fmap.back()) c = '0';
    }
  if(x == 0) {
    paintmode = 0;
    for(string& s: fmap) s = '0' + s;
    }
  if(x == X-1) {
    paintmode = 0;
    for(string& s: fmap) s = s + '0';
    }
  coord cc{x,y};
  auto& us = fmap[y][x];
  if(fmap[y][x] >= '4') {
    if(fmap[y][x] > '5') {
      int q = 0;        
      for(int k=0; k<4; k++) if(fmap_at(cc+k) == '0') q++;
      if(q != 1) return;
      }
    else {
      for(int k=0; k<4; k++) if(fmap_at(cc+k) == (us^ '1')) return;
      int q = 0;
      for(int k=0; k<4; k++) if(fmap_at(cc+k) >= '4') q++;
      if(q > 2) return;
      }
    for(int k=0; k<4; k++) 
      if(fmap_at(cc+k) != '1') 
      if(fmap_at(cc+k+(k+1)) == '1') 
      if(fmap_at(cc+(k+1)) != '1') 
        return;

    for(int k=0; k<4; k++) 
      if(fmap_at(cc+k) != '1') 
      if(fmap_at(cc+k+k) == '1') 
      if(fmap_at(cc+k+(k+1)) != '1') 
      if(fmap_at(cc+k+(k-1)) != '1') 
        return;
    
    if(fmap[y-1][x] == '0') fmap[y-1][x] = us;
    if(fmap[y][x-1] == '0') fmap[y][x-1] = us;
    if(fmap[y+1][x] == '0') fmap[y+1][x] = us;
    if(fmap[y][x+1] == '0') fmap[y][x+1] = us;
    us = '1';
    }
  else if(us == '1') {
    int q = 0;
    for(int k=0; k<4; k++) if(fmap_at(cc+k) == '1') q++;
    if(q == 0) return;
    if(q == 4) return;
    if(q == 3) {
      for(int k=0; k<4; k++) if(fmap_at(cc+k) != '1') {
        int nei = 0;
        if(fmap_at(cc+k+(k+1)) == '1') nei++;
        if(fmap_at(cc+k+(k+3)) == '1') nei++;
        if(nei == 2) return;
        us = fmap_at(cc+k);
        if(nei == 0) fmap_at(cc+k) = '0';
        }
      return;
      }
    if(q == 2 && fmap_at(cc+0) == '1' && fmap_at(cc+2) == '1') return;
    if(q == 2 && fmap_at(cc+1) == '1' && fmap_at(cc+3) == '1') return;
    for(int k=0; k<4; k++) if(fmap_at(cc+k) == '1' && fmap_at(cc+(k+1)) == '1' && fmap_at(cc+k+(k+1)) != '1') return;
    bool have4 = false, kill6 = false, kill7 = false, live[4];
    for(int k=0; k<4; k++) {
      char ch = fmap_at(cc+k);

      live[k] = false;
      for(int k=0; k<4; k++) {
        for(int l=0; l<4; l++) if(k!=(l^2) && fmap_at(cc+k+l) == '1') live[k] = true;
        }

      switch(ch) {
        case '4': have4 = true; break;
        case '5': break;
        case '6': if(!live[k]) kill6 = true; break;
        case '7': if(!live[k]) kill7 = true; break;
        };
      }
    
    if(kill6 && kill7) return;
    
    if(kill6) us = '6';
    else if(kill7) us = '7';
    else if(have4) us = '4';
    else us = '5';
    
    for(int k=0; k<4; k++) if(!live[k]) fmap_at(cc+k) = '0';
    }
  }

bool showmenu = true;

void conf_shapes() {
  cmode = 0;
  dialog::init(XLAT("shapes"));
  dialog::addItem("square 11x11", 'a');
  dialog::add_action([] { fmap = gensquare(13, 13); reset_vxy(); popScreen(); });
  dialog::addItem("rectangle 15x8", 'b');
  dialog::add_action([] { fmap = gensquare(15, 8); reset_vxy(); popScreen(); });
  dialog::addItem("T-shape", 'c');
  dialog::add_action([] { fmap = gent(19, 7); reset_vxy(); popScreen(); });
  dialog::addItem("snake", 'd');
  dialog::add_action([] { fmap = snake; reset_vxy(); popScreen(); });
  dialog::addItem("ellipse 50", 'e');
  dialog::add_action([] { fmap = genellipse(50, hrand(180)); reset_vxy(); popScreen(); });
  dialog::addItem("apple", 'f');
  dialog::add_action([] { fmap = apple; reset_vxy(); popScreen(); });
  dialog::addBreak(100);
  dialog::addBack();
  dialog::display();
  }

int algo_speed = 10000;

int algo_ticks = 0;

bool in_visualization;
int nconf_pos;
ld steps_to_do;

#define DELTA [] { static int oldticks = ticks; int res = ticks - oldticks; oldticks = ticks; return res; }

auto nconf_delta = DELTA;

#if CAP_NCONF
void nconf_prepare(bool fast) {
  // pretend we are solving
  nconf_solve();
  build_equations(nconf::pts, 1, fast);
  nconf_pos = 0;
  in_visualization = true;
  steps_to_do = 0;
  algo_ticks = 0;
  nconf_delta();
  printf("points: %d\n", isize(nconf::allpoints));
  }

void nconf_run() {
  int d = nconf_delta();
  algo_ticks += d;
  steps_to_do += d * algo_speed / 1000.;
  while(steps_to_do > 0) {
    if(nconf_pos == isize(nconf::allpoints)) { in_visualization = false; break; }
    auto co = nconf::allpoints[nconf_pos++];
    auto &p = nconf::pts[co];
    nconf::eliminate(nconf::pts, co);
    printf("point #%d has %d equations\n", nconf_pos-1, hr::isize(p.eqs));
    steps_to_do -= pow(hr::isize(p.eqs), 2);
    }
  }

void pick_algorithm() {
  cmode = 0;
  dialog::init(XLAT("solving"));
  dialog::addItem("iterative/reset", 'r');
  dialog::add_action([] { reset_vxy(); algo_ticks = 0; popScreen(); });
  dialog::addItem("solve linear equations", 's');
  dialog::add_action([] { nconf_solve(); algo_ticks = 0; popScreen(); });
  dialog::addItem("visualize (slow)", 'a');
  dialog::add_action([] { nconf_prepare(false); popScreen(); });
  dialog::addItem("visualize (fast)", 'b');
  dialog::add_action([] { nconf_prepare(true); popScreen(); });
  dialog::addSelItem("visualization speed", its(algo_speed), 'v');
  dialog::add_action([] { dialog::editNumber(algo_speed, 100, 1000000, 0.1, 10000, "", ""), dialog::scaleLog(), dialog::dialogflags = sm::NOSCR; });
  dialog::addBreak(50);
  dialog::addBoolItem_action("pretty corners", pretty, 'p');
  dialog::addBreak(50);
  dialog::addBack();
  dialog::display();
  }
#endif

void ncee_work();

bool animated_pattern = false;

int redraws;

void redraw_texture() {
  View = Id;
  if(arcm::in()) View = View * spin(45._deg);
  dynamicval<int> cgl(vid.cells_generated_limit, 9999999);
  dynamicval<int> cdl(vid.cells_drawn_limit, 9999999);
  dynamicval<bool> r(mousing, false);
  rug::rugged = true;
  rug::prepareTexture();
  rug::rugged = false;
  }

template<class T> void chg_pattern(const T& f) {
  tactic::on = true;
  autocheat = true;
  reptilecheat = true;
  stop_game();
  set_geometry(gNormal);
  set_variation(eVariation::bitruncated);
  f();
  start_game();
  clearMessages();
  redraws = 1;
  }

void pick_pattern() {
  cmode = sm::SIDE | sm::MAYDARK | sm::DIALOG_STRICT_X;
  ncee_work();
  dialog::init(XLAT("patterns"));

  dialog::addItem("green football", 'g');
  dialog::add_action([] { 
    chg_pattern([] {
      enable_canvas();
      patterns::whichCanvas = 'B';
      });
    });

  dialog::addItem("Goldberg football", 'G');
  dialog::add_action([] { 
    chg_pattern([] {
      gp::param.first = 9;
      gp::param.second = 0;
      set_variation(eVariation::goldberg);
      enable_canvas();
      patterns::whichCanvas = 'F';
      });
    });

  dialog::addItem("octagons", 'o');
  dialog::add_action([] { 
    chg_pattern([] {
      set_geometry(gOctagon);
      enable_canvas();
      patterns::whichCanvas = 'T';
      });
    });

  dialog::addItem("windy plains", 'w');
  dialog::add_action([] { 
    chg_pattern([] {
      firstland = specialland = laWhirlwind;
      });
    vid.smart_range_detail = 2.5;
    });
  
  dialog::addItem("reptiles", 'r');
  dialog::add_action([] { 
    chg_pattern([] {
      firstland = specialland = laReptile;
      });
    });
  
  dialog::addItem("zebra", 'z');
  dialog::add_action([] { 
    chg_pattern([] {
      firstland = specialland = laZebra;
      });
    });
  
  dialog::addItem("colored squares", 's');
  dialog::add_action([] { 
    chg_pattern([] {
      set_variation(eVariation::pure);
      arcm::current.parse("4^5");
      set_geometry(gArchimedean);
      enable_canvas();
      patterns::whichCanvas = 'A';
      });
    });
    
  if(specialland == laWhirlwind) {
    dialog::addBoolItem_action("animated", animated_pattern, 'a');
    }
  else dialog::addBreak(100);

  dialog::addBreak(50);
  dialog::addBack();
  dialog::display();
  }

namespace ncee_scr {
  int X, Y, xc, yc, x0, y0, siz;
  }

void draw_ncee() {
  using namespace ncee_scr;
  auto cd = current_display;
  
  Y = isize(fmap);
  X = isize(fmap[0]);
  siz = min((cd->ysize / (show_mapping ? 2 : 1) - 5) / Y, showmenu ? (cd->xcenter -5 )*2/X : (cd->xsize - 5) / X);
  
  xc = 0;
  yc = vid.yres * (show_mapping ? mapping_split : 1) / 2 - cd->ycenter;
  
  x0 = - int(siz * X / 2);
  y0 = - int(siz * Y / 2);
  
  ld period;
  
  if(geometry == gNormal) {

    auto cw = heptspin(cwt.at->master, 0);
    cw = cw + wstep + 3 + wstep + 5 + wstep;
    period = hdist0(tC0(currentmap->relative_matrix(cwt.at, cw.at->c7, C0)));
  
    if(specialland == laWhirlwind)
      period *= 9;
    if(specialland == laZebra)
      period *= 6;
    if(specialland == laReptile)
      period *= 3;
    }

  else if(geometry == gOctagon) {
    period = 2 * hdist0(tC0(currentmap->adj(cwt.at->master, 0)));
    }

  else {
    period = 2 * hdist0(tC0(currentmap->adj(cwt.at, 0)));
    }

  period /= 90._deg;
  
  dynamicval<eModel> pm(pmodel, mdPixel);
  dynamicval<eGeometry> pg(geometry, gEuclid);
  
  initquickqueue();
  nctinf2.texture_id = rug::glbuf->renderedTexture;
  nctinf2.tvertices.clear();
  
  ld map_ypos = vid.yres * (mapping_split + 1) / 2 - cd->ycenter;
  ld sca2 = (vid.yres * (1-mapping_split) / 2 - 10)  / pconf.scale;

  if(show_mapping) {
    for(int iter=-10; iter<=10; iter++) {
      ld maxx = period * pconf.scale / 4;
      ld scax = sca2 * maxx / 0.5;
      ld xpos = scax * 2 * iter;
      curvepoint(hpxy(xpos-scax, map_ypos-sca2));
      nctinf2.tvertices.push_back(glhr::makevertex(0.5-maxx, 0, 0));
      curvepoint(hpxy(xpos-scax, map_ypos+sca2));
      nctinf2.tvertices.push_back(glhr::makevertex(0.5-maxx, 1, 0));
      curvepoint(hpxy(xpos+scax, map_ypos-sca2));
      nctinf2.tvertices.push_back(glhr::makevertex(0.5+maxx, 0, 0));
      curvepoint(hpxy(xpos-scax, map_ypos+sca2));
      nctinf2.tvertices.push_back(glhr::makevertex(0.5-maxx, 1, 0));
      curvepoint(hpxy(xpos+scax, map_ypos-sca2));
      nctinf2.tvertices.push_back(glhr::makevertex(0.5+maxx, 0, 0));
      curvepoint(hpxy(xpos+scax, map_ypos+sca2));
      nctinf2.tvertices.push_back(glhr::makevertex(0.5+maxx, 1, 0));
      }
    auto& q = queuecurve(shiftless(Id), 0, show_mgrid ? 0x404040FF : 0xFFFFFFFF, PPR::LINE);
    q.tinf = &nctinf2;
    q.flags |= POLY_TRIANGLES;
    q.offset_texture = 0;
    }
    
  auto h = [&] (int x, int y) { return hpxy(x0 + x * siz + xc, y0 + y * siz + yc); };
  auto hc = [&] (int x, int y) { return hpxy(x0 + x * siz + siz/2 + xc, y0 + y * siz + siz/2 + yc); };
  
  color_t typecols[8] = {
    0x101010FF, 0xD0D0D0FF, 0, 0, 0xF04040FF, 0x4040F0FF, 0xF0F040FF, 0x40F0F0FF
    };

  for(int x=0; x<X; x++) for(int y=0; y<Y; y++) {
    curvepoint(h(x,y));
    curvepoint(h(x+1,y));
    curvepoint(h(x+1,y+1));
    curvepoint(h(x,y+1));
    #if CAP_NCONF
    bool ineq = 
      in_visualization && fmap[y][x] == '1';
    #endif
    queuecurve(shiftless(Id), 0, 
    #if CAP_NCONF
      (ineq && nconf::pts[y][x].state == 1) ? 0xFF8000FF :
      (ineq && nconf::pts[y][x].state == 2) ? 0x00FF00FF :
    #endif
      (fmap[y][x] == '1' && show_mgrid && show_mapping) ? 0x404040FF : typecols[fmap[y][x] - '0'], PPR::LINE);
    }
  
  if(inHighQual) for(int x=0; x<X; x++) for(int y=0; y<Y; y++) {
    curvepoint(h(x,y));
    curvepoint(h(x,y+1));
    curvepoint(h(x+1,y+1));
    curvepoint(h(x+1,y));
    queuecurve(shiftless(Id), 0, 
      typecols[fmap[y][x] - '0'], PPR::LINE);
    }

  quickqueue();
  initquickqueue();

  nctinf.texture_id = rug::glbuf->renderedTexture;
  nctinf.tvertices.clear();
  
  static int z = 0;

  auto tri = [&] (const array<coord,3>& c)  {
    
    int id = -1;
    for(int i=0; i<3; i++) if(fmap_at(c[i])) id = i;
    if(id == -1) return;
    ld delta = (int((vx[c[id].y][c[id].x]) / cscale / period + 1000.5) - 1000) * period;
    z = !z;
    for(int s=0; s<3; s++) {
      curvepoint(hc(c[s].x, c[s].y)); 
      nctinf.tvertices.push_back(glhr::makevertex((vx[c[s].y][c[s].x]/cscale-delta)*pconf.scale/2+.5, vy[c[s].y][c[s].x]*pconf.scale/2+.5, 0));
      }
    };
    
  if(viewmap && !in_visualization) for(int x=0; x<X-1; x++) for(int y=0; y<Y-1; y++) {
    if(fmap[y][x+1] > '0' && fmap[y+1][x] > '0') {
      if(fmap[y][x] > '0') tri(make_array(coord{x,y}, coord{x+1,y}, coord{x,y+1}));
      if(fmap[y+1][x+1] > '0') tri(make_array(coord{x+1,y+1}, coord{x+1,y}, coord{x,y+1}));
      }
    else if(fmap[y][x] > '0' && fmap[y+1][x+1] > '0') {
      if(fmap[y][x+1] > '0') tri(make_array(coord{x,y}, coord{x+1,y}, coord{x+1,y+1}));
      if(fmap[y+1][x] > '0') tri(make_array(coord{x,y}, coord{x,y+1}, coord{x+1,y+1}));
      }
    }
  
  auto& q = queuecurve(shiftless(Id), 0, (show_mgrid && show_mapping) ? 0x404040FF : 0xFFFFFFFF, PPR::LINE);
  q.tinf = &nctinf;
  q.flags |= POLY_TRIANGLES;
  q.offset_texture = 0;
  
  hyperpoint vmap[256][256];

  pair<int, int> mpt = {(mousex - xc - cd->xcenter - x0) / siz, (mousey - yc - cd->ycenter - y0) / siz};

  queueline1(h(0,0), h(0,-1), 0x1010101);
  
  const color_t gridcol = 0xFFFFFFFF;
  if(inHighQual) ;
  else if(show_mapping && show_mgrid && !in_visualization) {
    for(int x=0; x<X-1; x++) for(int y=0; y<Y-1; y++) 
      if(fmap[y][x] > '0')
        vmap[y][x] = hpxy(vx[y][x]/cscale * sca2 / 2, vy[y][x] * sca2 / 2+ map_ypos);
    for(int x=0; x<X-1; x++) for(int y=0; y<Y-1; y++) {
      if(y < Y-2 && fmap[y][x] > '0' && fmap[y+1][x] > '0') {
        color_t col = (make_pair(x,y) == mpt || make_pair(x,y+1) == mpt) ? 0xFFFF00FF : gridcol;
        dynamicval<ld> lw(vid.linewidth, vid.linewidth * (col == 0xFFFF00FF ? 4 : 1));
        queueline1(hc(x, y), hc(x, y+1), col, 0, PPR::CIRCLE);
        queueline1(vmap[y][x], vmap[y+1][x], col, 0, PPR::CIRCLE);
        }
      if(x < X-2 && fmap[y][x] > '0' && fmap[y][x+1] > '0') {
        color_t col = (make_pair(x,y) == mpt || make_pair(x+1,y) == mpt) ? 0xFFFF00FF : gridcol;
        dynamicval<ld> lw(vid.linewidth, vid.linewidth * (col == 0xFFFF00FF ? 4 : 1));
        queueline1(hc(x, y), hc(x+1, y), col, 0, PPR::CIRCLE);
        queueline1(vmap[y][x], vmap[y][x+1], col, 0, PPR::CIRCLE);
        }
      }
    }
  else {
    int x = mpt.first;
    int y = mpt.second;
    vid.linewidth *= 3;
    color_t col = 0;
    if(x >= 0 && y >= 0 && x < X && y < Y) {
      if(pointmode != 0) 
        col = 0x00FF00FF;
      else if(mousepressed)
        col = paintmode == '1' ? 0xFF0000FF : 0xFFFF00FF;
      else if(fmap[y][x] == '1')
        col = 0xFF0000FF;
      else if(fmap[y][x] == '4' || fmap[y][x] == '5')
        col = 0xFFFF00FF;
      }
    queueline1(h(x,y), h(x,y+1), col);
    queueline1(h(x,y), h(x+1,y), col);
    queueline1(h(x+1,y), h(x+1,y+1), col);
    queueline1(h(x,y+1), h(x+1,y+1), col);
    vid.linewidth /= 3;
    }
  
  for(int x=0; x<=X; x++) queueline1(h(x,0), h(x,Y), 0x80808080);
  for(int y=0; y<=Y; y++) queueline1(h(0,y), h(X,y), 0x80808080);
  
  queueline1(h(0,0), h(0,-1), 0x1010101);

  quickqueue();
  glflush();
  }

int ncee_map_prepared;

void prepare_ncee_map() {
  ncee_map_prepared = 5;
  pmodel = mdBand;
  dynamicval<int> cgl(vid.cells_generated_limit, 9999999);
  dynamicval<bool> r(rug::display_warning, false);
  // vid.consider_shader_projection = false;
  pconf.scale = 0.5;
  rug::init();
  rug::prepareTexture();
  rug::rugged = false;
  }
  
void ncee_work() {

  if(specialland != laWhirlwind)
    animated_pattern = false;

  if(redraws > 0) { 
    redraws--; 
    vid.consider_shader_projection = false;
    redraw_texture(); 
    }
  if(animated_pattern) {
    vid.consider_shader_projection = true;
    redraw_texture();
    }

  calcparam();
  
  if(ncee_map_prepared < 5) { cmode = sm::NORMAL | sm::DARKEN; ncee_map_prepared++; if(ncee_map_prepared == 5) prepare_ncee_map(); gamescreen(); return; }

  #if CAP_NCONF
  if(in_visualization) 
    nconf_run();
  else 
  #endif
    iterate();
  
  draw_ncee();
  }

void ncee() {
  cmode = showmenu ? (sm::SIDE | sm::MAYDARK | sm::DIALOG_STRICT_X) : 0;
  ncee_work();
  using namespace ncee_scr;
  auto cd = current_display;

  getcstat = '-';
  
  if(paintmode && mousepressed) {
    int x = (mousex - cd->xcenter - xc - x0) / siz;
    int y = (mousey - cd->ycenter - yc - y0) / siz;
    if(fmap[y][x] == paintmode)
      changepoint(x, y, false);
    }
  
  if(showmenu) {
    dialog::init(XLAT("newconformist"));
    dialog::addBoolItem("edit shape", pointmode == 0, 'e');
    dialog::addBoolItem("set left end", pointmode == '6', 'a');
    dialog::addBoolItem("set right end", pointmode == '7', 'b');
    dialog::addBreak(50);
    dialog::addBoolItem("show the end result", viewmap, 'm');
    dialog::addBoolItem("display the band model", show_mapping, 's');
    if(show_mapping)
      dialog::addBoolItem("display the grid on model", show_mgrid, 'g');
    else
      dialog::addBreak(100);
    dialog::addBreak(50);
    if(show_mapping)
      dialog::addSelItem("mapping split", fts(mapping_split), 'y');
    else
      dialog::addBreak(100);
    dialog::addSelItem("double precision", its(isize(fmap[0])) + "x" + its(isize(fmap)), 'd');
    #if CAP_NCONF
    dialog::addItem("solving method", 'l');
    #endif
    dialog::addItem("shapes", 't');
    dialog::addItem("patterns", 'p');
    dialog::addItem("hide the menu", 'v');
    dialog::addItem("stop", 'x');
    dialog::display();
    }
  else
    displayButton(vid.xres - 8, 8 + vid.fsize, XLAT("(v) menu"), 'v', 16);

  if(algo_ticks)
    displaystr(8, 8 + vid.fsize, 0, vid.fsize * 2, format("%d.%03d", algo_ticks/1000, algo_ticks%1000), 0xFFFFFF, 0);
  
  keyhandler = [=] (int sym, int uni) {
    // dialog::handleNavigation(sym, uni);
    if(uni == 'z')
      prepare_ncee_map();
    
    if(uni == 'x') {
      popScreen();
      // rug::rugged = true;
      rug::close();
      }
    if(uni == 'e') pointmode = 0;
    if(uni == 'a') pointmode = '6';
    if(uni == 'b') pointmode = '7';
    #if CAP_NCONF
    if(uni == 'l') pushScreen(pick_algorithm);
    #endif
    if(uni == 'p') pushScreen(pick_pattern);
    // if(uni == 'w') edit_whatever('f', 0);
    if(uni == 'd') doublemap();
    if(uni == 'm') viewmap = !viewmap;
    if(uni == 's') show_mapping = !show_mapping;
    if(uni == 'g') show_mgrid = !show_mgrid;
    if(uni == 't') pushScreen(conf_shapes);
    if(uni == 'y') dialog::editNumber(mapping_split, 0, 1, 0.05, 0.75, "", ""), dialog::dialogflags = sm::NOSCR;
    if(uni == '-') {
      int x = (mousex - cd->xcenter - xc - x0) / siz;
      int y = (mousey - cd->ycenter - yc - y0) / siz;
      if(x < 0 || y < 0 || x >= X || y >= Y) return;
      paintmode = fmap[y][x];
      changepoint(x, y, true);
      }
    if(uni == 'v') showmenu = !showmenu;
    if(uni == 'X') {
      int D = 100;
  
      fmap = genellipse(D, -10._deg), reset_vxy();
      #if CAP_NCONF
      nconf_solve(); 
      #endif
      iterate();
      iterate();      
      iterate();
      
      int slow = 2;
  
      for(int i=-100; i<180*slow; i++) {
        ticks = anims::period * i / 180 / slow;
        redraw_texture();
        fmap = genellipse(D, i * degree / slow);
        println(hlog, "i = ", i);
        for(int a=0; a<10; a++) iterate();
        if(i >= 0) shot::take(format(rfname.c_str(), i), draw_ncee);
        }
      }
    };
  }

extern "C" {
  void nconf_view(int i) {
    if(i == 1) 
      show_mapping = false, viewmap = false;
    else if(i == 2)
      show_mapping = false, viewmap = true;
    else if(i == 3)
      show_mapping = true, viewmap = true, show_mgrid = true;
    else if(i == 4)
      showmenu = false;
    else if(i == 5)
      showmenu = true;
    else if(i == 6)
      reset_vxy(), algo_ticks = 0;
    #if CAP_NCONF
    else if(i == 7)
      nconf_solve(), algo_ticks = 0;
    else if(i == 8)
      nconf_prepare(false);
    else if(i == 9)
      nconf_prepare(true);
    #endif
    else if(i == 10)
      doublemap();
    else if(i == 11)
      fmap = gensquare(13, 13), reset_vxy();
    else if(i == 12)
      fmap = gensquare(15, 8), reset_vxy();
    else if(i == 13)
      fmap = gensquare(19, 7), reset_vxy();
    else if(i == 14)
      fmap = snake, reset_vxy();
    else if(i == 15)
      pushScreen(pick_pattern);
    else if(i == 16)
      fmap = apple, reset_vxy();
    }
  }
  
int niceArgs() {
  using namespace arg;
           
  if(0) ;
  else if(argis("-ncee")) {
    PHASE(3);
    pushScreen(ncee);
    showstartmenu = false;
    clearMessages();
    }
  else if(argis("-ncv")) {
    PHASE(3);
    shift();
    nconf_view(argi());
    }
  else if(argis("-ncvid")) {
    shift(); rfname = args();
    }
    
  else return 1;
  return 0;
  }

auto nhook = 
  addHook(hooks_args, 100, niceArgs)
+ 0;

}

}
