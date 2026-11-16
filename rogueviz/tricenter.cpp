// a program to test whether the "Euler line" holds in hyperbolic/spherical geometry
// compile with: mymake -O3 rogueviz/tricenter.cpp
// run with e.g.
// ./hyper -canvas-random 0 -canvas i -geo 0 -tricenter -noplayer -noscr -zoom .95 -run -shot-1000 -shott 1 -fillmodel 000000 -pngshot euler-fail.png
// press 'o' to change options

#include "rogueviz.h"

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

hyperpoint d1(hyperpoint A, hyperpoint B, ld d = 1.5) {
  hyperpoint X;
  if(1) {
    simplifier s1(gpushxto0(A), {&A, &B}, {&X});
    simplifier s2(spintox(B), {&A, &B}, {&X});
    X = xpush0(d);
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

hyperpoint mirror_line_point(hyperpoint A, hyperpoint B, hyperpoint X) {
  if(1) {
    simplifier s1(gpushxto0(A), {&A, &B, &X});
    simplifier s2(spintox(B), {&A, &B, &X});
    X[1] = -X[1];
    }
  return X;
  }

hyperpoint linecross3(hyperpoint A, hyperpoint A1, hyperpoint B, hyperpoint B1, hyperpoint C, hyperpoint C1, const string& s) {
  hyperpoint res = linecross(A, A1, B, B1);
  println(hlog, s, is_on_line(C, C1, res) ? " verification: OK" : " verification: failed");
  return res;
  }

double a, b, c;

bool show_triangle = true, show_incenter, show_centroid, show_circumcenter, show_orthocenter,
  show_euler_line, show_ninepoint, show_symmedian, show_gergonne, show_nagel;

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

  if(show_triangle) {
    markpoint(A, 0xFF0000);
    markpoint(B, 0xFF0000);
    markpoint(C, 0xFF0000);
    markseg(A, B, 0xFF0000FF); markseg(B, C, 0xFF0000FF); markseg(C, A, 0xFF0000FF);
    }

  // indices as in: https://faculty.evansville.edu/ck6/encyclopedia/ETC.html

  hyperpoint A1 = bisector(A, B, C);
  hyperpoint B1 = bisector(B, C, A);
  hyperpoint C1 = bisector(C, A, B);
  hyperpoint D1 = linecross3(A, A1, B, B1, C, C1, "incenter"); ignore(D1);
  if(show_incenter) {
    markseg(A, A1, 0x00FF00FF); markseg(B, B1, 0x00FF00FF); markseg(C, C1, 0x00FF00FF);
    markpoint(D1, 0x00FF00);
    }

  hyperpoint C2 = mid(A, B);
  hyperpoint A2 = mid(B, C);
  hyperpoint B2 = mid(C, A);
  hyperpoint D2 = linecross3(A2, A, B2, B, C2, C, "centroid");
  if(show_centroid) {
    markseg(A2, A, 0xFF00FFFF); markseg(B2, B, 0xFF00FFFF); markseg(C2, C, 0xFF00FFFF);
    markpoint(D2, 0xFF00FF);
    }

  hyperpoint C3 = ortho1(C2, A);
  hyperpoint A3 = ortho1(A2, B);
  hyperpoint B3 = ortho1(B2, C);
  hyperpoint D3 = linecross3(A2, A3, B2, B3, C2, C3, "circumcenter");
  if(show_circumcenter) {
    markseg(A2, A3, 0x00FFFFFF); markseg(B2, B3, 0x00FFFFFF); markseg(C2, C3, 0x00FFFFFF);
    markpoint(D3, 0x00FFFF);
    }

  hyperpoint C4 = perpendicular_drop(A, B, C);
  hyperpoint A4 = perpendicular_drop(B, C, A);
  hyperpoint B4 = perpendicular_drop(C, A, B);
  hyperpoint D4 = linecross3(A, A4, B, B4, C, C4, "orthocenter");
  if(show_orthocenter) {
    markseg(A, A4, 0xFFFF00FF); markseg(B, B4, 0xFFFF00FF); markseg(C, C4, 0xFFFF00FF);
    markpoint(D4, 0xFFFF00);
    }

  auto markcc = [&] (hyperpoint h1, hyperpoint h2, hyperpoint h3, color_t col, const string& s) {
    if(is_on_line(h1, h2, h3)) {
      println(hlog, s, ": lie on a line");
      if(h1[0] > h2[0]) swap(h1, h2);
      if(h2[0] > h3[0]) swap(h3, h2);
      if(h1[0] > h2[0]) swap(h1, h2);
      markseg(h1, h3, col);
      }
    else {
      hyperpoint ctr = circumscribe(h1, h2, h3);
      ld rad = hdist(h1, ctr);
      for(int i=0; i<=360; i++) curvepoint(xspinpush0(i*1._deg, rad));
      queuecurve(S * rgpushxto0(ctr), col, 0, PPR::LINE);
      }
    };

  if(show_euler_line) markcc(D2, D3, D4, 0xFFFFFFFF, "Euler line");

  // the nine-point circle center:
  // - passes through the centers of the sides
  hyperpoint D5_a = circumscribe(A2, B2, C2);
  // - passes through the feet of the altitudes
  hyperpoint D5_b = circumscribe(A4, B4, C4);
  // - passes through the midpoints of AD4, BD4 and CD4
  hyperpoint D5_c = circumscribe(mid(A, D4), mid(B, D4), mid(C, D4));
  if(show_ninepoint) {
    markpoint(D5_a, 0xC0C0C0);
    markpoint(D5_b, 0xC0C0C0);
    markpoint(D5_c, 0xC0C0C0);
    }

  vector<hyperpoint> all = { A2, B2, C2, A4, B4, C4, mid(A, D4), mid(B, D4), mid(C, D4) };
  hyperpoint eucenter = circumscribe(D2, D3, D4); ld rad = hdist(eucenter, D2);
  int score = 0, outof = 0;
  for(int a=0; a<9; a++)
  for(int b=0; b<9; b++)
  for(int c=0; c<9; c++) if(a<b && b<c) {
    hyperpoint x = circumscribe(all[a], all[b], all[c]);
    if(abs(hdist(eucenter, x) - rad) < 1e-6) score++;
    outof++;
    ld xrad = hdist(x, all[a]);
    for(int d=0; d<9; d++) if(!among(d, a, b, c)) {
      if(abs(hdist(all[d], x) - xrad) < 1e-6) score++;
      outof++;
      }
    }
  println(hlog, "score = ", score, " out of ", outof);

  hyperpoint A6 = mirror_line_point(A, A1, A2);
  hyperpoint B6 = mirror_line_point(B, B1, B2);
  hyperpoint C6 = mirror_line_point(C, C1, C2);
  hyperpoint D6 = linecross3(A, A6, B, B6, C, C6, "symmedian point");
  if(show_symmedian) {
    markseg(A, A6, 0x8000FFFF); markseg(B, B6, 0x8000FFFF); markseg(C, C6, 0x8000FFFF);
    markpoint(D6, 0x8000FF);
    }

  hyperpoint C7 = perpendicular_drop(A, B, D1);
  hyperpoint A7 = perpendicular_drop(B, C, D1);
  hyperpoint B7 = perpendicular_drop(C, A, D1);
  hyperpoint D7 = linecross3(A, A7, B, B7, C, C7, "Gergonne point");
  if(show_gergonne) {
    markseg(A, A7, 0xC06040FF); markseg(B, B7, 0xC06040FF); markseg(C, C7, 0xC06040FF);
    markpoint(D7, 0xC06040);
    }

  hyperpoint A8 = d1(B, C, (hdist(A, C) + hdist(B, C) - hdist(A, B)) / 2);
  hyperpoint B8 = d1(C, A, (hdist(B, A) + hdist(C, A) - hdist(B, C)) / 2);
  hyperpoint C8 = d1(A, B, (hdist(C, B) + hdist(A, B) - hdist(C, A)) / 2);
  hyperpoint D8 = linecross3(A, A8, B, B8, C, C8, "Nagel point");
  if(show_nagel) {
    markseg(A, A8, 0x9090E0FF); markseg(B, B8, 0x9090E0FF); markseg(C, C8, 0x9090E0FF);
    markpoint(D8, 0x9090E0);
    }
  }

void show() {
  cmode = sm::SIDE | sm::MAYDARK;
  gamescreen();
  dialog::init(XLAT("tricenter"), 0xFFFFFFFF, 150, 0);

  dialog::addBoolItem_action("show triangle", show_triangle, 't');
  dialog::addBoolItem_action("show incenter", show_incenter, 'i');
  dialog::addBoolItem_action("show centroid", show_centroid, 'c');
  dialog::addBoolItem_action("show circumcenter", show_circumcenter, 'C');
  dialog::addBoolItem_action("show orthocenter", show_orthocenter, 'o');
  dialog::addBoolItem_action("show Euler line", show_euler_line, 'e');
  dialog::addBoolItem_action("show nine-point", show_ninepoint, '9');
  dialog::addBoolItem_action("show symmedian", show_symmedian, 's');
  dialog::addBoolItem_action("show Gergonne point", show_gergonne, 'g');
  dialog::addBoolItem_action("show Nagel point", show_nagel, 'n');
  dialog::addSelItem("randomize points", "?", 'r');
  dialog::add_action([] {
    a = rand() % 1000;
    b = rand() % 1000;
    c = rand() % 1000;
    });
  dialog::addBack();
  dialog::display();
  }

void enable() {

  a = 1; b = 2; c = 5;

  rv_hook(hooks_o_key, 80, [] (o_funcs& v) { v.push_back(named_dialog("tricenter", show)); });

  rv_hook(hooks_frame, 100, tricenter);
  }

auto tricenter_hooks = arg::add2("-tricenter", enable)
  + addHook_rvslides(187, [] (string s, vector<tour::slide>& v) {
      if(s != "mixed") return;
      using namespace tour;
      v.push_back(slide{
        "triangle centers", 10, LEGAL::NONE | QUICKGEO,
        "An exploration of various 'triangle centers' in non-Euclidean geometry. Press 'o'."
        ,
        [] (presmode mode) {
          if(mode == pmStart) enable();
          }});
      });


}
}
