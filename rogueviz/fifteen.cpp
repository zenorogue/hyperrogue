// Hyperbolic Rogue -- fifteen+4 puzzle
// Copyright (C) 2011-2019 Zeno Rogue, see 'hyper.cpp' for details

/** \file fifteen.cpp
 *  \brief fifteen+4 puzzle
 */

#include "rogueviz.h"

namespace hr {

EX namespace fifteen {

static const int Empty = 0;

struct celldata {
  int target, targetdir;
  bool targetmirror;
  int current, currentdir;
  bool currentmirror;
  };

map<cell*, celldata> fif;

vector<int> triangle_markers;
vector<cell*> seq;
vector<ld> turns;

eWall empty = waChasm;

enum ePenMove { pmJump, pmRotate, pmAdd, pmMirrorFlip };
ePenMove pen;

bool show_triangles = false;
bool show_dots = true;

void init_fifteen(int q = 20) {  
  println(hlog, "init_fifteen");
  auto ac = currentmap->allcells();
  for(int i=0; i<min(isize(ac), q); i++) {
    fif[ac[i]] = {i, 0, false, i, 0, false};
    }  
  cwt.at = ac[0];
  println(hlog, "ok");
  }

void compute_triangle_markers() {
  triangle_markers.resize(isize(fif));
  seq.resize(isize(fif));
  for(auto& p: fif) {
    cell *c = p.first;
    
    forCellIdEx(c1, i, c) if(fif.count(c1) && fif[c1].target == p.second.target + 1) {
      triangle_markers[p.second.target] = gmod((1 + i - p.second.targetdir) * (p.second.targetmirror ? -1 : 1), c->type);
      }
    
    if(p.second.current == 0)
      seq.back() = c;
    else {
      seq[p.second.current-1] = c;
      }
    }

  println(hlog, triangle_markers);

  for(int i=0; i<isize(fif); i++) {
    turns.push_back(triangle_markers[i+1] == 0 ? 90*degree : 0);
    }
  }

string dotted(int i) {
  string s = its(i);
  if(!show_dots) return s;
  bool confusing = true;
  for(char c: s) if(!among(c, '0', '6', '8', '9') && !(nonorientable && c == '3'))
    confusing = false;
  if(confusing) s += ".";
  return s;
  }

/** where = empty square */
void make_move(cell *where, int dir) {
  auto nw = where->cmove(dir);
  auto mir = where->c.mirror(dir);
  auto& f0 = fif[where];
  auto& f1 = fif[nw];
  f0.current = f1.current;
  f0.currentmirror = f1.currentmirror ^ mir;
  int d = f1.currentdir;
  d -= where->c.spin(dir);
  if(mir) d *= -1;
  d += dir;
  if(!mir) d += nw->type/2;
  f0.currentdir = gmod(d, nw->type);
  f1.current = Empty;
  }

void check_move() {
  for(int i=0; i<cwt.at->type; i++) {
    cell *c1 = cwt.at->cmove(i);
    if(fif.count(c1) && fif[c1].current == Empty) {
      make_move(c1, cwt.at->c.spin(i));
      }
    }
  }

void scramble() {
  for(int i=0; i<1000; i++) {
    int d = hrand(cwt.at->type);
    cell *c1 = cwt.at->move(d);
    if(fif.count(c1)) {
      make_move(cwt.at, d);
      cwt.at = c1;
      }
    }     
  }

bool draw_fifteen(cell *c, const shiftmatrix& V) {
  lastexplore = turncount;
  if(!fif.count(c)) { c->land = laNone; c->wall = waChasm; return false; }
  check_move();
    
  auto& cd = fif[c];
  
  int cur = anyshiftclick ? cd.target : cd.current;
  int cdir = anyshiftclick ? cd.targetdir : cd.currentdir;
  bool cmir = anyshiftclick ? cd.targetmirror : cd.currentmirror;
  
  if(cur == Empty) {
    c->land = laCanvas;
    c->wall = waNone;
    c->landparam = 0x101080;
    }
  else {
    c->land = laCanvas;
    c->wall = waNone;
    c->landparam = 0xFFFFFF;
    if(cdir < 0 || cdir >= c->type) {
      println(hlog, "ERROR: invalid dir ", cdir);
      cdir = 0;
      }
    write_in_space(V * ddspin(c,cdir,0) * (cmir ? MirrorX: Id), 72, 1, dotted(cur), 0xFF, 0, 8);
    if(show_triangles) {
      cellwalker cw(c, cdir);
      cw += triangle_markers[cur] - 1;
      poly_outline = 0xFF;
      queuepoly(V * ddspin(c, cw.spin, 0) * xpush(hdist0(tC0(currentmap->adj(c, cw.spin))) * .45 - cgi.zhexf * .3), cgi.shTinyArrow, 0xFF);
      }
    }
  
  return false;
  }

void edit_fifteen() {

  if(!fif.count(cwt.at)) 
    init_fifteen();

  clearMessages();

  getcstat = '-';
  cmode = 0;
  cmode = sm::SIDE | sm::DIALOG_STRICT_X | sm::MAYDARK;

  auto ss = mapstream::save_start();
  ss->item = itGold;
  gamescreen();
  ss->item = itNone;
  
  dialog::init("Fifteen Puzzle", iinf[itPalace].color, 150, 100);

  dialog::addBoolItem("jump", pen == pmJump, 'j');
  dialog::add_action([] { pen = pmJump; });

  dialog::addBoolItem("rotate", pen == pmRotate, 'r');
  dialog::add_action([] { pen = pmRotate; });

  dialog::addBoolItem("mirror flip", pen == pmMirrorFlip, 'f');
  dialog::add_action([] { pen = pmMirrorFlip; });

  dialog::addBoolItem("add tiles", pen == pmAdd, 'a');
  dialog::add_action([] { pen = pmAdd; });

  dialog::addBreak(100);

  dialog::addItem("this is the goal", 'g');
  dialog::add_action([] { 
    for(auto& sd: fif) {
      sd.second.target = sd.second.current;
      sd.second.targetdir = sd.second.currentdir;
      sd.second.targetmirror = sd.second.currentmirror;
      }
    });

  dialog::addItem("remove all tiles", 'r');
  dialog::add_action([] {
    fif.clear();
    init_fifteen(1);
    });

  dialog::addItem("scramble", 's');
  dialog::add_action(scramble);
  
  dialog::addItem("save this puzzle", 'S');
  dialog::add_action([] { 
    mapstream::saveMap("fifteen.lev");
    #if ISWEB
    offer_download("fifteen.lev", "mime/type");
    #endif
    });

  dialog::addItem("settings", 'X');
  dialog::add_action_push(showSettings);

  mine_adjacency_rule = true;
  
  dialog::addItem("new geometry", 'G');
  dialog::add_action(runGeometryExperiments);

  dialog::addItem("load a puzzle", 'L');
  dialog::add_action([] { 
    #if ISWEB
    offer_choose_file([] {
      mapstream::loadMap("data.txt");
      });
    #else
    mapstream::loadMap("fifteen.lev");
    #endif
    mapeditor::drawplayer = false;
    });

  dialog::addBack();
  
  dialog::display();
  
  keyhandler = [] (int sym, int uni) {
    dialog::handleNavigation(sym, uni);
    
    if(sym == '-' && mouseover && !holdmouse) {
      cell *c = mouseover;
      
      if(pen == pmJump) {
        if(fif.count(c)) {
          auto& f0 = fif[cwt.at];
          auto& f1 = fif[c];
          swap(f0, f1);
          cwt.at = c;
          }
        else {
          fif[c] = fif[cwt.at];
          fif.erase(cwt.at);
          cwt.at = c;
          }
        }
      
      if(pen == pmRotate) {
        if(fif.count(c) == 0) return;
        auto& f1 = fif[c];
        f1.currentdir = gmod(1+f1.currentdir, c->type);        
        f1.targetdir = gmod(1+f1.targetdir, c->type);        
        }
      
      if(pen == pmMirrorFlip) {
        if(fif.count(c) == 0) return;
        auto& f1 = fif[c];
        f1.currentmirror ^= true;
        f1.targetmirror ^= true; 
        }
      
      if(pen == pmAdd) {
        if(fif.count(c) == 0) {
          auto& f = fif[c];
          f.current = f.target = isize(fif)-1;
          f.currentdir = f.targetdir == 0;
          }
        else {
          auto& f = fif[c];
          if(f.current == isize(fif)-1)
            fif.erase(c);
          }
        }

      compute_triangle_markers();
      }

    else if(doexiton(sym, uni)) popScreen();
    };
  }

void launch() {  
  /* setup */
  stop_game();
  enable_canvas();
  canvas_default_wall = waChasm;
  start_game();
  init_fifteen();

  showstartmenu = false;
  mapeditor::drawplayer = false;
  }

void enable();

void load_fifteen(hstream& f) {
  int num;
  f.read(num);
  fif.clear();
  println(hlog, "read num = ", num);
  for(int i=0; i<num; i++) {
    int32_t at = f.get<int>();
    println(hlog, "at = ", at);
    cell *c = mapstream::cellbyid[at];
    auto& cd = fif[c];      
    f.read(cd.target);
    f.read(cd.targetdir);
    cd.targetdir = mapstream::fixspin(mapstream::relspin[at], cd.targetdir, c->type, f.vernum);
    if(nonorientable) 
      f.read(cd.targetmirror);
    f.read(cd.current);
    f.read(cd.currentdir);
    if(nonorientable) 
      f.read(cd.currentmirror);
    cd.currentdir = mapstream::fixspin(mapstream::relspin[at], cd.currentdir, c->type, f.vernum);
    println(hlog, "assigned ", cd.current, " to ", c);
    }
  compute_triangle_markers();
  enable();
  }

void o_key(o_funcs& v) {
  v.push_back(named_dialog("edit the Fifteen puzzle", edit_fifteen));
  }

void enable() {
  rogueviz::rv_hook(hooks_o_key, 80, o_key);
  rogueviz::rv_hook(hooks_drawcell, 100, draw_fifteen);
  rogueviz::rv_hook(mapstream::hooks_savemap, 100, [] (hstream& f) {
    f.write<int>(15);
    f.write<int>(isize(fif));
    for(auto cd: fif) {
      f.write(mapstream::cellids[cd.first]);
      println(hlog, cd.first, " has id ", mapstream::cellids[cd.first]);
      f.write(cd.second.target);
      f.write(cd.second.targetdir);
      if(nonorientable) 
        f.write(cd.second.targetmirror);      
      f.write(cd.second.current);
      f.write(cd.second.currentdir);
      if(nonorientable) 
        f.write(cd.second.currentmirror);
      }
    });
  rogueviz::rv_hook(hooks_clearmemory, 40, [] () {
    fif.clear();
    });
  }

#if CAP_COMMANDLINE
int rugArgs() {
  using namespace arg;
           
  if(0) ;
  else if(argis("-fifteen")) {
    PHASEFROM(3);
    launch();
    addHook(mapstream::hooks_loadmap_old, 100, load_fifteen);
    #if ISWEB
    mapstream::loadMap("1");    
    #else
    enable();
    #endif
    }
  else if(argis("-fifteen-center")) {
    // format: -fifteen-center 5 V 0
    // to center at 0'th vertex of 5
    shift(); int i = argi();
    dynamicval<eCentering> ctr(centering);
    dynamicval<cellwalker> lctr(cwt);
    for(auto& p: fif) {
      auto& c = p.first;
      auto& data = p.second;
      if(data.target == i) {
        int dir = 0;
        while(true) {
          shift(); string s = args();
          if(s == "F") { centering = eCentering::face; continue; }
          if(s == "E") { centering = eCentering::edge; continue; }
          if(s == "V") { centering = eCentering::vertex; continue; }
          dir = argi();
          break;
          }
        cwt = cellwalker(c, dir);
        fullcenter();
        }
      }
    playermoved = false;
    vid.sspeed = -5;
    }

  else return 1;
  return 0;
  }

auto fifteen_hook = 
  addHook(hooks_args, 100, rugArgs)
#if CAP_SHOT
+ arg::add3("-fifteen-animate", [] { 
    rogueviz::rv_hook(anims::hooks_record_anim, 100, [] (int i, int nof) {
    double at = (i * (isize(seq)-1) * 1.) / nof;
    int ati = at;
    double atf = at - ati;
    hyperpoint h0 = unshift(ggmatrix(seq[ati]) * C0);
    hyperpoint h1 = unshift(ggmatrix(seq[ati+1]) * C0);
    atf = atf*atf*(3-2*atf);
    hyperpoint h2 = lerp(h0, h1, atf);
    println(hlog, "h0=", h0, " h1=", h1, " h2=", h2);
    if(invalid_point(h2) || h2[2] < .5) return;
    h2 = normalize(h2);
    static ld last_angle = 0;
    static int last_i = 0;

    View = gpushxto0(h2) * View;

    if(ati != last_i && last_angle) {
      View = spin(-(turns[last_i] - last_angle)) * View;
      last_angle = 0;
      }
    
    if(true) {
      ld angle = lerp(0, turns[ati], atf);
      ld x = -(angle - last_angle);
      View = spin(x) * View;
      last_angle = angle;
      last_i = ati;
      }

    anims::moved();
    }); })
#endif
+ addHook(mapstream::hooks_loadmap, 100, [] (hstream& f, int id) {
    if(id == 15) load_fifteen(f);
    })
+ addHook(hooks_configfile, 100, [] {
    param_b(show_dots, "fifteen_dots");
    param_b(show_triangles, "fifteen_tris");
    })
+ addHook_slideshows(120, [] (tour::ss::slideshow_callback cb) {

    using namespace rogueviz::pres;
    static vector<slide> fifteen_slides;

    if(fifteen_slides.empty()) {
      fifteen_slides.emplace_back(
        slide{"Introduction", 999, LEGAL::NONE, 
          "This is a collection of some geometric and topological variants of the Fifteen puzzle. Most of these "
          "are digital implementations of the mechanical designs by Henry Segerman."
          ,
          [] (presmode mode) {}
          });
      
      auto add = [&] (string s, string lev, string text, string youtube = "") {
        fifteen_slides.emplace_back(
          tour::slide{s, 100, LEGAL::NONE | QUICKGEO, text,
            [=] (presmode mode) {
              if(youtube != "")
                slide_url(mode, 'y', "YouTube link", youtube);
              string fname = "fifteen/" + lev + ".lev";
              if(!file_exists(fname)) {
                slide_error(mode, "file " + fname + " not found");
                return;
                }
              setCanvas(mode, '0');
              if(mode == pmStart) {
                slide_backup(mapeditor::drawplayer, mapeditor::drawplayer);
                slide_backup(vid.wallmode, 2);
                slide_backup(pconf.scale, .6);
                slide_backup(no_find_player, true);
                stop_game();
                mapstream::loadMap(fname);
                popScreenAll();
                fullcenter();
                if(lev == "coiled" || lev == "mobiusband")
                  View = spin(90*degree) * View;
                if(lev == "mobiusband")
                  View = MirrorX * View;
                }
              }});
        };
      
      add("15", "classic", "The original Fifteen puzzle.");
      add("15+4", "fifteen", "The 15+4 puzzle by Henry Segerman.", "https://www.youtube.com/watch?v=Hc3yfuXiWe0");
      add("15-4", "sphere11", "The 15-4 puzzle.");
      add("coiled", "coiled", "Coiled fifteen puzzle by Henry Segerman.", "https://www.youtube.com/watch?v=rfAEgxNEOrQ");
      add("Möbius band", "mobiusband", "Fifteen puzzle on a Möbius band.");
      add("Kite-and-dart", "kitedart", "Kite-and-dart puzzle.");
      add("29", "29", "The 29 puzzle by Henry Segerman.", "https://www.youtube.com/watch?v=EitWHthBY30");
      add("12", "12", "The 12 puzzle mentioned in the same video by Henry Segerman.", "https://www.youtube.com/watch?v=EitWHthBY30");
      add("124", "124", "The 124 puzzle mentioned in the same video by Henry Segerman.", "https://www.youtube.com/watch?v=EitWHthBY30");
      add("60", "60", "The 124 puzzle mentioned in the same video by Henry Segerman.", "https://www.youtube.com/watch?v=EitWHthBY30");
      add("Continental drift", "sphere19", "Based on the Continental Drift puzzle by Henry Segerman.", "https://www.youtube.com/watch?v=0uQx33KFMO0");
      
      add_end(fifteen_slides);
      }

    cb(XLAT("variants of the fifteen puzzle"), &fifteen_slides[0], 'f');
    });
#endif

EX }

EX }

