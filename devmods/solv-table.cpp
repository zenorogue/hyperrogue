// This generates the inverse geodesics tables.

// Usage: 

// [executable] -geo sol -build -write solv-geodesics.dat
//              -geo 3:2 -build -write shyp-geodesics.dat
//              -geo 3:1/2 -build -write ssol-geodesics.dat
//              -exit

// Loading generated tables and visualization: 
// [executable] /hyper -fsol [filename] -geo sol -visualize filename-%03d.png

// You can also do -geo [...] -build to build and test the table
// without writing it.

// By default this generates 64x64x64 tables.
// Add e.g. '-dim 128 128 128' before -write to generate
// a more/less precise table.

// # ./hyper -rk-steps 100 -geo Sol -iz-list -sn-unittest -build -write solv-geodesics-a.dat -visualize devmods/san1/solva-%04d.png -improve -write solv-geodesics.dat -visualize devmods/san1/solvb-%04d.png
// # ./hyper -dim 32 32 32 -geo 3:1/2 -iz-list -sn-unittest -build -write ssol-geodesics-a.dat -visualize devmods/san1/ssola-%04d.png -improve -write ssol-geodesics.dat -visualize devmods/san1/ssolb-%04d.png
// # ./hyper -dim 32 32 32 -geo 3:2 -iz-list -sn-unittest -build -write shyp-geodesics.dat -visualize devmods/san1/shypa-%04d.png

#include "../hyper.h"

#include <thread>
#include <mutex>

namespace hr {

transmatrix parabolic1(ld u);

namespace sn {

template<class T> void parallelize(int threads, int Nmin, int Nmax, T action) {
  std::vector<std::thread> v;
  for(int k=0; k<threads; k++)
    v.emplace_back([&,k] () { 
      for(int i=Nmin+k; i < Nmax; i += threads) action(k, i);
      });
  for(std::thread& t:v) t.join();
  }

ld solerror(hyperpoint ok, hyperpoint chk) {
  auto zok  = point3( x_to_ix(ok[0]), x_to_ix(ok[1]), z_to_iz(ok[2]) );
  auto zchk = point3( x_to_ix(chk[0]), x_to_ix(chk[1]), z_to_iz(chk[2]) );
  return hypot_d(3, zok - zchk);
  }

int max_iter = 999999;

hyperpoint fail(.1, .2, .3, .4);

hyperpoint iterative_solve(hyperpoint xp, hyperpoint candidate, ld minerr, bool debug = false) {

  transmatrix T = Id; T[0][1] = 8; T[2][2] = 5;
  
  auto f = [&] (hyperpoint x) { return nisot::numerical_exp(x); }; // T * x; };

  auto ver = f(candidate);
  ld err = solerror(xp, ver);
  auto at = candidate;
  
  ld eps = 1e-6;

  hyperpoint c[3];  
  for(int a=0; a<3; a++) c[a] = point3(a==0, a==1, a==2);
  
  int iter = 0;
  
  while(err > minerr) {

    iter++; if(iter > max_iter) return fail;

    if(debug) println(hlog, "\n\nf(", at, "?) = ", ver, " (error ", err, ")");
    array<hyperpoint, 3> pnear;
    for(int a=0; a<3; a++) {
      auto x = at + c[a] * eps;
      if(debug) println(hlog, "f(", x, ") = ", f(x), " = y + ", f(x)-ver );
      pnear[a] = (f(x) - ver) / eps; //  (direct_exp(at + c[a] * eps, prec) - ver) / eps;
      }
    
    transmatrix U = Id;
    for(int a=0; a<3; a++) 
    for(int b=0; b<3; b++)
      U[a][b] = pnear[b][a];

    hyperpoint diff = (xp - ver);
    
    hyperpoint bonus = inverse(U) * diff;
    
    if(hypot_d(3, bonus) > 0.1) bonus = bonus * 0.1 / hypot_d(3, bonus);
    
    int fixes = 0;
    
    if(debug) 
      println(hlog, "\nU = ", U, "\ndiff = ", diff, "\nbonus = ", bonus, "\n");
    
    nextfix:
    hyperpoint next = at + bonus;
    hyperpoint nextver = f(next);
    ld nexterr = solerror(xp, nextver);
    if(debug) println(hlog, "f(", next, ") = ", nextver, ", error = ", nexterr);
    
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
        return fail;
        }
      goto nextfix;
      }
    
