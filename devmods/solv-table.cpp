// This generates the 'solv-geodesics.dat' file.
// You may change the _PREC* values for more precise geodesics.

#include "../hyper.h"

#include <thread>
#include <mutex>

namespace hr {

const int _PRECX = 64;
const int _PRECY = 64;
const int _PRECZ = 64;

transmatrix parabolic1(ld u);

namespace nisot {

typedef hyperpoint pt;

using solnihv::x_to_ix;

ld z_to_iz(ld z) { if(sol) return tanh(z); else return tanh(z/4)/2 + .5; }

ptlow be_low(hyperpoint x) { return ptlow({float(x[0]), float(x[1]), float(x[2])}); }

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

hyperpoint iterative_solve(hyperpoint xp, hyperpoint candidate, int prec, ld minerr, bool debug = false) {

  transmatrix T = Id; T[0][1] = 8; T[2][2] = 5;
  
  auto f = [&] (hyperpoint x) { return nisot::numerical_exp(x, prec); }; // T * x; };

  auto ver = f(candidate);
  ld err = solerror(xp, ver);
  auto at = candidate;
  
  ld eps = 1e-6;

  hyperpoint c[3];  
  for(int a=0; a<3; a++) c[a] = point3(a==0, a==1, a==2);
  
  while(err > minerr) {
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
        break;
        }
      goto nextfix;
      }
    
    nextiter: ;
    }
  
  return at;
  }

ptlow solution[_PRECZ][_PRECY][_PRECX];

ptlow mlow(ld x, ld y, ld z) { return ptlow({float(x), float(y), float(z)}); }

hyperpoint atxyz(ld x, ld y, ld z) { return hyperpoint({x, y, z, 1}); }

ptlow operator +(ptlow a, ptlow b) { return mlow(a[0]+b[0], a[1]+b[1], a[2]+b[2]); }
ptlow operator -(ptlow a, ptlow b) { return mlow(a[0]-b[0], a[1]-b[1], a[2]-b[2]); }
ptlow operator *(ptlow a, ld x) { return mlow(a[0]*x, a[1]*x, a[2]*x); }

ptlow can(hyperpoint x) {
  // azimuthal equidistant to Klein
  ld r = sqrt(x[0] * x[0] + x[1] * x[1] + x[2] * x[2]);
  if(r == 0) return mlow(0,0,0);
  ld make_r = tanh(r);
  ld d = make_r / r;
  return mlow(x[0]*d, x[1]*d, x[2]*d);
  }

hyperpoint uncan(ptlow x) {
  ld r = sqrt(x[0] * x[0] + x[1] * x[1] + x[2] * x[2]);
  if(r == 0) return atxyz(0,0,0);
  ld make_r = atanh(r);
  if(r == 1) make_r = 30;
  ld d = make_r / r;
  return atxyz(x[0]*d, x[1]*d, x[2]*d);
  }

hyperpoint uncan_info(ptlow x) {
  ld r = sqrt(x[0] * x[0] + x[1] * x[1] + x[2] * x[2]);
  println(hlog, "r = ", r);
  if(r == 0) return atxyz(0,0,0);
  ld make_r = atanh(r);
  println(hlog, "make_r = ", make_r);
  ld d = make_r / r;
  println(hlog, "d = ", d);
  return atxyz(x[0]*d, x[1]*d, x[2]*d);
  }

void fint(FILE *f, int x) { fwrite(&x, sizeof(x), 1, f); }
void ffloat(FILE *f, float x) { fwrite(&x, sizeof(x), 1, f); }

void write_table(const char *fname) {
  FILE *f = fopen(fname, "wb");    
  fint(f, _PRECX);
  fint(f, _PRECY);
  fint(f, _PRECZ);
  fwrite(solution, sizeof(solution), 1, f);
  fclose(f);
  }

void load_table(const char *fname) {
  int s;
  FILE *f = fopen(fname, "rb");    
  fread(&s, 4, 1, f);
  fread(&s, 4, 1, f);
  fread(&s, 4, 1, f);
  fread(solution, sizeof(solution), 1, f);
  fclose(f);
  }

ld ix_to_x(ld ix) {
  ld minx = 0, maxx = 1;
  for(int it=0; it<100; it++) {
    ld x = (minx + maxx) / 2;
    if(x_to_ix(atanh(x)) < ix) minx = x;
    else maxx = x;
    }
  return atanh(minx);
  }

