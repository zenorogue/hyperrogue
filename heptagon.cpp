// Hyperbolic Rogue
// Copyright (C) 2011-2012 Zeno Rogue, see 'hyper.cpp' for details

// heptagon here refers to underlying heptagonal tesselation
// (which you can see by changing the conditions in graph.cpp)

#define MIRR(x) x.mirrored

// automaton state
enum hstate { hsOrigin, hsA, hsB, hsError, hsA0, hsA1, hsB0, hsB1, hsC };

int fixrot(int a) { return (a+490)% S7; }
int fix42(int a) { return (a+420)% S42; }

struct heptagon;

struct cell;
cell *newCell(int type, heptagon *master);

// spintable functions

int tspin(uint32_t& t, int d) {
  return (t >> (d<<2)) & 7;
  }

int tmirror(uint32_t& t, int d) {
  return (t >> ((d<<2)+3)) & 1;
  }

void tsetspin(uint32_t& t, int d, int spin) {
  t &= ~(15 << (d<<2));
  t |= spin << (d<<2);
  }

struct heptagon {
  // automaton state
  hstate s : 8;
  // we are spin[i]-th neighbor of move[i]
  uint32_t spintable;
  int spin(int d) { return tspin(spintable, d); }
  int mirror(int d) { return tmirror(spintable, d); }
  void setspin(int d, int sp) { tsetspin(spintable, d, sp); }
  // neighbors; move[0] always goes towards origin,
  // and then we go clockwise
  heptagon* move[7];
  // distance from the origin
  short distance;
  // emerald/wineyard generator
  short emeraldval;
  // fifty generator
  short fiftyval;
  // zebra generator (1B actually)
  short zebraval;
  // field id
  int fieldval;
  // evolution data
  short rval0, rval1;
  struct cdata *cdata;
  // central cell
  cell *c7;
  // associated generator of alternate structure, for Camelot and horocycles
  heptagon *alt;
  // functions
  heptagon*& modmove(int i) { return move[fixrot(i)]; }
  unsigned char gspin(int i) { return spin(fixrot(i)); }
  };

// the automaton is used to generate each heptagon in an unique way
// (you can see the tree obtained by changing the conditions in graph.cpp)
// from the origin we can go further in any direction, and from other heptagons
// we can go in directions 3 and 4 (0 is back to origin, so 3 and 4 go forward),
// and sometimes in direction 5

hstate transition(hstate s, int dir) {
  if(sphere) {
    if(S7 == 4) {
      if(s == hsOrigin) return dir == 0 ? hsB0 : hsB1;
      }
    if(S7 == 3) {
      if(s == hsOrigin) return hsB1;
      }
    if(s == hsOrigin) return dir == 0 ? hsA0 : hsA1;
    if(s == hsA0 && dir == 2) return hsB0;
    if(s == hsA1 && dir == 2) return hsB1;
    if(s == hsB0 && dir == S7-2) return hsC;
    return hsError;
    }
  else {
    if(s == hsOrigin) return hsA;
    if(s == hsA && dir >= 3 && dir <= 4) return hsA;
    if(s == hsA && dir == 5) return hsB;
    if(s == hsB && dir == 4) return hsB;
    if(s == hsB && dir == 3) return hsA;
    }
  return hsError;
  }

// create h->move[d] if not created yet
heptagon *createStep(heptagon *h, int d);

