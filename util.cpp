// Hyperbolic Rogue -- basic utility functions
// Copyright (C) 2011-2018 Zeno Rogue, see 'hyper.cpp' for details

/** \file util.cpp
 *  \brief basic utility functions: maths, parsing expressions
 */

#include "hyper.h"
namespace hr {

#if CAP_TIMEOFDAY
#if !CAP_SDL
int lastusec;
int uticks;

int SDL_GetTicks() {
  struct timeval tim;
  gettimeofday(&tim, NULL);
  int newusec = tim.tv_usec;
  uticks += newusec - lastusec;
  if(newusec <= lastusec)
    uticks += 1000000;
  lastusec = newusec;
  return uticks / 1000;
  }

#endif
#endif

EX long double sqr(long double x) { return x*x; }

EX ld round_nearest(ld x) { if(x > 0) return int(x+.5); else return -int(.5-x); }
EX ld round_nearest(ld x, ld multiple_of) { return multiple_of * round_nearest(x / multiple_of); }

EX int gcd(int i, int j) {
  return i ? gcd(j%i, i) : j;
  }

EX int gmod(int i, int j) {
  i %= j; if(i<0) i += j;
  return i;
  }

EX int zgmod(int a, int b) { return b ? gmod(a, b) : a; }

EX int szgmod(int a, int b) { 
  if(!b) return a;
  a = gmod(a, b);
  if(2*a >= b) return a - b;
  return a;
  }

EX int gdiv(int i, int j) {
  return (i - gmod(i, j)) / j;
  }

EX ld frac(ld x) {
  x -= int(x);
  if(x < 0) x++;
  return x;
  }

EX ld lerp(ld a0, ld a1, ld x) {
  return a0 + (a1-a0) * x;
  }

EX ld ilerp(ld a0, ld a1, ld x) {
  return (x-a0) / (a1-a0);
  }

// debug utilities

#if CAP_PROFILING

#define FRAMES 64
#define CATS 16

long long proftable[16][FRAMES];
int pframeid;

void profile_frame() { 
  pframeid++; pframeid %=  FRAMES;
  for(int t=0; t<16; t++) proftable[t][pframeid] = 0;
  }

EX void profile_start(int t) { proftable[t][pframeid] -= getms(); }
EX void profile_stop(int t) { proftable[t][pframeid] += getms(); }

void profile_info() {
  for(int t=0; t<16; t++) {
    sort(proftable[t], proftable[t]+FRAMES);
    if(proftable[t][FRAMES-1] == 0) continue;
    long long sum = 0;
    for(int f=0; f<FRAMES; f++) sum += proftable[t][f];
    printf("Category %d: avg = %Ld, %Ld..%Ld..%Ld..%Ld..%Ld\n",
      t, sum / FRAMES, proftable[t][0], proftable[t][16], proftable[t][32],
      proftable[t][48], proftable[t][63]);
    }
  }
#endif

#if !CAP_PROFILING
#if HDR
#define profile_frame()
#define profile_start(t)
#define profile_stop(t)
#define profile_info()
#endif
#endif

EX purehookset hooks_tests;

EX string simplify(const string& s) {
  string res;
  for(char c: s) if(isalnum(c)) res += c;
  return res;
  }

EX bool appears(const string& haystack, const string& needle) {
  return simplify(haystack).find(simplify(needle)) != string::npos;
  }

#if HDR
struct exp_parser {
  string s;
  int at;
  exp_parser() { at = 0; }
  
  map<string, cld> extra_params;

  bool ok() { return at == isize(s); }
  char next(int step=0) { if(at >= isize(s)-step || at == -1) return 0; else return s[at+step]; }
  
  bool eat(const char *c) {
    int orig_at = at;
    while(*c && *c == next()) at++, c++;
    if(*c == 0) return true;
    else at = orig_at;
    return false;
    }

  cld parse(int prio = 0);

