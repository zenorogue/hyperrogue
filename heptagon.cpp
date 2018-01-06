// Hyperbolic Rogue
// Copyright (C) 2011-2012 Zeno Rogue, see 'hyper.cpp' for details

// heptagon here refers to underlying heptagonal tesselation
// (which you can see by changing the conditions in graph.cpp)

#define MIRR(x) x.mirrored

int heptacount = 0;

struct cell;
cell *newCell(int type, heptagon *master);

// spintable functions

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
  else if(S6 == 8) {
    if(s == hsOrigin) return hsA;
    if(s == hsA && (dir >= 2 && dir < S7-1)) return hsA;
    if(s == hsA && (dir == S7-1)) return hsB;
    if(s == hsB && (dir >= 2 && dir < S7-2)) return hsA;
    if(s == hsB && (dir == S7-2)) return hsB;
    }
  else {
    if(s == hsOrigin) return hsA;
    if(s == hsA && dir >= 3 && dir <= S7-3) return hsA;
    if(s == hsA && dir == S7-2) return hsB;
    if(s == hsB && dir >= 3 && dir <= S7-4) return hsA;
    if(s == hsB && dir == S7-3) return hsB;
    }
  return hsError;
  }

/*
int indent = 0;

struct indenter {
  indenter() { indent += 2; }
  ~indenter() { indent -= 2; }
  };

template<class... T> auto iprintf(T... t) { for(int i=0; i<indent; i++) putchar(' '); return printf(t...); }
*/

#define COMPUTE -1000000

// create a new heptagon
heptagon *buildHeptagon(heptagon *parent, int d, hstate s, int pard = 0, int fixdistance = COMPUTE) {
  heptagon *h = new heptagon;
  h->alt = NULL;
  h->s = s;
  for(int i=0; i<MAX_EDGE; i++) h->move[i] = NULL;
  h->spintable = 0;
  h->move[pard] = parent; tsetspin(h->spintable, pard, d);
  parent->move[d] = h; tsetspin(parent->spintable, d, pard);
  if(parent->c7) {
    h->c7 = newCell(S7, h);
    h->rval0 = h->rval1 = 0; h->cdata = NULL;
    h->emeraldval = emerald_heptagon(parent->emeraldval, d);
    h->zebraval = zebra_heptagon(parent->zebraval, d);
    h->fieldval = currfp.connections[fieldpattern::btspin(parent->fieldval, d)];
    if(a38) 
      h->fiftyval = fifty_38(parent->fiftyval, d);  
    else if(parent->s == hsOrigin)
      h->fiftyval = firstfiftyval(d);
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
    h->dm4 = parent->dm4+1;
    if(fixdistance != COMPUTE) h->distance = fixdistance;
    else if(nonbitrunc) h->distance = parent->distance + 1;
    else if(parent->s == hsOrigin) h->distance = parent->distance + 2;
    else if(S3 == 4) {
      h->distance = parent->distance + 2;
      if(h->spin(0) == 2 || (h->spin(0) == 3 && S7 <= 5))
        h->distance = min<short>(h->distance, createStep(h->move[0], 0)->distance + 3);
      if(h->spin(0) == 2 && h->move[0]) {
        int d = h->spin(0);
        int d1 = (d+S7-1)%S7;
        heptagon* h1 = createStep(h->move[0], d1);
        if(h1->distance <= h->move[0]->distance)
          h->distance = h->move[0]->distance+1;
        }
      if((h->s == hsB && h->move[0]->s == hsB) || h->move[0]->s == hsA) {
        int d = h->spin(0);
        heptagon* h1 = createStep(h->move[0], (d+1)%S7);
        if(h1->distance <= h->move[0]->distance)
          h->distance = h->move[0]->distance+1;
        }
      if(h->spin(0) == S7-1)
        h->distance = min(
          h->move[0]->move[0]->distance + 2,
          createStep(h, S7-1)->distance + 1
          );
      }
    else if(h->spin(0) == S7-2) 
      h->distance = parent->distance + 1;
    else if(h->spin(0) == S7-3 && h->move[0]->s == hsB)
      h->distance = createStep(h->move[0], (h->spin(0)+2)%S7)->distance + 3;
    else h->distance = parent->distance + 2;
    }
  else {
    h->distance = parent->distance - (nonbitrunc?1:2);
    if(S3 == 4 && S7 == 5) {
      if(h->s == hsOrigin) {
        printf("had to cheat!\n");
        h->distance = parent->distance - 2;
        }
      else {
        h->distance = parent->distance - 1;
        buildHeptagon(h, 2, hsA, 0, h->distance + 2);
        buildHeptagon(h, 4, hsB, 0, h->distance);
        }
      }
    h->dm4 = parent->dm4-1;
    }
  return h;
  }

