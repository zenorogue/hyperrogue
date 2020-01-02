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

vector<tuple<transmatrix, transmatrix, cell*> > saved;

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

bool move_camera(transmatrix T) {
  for(int it=0; it<5; it++) {
    saved.emplace_back(View, current_display->local_perspective, centerover);
    println(hlog, "frames = ", isize(saved));
    shift_view(ztangent(-stepdist));
    rotate_view(T);
    }
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
  else {
    auto c1 = sn::getcoord(h1);
    auto c2 = sn::getcoord(h2);
    println(hlog, "X difference: ", bdiff(c1.first, c2.first).get_str(10000));
    println(hlog, "Y difference: ", bdiff(c1.second, c2.second).get_str(10000));
    println(hlog, "X difference> ", bdiff(c2.first, c1.first).get_str(10000));
    println(hlog, "Y difference> ", bdiff(c2.second, c1.second).get_str(10000));
    }
  }

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
        tie(View, current_display->local_perspective, centerover) = saved.back();
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
  
    if(sym == SDLK_F4) {
      saving_positions = !saving_positions;
      next_pos_tick = ticks;
      println(hlog, "saving_positions = ", saving_positions);
      return true;
      }
  
    if(sym == 'b' && isize(saved) > 1) {
      saved.pop_back();
      println(hlog, "back to ", isize(saved), " frames");
      tie(View, current_display->local_perspective, centerover) = saved.back();
      return true;
      }
  
    if(sym == 'u' && isize(saved) > 40) {
      for(int i=0; i<30; i++) saved.pop_back();
      println(hlog, "back to ", isize(saved), " frames");
      tie(View, current_display->local_perspective, centerover) = saved.back();
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
        auto at = crystal::get_coord(d->master);
        hwrite_raw(f, at);
        }
      println(hlog, "saved animation of ", isize(saved), " frames");
      }
  
    if(sym == ']') {
      fhstream f("devmods/manan-record.mar", "r");
      int siz;
      f.read<int>(siz);
      saved.resize(siz);
      for(int i=0; i<isize(saved); i++) {
        auto& [a, b, d] = saved[i];
        hread_raw(f, a);
        hread_raw(f, b);
        int tmp = 0; hread_raw(f, tmp);
        crystal::coord co;
        hread_raw(f, co);
        d = crystal::get_heptagon_at(co)->c7;
        }
      println(hlog, "loaded animation of ", isize(saved), " frames");
      }
  
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
//      shot::shotx = 1920;
//      shot::shoty = 1080;
      shot::shot_aa = 2;
      solnihv::solrange_xy = 30;
      solnihv::solrange_z = 6;
      vid.cells_drawn_limit = 200000;
      vid.cells_generated_limit = 10000;
//    sightranges[geometry] = 4;
      static int lasti = 0;
      
      system("mkdir -p devmods/manual/");
        
      for(auto& p: saved) {
        // sightranges[geometry] = 4 + i * 2. / isize(saved);
        tie(View, current_display->local_perspective, centerover) = p;
        ticks = i * 1000 / 30;
        // if(i % 10 != 0) {i++; continue; }

        if(true) {
          println(hlog, "recording frame ", i);
          shot::take(format("devmods/manual/%05d.png", i));
          }
        else 
          println(hlog, "skipping frame ", i);
        
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