  cld parsepar() {
    cld res = parse();
    if(next() != ')') { at = -1; return res; }
    at++;
    return res;
    }

  };
#endif

cld exp_parser::parse(int prio) {
  cld res;
  while(next() == ' ') at++;
  if(eat("sin(")) res = sin(parsepar());
  else if(eat("cos(")) res = cos(parsepar());
  else if(eat("sinh(")) res = sinh(parsepar());
  else if(eat("cosh(")) res = cosh(parsepar());
  else if(eat("asin(")) res = asin(parsepar());
  else if(eat("acos(")) res = acos(parsepar());
  else if(eat("asinh(")) res = asinh(parsepar());
  else if(eat("acosh(")) res = acosh(parsepar());
  else if(eat("exp(")) res = exp(parsepar());
  else if(eat("log(")) res = log(parsepar());
  else if(eat("tan(")) res = tan(parsepar());
  else if(eat("tanh(")) res = tanh(parsepar());
  else if(eat("atan(")) res = atan(parsepar());
  else if(eat("atanh(")) res = atanh(parsepar());
  else if(eat("abs(")) res = abs(parsepar());
  else if(eat("re(")) res = real(parsepar());
  else if(eat("im(")) res = imag(parsepar());
  else if(eat("conj(")) res = std::conj(parsepar());
  else if(eat("floor(")) res = floor(real(parsepar()));
  else if(eat("frac(")) { res = parsepar(); res = res - floor(real(res)); }
  else if(eat("to01(")) { res = parsepar(); return atan(res) / ld(M_PI) + ld(0.5); }
  else if(eat("ifp(")) {
    cld cond = parse(0);
    if(next() != ',') {at = -1; return 0; } at++;
    cld yes = parse(0);
    if(next() != ',') {at = -1; return 0; } at++;
    cld no = parsepar();
    return real(cond) > 0 ? yes : no;
    }  
  else if(eat("wallif(")) {
    cld val0 = parse(0);
    if(next() != ',') {at = -1; return 0; } at++;
    cld val1 = parsepar();
    if(real(extra_params["p"]) >= 3.5) return val0;
    else return val1;
    }
  else if(eat("rgb(")) {     
    cld val0 = parse(0);
    if(next() != ',') {at = -1; return 0; } at++;
    cld val1 = parse(0);
    if(next() != ',') {at = -1; return 0; } at++;
    cld val2 = parsepar();
    switch(int(real(extra_params["p"]) + .5)) {
      case 1: return val0;
      case 2: return val1;
      case 3: return val2;
      default: return 0;
      }
    }
  else if(eat("let(")) {
    string name;
    while(true) {
      char c = next();
      if((c >= '0' && c <= '9') || (c == '.' && next(1) != '.') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_')
        name += c, at++;
      else break;
      }
    if(next() != '=') { at = -1; return 0; }
    at++;
    cld val = parse(0);
    if(next() != ',') { at = -1; return 0; }
    at++;
    dynamicval<cld> d(extra_params[name], val);
    return parsepar();
    }
  #if CAP_TEXTURE
  else if(eat("txp(")) {
    cld val = parsepar();
    return texture::get_txp(real(val), imag(val), int(real(extra_params["p"]) + .5)-1);
    }
  #endif
  else if(next() == '(') at++, res = parsepar(); 
  else {
    string number;
    while(true) {
      char c = next();
      if((c >= '0' && c <= '9') || (c == '.' && next(1) != '.') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_')
        number += c, at++;
      else break;
      }
    if(number == "e") res = exp(1);
    else if(number == "i") res = cld(0, 1);
    else if(number == "p" || number == "pi") res = M_PI;
    else if(number == "" && next() == '-') { at++; res = -parse(prio); }
    else if(number == "") at = -1;
    else if(number == "s") res = ticks / 1000.;
    else if(number == "ms") res = ticks;
    else if(number[0] == '0' && number[1] == 'x') res = strtoll(number.c_str()+2, NULL, 16);
    else if(number == "mousex") res = mousex;
    else if(number == "mousey") res = mousey;
    else if(number == "random") res = randd();
    else if(number == "mousez") res = cld(mousex - current_display->xcenter, mousey - current_display->ycenter) / cld(current_display->radius, 0);
    else if(number == "shot") res = inHighQual ? 1 : 0;
    else if(extra_params.count(number)) res = extra_params[number];
    else if(params.count(number)) res = params.at(number);
    else if(number[0] >= 'a' && number[0] <= 'z') at = -1;
    else { std::stringstream ss; res = 0; ss << number; ss >> res; }
    }
  while(true) {
    #if CAP_ANIMATIONS
    if(next() == '.' && next(1) == '.' && prio == 0) {
      vector<cld> rest = { res };
      while(next() == '.' && next(1) == '.') {
        at += 2; rest.push_back(parse(10));
        }
      ld v = ticks * (isize(rest)-1.) / anims::period;
      int vf = v;
      v -= vf;
      vf %= (isize(rest)-1);
      res = rest[vf] + (rest[vf+1] - rest[vf]) * v;
      return res;
      }
    else 
    #endif
    if(next() == '+' && prio <= 10) at++, res = res + parse(20);
    else if(next() == '-' && prio <= 10) at++, res = res - parse(20);
    else if(next() == '*' && prio <= 20) at++, res = res * parse(30);
    else if(next() == '/' && prio <= 20) at++, res = res / parse(30);
    else if(next() == '^') at++, res = pow(res, parse(40));
    else break;
    }
  return res;
  }

EX ld parseld(const string& s) {
  exp_parser ep;
  ep.s = s;
  return real(ep.parse());
  }

EX string parser_help() {
  return XLAT("Functions available: %1", 
    "(a)sin(h), (a)cos(h), (a)tan(h), exp, log, abs, re, im, conj, let(t=...,...t...), floor, frac, e, i, pi, s, ms, mousex, mousey, mousez, shot [1 if taking screenshot/animation], to01, random, ifp(a,v,w) [if positive]");
  }

#if HDR
struct bignum {
  static const int BASE = 1000000000;
  static const long long BASE2 = BASE * (long long)BASE;
  vector<int> digits;
  bignum() {}
  bignum(int i) : digits() { digits.push_back(i); }
  void be(int i) { digits.resize(1); digits[0] = i; }
  bignum& operator +=(const bignum& b);
  void addmul(const bignum& b, int factor);
  string get_str(int max_length) const;
  bignum(ld d);
  
  bool operator < (const bignum&) const;
  bool operator > (const bignum& b) const { return b < self; }

  ld leading() const {
    switch(isize(digits)) {
      case 0:
        return 0;
      case 1:
        return digits.back();
      default:
        return digits.back() + ld(digits[isize(digits)-2]) / BASE;
      }
    }

  ld approx() const {
    return leading() * pow(BASE, isize(digits) - 1);
    }
  
  ld log_approx() const {
    return log(leading()) * log(BASE) * (isize(digits) - 1);
    }
  
  ld approx_div(const bignum& b) const {
    return leading() / b.leading() * pow(BASE, isize(digits) - isize(b.digits));
    }
  
  int approx_int() const {
    if(isize(digits) > 1) return BASE;
    if(digits.empty()) return 0;
    return digits[0];
    }
  
  bool nonzero() { return approx_ld() != 0; }
  
  bignum randomized_div(int x) const;

  ld approx_ld() const {
    ld res = 0;
    for(int i=0; i<isize(digits); i++) res += digits[i] * pow(BASE, i);
    return res;
    }
    
  long long approx_ll() const {
    if(isize(digits) > 2) return BASE2;
    if(digits.empty()) return 0;
    if(isize(digits) == 1) return digits[0];
    return digits[0] + digits[1] * (long long) BASE;
    }
  
  friend inline bignum operator +(bignum a, const bignum& b) { a.addmul(b, 1); return a; }
  friend inline bignum operator -(bignum a, const bignum& b) { a.addmul(b, -1); return a; }
  };
#endif

bignum& bignum::operator +=(const bignum& b) {
  int K = isize(b.digits);
  if(K > isize(digits)) digits.resize(K);
  int carry = 0;
  for(int i=0; i<K || carry; i++) {
    if(i >= isize(digits)) digits.push_back(0);
    digits[i] += carry;
    if(i < K) digits[i] += b.digits[i];
    if(digits[i] >= BASE) {
      digits[i] -= BASE;
      carry = 1;
      }
    else carry = 0;
    }
  return *this;
  }

bool bignum::operator < (const bignum& b) const {
  if(isize(digits) != isize(b.digits))
    return isize(digits) < isize(b.digits);
  for(int i = isize(digits)-1; i>=0; i--)
    if(digits[i] != b.digits[i])
      return digits[i] < b.digits[i];
  return false;
  }

bignum bignum::randomized_div(int x) const {
  bignum res = self;
  long long carry = 0;
  int K = isize(res.digits);
  for(int i=K-1; i>=0; i--) {
    carry *= BASE;
    carry += digits[i];
    tie(carry, res.digits[i]) = make_pair(carry % x, carry / x);
    }
  while(isize(res.digits) && res.digits.back() == 0) res.digits.pop_back();
  if(rand() % x < carry) res += 1;
  println(hlog, get_str(100), " / ", x, " = ", res.get_str(100));
  return res;
  }

void bignum::addmul(const bignum& b, int factor) {
  int K = isize(b.digits);
  if(K > isize(digits)) digits.resize(K);
  int carry = 0;
  for(int i=0; i<K || (carry > 0 || carry < -1) || (carry == -1 && i < isize(digits)); i++) {
    if(i >= isize(digits)) digits.push_back(0);
    long long l = digits[i];
    l += carry;
    if(i < K) l += b.digits[i] * factor;
    carry = 0;
    if(l >= BASE) carry = l / BASE;
    if(l < 0) carry = -(BASE-1-l) / BASE;
    l -= carry * BASE;
    digits[i] = l;
    }
  if(carry < 0) digits.back() -= BASE;
  while(isize(digits) && digits.back() == 0) digits.pop_back();
  }

EX bignum hrand(bignum b) {
  bignum res;
  int d = isize(b.digits);
  while(true) {
    res.digits.resize(d);
    for(int i=0; i<d-1; i++) res.digits[i] = hrand(bignum::BASE);
    res.digits.back() = hrand(b.digits.back() + 1);
    if(res < b) return res;
    }  
  }

EX void operator ++(bignum &b, int) {
  int i = 0;
  while(true) {
    if(isize(b.digits) == i) { b.digits.push_back(1); break; }
    else if(b.digits[i] == bignum::BASE-1) {
      b.digits[i] = 0;
      i++;
      }
    else {
      b.digits[i]++;
      break;
      }      
    }
  }

EX void operator --(bignum &b, int) {
  int i = 0;
  while(true) {
    if(isize(b.digits) == i) { b.digits.push_back(bignum::BASE-1); break; }
    else if(b.digits[i] == 0) {
      b.digits[i] = bignum::BASE-1;
      i++;
      }
    else {
      b.digits[i]--;
      break;
      }      
    }
  }

string bignum::get_str(int max_length) const {
  if(digits.empty()) return "0";
  string ret = its(digits.back());
  for(int i=isize(digits)-2; i>=0; i--) {
    if(isize(ret) > max_length && i) {
      ret += XLAT(" (%1 more digits)", its(9 * (i+1)));
      return ret;
      }

    ret += " ";
    string val = its(digits[i]);
    while(isize(val) < 9) val = "0" + val;
    ret += val;
    }
  return ret;
  }

bignum::bignum(ld d) {
  if(d == 0) return;
  int n = 1;
  while(d > BASE) d /= BASE, n++;
  digits.resize(n);
  n--;
  while(n >= 0) { digits[n] = int(d); d -= digits[n]; d *= BASE; n--; }
  }

}
