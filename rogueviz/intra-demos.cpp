// E3 -> H2xE -> Solv:
// -run -debug-portal -1 -canvas-random 100 -geo beti -intra-add -intra-bxe 4 4 3 -intra-sol 4 4 3 -ray-do  -noplayer

// S2xE->S3:
// -debug-portal -1 -run -canvas-random 0 -intra-120 -intra-add -intra-1440 -intra-start -ray-do

#include "../rogueviz/rogueviz.h"

namespace hr {

#if CAP_PORTALS
namespace intraf {

using namespace intra;

void set_wall(cell *c, color_t col) {
  c->wall = waWaxWall;
  c->landparam = col;
  }

map<cellwalker, int> plane;

cellwalker flatspin(cellwalker cw, int i) {
  if(mhybrid && cw.spin < cw.at->type - 2)
    cw.spin = gmod(cw.spin + (cw.mirrored ? -i : i), cw.at->type - 2);
  return cw;
  }

cellwalker gstrafe(cellwalker cw, int i) {
  if(reg3::in()) return currentmap->strafe(cw, i);
  if(mproduct) {
    if(i == cw.at->type-2 || i == cw.at->type-1)
      return cellwalker(cw.at->move(i), cw.spin);
    else for(int k: {-1, 1})
      if(i == flatspin(cw, k).spin)
        return cellwalker(cw.at->move(i), flatspin(flatspin(cw, k) + wstep, +k).spin);      
    }
  if(euc::in())
    return cellwalker(cw.at->move(i), cw.spin);
  throw hr_exception("unknown gstrafe");
  }

void make_plane(cellwalker cw, int d) {
  if(plane.count(cw)) return;
  plane[cw] = d;
  auto& ss = currentmap->get_cellshape(cw.at);
  for(int i=0; i<cw.at->type; i++)
    if(ss.dirdist[i][cw.spin] == 1)
      make_plane(gstrafe(cw, i), d+1);
  }

void build_wall(cellwalker cw, color_t a, color_t b) {
  println(hlog, "trying to build wall in ", full_geometry_name());
  cw += wstep;
  plane.clear();
  make_plane(cw, 0);
  for(auto p: plane)
    set_wall(p.first.at, p.  second & 1 ? a : b);
  println(hlog, "success, ", isize(plane));
  }

void check_shape() {
  start_game();
  auto sh = currentmap->get_cellshape(cwt.at);
  int j = 0;
  for(auto f: sh.faces) {
    j++;
    for(int i=0; i<isize(f); i++) {
      ld d = hdist(f[i], f[(1+i)%isize(f)]);
      println(hlog, tie(j,i), " : ", fts_fixed(d, 20));
      }
    }
  ld edgelen = 2 * edge_of_triangle_with_angles(M_PI/S7, M_PI / S3, M_PI/2);

  println(hlog, "crossf = ", cgi.crossf);
  println(hlog, "hcrossf = ", cgi.hcrossf);
  println(hlog, "edgelen = ", edgelen);
  println(hlog, sh.faces);  
  }

void create_intra_floors() {
  if(intra::in) intra::become();
  else stop_game();

  set_geometry(gSpace534);
  field_quotient_3d(5, 0x72414D0C);
  start_game();
  build_wall(cwt, 0xFFFF40, 0xD0D000);
  become();

  hybrid::csteps = 6;
  set_geometry(gBring);
  set_variation(eVariation::pure);
  check_cgi();
  cgi.require_basics();
  vid.plevel_factor = cgi.edgelen / cgi.scalefactor;
  check_cgi();
  cgi.require_basics();
  set_geometry(gProduct);
  start_game();
  check_shape();
  
  cwt.spin = 0;
  build_wall(cwt, 0xC0FFC0, 0x80C080);
  become();
  
  auto& T0 = euc::eu_input.user_axes;
  for(int i=0; i<3; i++)
  for(int j=0; j<3; j++)
    T0[i][j] = i==j ? 6 : 0;
  euc::eu_input.twisted = 0;
  euc::build_torus3();
  set_geometry(gCubeTiling);
  start_game();
  build_wall(cwt, 0xC0FFFF, 0x40FFFF);
  become();

  hybrid::csteps = 6;
  set_geometry(gOctahedron);
  check_cgi();
  cgi.require_basics();
  vid.plevel_factor = cgi.edgelen / cgi.scalefactor;
  check_cgi();
  cgi.require_basics();
  set_variation(eVariation::pure);
  set_geometry(gProduct);
  start_game();
  cwt.spin = 0;
  build_wall(cwt, 0x8080FF, 0x0000FF);
  become();

  set_geometry(gCell16);
  start_game();
  build_wall(cwt, 0xFF80FF, 0xFF00FF);
  become();

  intra::start();

  // rogueviz::rv_hook(hooks_handleKey, 150, intra_key);
  }

void create_intra_solv() {
  if(intra::in) intra::become();
  else stop_game();

  arg::shift(); int x = arg::argi();
  arg::shift(); int y = arg::argi();
  
  println(hlog, "x=", x, " y=", y);
  
  vector<vector<cell*> > h(y), s(y);
  
  for(int i=0; i<y; i++) h[i].resize(x), s[i].resize(x);  

  stop_game();

  set_geometry(gBinary3);
  start_game();
  h[0][0] = cwt.at;
  for(int i=1; i<y; i++) h[i][0] = h[i-1][0]->cmove(6);
  for(int i=0; i<y; i++) for(int j=1; j<x; j++)
    h[i][j] = h[i][j-1]->cmove(4);

  for(int i=0; i<y; i++)
  for(int j=0; j<x; j++) {
    setdist(h[i][j], 7, nullptr);
    h[i][j]->wall = waNone;
    forCellCM(c1, h[i][j]) {}
    set_wall(h[i][j]->cmove(0), 0xFF00FF ^ (hrand(0x1000000) & 0x1F1F1F));
    set_wall(h[i][j]->cmove(1), 0xC000FF ^ (hrand(0x1000000) & 0x1F1F1F));
    set_wall(h[i][j]->cmove(2), 0xFF00C0 ^ (hrand(0x1000000) & 0x1F1F1F));
    set_wall(h[i][j]->cmove(3), 0xC000C0 ^ (hrand(0x1000000) & 0x1F1F1F));
    set_wall(h[i][j]->cmove(8), 0xFF40FF ^ (hrand(0x1000000) & 0x1F1F1F));
    }

  become();

  set_geometry(gSol);
  start_game();
  s[0][0] = cwt.at;
  for(int i=1; i<y; i++) s[i][0] = s[i-1][0]->cmove(5);
  for(int i=0; i<y; i++) for(int j=1; j<x;j++)
    s[i][j] = s[i][j-1]->cmove(4);
  
  for(int i=0; i<y; i++)
  for(int j=0; j<x; j++) {
    setdist(s[i][j], 7, nullptr);
    s[i][j]->wall = waNone;
    forCellCM(c1, s[i][j]) {}
    set_wall(s[i][j]->cmove(2), 0xFFFFFF ^ (hrand(0x1000000) & 0x1F1F1F));
    set_wall(s[i][j]->cmove(3), 0xC0FFFF ^ (hrand(0x1000000) & 0x1F1F1F));
    set_wall(s[i][j]->cmove(6), 0xFFC0FF ^ (hrand(0x1000000) & 0x1F1F1F));
    set_wall(s[i][j]->cmove(7), 0xFFFFC0 ^ (hrand(0x1000000) & 0x1F1F1F));
    }
  become();

  intra::start();
  
  println(hlog, "Started");

  for(int i=0; i<y; i++)
    connect_portal(cellwalker(h[i][0], 5), cellwalker(s[i][x-1], 4), 1);
  
  for(int i=0; i<y; i++)
    connect_portal(cellwalker(s[i][0], 0), cellwalker(h[i][x-1], 4), 1);

  for(int j=0; j<x; j++)
    connect_portal(cellwalker(h[0][j], 7), cellwalker(s[y-1][x-1-j], 5), 0);

  for(int j=0; j<x; j++)
    connect_portal(cellwalker(s[0][j], 1), cellwalker(h[y-1][x-1-j], 6), 0);
  
  set_wall(s[hrand(y)][hrand(x)], 0xA02020);
  set_wall(h[hrand(y)][hrand(x)], 0x2020A0);

  // rogueviz::rv_hook(hooks_handleKey, 150, intra_key);
  }

void create_intra_120() {
  patterns::whichCanvas = 'r';
  patterns::rwalls = 0;
  if(intra::in) intra::become();
  else stop_game();
  arcm::current.parse("8,4,6");
  set_geometry(gArchimedean);
//  set_variation(eVariation::pure);
  set_variation(eVariation::dual);
  be_ratio_edge(0);
  for(cell *c: currentmap->allcells()) {
    println(hlog, currentmap->full_shvid(c), " : ", currentmap->get_corner(c, 0), currentmap->get_corner(c, 1), currentmap->get_corner(c, 2));
    }
  hybrid::csteps = 10;
  set_geometry(gProduct);
  start_game();
  vector<cell*> bound;
  PIU( [&] {
    cell *s = currentmap->gamestart();
    hyperpoint h = currentmap->get_corner(s, 1);
    for(cell *c: currentmap->allcells()) {
      hyperpoint j = currentmap->relative_matrix(c, s, C0) * C0;
      if(hdist(h, j) > 90._deg) c->wall = waPalace;
      }
    for(cell *c: currentmap->allcells()) if(c->wall == waPalace) {
      int nei = 0;
      forCellCM(c1, c) if(c1->wall != waPalace) nei++;
      if(nei == 1) bound.push_back(c);
      }
    auto ang = [&] (cell *c) {
      hyperpoint j = currentmap->relative_matrix(c, s, C0) * C0;
      j = gpushxto0(h) * j;
      return atan2(j[0], j[1]);
      };
    for(cell *c: bound) println(hlog, "j= ", ang(c));
    sort(bound.begin(), bound.end(), [&] (cell *a, cell *b) { return ang(a) < ang(b); });
    int id = 0;
    for(auto b: bound) set_wall(b, ((id++)&1) ? 0xFFFFFF : 0x202020);
    } ());
  for(int i=0; i<isize(bound); i++)
  for(int j=0; j<hybrid::csteps-2; j++)
    set_wall(hybrid::get_at(bound[i], j), (i+j)&1 ? 0x8080FF : 0x00000FF);
  }

void create_intra_1440() {
  patterns::whichCanvas = 'r';
  patterns::rwalls = 0;
  if(intra::in) intra::become();
  else stop_game();
  set_geometry(gCell8);
//  set_variation(eVariation::pure);
  set_variation(eVariation::coxeter);
  reg3::coxeter_param = 7;
  start_game();

  cell *s = currentmap->gamestart();
  auto vs = currentmap->get_cellshape(s).vertices_only_local;
  println(hlog, vs);
  hyperpoint h = vs[1];
  for(cell *c: currentmap->allcells()) {
    hyperpoint j = currentmap->relative_matrix(c, s, C0) * C0;
    // hyperpoint j = inverse(currentmap->relative_matrix(cwt.at, c, C0)) * C0;
    if(hdist(h, j) > 90._deg)
      set_wall(c, (celldistance(c, s)&1) ? 0xFF80FF : 0xFF00FF);
    }
  }

void recurse(int r, cell *c, int i, int j) {
  setdist(c, 7, nullptr);
  forCellCM(c1, c) setdist(c1, 7, nullptr);
  c->wall = waNone;
  if(r == 1) {
    if(sol) {
      int flip = (j&2)>>1;
      set_wall(c->cmove(6^flip), 0xC000C0);
      set_wall(c->cmove(7^flip), 0x800080);
      }
    else {
      set_wall(c->cmove((j&1)), 0x64BF95);
      set_wall(c->cmove(1-(j&1)), 0x449F75);
      }
    }
  else {
    if(sol) {
      recurse(r-1, c->cmove(6), 2*i, j>>1);
      recurse(r-1, c->cmove(7), 2*i+1, j>>1);
      }
    else {
      recurse(r-1, c->cmove(0), i, j);
      recurse(r-1, c->cmove(1), i, j);
      }
    }
  }

void connect_portal_x(cellwalker cw1, cellwalker cw2, int spin) {
  setdist(cw1.cpeek(), 7, nullptr);
  setdist(cw2.cpeek(), 7, nullptr);
  set_wall(cw1.cpeek(), 0xFF0000);
  set_wall(cw2.cpeek(), 0xFF0000);
  connect_portal(cw1, cw2, spin);
  }

void recurse_portal(int r, cell *cl, cell *cr) {
  connect_portal_x(cellwalker(cl, 6), cellwalker(cr, 5), 3);
  if(r > 1) {
    recurse_portal(r-1, cl->cmove(0), cr->cmove(0));
    recurse_portal(r-1, cl->cmove(1), cr->cmove(1));
    }
  }

vector<reaction_t> portals;

void create_intra_bxe() {
  println(hlog, "called create_intra_bxe");
  patterns::whichCanvas = 'r';
  patterns::rwalls = 100;
  if(intra::in) intra::become();
  else stop_game();
  hybrid::csteps = 0;

  arg::shift(); int x = arg::argi();
  arg::shift(); int y = arg::argi();
  arg::shift(); int z = arg::argi();
  
  vector<vector<cell*> > h(y);
  
  for(int i=0; i<y; i++) h[i].resize(x);

  stop_game();

  set_geometry(gBinary4);
  vid.plevel_factor = log(2);
  set_geometry(gProduct);
  start_game();
  h[0][0] = cwt.at;
  
  for(int i=1; i<y; i++) h[i][0] = h[i-1][0]->cmove(2);
  for(int i=0; i<y; i++) for(int j=1; j<x; j++)
    h[i][j] = h[i][j-1]->cmove(5);
  
  println(hlog, h);

  for(int i=0; i<y; i++)
  for(int j=0; j<x; j++) {
    setdist(h[i][j], 7, nullptr);
    }

  for(int i=0; i<y; i++)
  for(int j=0; j<x; j++) {
    set_wall(h[i][j]->cmove(3), ((j+(i>>1))&1) ? 0xA4FFD5 : 0x64BF95);
    recurse(z, h[i][j], i, j);
    }

  become();
  start(isize(intra::data)-1);
  
  for(int i=0; i<y; i++)
    recurse_portal(z, h[i][0], h[i][x-1]);

  for(int i=0; i<x; i++) {
    cell *cl = h[0][i];
    cell *cr = h[y-1][i];
    for(int j=0; j<z; j++) {
      connect_portal_x(cellwalker(cl, 4), cellwalker(cr, 2), 2);
      cl = cl->move(0);
      cr = cr->move(1);
      }
    }
    
  }
  
void recurse_portal_solv1(int r, cell *cl, cell *cr) {
  connect_portal_x(cellwalker(cl, 0), cellwalker(cr, 4), 0);
  if(r > 1) {
    recurse_portal_solv1(r-1, cl->cmove(6), cr->cmove(6));
    recurse_portal_solv1(r-1, cl->cmove(7), cr->cmove(7));
    }
  }

void recurse_portal_solv2(int r, cell *cl, cell *cr) {
  connect_portal_x(cellwalker(cl, 1), cellwalker(cr, 5), 0);
  if(r > 1) {
    recurse_portal_solv2(r-1, cl->cmove(7), cr->cmove(6));
    }
  }

void create_intra_sol() {
  println(hlog, "called create_intra_sol");
  patterns::whichCanvas = 'r';
  patterns::rwalls = 100;
  if(intra::in) intra::become();
  else stop_game();

  arg::shift(); int x = arg::argi();
  arg::shift(); int y = arg::argi();
  arg::shift(); int z = arg::argi();
  
  vector<vector<cell*> > h(y);
  
  for(int i=0; i<y; i++) h[i].resize(x);

  stop_game();

  set_geometry(gSol);
  start_game();
  h[0][0] = cwt.at;

  for(int i=0; i<x-1; i++) h[0][0] = h[0][0]->cmove(0);
  for(int i=0; i<y-1; i++) h[0][0] = h[0][0]->cmove(1);
  
  for(int i=1; i<y; i++) h[i][0] = h[i-1][0]->cmove(5);
  for(int i=0; i<y; i++) for(int j=1; j<x; j++)
    h[i][j] = h[i][j-1]->cmove(4);

  for(int i=0; i<y; i++)
  for(int j=0; j<x; j++) {
    setdist(h[i][j], 7, nullptr);
    }

  for(int i=0; i<y; i++)
  for(int j=0; j<x; j++) {
    set_wall(h[i][j]->cmove(2^((i>>1)&1)), 0xFFFDD0);
    set_wall(h[i][j]->cmove(3^((i>>1)&1)), 0xFFE080);
    recurse(z, h[i][j], i, j);
    }

  become();
  start(isize(intra::data)-1);
  
  for(int i=0; i<y; i++)
    recurse_portal_solv1(z, h[i][0], h[i][x-1]);

  for(int i=0; i<x; i++)
    recurse_portal_solv2(z, h[0][i], h[y-1][i]);

  println(hlog, "finished create_intra_sol");
  }

map<int, cell*> starter;

bool vr_keys(int sym, int uni) {
  if(intra::in && !starter.count(intra::current)) starter[intra::current] = cwt.at;
  const Uint8 *keystate = SDL12_GetKeyState(NULL);
  #if CAP_SDL2
  if(keystate[SDL_SCANCODE_LALT] || keystate[SDL_SCANCODE_RALT]) 
  #else
  if(keystate[SDLK_LALT] || keystate[SDLK_RALT])
  #endif
    {
    if(sym == 'e' && intra::in) {
      println(hlog, "intra::current = ", intra::current);
      intra::switch_to(2);
      if(starter.count(intra::current)) cwt.at = centerover = starter[intra::current];
      fullcenter();
      View = cspin90(0, 1);
      playermoved = false;
      walking::handle();
      return true;
      }
    if(sym == 'h' && intra::in) {
      intra::switch_to(0);
      if(starter.count(intra::current)) cwt.at = centerover = starter[intra::current];
      fullcenter();
      View = cspin90(0, 1);
      playermoved = false;
      walking::handle();
      return true;
      }
    if(sym == 's' && intra::in) {
      intra::switch_to(6);
      if(starter.count(intra::current)) cwt.at = centerover = starter[intra::current];
      fullcenter();
      View = cspin90(0, 1);
      playermoved = false;
      walking::handle();
      return true;
      }
    if(sym == ',') {
      camera_speed *= 1.2;
      println(hlog, "camera_speed set to ", camera_speed);
      return true;
      }
    if(sym == '.') {
      camera_speed /= 1.2;
      println(hlog, "camera_speed set to ", camera_speed);
      return true;
      }
    #if CAP_VR
    if(sym == 'a') {
      vrhr::absolute_unit_in_meters *= 1.2;
      walking::eye_level *= 1.2;
      println(hlog, "vr absolute unit set to ", vrhr::absolute_unit_in_meters);
      return true;
      }
    if(sym == 'z') {
      vrhr::absolute_unit_in_meters /= 1.2;
      walking::eye_level /= 1.2;
      println(hlog, "vr absolute unit set to ", vrhr::absolute_unit_in_meters);
      return true;
      }
    #endif
    if(sym == 'w') {
      walking::switch_walking();
      println(hlog, "walking set to ", ONOFF(walking::on));
      return true;
      }
    #if CAP_VR
    if(sym == 'x') {
      vrhr::always_show_hud = false;
      return true;
      }
    #endif
    }
  return false;
  }

// all generators will add to the current scene

auto hooks = 
  // generate scene with H3, H2xE, E3, S2xE (8x6), S3 (16-cell) with floors; runs automatically
  arg::add3("-intra-floors", create_intra_floors)
+ arg::add3("-check-shape", check_shape)
  // generate scene with H3 and Solv landscape, need X Y parameters, runs automatically
+ arg::add3("-intra-solv", create_intra_solv)
  // generate S2xE with triangles (-intra-start to run it)
+ arg::add3("-intra-120", create_intra_120)
  // generate S3 with triangles (-intra-start to run it)
+ arg::add3("-intra-1440", create_intra_1440)
  // generate binary-tiling H2xE with floors to the current scene, runs automatically
+ arg::add3("-intra-bxe", create_intra_bxe)
  // generate Sol with floors to the current scene, runs autimatically
+ arg::add3("-intra-sol", create_intra_sol)
//+ arg::add3("-intra-more", create_intra_more);
+ arg::add3("-intra-demo-floors", [] {
  walking::colors_of_floors = {
    0xFFFF40, 0xD0D000,
    0xC0FFC0, 0x80C080,
    0xC0FFFF, 0x40FFFF,
    0x8080FF, 0x0000FF,
    0xFF80FF, 0xFF00FF,
    0x64BF95, 0xA4FFD5,
    0xFFFDD0, 0xFFD080
    };
  })
+ addHook_rvslides(10, ([] (string s, vector<tour::slide>& v) {
    println(hlog, "called with s='", s, "'");
    if(s != "portal") return;

    using namespace tour;
    auto load = [] (string s, ld x, int y) {
      return [s, x, y] {
        slide_backup(vid.cells_drawn_limit, 100);
        slide_backup(ray::max_cells, 999999);
        slide_backup(walking::on, true);
        slide_backup(walking::eye_level, x);
        mapstream::loadMap(s);
        slide_backup(ray::fixed_map, true);
        slide_backup(ray::max_iter_intra, y);
        #if CAP_VR
        slide_backup(vrhr::hsm, vrhr::eHeadset::holonomy);
        slide_backup(vrhr::eyes, vrhr::eEyes::truesim);
        slide_backup(vrhr::cscr, vrhr::eCompScreen::eyes);
        #endif
        starter.clear();
        rogueviz::rv_hook(hooks_handleKey, 101, vr_keys);
        popScreenAll();
        resetGL();
        };
      };

    struct loader {
      string desc;
      char key;
      reaction_t loader;
      };

    auto add = [&] (string s, string desc, string youtube, string twitter, vector<loader> loaders) {
      v.push_back(tour::slide{
        s, 10, tour::LEGAL::NONE | tour::QUICKSKIP | tour::QUICKGEO | tour::ALWAYS_TEXT, desc,
        [=] (tour::presmode mode) {
          setCanvas(mode, '0');
          if(youtube != "")
            slide_url(mode, 'y', "YouTube link", youtube);
          if(twitter != "")
            slide_url(mode, 't', "Twitter link", twitter);

          slide_url(mode, 'b', "Bridges paper link", "https://archive.bridgesmathart.org/2022/bridges2022-297.html");

          for(auto& loader: loaders)
            slide_action(mode, loader.key, loader.desc, loader.loader);
          slidecommand = "portal options";
          if(mode == tour::pmKey) pushScreen(intra::show_portals);
          rogueviz::pres::non_game_slide_scroll(mode);
          }
        });
      };

    add("inter-geometric portals",
      "In this world we can find portals between six different geometries. The camera is in 'walking mode' i.e. restricted to keep close to the floor (this can be disabled with '5').\n\n"
      "In VR, you might want to prefer the VR version, which adds extra blocks to Solv to improve the performance.\n\n"
      "Note: this is a very complex shader, so compiling it may take long time!\n\n"
      "Hotkeys, mostly useful when showing VR to someone:\n\n"
      "Alt+EHS -- go to Euclidean/hyperbolic/spherical space, Alt+,. -- change the camera speed, Alt+AZ -- change altitude, Alt+W -- switch walking, Alt+X -- change HUD"
      ,
      "https://youtu.be/yqUv2JO2BCs", "https://twitter.com/ZenoRogue/status/1496867204419452935",
      {loader{"run this visualization", 'r', load("portalscene3.lev", 0.2174492, 600)}, 
       loader{"run this visualization (VR)", 'v', load("portalscene3-solv.lev", 0.2174492, 600)}}
      );
    add("curved landscape",
      "Here we create portals between Solv and H3 geometries, resulting in a scene looking a bit like a curved landscape.",
      "", "https://twitter.com/ZenoRogue/status/1446127100516130826",
      {loader{"run this visualization", 'r', load("solv-h3-scene.lev", 0.05, 3000)}});
    }));
}
#endif
}
