// Hyperbolic Rogue -- HyperRogue streams
// Copyright (C) 2011-2018 Zeno Rogue, see 'hyper.cpp' for details

/** \file hprint.cpp
 *  \brief Routines related to displaying various things, and writing them to console and files. Nicer than the standard C++ streams.
 */

#include "hyper.h"
namespace hr {

EX FILE *debugfile;

#if HDR
#define DF_INIT              1 // always display these
#define DF_MSG               2 // always display these
#define DF_WARN              4 // always display these
#define DF_ERROR             8 // always display these
#define DF_STEAM            16
#define DF_GRAPH            32
#define DF_TURN             64
#define DF_FIELD           128
#define DF_GEOM            256
#define DF_MEMORY          512
#define DF_TIME           1024 // a flag to display timestamps
#define DF_GP             2048
#define DF_POLY           4096
#define DF_LOG            8192
#define DF_VERTEX        16384
#define DF_KEYS "imwesxufgbtoplv"
#endif

EX int debugflags = DF_INIT | DF_ERROR | DF_WARN | DF_MSG | DF_TIME | DF_LOG;

EX string s0;

EX string its(int i) { char buf[64]; sprintf(buf, "%d", i); return buf; }

EX string itsh8(int i) {static char buf[16]; sprintf(buf, "%08X", i); return buf; }

EX string fts(ld x, int prec IS(6)) {
  std::stringstream ss;
  ss.precision(prec);
  ss << x;
  return ss.str();
  }

EX map<void*, int> pointer_indices;

EX string index_pointer(void *v) {
  if(v == nullptr) return "0";
  if(!pointer_indices.count(v)) {
    int s = isize(pointer_indices);
    pointer_indices[v] = s;
    }
  int i = pointer_indices[v];
  string res;
  while(true) { res += ('A' + (i % 26)); i /= 26; if(!i) break; i--; }
  return res;
  }

EX int stamplen = 6;

EX string get_stamp() {
  if(stamplen == 0) return "";
  int t = SDL_GetTicks();
  int pow10 = 1;
  for(int i=0; i<stamplen; i++) pow10 *= 10;
  if(t < 0) t = pow10 - 1;
  t %= pow10;
  string s = its(t);
  while(isize(s) < stamplen) s = "0" + s;
  return s;
  }

#if HDR
inline string ONOFF(bool b) { return b ? XLAT("ON") : XLAT("OFF"); }

struct hstream {
  color_t vernum;
  virtual void write_char(char c) = 0;
  virtual void write_chars(const char* c, size_t q) { while(q--) write_char(*(c++)); }
  virtual char read_char() = 0;
  virtual void read_chars(char* c, size_t q) { while(q--) *(c++) = read_char(); }
  virtual color_t get_vernum() { return vernum; }
  virtual void flush() {}
  
  hstream() { vernum = VERNUM_HEX; }

