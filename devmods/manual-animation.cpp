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

int next_pos_tick;

using frame = tuple<transmatrix, transmatrix, cell*>;
vector<frame> saved;

bool trailer_turn(int delta) {
  if(saving_positions)
    View = cpush(2, -delta/8000.) * cspin(0, 2, (mousex - current_display->xcenter) * delta / -1000000.) * cspin(1, 2, (mousey - current_display->ycenter) * delta / -1000000.) * View;
  return true;
  }

bool recording;

bool keys_on = false;

void trailer_frame() {
  // if(saving_positions || !isize(saved)) 
  if(!recording && keys_on) queuestr(current_display->xcenter, current_display->ycenter, 0, 16, "+", 0xFFFFFFFF);
  if(!recording && keys_on) queuestr(current_display->xcenter/2, current_display->ycenter, 0, 16, "+", 0xFFFFFFFF);
  if(!recording && keys_on) queuestr(current_display->xcenter*3/2, current_display->ycenter, 0, 16, "+", 0xFFFFFFFF);

  if(saving_positions && ticks > next_pos_tick) {
    next_pos_tick += 66;
    saved.emplace_back(View, current_display->local_perspective, centerover);
    println(hlog, "frames = ", isize(saved));
    }
  }

ld stepdist = 0.02;

ld stepang = 0.01;

ld spin_distance = 0;

bool spinning_around;

bool fixed_orientation;

transmatrix orientation_to_fix;

