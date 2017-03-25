// Hyperbolic Rogue
// Copyright (C) 2011-2016 Zeno Rogue, see 'hyper.cpp' for details

// basic utility functions

#ifdef MOBWEB
#define MWP
#endif

#ifdef PANDORA
#define MWP
#endif

#ifdef MWP
typedef double ld;
#define LDF "%lf"
#define PLDF "lf"
#define ASINH asinh
#else
typedef long double ld;
#define LDF "%Lf"
#define PLDF "Lf"
#define ASINH asinhl
#endif

template<class T> int size(const T& x) {return int(x.size()); }
string its(int i) { char buf[64]; sprintf(buf, "%d", i); return buf; }
string fts(float x) { char buf[64]; sprintf(buf, "%4.2f", x); return buf; }
string fts3(float x) { char buf[64]; sprintf(buf, "%5.3f", x); return buf; }
string fts4(float x) { char buf[64]; sprintf(buf, "%6.4f", x); return buf; }
string cts(char c) { char buf[8]; buf[0] = c; buf[1] = 0; return buf; }
string llts(long long i) {
    // sprintf does not work on Windows IIRC
    if(i < 0) return "-" + llts(-i);
    if(i < 10) return its((int) i);
    return llts(i/10) + its(i%10);
}
string itsh(int i) {static char buf[16]; sprintf(buf, "%03X", i); return buf; }

// debug utilities

extern FILE *debugfile;
extern int debugflags;

#ifdef ANDROID
#define DEBB(r,x)
#else
#define DEBB(r,x) { if(debugfile && (!(r) || (debugflags & (r)))) { fprintf x; fflush(debugfile); } }
#endif

#define DF_INIT              0 // always display these
#define DF_MSG               0 // always display these
#define DF_STEAM             1
#define DF_GRAPH             2
#define DF_TURN              4
#define DF_FIELD             8

#ifdef PROFILING

#include <sys/time.h>
long long getms() {
  struct timeval tval;
  gettimeofday(&tval, NULL);
  return tval.tv_sec * 1000000 + tval.tv_usec;
  }

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

