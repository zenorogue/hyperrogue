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

vector<tuple<transmatrix, transmatrix, int, heptspin> > saved;

bool trailer_turn(int delta) {
  if(saving_positions)
    View = cpush(2, -delta/8000.) * cspin(0, 2, (mousex - current_display->xcenter) * delta / -1000000.) * cspin(1, 2, (mousey - current_display->ycenter) * delta / -1000000.) * View;
  return true;
  }

bool recording;

void trailer_frame() {
  // if(saving_positions || !isize(saved)) 
  if(!recording) queuechr(current_display->xcenter, current_display->ycenter, 0, 16, '+', 0xFFFFFFFF);

  if(saving_positions && ticks > next_pos_tick) {
    next_pos_tick += 66;
    saved.emplace_back(View, nisot::local_perspective, hybrid::current_view_level, viewctr);
    println(hlog, "frames = ", isize(saved));
    }
  }

bool keys_on = false;

ld stepdist = 0.02;

ld stepang = 0.01;

bool move_camera(transmatrix T) {
  for(int it=0; it<5; it++) {
    saved.emplace_back(View, nisot::local_perspective, hybrid::current_view_level, viewctr);
    println(hlog, "frames = ", isize(saved));
    shift_view(ztangent(-stepdist));
    rotate_view(T);
    }
  playermoved = false;
  return true;
  }

namespace solnihv {   pair<heptagon*,heptagon*> getcoord(heptagon *h); }

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
  heptagon *h2 = viewctr.at;
  if(h1->distance != h2->distance)
    println(hlog, "Z difference: ", h2->distance - h1->distance);
  else {
    auto c1 = solnihv::getcoord(h1);
    auto c2 = solnihv::getcoord(h2);
    println(hlog, "X difference: ", bdiff(c1.first, c2.first).get_str(10000));
    println(hlog, "Y difference: ", bdiff(c1.second, c2.second).get_str(10000));
    println(hlog, "X difference> ", bdiff(c2.first, c1.first).get_str(10000));
    println(hlog, "Y difference> ", bdiff(c2.second, c1.second).get_str(10000));
    }
  }

bool trailer_handleKey(int sym, int uni) {  

  if(sym == 'f') {
    keys_on = !keys_on;
    println(hlog, "keys_on = ", keys_on);
    return true;
    }
  
  // println(hlog, "cells_drawn = ", cells_drawn, " cells_generated = ", cells_generated);
  
  if(keys_on) {

    if(sym == 't') {
      if(!saved.empty()) {
        println(hlog, "frames = ", isize(saved));
        saved.pop_back();
        }
      if(!saved.empty()) {
        tie(View, nisot::local_perspective, hybrid::current_view_level, viewctr) = saved.back();
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

    
    if(sym == '1') { stepdist = 0; println(hlog, stepdist); return true; }
    if(sym == '2') { stepdist = 0.01; println(hlog, stepdist); return true; }
    if(sym == '3') { stepdist = 0.02; println(hlog, stepdist); return true; }
    if(sym == '4') { stepdist = 0.05; println(hlog, stepdist); return true; }

    if(sym == '6') { stepang = 0.001; println(hlog, stepang); return true; }
    if(sym == '7') { stepang = 0.003; println(hlog, stepang); return true; }
    if(sym == '8') { stepang = 0.01; println(hlog, stepang); return true; }
    if(sym == '9') { stepang = 0.03; println(hlog, stepang); return true; }
    if(sym == '0') { stepang = 0.1; println(hlog, stepang); return true; }
    
    if(sym == 'p') { get_b4_distance(); return true; }
  
    if(sym == SDLK_F4) {
      saving_positions = !saving_positions;
      next_pos_tick = ticks;
      println(hlog, "saving_positions = ", saving_positions);
      return true;
      }
  
    if(sym == 'b') {
      saved.pop_back();
      tie(View, nisot::local_perspective, hybrid::current_view_level, viewctr) = saved.back();
      return true;
      }
  
    if(sym == 'u' && isize(saved) > 40) {
      for(int i=0; i<30; i++) saved.pop_back();
      tie(View, nisot::local_perspective, hybrid::current_view_level, viewctr) = saved.back();
      return true;
      }
  
    if(sym == 'r') {
      recording = true;
      if(mouseaim_sensitivity) {
        mouseaim_sensitivity = 0;
        return true;
        }
      if(musicvolume) {
        musicvolume = 0;
        Mix_VolumeMusic(0);
        return true;
        }
      saving_positions = false;
//      vid.cells_drawn_limit = 1000000;      
      int i = 0;
      shot::take("anim/start.png");
      shot::transparent = false;
//      shot::shotx = 1920;
//      shot::shoty = 1080;
      shot::shot_aa = 2;
      solnihv::solrange_xy = 30;
      solnihv::solrange_z = 6;
      vid.cells_drawn_limit = 200000;
      vid.cells_generated_limit = 10000;
//    sightranges[geometry] = 4;
      static int lasti = 0;
      
      for(auto& p: saved) {
        // sightranges[geometry] = 4 + i * 2. / isize(saved);
        tie(View, nisot::local_perspective, hybrid::current_view_level, viewctr) = p;
        ticks = i * 1000 / 30;
        if(i < lasti) continue;
        
        system("mkdir -p devmods/manual/");
        
        shot::take(format("devmods/manual/%05d.png", i));
        println(hlog, "frame ", i);
        i++;
        }

      lasti = i;
      recording = false;
      }
    }
    
  return false;
  }

auto hook = 
  addHook(hooks_handleKey, 100, trailer_handleKey)
+ addHook(hooks_drawmap, 100, trailer_frame)
+ addHook(shmup::hooks_turn, 100, trailer_turn)
+ 0;

}
