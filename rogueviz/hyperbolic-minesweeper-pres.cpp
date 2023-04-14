#include "rogueviz.h"

#define RVPATH HYPERPATH "rogueviz/"

#include "dynamic-wfc.cpp"

namespace hr {
#if CAP_RVSLIDES
cell* starter;

void geom_euc_rec() {
  set_geometry(gEuclidSquare);
  set_variation(eVariation::pure);
  tour::slide_backup(vid.use_smart_range, 2);
  }

void geom_sphere() {
  set_geometry(gOctahedron);
  set_variation(eVariation::unrectified);
  gp::param = {5, 5};
  tour::slide_backup(vid.use_smart_range, 2);
  }

void geom_tes() {
  tour::slide_backup(vid.creature_scale, 0.5);
  arb::run("tessellations/rogueviz/hybrid34.tes");
  tour::slide_backup(vid.use_smart_range, 2);
  tour::slide_backup(vid.smart_range_detail, 1);
  }

void geom_pentagos() {
  set_geometry(g45);
  set_variation(eVariation::pure);
  tour::slide_backup(vid.use_smart_range, 2);
  }

void geom_h3() {
  set_geometry(gBinary3);
  }

void geom_binary() {
  set_geometry(gBinary4);
  }

void geom_klein_quartic() {
  set_geometry(gKleinQuartic);
  set_variation(eVariation::bitruncated);
  }

void geom_hr() {
  set_geometry(gNormal);
  set_variation(eVariation::bitruncated);
  }

void geom_high_hyper() {
  set_geometry(g45);
  set_variation(eVariation::unrectified);
  gp::param = {5, 5};
  tour::slide_backup(vid.use_smart_range, 2);
  tour::slide_backup(vid.smart_range_detail, 1);
  }

void sd(cell *c) { setdist(c, 7, nullptr); }

vector<cell*> cl_sphere() {
  cellwalker cs (currentmap->gamestart());
  cs += 2;
  cs += wstep;
  cs += 2;
  cs += wstep;
  cs ++;
  cs += wstep;
  cs += 2;
  cs += wstep;
  starter = cs.at;
  vector<cell*> lst;
  auto all = currentmap->allcells();
  for(cell *c: all) {
    hyperpoint h = spin(45._deg) * currentmap->relative_matrix(c, cs.at, C0) * C0;
    if(-h[2] < max(abs(h[0]), abs(h[1]))) {
      lst.push_back(c);
      }
    else {
      sd(c);
      c->landparam = 0xC00000;
      }
    }
  return lst;
  }

void add_square(vector<cell*>& lst, set<cell*>& seen, cellwalker cw0) {
  for(int y=-2; y<=2; y++)
  for(int x=-2; x<=2; x++) {
    cellwalker cw = cw0;
    for(int i=0; i<y; i++) cw += wstep, cw+= 2, sd(cw.at);
    cw++;
    for(int i=0; i<x; i++) cw += wstep, cw+= 2, sd(cw.at);
    cw++;
    for(int i=0; i<-y; i++) cw += wstep, cw+= 2, sd(cw.at);
    cw++;
    for(int i=0; i<-x; i++) cw += wstep, cw+= 2, sd(cw.at);
    cw++;
    if(!seen.count(cw.at)) {
      seen.insert(cw.at);
      lst.push_back(cw.at);
      }
    }
  }

void add_square_recurse(vector<cell*>& lst, set<cell*>& seen, cellwalker cw0, int levels) {
  add_square(lst, seen, cw0);
  if(!levels) return;
  for(int i=0; i<4; i++) {
    if(i == 2&& levels < 3) continue;
    cellwalker cw = cw0;
    cw += i;
    for(int i=0; i<5; i++) cw += wstep, cw += 2, sd(cw.at);
    add_square(lst, seen, cw);
    for(int i=0; i<5; i++) cw += wstep, cw += 2, sd(cw.at);
    add_square_recurse(lst, seen, cw, levels-1);
    }
  }

vector<cell*> cl_high_hyper() {
  cellwalker cs (currentmap->gamestart());
  cs += 2;
  cs += wstep;
  cs += 2;
  cs += wstep;
  cs ++;
  cs += wstep;
  cs += 2;
  cs += wstep;
  starter = cs.at;
  set<cell*> seen;
  vector<cell*> lst;
  add_square_recurse(lst, seen, starter, 3);
  return lst;
  }

vector<cell*> cl_pentagons() {
  celllister cs(cwt.at, 7, 999999, nullptr);
  starter = cwt.at;
  return cs.lst;
  }

vector<cell*> cl_r10() {
  celllister cs(cwt.at, 10, 999999, nullptr);
  starter = cwt.at;
  return cs.lst;
  }

vector<cell*> cl_all() {
  starter = cwt.at;
  return currentmap->allcells();
  }

vector<cell*> cl_rectangle() {
  tour::slide_backup(pconf.scale, 0.25);
  cell *cs = currentmap->gamestart();
  starter = cs;
  vector<cell*> lst;
  for(int x=-10; x<=10; x++)
  for(int y=-6; y<=6; y++) {
    cell *c = cs;
    for(int i=0; i<x; i++) sd(c), c = c->cmove(2);
    for(int i=0; i<y; i++) sd(c), c = c->cmove(1);
    for(int i=0; i<-x; i++) sd(c), c = c->cmove(0);
    for(int i=0; i<-y; i++) sd(c), c = c->cmove(3);
    sd(c);
    lst.push_back(c);
    }
  return lst;
  }

vector<cell*> cl_h3_rectangle() {
  tour::slide_backup(pconf.scale, 0.25);
  cell *cs = currentmap->gamestart();
  starter = cs;
  vector<cell*> lst;
  for(int x=-8; x<=8; x++)
  for(int y=-8; y<=8; y++) {
    cell *c = cs;
    for(int i=0; i<x; i++) sd(c), c = c->cmove(4);
    for(int i=0; i<y; i++) sd(c), c = c->cmove(6);
    for(int i=0; i<-x; i++) sd(c), c = c->cmove(5);
    for(int i=0; i<-y; i++) sd(c), c = c->cmove(7);
    sd(c);
    lst.push_back(c);
    }
  return lst;
  }

vector<cell*> cl_narrow_rectangle() {
  tour::slide_backup(pconf.scale, 0.25);
  cell *cs = currentmap->gamestart();
  starter = cs;
  vector<cell*> lst;
  for(int x=-10; x<10; x++)
  for(int y=-2; y<=2; y++) {
    cell *c = cs;
    for(int i=0; i<x; i++) sd(c), c = c->cmove(2);
    for(int i=0; i<y; i++) sd(c), c = c->cmove(1);
    for(int i=0; i<-x; i++) sd(c), c = c->cmove(0);
    for(int i=0; i<-y; i++) sd(c), c = c->cmove(3);
    sd(c);
    lst.push_back(c);
    }
  return lst;
  }

void assigner(cell *c) {
  int i = hrand(100);
  if(i < 20)
    c->wall = waMineMine;
  else if(i < 30)
    c->wall = waMineOpen;
  else
    c->wall = waMineUnknown;
  }

void assigner_tons(cell *c) {
  int i = hrand(100);
  if(i < 45)
    c->wall = waMineMine;
  else if(i < 55)
    c->wall = waMineOpen;
  else
    c->wall = waMineUnknown;
  }

void assigner_fullmine(cell *c) {
  c->wall = waMineMine;
  }

void chessboard_assigner(cell *c) {
  c->land = laCanvas;
  c->wall = waNone;
  if(chessvalue(c))
    c->landparam = 0x20C020;
  else
    c->landparam = 0x208020;  
  }

vector<cell*> current_list;

void mine_slide(tour::presmode mode, reaction_t set_geom, function<vector<cell*>()> celllister, function<void(cell*)> assigner) {
  using namespace tour;
  patterns::canvasback = 0;
  setCanvas(mode, '0');
  if(mode == pmStart) {
    slide_backup(mapeditor::drawplayer, false);
    slide_backup(no_find_player, true);
    slide_backup(dont_display_minecount, true);
    slide_backup(numerical_minefield, true);
    slide_backup(disable_orb_range, true);
    slide_backup(nohud, true);
    slide_backup(cheater, true);
    slide_backup(vid.sspeed, -5);
    slide_backup(mine_adjacency_rule, 1);
    slide_backup(vid.particles, false);
    stop_game();
    set_geom();    
    start_game();
    current_list = celllister();
    cwt.at = centerover = starter;
    View = Id;
    if(hyperbolic)
      View = spin(45._deg) * View;
    if(sphere) {
      View = spin(45._deg) * View;
      slide_backup(pconf.scale, 1000);
      slide_backup(pconf.alpha, 1000);
      slide_backup(modelcolor, 0xFF);
      slide_backup(backbrightness, 0.1);
      }
    for(auto c: current_list) {
      c->land = laMinefield;
      c->landparam = 0;
      assigner(c);
      }
    }
  items[itOrbTeleport] = 1;
  clearMessages();
  if(mode == pmFrame)
  for(auto c: current_list) {
    int mines = 0;
    for(auto c1: adj_minefield_cells(c))
      if(c1->wall == waMineMine)
        mines++;
    if(mines == 0)
      for(auto c1: adj_minefield_cells(c))
        if(c1->wall == waMineUnknown)
          c1->wall = waMineOpen;
    }
  }  

using namespace rogueviz::pres;

void wfc_slide(presmode mode, int type, int rad, int cutoff) {
  static vector<pair<cell*, int>> colors;
  setCanvas(mode, '0');
  dynamic_wfc::wfctype = type;
  dynamic_wfc::wfcrad = rad;
  dynamic_wfc::cutoff = cutoff;
  dynamic_wfc::animated = true;  
  history::progress_each = 40; 
  add_temporary_hook(mode, hooks_prestats, 300, [] { return true; });
  if(mode == pmStart) slide_backup(no_find_player, true);
  if(mode == pmStart) tour::slide_backup(mapeditor::drawplayer, false);
  if(mode == pmStart) slide_backup(vid.particles, false);
  if(mode == pmStart) slide_backup(disable_orb_range, true);
  if(mode == pmStart) dynamic_wfc::wfc_clear();
  slidecommand = "regenerate WFC";
  if(mode == pmStart) {
    dynamic_wfc::wfc();
    colors.clear();
    for(cell* c: currentmap->allcells()) colors.emplace_back(c, c->landparam);
    }
  if(mode == pmKey)
    dynamic_wfc::wfc_build();
  static bool marked = false;
  if(mode == pmStart) marked = false;
  add_temporary_hook(mode, mine::hooks_mark, 100, [] (cell *c) {
    for(auto p: colors) p.first->landparam = p.second;
    marked = !marked;
    if(marked) forCellEx(c1, c) c1->landparam = gradient(c1->landparam, 0xFFFFFF, 0, 0.5, 1);
    return true;
    });
  }

void enable_earth() {
  texture::texture_aura = true;
  stop_game();
  set_geometry(gSphere);
  enable_canvas();
  patterns::whichCanvas = 'F';
  start_game();        
  texture::config.configname = "textures/earth.txc";
  texture::config.load();
  pmodel = mdDisk;
  pconf.alpha = 1000; pconf.scale = 999;
  texture::config.color_alpha = 255;
  mapeditor::drawplayer = false;
  fullcenter();
  View = spin(234._deg) * View;
  }

slide sweeper_slides[] = {
  {"Title Page", 123, LEGAL::ANY | QUICKSKIP | QUICKGEO | NOTITLE, "", 
    [] (presmode mode) {
      mine_slide(mode, geom_hr, cl_all, assigner);
      white_screen(mode);
      if(mode == pmStart) slide_backup(nohud, false);
      if(mode == pmStart) slide_backup(pconf.scale, .65);
      if(mode == pmStart) slide_backup(vid.grid, false);
      if(mode == pmStart) slide_backup(bright, true);
      // if(mode == pmStart) slide_backup(winf[waMineUnknown].color, 0x404040);
      // if(mode == pmStart) slide_backup(winf[waMineOpen].color, 0xC0C0C0);
      // empty_screen(mode);
      add_stat(mode, [] {
        tour::slide_backup(nohelp, true);
        cmode |= sm::DARKEN;
        gamescreen();
        dialog::init();
        tour::slide_backup(no_find_player, true);
        dialog::addTitle("Hyperbolic Minesweeper is in P", dialog::dialogcolor, 150);
        dialog::addBreak(1600);
        dialog::addTitle(" ", dialog::dialogcolor, 150);
        dialog::addItem("paper on arXiv",  '5');
        dialog::display();
        return true;
        });
      no_other_hud(mode);
      if(mode == pmKey) open_url("https://arxiv.org/abs/2002.09534");
      }
    },

  {"Minesweeper", 999, LEGAL::NONE | QUICKGEO, 
    "The standard Euclidean minesweeper. You probably know this game. Every number tells you the number of adjacent mines. Click a number to reveal the number (or lose if it contained a mine).\n\n"
    "This presentation is actually about the puzzle version of Minesweeper. You do not click anywhere. You are given some numbers, and you need to determine the arrangement of mines consistent with these numbers.",
    [] (presmode mode) {
      mine_slide(mode, geom_euc_rec, cl_rectangle, assigner);
      }
    },

  {"Minesweeper is NP-complete", 999, LEGAL::NONE | QUICKGEO, 
    "The standard Euclidean minesweeper is NP-complete (as a decision problem: is the board consistent?). But what about non-standard ones?\n\n",
    [] (presmode mode) {
      empty_screen(mode);
      add_stat(mode, [] {
        cmode |= sm::DARKEN;
        gamescreen();
        dialog::init();
        tour::slide_backup(nohelp, true);
        dialog::addTitle("Minesweeper is NP-complete", 0xC00000, 200);
        dialog::addInfo("Richard Kaye, 2000");
        dialog::addBreak(400);
        dialog::addInfo("proof: reduction from SAT");
        dialog::addBreak(100);
        dialog::addHelp("build gadgets on the Minesweeper board which resemble a given logical circuit");
        dialog::display();
        return true;
        });
      no_other_hud(mode);
      }
    },

  {"Variants: Warped Mines", 123, LEGAL::ANY | QUICKSKIP, 
    "This talk is about hyperbolic Minesweeper. There are actually at least two implementations available of hyperbolic Minesweeper. One of them is Warped Mines, available online.", 
    [] (presmode mode) {
      empty_screen(mode);
      // show_picture(mode, "rogueviz/mine/warped-mines.png");
      no_other_hud(mode);
      slide_url(mode, 'w', "link to Warped Mines", "https://cdn.warpedmines.com/");
      }
    },

  {"Variants: HyperRogue minefield", 123, LEGAL::ANY | QUICKGEO, 
    "The other one is the Minefield land in HyperRogue. It is heavily modified from the original but the basic idea stays the same.", 
    [] (presmode mode) {
      setCanvas(mode, '0');
      if(mode == pmStart) {
        stop_game();
        firstland = specialland = laMinefield;
        start_game();
        items[itBombEgg] = 20;
        activateSafety(laMinefield);
        safetyat = 0;
        tour::slide_backup(numerical_minefield, true);
        }
      }
    },

  {"what is hyperbolic geometry?", 999, LEGAL::NONE | QUICKGEO | NOTITLE, 
    "",
    [] (presmode mode) {
      empty_screen(mode);
      add_stat(mode, [] {
        cmode |= sm::DARKEN;
        gamescreen();
        dialog::init();
        tour::slide_backup(nohelp, true);
        dialog::addTitle("what does", dialog::dialogcolor, 200);
        dialog::addTitle("hyperbolic", dialog::dialogcolor ^ 0xC00000, 300);
        dialog::addTitle("mean here?", dialog::dialogcolor, 200);
        dialog::display();
        return true;
        });
      no_other_hud(mode);
      }
    },

  {"Do the rectangles exist?", 999, LEGAL::NONE | QUICKGEO, 
    "Minesweeper is usually played on a rectangular board.\n"
    "In fact, Kaye's proof even relies on that.\n"
    "But, do the rectangles really exist?\n\n"
    "It is actually not clear!\n\n"
    "Euclid's Elements was the first \"modern\" exposition of mathematics, "
    "where more complex geometric facts (such as the Pythagorean Theorem) "
    "follow from simpler ones, which eventually follow from his postulates."
    ,
    [] (presmode mode) {
      mine_slide(mode, geom_euc_rec, cl_rectangle, assigner);
      tour::slide_backup(no_find_player, true);
      add_stat(mode, [] {
        gamescreen();
        dialog::init();
        dialog::addTitle("uh", 100, 0);
        dialog::addBreak(2000);
        dialog_may_latex("a {\\bf rectangle} has four {\\bf right angles} and four {\\bf straight edges}", "four right angles, four straight edges");
        dialog::addBreak(-200);
        dialog::display();
        return true;
        });
      non_game_slide_scroll(mode);
      if(mode == pmStart) slide_backup(nohud, false);
      }
    },

  {"Euclid's Postulates", 999, LEGAL::NONE | QUICKGEO | NOTITLE, 
    "The list of Euclid's postulates.\n\n",
    [] (presmode mode) {
      empty_screen(mode);
      add_stat(mode, [] {
        cmode |= sm::DARKEN;
        gamescreen();
        dialog::init();
        tour::slide_backup(nohelp, true);
        dialog::addTitle("Euclid's Postulates", 0xC00000, 200);
        dialog::addBreak(200);
        dialog::addTitle("Postulates 1-4:", 0xC000C0, 150);
        dialog::addTitle("(very simple statements)", dialog::dialogcolor, 100);
        dialog::addBreak(200);
        dialog::addTitle("Fifth Postulate:", 0xC000C0, 150);
        dialog::addTitle("(aka Parallel Postulate)", dialog::dialogcolor, 100);
        dialog::addTitle("Minesweeper edition:", dialog::dialogcolor, 80);
        dialog::addTitle("rectangles exist", dialog::dialogcolor, 100);
        dialog::display();
        return true;
        });
      no_other_hud(mode);
      }
    },

  {"meridians on a sphere", 999, LEGAL::NONE | QUICKGEO, 
    "The simplest non-Euclidean geometry is the geometry on the sphere.\n\n"
    "Here we see a spherical triangle with three right angles.\n\n"
    "For creatures restricted to just this surface, they are indeed striaght lines!\n\n"
    ,
    [] (presmode mode) {
      setCanvas(mode, '0');
      if(mode == pmStart) {
        tour::slide_backup(mapeditor::drawplayer, false);
        enable_earth();
      
        View = Id;
        View = spin(108._deg) * View;
        View = spin(90._deg) * View;
        View = cspin(2, 0, 45._deg) * View;
        View = cspin(1, 2, 30._deg) * View;
        playermoved = false;
        tour::slide_backup(vid.axes, 0);
        tour::slide_backup(vid.drawmousecircle, false);
        tour::slide_backup(draw_centerover, false);
        }
      if(mode == pmStop) {
        texture::config.tstate = texture::tsOff;
        }
      /*using linepatterns::patMeridians;
      tour::slide_backup(patMeridians.color, 0xFFFFFFFF);
      tour::slide_backup(patMeridians.multiplier, 5); */
      non_game_slide_scroll(mode);
      }
    },

  {"hyperbolic geometry", 999, LEGAL::NONE | QUICKGEO, 
    "The hyperbolic geometry is the opposite. We can have a pentagon with five right angles.\n\n"
    ,
    [] (presmode mode) {
      setCanvas(mode, 'c');
      if(mode == pmStart) {
        stop_game();
        set_geometry(g45);
        set_variation(eVariation::pure);
        tour::slide_backup(colortables['c'][0], 0x104010);
        tour::slide_backup(colortables['c'][1], 0x10F010);
        tour::slide_backup(vid.use_smart_range, 2);
        tour::slide_backup(vid.smart_range_detail, 1);
        start_game();
        }        
      // mine_slide(mode, geom_pentagos, cl_pentagons, chessboard_assigner);
      non_game_slide_scroll(mode);
      tour::slide_backup(draw_centerover, false);
      }
    },

  {"hyperbolic geometry is tree-like", 999, LEGAL::NONE | QUICKGEO, 
    "The hyperbolic plane has a tree-like structure. It is best seen in the 'binary tiling'. Press '5' to show the pattern.\n\n"
    ,
    [] (presmode mode) {
      setCanvas(mode, 'g');
      non_game_slide_scroll(mode);
      if(mode == pmStart) {
        tour::slide_backup(patterns::canvasback, 0x10A010);
        stop_game();
        set_geometry(gBinary4);
        set_variation(eVariation::pure);
        start_game();
        tour::slide_backup(vid.use_smart_range, 2);
        tour::slide_backup(vid.smart_range_detail, 1);
        View = spin90();
        using linepatterns::patTree;
        tour::slide_backup(patTree.color, 0);
        }        
      if(mode == pmFrame) clearMessages();
      slidecommand = "show the tree";
      if(mode == pmKey) {
        using linepatterns::patTree;
        println(hlog, "key called");
        patTree.color ^= 0xFFFFFFFF;
        tour::slide_backup(patTree.multiplier, 5);
        }
      no_other_hud(mode);
      }
    },

  {"hyperbolic geometry is tree-like... another way", 999, LEGAL::NONE | QUICKGEO, 
    "Other hyperbolic tessellations have a similar structure, just a bit more complicated...\n\n"
    ,
    [] (presmode mode) {
      setCanvas(mode, '0');
      non_game_slide_scroll(mode);
      if(mode == pmStart) {
        stop_game();
        set_variation(eVariation::pure);
        tour::slide_backup(vid.creature_scale, 0.5);
        start_game();
        tour::slide_backup(viewdists, true);
        using linepatterns::patTree;
        tour::slide_backup(patTree.color, 0xFFFFFFFF);
        tour::slide_backup(patTree.multiplier, 5);
        tour::slide_backup(mapeditor::drawplayer, false);
        tour::slide_backup(number_coding, ncType);
        }        
      // mine_slide(mode, geom_pentagos, cl_pentagons, chessboard_assigner);
      if(mode == pmFrame) clearMessages();
      // no_other_hud(mode);
      }
    },

  {"hyperbolic tessellation by Marek Čtrnáct", 999, LEGAL::NONE | QUICKGEO, 
    "So we have lots of tessellations to come from. Our result about hyperbolic Minesweeper will actually work for any sufficiently regular hyperbolic tessellation!\n\n"
    ,
    [] (presmode mode) {
      mine_slide(mode, geom_tes, cl_r10, assigner);
      }
    },

  {"Minesweeper on a narrow board", 999, LEGAL::NONE | QUICKGEO, 
    "When Minesweeper is played on a narrow board (of width w), it is actually easy to solve more efficiently than what the NP-completeness suggests. "
    "The trick is to use Dynamic Programming, and consider a strip go from left to right. The states of DP corresponds to the positions of the strip, together "
    "with the possible assignments of mines to the tiles in the strip (so the number of states is exponential only in w). For each state, we record whether "
    "such an arrangement is possible -- the strip, and the part of the board to the left from it, can be filled this way."
    ,
    [] (presmode mode) {
      mine_slide(mode, geom_euc_rec, cl_narrow_rectangle, assigner_tons);
      if(mode == pmStart) slide_backup(nohud, false);
      if(mode == pmStart) slide_backup(vid.ispeed, 0);
      static int start;
      slidecommand = "start animation";
      if(mode == pmKey) start = ticks;
      if(mode == pmFrame) {
        for(auto c: current_list) c->item = itNone;
        int j = (ticks - start) / 500;
        println(hlog, "j = ", j, " / ", isize(current_list));
        for(int i=0; i<isize(current_list); i++) {
          if(i >= j && i <= j+5) {
            current_list[i]->item = itDiamond;
            println(hlog, "highlight ", i);
            }
          }
        }
      tour::slide_backup(no_find_player, true);
      add_stat(mode, [] {
        gamescreen();
        dialog::init();
        dialog::addTitle("uh", 100, 0);
        dialog::addBreak(1600);
        dialog_may_latex("dynamic programming: $O(c^w\\cdot n)$", "dynamic programming: O(c^w*n)");
        dialog_may_latex("$n$ -- number of tiles, $w$ -- width, $c$ -- fixed constant", "n -- number of tiles, w -- width, c -- fixed constant");
        dialog::display();
        return true;
        });
      non_game_slide_scroll(mode);
      }
    },

  {"Minesweeper on a tree-like board", 999, LEGAL::NONE | QUICKGEO, 
    "The same trick can also be used on a tree-like board. What is width then? Well, of course we will need to use the graph parameter called 'tree width'."
    ,
    [] (presmode mode) {
      mine_slide(mode, geom_high_hyper, cl_high_hyper, assigner_tons);
      if(mode == pmStart) slide_backup(nohud, false);
      static bool show = false;
      slidecommand = "show the stats";
      if(mode == pmKey) show = !show;
      add_stat(mode, [] {
        tour::slide_backup(no_find_player, true);
        gamescreen();
        if(!show) return true;
        dialog::init();
        dialog::addTitle("uh", 100, 0);
        dialog::addBreak(1600);
        dialog_may_latex("dynamic programming: $O(c^w\\cdot n)$", "dynamic programming: O(c^w*n)");
        dialog_may_latex("$n$ -- number of tiles, $w$ -- treewidth, c -- fixed constant", "n -- number of tiles, w -- treewidth, c -- fixed constant");
        dialog::display();
        return true;
        });
      }
    },

  {"What is tree-width?", 999, LEGAL::NONE | QUICKGEO, 
    "The definition of treewidth, using a cops-and-robber game."
    ,
    [] (presmode mode) {
      empty_screen(mode);
      add_stat(mode, [] {
        cmode |= sm::DARKEN;
        gamescreen();
        dialog::init();
        tour::slide_backup(no_find_player, true);
        dialog::addTitle("cops", 0x8080FF, 150);
        dialog::addInfo("they know where the robber is");
        dialog::addInfo("can use a helicopter to land somewhere on the board");
        dialog::addBreak(100);
        dialog::addTitle("robber", 0xFF8080, 150);
        dialog::addInfo("can move very fast while a cop is flying");
        dialog::addInfo("but cannot move through the cops currently on board");
        dialog::addBreak(100);
        dialog::addInfo("cops who are no longer blocking the robber can be removed");
        dialog::addBreak(100);
        dialog::addInfo("treewidth+1 = the number of cops necessary to catch the robber");
        dialog::display();
        return true;
        });
      no_other_hud(mode);

/*
      empty_screen(mode);
      add_stat(mode, [] {
        tour::slide_backup(nohelp, true);
        tour::slide_backup(no_find_player, true);
        gamescreen(2);
        dialog::init();
        dialog::addTitle("cops", 150, 0x8080FF);
        dialog::addInfo("can use a helictoper to land somewhere on the board");
        dialog::addTitle("robber", 150, 0xFF8080);
        dialog::addInfo("can move very fast while a cop is flying");
        dialog::addInfo("but cannot move through the cops currently on board");
        dialog::addBreak(100);
        dialog::addInfo("cops who are no longer blocking the robber can be removed");
        dialog::addInfo("treewidth+1 = the number of cops necessary to catch the robber");
        dialog::display();
        return true;
        });
      no_other_hud(mode); */
      }
    },

  {"Tree-like board revisited", 999, LEGAL::NONE | QUICKGEO, 
    "You can play the cops-and-robber game on this tree-like board ('m' to place a cop), and see how many cops you need to catch the robber (not implemented in this slide show, you have to imagine them)."
    ,
    [] (presmode mode) {
      mine_slide(mode, geom_high_hyper, cl_high_hyper, assigner_fullmine);
      if(mode == pmStart) slide_backup(mine::mark_always, true);
      static bool show = false;
      if(mode == pmStart) slide_backup(nohud, false);
      slidecommand = "show the stats";
      if(mode == pmKey) show = !show;
      add_stat(mode, [] {
        tour::slide_backup(no_find_player, true);
        gamescreen();
        if(!show) return true;
        dialog::init();
        dialog::addTitle("uh", 100, 0);
        dialog::addBreak(1600);
        dialog_may_latex("dynamic programming: $O(c^w\\cdot n)$", "dynamic programming: O(c^w*n)");
        dialog_may_latex("$n$ -- number of tiles, $w$ -- treewidth, c -- fixed constant", "n -- number of tiles, w -- treewidth, c -- fixed constant");
        dialog::display();
        return true;
        });
      }
    },

  {"Grid minor", 999, LEGAL::NONE | QUICKGEO, 
    "To show that Hyperbolic Minesweeper is in P, we will need to use a theorem about grid minors."
    ,
    [] (presmode mode) {
      empty_screen(mode);
      add_stat(mode, [] {
        tour::slide_backup(no_find_player, true);
        gamescreen();
        dialog::init();
        dialog_may_latex(
          "{\\bf Theorem (Robertson, Seymour, Thomas '94)}\n\n"
          "Given a planar graph $G = (V,E)$ and a number $t$,\n\n"
          "it is possible to either find a $t \\times t$ grid as a minor of $G$,\n\n"
          "or produce a tree decomposition of $G$ of width $\\leq 5t-6$,\n\n"
          "in time $O(n^2 \\log(n))$, where $n = |V|$.\n\n\\vskip 1em\n\nTL;DR: a graph has a $t \\times t$ grid minor or $O(t)$ treewidth.",
          "(RST theorem)"
          );
        dialog::display();
        return true;
        });
      no_other_hud(mode);
      }
    },

  {"Hyperbolic Minesweeper is in P", 999, LEGAL::NONE | QUICKGEO, 
    "Hyperbolic Minesweeper is in P, as an easy consequence."
    ,
    [] (presmode mode) {
      empty_screen(mode);
      add_stat(mode, [] {
        tour::slide_backup(no_find_player, true);
        gamescreen();
        dialog::init();
        dialog_may_latex(
          "A graph has a $t \\times t$ grid minor or $O(t)$ treewidth.\n\n"
          "\\vskip 1em\n\n"
          "Assume a {\\bf fixed} hyperbolic tessellation.\n\n"
          "\\vskip 1em\n\n"
          "Circles in the hyperbolic plane grow {\\bf exponentially} with radius.\n\n"
          "\\vskip 1em\n\n"
          "Thus, a $t \\times t$ grid minor requires $\\Omega(c^t)$ vertices.\n\n"
          "\\vskip 1em\n\n"
          "Thus, treewidth $w$ is {\\bf logarithmic} in $n$.\n\n"
          "\\vskip 1em\n\n"
          "Thus, $O(c^w\\cdot n)$ is actually {\\bf polynomial}!",
          "(our theorem)"
          );
        dialog::display();
        return true;
        });
      no_other_hud(mode);
      }
    },

  {"constraint satisfaction: one green block and one blue block around every tile", 999, LEGAL::NONE | QUICKGEO, 
    "This slideshow is not really about Minesweeper! We can also do the same reasoning for other counting/existence problems based on satisfying local constraints. Here we create a random map satisfying some constraints. Can you tell what these constraints are?\n\n"
    "(Press '5' to generate another one)"
    ,
    [] (presmode mode) {
      wfc_slide(mode, 1, 5, 2);
      }
    },

  {"constraint satisfaction: two blocks of every color", 999, LEGAL::NONE | QUICKGEO, 
    "Another set of constraints!\n\n"
    ,
    [] (presmode mode) {
      wfc_slide(mode, 2, 5, 2);
      }
    },

  {"constraint satisfaction: higher, equal, lower, equal", 999, LEGAL::NONE | QUICKGEO, 
    "Another set of constraints!\n\n"
    ,
    [] (presmode mode) {
      wfc_slide(mode, 3, 5, 3);
      }
    },

  {"what about 3D hyperbolic space? (1)", 999, LEGAL::NONE | QUICKGEO, 
    "This result does not extend to 3D hyperbolic space. Because you can put a horosphere in a 3D hyperbolic space, and the geometry on the horosphere is Euclidean, so 3D hyperbolic Minesweeper is at least as hard as Euclidean 2D!\n\n"
    "To see this, let's remind you of the binary tiling..."
    ,
    [] (presmode mode) {
      setCanvas(mode, 'g');
      non_game_slide_scroll(mode);
      if(mode == pmStart) {
        tour::slide_backup(patterns::canvasback, 0x10A010);
        stop_game();
        set_geometry(gBinary4);
        set_variation(eVariation::pure);
        start_game();
        tour::slide_backup(vid.use_smart_range, 2);
        tour::slide_backup(vid.smart_range_detail, 1);
        View = spin90();
        using linepatterns::patTree;
        tour::slide_backup(patTree.color, 0);
        }        
      if(mode == pmFrame) clearMessages();
      slidecommand = "show the tree";
      if(mode == pmKey) {
        using linepatterns::patTree;
        println(hlog, "key called");
        patTree.color ^= 0xFFFFFFFF;
        tour::slide_backup(patTree.multiplier, 5);
        }
      no_other_hud(mode);
      }
    },

  {"what about 3D hyperbolic space? (2)", 999, LEGAL::NONE | QUICKGEO, 
    "The 3D equivalent of the binary tiling is similar: every cell has 2x2 children cells. And thus it has 2^n x 2^n n-th descendant cells, arranged in an Euclidean square.\n\n"
    ,
    [] (presmode mode) {
      non_game_slide_scroll(mode);
      mine_slide(mode, geom_h3, cl_h3_rectangle, assigner_fullmine);
      tour::slide_backup(stdgridcolor, 0xFFFFFFFF);
      tour::slide_backup(vid.grid, true);
      if(mode == pmStart) {
        centerover = currentmap->gamestart();
        View = Id;
        rotate_view(cspin(0, 2, 90));
        shift_view(ztangent(5));
        shift_view(ztangent(-4));
        // View = cspin(0, 2, 90) * View;
        }
      if(mode == pmKey) {
        shift_view(ztangent(.2));
        }
      }
    },

  {"what about hyperbolic quotient spaces?", 999, LEGAL::NONE | QUICKGEO, 
    "Can we tell anything about hyperbolic quotient spaces? (No idea yet?)\n\n"
    ,
    [] (presmode mode) {
      mine_slide(mode, geom_klein_quartic, cl_all, assigner);
      }
    },

  {"final slide", 123, LEGAL::ANY | NOTITLE | QUICKSKIP, 
    "This was the last slide. Thanks for watching!",
  
    [] (presmode mode) {
      empty_screen(mode);
      add_stat(mode, [] {
        dialog::init();
        // color_t d = dialog::dialogcolor;

        dialog::addTitle("Thanks for your attention!", 0xC00000, 200);

        // dialog::addBreak(100);

        // dialog::addTitle("twitter.com/zenorogue/", d, 150);
        
        dialog::display();
        return true;
        });
      no_other_hud(mode);
      }
    },

  // extra slides

  {"Variants: Minesweeper 6D", 123, LEGAL::ANY | QUICKSKIP, "Minesweeper 6D, downloadable", 
    [] (presmode mode) {
      empty_screen(mode);
      if(mode == pmStart) {
        tour::slide_backup(backcolor, 0xC0C0C0);
        tour::slide_backup(bordcolor, 0);
        }
      show_picture(mode, "rogueviz/mine/mine4d.png");
      no_other_hud(mode);
      }
    },

  {"spherical Minesweeper", 999, LEGAL::NONE | QUICKGEO, 
    "aaa\n\n"
    ,
    [] (presmode mode) {
      non_game_slide_scroll(mode);
      mine_slide(mode, geom_sphere, cl_sphere, assigner);
      }
    },

  {"final slide", 123, LEGAL::ANY | NOTITLE | QUICKSKIP | FINALSLIDE, 
    "Well, now, this was really the last slide. Thanks for watching!",
  
    [] (presmode mode) {
      empty_screen(mode);
      add_stat(mode, [] {
        dialog::init();
        // color_t d = dialog::dialogcolor;

        dialog::addTitle("Thanks for your attention!", 0xC00000, 200);

        // dialog::addBreak(100);

        // dialog::addTitle("twitter.com/zenorogue/", d, 150);
        
        dialog::display();
        return true;
        });
      no_other_hud(mode);
      }
    }
  };
  
int phooks = 
  0 +
  addHook_slideshows(100, [] (tour::ss::slideshow_callback cb) {
    cb(XLAT("Hyperbolic Minesweeper is in P"), &sweeper_slides[0], 'h');
    });
#endif 
}

// kolor zmienic dziada