  template<class T> void write(const T& t) { hwrite(*this, t); }
  template<class T> void read(T& t) { hread(*this, t); }
  template<class T> T get() { T t; hread(*this, t); return t; }
  template<class T> T get_raw() { T t; hread_raw(*this, t); return t; }
  };

template<class T> void hwrite_raw(hstream& hs, const T& c) { hs.write_chars((char*) &c, sizeof(T)); }
template<class T> void hread_raw(hstream& hs, T& c) { hs.read_chars((char*) &c, sizeof(T)); }

template<class T, typename = typename std::enable_if<std::is_integral<T>::value || std::is_enum<T>::value>::type> void hwrite(hstream& hs, const T& c) { hwrite_raw(hs, c); }
template<class T, typename = typename std::enable_if<std::is_integral<T>::value || std::is_enum<T>::value>::type> void hread(hstream& hs, T& c) { hread_raw(hs, c); }

inline void hwrite(hstream& hs, const string& s) {
  if(isize(s) >= 255) {
    hs.write_char((char)255);
    hs.write<int>(isize(s));
    }
  else 
    hs.write_char(isize(s));    
  for(char c: s) hs.write_char(c);
  }
inline void hread(hstream& hs, string& s) {
  s = ""; int l = (unsigned char) hs.read_char(); 
  if(l == 255) l = hs.get<int>();
  for(int i=0; i<l; i++) s += hs.read_char();
  }
inline void hwrite(hstream& hs, const ld& h) { double d = h; hs.write_chars((char*) &d, sizeof(double)); }
inline void hread(hstream& hs, ld& h) { double d; hs.read_chars((char*) &d, sizeof(double)); h = d; }
  
template<class T, size_t X> void hwrite(hstream& hs, const array<T, X>& a) { for(auto &ae: a) hwrite(hs, ae); }
template<class T, size_t X> void hread(hstream& hs, array<T, X>& a) { for(auto &ae: a) hread(hs, ae); }

inline void hread(hstream& hs, hyperpoint& h) { for(int i=0; i<MDIM; i++) hread(hs, h[i]); }
inline void hwrite(hstream& hs, hyperpoint h) { for(int i=0; i<MDIM; i++) hwrite(hs, h[i]); }

template<class T> void hwrite(hstream& hs, const vector<T>& a) { hwrite<int>(hs, isize(a)); for(auto &ae: a) hwrite(hs, ae); }
template<class T> void hread(hstream& hs, vector<T>& a) { a.resize(hs.get<int>()); for(auto &ae: a) hread(hs, ae); }

template<class T, class U> void hwrite(hstream& hs, const map<T,U>& a) { 
  hwrite<int>(hs, isize(a)); for(auto &ae: a) hwrite(hs, ae.first, ae.second);
  }
template<class T, class U> void hread(hstream& hs, map<T,U>& a) { 
  a.clear();
  int N = hs.get<int>();
  for(int i=0; i<N; i++) {
    T key; hread(hs, key);
    hread(hs, a[key]);
    }
  }

template<class C, class C1, class... CS> void hwrite(hstream& hs, const C& c, const C1& c1, const CS&... cs) { hwrite(hs, c); hwrite(hs, c1, cs...); }
template<class C, class C1, class... CS> void hread(hstream& hs, C& c, C1& c1, CS&... cs) { hread(hs, c); hread(hs, c1, cs...); }

struct hstream_exception : hr_exception { hstream_exception() {} };

struct fhstream : hstream {
  FILE *f;
  explicit fhstream() { f = NULL; }
  explicit fhstream(const string pathname, const char *mode) { f = fopen(pathname.c_str(), mode); vernum = VERNUM_HEX; }
  ~fhstream() { if(f) fclose(f); }
  void write_char(char c) override { write_chars(&c, 1); }
  void write_chars(const char* c, size_t i) override { if(fwrite(c, i, 1, f) != 1) throw hstream_exception(); }
  void read_chars(char* c, size_t i) override { if(fread(c, i, 1, f) != 1) throw hstream_exception(); }
  char read_char() override { char c; read_chars(&c, 1); return c; }
  virtual void flush() override { fflush(f); }
  };

struct shstream : hstream { 
  string s;
  int pos;
  explicit shstream(const string& t = "") : s(t) { pos = 0; vernum = VERNUM_HEX; }
  void write_char(char c) override { s += c; }
  char read_char() override { if(pos == isize(s)) throw hstream_exception(); return s[pos++]; }
  };

inline void print(hstream& hs) {}

template<class... CS> string sprint(const CS&... cs) { shstream hs; print(hs, cs...); return hs.s; }

template<class C, class C1, class... CS> void print(hstream& hs, const C& c, const C1& c1, const CS&... cs) { print(hs, c); print(hs, c1, cs...); }

template<class... CS> void println(hstream& hs, const CS&... cs) { print(hs, cs...); hs.write_char('\n'); }

inline string spaced(int i) { return its(i); }
inline string spaced(color_t col) { return itsh8(col); }
inline string spaced(const string& s) { return s; }
inline string spaced(ld x) { return fts(x, 10); }
template<class T> string spaced_of(T a[], int q) { string s = spaced(a[0]); for(int i=1; i<q; i++) s += ' ', s += spaced(a[i]); return s; }
template<class T, int i> string spaced(const array<T,i>& a) { return spaced_of(&a[0], isize(a)); }
template<class C, class C1, class... CS> string spaced(const C& c, const C1& c1, const CS&... cs) { return spaced(c) + " " + spaced(c1, cs...); }

bool scan(fhstream& hs, int&);
bool scan(fhstream& hs, ld&);
bool scan(fhstream& hs, string&);
bool scan(fhstream& hs, color_t& c);
template<class C, class C1, class... CS> bool scan(fhstream& hs, C& c, C1& c1, CS&... cs) { return scan(hs, c) && scan(hs, c1, cs...); }

string scanline(fhstream& hs);
template<class T> T scan(fhstream& hs) { T t {}; scan(hs, t); return t; }

// copied from: https://stackoverflow.com/questions/16387354/template-tuple-calling-a-function-on-each-element

namespace detail
{
    template<int... Is>
    struct seq { };

