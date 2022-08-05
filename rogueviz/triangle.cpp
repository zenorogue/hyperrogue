#include "rogueviz.h"

// Impossible Triangle visualization

// used in: https://www.youtube.com/watch?v=YmFDd49WsrY

// settings:
// ./mymake -O3 rogueviz/triangle
// ./hyper -geo Nil -canvas x -tstep 8 -nilperiod 3 3 3

// also used in: https://youtu.be/RPL4-Ydviug

// ./hyper -geo Nil -nilwidth .9 -canvas x -tstep 1 -nilperiod 1 10 1 -triset 32 31 992

// network of triangles:

// ./hyper -geo Nil -canvas x -tri-net

namespace rogueviz {
namespace itri {

bool on = false;

bool net = false;

hyperpoint operator+(hyperpoint x) { return x; }

// do not change this
int shape = 1;

// how many cubes to subdivide edges to
int how = 8;

// how many cubes to draw (should be smaller than how because they are not really cubes and thus they get into each other)
int how1 = how - 1;

// precision: number of substeps to simulate (best if divisible by how and how1)
int isteps = 4 * 1024;

/* the generators correspond to: */

nilv::mvec a(1,0,0);
nilv::mvec b(0,1,0);
nilv::mvec c = (a * b).inverse();
  
vector<nilv::mvec> gens = { a, b, c, a.inverse(), b.inverse(), c.inverse() };

struct triangledata {
  hyperpoint at;
  bool computed;
  int tcolor;
  int id;
  // each color group (i.e., each face direction) is a different hpcshape
  triangledata(hyperpoint h) : at(h), computed(false) { tcolor = 0; id = 0; }
  };

struct trianglemaker {

  geometry_information *icgi;

  map<cell*, vector<triangledata> > tds;

  array<hpcshape, 6> ptriangle;

  array<hpcshape, 6> pcube;
  
  hyperpoint ds[4], uds[4], dmoves[6];
  
  ld scale;
  