    nextiter: ;
    }
  
  // println(hlog, "#it = ", iter);
  
  return at;
  }

using ptlow = compressed_point;

ptlow operator +(ptlow a, ptlow b) { return make_array<float>(a[0]+b[0], a[1]+b[1], a[2]+b[2]); }
ptlow operator -(ptlow a, ptlow b) { return make_array<float>(a[0]-b[0], a[1]-b[1], a[2]-b[2]); }
ptlow operator *(ptlow a, ld x) { return make_array<float>(a[0]*x, a[1]*x, a[2]*x); }

void fint(FILE *f, int x) { fwrite(&x, sizeof(x), 1, f); }
void ffloat(FILE *f, float x) { fwrite(&x, sizeof(x), 1, f); }

void write_table(sn::tabled_inverses& tab, const char *fname) {
  FILE *f = fopen(fname, "wb");
  fint(f, tab.PRECX);
  fint(f, tab.PRECY);
  fint(f, tab.PRECZ);
  fwrite(&tab.tab[0], sizeof(ptlow) * tab.PRECX * tab.PRECY * tab.PRECZ, 1, f);
  fclose(f);
  }

void alloc_table(sn::tabled_inverses& tab, int X, int Y, int Z) {
  tab.PRECX = X;
  tab.PRECY = Y;
  tab.PRECZ = Z;
  tab.tab.resize(X*Y*Z);
  }

ld ptd(ptlow p) {
  return p[0]*p[0] + p[1]*p[1] + p[2] * p[2];
  }

void fix_boundaries(sn::tabled_inverses& tab, int last_x, int last_y, int last_z) {
  int PRECX = tab.PRECX;
  int PRECY = tab.PRECY;
  int PRECZ = tab.PRECZ;
  for(int x=0; x<last_x; x++)
  for(int y=0; y<last_y; y++) {
    for(int z=last_z; z<PRECZ; z++)
      tab.get_int(x,y,z) = tab.get_int(x,y,z-1) * 2 - tab.get_int(x,y,z-2);
    if(nih)
      tab.get_int(x,y,0) = tab.get_int(x,y,1) * 2 - tab.get_int(x,y,2);
    }
  
  for(int x=0; x<last_x; x++)
  for(int y=last_y; y<PRECY; y++)
  for(int z=0; z<PRECZ; z++)
    tab.get_int(x,y,z) = tab.get_int(x,y-1,z) * 2 - tab.get_int(x,y-2,z);
  
  for(int x=last_x; x<PRECX; x++)
  for(int y=0; y<PRECY; y++)
  for(int z=0; z<PRECZ; z++)
    tab.get_int(x,y,z) = tab.get_int(x-1,y,z) * 2 - tab.get_int(x-2,y,z);
  }

