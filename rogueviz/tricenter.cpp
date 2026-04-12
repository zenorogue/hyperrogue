// a program to test whether the "Euler line" holds in hyperbolic/spherical geometry
// compile with: mymake -O3 rogueviz/tricenter.cpp
// run with e.g.
// ./hyper -canvas-random 0 -canvas i -geo 0 -tricenter -noplayer -noscr -zoom .95 -run -shot-1000 -shott 1 -fillmodel 000000 -pngshot euler-fail.png
// press 'r' to use a different triangle

#include "../hyper.h"

namespace hr {

namespace tricenter {

struct simplifier {
  transmatrix T;
  vector<hyperpoint*> hs;
  simplifier(transmatrix _T, vector<hyperpoint*> _hs, vector<hyperpoint*> extra = {}) {
    T = _T; hs = _hs;
    for(auto h: hs) (*h) = T * (*h);
    for(auto h: extra) hs.push_back(h);
    }
  ~simplifier() { for(auto h: hs) (*h) = iso_inverse(T) * (*h); }
  };

bool is_on_line(hyperpoint A, hyperpoint B, hyperpoint X) {
  simplifier s1(gpushxto0(A), {&A, &B, &X});
  simplifier s2(spintox(B), {&A, &B, &X});
  return abs(X[1]) < 1e-6;
  }

hyperpoint perpendicular_drop(hyperpoint A, hyperpoint B, hyperpoint X) {
  if(1) {
    simplifier s1(gpushxto0(A), {&A, &B, &X});
    simplifier s2(spintox(B), {&A, &B, &X});
    X[1] = 0; X = normalize(X);
    }
  return X;
  }

hyperpoint d1(hyperpoint A, hyperpoint B) {
  hyperpoint X;
  if(1) {
    simplifier s1(gpushxto0(A), {&A, &B}, {&X});
    simplifier s2(spintox(B), {&A, &B}, {&X});
    X = xpush0(1.5);
    }
  return X;
  }

hyperpoint bisector(hyperpoint X, hyperpoint A, hyperpoint B) {
  hyperpoint A1 = d1(X, A);
  hyperpoint B1 = d1(X, B);
  return mid(A1, B1);
  }

hyperpoint ortho1(hyperpoint A, hyperpoint B) {
  hyperpoint X;
  if(1) {
    simplifier s1(gpushxto0(A), {&A, &B}, {&X});
    simplifier s2(spintox(B), {&A, &B}, {&X});
    X = ypush0(1.5);
    }
  return X;
  }

hyperpoint linecross3(hyperpoint A, hyperpoint A1, hyperpoint B, hyperpoint B1, hyperpoint C, hyperpoint C1, const string& s) {
  hyperpoint res = linecross(A, A1, B, B1);
  println(hlog, s, is_on_line(C, C1, res) ? " verification: OK" : " verification: failed");
  return res;
  }

double a, b, c;

void tricenter() {
  hyperpoint A = xspinpush0(a, 1);
  hyperpoint B = xspinpush0(b, 1);
  hyperpoint C = xspinpush0(c, 1);

  shiftmatrix S = ggmatrix(cwt.at);

  auto markpoint = [&] (hyperpoint h, color_t c) {
    for(int i=0; i<=360; i++) curvepoint(xspinpush0(i*1._deg, 0.02));
    queuecurve(S * rgpushxto0(h), (c << 8) | 0xFF, (c << 8) | 0xC0, PPR::LINE);
    };

  auto markseg = [&] (hyperpoint h1, hyperpoint h2, color_t c) {
    vid.linewidth *= 3;
    queueline(S * h1, S * h2, c, 2);
    vid.linewidth /= 3;
    };

  markpoint(A, 0xFF0000);
  markpoint(B, 0xFF0000);
  markpoint(C, 0xFF0000);
  markseg(A, B, 0xFF0000FF); markseg(B, C, 0xFF0000FF); markseg(C, A, 0xFF0000FF);

  /* hyperpoint A1 = bisector(A, B, C);
  hyperpoint B1 = bisector(B, C, A);
  hyperpoint C1 = bisector(C, A, B);
  hyperpoint D1 = linecross3(A, A1, B, B1, C, C1, "incenter");
  markseg(A, A1, 0x00FF00FF); markseg(B, B1, 0x00FF00FF); markseg(C, C1, 0x00FF00FF);
  markpoint(D1, 0x00FF00); */

  hyperpoint C2 = perpendicular_drop(A, B, C);
  hyperpoint A2 = perpendicular_drop(B, C, A);
  hyperpoint B2 = perpendicular_drop(C, A, B);
  hyperpoint D2 = linecross3(A, A2, B, B2, C, C2, "orthocenter");
  markseg(A, A2, 0xFFFF00FF); markseg(B, B2, 0xFFFF00FF); markseg(C, C2, 0xFFFF00FF);
  markpoint(D2, 0xFFFF00);

  hyperpoint C3 = mid(A, B); 
  hyperpoint A3 = mid(B, C); 
  hyperpoint B3 = mid(C, A); 
  hyperpoint D3 = linecross3(A3, A, B3, B, C3, C, "centroid");
  markseg(A3, A, 0xFF00FFFF); markseg(B3, B, 0xFF00FFFF); markseg(C3, C, 0xFF00FFFF);
  markpoint(D3, 0xFF00FF);

  hyperpoint C4 = ortho1(C3, A);
  hyperpoint A4 = ortho1(A3, B);
  hyperpoint B4 = ortho1(B3, C);
  hyperpoint D4 = linecross3(A3, A4, B3, B4, C3, C4, "circumcenter");
  markseg(A3, A4, 0x00FFFFFF); markseg(B3, B4, 0x00FFFFFF); markseg(C3, C4, 0x00FFFFFF);
  markpoint(D4, 0x00FFFF);

  markseg(D3, D2, 0xFFFFFFFF); markseg(D3, D4, 0xFFFFFFFF); markseg(D2, D4, 0xFFFFFFFF);
  if(is_on_line(D3, D2, D4)) println(hlog, "Euler line verified"); else println(hlog, "Euler line failed");
  }

void enable() {

  a = 1; b = 2; c = 5;

  addHook(hooks_frame, 100, tricenter);

  addHook(hooks_handleKey, 101, [] (int sym, int uni) {
    if((cmode & sm::NORMAL) && uni == 'r') {
      a = rand() % 1000;
      b = rand() % 1000;
      c = rand() % 1000;
      return true;
      }
    return false;
    });

  }

auto tricenter_hooks = arg::add2("-tricenter", enable);

}
}
