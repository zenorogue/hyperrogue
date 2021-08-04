#include "../hyper.h"

// This program generates the error table for Solv approxiations.

#define D3 1
#define D2 0

#if CAP_FIELD
namespace hr {

ld solerror(hyperpoint ok, hyperpoint chk) {
  return geo_dist(chk, ok);
  }

ld minz = -1e-9, maxz = 1e-9;

int max_iter = 999999;

bool isok;

hyperpoint iterative_solve(hyperpoint xp, hyperpoint candidate, ld minerr, bool debug = false) {

  transmatrix T = Id; T[0][1] = 8; T[2][2] = 5;
  
  auto f = [&] (hyperpoint x) { return nisot::numerical_exp(x); }; // T * x; };

  auto ver = f(candidate);
  ld err = solerror(xp, ver);
  auto at = candidate;
  
  ld eps = 1e-6;

  hyperpoint c[6];
  for(int a=0; a<3; a++) c[a] = point3(a==0, a==1, a==2);
  for(int a=0; a<3; a++) c[3+a] = point3(-(a==0), -(a==1), -(a==2));
  
  int iter = 0;
  
  while(err > minerr) { again:
    iter++; if(iter > max_iter) { isok = false; return at; }
    // cands.push_back(at);
    if(debug) println(hlog, "\n\nf(", at, "?) = ", ver, " (error ", err, ")");
    array<hyperpoint, 3> pnear;
    
    for(int a=0; a<3; a++) {
      auto x = at + c[a] * eps;
      if(debug) println(hlog, "f(", x, ") = ", f(x), " = y + ", f(x)-ver, "imp ", err - solerror(xp, f(x)) );
      auto y = at - c[a] * eps;
      if(debug) println(hlog, "f(", y, ") = ", f(y), " = y + ", f(y)-ver, "imp ", err - solerror(xp, f(y)) );
      pnear[a] = (f(x) - ver) / eps; //  (direct_exp(at + c[a] * eps, prec) - ver) / eps;
      }
    
    transmatrix U = Id;
    for(int a=0; a<3; a++) 
    for(int b=0; b<3; b++)
      U[a][b] = pnear[b][a];

    hyperpoint diff = (xp - ver);
    
    hyperpoint bonus = inverse(U) * diff;
    
    ld lbonus = hypot_d(3, bonus);
    
    if(lbonus > 0.1) bonus = bonus * 0.1 / hypot_d(3, bonus);
    
    if(false && lbonus > 1000) {
      int best = -1;
      ld besti = err;
      for(int a=0; a<6; a++) {
        auto x = at + c[a] * eps;
        auto nerr = solerror(xp, f(x));
        if(nerr < besti) best = a, besti = nerr;
        }
      if(best == -1) { 
        println(hlog, "local best"); 
        for(int a=0; a<1000000; a++) {
          auto x = at;
          for(int i=0; i<3; i++) x[i] += (hrand(1000000) - hrand(1000000)) * 1e-5;
          auto nerr = solerror(xp, f(x));
          if(nerr < besti) { println(hlog, "moved to ", x); at = x; goto again; }
          }
        break; 
        }
      bonus = c[best] * 1e-3;
      }
    
    int fixes = 0;
    
    if(debug) 
      println(hlog, "\nU = ", U, "\ndiff = ", diff, "\nbonus = ", bonus, " of ", lbonus, "\n");
    
    nextfix:
    hyperpoint next = at + bonus;
    hyperpoint nextver = f(next);
    ld nexterr = solerror(xp, nextver);
    if(debug) println(hlog, "f(", next, ") = ", nextver, ", imp = ", err - nexterr);
    
    if(nexterr < err) {
      // println(hlog, "reduced error ", err, " to ", nexterr);
      at = next;
      ver = nextver;
      err = nexterr;
      continue;
      }
    else {
      bonus /= 2;
      fixes++;
      if(fixes > 10) {
        if(err > 999) {
          for(ld s = 1; abs(s) > 1e-9; s *= 0.5)
          for(int k=0; k<27; k++) {
            int kk = k;
            next = at;
            for(int i=0; i<3; i++) { if(kk%3 == 1) next[i] += s; if(kk%3 == 2) next[i] -= s; kk /= 3; }
            // next = at + c[k] * s;
            nextver = f(next);
            nexterr = solerror(xp, nextver);
            // println(hlog, "f(", next, ") = ", nextver, ", error = ", nexterr);
            if(nexterr < err) { at = next; ver = nextver; err = nexterr; goto nextiter; }
            }
            println(hlog, "cannot improve error ", err);
            exit(1);
          }
        if(debug) println(hlog, "fixes = ", fixes, " : break");
        isok = false;
        return at;
        }
      goto nextfix;
      }
    
    nextiter: ;
    }

  if(debug) println(hlog, "\n\nsolution found: f(", at, ") = ", ver, " (error ", err, ")");
  
  isok = true;
  
  return at;
  }

EX void geodesic_step_euler(hyperpoint& at, hyperpoint& velocity) {
  auto acc = nisot::christoffel(at, velocity, velocity);
  at = at + velocity + acc / 2;
  velocity += acc;
  }

EX void geodesic_step_poor(hyperpoint& at, hyperpoint& velocity) {
  auto acc = nisot::christoffel(at, velocity, velocity);
  at = at + velocity;
  velocity += acc;
  }

EX void geodesic_step_midpoint(hyperpoint& at, hyperpoint& velocity) {

  // y(n+1) = y(n) + f(y(n) + 1/2 f(y(n)))
  
  auto acc = nisot::christoffel(at, velocity, velocity);
  auto at2 = at + velocity / 2;
  auto velocity2 = velocity + acc / 2;
      
  auto acc2 = nisot::christoffel(at2, velocity2, velocity2);
  
  at = at + velocity + acc2 / 2;
  
  velocity = velocity + acc2;
  }

auto& chr = nisot::get_acceleration;

EX bool invalid_any(const hyperpoint h) {
  return isnan(h[0]) || isnan(h[1]) || isnan(h[2]) || isinf(h[0]) || isinf(h[1]) || isinf(h[2]) ||
    abs(h[0]) > 1e20 || abs(h[1]) > 1e20 || abs(h[2]) > 1e20;
  }

EX void geodesic_step_rk4(hyperpoint& at, hyperpoint& vel) {
  auto acc1 = chr(at, vel);
  auto acc2 = chr(at + vel/2, vel + acc1/2);
  auto acc3 = chr(at + vel/2 + acc1/4, vel + acc2/2);
  auto acc4 = chr(at + vel + acc2/2, vel + acc3);
  
  at += vel + (acc1+acc2+acc3)/6;
  vel += (acc1+2*acc2+2*acc3+acc4)/6;
  }

template<class T> 
hyperpoint numerical_exp(hyperpoint v, int steps, const T& gstep) {
  hyperpoint at = point31(0, 0, 0);
  v /= steps;
  v[3] = 0;
  for(int i=0; i<steps; i++) {
    if(invalid_any(at)) return at;
    gstep(at, v);
    }    
  return at;
  }

ld x_to_scr(ld x) { return 150 + 100 * x; }
ld y_to_scr(ld x) { return 950 - log(x * 1e9) / log(10) * 80; }

hyperpoint pt(ld x, ld y) { return tC0(atscreenpos(x, y, 1)); };

map<pair<string, color_t>, map<double, double>> maxerr;

bool scatterplot;

void queueline1(hyperpoint a, hyperpoint b, color_t c) {
  queueline(shiftless(a), shiftless(b), c);
  }

void draw_graph() {
  vid.linewidth *= 2;
  queueline1(pt(0, 950), pt(1500, 950), 0xFF);
  queueline1(pt(150, 0), pt(150, 1000), 0xFF);
  
  vid.linewidth /= 2;
  
  for(int i=1; i<=9; i++) {
    queueline1(pt(x_to_scr(i), 950), pt(x_to_scr(i), 960), 0xFF);
    queuestr(x_to_scr(i), 980, 0, 60, its(i), 0, 0, 8);
    }
  
  for(int i=-8; i<=2; i++) {
    ld v = pow(10, i);
    queueline1(pt(140, y_to_scr(v)), pt(150, y_to_scr(v)), 0xFF);
    queuestr(70, y_to_scr(v), 0, 60, "1e"+its(i), 0, 0, 8);
    vid.linewidth /= 2;
    queueline1(pt(1100, y_to_scr(v)), pt(150, y_to_scr(v)), 0xFF);
    vid.linewidth *= 2;
    }
  
  vid.linewidth *= 2;
  for(auto& [id, graph]: maxerr) {
    auto& [name, col] = id;
    ld last = 1e-9;
    ld lastx = 0;
    for(auto [x, y]: graph) {
      if(scatterplot) {
        curvepoint(pt(x_to_scr(x)+2, y_to_scr(y)));
        curvepoint(pt(x_to_scr(x)-2, y_to_scr(y)));
        queuecurve(shiftless(Id), col, 0, PPR::LINE);
        curvepoint(pt(x_to_scr(x), y_to_scr(y)+2));
        curvepoint(pt(x_to_scr(x), y_to_scr(y)-2));
        queuecurve(shiftless(Id), col, 0, PPR::LINE);
        }
      if(y_to_scr(y) > y_to_scr(last) - x_to_scr(lastx) + x_to_scr(x)) continue;
      if(y > 100) y = 100;
      last = y;
      lastx = x;
      ld xx = x;
      if(xx > 9) xx = 9;
      if(!scatterplot) curvepoint(pt(x_to_scr(x), y_to_scr(y)));
      if(xx == 9) break;
      }
    if(!scatterplot) {
      queuestr(1100, y_to_scr(last), 0, 60, name, col >> 8, 0, 0);
      queuecurve(shiftless(Id), col, 0, PPR::LINE);
      }
    }
  vid.linewidth /= 2;
  
  drawqueue();
  }
  
void draw_sol_diffeq_graph() {
  }

void make_graph(string fname) {

  start_game();
  
  flat_model_enabler fme;

  shot::shotx = 1500;
  shot::shoty = 1000;
  shot::format = shot::screenshot_format::svg;
  svg::divby = 1;

  shot::take(fname, draw_graph);
  }  

void sol_diffeq_graph() {

  auto& s = sn::get_tabled();
  s.load();

  for(int x=0; x<s.PRECX-1; x++)
  for(int y=0; y<s.PRECY-1; y++)
  for(int z=0; z<s.PRECZ-1; z++) {
    println(hlog, tie(x,y,z));
    auto ax = sn::ix_to_x(x / (s.PRECX-1.));
    auto ay = sn::ix_to_x(y / (s.PRECY-1.));
    auto az = sn::iz_to_z(z / (s.PRECZ-1.));
    
    ld d = hypot(ax, hypot(ay, az));
    
    hyperpoint h = point31(ax, ay, az);
    hyperpoint v = inverse_exp(shiftless(h)); // , pfNO_INTERPOLATION);
    
    hyperpoint actual = numerical_exp(v, 2000, geodesic_step_rk4);
    if(invalid_any(actual)) continue;
    
    auto test = [&] (string name, color_t col, int iter, auto method) {
      hyperpoint res = numerical_exp(v, iter, method);
      if(invalid_any(res)) return;
      ld err = geo_dist(actual, res);
      ld& me = maxerr[{name, col}][d];
      me = max(me, err);
      };
    
    test("RK2 5", 0xB0E0B0FF, 5, geodesic_step_rk4);
    test("RK2 10", 0x8AD0A0FF, 10, geodesic_step_rk4);
    test(" ", 0x90E090FF, 20, geodesic_step_rk4);
    test("RK2 30", 0x80C080FF, 30, geodesic_step_rk4);
    test("RK4 100", 0x408040FF, 100, geodesic_step_rk4);
    test("RK4 300", 0x306030FF, 300, geodesic_step_rk4);
    test("RK4 1000", 0x204020FF, 1000, geodesic_step_rk4);
    test("mid 100", 0x8080C0FF, 100, geodesic_step_midpoint);
    test("mid 1000", 0x404080FF, 1000, geodesic_step_midpoint);
    }
  
  make_graph("sol-diff-graph.svg");
  }

void sol_numerics_out() {
  hyperpoint v = inverse_exp(shiftless(point31(2, 1, 0)));
  // point3(0.1, 0, 10);
  
  hyperpoint result = numerical_exp(v, 1000000, geodesic_step_rk4);
  
  println(hlog, "exp(", v, ") = ", result);

  for(int steps: {1, 2, 10, 20, 50, 100, 200, 500, 1000, 2000, 5000, 10000, 20000, 50000, 100000}) {
    shstream ss;
    auto experiment = [&] (string name, auto f) {
      print(ss, name, lalign(30, hdist0(numerical_exp(v, steps, f) - result)));
      };
    experiment(" P ", geodesic_step_poor);
    experiment(" E ", geodesic_step_euler);
    experiment(" M ", geodesic_step_midpoint);
    experiment(" R ", geodesic_step_rk4);
    println(hlog, " steps=", lalign(6, steps), ss.s);
    }
  
  println(hlog, "timing M");
  numerical_exp(v, 10000000, geodesic_step_midpoint);

  println(hlog, "timing R");
  numerical_exp(v, 10000000, geodesic_step_rk4);
  
  println(hlog, "ok");
  }

vector<ld> quantiles(vector<ld> data) {
  sort(data.begin(), data.end());
  if(isize(data) <= 20) return data;
  vector<ld> q;
  for(int i=0; i<=20; i++)
    q.push_back(data[(isize(data)-1)*i/20]);
  return q;
  }

auto smax(auto& tab, ld& i, ld x) { if(x) tab[i] = max(tab[i], x); }

ld median(vector<ld> v) {
  sort(v.begin(), v.end());
  return v[isize(v)/2];
  }
  
void sol_table_test() {

  // auto& length_good = maxerr[{"length/good", 0x408040FF}];
  // auto& angle_good = maxerr[{"angle/good", 0x404080FF}];

  // auto& length_good2 = maxerr[{"length/mid", 0x808040FF}];
  // auto& angle_good2 = maxerr[{"angle/mid", 0x804080FF}];

  // auto& length_bad = maxerr[{"length/bad", 0xC08040FF}];
  // auto& angle_bad = maxerr[{"angle/bad", 0xC04080FF}];
  
  // map<string, int> wins;

  auto& s = sn::get_tabled();
  s.load();
  
  map<double, double> maxerr;
  
  int good = 0, bad = 0;
  
  vector<ld> length_errors;
  vector<ld> angle_errors;
  
  vector<ld> split;
  
  vector<ld> lerrs[4][4][4], aerrs[4][4][4];
  
  for(int a: {16, 32, 48, 60})
    println(hlog, "xy_", a, " : ", sn::ix_to_x(a / (s.PRECX-1.)));

  for(int a: {16, 32, 48, 60})
    println(hlog, "z_", a, " : ", sn::iz_to_z(a / (s.PRECZ-1.)));
    
  
  FILE *g = fopen("solv-error-data.csv", "wt");
  
  for(ld x=0; x<s.PRECX-4; x+=.25)
  for(ld y=0; y<s.PRECY-4; y+=.25)
  for(ld z=0; z<s.PRECZ-4; z+=.25) {
  
    int xp = x * 4 / s.PRECX;
    int yp = y * 4 / s.PRECY;
    int zp = z * 4 / s.PRECZ;
  
    if(y == 0.5 && z== 0.5) println(hlog, x, " : ", good, " vs ", bad);
  
    int a0 = 0, b0 = 0;
    
    for(ld x1: {floor(x), ceil(x)})
    for(ld y1: {floor(y), ceil(y)})
    for(ld z1: {floor(z), ceil(z)}) {
      auto ax = sn::ix_to_x(x1 / (s.PRECX-1.));
      auto ay = sn::ix_to_x(y1 / (s.PRECY-1.));
      auto az = sn::iz_to_z(z1 / (s.PRECZ-1.));
  
      hyperpoint h = point31(ax, ay, az);
  
      hyperpoint v = inverse_exp(shiftless(h), pfNO_INTERPOLATION);
      
      if(v[2] > 0) a0++;
      else b0++;
      }
    
    bool bad_region = x > s.PRECX/2 && y > s.PRECY/2 && z < s.PRECZ/2;
    
    bool bad_break = bad_region && a0 && b0;
    
    auto ax = sn::ix_to_x(x / (s.PRECX-1.));
    auto ay = sn::ix_to_x(y / (s.PRECY-1.));
    auto az = sn::iz_to_z(z / (s.PRECZ-1.));

    hyperpoint h = point31(ax, ay, az);

    hyperpoint v = inverse_exp(shiftless(h), bad_break ? pfNO_INTERPOLATION : pNORMAL);
    
    // println(hlog, "looking for ", h);
    
    // println(hlog, "exp(", v, ") = ", nisot::numerical_exp(v));
    
    hyperpoint v1 = iterative_solve(h, v, 1e-9, false);

    // println(hlog, "exp(", v1, ") = ", nisot::numerical_exp(v1));
    
    hyperpoint h2 = nisot::numerical_exp(v1);
    
    if(sqhypot_d(3, h-h2) > 1e-6) {
      bad++; 
      continue;
      }
    else good++;
    
    ld dv  = hypot_d(3, v);
    ld dv1 = hypot_d(3, v1);
    
    ld lerr = abs(dv - dv1);
    ld aerr = asin(hypot_d(3, v^v1) / dv / dv1);
    
    ld d    = hypot_d(3, v1);
    
    if(dv == 0 || dv1 == 0) continue;
    
    if(invalid_any(v1) || invalid_any(v)) {
      println(hlog, "invalid");
      continue;
      }
    
    if(isnan(aerr)) println(hlog, "v = ", v, " v1 = ", v1, "aerr");
    
    else fprintf(g, "%lf;%lf;%lf;%lf;%lf;%lf;%lf;%lf;%d\n",
      x, y, z, 
      ax, ay, az, 
      lerr, aerr,
      bad_break
      );
    
    lerrs[zp][yp][xp].push_back(lerr);
    aerrs[zp][yp][xp].push_back(aerr);
    }
  
  fclose(g);

    
/*
    if(d >= 3 && d <= 3.1 && !bad_region) {
      println(hlog, tie(x,y,z), " : ", lerr);
      split.push_back(lerr);
      }
    
    if(bad_break) 
      smax(length_bad, d, lerr), 
      smax(angle_bad, d, aerr),
      0;
    else if(bad_region) 
      smax(length_good2, d, lerr), 
      smax(angle_good2, d, aerr),
      0;
    else
      smax(length_good, d, lerr),
      smax(angle_good, d, aerr),
      0;
    length_errors.push_back(lerr);
    
    ld cross = hypot_d(3, v^v1) / dv / dv1;
    
    angle_errors.push_back(cross);
    } 
  
  // println(hlog, quantiles(length_errors));
  println(hlog, quantiles(split)); */
  
  // for(auto p: angle_good) println(hlog, p);
  
  // make_graph("sol-la-errors.svg");
  
  FILE *f = fopen("devmods/graph.tex", "wt");


  fprintf(f, "\\documentclass{article}\n\\begin{document}\n");
  fprintf(f, "\\small\\setlength{\\tabcolsep}{3pt}\n");

  fprintf(f, "\\begin{tabular}{|c|cccc|cccc|cccc|cccc|}\n\\hline\n");
  for(int z=0; z<4; z++) {
    fprintf(f, " & ");
    fprintf(f, "\\multicolumn{4}{|c%s}{$z_%d$}", z==3?"|":"", z);
    }
  fprintf(f, "|\\\\\n");
  for(int z=0; z<4; z++) {
    for(int x=0; x<4; x++) {
      fprintf(f, " & ");
      fprintf(f, "$x_%d$", x);
      }
    }
  fprintf(f, "\\\\\n\\hline");
  for(int y=0; y<4; y++) {
    fprintf(f, "$y_%d$ ", y);
    for(int z=0; z<4; z++) {
      for(int x=0; x<4; x++) {
        fprintf(f, " & ");
        fprintf(f, "%4.2g", log10(median(lerrs[z][y][x])));
        }
      }
    fprintf(f, "\\\\\n");
    }
  fprintf(f, "\\hline \n");
  for(int y=0; y<4; y++) {
    fprintf(f, "$y_%d$ ", y);
    for(int z=0; z<4; z++) {
      for(int x=0; x<4; x++) {
        fprintf(f, " & ");
        fprintf(f, "%4.2g", log10(median(aerrs[z][y][x])));
        }
      }
    fprintf(f, "\\\\\n");
    }
  fprintf(f, "\\hline\n");
  fprintf(f, "\\end{tabular}\n");
  fprintf(f, "\\end{document}\n");
  fclose(f);
  }

int readArgs() {
  using namespace arg;
           
  if(0) ;

  else if(argis("-sol-diff-graph")) {
    sol_diffeq_graph();
    }

  else if(argis("-sol-tabletest")) {
    sol_table_test();
    }

  else if(argis("-sol-numerics")) {
    sol_numerics_out();
    }

  else return 1;
  return 0;
  }

auto nhook = addHook(hooks_args, 100, readArgs);

}
#endif