void build_sols(int PRECX, int PRECY, int PRECZ) {
  std::mutex file_mutex;
  ld max_err = 0;
  auto& tab = sn::get_tabled();
  alloc_table(tab, PRECX, PRECY, PRECZ);
  int last_x = PRECX-1, last_y = PRECY-1, last_z = PRECZ-1;
  auto act = [&] (int tid, int iz) {
    if((nih && iz == 0) || iz == PRECZ-1) return;
  
    auto solve_at = [&] (int ix, int iy) {
      // if(ix != 30 || iy <= 50 || iz) return;
      ld x = ix_to_x(ix / (PRECX-1.));
      ld y = ix_to_x(iy / (PRECY-1.));
      ld z = iz_to_z(iz / (PRECZ-1.));
      
      
      auto v = hyperpoint ({x,y,z,1});
      
      vector<hyperpoint> candidates;
      hyperpoint cand;
      
      candidates.push_back(point3(0,0,0)); 
      
      // sort(candidates.begin(), candidates.end(), [&] (hyperpoint a, hyperpoint b) { return solerror(v, direct_exp(a, prec)) > solerror(v, direct_exp(b, prec)); });
      
      // cand_best = candidates.back();
      
      vector<hyperpoint> solved_candidates;
      
      for(auto c: candidates)  {
        auto solt = iterative_solve(v, c, 1e-6, false);
        solved_candidates.push_back(solt);
        if(solerror(v, nisot::numerical_exp(solt)) < 1e-9) break;
        }

      sort(solved_candidates.begin(), solved_candidates.end(), [&] (hyperpoint a, hyperpoint b) { return solerror(v, nisot::numerical_exp(a)) > solerror(v, nisot::numerical_exp(b)); });
      
      cand = solved_candidates.back();

      auto xerr = solerror(v, nisot::numerical_exp(cand));
      
      if(cand == fail) {
        println(hlog, format("[%2d %2d %2d] FAIL", iz, iy, ix));
        }
      
      else if(xerr > 1e-3) {
        println(hlog, format("[%2d %2d %2d] ", iz, iy, ix));
        println(hlog, "f(?) = ", v);
        println(hlog, "f(", cand, ") = ", nisot::numerical_exp(cand));
        println(hlog, "error = ", xerr);
        println(hlog, "canned = ", compress(azeq_to_table(cand)));
        max_err = xerr;
        return;
        }

      auto& so = tab.get_int(ix, iy, iz);
      
      so = compress(azeq_to_table(cand));

      for(int z=0; z<3; z++) if(isnan(so[z]) || isinf(so[z])) {
        println(hlog, cand, "canned to ", so);
        exit(4);
        }
      };
    
    for(int it=0; it<max(last_x, last_y); it++) {
      for(int a=0; a<it; a++) {
        if(it < last_x && a < last_y) solve_at(it, a);
        if(a < last_x && it < last_y) solve_at(a, it);
        }
      if(it < last_x && it < last_y) solve_at(it, it);
      std::lock_guard<std::mutex> fm(file_mutex);
      if(0) println(hlog, format("%2d: %2d", iz, it));
      }
    };

  parallelize(PRECZ, 0, PRECZ, act);
  
  fix_boundaries(tab, last_x, last_y, last_z);
  }

std::mutex file_mutex_global;

bool deb = false;

