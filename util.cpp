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

EX int SDL_GetTicks() {
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

EX cld lerp(cld a0, cld a1, ld x) {
  return a0 + (a1-a0) * x;
  }

EX ld ilerp(ld a0, ld a1, ld x) {
  return (x-a0) / (a1-a0);
  }

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
struct hr_parse_exception : hr_exception {
  string s;
  hr_parse_exception(const string& z) : s(z) {}
  ~hr_parse_exception() noexcept(true) {}
  };

struct exp_parser {
  string s;
  int at;
  int line_number, last_line;
  exp_parser() { at = 0; line_number = 1; last_line = 0; }
  
  string where() { 
    if(s.find('\n')) return "(line " + its(line_number) + ", pos " + its(at-last_line) + ")";
    else return "(pos " + its(at) + ")";
    }
  
  map<string, cld> extra_params;

  bool ok() { return at == isize(s); }
  char next(int step=0) { if(at >= isize(s)-step) return 0; else return s[at+step]; }
  
  char eatchar() {
    return s[at++];
    }

  bool eat(const char *c) {
    int orig_at = at;
    while(*c && *c == next()) at++, c++;
    if(*c == 0) return true;
    else at = orig_at;
    return false;
    }

  void skip_white();

  string next_token();

  char snext(int step=0) { skip_white(); return next(step); }

  vector<pair<ld, ld>> parse_with_reps();

  cld parse(int prio = 0);

  ld rparse(int prio = 0) { return validate_real(parse(prio)); }
  int iparse(int prio = 0) { return int(floor(rparse(prio) + .5)); }

  cld parsepar() {
    cld res = parse();
    force_eat(")");
    return res;
    }

  ld validate_real(cld x) {
    if(kz(imag(x))) throw hr_parse_exception("expected real number but " + lalign(-1, x) + " found at " + where());
    return real(x);
    }
  
  void force_eat(const char *c) {
    skip_white();
    if(!eat(c)) throw hr_parse_exception("expected: " + string(c) + " at " + where());
    }

  };
#endif

void exp_parser::skip_white() {
  while(next() == ' ' || next() == '\n' || next() == '\r' || next() == '\t') {
    if(next() == '\r') last_line++;
    if(next() == '\n') {
      line_number++, last_line = at;
      }
    at++;
    }
  }

string exp_parser::next_token() {
  skip_white();
  string token;
  while(true) {
    char c = next();
    if((c >= '0' && c <= '9') || (c == '.' && next(1) != '.') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_')
      token += c, at++;
    else break;
    }
  return token;
  }

vector<pair<ld, ld>> exp_parser::parse_with_reps() {
  vector<pair<ld, ld>> vals;
  vals.emplace_back(rparse(0), 1);
  while(true) {
    skip_white();
    if(eat(":^")) {
      ld rep = rparse(0);
      vals.back().second *= rep;
      }
    if(eat(",")) vals.emplace_back(rparse(0), 1);
    else break;
    }
  return vals;
  }

cld exp_parser::parse(int prio) {
  cld res;
  skip_white();
  if(eat("sin(")) res = sin(parsepar());
  else if(eat("cos(")) res = cos(parsepar());
  else if(eat("sinh(")) res = sinh(parsepar());
  else if(eat("cosh(")) res = cosh(parsepar());
  else if(eat("asin(")) res = asin(parsepar());
  else if(eat("acos(")) res = acos(parsepar());
  else if(eat("asinh(")) res = asinh(parsepar());
  else if(eat("acosh(")) res = acosh(parsepar());
  else if(eat("exp(")) res = exp(parsepar());
  else if(eat("sqrt(")) res = sqrt(parsepar());
  else if(eat("log(")) res = log(parsepar());
  else if(eat("tan(")) res = tan(parsepar());
  else if(eat("tanh(")) res = tanh(parsepar());
  else if(eat("atan(")) res = atan(parsepar());
  else if(eat("atanh(")) res = atanh(parsepar());
  else if(eat("abs(")) res = abs(parsepar());
  else if(eat("re(")) res = real(parsepar());
  else if(eat("im(")) res = imag(parsepar());
  else if(eat("conj(")) res = std::conj(parsepar());
  else if(eat("floor(")) res = floor(validate_real(parsepar()));
  else if(eat("frac(")) { res = parsepar(); res = res - floor(validate_real(res)); }
  else if(eat("to01(")) { res = parsepar(); return atan(res) / ld(M_PI) + ld(0.5); }
  else if(eat("min(")) {
    ld a = rparse(0);
    while(skip_white(), eat(",")) a = min(a, rparse(0));
    force_eat(")");
    res = a;
    }
  else if(eat("max(")) {
    ld a = rparse(0);
    while(skip_white(), eat(",")) a = max(a, rparse(0));
    force_eat(")");
    res = a;
    }
  else if(eat("edge(")) {
    ld a = rparse(0);
    force_eat(",");
    ld b = rparse(0);
    force_eat(")");
    res = edge_of_triangle_with_angles(TAU/a, M_PI/b, M_PI/b);
    }
  else if(eat("edge_angles(")) {
    cld a = rparse(0);
    force_eat(",");
    cld b = rparse(0);
    force_eat(",");
    cld c = rparse(0);
    force_eat(")");

    if (auto *angleunit = hr::at_or_null(extra_params, "angleunit")) {
      a *= *angleunit;
      b *= *angleunit;
      c *= *angleunit;
      }

    return edge_of_triangle_with_angles(real(a), real(b), real(c));
    }
  else if(eat("regradius(")) {
    ld a = rparse(0);
    force_eat(",");
    ld b = rparse(0);
    force_eat(")");
    res = edge_of_triangle_with_angles(90._deg, M_PI/a, M_PI/b);
    }
  #if CAP_ARCM
  else if(eat("arcmedge(")) {
    vector<pair<ld, ld>> vals = parse_with_reps();
    force_eat(")");
    res = euclid ? 1 : arcm::compute_edgelength(vals);
    if (auto *distunit = hr::at_or_null(extra_params, "distunit"))
      res /= *distunit;
    }
  else if(eat("arcmcurv(")) {
    vector<pair<ld, ld>> vals = parse_with_reps();
    force_eat(")");
    ld total = 0;
    for(auto p: vals) total += p.second * (180 - 360 / p.first);
    total = (360 - total) * degree;
    if(abs(total) < 1e-10) total = 0;
    res = total;
    }
  #endif
  else if(eat("ideal_angle(")) {
    ld edges = rparse(0);
    ld u = 1;
    skip_white(); if(eat(",")) u = rparse(0);
    force_eat(")");
    return arb::rep_ideal(edges, u).second;
    }
  else if(eat("ideal_edge(")) {
    ld edges = rparse(0);
    ld u = 1;
    skip_white(); if(eat(",")) u = rparse(0);
    force_eat(")");
    return arb::rep_ideal(edges, u).first;
    }
  else if(eat("regangle(")) {
    cld edgelen = parse(0);
    if (auto *distunit = hr::at_or_null(extra_params, "distunit")) {
      edgelen *= *distunit;
      }
    
    force_eat(",");
    ld edges = rparse(0);
    force_eat(")");
    ld alpha = M_PI / edges;
    if(isinf(edges)) {
      ld u = sqrt(cosh(validate_real(edgelen)) * 2 - 2);
      ld a = atan2(1, u/2);
      res = 2 * a;
      }
    else {
      ld c = asin_auto(sin_auto(validate_real(edgelen)/2) / sin(alpha));
      hyperpoint h = xpush(c) * spin(M_PI - 2*alpha) * xpush0(c);
      ld result = 2 * atan2(h);
      if(result < 0) result = -result;
      cyclefix(result, 0);
      res = result;
      }

    if (auto *angleunit = hr::at_or_null(extra_params, "angleunit"))
      res /= *angleunit;
    }
  else if(eat("test(")) {
    res = parsepar();
    println(hlog, "res = ", res, ": ", fts(real(res), 10), ",", fts(imag(res), 10));
    }
  else if(eat("ifp(")) {
    cld cond = parse(0);
    force_eat(",");
    cld yes = parse(0);
    force_eat(",");
    cld no = parsepar();
    res = real(cond) > 0 ? yes : no;
    }  
  else if(eat("ifz(")) {
    cld cond = parse(0);
    force_eat(",");
    cld yes = parse(0);
    force_eat(",");
    cld no = parsepar();
    res = abs(cond) < 1e-8 ? yes : no;
    }
  else if(eat("wallif(")) {
    cld val0 = parse(0);
    force_eat(",");
    cld val1 = parsepar();
    if(real(extra_params["p"]) >= 3.5) res = val0;
    else res = val1;
    }
  else if(eat("rgb(")) {     
    cld val0 = parse(0);
    force_eat(",");
    cld val1 = parse(0);
    force_eat(",");
    cld val2 = parsepar();
    switch(int(real(extra_params["p"]) + .5)) {
      case 1: res = val0; break;
      case 2: res = val1; break;
      case 3: res = val2; break;
      default: res = 0;
      }
    }
  else if(eat("let(")) {
    string name = next_token();
    force_eat("=");
    cld val = parse(0);
    force_eat(",");
    dynamicval<cld> d(extra_params[name], val);
    res = parsepar();
    }
  #if CAP_TEXTURE
  else if(eat("txp(")) {
    cld val = parsepar();
    res = texture::get_txp(real(val), imag(val), int(real(extra_params["p"]) + .5)-1);
    }
  #endif
  else if(next() == '(') at++, res = parsepar(); 
  else {
    string number = next_token();
    if (auto *p = hr::at_or_null(extra_params, number)) res = *p;
    else if (auto *p = hr::at_or_null(params, number)) res = (*p)->get_cld();
    else if(number == "e") res = exp(1);
    else if(number == "i") res = cld(0, 1);
    else if(number == "inf") res = HUGE_VAL;
    else if(number == "p" || number == "pi") res = M_PI;
    else if(number == "" && next() == '-') { at++; res = -parse(20); }
    else if(number == "") throw hr_parse_exception("number missing, " + where());
    else if(number == "s") res = ticks / 1000.;
    else if(number == "ms") res = ticks;
    else if(number[0] == '0' && number[1] == 'x') res = strtoll(number.c_str()+2, NULL, 16);
    else if(number == "mousex") res = mousex;
    else if(number == "deg") res = degree;
    else if(number == "ultra_mirror_dist") res = cgi.ultra_mirror_dist;
    else if(number == "psl_steps") res = cgi.psl_steps;
    else if(number == "single_step") res = cgi.single_step;
    else if(number == "step") res = hdist0(tC0(currentmap->adj(cwt.at, 0)));
    else if(number == "edgelen") { start_game(); res = hdist(get_corner_position(cwt.at, 0), get_corner_position(cwt.at, 1)); }
    else if(number == "mousey") res = mousey;
    else if(number == "random") res = randd();
    else if(number == "mousez") res = cld(mousex - current_display->xcenter, mousey - current_display->ycenter) / cld(current_display->radius, 0);
    else if(number == "shot") res = inHighQual ? 1 : 0;
    #if CAP_ARCM
    else if(number == "fake_edgelength") res = arcm::fake_current.edgelength;
    #endif
    else if(number == "MAX_EDGE") res = FULL_EDGE;
    else if(number == "MAX_VALENCE") res = 120;
    else if(number[0] >= 'a' && number[0] <= 'z') throw hr_parse_exception("unknown value: " + number);
    else if(number[0] >= 'A' && number[0] <= 'Z') throw hr_parse_exception("unknown value: " + number);
    else if(number[0] == '_') throw hr_parse_exception("unknown value: " + number);
    else { std::stringstream ss; res = 0; ss << number; ss >> res; }
    }
  while(true) {
    skip_white();
    #if CAP_ANIMATIONS
    if(next() == '.' && next(1) == '.' && prio == 0) {
      static const cld NO_DERIVATIVE(3.1, 2.5);
      vector<array<cld, 4>> rest = { make_array(res, NO_DERIVATIVE, res, NO_DERIVATIVE) };
      bool second = true;
      while(next() == '.' && next(1) == '.') {
        /* spline interpolation */
        if(next(2) == '/') {
          at += 3;
          rest.back()[second ? 3 : 1] = parse(10);
          continue;
          }
        /* sharp end */
        else if(next(2) == '|') {
          at += 3;
          rest.back()[2] = parse(10);
          rest.back()[3] = NO_DERIVATIVE;
          second = true;
          continue;
          }
        at += 2; 
        auto val = parse(10);
        rest.emplace_back(make_array(val, NO_DERIVATIVE, val, NO_DERIVATIVE));
        second = false;
        }
      ld v = ticks * (isize(rest)-1.) / anims::period;
      int vf = v;
      v -= vf;
      if(isize(rest) == 1) rest.push_back(rest[0]);
      vf %= (isize(rest)-1);
      auto& lft = rest[vf];
      auto& rgt = rest[vf+1];
      if(lft[3] == NO_DERIVATIVE && rgt[1] == NO_DERIVATIVE)
        res = lerp(lft[2], rgt[0], v);
      else if(rgt[1] == NO_DERIVATIVE)
        res = lerp(lft[2] + lft[3] * v, rgt[0], v*v);
      else if(lft[3] == NO_DERIVATIVE)
        res = lerp(lft[2], rgt[0] + rgt[1] * (v-1), (2-v)*v);
      else {
        res = lerp(lft[2] + lft[3] * v, rgt[0] + rgt[1] * (v-1), v*v*(3-2*v));
        }
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
  return ep.rparse();
  }

EX int parseint(const string& s) {
  exp_parser ep;
  ep.s = s;
  return ep.iparse();
  }

EX string available_functions() {
  return 
    "(a)sin(h), (a)cos(h), (a)tan(h), exp, log, abs, re, im, conj, let(t=...,...t...), floor, frac, sqrt, to01, random, edge(7,3), regradius(7,3), ifp(a,v,w) [if positive]";
  }

EX string available_constants() {
  return 
    "e, i, pi, s, ms, mousex, mousey, mousez, shot [1 if taking screenshot/animation]";
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
  
  #if CAP_GMP
  mpq_class as_mpq() const {
    string s = get_str(999999);
    string t;
    for(char c: s) if(c != ' ') t += c;
    return mpq_class(t);
    }
  #endif
  
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
    // strange compiler buug:
    // if I do / and %, function 'divmod' is called, and it complains on launch that divmod is unimplemented
    res.digits[i] = int(carry / x);
    carry -= res.digits[i] * (long long)(x);
    }
  while(isize(res.digits) && res.digits.back() == 0) res.digits.pop_back();
  if(rand() % x < carry) res += 1;
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
    if(l >= BASE) carry = int(l / BASE);
    if(l < 0) carry = -int((BASE-1-l) / BASE);
    l -= carry * BASE;
    digits[i] = int(l);
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

EX string short_form(bignum b) {
  if(b < 0) return "-" + short_form(0-b);
  else if(b < 100000) return its(b.approx_int());
  else {
    long long val;
    int q;
    if(isize(b.digits) >= 2) {
      q = max(isize(b.digits) - 2, 0);
      val = b.digits[q] + (long long)(bignum::BASE) * b.digits[q+1];
      }
    else {
      q = 0;
      val = b.digits[0];
      }

    int digits = q * 9;
    while(val >= 1000) { val /= 10; digits++; }
    string str = its(int(val)) + "E" + its(digits + 2);
    str.insert(1, ".");
    return str;
    }
  }

bignum::bignum(ld d) {
  if(d == 0) return;
  int n = 1;
  while(d > BASE) d /= BASE, n++;
  digits.resize(n);
  n--;
  while(n >= 0) { digits[n] = int(d); d -= digits[n]; d *= BASE; n--; }
  }

/** in s, replace occurences of a with b */
EX void replace_str(string& s, string a, string b) {
  while(s.find(a) != string::npos)
    s.replace(s.find(a), isize(a), b);
  }

#if CAP_ZLIB
/* compression/decompression */

EX string compress_string(string s) {
  z_stream strm;
  strm.zalloc = Z_NULL;
  strm.zfree = Z_NULL;
  strm.opaque = Z_NULL;
  println(hlog, "pre init");
  auto ret = deflateInit(&strm, 9);
  if(ret != Z_OK) throw hr_exception("z-error");
  println(hlog, "init ok");
  strm.avail_in = isize(s);
  strm.next_in = (Bytef*) &s[0];
  vector<char> buf(10000000, 0);
  strm.avail_out = 10000000;
  strm.next_out = (Bytef*) &buf[0];
  if(deflate(&strm, Z_FINISH) != Z_STREAM_END) throw hr_exception("z-error-2");
  println(hlog, "deflate ok");
  string out(&buf[0], (char*)(strm.next_out) - &buf[0]);
  deflateEnd(&strm);
  println(hlog, isize(s), " -> ", isize(out));
  return out;
  }

EX string decompress_string(string s) {
  z_stream strm;
  strm.zalloc = Z_NULL;
  strm.zfree = Z_NULL;
  strm.opaque = Z_NULL;
  auto ret = inflateInit(&strm);
  if(ret != Z_OK) throw hr_exception("z-error");
  strm.avail_in = isize(s);
  strm.next_in = (Bytef*) &s[0];
  vector<char> buf(10000000, 0);
  strm.avail_out = 10000000;
  strm.next_out = (Bytef*) &buf[0];
  if(inflate(&strm, Z_FINISH) != Z_STREAM_END) throw hr_exception("z-error-2");  
  string out(&buf[0], (char*)(strm.next_out) - &buf[0]);
  inflateEnd(&strm);
  println(hlog, isize(s), " -> ", isize(out));
  return out;
  }
#endif

EX bool file_exists(string fname) {
  return access(fname.c_str(), F_OK) != -1;
  }

/** find a file named s, possibly in HYPERPATH */
EX string find_file(string s) {
  string s1;
  if(file_exists(s)) return s;
  char *p = getenv("HYPERPATH");
  if(p && file_exists(s1 = s0 + p + s)) return s1;
  if(file_exists(s1 = HYPERPATH + s)) return s1;
#ifdef FHS
  if(file_exists(s1 = "/usr/share/hyperrogue/" + s)) return s1;
#endif
  return s;
  }

EX void open_url(string s) {
  #if ISWEB
  EM_ASM_({
    window.open(UTF8ToString($0, 1000));
    }, s.c_str());
  #else

#ifdef WINDOWS  
  ShellExecute(0, 0, s.c_str(), 0, 0, SW_SHOW);
#endif

#ifdef LINUX
  ignore(system(("xdg-open "+s).c_str()));
#endif

#ifdef MAC
  ignore(system(("open "+s).c_str()));
#endif
#endif
  }
  
const char *urlhex = "0123456789ABCDEF";
EX void open_wiki(const char *title) {
  // Since "Crossroads" is ambiguous, we use the direct link to Crossroads I.
  if (!strcmp(title, "Crossroads")) {
    title = "Crossroads (Land)";
  }

  string url = "https://hyperrogue.miraheze.org/wiki/";
  unsigned char c;
  for (size_t i = 0; (c = title[i]); ++i) {
    if (('0' <= c && c <= '9') || ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || c == '_' || c == '-')
      url += c;
    else if (c == ' ')
      url += "_";
    else
      url += string("%") + urlhex[c/16] + urlhex[c%16];
    }
  open_url(url);
}

EX string read_file_as_string(string fname) {
  string buf;
  #if ISANDROID || ISIOS
  buf = get_asset(fname);
  #else
  FILE *f = fopen(fname.c_str(), "rb");
  if(!f) f = fopen((rsrcdir + fname).c_str(), "rb");
  buf.resize(1000000);
  int qty = fread(&buf[0], 1, 1000000, f);
  buf.resize(qty);
  fclose(f);
  #endif
  return buf;
  }

EX void floyd_warshall(vector<vector<char>>& v) {
  int N = isize(v);
  for(int k=0; k<N; k++)
  for(int i=0; i<N; i++)
  for(int j=0; j<N; j++)
    v[i][j] = min<int>(v[i][j], v[i][k] + v[k][j]);
  }
}
