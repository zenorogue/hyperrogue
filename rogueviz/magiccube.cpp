#include "rogueviz.h"

namespace rogueviz {

#if CAP_CRYSTAL

namespace magic {

int back = 0x202020;

int magiccolors[14] = { 0xFFFFFF, 0xFFFF00, 0x0000FF, 0x00FF00, 0xFF0000, 0xFF8000, 0x800080, 0x808080, 0x00FFFF, 0x80FFFF, 0x4040C0, 0x40C040, 0xC04040, 0xC0A040 };

void build(crystal::coord co, int at) {
  if(at < crystal::get_dim()) {
    for(int z: {0,2,-2,4,-4}) co[at] = z, build(co, at+1);
    return;
    }
  for(int i=0; i<S7; i++) crystal::get_heptagon_at(co)->cmove(i);

  int twos = 0, index = 0;
  for(int a=0; a<crystal::get_dim(); a++)
    if(co[a] == 4) twos++, index = 2*a;
    else if(co[a] == -4) twos++, index = 2*a+1;
  
  auto c = crystal::get_heptagon_at(co)->c7;
  setdist(c, 7, NULL);
  if(twos == 0) 
    c->landparam = back;
  else if(twos == 1) {
    c->landparam = magiccolors[index];
    if(WDIM == 3) c->wall = waWaxWall;
    }

  println(hlog, co, " twos = ", twos, " index = ", index, " set = ", format("%06X", c->landparam));
  
  }

void curveline(hyperpoint a, hyperpoint b, int lev) {
  if(lev>0) {
    hyperpoint c = mid(a, b);
    curveline(a, c, lev-1);
    curveline(c, b, lev-1);
    }
  curvepoint(b);
  }

bool magic_markers(cell *c, const shiftmatrix& V) {
  timerghost = false;
  if(c->landparam == back) {
    if(GDIM == 2) {
      auto co = crystal::get_coord(c->master);
      for(int a=0; a<S7/2; a++) if(co[a] >= 6 || co[a] <= -6) c->landparam = 0;
      }
    if(GDIM == 2)
    for(int i=0; i<S7; i++) {
      cell *c2 = c->move(i);
      if(c2->landparam != back) {
        hyperpoint h1 = get_corner_position(c, i, 3/.9);
        hyperpoint h2 = get_corner_position(c, i+1, 3/.9);
        curvepoint(h1);
        curveline(h1, h2, 3);
        hyperpoint h3 = get_corner_position(c, i, 3/.7);
        hyperpoint h4 = get_corner_position(c, i+1, 3/.7);
        curvepoint(h4);
        curveline(h4, h3, 3);
        queuecurve(V, 0xFF, (c2->landparam << 8) | 0xFF, PPR::LINE);
        }
      }
    }
  else {
    c->wall = GDIM == 2 ? waInvisibleFloor : waWaxWall;
    }
  return false;
  }

void twos_to_fours(vector<int>& zeros, crystal::coord co, int d) {
  if(d == crystal::get_dim()) {
    int facetable[5][5];
    for(int x=0; x<5; x++)
    for(int y=0; y<5; y++) {
      co[zeros[0]] = 2*x-4;
      co[zeros[1]] = 2*y-4;
      facetable[y][x] = crystal::get_heptagon_at(co)->c7->landparam;
      }
    for(int x=0; x<5; x++)
    for(int y=0; y<5; y++) {
      co[zeros[0]] = 2*y-4;
      co[zeros[1]] = 4-2*x;
      crystal::get_heptagon_at(co)->c7->landparam = facetable[y][x];
      }
    }
  else {
    twos_to_fours(zeros, co, d+1);
    if(co[d] == 2 || co[d] == -2) {
      co[d] *= 2;
      twos_to_fours(zeros, co, d+1);
      }
    }
  }

bool magic_rotate(cell *c) {
  if(c->landparam != back) return false;
  vector<int> zeros;
  auto co = crystal::get_coord(c->master);
  println(hlog, "co = ", co);
  for(int i=0; i<crystal::get_dim(); i++) {
    if(co[i] == 0) zeros.push_back(i);
    else if(co[i] == 2 || co[i] == -2) ;
    else if(co[i] == 4 || co[i] == -4) ;
    else return false;
    }
  println(hlog, "zeros = ", zeros);
  if(isize(zeros) != 2) return false;
  twos_to_fours(zeros, co, 0);
  return true;
  }

bool magic_rugkey(int sym, int uni) {
  if((cmode & sm::NORMAL) && uni == 'p') {
    rug::texturesize = 4096;
    if(rug::rugged) rug::close();
    else rug::init();
    return true;
    }
  if((cmode & sm::NORMAL) && uni == 'r') {
    mine::performMarkCommand(mouseover);
    return true;
    }
  if((cmode & sm::NORMAL) && uni == 'R') {
    build(crystal::c0, 0);
    }  
  if((cmode & sm::NORMAL) && uni == 'k') {
    crystal::view_coordinates = !crystal::view_coordinates;
    return true;
    }
  return false;
  }

void magic(int sides) {
  stop_game();
  if(sides < 0)
    set_geometry(gCrystal344);
  else
    crystal::set_crystal(sides);
  set_variation(eVariation::pure);
  firstland = specialland = laCanvas;
  patterns::whichCanvas = 'g';
  patterns::canvasback = back;
  check_cgi();
  start_game();
  
  build(crystal::c0, 0);
 
  rv_hook(hooks_drawcell, 100, magic_markers);
  rv_hook(mine::hooks_mark, 150, magic_rotate);
  rv_hook(hooks_handleKey, 150, magic_rugkey);
  }

auto magichook = arg::add2("-magic3", [] { magic(-1); }) + arg::add2("-magic", [] { magic(arg::shift_argi()); });
 
}
#endif
}