hyperpoint find_optimal_geodesic(hyperpoint res) {
  auto p0 = point3(0, 0, 0);
  hyperpoint h = iterative_solve(res, p0, 100, 1e-9);
  if(h == fail) return fail;
  
  ld d = hypot_d(3, h);
  
  auto solve = [&] (hyperpoint m, pair<hyperpoint, hyperpoint> last) {  
    hyperpoint t = // inverse_exp(m, iTable, false);
      iterative_solve(m, last.first, 100, 1e-9);
    hyperpoint u = // inverse_exp(inverse(nisot::translate(m)) * res, iTable, false);
      iterative_solve(inverse(nisot::translate(m)) * res, last.second, 100, 1e-6);
    return make_pair(t, u);
    };
    
  auto quality = [&] (pair<hyperpoint, hyperpoint> p) {
    return hypot_d(3, p.first) + hypot_d(3, p.second);
    };
  
  auto attempt = [&] (hyperpoint mid) {
    auto p = solve(mid, {p0, p0});
    ld qd = quality(p);
    
    if(true) {     
      // println(hlog, "there is something better: ", qd, " vs ", d);
      
      bool found;
      bool failed = false;
      
      auto tryit = [&] (hyperpoint h) {
        auto p2 = solve(h, p);
        auto qd2 = quality(p2);
        if(p2.first == fail || p2.second == fail) failed = true;
        else if(qd2 < qd) {          
          qd = qd2, p = p2, mid = h;
          found = true;
          return true;
          }
        return false;
        };
      
      ld delta = 1e-3;
      
      /*
      auto q_x = quality(solve(mid + point3(delta, 0, 0), p)) - qd;
      auto q_xx = quality(solve(mid + point3(delta+delta, 0, 0), p)) - qd - 2 * q_x;
      auto q_y = quality(solve(mid + point3(0, delta, 0), p)) - qd;
      auto q_yy = quality(solve(mid + point3(0, delta+delta, 0), p)) - qd - 2 * q_y;
      auto q_z = quality(solve(mid + point3(0, 0, delta), p)) - qd;
      auto q_zz = quality(solve(mid + point3(0, 0, delta+delta), p)) - qd - 2 * q_z;
      auto q_xy = quality(solve(mid + point3(delta, delta, 0), p)) - qd - q_x - q_y;
      auto q_xz = quality(solve(mid + point3(delta, 0, delta), p)) - qd - q_x - q_z;
      auto q_yz = quality(solve(mid + point3(0, delta, delta), p)) - qd - q_y - q_z;
      
      transmatrix T = build_matrix(
        hyperpoint(q_xx, q_xy, q_xz, 0),
        hyperpoint(q_xy, q_yy, q_yz, 0),
        hyperpoint(q_xz, q_yz, q_zz, 0),
        hyperpoint(0, 0, 0, 1)
        );
      
      hyperpoint q = hyperpoint(q_x, q_y, q_z, 0);
      */

      int itera = 0;
      
      while(true) {
        itera++;
        if(itera % 1000 == 0) { 
          std::lock_guard<std::mutex> fm(file_mutex_global);
          println(hlog, "itera = ", itera); 
          if(itera >= 5000) return;
          }
        auto q_v = quality(solve(mid + point3(delta, -delta, 0), p)) - qd;
        auto q_vv = quality(solve(mid + point3(delta+delta, -delta-delta, 0), p)) - qd - 2 * q_v;
        auto q_z = quality(solve(mid + point3(0, 0, delta), p)) - qd;
        auto q_zz = quality(solve(mid + point3(0, 0, delta+delta), p)) - qd - 2 * q_z;
        auto q_vz = quality(solve(mid + point3(delta, -delta, delta), p)) - qd - q_v - q_z;
        
        ld d = q_vv * q_zz - q_vz * q_vz;
        if(d == 0 || isnan(d) || isinf(d)) {
          std::lock_guard<std::mutex> fm(file_mutex_global);
          println(hlog, "bad matrix in iteration #", itera);
          println(hlog, "p = ", p, " mid = ", mid);
          println(hlog, solve(mid, p));
          return;
          }
        
        transmatrix T = build_matrix(
          hyperpoint(q_vv, 0, q_vz, 0),
          hyperpoint(0, 1, 0, 0),
          hyperpoint(q_vz, 0, q_zz, 0),
          hyperpoint(0, 0, 0, 1)
          );
  
        hyperpoint q = hyperpoint(q_v, 0, q_z, 0);
  
        hyperpoint res = inverse(T) * -q;
        
        // println(hlog, "res = ", res);
  
        // println(hlog, "check = ", q + T * res);
        
        res[1] = -res[0];
  
        res = res * delta;
        
        res /= 10.;
        
        if(tryit(mid + res)) continue;
        if(tryit(mid + res/2)) continue;
        if(tryit(mid + res/4)) continue;

        break;
        }

      // q_x + q_xx * x + q_xy * y + q_xz * z == 0
      // q + Txyz == 0
      
      int it = 0;      
      
      ld qd0 = qd;
      
      if(false) while(delta > 1e-6) {
        it++;
        // if(it % 1000 == 0) println(hlog, "iterations ", it);
        if(it > 1000) return;
        if(failed) return;
        found = false;
        while(tryit(mid + point3(delta, -delta, 0)));
        while(tryit(mid + point3(-delta, +delta, 0)));
        while(tryit(mid + point3(0, 0, delta)));
        while(tryit(mid + point3(0, 0, -delta)));
        // while(tryit(mid + point3(delta, delta, 0)));
        // while(tryit(mid + point3(-delta, -delta, 0)));
        if(found) println(hlog, "improved further from ", qd0, " to ", qd);
        if(!found) delta /= 2;
        }
      
      max_iter = 1000;
      auto h1 = iterative_solve(res, p.first * quality(p) / hypot_d(3, p.first), 100, 1e-6);

      if(deb) println(hlog, "h1 returns ", h1, " of length ", hypot_d(3, h1), " and error ", hypot_d(3, nisot::numerical_exp(h1) - res));

      if(h1 == fail) return;
      
      auto d1 = hypot_d(3, h1);
      if(d1 < d) h = h1, d = d1;
      }
    };

  hyperpoint old = h;
  attempt(point31(res[0], 0, res[2]/2));
  attempt(point31(0, res[1], res[2]/2));

  std::lock_guard<std::mutex> fm(file_mutex_global);
  if(h != old && hypot_d(3, h) < hypot_d(3, old) - 1e-5)
    println(hlog, "new = ", h, " vs old = ", old, " length ", hypot_d(3, h), " vs ", hypot_d(3, old));
  else if(deb)
    println(hlog, " not improved: ", old);
  
  return h;
  }

