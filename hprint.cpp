// Hyperbolic Rogue -- HyperRogue streams
// Copyright (C) 2011-2018 Zeno Rogue, see 'hyper.cpp' for details

/** \file hprint.cpp
 *  \brief Routines related to displaying various things, and writing them to console and files. Nicer than the standard C++ streams.
 */

namespace hr {

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
#define DF_KEYS "imwesxufgbtopl"
#endif

EX string its(int i) { char buf[64]; sprintf(buf, "%d", i); return buf; }

EX string itsh8(int i) {static char buf[16]; sprintf(buf, "%08X", i); return buf; }

EX string fts(ld x, int prec IS(6)) {
  std::stringstream ss;
  ss.precision(prec);
  ss << x;
  return ss.str();
  }

#if HDR
inline string ONOFF(bool b) { return XLAT(b ? "ON" : "OFF"); }

struct hstream {
  virtual void write_char(char c) = 0;
  virtual void write_chars(const char* c, size_t q) { while(q--) write_char(*(c++)); }
  virtual char read_char() = 0;
  virtual void read_chars(char* c, size_t q) { while(q--) *(c++) = read_char(); }
  virtual color_t get_vernum() { return VERNUM_HEX; }

  template<class T> void write(const T& t) { hwrite(*this, t); }
  template<class T> void read(T& t) { hread(*this, t); }
  template<class T> T get() { T t; hread(*this, t); return t; }
  template<class T> T get_raw() { T t; hread_raw(*this, t); return t; }
  };

template<class T> void hwrite_raw(hstream& hs, const T& c) { hs.write_chars((char*) &c, sizeof(T)); }
template<class T> void hread_raw(hstream& hs, T& c) { hs.read_chars((char*) &c, sizeof(T)); }

template<class T, typename = typename std::enable_if<std::is_integral<T>::value || std::is_enum<T>::value>::type> void hwrite(hstream& hs, const T& c) { hwrite_raw(hs, c); }
template<class T, typename = typename std::enable_if<std::is_integral<T>::value || std::is_enum<T>::value>::type> void hread(hstream& hs, T& c) { hread_raw(hs, c); }

inline void hwrite(hstream& hs, const string& s) { hs.write_char(isize(s)); for(char c: s) hs.write_char(c); }  
inline void hread(hstream& hs, string& s) { s = ""; int l = (unsigned char) hs.read_char(); for(int i=0; i<l; i++) s += hs.read_char(); }
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
  color_t vernum;
  virtual color_t get_vernum() override { return vernum; }
  FILE *f;
  virtual void write_char(char c) override { write_chars(&c, 1); }
  virtual void write_chars(const char* c, size_t i) override { if(fwrite(c, i, 1, f) != 1) throw hstream_exception(); }
  virtual void read_chars(char* c, size_t i) override { if(fread(c, i, 1, f) != 1) throw hstream_exception(); }
  virtual char read_char() override { char c; read_chars(&c, 1); return c; }
  fhstream() { f = NULL; vernum = VERNUM_HEX; }
  fhstream(const string pathname, const char *mode) { f = fopen(pathname.c_str(), mode); vernum = VERNUM_HEX; }
  ~fhstream() { if(f) fclose(f); }
  };

struct shstream : hstream { 
  string s;
  int pos;
  shstream() { pos = 0; }
  virtual void write_char(char c) { s += c; }
  virtual char read_char() { if(pos == isize(s)) throw hstream_exception(); return s[pos++]; }
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

inline void print(hstream& hs, const hyperpoint h) { print(hs, (const array<ld, MAXMDIM>&)h); }
inline void print(hstream& hs, const transmatrix T) { 
  print(hs, "("); comma_printer c(hs);
  for(int i=0; i<MDIM; i++)
  for(int j=0; j<MDIM; j++) c(T[i][j]);
  print(hs, ")"); }

template<class T, class U> void print(hstream& hs, const pair<T, U> & t) { print(hs, "(", t.first, ",", t.second, ")"); }

template<class... T> void print(hstream& hs, const tuple<T...> & t) { 
  print(hs, "(");
  comma_printer p(hs);
  for_each_in_tuple(t, p);
  print(hs, ")");
  }

FILE *debugfile;

#ifndef SPECIAL_LOGGER
inline void special_log(char c) { if(debugfile) fputc(c, debugfile); putchar(c); }
#endif

#if !CAP_SDL && !ISFAKEMOBILE
int SDL_GetTicks();
#endif

struct logger : hstream {
  int indentation;
  bool doindent;
  logger() { doindent = false; }
  virtual void write_char(char c) { if(doindent) { doindent = false; 
    if(debugflags & DF_TIME) { int t = SDL_GetTicks(); if(t < 0) t = 999999; t %= 1000000; string s = its(t); while(isize(s) < 6) s = "0" + s; for(char c: s) special_log(c); special_log(' '); }
    for(int i=0; i<indentation; i++) special_log(' '); } special_log(c); if(c == 10) doindent = true; if(c == 10 && debugfile) fflush(debugfile); }
  virtual char read_char() { throw hstream_exception(); }
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

inline void print(hstream& hs, heptagon* h) { print(hs, format("H%p", h)); }
inline void print(hstream& hs, cell* h) { print(hs, format("C%p", h)); }

inline void print(hstream& hs, cellwalker cw) { 
  if(cw.at) print(hs, "[", cw.at, "/", cw.at->type, ":", cw.spin, ":", cw.mirrored, "]");
  else print(hs, "[NULL]");
  }

struct indenter {
  dynamicval<int> ind;
  
  indenter(int i = 2) : ind(hlog.indentation, hlog.indentation + (i)) {}
  };

struct indenter_finish : indenter {
  indenter_finish(bool b): indenter(b ? 2:0) {}
  ~indenter_finish() { if(hlog.indentation != ind.backup) println(hlog, "(done)"); }
  };

#endif

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
string scanline(fhstream& hs) { char buf[10000]; buf[0] = 0; fgets(buf, 10000, hs.f); return buf; }

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
EX string itsh(int i) {static char buf[16]; sprintf(buf, "%03X", i); return buf; }
EX string itsh2(int i) {static char buf[16]; sprintf(buf, "%02X", i); return buf; }

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

EX string pick123() { return cts('1' + rand() % 3); }
EX string pick12() { return cts('1' + rand() % 2); }

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

}