ld iz_to_z(ld z) {
  return nih ? atanh(z * 2 - 1)*4 : atanh(z); // atanh(z * 2 - 1);
  }

int last_x = _PRECX-1, last_y = _PRECY-1, last_z = _PRECZ-1;

ld ptd(ptlow p) {
  return p[0]*p[0] + p[1]*p[1] + p[2] * p[2];
  }

ptlow zflip(ptlow x) { return mlow(x[1], x[0], -x[2]); }

void build_sols() {
  std::mutex file_mutex;
  ld max_err = 0;
  auto act = [&] (int tid, int iz) {
    if((nih && iz == 0) || iz == _PRECZ-1) return;
  
    auto solve_at = [&] (int ix, int iy) {
      ld x = ix_to_x(ix / (_PRECX-1.));
      ld y = ix_to_x(iy / (_PRECY-1.));
      ld z = iz_to_z(iz / (_PRECZ-1.));
      
      auto v = hyperpoint ({x,y,z,1});
      
      vector<hyperpoint> candidates;
      hyperpoint cand;
      
      candidates.push_back(atxyz(0,0,0)); 
      
      static constexpr int prec = 100;
      
      // sort(candidates.begin(), candidates.end(), [&] (hyperpoint a, hyperpoint b) { return solerror(v, direct_exp(a, prec)) > solerror(v, direct_exp(b, prec)); });
      
      // cand_best = candidates.back();
      
      vector<hyperpoint> solved_candidates;
      
      for(auto c: candidates)  {
        auto solt = iterative_solve(v, c, prec, 1e-6);
        solved_candidates.push_back(solt);
        if(solerror(v, nisot::numerical_exp(solt, prec)) < 1e-9) break;
        }

      sort(solved_candidates.begin(), solved_candidates.end(), [&] (hyperpoint a, hyperpoint b) { return solerror(v, nisot::numerical_exp(a, prec)) > solerror(v, nisot::numerical_exp(b, prec)); });
      
      cand = solved_candidates.back();

      auto xerr = solerror(v, nisot::numerical_exp(cand, prec));
      
      if(xerr > 1e-3) {
        println(hlog, format("[%2d %2d %2d] ", iz, iy, ix));
        println(hlog, "f(?) = ", v);
        println(hlog, "f(", cand, ") = ", nisot::numerical_exp(cand, prec));
        println(hlog, "error = ", xerr);
        println(hlog, "canned = ", can(cand));
        max_err = xerr;
        hyperpoint h1 = uncan(solution[iz][iy-1][ix]);
        hyperpoint h2 = uncan(solution[iz][iy][ix-1]);
        hyperpoint h3 = uncan(solution[iz][iy-1][ix-1]);
        hyperpoint h4 = h1 + h2 - h3;
        solution[iz][iy][ix] = can(h4);
        return;
        }

      solution[iz][iy][ix] = can(cand);
      
      for(int z=0; z<3; z++) if(isnan(solution[iz][iy][ix][z]) || isinf(solution[iz][iy][ix][z])) {
        println(hlog, cand, "canned to ", solution[iz][iy][ix]);
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
      println(hlog, format("%2d: %2d", iz, it));
      }
    };

  parallelize(_PRECZ, 0, _PRECZ, act);
  
  for(int x=0; x<last_x; x++)
  for(int y=0; y<last_y; y++) {
    for(int z=last_z; z<_PRECZ; z++)
      solution[z][y][x] = solution[z-1][y][x] * 2 - solution[z-2][y][x];
    if(nih)
      solution[0][y][x] = solution[1][y][x] * 2 - solution[2][y][x];
    }
  
  for(int x=0; x<last_x; x++)
  for(int y=last_y; y<_PRECY; y++)
  for(int z=0; z<_PRECZ; z++)
    solution[z][y][x] = solution[z][y-1][x] * 2 - solution[z][y-2][x];
  
  for(int x=last_x; x<_PRECX; x++)
  for(int y=0; y<_PRECY; y++)
  for(int z=0; z<_PRECZ; z++)
    solution[z][y][x] = solution[z][y][x-1] * 2 - solution[z][y][x-2];
  }

int main(int argc, char **argv) {

  println(hlog);
  
  /*
  geometry = gSol;

  build_sols();
  write_table("solv-geodesics-generated.dat");
  */
  
  geometry = gNIH;
  build_sols();
  write_table("h23-geodesics-generated.dat");

  exit(0);
  }

int phooks = addHook(hooks_main, 0, main);

}
}
