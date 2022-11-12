#include "rogueviz.h"

namespace hr {

namespace bricks {

struct brick {
  euc::coord co;
  color_t col;
  int walls;
  hyperpoint location;
  hpcshape shRotWall[6];
  };

vector<brick> bricks;

euc::coord centerer (-3, -1, -1);

euc::coord last_co;
hyperpoint last_loc;

transmatrix to_iso, from_iso;

hyperpoint to_rot(hyperpoint h) {
  if(nil) h[2] -= h[0] * h[1] / 2;
  return h;
  }

hyperpoint to_heis(hyperpoint h) {
  if(nil) h[2] += h[0] * h[1] / 2;
  return h;
  }

void make_wall(hpcshape& sh, vector<hyperpoint> hs) {
  cgi.bshape(sh, PPR::WALL);
  cgi.last->flags |= POLY_TRIANGLES | POLY_PRINTABLE;

  hyperpoint ctr = (hs[0] + hs[1] + hs[2] + hs[3]) / 4;
  hs.push_back(hs[0]);

  for(int a=0; a<4; a++) 
    texture_order([&] (ld x, ld y) {
      hyperpoint h = ctr + (hs[a+1]-ctr) * x + (hs[a]-ctr) * y;
      cgi.hpcpush(h);
      });
  cgi.finishshape();
  }

void place_brick(euc::coord co, color_t col = 0xFFD500, int which = -1) {
  bricks.emplace_back();
  brick& bri = bricks.back();
  bri.co = co;
  bri.col = col;
  bri.walls = which;
  auto rel = co - last_co;
  bri.location = eupush(last_loc) * to_heis(to_iso * (point31(rel[0], rel[1], rel[2])));

  hyperpoint sloc, floc;
  
  array<hyperpoint, 8> vertices;
  for(int a=0; a<8; a++) {
    hyperpoint h = C0;
    for(int c=0; c<3; c++)
      h[c] = (a & (1<<c)) ? .5 : -.5;
    h = to_iso * h;
    to_heis(h);
    vertices[a] = h;
    }

  euc::coord root = co, next = co;
  hyperpoint root_loc = bri.location, next_loc = bri.location;
  for(int c=0; c<3; c++) {
    while(root[c] % 6 != 0) 
      root[c]--, root_loc = eupush(root_loc) * to_heis(to_iso * (C0 - ctangent(c, 1)));
    while(next[c] % 6 != 0) 
      next[c]++, next_loc = eupush(next_loc) * to_heis(to_iso * (C0 + ctangent(c, 1)));
    }

  array<hyperpoint, 8> mvertices = vertices;
  
  println(hlog, co, root, next, root_loc, next_loc);

  for(int c=0; c<3; c++) 
    if(co[c] % 6)
      for(int a=0; a<8; a++) 
        if(a & (1<<c)) {
          int b = a ^ (1<<c);
          
          ld p0 = (gmod(co[c], 6) - 1) / 5.;
          ld p1 = (gmod(co[c], 6) - 0) / 5.;
          
          hyperpoint start = mvertices[a];
          hyperpoint end   = inverse(eupush(root_loc)) * eupush(next_loc) * mvertices[b];
          end = inverse(eupush(start)) * end;
          end = to_rot(end);
          vertices[b] = inverse(eupush(bri.location)) * eupush(root_loc) * eupush(start) * to_heis(lerp(C0, end, p0));
          vertices[a] = inverse(eupush(bri.location)) * eupush(root_loc) * eupush(start) * to_heis(lerp(C0, end, p1));

          /*
          vertices[b] = lerp(
            inverse(eupush(bri.location)) * eupush(root_loc) * mvertices[a], 
            inverse(eupush(bri.location)) * eupush(next_loc) * mvertices[b], 
            (gmod(co[c], 6) - 1) / 5.);

          vertices[a] = lerp(
            inverse(eupush(bri.location)) * eupush(root_loc) * mvertices[a], 
            inverse(eupush(bri.location)) * eupush(next_loc) * mvertices[b], 
            (gmod(co[c], 6) - 0) / 5.);
          */
            
          }  
  
  last_loc = bri.location;
  last_co = co;
  
  for(int di=0; di<6; di++) {
    vector<hyperpoint> hs;
    for(int a=0; a<4; a++) {
      int b = a;
      int id = 0;
      for(int c=0; c<3; c++) {
        if(c == di)
          id += (1<<c);
        else if(c != di-3) {
          if(b&1) id += (1<<c);
          b >>= 1;
          }
        }
      hs.push_back(vertices[id]);
      }
    
    swap(hs[2], hs[3]);
    make_wall(bri.shRotWall[di], hs);
    }
  }

void place_brick(int x, int y, int z, color_t col = 0xFFD500, int which = -1) {
  place_brick(euc::coord(x,y,z), col, which);
  }

bool walls_created = false;

const int darkval_e6[6] = {0,4,6,0,4,6};
  
void draw_ro() {

  shiftmatrix Zero = ggmatrix(currentmap->gamestart());

  if(true) {

    int bid = 0;
    for(auto& b: bricks) { bid++;
      transmatrix V = eupush(b.location);

      int which = b.walls;
      color_t wcol = b.col;
      int d = (wcol & 0xF0F0F0) >> 4;

      for(int di=0; di<6; di++) if((1<<di)&which) {
        auto &w1 = queuepoly(Zero * V, b.shRotWall[di], darkena(wcol - d * darkval_e6[di], 0, 0xFF));
        w1.tinf = &floor_texture_vertices[cgi.shFloor.id];
        ensure_vertex_number(*w1.tinf, w1.cnt);
        }
      }
    
    if(!among(pmodel, mdPerspective, mdGeodesic)) {
      shiftmatrix S = euclid ? cview() : shiftless(Id);
      vid.linewidth *= 3;
      for(int dir=0; dir<3; dir++) {
        curvepoint(C0 + ctangent(dir, -1));
        curvepoint(C0 + ctangent(dir, 1));
        queuecurve(S, 0xFFFFFFFF, 0, PPR::LINE);
        // queuestr(C0 + ctangent(dir, 1), 1, "X", 0xFFFFFFFF);
        }
      vid.linewidth /= 3;
      }
    }
  }

vector<hyperpoint> path;

void build(bool in_pair) {
  to_iso = cspin(1, 2, atan(1/sqrt(2))) * cspin(0, 2, 45._deg);
  from_iso = inverse(to_iso);
  
  last_co = euc::coord(0, 0, 3);
  last_loc = C0;
  place_brick(0, 0, 4, 0x00FF00, 0);
  place_brick(0, 0, 5, 0x00FF00, 0);
  
  place_brick(0, 0, 5, 0x00FF00, euclid ? -1 : 0);
  place_brick(0, 0, 4, 0xFFD500, 63 - 9 * 4);
  place_brick(0, 0, 3, 0xFFD500, 63 - 9 * 4);
  place_brick(0, 0, 2, 0xFFD500, 63 - 9 * 4);
  place_brick(0, 0, 1, 0xFFD500, 63 - 9 * 4);
  place_brick(0, 0, 0, 0xFFD500, 63 - 4 - 1);
  path.push_back(last_loc);
  if(nil && in_pair) {
    place_brick(0, 1, 0, 0xFF0000, 1 + 32);
    place_brick(0, 0, 0, 0xFFD500, 0);
    }
  place_brick(1, 0, 0, 0xFFD500, 63 - 9 * 1);
  place_brick(2, 0, 0, 0xFFD500, 63 - 9 * 1);
  place_brick(3, 0, 0, 0xFFD500, 63 - 9 * 1);
  place_brick(4, 0, 0, 0xFFD500, 63 - 9 * 1);
  place_brick(5, 0, 0, 0xFFD500, 63 - 9 * 1);
  place_brick(6, 0, 0, 0xFFD500, 63 - 8 - 2);
  path.push_back(last_loc);
  place_brick(6, 1, 0, 0xFFD500, 63 - 9 * 2);
  place_brick(6, 2, 0, 0xFFD500, 63 - 9 * 2);
  place_brick(6, 3, 0, 0xFFD500, 63 - 9 * 2);
  place_brick(6, 4, 0, 0xFFD500, 63 - 9 * 2);
  place_brick(6, 5, 0, 0xFFD500, 63 - 9 * 2);
  place_brick(6, 6, 0, 0xFFD500, 63 - 16 - 32);
  path.push_back(last_loc);
  place_brick(6, 6, -1, 0xFF0000, euclid ? 3 : 63 - 9 * 4);
  
  if(in_pair) {
    place_brick(6, 6, 0, 0xFFD500, 0);
    place_brick(5, 6, 0, 0xFFD500, 63 - 9*1);
    place_brick(4, 6, 0, 0xFFD500, 63 - 9*1);
    place_brick(3, 6, 0, 0xFFD500, 63 - 9*1);
    place_brick(2, 6, 0, 0xFFD500, 63 - 9*1);
    place_brick(1, 6, 0, 0xFFD500, 63 - 9*1);
    place_brick(0, 6, 0, 0xFFD500, 63 - 9*1);
    place_brick(0, 5, 0, 0xFFD500, 63 - 9*2);
    place_brick(0, 4, 0, 0xFFD500, 63 - 9*2);
    place_brick(0, 3, 0, 0xFFD500, 63 - 9*2);
    place_brick(0, 2, 0, 0xFFD500, 63 - 9*2);
    if(nil)
      place_brick(0, 1, 0, 0x00FF00, 63);
    else
      place_brick(0, 1, 0, 0xFF0000, 63);
    }
  
  ld midz = 0;
  for(auto b: bricks) midz += b.location[2];
  midz /= isize(bricks);
  for(auto& b: bricks) b.location[2] -= midz;
  
  cgi.extra_vertices();
  }

void build_net() {
  to_iso = cspin(1, 2, atan(1/sqrt(2))) * cspin(0, 2, 45._deg);
  from_iso = inverse(to_iso);
  
  last_co = euc::coord(0, 0, 0);
  last_loc = C0;
  
  place_brick(0, 0, 0, 0xFFD500, 0);  
  vector<brick> endbricks = { bricks[0] };
  
  for(int i=0; i<20; i++) {
    bool bad = false;
    for(int j=0; j<i; j++) if(sqhypot_d(3, endbricks[j].location - endbricks[i].location) < 1e-3) bad = true;
    if(bad) continue;
    for(int c=0; c<6; c++) {
      last_co = endbricks[i].co;
      last_loc = endbricks[i].location;
      euc::coord step(0, 0, 0);
      step[c%3] = (c<3?1:-1);
      for(int w=0; w<5; w++) {        
        place_brick(last_co + step, 0xFFD500, 63 - 9 * (1<<(c%3)));
        }
      place_brick(last_co + step, 0xFFD500, 0);
      endbricks.push_back(bricks.back());
      }
    }
  
  cgi.extra_vertices();
  }

void build_stair() {

  ld xx = .1;
  ld dx = 2 * xx;
  ld zz = dx * dx;
  ld hei = 2 * zz;

  hyperpoint at = point31(-2*dx,+2*dx,0);
  

  hyperpoint shift = xtangent(dx) + ztangent(zz);
  
  for(int dz=0; dz<4; dz++) {
    path.push_back(at);
    for(int step=0; step<4; step++) {
      bricks.emplace_back();
      brick& bri = bricks.back();
      bri.walls = -1;
      bri.col = 0xFFD500;
      bri.location = at;
            
      for(int di=0; di<6; di++) {
        vector<hyperpoint> hs(4);
        int dix = di;
        if(dix >= 3) dix--;
        if(dix == 3) dix = 2;
        else if(dix == 2) dix = 3;
        int diy = dix;
        dix -= dz;
        dix &= 3;

        if(di == 5) {
          hs[0] = point31(-xx, -xx, hei);
          hs[1] = point31(-xx, +xx, hei);
          hs[2] = point31(+xx, +xx, hei);
          hs[3] = point31(+xx, -xx, hei);
          }
        else if(di == 2) {
          hs[0] = point31(-xx, -xx, -hei);
          hs[1] = point31(-xx, +xx, -hei);
          hs[2] = point31(+xx, +xx, -hei);
          hs[3] = point31(+xx, -xx, -hei);
          }
        else if(dix == 3 || (dix == 2 && step == 0) || (dix == 1 && step > 0)) {
          transmatrix T = spin(90._deg*diy);
          hs[0] = T * point31(+xx, -xx, -hei);
          hs[1] = T * point31(+xx, +xx, -hei);
          hs[2] = T * point31(+xx, +xx, +hei);
          hs[3] = T * point31(+xx, -xx, +hei);
          }
        else if(dix == 0) {
          transmatrix T = spin(90._deg*diy);
          hs[0] = T * point31(+xx, -xx, -hei);
          hs[1] = T * point31(+xx, +xx, -hei);
          hs[2] = to_rot(eupush(C0 + shift) * to_heis(T * point31(-xx, +xx, -hei)));
          hs[3] = to_rot(eupush(C0 + shift) * to_heis(T * point31(-xx, -xx, -hei)));
          }
        else {
          transmatrix T = spin(90._deg*diy);
          hyperpoint lshift = step ? shift : spin270() * shift;
          hs[0] = to_rot(eupush(C0 - lshift) * to_heis(T * point31(-xx, +xx, hei)));
          hs[1] = to_rot(eupush(C0 - lshift) * to_heis(T * point31(-xx, -xx, hei)));
          hs[2] = T * point31(+xx, -xx, hei);
          hs[3] = T * point31(+xx, +xx, hei);
          }
        for(auto& h: hs) h = to_heis(h);
        make_wall(bri.shRotWall[di], hs);
        }      
      
      at = eupush(at) * (C0 + shift);
      }
    
    shift = spin90() * shift;
    }
  
  println(hlog, "path = ", path);
  
  cgi.extra_vertices();
  }

int animation;

hyperpoint interp(ld t) {
  int no = isize(path);
  int ti = t;
  t -= ti;
  ti %= no;
  
  hyperpoint prev = path[ti];
  hyperpoint next = path[(ti+1)%no];
  
  hyperpoint n = inverse(eupush(prev)) * next;
  n = to_rot(n);
  n = lerp(C0, n, t);
  
  return eupush(prev) * to_heis(n);
  }

void enable() {
  rogueviz::rv_hook(hooks_frame, 100, draw_ro);
  rogueviz::rv_hook(hooks_clearmemory, 40, [] () {
      bricks.clear();
      path.clear();
      });
  rogueviz::rv_hook(anims::hooks_anim, 100, [] { 

    if(!animation) return;
    ld t = ticks * 1. / anims::period;
    t *= isize(path);
    
    hyperpoint at = interp(t);
    
    if(animation == 1) {

      transmatrix T = View;
      transmatrix T2 = eupush( tC0(inverse(T)) );
      T = T * T2;
      
      View = T * inverse(eupush(at));
      }
    else if(animation == 2) {
      set_view(at, C0, at - ztangent(1));
      }
    else if(animation == 3) {
      set_view(at, interp(t + .25), at - ztangent(1));
      }
    
    centerover = currentmap->gamestart();
    
    anims::moved();
    });
  }

int args() {
  using namespace arg;
           
  if(0) ;

  else if(argis("-bstair")) {
    PHASEFROM(3);
    cgi.require_shapes();
    
    build_stair();

    enable();

    pconf.clip_min = -100;
    pconf.clip_max = 10;
    pconf.scale = .5;
    
    View = Id;
    }

  else if(argis("-brot")) {
    PHASEFROM(3);
    cgi.require_shapes();
    
    build(false);
    
    enable();

    pconf.clip_min = -100;
    pconf.clip_max = 10;
    pconf.scale = .5;
    
    View = Id;
    }

  else if(argis("-brot2")) {
    PHASEFROM(3);
    cgi.require_shapes();
    
    build(true);
    
    enable();

    pconf.clip_min = -100;
    pconf.clip_max = 10;
    pconf.scale = .5;
    
    View = Id;
    }

  else if(argis("-bnet")) {
    PHASEFROM(3);
    cgi.require_shapes();
    
    build_net();
    
    enable();

    pconf.clip_min = -100;
    pconf.clip_max = 10;
    pconf.scale = .1;
    
    View = Id;
    }

  else if(argis("-b-anim")) {
    shift(); animation = argi();
    }

  else return 1;
  return 0;
  }

auto hooks = addHook(hooks_args, 100, args);

}

}