void connectHeptagons(heptagon *h1, int d1, heptagon *h2, int d2) {
  h1->move[d1] = h2;
  h1->setspin(d1, d2);
  h2->move[d2] = h1;
  h2->setspin(d2, d1);
  }

int recsteps;

void addSpin(heptagon *h, int d, heptagon *from, int rot, int spin) {
  rot = fixrot(rot);
  createStep(from, rot);
  int fr = fixrot(from->spin(rot) + spin);
  connectHeptagons(h, d, from->move[rot], fr);
/*  h->move[d] = from->move[rot];
  h->setspin(d, fr);
  h->move[d]->move[fr] = h;
  h->move[d]->setspin(fr, d); */
//generateEmeraldval(h->move[d]); generateEmeraldval(h);
  }

extern int hrand(int);

// a structure used to walk on the heptagonal tesselation
// (remembers not only the heptagon, but also direction)

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

heptagon *createStep(heptagon *h, int d) {
  d = fixrot(d);
  if(!h->move[0] && h->s != hsOrigin) {
    // cheating: 
    int pard=0;
    if(S3 == 3) 
      pard = 3 + hrand(2);
    else if(S3 == 4 && S7 == 5)
      pard = 3; // to do: randomize
    else if(S3 == 4)
      pard = 3;
    buildHeptagon(h, 0, h->distance < -10000 ? hsOrigin : hsA, pard);
    }
  if(h->move[d]) return h->move[d];
  if(h->s == hsOrigin) {
    buildHeptagon(h, d, hsA);
    }
  else if(S3 == 4) {
    if(d == 1) {
      heptspin hs;
     hs.h = h;
      hs.spin = 0;
      hs.mirrored = false;
      hs = hsstep(hs, -1);
      hs = hsstep(hs, -1);
      hs = hsstep(hs, -1);
      connectHeptagons(h, d, hs.h, hs.spin);
      }
    else if(h->s == hsB && d == S7-1) {
      heptspin hs;
      hs.h = h;
      hs.spin = 0;
      hs.mirrored = false;
      hs = hsstep(hs, 1);
      hs = hsstep(hs, 1);
      hs = hsstep(hs, 1);
      connectHeptagons(h, d, hs.h, hs.spin);    
      }
    else 
      buildHeptagon(h, d, transition(h->s, d));
    }
  else if(d == 1) {
    addSpin(h, d, h->move[0], h->spin(0)-1, -1);
    }
  else if(d == S7-1) {
    addSpin(h, d, h->move[0], h->spin(0)+1, +1);
    }
  else if(d == 2) {
    createStep(h->move[0], h->spin(0)-1);
    addSpin(h, d, h->move[0]->modmove(h->spin(0)-1), S7-2 + h->move[0]->gspin(h->spin(0)-1), -1);
    }
  else if(d == S7-2 && h->s == hsB) {
    createStep(h->move[0], h->spin(0)+1);
    addSpin(h, d, h->move[0]->modmove(h->spin(0)+1), 2 + h->move[0]->gspin(h->spin(0)+1), +1);
    }
  else
    buildHeptagon(h, d, (d == S7-2 || (h->s == hsB && d == S7-3)) ? hsB : hsA);
  return h->move[d];
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

// create h->move[d] if not created yet
heptagon *createStep(heptagon *h, int d);

