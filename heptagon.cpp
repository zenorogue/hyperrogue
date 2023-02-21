// Hyperbolic Rogue -- Heptagon
// Copyright (C) 2011-2018 Zeno Rogue, see 'hyper.cpp' for details

/** \file heptagon.cpp
 *  \brief implementation of Heptagons
 *
 *  Start with locations.cpp
 */

#include "hyper.h"
namespace hr {

#define MIRR(x) x.mirrored

int heptacount = 0;

struct cell;
cell *newCell(int type, heptagon *master);

/** the automaton is used to generate each heptagon in an unique way.
 *  See http://roguetemple.com/z/dev.php for more help.
 *  From the origin we can go further in any direction, and from other heptagons
 *  we can go in directions 3 and 4 (0 is back to origin, so 3 and 4 go forward),
 *  and sometimes in direction 5
 */

EX hstate transition(hstate s, int dir) {
  if(embedded_plane) return IPF(transition(s, dir));
  if(sphere) {
    if(S7 == 4) {
      if(s == hsOrigin) return dir == 0 ? hsB0 : hsB1;
      }
    if(S7 == 3 && S3 == 3) {
      if(s == hsOrigin) return hsB1;
      }
    if(S7 == 3 && S3 == 4) {
      if(s == hsOrigin) return dir == 0 ? hsA0 : hsA1;
      if(s == hsA0 && dir == 1) return hsB0;
      if(s == hsA1 && dir == 1) return hsB1;
      if(s == hsB0 && dir == 2) return hsC;
      return hsError;
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
  else if(S3 >= OINF) {
    if(s == hsOrigin) return hsA;
    if(s == hsA && dir) return hsA;
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

#define COMPUTE -1000000

// create a new heptagon
EX heptagon *buildHeptagon1(heptagon *h, heptagon *parent, int d, hstate s, int pard IS(0)) {
  h->alt = NULL;
  h->s = s;
  h->c.connect(pard, parent, d, false);
  h->cdata = NULL;
  return h;
  }

EX heptagon *init_heptagon(int type) {
  heptagon *h = tailored_alloc<heptagon> (type);
  h->emeraldval = 0;
  h->zebraval = 0;
  h->fiftyval = 0;
  h->fieldval = 0;
  h->rval0 = h->rval1 = 0;
  h->cdata = NULL;
  h->alt = NULL;
  h->c7 = NULL;
  h->distance = 0;
  h->dm4 = 0;
  return h;
  }
  
heptagon *buildHeptagon(heptagon *parent, int d, hstate s, int pard = 0, int fixdistance = COMPUTE) {
  heptagon *h = buildHeptagon1(tailored_alloc<heptagon> (S7), parent, d, s, pard);
  if(bt::in() || arcm::in()) return h;
  if(parent->c7) {
    #if CAP_IRR
    if(IRREGULAR)
      irr::link_next(parent, d);
    else
    #endif
      h->c7 = newCell(S7, h);
    h->rval0 = h->rval1 = 0;
    h->emeraldval = emerald_heptagon(parent->emeraldval, d);
    h->zebraval = zebra_heptagon(parent->zebraval, d);
    #if CAP_FIELD
    if(&currfp != &fieldpattern::fp_invalid)
      h->fieldval = currfp.connections[fieldpattern::btspin(parent->fieldval, d)];
    #endif
    if(a38) 
      h->fiftyval = fifty_38(parent->fiftyval, d);  
    else if(parent->s == hsOrigin)
      h->fiftyval = firstfiftyval(d);
    else
      h->fiftyval = nextfiftyval(parent->fiftyval, parent->move(0)->fiftyval, d);
    }
  else {
    h->c7 = NULL;
    h->emeraldval = 0;
    h->fiftyval = 0;
    h->cdata = NULL;
    }
//generateEmeraldval(parent);
//generateEmeraldval(h);
  if(pard == 0) {
    h->dm4 = parent->dm4+1;
    if(fixdistance != COMPUTE) h->distance = fixdistance;
    else if(S3 == 4 && BITRUNCATED) {
      h->distance = parent->distance + 2;
      if(h->c.spin(0) == 2 || (h->c.spin(0) == 3 && S7 <= 5))
        h->distance = min<short>(h->distance, createStep(h->move(0), 0)->distance + 3);
      if(h->c.spin(0) == 2 && h->move(0)) {
        int d = h->c.spin(0);
        int d1 = (d+S7-1)%S7;
        bool missing0 = !h->move(0)->move(d1);
        if(missing0) {
          if(s == 1 && h->move(0)->s != hsOrigin)
            h->distance = h->move(0)->distance + 1;
          }
        else {
          heptagon* h1 = createStep(h->move(0), d1);
          if(h1->distance <= h->move(0)->distance)
            h->distance = h->move(0)->distance+1;
          }
        }
      if((h->s == hsB && h->move(0)->s == hsB) || h->move(0)->s == hsA) {
        int d = h->c.spin(0);
        heptagon* h1 = createStep(h->move(0), (d+1)%S7);
        if(h1->distance <= h->move(0)->distance)
          h->distance = h->move(0)->distance+1;
        }
      if(h->c.spin(0) == S7-1 && (h->move(0)->s != hsOrigin) && BITRUNCATED) {
        bool missing = !h->move(S7-1);
        if(missing) {
          h->distance = parent->distance;
          if(
            parent->distance - h->move(0)->move(0)->distance == 1 &&
            h->c.spin(0) == S7 - 1 &&
            h->move(0)->c.spin(0) == 2)
            h->distance++;
          }
        else {
          h->distance = min(
            h->move(0)->move(0)->distance + 2,
            createStep(h, S7-1)->distance + 1
            );
          }
        }
      }
    else if(parent->s == hsOrigin) h->distance = parent->distance + gp::dist_2();
    #if CAP_GP
    else if(S3 == 4 && GOLDBERG && h->c.spin(0) == S7-2 && h->move(0)->c.spin(0) >= S7-2 && h->move(0)->move(0)->s != hsOrigin) {
      heptspin hs(h, 0);
      hs += wstep;
      int d1 = hs.at->distance;
      hs += 1; hs += wstep;
      int dm = hs.at->distance;
      hs += -1; hs += wstep;
      int d0 = hs.at->distance;
      h->distance = gp::solve_triangle(dm, d0, d1, gp::param * gp::loc(-1,1));
      }
    else if(S3 == 4 && GOLDBERG && h->c.spin(0) == S7-1 && among(h->move(0)->c.spin(0), S7-2, S7-3) && h->move(0)->move(0)->s != hsOrigin) {
      heptspin hs(h, 0);
      hs += wstep;
      int d0 = hs.at->distance;
      hs += 1; hs += wstep;
      int dm = hs.at->distance;
      hs += 1; hs += wstep;
      int d1 = hs.at->distance;
      h->distance = gp::solve_triangle(dm, d0, d1, gp::param * gp::loc(1,1));
      }
    else if(S3 == 4 && GOLDBERG && h->c.spin(0) >= 2 && h->c.spin(0) <= S7-2) {
      h->distance = parent->distance + gp::dist_2();
      }
    #endif
    else if(h->c.spin(0) == S7-2) {
      #if CAP_GP
      if(GOLDBERG) {
        int d0 = parent->distance;
        int d1 = createStep(parent, S7-1)->distance;
        int dm = createStep(parent, 0)->distance;
        h->distance = gp::solve_triangle(dm, d0, d1, gp::param * gp::loc(1,1));
        } else
      #endif
      h->distance = parent->distance + gp::dist_1();
      }
    else if(h->c.spin(0) == S7-3 && h->move(0)->s == hsB) {
      #if CAP_GP
      if(GOLDBERG) {
        int d0 = parent->distance;
        int d1 = createStep(parent, S7-2)->distance;
        int dm = createStep(parent, S7-1)->distance;
        h->distance = gp::solve_triangle(dm, d0, d1, gp::param * gp::loc(1,1));
        } else
      #endif
      h->distance = createStep(h->move(0), (h->c.spin(0)+2)%S7)->distance + gp::dist_3();
      }
    else if(h->c.spin(0) == S7-1 && S3 == 4 && GOLDBERG) {
      h->distance = parent->distance + gp::dist_1();
      }
    else h->distance = parent->distance + gp::dist_2();
    }
  else {
    h->distance = parent->distance - gp::dist_2();
    if(S3 == 4 && S7 > 5 && BITRUNCATED) {
      h->distance = parent->distance - 2;
      }
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

int recsteps;

void addSpin(heptagon *h, int d, heptagon *from, int rot, int spin) {
  rot = h->c.fix(rot);
  auto h1 = createStep(from, rot);
  int fr = h1->c.fix(from->c.spin(rot) + spin);
  h->c.connect(d, from->move(rot), fr, false);
  }

extern int hrand(int);

EX hookset<void(heptagon*, int)> hooks_createStep;

// create h->move(d) if not created yet
heptagon *createStep(heptagon *h, int d) {
  d = h->c.fix(d);
  if(h->move(d)) return h->move(d);
  callhooks(hooks_createStep, h, d);
  return currentmap->create_step(h, d);
  }

heptagon *hrmap_standard::create_step(heptagon *h, int d) {
  if(!h->move(0) && h->s != hsOrigin && !bt::in() && !cryst) {
    // cheating: 
    int pard=0;
    if(S3 == 3) 
      pard = 3 + hrand(2);
    else if(S3 == 4 && S7 == 5)
      pard = 3; // to do: randomize
    else if(S3 == 4)
      pard = 3;
    buildHeptagon(h, 0, h->distance < -global_distance_limit - 200 ? hsOrigin : hsA, pard);
    }
  if(h->move(d)) return h->move(d);
  if(h->s == hsOrigin) {
    buildHeptagon(h, d, hsA);
    }
  else if(S3 == 4) {
    if(d == 1) {
      heptspin hs(h, 0, false);
      hs = hs + wstep - 1 + wstep - 1 + wstep - 1;
      h->c.connect(d, hs);
      }
    else if(h->s == hsB && d == S7-1) {
      heptspin hs(h, 0, false);
      hs = hs + wstep + 1 + wstep + 1 + wstep + 1;
      h->c.connect(d, hs);
      }
    else 
      buildHeptagon(h, d, transition(h->s, d));
    }
  else if(S3 > 4 && quotient) {
    /* this branch may be used for some >4-valent quotient spaces outside of standard HyperRogue */
    /* this is wrong, but we don't care in quotient */
    h->move(d) = h;
    // buildHeptagon(h, d, transition(h->s, d));
    }
  else if(d == 1) {
    addSpin(h, d, h->move(0), h->c.spin(0)-1, -1);
    }
  else if(d == S7-1) {
    addSpin(h, d, h->move(0), h->c.spin(0)+1, +1);
    }
  else if(d == 2) {
    createStep(h->move(0), h->c.spin(0)-1);
    addSpin(h, d, h->move(0)->modmove(h->c.spin(0)-1), S7-2 + h->move(0)->c.modspin(h->c.spin(0)-1), -1);
    }
  else if(d == S7-2 && h->s == hsB) {
    createStep(h->move(0), h->c.spin(0)+1);
    addSpin(h, d, h->move(0)->modmove(h->c.spin(0)+1), 2 + h->move(0)->c.modspin(h->c.spin(0)+1), +1);
    }
  else
    buildHeptagon(h, d, (d == S7-2 || (h->s == hsB && d == S7-3)) ? hsB : hsA);
  return h->move(d);
  }

// display the coordinates of the heptagon
void backtrace(heptagon *pos) {
  if(pos->s == hsOrigin) return;
  backtrace(pos->move(0));
  printf(" %d", pos->c.spin(0));
  }

void hsshow(const heptspin& t) {
  printf("ORIGIN"); backtrace(t.at); printf(" (spin %d)\n", t.spin);
  }

}