    template<int N, int... Is>
    struct gen_seq : gen_seq<N - 1, N - 1, Is...> { };

    template<int... Is>
    struct gen_seq<0, Is...> : seq<Is...> { };

    template<typename T, typename F, int... Is>
    void for_each(T&& t, F f, seq<Is...>)
    {
        auto l = { (f(std::get<Is>(t)), 0)... }; ignore(l);
    }
}

template<typename... Ts, typename F>
void for_each_in_tuple(std::tuple<Ts...> const& t, F f)
{
    detail::for_each(t, f, detail::gen_seq<sizeof...(Ts)>());
}

inline void print(hstream& hs, const string& s) { hs.write_chars(s.c_str(), isize(s)); }
inline void print(hstream& hs, int i) { print(hs, its(i)); }
inline void print(hstream& hs, ld x) { print(hs, fts(x, 6)); }
inline void print(hstream& hs, color_t col) { print(hs, itsh8(col)); }

template<class T> void print(hstream& hs, const walker<T>& w) { print(hs, "[", w.at, "/", w.spin, "/", w.mirrored, "]"); }

struct comma_printer {
  bool first;
  hstream& hs;
  template<class T> void operator() (const T& t) { if(first) first = false; else print(hs, ","); print(hs, t); }
  comma_printer(hstream& hs) : first(true), hs(hs) {}
  };

template<class T, size_t X> void print(hstream& hs, const array<T, X>& a) { print(hs, "("); comma_printer c(hs); for(const T& t: a) c(t); print(hs, ")"); }
template<class T> void print(hstream& hs, const vector<T>& a) { print(hs, "("); comma_printer c(hs); for(const T& t: a) c(t); print(hs, ")"); }

template<class T, class U> void print(hstream& hs, const map<T,U>& a) { print(hs, "("); comma_printer c(hs); for(auto& t: a) c(t); print(hs, ")"); }
template<class T> void print(hstream& hs, const set<T>& a) { print(hs, "("); comma_printer c(hs); for(auto& t: a) c(t); print(hs, ")"); }

template<class T> string separated(string separator, const vector<T>& a) {
  shstream ss;
  bool first = true;
  for(auto& v: a) { if(first) first = false; else print(ss, separator); print(ss, v); }
  return ss.s;
  }

inline void print(hstream& hs, const hyperpoint h) { print(hs, "("); for(int i=0; i<MDIM; i++) { if(i) print(hs, ","); print(hs, h[i]); } print(hs, ")"); } // (const array<ld, MAXMDIM>&)h); }
inline void print(hstream& hs, const transmatrix T) { 
  print(hs, "("); comma_printer c(hs);
  for(int i=0; i<MDIM; i++)
  for(int j=0; j<MDIM; j++) c(T[i][j]);
  print(hs, ")"); }

inline void print(hstream& hs, const shiftpoint h) { print(hs, h.h, "@", h.shift); }
inline void print(hstream& hs, const shiftmatrix T) { print(hs, T.T, "@", T.shift); }

template<class T, class U> void print(hstream& hs, const pair<T, U> & t) { print(hs, "(", t.first, ",", t.second, ")"); }

template<class... T> void print(hstream& hs, const tuple<T...> & t) { 
  print(hs, "(");
  comma_printer p(hs);
  for_each_in_tuple(t, p);
  print(hs, ")");
  }

#ifndef SPECIAL_LOGGER
inline void special_log(char c) { if(debugfile) fputc(c, debugfile); putchar(c); }
#endif

#if !CAP_SDL && CAP_TIMEOFDAY
int SDL_GetTicks();
#endif

struct logger : hstream {
  int indentation;
  bool doindent;
  explicit logger() { doindent = false; }
  void write_char(char c) override;  
  char read_char() override { throw hstream_exception(); }
  void flush() override { fflush(stdout); }
  };

extern logger hlog;
template<class... T> void println_log(T... t) { println(hlog, t...); }
template<class... T> void print_log(T... t) { print(hlog, t...); }

#ifdef __GNUC__
__attribute__((__format__ (__printf__, 1, 2)))
#endif
inline string format(const char *fmt, ...) {
  char buf[1000];
  va_list ap;
  va_start(ap, fmt);
  vsnprintf(buf, 1000, fmt, ap);
  va_end(ap);
  return buf;
  }

inline void print(hstream& hs, heptagon* h) { print(hs, "H", index_pointer(h)); }
inline void print(hstream& hs, cell* h) { print(hs, "C", index_pointer(h)); }
inline void print(hstream& hs, hrmap* h) { print(hs, "M", index_pointer(h)); }

inline void print(hstream& hs, cellwalker cw) { 
  if(cw.at) print(hs, "[", cw.at, "/", cw.at->type, ":", cw.spin, ":", cw.mirrored, "]");
  else print(hs, "[NULL]");
  }

struct indenter {
  dynamicval<int> ind;
  