void fix_bugs(sn::tabled_inverses& tab) {
  auto bug = compress(azeq_to_table(fail));
  for(int iz=0; iz<tab.PRECZ; iz++) 
    for(int iy=0; iy<tab.PRECY; iy++)
      for(int ix=0; ix<tab.PRECX; ix++)  {
        if(tab.get_int(ix, iy, iz) == bug)
          for(int a=0; a<3; a++)
          tab.get_int(ix, iy, iz)[a] = (tab.get_int(ix-1, iy, iz)[a]*2 - tab.get_int(ix-2, iy, iz)[a]);
        }
  }

void visualize_table(sn::tabled_inverses& tab, const string& s) {
  renderbuffer rb(tab.PRECX, tab.PRECY, false);
  rb.make_surface();
  
  for(int iz=0; iz<tab.PRECZ; iz++) {
    println(hlog, "iz=", iz);
    for(int iy=0; iy<tab.PRECY; iy++)
      for(int ix=0; ix<tab.PRECX; ix++)  {
        auto& p = qpixel(rb.srf, ix, iy);
        if(ix == 52 && iy >= 30 && iy <= 40 && iz == 15)
          println(hlog, "A ", tie(ix,iy,iz), " : ", tab.get_int(ix, iy, iz));
        // println(hlog, ix, ", ", iy);
        p = 0xFFFFFFFF;
        for(int i=0; i<3; i++)
          part(p, i) = 0x80 + 0x70 * tab.get_int(ix, iy, iz)[i];
        }
    SDL_SavePNG(rb.srf, format(s.c_str(), iz).c_str());
    }
  }

