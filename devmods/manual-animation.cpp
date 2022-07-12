#include "../hyper.h"

// A tool to create smooth manually controlled movement animations in 3D geometries.

// Press 'f' to start controlling

// 'qweasdzxc' will move the camera and additionally rotate the view a bit (except 's')

// 'b' goes back 1 frame, 'e' undoes 30 frames

// 'r' starts recording

// 1234 change movement speed (see console for details)
// 67890 change rotation speed (see console for details)

namespace hr {

bool saving_positions;

bool smooth_aim = true;

int next_pos_tick;

struct frame {
  transmatrix V;
  transmatrix lp;
  transmatrix msm;
  cell *co;
  int steps_to_change;
  int step_smoothing;
  ld stepdist;
  ld stepang;
  };

vector<frame> saved;

int collection;

bool recording;
bool keys_on = false;
ld stepdist = 0.02;
ld stepang = 0.01;
EX int step_smoothing = 1;
EX int steps_to_change;

void move_camera1(transmatrix T);

void recall(frame& f = saved.back()) {
  View = f.V;
  current_display->local_perspective = f.lp;
  stretch::mstretch_matrix = f.msm;
  centerover = f.co;
  steps_to_change = f.steps_to_change;
  step_smoothing = f.step_smoothing;
  stepdist = f.stepdist;
  stepang = f.stepang;
  playermoved = false;
  }

frame saveframe() {
  frame f;
  f.V = View;
  f.lp = current_display->local_perspective;
  f.msm = stretch::mstretch_matrix;
  f.co = centerover;
  f.steps_to_change = steps_to_change;
  f.step_smoothing = step_smoothing;
  f.stepdist = stepdist;
  f.stepang = stepang;
  playermoved = false;
  return f;
  }

void trailer_frame() {
  // if(saving_positions || !isize(saved)) 
  if(!recording && keys_on) queuestr(current_display->xcenter, current_display->ycenter, 0, 16, "+", 0xFFFFFFFF);
  if(!recording && keys_on) queuestr(current_display->xcenter/2, current_display->ycenter, 0, 16, "+", 0xFFFFFFFF);
  if(!recording && keys_on) queuestr(current_display->xcenter*3/2, current_display->ycenter, 0, 16, "+", 0xFFFFFFFF);
  if(!recording && keys_on) queuestr(mousex, mousey, 0, 16, "+", 0xFFFFFFFF);
  }

ld next_stepdist = stepdist;
ld next_stepang = stepang;

void apply_steps_to_change() {
  if(steps_to_change) {
    stepang = stepang + (next_stepang-stepang) / steps_to_change;
    stepdist = stepdist + (next_stepdist-stepdist) / steps_to_change;
    steps_to_change--;
    }
  }

bool trailer_turn(int delta) {
  if(saving_positions) {
    collection += delta * 60;
    while(collection > 1000) {
      apply_steps_to_change();
      collection -= 1000;
      View = cpush(2, -stepdist) * View;
            
      if(smooth_aim) {
        ld dx = (mousex - current_display->xcenter) * stepang / -50.;
        ld dy = (mousey - current_display->ycenter) * stepang / -50.;
        View = cspin(0, 2, dx) * cspin(1, 2, dy) * View;
        }
      optimizeview();
      saved.emplace_back(saveframe());
      if(isize(saved) % 100 == 0)
        println(hlog, "frames = ", isize(saved));
      }
    }
  return true;
  }

ld spin_distance = 0;

bool spinning_around;

bool fixed_orientation;

transmatrix orientation_to_fix;

string videofile;

void move_camera1(transmatrix T) {
  // println(hlog, "rayfix: saving frame ", isize(saved));
  // optimizeview();
  // ray::cast();
  optimizeview();
  saved.emplace_back(saveframe());
  if(spinning_around) {
    for(int s=0; s<100; s++) 
      shift_view(ztangent(-spin_distance/100));
    rotate_view(T);
    for(int s=0; s<100; s++) 
      shift_view(ztangent(spin_distance/100));
    }
  else {
    shift_view(ztangent(-stepdist));
    spin_distance += stepdist;
    rotate_view(T);
    }
  if(fixed_orientation) {
    println(hlog, "cat ", inverse(View) * C0);
    transmatrix iView = inverse(View);
    iView = nisot::translate(iView * C0) * orientation_to_fix;
    View = inverse(iView);
    println(hlog, "cat ", inverse(View) * C0);
    }
  }

bool move_camera(transmatrix T) {
  for(int it=0; it<5; it++) 
    move_camera1(T);
  println(hlog, "frames = ", isize(saved), " distance = ", spin_distance);
  playermoved = false;
  return true;
  }

template<class Type> bool move_camera_smoothchange(const Type& T) {
  for(int it=0; it<5; it++) {
    println(hlog, "steps_to_change = ", steps_to_change, " stepdist = ", stepdist);
    apply_steps_to_change();
    move_camera1(T());
    }
  println(hlog, "frames = ", isize(saved), " distance = ", spin_distance);
  playermoved = false;
  return true;
  }

bool spin_around(transmatrix T) {
  for(int it=0; it<5; it++) {
    }
  println(hlog, "frames = ", isize(saved), " spinning");
  playermoved = false;
  return true;
  }

namespace sn {   pair<heptagon*,heptagon*> getcoord(heptagon *h); }

bignum bdiff(heptagon *h1, heptagon *h2) {
  if(h1 == h2) return 0;
  auto p = bdiff(h1->move(3), h2->move(3));
  int d = h1->c.spin(3) - h2->c.spin(3);
  println(hlog, "d=", d, " p = ", p.get_str(10000));
  return p + p + bignum(d);
  }

#define BASE 10

/*
void o_addmul(bignum& b0, const bignum& b, int factor) {
  int K = isize(b.digits);
  if(K > isize(b0.digits)) b0.digits.resize(K);
  int carry = 0;
  for(int i=0; ; i++) {
    bool cnt = (i<K || (carry > 0 && carry < -1) || (carry == -1 && i < isize(b0.digits)));
    println(hlog, "cnt = ", cnt, " carry = ", carry
    if(!cnt) break;
    println(hlog, "i=", i, " carry start=", carry);
    if(i >= isize(b0.digits)) b0.digits.push_back(0);
    long long l = b0.digits[i];
    l += carry;
    if(i < K) l += b.digits[i] * factor;
    carry = 0;
    if(l >= BASE) carry = l / BASE;
    if(l < 0) carry = -(BASE-1-l) / BASE;
    l -= carry * BASE;
    b0.digits[i] = l;
    println(hlog, "carry=", carry);
    }
  println(hlog, "carry end=", carry);
  if(carry < 0) b0.digits.back() -= BASE;
  while(isize(b0.digits) && b0.digits.back() == 0) b0.digits.pop_back();
  } */

void get_b4_distance() {
  heptagon *h1 = currentmap->gamestart()->master;
  heptagon *h2 = centerover->master;
  if(h1->distance != h2->distance)
    println(hlog, "Z difference: ", h2->distance - h1->distance);
  else if(sn::in()) {
    auto c1 = sn::getcoord(h1);
    auto c2 = sn::getcoord(h2);
    println(hlog, "X difference: ", bdiff(c1.first, c2.first).get_str(10000));
    println(hlog, "Y difference: ", bdiff(c1.second, c2.second).get_str(10000));
    println(hlog, "X difference> ", bdiff(c2.first, c1.first).get_str(10000));
    println(hlog, "Y difference> ", bdiff(c2.second, c1.second).get_str(10000));
    }
  }

void load_animation(string fname) {
  fhstream f(fname, "r");
  int siz;
  f.read<int>(siz);
  saved.resize(siz);
  for(int i=0; i<isize(saved); i++) {
    auto& s = saved[i];
    hread_raw(f, s.V);
    hread_raw(f, s.lp);
    int tmp = 0; hread_raw(f, tmp);
    if(cryst) {
      crystal::coord co;
      hread_raw(f, co);
      s.co = crystal::get_heptagon_at(co)->c7;
      }
    else
    if(nil) {
      nilv::mvec co;
      hread_raw(f, co);
      s.co = nilv::get_heptagon_at(co)->c7;
      }
    else if(bounded) {    
      auto ac = currentmap->allcells();
      int i;
      hread_raw(f, i);
      s.co = ac[i];
      }
    else s.co = cwt.at;
    }
  println(hlog, "loaded animation of ", isize(saved), " frames");
  recall();
  }

EX transmatrix spintox_good(const hyperpoint& H) {
  if(GDIM == 2 || prod) return spintox(H); 
  double v = -atan2(H[2], H[1]);
  return cspin(2, 1, -v) * spintoc(cspin(2, 1, v) * H, 0, 1) *cspin(2, 1, v);
  // cspin(2, 1, v) * spintoc(cspin(2, 1, -v) * H, 0, 1);
  }

void denan() {
  for(int i=1; i<isize(saved)-1; i++) {
    auto& s = saved[i];
    if(isnan(s.V[0][0])) {
      println(hlog, "nan at ", i, " @ ", s.co);
      saved[i] = saved[i-1];
      }
    }
  }

transmatrix relm(cell *a, cell *b) {
  forCellIdEx(c, id, b) if(c == a) return currentmap->adj(b, id);
  forCellIdEx(c, id, b) 
    forCellIdEx(d, id2, c) if(d == a) return currentmap->adj(b, id) * currentmap->adj(c, id2);
  return currentmap->relative_matrix(a, b, C0);
  }  

void smoothen() {
  ld total = 0;
  
  for(int a=1; a<3; a++)
  for(int i=1; i<isize(saved)-1; i++) if((a^i)&1) {
    auto& s = saved[i];
    auto& sl = saved[i-1];
    auto& sn = saved[i+1];
    forCellCM(c, s.co);
    hyperpoint hl = s.V * relm(sl.co, s.co) * inverse(sl.V) * C0;
    hyperpoint hn = s.V * relm(sl.co, s.co) * inverse(sn.V) * C0;
    hyperpoint hm = mid(hl, hn);
    if(isnan(hm[0])) {
      println(hlog, "Vl = ", sl.V);
      println(hlog, "V  = ", s.V);
      println(hlog, "Vn = ", sn.V);
      println(hlog, "cells ", sl.co, s.co, sn.co);
      println(hlog, "crl= ", relm(sl.co, s.co));
      println(hlog, "crm= ", relm(sn.co, s.co));
      continue;
      }
    total += hdist0(hm);
    s.V = gpushxto0(hm) * s.V;
    auto xhn = gpushxto0(hm) * hn;
    transmatrix T = cspin(0, 2, -M_PI/2) * spintox_good(cspin(0, 2, M_PI/2) * xhn) * cspin(0, 2, M_PI/2);
    s.V = T * s.V;
    // println(hlog, hn, " -> ", T * xhn);
    }
  println(hlog, "total = ", total);
  }

string mrec_file = "devmods/manual/%05d.png";

int mrec_fps = 60;

int mrec_first = 0, mrec_last = 999999;

int mrec_first_opt = 0, mrec_last_opt = 0;

void set_stepdist(ld x) {
  println(hlog, "stepdist = ", x);
  next_stepdist = x;
  steps_to_change = step_smoothing;
  }

void set_stepang(ld x) {
  println(hlog, "stepang = ", x);
  next_stepang = x;
  steps_to_change = step_smoothing;
  }

void do_recording() {
  recording = true;
  if(mouseaim_sensitivity) {
    mouseaim_sensitivity = 0;
    println(hlog, "disabled mouseaim");
    return;
    }
  if(musicvolume) {
    println(hlog, "disabled music");
    musicvolume = 0;
    Mix_VolumeMusic(0);
    return;
    }
  
  dynamicval dp(arg::pos, arg::pos);
  dynamicval vs(vid, vid);
  for(arg::pos=mrec_first_opt; arg::pos < mrec_last_opt; arg::pos++) {
    int r = callhandlers(1, hooks_args);
    switch (r) {
      case 0: arg::lshift(); break;
      case 1: 
        printf("Unknown option: %s\n", arg::argcs()); break;
      case 2:
        printf("Error\n"); break;
      }
    }

  println(hlog, "starting recording");
  shot::take("anim/start.png");
  saving_positions = false;
//      vid.cells_drawn_limit = 1000000;      
  int i = 0;

  system("mkdir -p devmods/manual/");
  
  auto f = [&] {
    for(auto& p: saved) {
      recall(p);
      println(hlog, "rayfix: render ", i);
      ticks = i * 1000 / mrec_fps;
      
      if(i >= mrec_first && i < mrec_last) {
        string s = format(mrec_file.c_str(), i);
        println(hlog, "recording frame ", i, "/", isize(saved), " to ", s);
        shot::take(s);
        }
      else 
        println(hlog, "skipping frame ", i);
      
      i++;
      }
    return true;
    };
    
  if(videofile != "") {
    anims::record_video(videofile, f);
    }
  else 
    f();

  // lasti = i;
  recording = false;
  }
  
void show_man_options() {
  gamescreen(2);
  dialog::init("manual animation");
  
  dialog::addSelItem("stepdist", fts(next_stepdist), 'd');
  dialog::add_action([] {
    dialog::editNumber(next_stepdist, 0, 1, 0.001, 0.02, "stepdist", "stepdist");
    dialog::reaction = [] { set_stepdist(next_stepdist); };
    });

  dialog::addSelItem("stepang", fts(next_stepang), 'a');
  dialog::add_action([] {
    dialog::editNumber(next_stepang, 0, 1, 0.001, 0.02, "stepang", "stepang");
    dialog::reaction = [] { set_stepang(next_stepang); };
    });

  dialog::addItem("b4 distance", 'b');
  dialog::add_action(get_b4_distance);

  dialog::addSelItem("step smoothing", its(step_smoothing), 's');
  dialog::add_action([] {
    dialog::editNumber(step_smoothing, 1, 30, 1, 1, "step_smoothing", "step_smoothing");
    dialog::reaction = [] { steps_to_change = step_smoothing; };
    });

  dialog::addItem("save path", '[');
  dialog::add_action([] {
    fhstream f("devmods/manan-record.mar", "w");
    f.write<int>(isize(saved));
    for(int i=0; i<isize(saved); i++) {
      auto& s = saved[i];
      hwrite_raw(f, s.V);
      hwrite_raw(f, s.lp);
      int tmp = 0; hwrite_raw(f, tmp);
      if(cryst) {
        auto at = crystal::get_coord(s.co->master);
        hwrite_raw(f, at);
        }
      else if(nil) {
        auto at = nilv::get_coord(s.co->master);
        hwrite_raw(f, at);
        }
      else if(bounded) {
        auto ac = currentmap->allcells();
        for(int i=0; i<isize(ac); i++) if(ac[i] == s.co)
          hwrite_raw(f, i);
        }
      }
    println(hlog, "saved animation of ", isize(saved), " frames");
    });

  dialog::addItem("load path", ']');
  dialog::add_action([] {
    load_animation("devmods/manan-record.mar");
    });
  
  dialog::addItem("do record", 'r');
  dialog::add_action(do_recording);

  dialog::addItem("clear", 'c');
  dialog::add_action([] { saved.clear(); });

  dialog::display();      
  }
  
bool trailer_handleKey(int sym, int uni) {  

  if(sym == 'f' && (cmode & sm::NORMAL)) {
    keys_on = !keys_on;
    println(hlog, "keys_on = ", keys_on);
    return true;
    }

  if(keys_on && (cmode & sm::NORMAL)) {

    if(sym == ',') { vid.fov *= 1.1; mouseaim_sensitivity *= 1.1; println(hlog, "fov = ", vid.fov, " sens = ", mouseaim_sensitivity); }
    if(sym == '.') { vid.fov /= 1.1; mouseaim_sensitivity /= 1.1; println(hlog, "fov = ", vid.fov, " sens = ", mouseaim_sensitivity); }

    if(sym == 't') {
      if(!saved.empty()) {
        println(hlog, "frames = ", isize(saved));
        saved.pop_back();
        }
      if(!saved.empty()) {
        recall();
        }
      return true;
      }
  
    /* if(sym == 'e') {
      dialog::editNumber(stepdist, 0, 1, 0.1, 0.1, "", "");
      dialog::scaleLog();
      } */
                
    if(sym == 's') return move_camera_smoothchange([&] { return Id; });
      
    if(sym == 'a') return move_camera_smoothchange([&] { return cspin(0, 2, stepang); });
  
    if(sym == 'd') return move_camera_smoothchange([&] { return cspin(0, 2, -stepang); });
  
    if(sym == 'q') return move_camera_smoothchange([&] { return cspin(0, 2, stepang) * cspin(1, 2, stepang); });
    if(sym == 'w') return move_camera_smoothchange([&] { return cspin(1, 2, stepang); });
    if(sym == 'e') return move_camera_smoothchange([&] { return cspin(0, 2, -stepang) * cspin(1, 2, stepang); });
  
    if(sym == 'z') return move_camera_smoothchange([&] { return cspin(0, 2, stepang) * cspin(1, 2, -stepang); });
    if(sym == 'x') return move_camera_smoothchange([&] { return cspin(1, 2, -stepang); });
    if(sym == 'c') return move_camera_smoothchange([&] { return cspin(0, 2, -stepang) * cspin(1, 2, -stepang); });

    
    if(sym == '1') { set_stepdist(0); return true; }
    if(sym == '2') { set_stepdist(0.005); return true; }
    if(sym == '3') { set_stepdist(0.02); return true; }
    if(sym == '5') { set_stepang(0); return true; }

    if(sym == '6') { set_stepang(0.001); return true; }
    if(sym == '7') { set_stepang(0.003); return true; }
    if(sym == '8') { set_stepang(0.01); return true; }
    if(sym == '9') { set_stepang(0.03); return true; }
    if(sym == '0') { set_stepang(0.1); return true; }
    
    if(sym == 'o') { 
      println(hlog, "spin_distance = ", spin_distance, " reset to 0, i to spin");
      spin_distance = 0;
      return true;
      }
    
    if(sym == 'j') {      
      fixed_orientation = !fixed_orientation;
      orientation_to_fix = inverse(inverse(nisot::translate(inverse(View) * C0)) * inverse(View));


      transmatrix iView = inverse(View);
      orientation_to_fix = inverse(nisot::translate(iView * C0)) * iView;

      println(hlog, "fixed_orientation = ", orientation_to_fix);
      return true;
      }
  
    if(sym == 'i') { 
      spinning_around = !spinning_around;
      if(spinning_around) println(hlog, "spinning mode");
      else println(hlog, "tank mode");
      }
  
    if(sym == SDLK_F4) {
      saving_positions = !saving_positions;
      next_pos_tick = ticks;
      println(hlog, "saving_positions = ", saving_positions);
      mouseaim_sensitivity = 0;
      return true;
      }
  
    if(sym == 'b' && isize(saved) > 1) {
      saved.pop_back();
      println(hlog, "back to ", isize(saved), " frames");
      recall();
      return true;
      }
  
    if(sym == 'u') {
      for(int i=0; i<30; i++) if(isize(saved)) saved.pop_back();
      println(hlog, "back to ", isize(saved), " frames");
      if(isize(saved)) recall();
      return true;
      }
    
    if(sym == 'r') pushScreen(show_man_options);
    }
    
  return false;
  }

int ma_readArgs() {
  using namespace arg;
           
  if(0) ;
  else if(argis("-loada")) {
    start_game();
    shift(); load_animation(args());
    }
  else if(argis("-smoothen")) {
    PHASEFROM(2); 
    shift(); int nsm = argi();
    denan();
    for(int i=0; i<nsm; i++) smoothen();
    }
  else if(argis("-mrecf")) {
    PHASEFROM(2);
    shift(); mrec_first = argi();
    }
  else if(argis("-mrecl")) {
    PHASEFROM(2);
    shift(); mrec_last = argi();
    }
  else if(argis("-mrec-to")) {
    PHASEFROM(2);
    shift(); mrec_file = args();
    }
  else if(argis("-mrecv")) {
    PHASEFROM(2);
    shift(); videofile = args();
    }
  else if(argis("-mrec-fps")) {
    PHASEFROM(2);
    shift(); mrec_fps = argi();    
    }
  else if(argis("-shot-half")) {
    shot::shotx /= 2;
    shot::shoty /= 2;
    }
  else if(argis("-mrec-opt")) {
    PHASEFROM(2);
    shift(); string cap = args();
    shift(); mrec_first_opt = pos;
    while(args() != cap) shift();
    mrec_last_opt = pos;
    }
  else return 1;
  return 0;
  }


auto mahook = 
  addHook(hooks_handleKey, 100, trailer_handleKey)
+ addHook(hooks_drawmap, 100, trailer_frame)
+ addHook(shmup::hooks_turn, 100, trailer_turn)
+ addHook(hooks_args, 100, ma_readArgs)
+ 0;

}