bool move_camera(transmatrix T) {
  for(int it=0; it<5; it++) {
    saved.emplace_back(View, current_display->local_perspective, centerover);
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

void recall(frame& f = saved.back()) {
  tie(View, current_display->local_perspective, centerover) = f;
  playermoved = false;
  }

void load_animation(string fname) {
  fhstream f(fname, "r");
  int siz;
  f.read<int>(siz);
  saved.resize(siz);
  for(int i=0; i<isize(saved); i++) {
    auto& [a, b, d] = saved[i];
    hread_raw(f, a);
    hread_raw(f, b);
    int tmp = 0; hread_raw(f, tmp);
    if(cryst) {
      crystal::coord co;
      hread_raw(f, co);
      d = crystal::get_heptagon_at(co)->c7;
      }
    if(nil) {
      nilv::mvec co;
      hread_raw(f, co);
      d = nilv::get_heptagon_at(co)->c7;
      }
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
    auto& [V, b, co] = saved[i];
    if(isnan(V[0][0])) {
      println(hlog, "nan at ", i, " @ ", co);
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
    auto& [V, b, co] = saved[i];
    auto& [Vl, bl, col] = saved[i-1];
    auto& [Vn, bn, con] = saved[i+1];
    forCellCM(c, co);
    hyperpoint hl = V * relm(col, co) * inverse(Vl) * C0;
    hyperpoint hn = V * relm(con, co) * inverse(Vn) * C0;
    hyperpoint hm = mid(hl, hn);
    if(isnan(hm[0])) {
      println(hlog, "Vl = ", Vl);
      println(hlog, "V  = ", V);
      println(hlog, "Vn = ", Vn);
      println(hlog, "cells ", col, co, con);
      println(hlog, "crl= ", relm(col, co));
      println(hlog, "crm= ", relm(con, co));
      continue;
      }
    total += hdist0(hm);
    V = gpushxto0(hm) * V;
    auto xhn = gpushxto0(hm) * hn;
    transmatrix T = cspin(0, 2, -M_PI/2) * spintox_good(cspin(0, 2, M_PI/2) * xhn) * cspin(0, 2, M_PI/2);
    V = T * V;
    // println(hlog, hn, " -> ", T * xhn);
    }
  println(hlog, "total = ", total);
  }

string mrec_file = "devmods/manual/%05d.png";

int mrec_x = 1920;
int mrec_y = 1080;
int mrec_cells = 24000;

int mrec_first = 0, mrec_last = 999999;

ld mrec_sightrange = 6;

int mrec_drawn = 10;
int mrec_generated = 1000;

bool trailer_handleKey(int sym, int uni) {  

  if(sym == 'f' && (cmode & sm::NORMAL)) {
    keys_on = !keys_on;
    println(hlog, "keys_on = ", keys_on);
    return true;
    }

  if(keys_on) {

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
                
    if(sym == 's') return move_camera(Id);
      
    if(sym == 'a') return move_camera(cspin(0, 2, stepang));
  
    if(sym == 'd') return move_camera(cspin(0, 2, -stepang));
  
    if(sym == 'q') return move_camera(cspin(0, 2, stepang) * cspin(1, 2, stepang));
    if(sym == 'w') return move_camera(cspin(1, 2, stepang));
    if(sym == 'e') return move_camera(cspin(0, 2, -stepang) * cspin(1, 2, stepang));
  
    if(sym == 'z') return move_camera(cspin(0, 2, stepang) * cspin(1, 2, -stepang));
    if(sym == 'x') return move_camera(cspin(1, 2, -stepang));
    if(sym == 'c') return move_camera(cspin(0, 2, -stepang) * cspin(1, 2, -stepang));

    
    if(sym == '1') { stepdist = 0; println(hlog, "dist = ", stepdist); return true; }
    if(sym == '2') { stepdist = 0.01; println(hlog, "dist = ", stepdist); return true; }
    if(sym == '3') { stepdist = 0.02; println(hlog, "dist = ", stepdist); return true; }
    if(sym == '4') { stepdist = 0.05; println(hlog, "dist = ", stepdist); return true; }

    if(sym == '6') { stepang = 0.001; println(hlog, "ang = ", stepang); return true; }
    if(sym == '7') { stepang = 0.003; println(hlog, "ang = ", stepang); return true; }
    if(sym == '8') { stepang = 0.01; println(hlog, "ang = ", stepang); return true; }
    if(sym == '9') { stepang = 0.03; println(hlog, "ang = ", stepang); return true; }
    if(sym == '0') { stepang = 0.1; println(hlog, "ang = ", stepang); return true; }
    
    if(sym == 'p') { get_b4_distance(); return true; }
    
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
    
    if(sym == '[') {
      fhstream f("devmods/manan-record.mar", "w");
      f.write<int>(isize(saved));
      for(int i=0; i<isize(saved); i++) {
        auto& [a, b, d] = saved[i];
        hwrite_raw(f, a);
        hwrite_raw(f, b);
        int tmp = 0; hwrite_raw(f, tmp);
        if(cryst) {
          auto at = crystal::get_coord(d->master);
          hwrite_raw(f, at);
          }
        else if(nil) {
          auto at = nilv::get_coord(d->master);
          hwrite_raw(f, at);
          }
        }
      println(hlog, "saved animation of ", isize(saved), " frames");
      }
  
    if(sym == ']') load_animation("devmods/manan-record.mar");
    
    if(sym == 'r') {
      recording = true;
      if(mouseaim_sensitivity) {
        mouseaim_sensitivity = 0;
        println(hlog, "disabled mouseaim");
        return true;
        }
      if(musicvolume) {
        println(hlog, "disabled music");
        musicvolume = 0;
        Mix_VolumeMusic(0);
        return true;
        }
      println(hlog, "starting recording");
      saving_positions = false;
//      vid.cells_drawn_limit = 1000000;      
      int i = 0;
      shot::take("anim/start.png");
      shot::transparent = false;

      shot::shotx = mrec_x;
      shot::shoty = mrec_y;
      
//       shot::shotx = 4096;
//      shot::shoty = 4096;

//      shot::shotx = 1920;
//      shot::shoty = 1080;
//      shot::shot_aa = 2;
      // solnihv::solrange_xy = 30;
      // solnihv::solrange_z = 6;

      sightranges[geometry] = mrec_sightrange;
      vid.cells_drawn_limit = mrec_drawn;
      vid.cells_generated_limit = mrec_generated;
      
      // vid.stereo_mode = sODS;
      // sightranges[geometry] = 7;
      // sightranges[geometry] += 1;
      // static int lasti = 68;
      
      ray::max_cells = mrec_cells;
      // 60..4352
      
      system("mkdir -p devmods/manual/");
        
      for(auto& p: saved) {
        // sightranges[geometry] = 4 + i * 2. / isize(saved);
        recall(p);
        // tie(View, current_display->local_perspective, centerover) = p;
        ticks = i * 1000 / 30;
        // if(i % 10 != 0) {i++; continue; }

        if(true) {
          println(hlog, "recording frame ", i);
          shot::take(format(mrec_file.c_str(), i));
          }
        else 
          println(hlog, "skipping frame ", i);
        
        i++;
        }

      // lasti = i;
      recording = false;
      }
    }
    
  return false;
  }

int readArgs() {
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
  else if(argis("-mrecxy")) {
    PHASEFROM(2);
    shift(); mrec_x = argi();
    shift(); mrec_y = argi();
    }
  else if(argis("-mrecq")) {
    PHASEFROM(2);
    shift(); mrec_cells = argi();
    }
  else if(argis("-mrecr")) {
    PHASEFROM(2);
    shift(); mrec_sightrange = argf();
    shift(); mrec_drawn = argi();
    shift(); mrec_generated = argi();
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
  else return 1;
  return 0;
  }


auto hook = 
  addHook(hooks_handleKey, 100, trailer_handleKey)
+ addHook(hooks_drawmap, 100, trailer_frame)
+ addHook(shmup::hooks_turn, 100, trailer_turn)
+ addHook(hooks_args, 100, readArgs)
+ 0;

}