  void init() {

    icgi = &cgi;

    ld rest = sqrt(8/9.);
    ld rex = sqrt(1 - 1/9. - pow(rest/2., 2));
  
    ds[0] = point3(rex, -rest/2, -1/3.);
    ds[1] = point3(0, rest, -1/3.);
    ds[2] = point3(-rex, -rest/2, -1/3.);
    ds[3] = point3(0, 0, +1);
    
    hyperpoint start = point31(0, 0, 0);
      
    double ca;
    
    // compute how to scale this in Nil so that everything fits
    
    ld amin = 0, amax = 1;
    
    for(int it=0; it<100; it++) {
      ld a = (amin + amax) / 2;
      ca = a;
      hyperpoint at = start;
      for(int d=0; d<3; d++) {
        for(int i=0; i<isteps; i++) {
          at = nisot::translate(at) * (start + ds[d] * a);
          }
        }
      
      println(hlog, "at = ", at, " for a = ", a, " sq = ", at[2] / a / a);
      if(at[2] > 0) {
        amax = a;
        }
      else {
        amin = a;
        }
      }
    
    // compute the shift between the cubes
  
    for(int d=0; d<3; d++) {
      hyperpoint at = start;
      for(int i=0; i<isteps/how; i++) {
        at = nisot::translate(at) * (start + ds[d] * ca);
        }
      uds[d] = (at - start) / 2.;
      }
    
    // println(hlog, "uds = ", uds);
  
    for(int a=0; a<3; a++) println(hlog, sqhypot_d(3, inverse_exp(shiftless(start + ds[a] * ca))));
  
    for(int a=0; a<3; a++) println(hlog, sqhypot_d(3, inverse_exp(shiftless(uds[a]))));
    
    // compute cube vertices
  
    hyperpoint verts[8];
    for(int a=0; a<8; a++) {
      verts[a] = start;
      for(int k=0; k<3; k++) 
        verts[a] += (a&(1<<k)) ? uds[k] : -uds[k];
      }
    
    // since Nil does not really have cubes, we need to move the vertices a bit so that it looks nicer
    
    // ugliness of the current vertices
    
    auto errf = [&] {
      ld res = 0;
      for(int s=0; s<8; s++)
      for(int t=0; t<3; t++) {
        if((s & (1<<t)) == 0) {
          int s1 = s | (1<<t);
          ld dix = sqhypot_d(3, inverse(nisot::translate(nisot::translate(start + 2*uds[t]) * verts[s])) * verts[s1]);
          // println(hlog, tie(s, t), "di = ", dix);
          res += dix * dix;
          }
        }
      return res;
      };
    
    // minimize the ugliness
  
    ld curerr = errf();
    println(hlog, "curerr = ", curerr);
    int att = 0;
    if(1) while(true) {
      int id = rand() % 8;
      int j = rand() % 3;
      ld eps = (rand() % 100 - rand() % 100) / 100000.;
      verts[id][j] += eps;
      ld nerr = errf();
      if(nerr < curerr) {
        curerr = nerr;
        println(hlog, "curerr = ", curerr, " # ", att);
        att = 0;
        }
      else {
        verts[id][j] -= eps;
        }
      att++;
      if(att > 50000) break;
      }
  
    for(int s=0; s<8; s++)
    for(int t=0; t<3; t++) {
      if((s & (1<<t)) == 0) {
        int s1 = s | (1<<t);
        ld dix = sqhypot_d(3, inverse(nisot::translate(nisot::translate(start + 2*uds[t]) * verts[s])) * verts[s1]);
        println(hlog, tie(s, t), "di = ", dix);
        }
      }
      
    scale = 1;

    for(int si=0; si<6; si++) {
      auto textured_square = [&] (auto f) {
        texture_order([&] (ld ix, ld iy) { f(.5 + ix/2 + iy/2, .5 + ix/2 - iy/2); });
        texture_order([&] (ld ix, ld iy) { f(.5 - ix/2 - iy/2, .5 - ix/2 + iy/2); });
        texture_order([&] (ld ix, ld iy) { f(.5 + ix/2 - iy/2, .5 - ix/2 - iy/2); });
        texture_order([&] (ld ix, ld iy) { f(.5 - ix/2 + iy/2, .5 + ix/2 + iy/2); });
        };
  
      auto cube_at = [&] (hyperpoint online) {
  
        auto sidesquare1 = [&] (hyperpoint a00, hyperpoint a01, hyperpoint a10, hyperpoint a11) {
          textured_square( [&] (ld ix, ld iy) {
            hyperpoint shf = lerp(lerp(a00, a01, ix), lerp(a10, a11, ix), iy);
            if(scale) shf = shf * scale - start * (scale-1);
            cgi.hpcpush(nisot::translate(online) * (shf));
            });
          };
            
        if(si == 0) sidesquare1(verts[0], verts[2], verts[4], verts[6]);
        if(si == 1) sidesquare1(verts[1], verts[3], verts[5], verts[7]);
        if(si == 2) sidesquare1(verts[0], verts[1], verts[4], verts[5]);
        if(si == 3) sidesquare1(verts[2], verts[3], verts[6], verts[7]);
        if(si == 4) sidesquare1(verts[0], verts[1], verts[2], verts[3]);
        if(si == 5) sidesquare1(verts[4], verts[5], verts[6], verts[7]);
        };
      
      scale = 2;
      cgi.bshape(pcube[si], PPR::WALL);
      cube_at(start);
      cgi.last->flags |= POLY_TRIANGLES;
      cgi.last->tinf = &floor_texture_vertices[0];
      cgi.last->texture_offset = 0;
      
      scale = 1;
      
      cgi.bshape(ptriangle[si], PPR::WALL);
      hyperpoint at = start;
      
      vector<hyperpoint> atx = {start};
      
      cube_at(start);
  
      for(int dx: {0, 1, 2}) {
        int d = dx;
        if(net) at = start;
        else cube_at(at);
        int d1 = (d+1) % 3;
        int d2 = (d+2) % 3;
  
        vector<hyperpoint> path(isteps+1);
        for(int i=0; i<isteps; i++) {
          path[i] = at;
          at = nisot::translate(at) * (start + ds[d] * ca * (dx >= 3 ? -1 : 1));
          }
        path[isteps] = at;
        
        auto &u = uds[d];
        auto &v = uds[d1];
        auto &w = uds[d2];
        
        auto sidewall = [&] (hyperpoint wide, hyperpoint shift) {
          textured_square( [&] (ld ix, ld iy) {
            hyperpoint online = path[int(ix * isteps + .1)];
            hyperpoint shf = lerp(u, -u, ix) + lerp(-wide, wide, iy) + shift;
            shf *= scale;
            cgi.hpcpush(nisot::translate(online) * (start + shf));
            });
          };
    
        auto sidesquare = [&] (hyperpoint wx, hyperpoint wy, hyperpoint shift, ld p) {
          textured_square( [&] (ld ix, ld iy) {
            hyperpoint online = path[int(p * isteps + .1)];
            hyperpoint shf = lerp(wx, -wx, ix) + lerp(wy, -wy, iy) + shift;
            shf *= scale;
            cgi.hpcpush(nisot::translate(online) * (start + shf));
            });
          };
        
        if(shape == 0) {
          if(si == d2*2) sidewall(v, w);
          if(si == d1*2) sidewall(w, v);
          if(si == d2*2+1) sidewall(v, -w);
          if(si == d1*2+1) sidewall(w, -v);
          
          if(si == d2*2) sidesquare(u, v, w, 0);
          if(si == d1*2) sidesquare(w, u, v, 0);
          if(si == d1*2+1) sidesquare(u, w, -v, 0);
          if(si == d*2+1) sidesquare(w, v, -u, 0);
          }
        
        if(shape == 1) for(int a=1; a<how1; a++) {
          ld c = a * 1. / how1;
          
          cube_at(path[int(c * isteps + .1)]);
          }
        
        dmoves[d] = at;        
        }
    
      cgi.last->flags |= POLY_TRIANGLES;
      cgi.last->tinf = &floor_texture_vertices[0];
      cgi.last->texture_offset = 0;
      cgi.finishshape();
      cgi.extra_vertices();
      }

    tds[cwt.at].emplace_back(start);
    dmoves[3] = inverse(nisot::translate(dmoves[0])) * C0;
    dmoves[4] = inverse(nisot::translate(dmoves[1])) * C0;
    dmoves[5] = inverse(nisot::translate(dmoves[2])) * C0;
    }

