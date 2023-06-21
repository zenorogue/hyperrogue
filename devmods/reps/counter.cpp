// a float-like type to count operations

namespace reps {

std::map<std::string, int> counts;

#define C(x) {}

std::array<int, 16> cbc;

constexpr int cbcAdd = 1;
constexpr int cbcMul = 2;
constexpr int cbcDiv = 3;
constexpr int cbcTrig = 4;

struct countfloat {
  ld x;

  countfloat() {}
  explicit countfloat(ld _x) : x(_x) {}
  explicit operator ld() { return x; }
  operator bool() { return x != 0; }
  
  countfloat operator +(countfloat a) const { C("plus"); cbc[1]++; return countfloat(x + a.x); }
  countfloat operator -(countfloat a) const { C("plus"); cbc[1]++; return countfloat(x + a.x); }
  countfloat operator -() const { return countfloat(-x); }
  countfloat operator +() const { return countfloat(+x); }
  countfloat operator *(countfloat a) const { C("mul"); cbc[2]++; return countfloat(x * a.x); }
  countfloat operator /(countfloat a) const { C("div"); cbc[3]++; return countfloat(x / a.x); }

  bool operator <(countfloat a) const { return x < a.x; }
  bool operator >(countfloat a) const { return x > a.x; }
  bool operator <=(countfloat a) const { return x <= a.x; }
  bool operator >=(countfloat a) const { return x >= a.x; }

  countfloat& operator +=(countfloat a) { C("plus"); cbc[1]++; x += a.x; return self; }
  countfloat& operator -=(countfloat a) { C("plus"); cbc[1]++; x -= a.x; return self; }
  countfloat& operator *=(countfloat a) { C("mul"); cbc[2]++; x *= a.x; return self; }
  countfloat& operator /=(countfloat a) { C("mul"); cbc[2]++; x /= a.x; return self; }
  countfloat& operator *=(int a) { if(a != 1 && a != -1) C("mul"+hr::its(a)); x *= a; return self; }
  countfloat& operator /=(int a) { if(a != 1 && a != -1) C("div"+hr::its(a)); x /= a; return self; }

  friend countfloat sin(countfloat a) { cbc[4]++; C("sin"); return countfloat(sin(a.x)); }
  friend countfloat cos(countfloat a) { cbc[4]++; C("cos"); return countfloat(cos(a.x)); }
  friend countfloat tan(countfloat a) { cbc[4]++; C("tan"); return countfloat(tan(a.x)); }
  friend countfloat sinh(countfloat a) { cbc[4]++; C("sinh"); return countfloat(sinh(a.x)); }
  friend countfloat cosh(countfloat a) { cbc[4]++; C("cosh"); return countfloat(cosh(a.x)); }
  friend countfloat tanh(countfloat a) { cbc[4]++; C("cosh"); return countfloat(tanh(a.x)); }
  friend countfloat asinh(countfloat a) { cbc[4]++; C("asinh"); return countfloat(asinh(a.x)); }
  friend countfloat acosh(countfloat a) { cbc[4]++; C("acosh"); return countfloat(acosh(a.x)); }
  friend countfloat acos(countfloat a) { cbc[4]++; C("acos"); return countfloat(acos(a.x)); }
  friend countfloat exp(countfloat a) { cbc[4]++; C("exp"); return countfloat(exp(a.x)); }
  friend countfloat log(countfloat a) { cbc[4]++; C("log"); return countfloat(log(a.x)); }
  friend countfloat sqrt(countfloat a) { cbc[4]++; C("sqrt"); return countfloat(sqrt(a.x)); }
  friend countfloat atan2(countfloat a, countfloat b) { cbc[4]++; C("atan"); return countfloat(atan2(a.x, b.x)); }
  friend countfloat pow(countfloat a, ld b) { cbc[4]++; C("pow" + hr::fts(b)); return countfloat(pow(a.x, b)); }
  friend countfloat abs(countfloat a) { return countfloat(abs(a.x)); }
  countfloat operator *(int a) const { if(a != 1 && a != -1) C("mul" + hr::its(a)); return countfloat(x * a); }
  countfloat operator /(int a) const { if(a != 1 && a != -1) C("div" + hr::its(a)); return countfloat(x / a); }

  friend bool isinf(countfloat a) { return isinf(a.x); }
  friend bool isnan(countfloat a) { return isnan(a.x); }
  };

template<> countfloat get_deg<countfloat> (int deg) { return countfloat( M_PI * deg / 180 ); }

}

namespace hr {
  void print(hr::hstream& hs, ::reps::countfloat b) {
    print(hs, b.x);
    }
}