  explicit indenter(int i = 2) : ind(hlog.indentation, hlog.indentation + (i)) {}
  };

struct indenter_finish : indenter {
  explicit indenter_finish(bool b = true): indenter(b ? 2:0) {}
  explicit indenter_finish(string s): indenter(2) {
    indenter tmp(-2);
    println(hlog, s);
    }
  ~indenter_finish() { if(hlog.indentation != ind.backup) println(hlog, "(done)"); }
  };

#endif

void logger::write_char(char c) { 
  if(doindent) { 
    doindent = false; 
    if(debugflags & DF_TIME) { 
      string s = get_stamp(); 
      if(s != "") { for(char c: s) special_log(c); special_log(' '); }
      }
    for(int i=0; i<indentation; i++) special_log(' ');
    }
  special_log(c); 
  if(c == 10) {
    doindent = true;
    if(debugfile) fflush(debugfile); 
    }
  }

EX void print(hstream& hs, cld x) { 
  int parts = 0;
  if(kz(real(x))) {
    print(hs, real(x)); 
    parts++;
    }
  
  if(kz(imag(x))) {
    if(parts && imag(x) > 0) print(hs, "+");
    parts++;
    print(hs, imag(x), "i"); 
    }
  
  if(!parts) print(hs, 0);
  }

EX string fts_fixed(ld x, int prec IS(6)) {
  std::stringstream ss;
  ss.precision(prec);
  ss << std::fixed << x;
  return ss.str();
  }

bool scan(fhstream& hs, int& i) { return fscanf(hs.f, "%d", &i) == 1; }
bool scan(fhstream& hs, color_t& c) { return fscanf(hs.f, "%x", &c) == 1; }
bool scan(fhstream& hs, ld& x) { return fscanf(hs.f, "%lf", &x) == 1; }
bool scan(fhstream& hs, string& s) { char t[10000]; t[0] = 0; int err = fscanf(hs.f, "%9500s", t); s = t; return err == 1 && t[0]; }
string scanline(fhstream& hs) { char buf[10000]; buf[0] = 0; ignore(fgets(buf, 10000, hs.f)); return buf; }

EX string scanline_noblank(fhstream& hs) {
  string s = scanline(hs);
  while(s.size() && among(s.back(), '\t', '\r', ' ', '\n')) s.pop_back();
  while(s.size() && among(s[0], '\t', '\r', ' ', '\n')) s = s.substr(1);
  return s;
  }

/*
string fts_smartdisplay(ld x, int maxdisplay) {
  string rv;
  if(x > 1e9 || x < -1e9) retrun fts(x);
  if(x<0) { rv = "-"; x = -x; }
  int i = int(x);
  rv += its(i);
  x -= i;
  bool nonzero = i;
  if(x == 0) return rv;
  if(x < 1e-9 && nonzero) return rv;
  rv += ".";
  while(maxdisplay > 0) {
    x *= 10;
    rv += '0' + int(x);
    if(int(x)) nonzero = true;
    x -= int(x);
    if(x == 0) return rv;
    if(x < 1e-9 && nonzero) return rv;
    maxdisplay--;
    }
  } */

EX string cts(char c) { char buf[8]; buf[0] = c; buf[1] = 0; return buf; }
EX string llts(long long i) {
    // sprintf does not work on Windows IIRC
    if(i < 0) return "-" + llts(-i);
    if(i < 10) return its((int) i);
    return llts(i/10) + its(i%10);
}
EX string itsh(unsigned int i) {static char buf[16]; sprintf(buf, "%03X", i); return buf; }
EX string itsh(int i) {static char buf[16]; sprintf(buf, "%03X", i); return buf; }
EX string itsh2(int i) {static char buf[16]; sprintf(buf, "%02X", i); return buf; }

EX string itsh(unsigned long long i) {
  int i0 = int(i);
  int i1 = int(i >> 32);
  if(i1) return itsh(i1) + itsh8(i0);
  else return itsh(i0);
  }

EX logger hlog;

// kz: utility for printing
// if it is close to 0, assume it is floating errors

EX ld kz(ld x) {
  if(abs(x) < 1e-6) return 0;
  return x;
  }

EX hyperpoint kz(hyperpoint h) {
  for(int d=0; d<MAXMDIM; d++) h[d] = kz(h[d]);
  return h;
  }

EX transmatrix kz(transmatrix h) {
  for(int d=0; d<MAXMDIM; d++) 
  for(int e=0; e<MAXMDIM; e++)
    h[d][e] = kz(h[d][e]);
  return h;
  }

#if HDR
template<class T> vector<T> kz(vector<T> v) {
  for(auto& el: v) el = kz(el);
  return v;
  }
#endif

EX string pick123() { return cts('1' + rand() % 3); }
EX string pick12() { return cts('1' + rand() % 2); }

#if HDR
template<class T> string serialize(const T& data) {
  shstream shs;
  hwrite(shs, data);
  return shs.s;
  }

template<class T> T deserialize(const string& s) {
  shstream shs;
  shs.s = s;
  T data;
  hread(shs, data);
  return data;
  }
#endif

EX string as_hexstring(string o) {
  string res;
  for(char x: o) {
    char buf[4];
    sprintf(buf, "%02X", (unsigned char)(x));
    res += buf;
    }
  return res;
  }

EX string from_hexstring(string o) {
  string res;
  for(int i=0; i<isize(o); i+=2) {
    char buf[4];
    buf[0] = o[i];
    buf[1] = o[i+1];
    buf[2] = 0;
    unsigned x;
    sscanf(buf, "%02X", &x);
    res += char(x);
    }
  return res;
  }

EX string as_cstring(string o) {
  string s = "string(\"";
  for(char c: o)
    s += format("\\x%02x", (unsigned char) c);
  s += format("\", %d)", isize(o));
  return s;
  }

EX string as_nice_cstring(string o) {
  string s = "\"";
  for(char c: o)
    if(c >= 32 && c < 126)
      s += c;
    else if(c == 10)
      s += "\\n";
    else
      s += format("\\x%02x", (unsigned char) c);
  s += "\"";
  return s;
  }

#if HDR
#if ISANDROID
#define DEBB(r,x)
#define DEBB0(r,x)
#define DEBBI(r,x)
#else
#define DEBB(r,x) { if(debugflags & (r)) { println_log x; } }
#define DEBB0(r,x) { if(debugflags & (r)) { print_log x; } }
#define DEBBI(r,x) { if(debugflags & (r)) { println_log x; } } indenter_finish _debbi(debugflags & (r));
#endif
#endif

#if CAP_GMP
EX string its(mpq_class x) { std::stringstream ss; ss << x; return ss.str(); }
EX void print(hstream& hs, const mpq_class& x) {
  std::stringstream ss; ss << x; print(hs, ss.str()); 
  }
#endif

#if HDR
template<class... T> string lalign(int len, T... t) {
  shstream hs;
  print(hs, t...);
  while(isize(hs.s) < len) hs.s += " ";
  return hs.s;
  }
#endif
}
