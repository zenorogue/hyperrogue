namespace whirl {
  bool whirl;
  loc param(1, 0);

  hyperpoint next;
  ld scale;
  ld alpha;
  int area;
  
  loc operator+(loc e1, loc e2) {
    return make_pair(e1.first+e2.first, e1.second+e2.second);
    }
  
  loc operator-(loc e1, loc e2) {
    return make_pair(e1.first-e2.first, e1.second-e2.second);
    }
  
  loc operator*(loc e1, loc e2) {
    return make_pair(e1.first*e2.first-e1.second*e2.second, 
      e1.first*e2.second + e2.first*e1.second + e1.second*e2.second);
    }
  
  struct whirlmap_t {
    cell *c;
    char rdir;
    int rspin;
    };

  loc eudir(int d) {
    d %= 6; if (d < 0) d += 6;
    switch(d) {
      case 0: return make_pair(1, 0);
      case 1: return make_pair(0, 1);
      case 2: return make_pair(-1, 1);
      case 3: return make_pair(-1, 0);
      case 4: return make_pair(0, -1);
      case 5: return make_pair(1, -1);
      default: return make_pair(0, 0);
      }
    }
  
  int get_code(const local_info& li) {
    return 
      ((li.relative.first & 15) << 0) +
      ((li.relative.second & 15) << 4) +
      ((fix6(li.total_dir)) << 8) +
      ((li.last_dir & 15) << 12);
    }
  
  local_info get_local_info(cell *c) {
    local_info li;
    if(c == c->master->c7) {
      li.relative = loc(0,0);
      li.first_dir = -1;
      li.last_dir = -1;
      li.total_dir = -1;
      }
    else {
      vector<int> dirs;
      while(c != c->master->c7) {
        dirs.push_back(c->spin(0));
        c = c->mov[0];
        }
      li.first_dir = dirs[0];
      li.last_dir = dirs.back();

      loc at(0,0);
      int dir = 0;
      at = at + eudir(dir);
      dirs.pop_back();
      while(dirs.size()) {
        dir += dirs.back() + 3;
        dirs.pop_back();
        at = at + eudir(dir);
        }
      li.relative = at;
      li.total_dir = dir + 3;
      }
    return li;
    }
    
  int last_dir(cell *c) {
    return get_local_info(c).last_dir;
    }

  loc get_coord(cell *c) {
    return get_local_info(c).relative;
    }
  
  int pseudohept_val(cell *c) {
    loc v = get_coord(c);
    return (v.first - v.second + MODFIXER)%3;
    }
  
  whirlmap_t whirlmap[20][20];
  void whirl_clear() {
    for(int y=0; y<20; y++) for(int x=0; x<20; x++) {
      whirlmap[y][x].c = NULL;
      whirlmap[y][x].rdir = -1;
      }
    }
  
  whirlmap_t& whirl_get(loc c) {
    return whirlmap[c.second + 10][c.first + 10];
    }

  const char *disp(loc at) { 
    static char bufs[16][16];
    static int bufid;
    bufid++; bufid %= 16;
    snprintf(bufs[bufid], 16, "[%d,%d]", at.first, at.second);
    return bufs[bufid];
    }

  int spawn;
  
#define WHD(x) // x

  void conn1(loc at, int dir, int dir1) {
    auto& wc = whirl_get(at);
    auto& wc1 = whirl_get(at + eudir(dir));
    int cdir = fixdir(dir + wc.rspin, wc.c);
    WHD( printf("  connection %s/%d %p/%d ", disp(at), dir, wc.c, cdir); )
    if(!wc1.c) {
      wc1.c = wc.c->mov[cdir];
      if(wc1.c) {
        // wc1.c/wc.c->spin(cdir) == dir1
        wc1.rspin = fixdir(wc.c->spin(cdir) - dir1, wc1.c);
        WHD( printf("(pulled) "); )
        }
      if(!wc1.c) {
        wc1.c = newCell(6, wc.c->master);
        spawn++;
        // 0 for wc1.c should be dir1
        wc1.rspin = fix6(-dir1);
        WHD( printf("(created) "); )
        }
      }
    int cdir1 = fixdir(dir1 + wc1.rspin, wc1.c);
    WHD( printf("(%p/%d) ", wc1.c, cdir1); )
    if(wc.c->mov[cdir] && wc.c->mov[cdir] != wc1.c) {
      WHD( printf("FAIL: %p\n", wc.c->mov[cdir]); exit(1); )
      }
    if(wc.c->mov[cdir]) {
      if(wc.c->spin(cdir) != cdir1) {
        printf("warning: wrong spin\n");
        exit(1);
        }
      }
    WHD( else printf("ok\n"); )
    wc.c->mov[cdir] = wc1.c;
    tsetspin(wc.c->spintable, cdir, cdir1);
    }

  void conn(loc at, int dir) {
    conn1(at, fix6(dir), fix6(dir+3));
    conn1(at + eudir(dir), fix6(dir+3), fix6(dir));
    }
  
  void extend_map(cell *c, int d) {
    WHD( printf("EXTEND %p %d\n", c, d); )
    if(c->master->c7 != c) {
      while(c->master->c7 != c) {
        d = c->spin(0);
        c = c->mov[0];
        }
      // c move 0 equals c' move spin(0)
      extend_map(c, d);
      extend_map(c, fixdir(d-1, c));
      extend_map(c, fixdir(d+1, c));
      return;
      }

    whirl_clear();

    // we generate a local map from an Euclidean grid to the
    // hyperbolic grid we build.
    
    // we fill the equilateral triangle with the following vertices:

    loc vc[3];
    vc[0] = loc(0,0);
    vc[1] = param;
    vc[2] = param * loc(0,1);
    
    // whirl_get(loc) gives our local map. We set the vertices first
    {
    auto h = c->master;
    auto& ac0 = whirl_get(vc[0]);
    ac0.c = h->c7;
    ac0.rspin = d;
    
    auto& ac1 = whirl_get(vc[1]);
    ac1.c = createStep(h, d)->c7;
    WHD( printf("%s : %p\n", disp(vc[1]), ac1.c); )
 
    // 3 ~ h->spin(d)
    ac1.rspin = h->spin(d) - 3;
    
    auto& ac2 = whirl_get(vc[2]);
    ac2.c = createStep(h, (d+1)%S7)->c7;
    WHD( printf("%s : %p\n", disp(vc[2]), ac2.c); )
    // 4 ~ h->spin(d+1)
    ac2.rspin = h->spin((d+1)%S7) - 4;
    }

    // then we set the edges of our big equilateral triangle (in a symmetric way)
    for(int i=0; i<3; i++) {
      loc start = vc[i];
      loc end = vc[(i+1)%3];
      WHD( printf("from %s to %s\n", disp(start), disp(end)); )
      loc rel = param;
      auto build = [&] (loc& at, int dx, bool forward) {
        int dx1 = dx + 2*i;
        WHD( printf("%s %d\n", disp(at), dx1); )
        conn(at, dx1);        
        if(forward) whirl_get(at).rdir = fix6(dx1);
        else whirl_get(at+eudir(dx1)).rdir = fix6(dx1+3);
        at = at + eudir(dx1);
        };
      while(rel.first >= 2) {
        build(start, 0, true);
        build(end, 3, false);
        rel.first -= 2;
        }
      while(rel.second >= 2) {
        build(start, 1, true);
        build(end, 4, false);
        rel.second -= 2;
        }
      while(rel.first>0 && rel.second) {
        build(start, 0, true);
        build(end, 3, false);
        rel.first -= 2;
        }
      for(int k=0; k<6; k++)
        if(start + eudir(k+2*i) == end)
          build(start, k, true);                         
      if(start != end) { printf("assertion failed: start %s == end %s\n", disp(start), disp(end)); exit(1); }
      }

    // now we can fill the interior of our big equilateral triangle
    loc at = vc[0];
    while(true) {
      auto& wc = whirl_get(at);
      int dx = wc.rdir;
      auto at1 = at + eudir(dx);
      auto& wc1 = whirl_get(at1);
      WHD( printf("%s (%d) %s (%d)\n", disp(at), dx, disp(at1), wc1.rdir); )
      int df = wc1.rdir - dx;
      if(df < 0) df += 6;
      if(df == 3) break;
      switch(df) {
        case 0:
        case 4:
        case 5:
          at = at1;
          continue;
        case 2: {
          conn(at, dx+1);
          wc.rdir = (dx+1) % 6;
          break;
          }
        case 1: {
          auto at2 = at + eudir(dx+1);
          auto& wc2 = whirl_get(at2);
          if(wc2.c) { at = at1; continue; }
          wc.rdir = (dx+1) % 6;
          conn(at, (dx+1) % 6);
          conn(at1, (dx+2) % 6);
          conn(at2, (dx+0) % 6);
          wc1.rdir = -1;
          wc2.rdir = dx; 
          break;
          }
        default:
          printf("case unhandled %d\n", df);
          exit(1);
        }
      }
    WHD( printf("DONE\n\n"); )
    }
  
  hyperpoint loctoh_ort(loc at) {
    return hpxyz(at.first, at.second, 1);
    }

  hyperpoint corner_coords[7] = {
    hpxyz(2, -1, 0),
    hpxyz(1, 1, 0),
    hpxyz(-1, 2, 0),
    hpxyz(-2, 1, 0),
    hpxyz(-1, -1, 0),
    hpxyz(1, -2, 0),
    hpxyz(0, 0, 0) // center, not a corner
    };
    
  int bak_sp;
  
  hyperpoint atz(const transmatrix& T, const transmatrix& corners, loc at, int cornerid = 6, ld cf = 3) {
    int sp = 0;
    again:
    auto corner = corners * hyperpoint_vec::operator+ (loctoh_ort(at), hyperpoint_vec::operator/ (corner_coords[cornerid], cf));
    if(corner[1] < -1e-6 || corner[2] < -1e-6) {
      at = at * eudir(1);
      if(cornerid < 6) cornerid = (1 + cornerid) % 6;
      sp++;
      goto again;
      }
    if(sp>3) sp -= 6; bak_sp = sp;

    return normalize(spin(2*M_PI*sp/S7) * T * corner);
    }
  
  transmatrix Tf[8][32][32][6];
  
  transmatrix corners;
  
  transmatrix dir_matrix(int i) {
    cell cc; cc.type = S7;
    return spin(-alpha) * build_matrix(
      C0, 
      ddspin(&cc, i) * xpush(tessf) * C0,
      ddspin(&cc, i+1) * xpush(tessf) * C0
      );
    }
  
  void prepare_matrices() {
    corners = inverse(build_matrix(
      loctoh_ort(loc(0,0)),
      loctoh_ort(param),
      loctoh_ort(param * loc(0,1))
      ));
    for(int i=0; i<S7; i++) {
      transmatrix T = dir_matrix(i);
      for(int x=-10; x<10; x++)
      for(int y=-10; y<10; y++)
      for(int d=0; d<6; d++) {
        loc at = loc(x, y);
        
        hyperpoint h = atz(T, corners, at, 6);
        hyperpoint hl = atz(T, corners, at + eudir(d), 6);
        Tf[i][x&31][y&31][d] = rgpushxto0(h) * rspintox(gpushxto0(h) * hl) * spin(M_PI);
        }
      }       
    }
  
  hyperpoint get_corner_position(cell *c, int cid, ld cf = 3) {
    auto li = get_local_info(c);
    int i = li.last_dir;
    if(i == -1) 
      return atz(dir_matrix(cid), corners, li.relative, 0, cf);
    else {
      auto& cellmatrix = Tf[i][li.relative.first&31][li.relative.second&31][fix6(li.total_dir)];
      return inverse(cellmatrix) * atz(dir_matrix(i), corners, li.relative, fix6(cid + li.total_dir), cf);
      }
    }
    
  void compute_geometry() {
    if(whirl) {
      int x = param.first;
      int y = param.second;
      area = ((2*x+y) * (2*x+y) + y*y*3) / 4;
      next = hpxyz(x+y/2., -y * sqrt(3) / 2, 0);
      scale = 1 / hypot2(next);
      crossf *= scale;
      hepvdist *= scale;
      rhexf *= scale;
//    spin = spintox(next);
//    ispin = rspintox(next);
      alpha = -atan2(next[1], next[0]);
      base_distlimit = (base_distlimit + log(scale) / log(2.618)) / scale;
      if(base_distlimit > 30)
        base_distlimit = 30;
      prepare_matrices();
      }
    else {
      scale = 1;
      alpha = 0;
      }
    }
  
  string operation_name() {
    if(!whirl::whirl) {
      if(nonbitrunc) return XLAT("OFF");
      else return XLAT("bitruncated");
      }
    else if(param == loc(1, 0))
      return XLAT("OFF");
    else if(param == loc(1, 1))
      return XLAT("bitruncated");
    else if(param == loc(2, 0))
      return XLAT("chamfered");
    else if(param == loc(3, 0))
      return XLAT("2x bitruncated");
    else
      return "(" + its(param.first) + "," + its(param.second) + ")";
    }
  
  int config_x, config_y;
  
  void whirl_set(int x, int y) {
    if(y < 0) { y = -y; x -= y; }
    if(x < 0) { x = -x; y = -y; }
    if(x < y) swap(x, y);
    if(x > 8) x = 8;
    if(y > 8) y = 8;
    config_x = x; config_y = y;
    param = loc(x, y);
    auto g = screens;
    if(x == 1 && y == 0) {
      if(whirl::whirl) restartGame('7');
      if(!nonbitrunc) restartGame('7');
      }
    else if(x == 1 && y == 1) {
      if(whirl::whirl) restartGame('7');
      if(nonbitrunc) restartGame('7');
      }
    else {
      if(nonbitrunc) restartGame('7');
      param = loc(x, y);
      restartGame('w');
      }
    screens = g;
    }

  string helptext() {
    return 
      "HyperRogue's map is obtained by applying an operator to the basic regular tesselation. "
      "Operator (x,y) means that, to get to a nearest non-hex from any non-hex, you should move x "
      "cells in any direction, turn right 60 degrees, and move y cells. "
      "By default HyperRogue uses bitruncation, which corresponds to (1,1).";
    }  

  void show() {
    cmode = sm::SIDE;
    gamescreen(0);  
    dialog::init(XLAT("operators"));
    
    dialog::addBoolItem(XLAT("OFF"), param == loc(1,0), 'a');
    dialog::lastItem().value = "(1,0)";

    dialog::addBoolItem(XLAT("bitruncated"), param == loc(1,1), 'b');
    dialog::lastItem().value = "(1,1)";

    dialog::addBoolItem(XLAT("chamfered"), param == loc(2,0), 'c');
    dialog::lastItem().value = "(2,0)";

    dialog::addBoolItem(XLAT("2x bitruncated"), param == loc(3,0), 'd');
    dialog::lastItem().value = "(3,0)";

    dialog::addBreak(100);
    dialog::addSelItem("x", its(config_x), 0);
    dialog::addSelItem("y", its(config_y), 0);
    dialog::addBoolItem(XLAT("tuned"), param == loc(config_x, config_y), 'f');

    dialog::addBreak(100);
    dialog::addItem(XLAT("help"), SDLK_F1);  
    dialog::addItem(XLAT("back"), '0');  
    dialog::display();

    keyhandler = [] (int sym, int uni) {
      dialog::handleNavigation(sym, uni);
      if(uni == 'a') 
        whirl_set(1, 0);
      else if(uni == 'b')
        whirl_set(1, 1);
      else if(uni == 'c')
        whirl_set(2, 0);
      else if(uni == 'd')
        whirl_set(3, 0);
      else if(uni == 'f')
        whirl_set(config_x, config_y);
      else if(uni == 'x')
        dialog::editNumber(config_x, 1, 10, 1, 1, "x", helptext());
      else if(uni == 'y')
        dialog::editNumber(config_y, 1, 10, 1, 1, "y", helptext());
      else if(doexiton(sym, uni))
        popScreen();
      };
    }
  
  void configure() {
    if(whirl::whirl)
      config_x = param.first, config_y = param.second;
    else if(nonbitrunc)
      config_x = 1, config_y = 0;
    else
      config_x = 1, config_y = 1;
    param = loc(config_x, config_y);
    pushScreen(whirl::show);
    }
  }

