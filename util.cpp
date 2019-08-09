// Hyperbolic Rogue
// Copyright (C) 2011-2018 Zeno Rogue, see 'hyper.cpp' for details

// basic utility functions

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

EX int gcd(int i, int j) {
  return i ? gcd(j%i, i) : j;
  }

EX int gmod(int i, int j) {
  i %= j; if(i<0) i += j;
  return i;
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

void profile_start(int t) { proftable[t][pframeid] -= getms(); }
void profile_stop(int t) { proftable[t][pframeid] += getms(); }

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

#else

#define profile_frame()
#define profile_start(t)
#define profile_stop(t)
#define profile_info()
#endif

purehookset hooks_tests;

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
    "(a)sin(h), (a)cos(h), (a)tan(h), exp, log, abs, re, im, conj, let(t=...,...t...), floor, frac, e, i, pi, s, ms, mousex, mousey, mousez, shot [1 if taking screenshot/animation], to01, ifp(a,v,w) [if positive]");
  }

}