void improve(sn::tabled_inverses& tab) {
  int PRECX = tab.PRECX;
  int PRECY = tab.PRECY;
  int PRECZ = tab.PRECZ;
  int last_x = PRECX-1, last_y = PRECY-1, last_z = PRECZ-1;

  max_iter = 1000;
  auto act = [&] (int tid, int iz) {
    if((nih && iz == 0) || iz == PRECZ-1) return;
    for(int iy=0; iy<last_y; iy++)
    for(int ix=0; ix<last_x; ix++) {
      if(ix < 32 || iy < 32) continue;
      if(deb) { if(ix < 50 || ix > 54 || iy != 46 || iz != 6) continue; }
      if(deb) println(hlog, tie(ix, iy, iz), ":");
      ld x = ix_to_x(ix / (PRECX-1.));
      ld y = ix_to_x(iy / (PRECY-1.));
      ld z = iz_to_z(iz / (PRECZ-1.));
      hyperpoint p = point31(x, y, z);
      // hyperpoint h1 = inverse_exp(p, iTable, false);
      hyperpoint h2 = find_optimal_geodesic(p);
      
      std::lock_guard<std::mutex> fm(file_mutex_global);
      if(ix == last_x-1) println(hlog, "solved ", tie(ix, iy, iz));
      
      if(h2 != fail) {
        auto& so = tab.get_int(ix, iy, iz);
        so = compress(azeq_to_table(h2)); 
        }
      }
    };
  max_iter = 1000000;
  
  parallelize(PRECZ, 0, PRECZ, act);
  if(deb) exit(7);


  fix_boundaries(tab, last_x, last_y, last_z);
  }

int dimX=64, dimY=64, dimZ=64;

EX hyperpoint recompress(hyperpoint h) { return decompress(compress(h)); }

int readArgs() {
  using namespace arg;
           
  if(0) ;
  else if(argis("-dim")) {
    PHASEFROM(2); 
    shift(); dimX = argi();
    shift(); dimY = argi();
    shift(); dimZ = argi();
    }
  else if(argis("-build")) {
    PHASEFROM(2); 
    build_sols(dimX, dimY, dimZ);
    }
  else if(argis("-load-old")) {
    sn::get_tabled().load();
    }
  else if(argis("-improve")) {
    sn::get_tabled().load();
    improve(sn::get_tabled());
    }
  else if(argis("-write")) {
    shift();
    write_table(sn::get_tabled(), argcs());
    }
  else if(argis("-fix-bugs")) {
    sn::get_tabled().load();
    fix_bugs(sn::get_tabled());
    }
  else if(argis("-iz-list")) {
    sn::get_tabled().load();    
    for(int iz=0; iz<dimZ-1; iz++)
      println(hlog, "iz=", iz, " z=", iz_to_z(iz / (dimZ-1.)));
    }
  else if(argis("-visualize")) {
    shift();
    sn::get_tabled().load();
    visualize_table(sn::get_tabled(), argcs());
    }
  else if(argis("-sn-unittest")) {
    println(hlog, "nih = ", (bool)nih);
    ld maxerr;
    auto test_result = [&maxerr] (ld a, ld b) { maxerr = max(maxerr, (a-b)); };
    auto test_result_p = [&maxerr] (hyperpoint a, hyperpoint b) { maxerr = max(maxerr, hypot_d(3, a-b)); };
    auto test = [&maxerr] (string s, reaction_t tester) {
      maxerr = 0;
      tester();
      println(hlog, "unittest: ", s, " error = ", maxerr);
      };
    test("x_to_ix", [&] { for(ld a=0; a<=20; a+=.1) test_result(a, sn::ix_to_x(sn::x_to_ix(a))); });
    test("z_to_iz", [&] { for(ld a=nih?-20:0; a<=20; a+=.1) test_result(a, sn::iz_to_z(sn::z_to_iz(a))); });
    test("azeq_to_table", [&] { for(ld a=-5; a<=5; a++) for(ld b=-5; b<=5; b++) for(ld c=-5; c<=5; c++) { hyperpoint h = point3(a,b,c); test_result_p(h, sn::table_to_azeq(sn::azeq_to_table(h))); }});
    test("azeq_to_table recompressed", [&] { 
      for(ld a=-5; a<=5; a++) for(ld b=-5; b<=5; b++) for(ld c=-5; c<=5; c++) { 
        hyperpoint h = point3(a,b,c); test_result_p(h, sn::table_to_azeq(recompress(sn::azeq_to_table(h)))); 
        }
      });
    }

  else return 1;
  return 0;
  }

auto hook = addHook(hooks_args, 100, readArgs);

}
}