  void compute(triangledata &td, cell *c) {
    if(td.computed) return;
    td.computed = true;
    if(!net) return;

    hyperpoint at;
    
    for(int d=0; d<6; d++) {
      hyperpoint at = nisot::translate(td.at) * dmoves[d];
      cell *c0 = c;
      virtualRebase(c0, at);
      
      bool newat = true;
      for(auto& td: tds[c0]) {
        ld d = sqhypot_d(3, at - td.at);
        if(d < .01) {      
          if(d>1e-5) println(hlog, "d = ", d);
          newat = false;
          }
        }
      
      if(newat) {
        triangledata ntd = at;
        ntd.id = td.id + 1;
        tds[c0].push_back(ntd);
        tds[c0].back().tcolor = (td.tcolor + (d < 3 ? 1 : 2)) % 3;
        }
      }
    }
    
  };

trianglemaker *mkr;

void reset() {
  if(mkr) delete mkr;
  mkr = nullptr;
  }

// Magic Cube (aka Rubik Cube) colors

color_t magiccolors[6] = { 0xFFFF00FF, 0xFFFFFFFF, 0x0000FFFF, 0x00FF00FF, 0xFF0000FF, 0xFF8000FF};

#define CTO (isize(cnts))

  vector<int> cnts;
  vector<ld> coef;
  int valid_from;
  int tested_to;
  int coefficients_known;

bool verify(int id) {
  if(id < isize(coef)) return false;
  ld res = 0;
  for(int t=0; t<isize(coef); t++)
    res += coef[t] * cnts[id-t-1];
  return abs(res - cnts[id]) < .5;
  }


int valid(int v, int step) {
  if(step < 0) return 0;
  if(step+v+v+5 >= CTO) return 0;
  ld matrix[100][128];
  for(int i=0; i<v; i++)
  for(int j=0; j<v+1; j++)
    matrix[i][j] = cnts[step+i+j];
  
  for(int k=0; k<v; k++) {
    int nextrow = k;
    while(nextrow < v && std::abs(matrix[nextrow][k]) < 1e-6)
      nextrow++;
    if(nextrow == v) return 1;
    if(nextrow != k) {
      // printf("swap %d %d\n", k, nextrow);
      for(int l=0; l<=v; l++) swap(matrix[k][l], matrix[nextrow][l]);
      // display();
      }
    ld divv = 1. / matrix[k][k];
    for(int k1=k; k1<=v; k1++) matrix[k][k1] *= divv;
    // printf("divide %d\n", k);
    // display();
    for(int k1=k+1; k1<v; k1++) if(matrix[k1][k] != 0) {
      ld coef = -matrix[k1][k];
      for(int k2=k; k2<=v; k2++) matrix[k1][k2] += matrix[k][k2] * coef;
      }
    // printf("zeros below %d\n", k);
    // display();
    }
  
  for(int k=v-1; k>=0; k--)
  for(int l=k-1; l>=0; l--)
    if(matrix[l][k]) matrix[l][v] -= matrix[l][k] * matrix[k][v];
  
  coef.resize(v);
  for(int i=0; i<v; i++) coef[i] = matrix[v-1-i][v];
    
  println(hlog, "coef = ", coef);
  for(int t=step+v; t<step+v+v+5; t++) {
    println(hlog, "verify(", t, ") = ", verify(t));
    if(!verify(t)) return 2;
    }
  println(hlog, "got here");
  tested_to = step+v+v+5;
  while(tested_to < CTO) {
    if(!verify(tested_to)) return 2;
    tested_to++;
    }
  
  valid_from = step+v;
  return 3;
  }

void find_coefficients() {
  if(coefficients_known) return;
  for(int v=1; v<25; v++) 
  for(int step=0; step<1000; step++) { 
    int val = valid(v, step);
    if(val == 0) break;
    println(hlog, "v=", v, "step=", step, " val=", val);
    if(val == 3) { coefficients_known = 2; return; }
    }
  coefficients_known = 1;
  }

void growthrate() {

  cnts.resize(20);

  for(int a=0; a<CTO; a++) {
    int cnt = 0;
    map<cell*, int> howmany;
    for(auto& p: mkr->tds) cnt += (howmany[p.first] = isize(p.second));
    for(auto& p: howmany) for(int i=0; i<p.second; i++) {
      // println(hlog, p.first, mkr->tds[p.first][i].at, mkr->tds[p.first][i].computed);
      mkr->compute(mkr->tds[p.first][i], p.first);
      }
    println(hlog, "cnt = ", cnt, " / ", cnt / pow(1+a, 4));
    cnts[a] = cnt;
    if(a >= 4) println(hlog, "D4 = ", cnts[a-4] - 4 * cnts[a-3] + 6 * cnts[a-2] - 4 * cnts[a-1] + cnts[a]);
    println(hlog, "cnts = ", cnts);
    }
  
  auto cnt2 = cnts;
  for(int i=isize(cnt2)-1; i>=1; i--) cnt2[i] -= cnt2[i-1];
  println(hlog, "cnts dif = ", cnt2);
  
  // this was computed on integers, not using the program above

  cnts = 
  {1,6,24,80,186,368,644,1046,1574,2260,3128,4198,5482,7006,8788,10860,13228,15918,18948,22350,26130,30314,34926,39986,45506,51518,58034,65086,72680,80842,89596,98968,108964,119610,130930,142950,155676,169140,183354,198350,214140,230744,248186,266492,285668,305746,326744,348688,371584,395464,420346,446256,473206,501216,530310,560520,591846,624320,657960,692792,728828,766094,804608,844396,885470,927856,971572,1016650,1063090,1110924,1160176,1210866,1263006,1316622,1371732,1428368,1486536,1546262,1607564,1670474,1734998,1801162,1868990,1938502,2009710,2082646,2157322,2233770,2311996,2392026,2473884,2557596,2643168,2730626,2819994,2911298,3004544,3099764,3196970,3296194,3397448,3500752,3606130,3713608,3823192};
  
  println(hlog, "coefficients_known = ", coefficients_known);
  if(coefficients_known == 2) {
    string fmt = "a(d+" + its(isize(coef)) + ") = ";
    bool first = true;
    for(int i=0; i<isize(coef); i++) if(kz(coef[i])) {
      if(first && !kz(coef[i]-1)) ;
      else if(first) fmt += fts(coef[i]);
      else if(!kz(coef[i]-1)) fmt += " + ";
      else if(!kz(coef[i]+1)) fmt += " - ";
      else if(coef[i] > 0) fmt += " + " + fts(coef[i]);
      else if(coef[i] < 0) fmt += " - " + fts(-coef[i]);
      fmt += "a(d";
      if(i != isize(coef) - 1)
        fmt += "+" + its(isize(coef) - 1 - i);
      fmt += ")";
      first = false;
      }
    fmt += " (d>" + its(valid_from-1) + ")";
    println(hlog, fmt);
    }
  }

color_t tcolors[3] = { 0xFF0000FF, 0x00FF00FF, 0x0000FFFF };


bool draw_ptriangle(cell *c, const shiftmatrix& V) {

  if(!on) return false;
  
  if(mkr && mkr->icgi != &cgi) reset();
     
  if(!mkr) { mkr = new trianglemaker; mkr->init(); 
    // growthrate();
    }
  
  for(auto& td: mkr->tds[c]) {
    mkr->compute(td, c);
    for(int side=0; side<6; side++) {
      auto &s = queuepoly(V * nisot::translate(td.at), mkr->ptriangle[side], magiccolors[side]);
      ensure_vertex_number(*s.tinf, s.cnt);

      /* auto& s1 = queuepoly(V * nisot::translate(td.at), mkr->pcube[side], gradient(tcolors[td.tcolor], magiccolors[side], 0, .2, 1));
      ensure_vertex_number(*s1.tinf, s1.cnt); */
      }
    }

  return false;
  }

void slide_itri(tour::presmode mode, int id) {
  using namespace tour;
  setCanvas(mode, '0');
  
  if(mode == pmStart) {
    stop_game();
    set_geometry(gNil);
    tour::slide_backup(on, true);
    tour::slide_backup(net, id == 2 ? true : false);
    tour::on_restore(nilv::set_flags);
    if(id == 0)
      tour::slide_backup(nilv::nilperiod, make_array(3, 3, 3));
    if(id == 1) {
      tour::slide_backup(nilv::nilperiod, make_array(1, 10, 1));
      tour::slide_backup(nilv::nilwidth, .9);
      tour::slide_backup(how, 32);
      tour::slide_backup(how1, 31);
      tour::slide_backup(isteps, 992);
      }
    nilv::set_flags();
   /* do nothing for id == 2 */
    start_game();
    playermoved = false;
    tour::on_restore(reset);
    }     
  rogueviz::pres::non_game_slide_scroll(mode);
  }

string cap = "Impossible architecture in Nil/";

auto hchook = addHook(hooks_drawcell, 100, draw_ptriangle)

+ addHook(hooks_args, 100, [] {
  using namespace arg;
           
  if(0) ;
  else if(argis("-triset")) {
    shift(); how = argi();
    shift(); how1 = argi();
    shift(); isteps = argi();
    }
  else if(argis("-tri-net")) {
    on = true; net = true;
    }
  else if(argis("-tri-one")) {
    on = true; net = false;
    }
  else return 1;
  return 0;
  })

+ addHook_rvslides(166, [] (string s, vector<tour::slide>& v) {
  using namespace tour;
  if(s != "noniso") return;

  v.push_back(
    tour::slide{cap+"impossible triangle", 18, LEGAL::NONE | QUICKGEO, 
      "This form of impossible triangle was first created by Oscar Reutersvärd. "
      "It was later independently discovered by Lionel Penrose and Roger Penrose, and popularized by M. C. Escher.\n\n"
      "Move with mouse/arrows/PgUpDn. Press '5' to enable animation, 'o' to change ring size.",
   
  [] (presmode mode) {
  
    slide_itri(mode, 0);
    }});

  v.push_back(
    tour::slide{cap+"impossible triangle chainmail", 18, LEGAL::NONE | QUICKGEO, 
      "Here we try to link the impossible triangles into a construction reminiscent of a chainmail.",
   
  [] (presmode mode) {
  
    slide_itri(mode, 1);
    }});

  v.push_back(
    tour::slide{cap+"impossible triangle network", 18, LEGAL::NONE | QUICKGEO, 
      "It is not possible to reconstruct Escher's Waterfall in Nil geometry, because one of the three triangles there "
      "has opposite orientation. For this reason, that one triangle would not connect correctly. Penrose triangles "
      "in Nil would not create a planar structure, but rather a three-dimensional one. This slide shows the picture. "
      "Note that, while the structure is three-dimensional, the number of nodes connected in d steps grows as the "
      "fourth power of d.",
    
  [] (presmode mode) {
  
    slide_itri(mode, 2);
    }});
  });

}
}