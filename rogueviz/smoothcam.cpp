#include "rogueviz.h"

// This module allows creating complex animations with smooth camera movement

// enable with -smoothcam option, or the new "smooth camera" option in the Animations dialog

// it still seems a bit buggy

// to add: insert positions? split/merge segments? improved join?

namespace hr {

using pcell = cell*;

inline void hread(hstream& hs, transmatrix& h) { for(int i=0; i<MDIM; i++) hread(hs, h[i]); }
inline void hwrite(hstream& hs, transmatrix h) { for(int i=0; i<MDIM; i++) hwrite(hs, h[i]); }

inline void hwrite(hstream& hs, const pcell& c) {
  hs.write<int>(mapstream::cellids[c]);
  }

inline void hread(hstream& hs, pcell& c) {
  int32_t at = hs.get<int>();
  c = mapstream::cellbyid[at];
  }
  
namespace smoothcam {

string smooth_camera_help = 
  "This feature lets you create animations with complex but smooth camera movement.\n\n"
  "An animation is composed from a number of segments.\n\n"
  "In each segment, you can provide a number of positions, and times for them. "
  "For example, if you add a camera position A at time 0 and a camera position B at time 1, "
  "we will move linearly from A to B. Polynomial approximation is used inside a segment, "
  "while separate segments are animated independently.\n\n"
  "The 'interval' values are the interval between the current and next position. "
  "The total sum of 'interval' values is made equal to the 'animation period'. "
  "If you place two positions X and Y with interval 0 between them, X will be used"
  "as the actual position, while Y-X will be the first derivative. Thus, for example, "
  "placing two equal positions with interval 0 will force the camera to smoothly stop.";

struct frame {
  string title;
  cell *where;
  transmatrix sView;
  transmatrix V;
  transmatrix ori;
  ld front_distance, up_distance;
  ld interval;
  };

struct animation {
  cell *start_cell;
  transmatrix start;
  ld start_interval;
  vector<frame> frames;
  };

map<cell*, map<hyperpoint, string> > labels;
map<cell*, vector<vector<hyperpoint> > > traces;

vector<animation> anims;

transmatrix last_view, current_position, last_view_comp;
cell *last_centerover;

// during the animation, transform original coordinates to the current view coordinates
transmatrix last_computed;
ld last_time;

void analyze_view_pre() {
  current_position = current_position * last_view * inverse(View);
  }

void analyze_view_post() {
  last_view = View;
  }

animation *current_segment;

void start_segment() {
  anims.emplace_back();
  auto& anim = anims.back();
  anim.start_cell = centerover;
  anim.start = Id;
  last_view = Id;
  current_position = Id;
  current_segment = &anim;
  }

/** does not work correctly -- should adjust to the current cell */
void join_segment() {
  int n = anims.back().frames.size();
  if(n < 2) return;
  auto s1 = anims.back().frames[n-2];
  auto s2 = anims.back().frames[n-1];
  start_segment();
  auto& l = anims[anims.size()-2];
  anims.back().frames.push_back(l.frames[n-2]);
  anims.back().frames.push_back(l.frames[n-1]);
  anims.back().start_cell = l.start_cell;
  anims.back().start = l.start;
  anims.back().start_interval = 0;
  }

map<cell*, int> indices;

string gentitle() {
  return lalign(0, centerover, ":", indices[centerover]++);
  }

bool animate_on;
bool view_labels, view_trace;

void edit_interval(ld& v) {
  dialog::add_action([&v] {
    dialog::editNumber(v, -10, 10, 1, 0, "interval", "");
    });
  }

transmatrix try_harder_relative_matrix(cell *at, cell *from) {
  transmatrix U = Id;
  int d = celldistance(at, from);
  again:
  while(d > 0) {
    forCellIdEx(c1, i, at) {
      int d1 = celldistance(c1, from);
      if(d1 < d) {
        U = currentmap->iadj(at, i) * U;
        d = d1;
        at = c1;
        goto again;
        }
      }
    println(hlog, "still failed");
    return Id;
    }
  println(hlog, "got U = ", U);
  return U;
  }

void edit_segment(int aid) {
  cmode = sm::PANNING;
  gamescreen();
  dialog::init(XLAT("animation segment"), 0xFFFFFFFF, 150, 0);
  dialog::addSelItem("interval", fts(anims[aid].start_interval), 'i');
  edit_interval(anims[aid].start_interval);
  dialog::addItem("delete", 'd');
  dialog::add_action([aid] {
    anims.erase(anims.begin()+aid);
    if(anims.empty()) start_segment();
    popScreen();
    });
  dialog::addItem("mirror", 'm');
  dialog::add_action([aid] {
    auto a = anims[aid];
    reverse(a.frames.begin(), a.frames.end());
    ld* last = &a.start_interval;
    for(auto& f: a.frames) { swap(*last, f.interval); last = &f.interval; }
    anims.push_back(std::move(a));
    popScreen();
    });
  dialog::addItem("copy before", 'c');
  dialog::add_action([aid] {
    auto a = anims[aid];
    anims.insert(anims.begin() + aid, a);
    current_segment = nullptr;
    popScreen();
    });
  dialog::addItem("swap with the last segment", 'x');
  dialog::add_action([aid] {
    swap(anims.back(), anims[aid]);
    current_segment = nullptr;
    popScreen();
    });
  dialog::addBack();
  dialog::display();
  }

void generate_trace();

void edit_step(animation& anim, int id) {
  cmode = 0;
  gamescreen();
  dialog::init(XLAT("animation step"), 0xFFFFFFFF, 150, 0);
  auto& f = anim.frames[id];
  dialog::addSelItem("title", f.title, 't');
  dialog::addSelItem("interval", fts(f.interval), 'i');
  edit_interval(f.interval);
  dialog::addSelItem("front distance", fts(f.front_distance), 'f');
  dialog::add_action([&f] {
    dialog::editNumber(f.front_distance, -5, 5, .1, 1, "front distance", "");
    });
  dialog::addSelItem("up distance", fts(f.up_distance), 'u');
  dialog::add_action([&f] {
    dialog::editNumber(f.up_distance, -5, 5, .1, 1, "up distance", "");
    });
  dialog::addItem("delete", 'd');
  dialog::add_action([&anim, id] {
    anim.frames.erase(anim.frames.begin()+id);
    popScreen();
    });
  if(&anim == current_segment) {
    dialog::addItem("change to current camera location", 'e');
    dialog::add_action([&f] {
      f.where = centerover;
      f.sView = View;
      f.V = current_position;
      popScreen();
      });
    }
  dialog::addItem("move the camera here", 'r');
  dialog::add_action([&f] {
    transmatrix Rel = calc_relative_matrix(centerover, f.where, inverse(View) * C0);
    println(hlog, "Rel = ", Rel);
    if(eqmatrix(Rel, Id) && centerover != f.where)
      Rel = try_harder_relative_matrix(centerover, f.where);
    View = f.sView * Rel;
    NLP = ortho_inverse(f.ori);
    playermoved = false;
    current_display->which_copy = 
      nonisotropic ? gpushxto0(tC0(view_inverse(View))) :
      View;
    popScreen();
    });
  dialog::addItem("edit this segment and move the camera here", 'p');
  dialog::add_action([&anim, &f] {
    last_view = View = f.sView;
    NLP = ortho_inverse(f.ori);
    centerover = f.where;
    current_position = f.V;    
    playermoved = false;
    current_display->which_copy = 
      nonisotropic ? gpushxto0(tC0(view_inverse(View))) :
      View;
    current_segment = &anim;
    popScreen();
    });
  dialog::addItem("start a new segment from here", 'n');
  dialog::add_action([&f] {
    View = f.sView;
    centerover = f.where;
    playermoved = false;
    NLP = ortho_inverse(f.ori);
    current_display->which_copy = 
      nonisotropic ? gpushxto0(tC0(view_inverse(View))) :
      View;
    start_segment();
    popScreen();
    });
  if(&anim == current_segment) {
    dialog::addItem("insert the current position before this", 'j');
    dialog::add_action([&anim, id] {
      anim.frames.insert(anim.frames.begin() + id, frame{gentitle(), centerover, View, current_position, ortho_inverse(NLP), 1, 1, 0});
      popScreen();
      });
    }

  dialog::addBack();
  dialog::display();
  }

int last_segment;

ld test_t = 0;
ld c_front_dist = 0, c_up_dist = 0;
void handle_animation(ld t);

bool side = true;

void snap_to_center() {
  cmode = side ? sm::SIDE : 0;
  gamescreen();
  draw_crosshair();
  dialog::init(XLAT("snap to center"), 0xFFFFFFFF, 150, 0);
  
  dialog::addItem("center on mouse", ' ');
  dialog::add_action([] {
    View = gpushxto0(unshift(mapeditor::mouse_snap())) * View;
    });

  dialog::addItem("mouse up", 'w');
  dialog::add_action([] {
    View = spin90() * spintox(unshift(mapeditor::mouse_snap())) * View;
    });

  dialog::addItem("mouse down", 's');
  dialog::add_action([] {
    View = spin270() * spintox(unshift(mapeditor::mouse_snap())) * View;
    });

  dialog::addItem("mouse left", 'a');
  dialog::add_action([] {
    View = spin180() * spintox(unshift(mapeditor::mouse_snap())) * View;
    });

  dialog::addItem("mouse left", 'd');
  dialog::add_action([] {
    View = Id * spintox(unshift(mapeditor::mouse_snap())) * View;
    });

  dialog::addBack();
  dialog::display();
  
  keyhandler = [] (int sym, int uni) {
    handlePanning(sym, uni);
    dialog::handleNavigation(sym, uni);
    if(doexiton(sym, uni)) popScreen();
    };
  }

void show() {
  cmode = side ? sm::SIDE : 0;
  gamescreen();
  draw_crosshair();
  dialog::init(XLAT("smooth camera"), 0xFFFFFFFF, 150, 0);
  int aid = 0;
  
  labels.clear();
  
  dialog::start_list(2000, 2000, 'A');
  
  for(auto& anim: anims) {
    dialog::addSelItem("segment #" + its(aid) + (&anim == current_segment ? "*" : ""), fts(anim.start_interval), dialog::list_fake_key++);
    dialog::add_action_push([aid] { edit_segment(aid); });
    int id = 0;
    for(auto& f: anim.frames) {
      labels[f.where][inverse(f.sView) * C0] = f.title;
      string dist;
      
      if(f.where != centerover)
        dist = its(celldistance(f.where, centerover)) + " cells";
      else {
        hyperpoint h1 = tC0(iso_inverse(View));
        hyperpoint h2 = tC0(iso_inverse(f.sView));
        ld d = hdist(h1, h2);
        if(d > 1e-3)
          dist = fts(d) + "au";
        else {
          transmatrix T = f.sView * iso_inverse(View);
          dist = fts(kz(acos_clamp(T[2][2]))) + "°/" + fts(kz(acos_clamp(T[1][1]))) + "°";
          }
        }
        
      dialog::addSelItem(f.title + " [" + dist + "]", fts(f.interval), dialog::list_fake_key++);
      dialog::add_action_push([&anim, id] { edit_step(anim, id); });
      id++;
      }
    aid++;
    }
  dialog::end_list();

  if(current_segment) {
    dialog::addItem("create a new position", 'a');
    dialog::add_action([] {
      current_segment->frames.push_back(frame{gentitle(), centerover, View, current_position, ortho_inverse(NLP), 1, 1, 0});
      });
    }

  dialog::addItem("create a new segment", 'b');
  dialog::add_action(start_segment);

  if(current_segment) {
    dialog::addItem("increase interval by 1", 's');
    dialog::add_key_action('s', [] {
      if(!current_segment->frames.empty())
        current_segment->frames.back().interval += 1;
      else
        current_segment->start_interval+=1;
      });
    }

  /* dialog::addItem("join a new segment", 'j');
  dialog::add_action(join_segment); */

  dialog::addBoolItem_action("view the labels", view_labels, 'l');
  dialog::addBoolItem("view the trace", view_trace, 't');
  dialog::add_action([] {
    view_trace = !view_trace;
    if(view_trace) generate_trace();
    });
  dialog::addBoolItem_action("side display", side, 'm');

  dialog::addItem("test the animation", 't');
  dialog::add_action([] {
    animate_on = false;
    last_time = HUGE_VAL;
    last_segment = -1;
    test_t = 0;
    dialog::editNumber(test_t, 0, 100, 0.1, 0, "enter the percentage", "");
    dialog::reaction = [] {
      handle_animation(test_t / 100);
      };
    dialog::extra_options = [] {
      dialog::addSelItem("current segment", its(last_segment), 'C');
      dialog::addSelItem("current front", fts(c_front_dist), 'F');
      dialog::addSelItem("current up", fts(c_up_dist), 'U');
      };
    });

  dialog::addBoolItem("view the crosshair", crosshair_size, 'x');
  dialog::add_action([] { crosshair_size = crosshair_size ? 0 : 10; });

  dialog::addBoolItem("run the animation", animate_on, 'r');
  dialog::add_action([] {
    animate_on = !animate_on;
    last_time = HUGE_VAL;
    });
  
  if(GDIM == 2) {
    dialog::addItem("centering", 'z');
    dialog::add_action_push(snap_to_center);
    }
    
  dialog::addHelp();
  dialog::add_action([] { gotoHelp(smooth_camera_help); });
  dialog::addBack();
  dialog::display();
  
  keyhandler = [] (int sym, int uni) {
    handlePanning(sym, uni);
    dialog::handleNavigation(sym, uni);
    if(doexiton(sym, uni)) popScreen();
    };
  }

void prepare_for_interpolation(hyperpoint& h) {
  if(gproduct) {
    h[3] = zlevel(h);
    ld t = exp(h[3]);
    h[0] /= t;
    h[1] /= t;
    h[2] /= t;
    }
  }

void after_interpolation(hyperpoint& h) {
  if(gproduct) {
    ld v = exp(h[3]) / sqrt(abs(intval(h, Hypc)));
    h[0] *= v;
    h[1] *= v;
    h[2] *= v;
    }
  else
    h = normalize(h);
  }

void handle_animation(ld t) {
  ld total_total = 0;
  
  vector<ld> totals;
  for(auto& anim: anims) {
    ld total = anim.start_interval;
    for(auto& f: anim.frames)
      total += f.interval;
    totals.push_back(total);
    total_total += total;
    }
    
  if(total_total == 0) return;

  t = frac(t);
  t *= total_total;
  int segment = 0;
  while(totals[segment] < t && segment < isize(totals)-1) t -= totals[segment++];

  auto& anim = anims[segment];

  if(t < last_time || segment != last_segment) {
    last_time = 0;
    last_segment = segment;
    current_segment = &anim;
    View = anim.start;
    last_view_comp = View;
    centerover = anim.start_cell;
    current_position = Id;
    last_view = View;
    last_centerover = centerover;
    }

  ld total = anim.start_interval;
  vector<ld> times;
  for(auto& f: anim.frames) {
    times.push_back(total);
    total += f.interval;
    }

  hyperpoint pts[3];
  
  for(int j=0; j<3; j++) {
    for(int i=0; i<4; i++) {
      vector<ld> values;
      for(auto& f: anim.frames) {
        hyperpoint h;
        if(j == 0)
          h = tC0(f.V);
        if(j == 1) {
          h = tC0(shift_object(f.V, f.ori, ztangent(f.front_distance), smGeodesic));
          }
        if(j == 2) {
          h = tC0(shift_object(f.V, f.ori, ctangent(1, -f.up_distance), smGeodesic));
          }
        prepare_for_interpolation(h);
        values.push_back(h[i]);
        }
      
      int n = isize(values);
      
      for(int ss=1; ss<=n-1; ss++) {
        for(int a=0; a<n-ss; a++) {
          // combining [a..a+(ss-1)] and [a+1..a+ss]
          if(times[a+ss] == times[a])
            values[a] = values[a] + (values[a+ss] - values[a]) * (t-times[a]);
          else
            values[a] = (values[a] * (times[a+ss] - t) + values[a+1] * (t - times[a])) / (times[a+ss] - times[a]);
          }
        values.pop_back();
        }
      
      pts[j][i] = values[0];
      }
    after_interpolation(pts[j]);
    }
  
  transmatrix V = View;
  set_view(pts[0], pts[1], pts[2]);
  c_front_dist = geo_dist(pts[0], pts[1]);
  c_up_dist = geo_dist(pts[0], pts[2]);

  transmatrix T = View * inverse(last_view_comp);
  last_view_comp = View;
  
  View = T * V;
  fixmatrix(View);
  
  if(invalid_matrix(View)) {
    println(hlog, "invalid_matrix ", View);
    println(hlog, pts[0]);
    println(hlog, pts[1]);
    println(hlog, pts[2]);
    println(hlog, "t = ", t);
    exit(1);
    }
  last_time = t;
  }

void handle_animation0() {
  if(!animate_on) return;
  handle_animation(ticks / anims::period);
  anims::moved();
  println(hlog, "at ", cview());
  }

void generate_trace() {
  last_time = HUGE_VAL;
  dynamicval<transmatrix> tN(NLP, NLP);
  dynamicval<transmatrix> tV(View, View);
  dynamicval<transmatrix> tC(current_display->which_copy, current_display->which_copy);
  dynamicval<cell*> tc(centerover, centerover);
  cell* cview = nullptr;
  vector<hyperpoint> at;
  traces.clear();
  auto send = [&] {
    if(cview && !at.empty()) traces[cview].push_back(at);
    cview = centerover;
    at.clear();
    };
  for(ld t=0; t<=1024; t ++) {
    handle_animation(t / 1024);
    if(cview != centerover) send();
    at.push_back(inverse(View) * C0);
    optimizeview();
    if(cview != centerover) {
      send();
      at.push_back(inverse(View) * C0);
      }
    }
  send();
  }

void hwrite(hstream& hs, const animation& anim) {
  hwrite(hs, anim.start_cell, anim.start, anim.start_interval, anim.frames);
  }

void hread(hstream& hs, animation& anim) {
  hread(hs, anim.start_cell, anim.start, anim.start_interval, anim.frames);
  }

void hwrite(hstream& hs, const frame& frame) {
  hwrite(hs, frame.title, frame.where, frame.sView, frame.V, frame.ori, frame.front_distance, frame.up_distance, frame.interval);
  }

void hread(hstream& hs, frame& frame) {
  hread(hs, frame.title, frame.where, frame.sView, frame.V, frame.ori, frame.front_distance, frame.up_distance, frame.interval);
  }

bool draw_labels(cell *c, const shiftmatrix& V) {
  if(view_labels) for(auto& p: labels[c])
    queuestr(V * rgpushxto0(p.first), .1, p.second, 0xFFFFFFFF, 1);
  if(view_trace) 
    for(auto& v: traces[c]) {
      for(auto p: v)
        curvepoint(p);
      queuecurve(V, 0xFFD500FF, 0, PPR::FLOOR);
      for(auto p: v)
        curvepoint(p);
      queuecurve(V, 0x80000080, 0, PPR::SUPERLINE);
      }
  return false;
  }

bool enabled;

void enable() { 
  if(enabled) return;
  enabled = true;
  rogueviz::cleanup.push_back([] { enabled = false; });
  rogueviz::rv_hook(hooks_preoptimize, 75, analyze_view_pre);
  rogueviz::rv_hook(hooks_postoptimize, 75, analyze_view_post);
  rogueviz::rv_hook(anims::hooks_anim, 100, handle_animation0);
  rogueviz::rv_hook(hooks_drawcell, 100, draw_labels);
  rogueviz::rv_hook(hooks_o_key, 190, [] (o_funcs& v) { v.push_back(named_dialog("smoothcam", show)); });
  rogueviz::rv_hook(mapstream::hooks_savemap, 100, [] (hstream& f) {
    f.write<int>(17);
    hwrite(f, anims);
    });
  anims.clear();
  start_segment();
  }

void enable_and_show() {
  showstartmenu = false;
  start_game();
  enable();
  pushScreen(show);
  }

auto hooks = arg::add3("-smoothcam", enable_and_show)
  + arg::add3("-smoothcam-on", [] { 
    enable_and_show(); 
    animate_on = true;
    last_time = HUGE_VAL;
    })
  + addHook(dialog::hooks_display_dialog, 100, [] () {
    if(current_screen_cfunction() == anims::show) {
      dialog::addItem(XLAT("smooth camera"), 'C'); 
      dialog::add_action(enable_and_show);
      }
    }) +
  + addHook(mapstream::hooks_loadmap, 100, [] (hstream& f, int id) {
    if(id == 17) {
      enable();
      hread(f, anims);
      current_segment = &anims.back();
      }
    });

}}