// create a new heptagon
heptagon *buildHeptagon(heptagon *parent, int d, hstate s, int pard = 0) {
  heptagon *h = new heptagon;
  h->alt = NULL;
  h->s = s;
  for(int i=0; i<7; i++) h->move[i] = NULL;
  h->spintable = 0;
  h->move[pard] = parent; tsetspin(h->spintable, pard, d);
  parent->move[d] = h; tsetspin(parent->spintable, d, pard);
  if(parent->c7) {
    h->c7 = newCell(7, h);
    h->emeraldval = emerald_heptagon(parent->emeraldval, d);
    h->zebraval = zebra_heptagon(parent->zebraval, d);
    h->fieldval = fp43.connections[fieldpattern::btspin(parent->fieldval, d)];
    h->rval0 = h->rval1 = 0; h->cdata = NULL;
    if(parent->s == hsOrigin)
      h->fiftyval = fiftytable[0][d];
    else
      h->fiftyval = nextfiftyval(parent->fiftyval, parent->move[0]->fiftyval, d);
    }
  else {
    h->c7 = NULL;
    h->emeraldval = 0;
    h->fiftyval = 0;
    }
//generateEmeraldval(parent);
//generateEmeraldval(h);
  if(pard == 0) {
    if(purehepta) h->distance = parent->distance + 1;
    else if(parent->s == hsOrigin) h->distance = 2;
    else if(h->spin(0) == 5) 
      h->distance = parent->distance + 1;
    else if(h->spin(0) == 4 && h->move[0]->s == hsB)
      h->distance = createStep(h->move[0], (h->spin(0)+2)%7)->distance + 3;
    else h->distance = parent->distance + 2;
    }
  else h->distance = parent->distance - (purehepta?1:2);
  return h;
  }

void addSpin(heptagon *h, int d, heptagon *from, int rot, int spin) {
  rot = fixrot(rot);
  createStep(from, rot);
  h->move[d] = from->move[rot];
  h->setspin(d, fixrot(from->spin(rot) + spin));
  h->move[d]->move[fixrot(from->spin(rot) + spin)] = h;
  h->move[d]->setspin(fixrot(from->spin(rot) + spin), d);
//generateEmeraldval(h->move[d]); generateEmeraldval(h);
  }

extern int hrand(int);

heptagon *createStep(heptagon *h, int d) {
  d = fixrot(d);
  if(!h->move[0] && h->s != hsOrigin) {
    buildHeptagon(h, 0, hsA, 3 + hrand(2));
    }
  if(h->move[d]) return h->move[d];
  if(h->s == hsOrigin) {
    buildHeptagon(h, d, hsA);
    }
  else if(d == 1) {
    addSpin(h, d, h->move[0], h->spin(0)-1, -1);
    }
  else if(d == 6) {
    addSpin(h, d, h->move[0], h->spin(0)+1, +1);
    }
  else if(d == 2) {
    createStep(h->move[0], h->spin(0)-1);
    addSpin(h, d, h->move[0]->modmove(h->spin(0)-1), 5 + h->move[0]->gspin(h->spin(0)-1), -1);
    }
  else if(d == 5 && h->s == hsB) {
    createStep(h->move[0], h->spin(0)+1);
    addSpin(h, d, h->move[0]->modmove(h->spin(0)+1), 2 + h->move[0]->gspin(h->spin(0)+1), +1);
    }
  else
    buildHeptagon(h, d, (d == 5 || (h->s == hsB && d == 4)) ? hsB : hsA);
  return h->move[d];
  }

// a structure used to walk on the heptagonal tesselation
// (remembers not only the heptagon, but also direction)
struct heptspin {
  heptagon *h;
  int spin;
  bool mirrored;
  heptspin() { mirrored = false; }
  };

heptspin hsstep(const heptspin &hs, int spin) {
  createStep(hs.h, hs.spin);
  heptspin res;
  res.h = hs.h->move[hs.spin];
  res.mirrored = hs.mirrored ^ hs.h->mirror(hs.spin);
  res.spin = fixrot(hs.h->spin(hs.spin) + (MIRR(res)?-spin:spin));
  return res;
  }

heptspin hsspin(const heptspin &hs, int val) {
  heptspin res;
  res.h = hs.h;
  res.spin = fixrot(hs.spin + (MIRR(hs)?-val:val));
  res.mirrored = hs.mirrored;
  return res;
  }

// display the coordinates of the heptagon
void backtrace(heptagon *pos) {
  if(pos->s == hsOrigin) return;
  backtrace(pos->move[0]);
  printf(" %d", pos->spin(0));
  }

void hsshow(const heptspin& t) {
  printf("ORIGIN"); backtrace(t.h); printf(" (spin %d)\n", t.spin);
  }

