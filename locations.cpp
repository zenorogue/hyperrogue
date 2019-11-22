// Hyperbolic Rogue -- Locations
// Copyright (C) 2011-2018 Zeno Rogue, see 'hyper.cpp' for details

/** \file locations.cpp
 *  \brief definition of connection tables, walkers, cell and heptagon structures
 *
 *  The standard geometry uses 'heptagons' for the underlying heptagonal tessellation, 
 *  and 'cells' for the tessellation that the game is actually played on. 
 *  Other geometries also use the class 'heptagon' even if they are not heptagon-based;
 *  there may be one 'heptagon' per each cell. Heptagons are not used in masterless
 *  geometries, though. This file implements the basic types and functions for navigating both graphs.
 */

#include "hyper.h"
namespace hr {

#if HDR

extern int cellcount, heptacount;

#define NODIR 126
#define NOBARRIERS 127

/** Cell information for the game. struct cell builds on this */
struct gcell {

#if CAP_BITFIELD
  /** which land does this cell belong to */
  eLand land : 8;
  /** wall type (waNone for no walls) */
  eWall wall : 8;
  /** monster on this cell -- note that player characters are handled separately */
  eMonster monst : 8;
  /** item on this cell */
  eItem item : 8;

  /** if this is a barrier, what lands on are on the sides? */
  eLand barleft : 8, barright : 8; 

  /** is it currently sparkling with lightning? */
  unsigned ligon : 1;

  signed 
    mpdist : 7,         ///< minimum player distance, the smaller value, the more generated it is */
    pathdist : 8,       ///< distance from the target -- actual meaning may change
    cpdist : 8;         ///< current distance to the player

  unsigned 
    mondir : 8,         ///< which direction the monster is facing (if relevant), also used for boats
    bardir : 8,         ///< may equal NODIR (no barrier here), NOBARRIERS (barriers not allowed here), or the barrier direction
    stuntime : 8,       ///< for stunned monsters, stun time left; also used for Mutant Ivy timing
    hitpoints : 7,      ///< hitpoints left, for Palace monsters, Dragons, Krakens etc. Also reused as cpid for mirrors
    monmirror : 1;      ///< monster mirroring state for nonorientable geometries
  
  unsigned landflags : 8; ///< some lands need additional flags
#else
  eLand land;
  eWall wall;
  eMonster monst;
  eItem item;
  eLand barleft, barright;
  bool ligon, monmirror;
  signed char pathdist, cpdist, mpdist;
  
  unsigned char mondir, bardir, stuntime, hitpoints;
  unsigned char landflags;
#endif
  
  /** 'landparam' is used for: 
   *  heat in Icy/Cocytus; 
   *  heat in Dry (0..10); 
   *  CR2 structure; 
   *  hive Weird Rock color / pheromones;
   *  Ocean/coast depth;
   *  Bomberbird Egg hatch time / mine marking;
   *  number of Ancient Jewelry;
   *  improved tracking in Trollheim
   */
  union { 
    int32_t landpar; 
    unsigned int landpar_color;
    float heat; 
    char bytes[4]; 
    struct fieldinfo { 
      uint16_t fieldval;
      unsigned rval : 4;
      unsigned flowerdist : 4;
      unsigned walldist : 4;
      unsigned walldist2 : 4;
      } fi;
  
  } LHU;

  /** wall parameter, used e.g. for remaining power of Bonfires and Thumpers */
  char wparam;
  
  #ifdef CELLID
  int cellid;
  #endif
  
  gcell() { cellcount++; 
    #ifdef CELLID
    cellid = cellcount;  
    #endif
    }
  ~gcell() { cellcount--; }
  };

#define landparam LHU.landpar
#define landparam_color LHU.landpar_color
#define fval LHU.fi.fieldval

#define MAX_EDGE 18

template<class T> struct walker;

/** Connection tables are used by heptagon and cell structures. They basically
 *  describe the structure of the graph on the given manifold. We assume that 
 *  the class T has a field c of type connection_table<T>,
 *  as its last field. Edges are listed in the clockwise order (for 2D tilings, 
 *  for 3D tilings the order is more arbitrary). For each edge we remember which other T
 *  we are connected to, as well as the index of this edge in the other T, and whether it is 
 *  mirrored (for graphs on non-orientable manifolds).
 *  To conserve memory, these classes need to be allocated with tailored_alloc
 *  and freed with tailored_free.
 */

template<class T> struct connection_table {

  /** Table of moves. This is the maximum size, but tailored_alloc allocates less. */
  T* move_table[MAX_EDGE + (MAX_EDGE + sizeof(char*) - 1) / sizeof(char*)];
  
  unsigned char *spintable() { return (unsigned char*) (&move_table[full()->degree()]); }

  /** get the full T from the pointer to this connection table */
  T* full() { T* x = (T*) this; return (T*)((char*)this - ((char*)(&(x->c)) - (char*)x)); }
  /** for the edge d, set the `spin` and `mirror` attributes */
  void setspin(int d, int spin, bool mirror) { 
    unsigned char& c = spintable() [d];
    c = spin;
    if(mirror) c |= 128;
    }
  /** we are spin(i)-th neighbor of move[i] */
  int spin(int d) { return spintable() [d] & 127; }
  /** on non-orientable surfaces, the d-th edge may be mirrored */
  bool mirror(int d) { return spintable() [d] & 128; }  
  /** 'fix' the edge number d to get the actual index in [0, degree()) */
  int fix(int d) { return (d + MODFIXER) % full()->degree(); }
  /** T in the direction i */
  T*& move(int i) { return move_table[i]; }
  /** T in the direction i, modulo degree() */
  T*& modmove(int i) { return move(fix(i)); }
  unsigned char modspin(int i) { return spin(fix(i)); }
  /** initialize the table */
  void fullclear() { 
    for(int i=0; i<full()->degree(); i++) move_table[i] = NULL;
    }
  /** connect this in direction d0 to c1 in direction d1, possibly mirrored */
  void connect(int d0, T* c1, int d1, bool m) {
    move(d0) = c1;
    c1->move(d1) = full();
    setspin(d0, d1, m);
    c1->c.setspin(d1, d0, m);    
    }
  /* like the other connect, but take the parameters of the other cell from a walker */
  void connect(int d0, walker<T> hs) {
    connect(d0, hs.at, hs.spin, hs.mirrored);
    }
  };

/** Allocate a class T with a connection_table, but 
 *  with only `degree` connections. Also set yet
 *  unknown connections to NULL.

 * Generating the hyperbolic world consumes lots of
 * RAM, so we really need to be careful on low memory devices. 
 */

template<class T> T* tailored_alloc(int degree) {
  const T* sample = (T*) &degree;
  T* result;
#ifndef NO_TAILORED_ALLOC
  int b = (char*)&sample->c.move_table[degree] + degree - (char*) sample;
  result = (T*) new char[b];
  new (result) T();
#else
  result = new T;
#endif
  result->type = degree;
  for(int i=0; i<degree; i++) result->c.move_table[i] = NULL;
  return result;
  }

/** Counterpart to tailored_alloc(). */
template<class T> void tailored_delete(T* x) {
  x->~T();  
  delete[] ((char*) (x));
  }

static const struct wstep_t { wstep_t() {} } wstep;
static const struct wmirror_t { wmirror_t() {}} wmirror;
static const struct rev_t { rev_t() {} } rev;
static const struct revstep_t { revstep_t() {}} revstep;

extern int hrand(int);

/** the walker structure is used for walking on surfaces defined via \ref connection_table. */
template<class T> struct walker {
  /** where we are at */
  T *at;
  /** in which direction (edge) we are facing */
  int spin;
  /** are we mirrored */
  bool mirrored;
  walker<T> (T *at = NULL, int s = 0, bool m = false) : at(at), spin(s), mirrored(m) { if(at) s = at->c.fix(s); }
  /** spin by i to the left (or right, when mirrored */
  walker<T>& operator += (int i) {
    spin = at->c.fix(spin+(mirrored?-i:i));
    return (*this);
    }
  /** spin by i to the right (or left, when mirrored */
  walker<T>& operator -= (int i) {
    spin = at->c.fix(spin-(mirrored?-i:i));
    return (*this);
    }
  /** add wmirror to mirror this walker */
  walker<T>& operator += (wmirror_t) {
    mirrored = !mirrored;
    return (*this);
    }
  /** add wstep to make a single step, after which we are facing the T we were originally on */
  walker<T>& operator += (wstep_t) {
    at->cmove(spin);
    int nspin = at->c.spin(spin);
    if(at->c.mirror(spin)) mirrored = !mirrored;
    at = at->move(spin);
    spin = nspin;
    return (*this);
    }
  /** add wrev to face the other direction, may be non-deterministic and use hrand */
  walker<T>& operator += (rev_t) {
    auto rd = reverse_directions(at, spin);
    if(rd.size() == 1) spin = rd[0];
    else spin = rd[hrand(rd.size())];
    return (*this);
    }
  /** adding revstep is equivalent to adding rev and step */
  walker<T>& operator += (revstep_t) {
    (*this) += rev; return (*this) += wstep; 
    }
  bool operator != (const walker<T>& x) const {
    return at != x.at || spin != x.spin || mirrored != x.mirrored;
    }
  bool operator == (const walker<T>& x) const {
    return at == x.at && spin == x.spin && mirrored == x.mirrored;
    }

  bool operator < (const walker<T>& cw2) const {
    return tie(at, spin, mirrored) < tie(cw2.at, cw2.spin, cw2.mirrored);
    }

  walker<T>& operator ++ (int) { return (*this) += 1; }
  walker<T>& operator -- (int) { return (*this) -= 1; }
  template<class U> walker operator + (U t) const { walker<T> w = *this; w += t; return w; }
  template<class U> walker operator - (U t) const { walker<T> w = *this; w += (-t); return w; }
  /** what T are we facing, without creating it */
  T*& peek() { return at->move(spin); }
  /** what T are we facing, with creating it */
  T* cpeek() { return at->cmove(spin); }
  /** would we create a new T if we stepped forwards? */
  bool creates() { return !peek(); }
  /** mirror this walker with respect to the d-th edge */
  walker<T> mirrorat(int d) { return walker<T> (at, at->c.fix(d+d - spin), !mirrored); }
  };

struct cell;

// automaton state
enum hstate { hsOrigin, hsA, hsB, hsError, hsA0, hsA1, hsB0, hsB1, hsC };

struct cell *createMov(struct cell *c, int d);
struct heptagon *createStep(struct heptagon *c, int d);

struct cdata {
  int val[4];
  int bits;
  };

/** in bitruncated/irregular/Goldberg geometries, heptagons form the 
 *  underlying regular tiling (not necessarily heptagonal); in pure
 *  geometries, they correspond 1-1 to tiles; in 'masterless' geometries
 *  heptagons are unused
 */

struct heptagon {
  /** Automata are used to generate the standard maps. s is the state of this automaton */
  hstate s : 6;
  /** distance modulo 4, in heptagons */
  unsigned int dm4: 2;
  /** distance from the origin; based on the final geometry of cells, not heptagons themselves */
  short distance;
  /** Wmerald/wineyard generator. May have different meaning in other geometries. */
  short emeraldval;
  /** Palace pattern generator. May have different meaning in other geometries. */
  short fiftyval;
  /** Zebra pattern generator. May have different meaning in other geometries. */
  short zebraval;
  /** Field quotient pattern ID. May have different meaning in other geometries. */
  int fieldval : 24;
  /** the number of adjacent heptagons */
  unsigned char type : 8;
  /** data for fractal landscapes */
  short rval0, rval1;
  /** for the main map, it contains the fractal landscape data
   *
   *  For alternate structures, cdata contains the pointer to the original.
   */
  struct cdata *cdata;
  /** which central cell does this heptagon correspond too
   *  
   *  For alternate geometries, c7 is NULL
   */
  cell *c7;
  /** associated generator of alternate structure, for Camelot and horocycles */
  heptagon *alt;
  /** connection table */
  connection_table<heptagon> c;
  // DO NOT add any fields after connection_table! (see tailored_alloc)
  heptagon*& move(int d) { return c.move(d); }
  heptagon*& modmove(int d) { return c.modmove(d); }
  // functions
  heptagon () { heptacount++; }
  ~heptagon () { heptacount--; }
  heptagon *cmove(int d) { return createStep(this, d); }
  heptagon *cmodmove(int d) { return createStep(this, c.fix(d)); }
  inline int degree() { return type; }

  // prevent accidental copying
  heptagon(const heptagon&) = delete;
  heptagon& operator=(const heptagon&) = delete;
  };

struct cell : gcell {
  char type;        ///< our degree
  int degree() { return type; }

  int listindex;    ///< used by celllister  
  heptagon *master; ///< heptagon who owns us; for 'masterless' tilings it contains coordinates instead

  connection_table<cell> c;
  // DO NOT add any fields after connection_table! (see tailored_alloc)

  cell*& move(int d) { return c.move(d); }
  cell*& modmove(int d) { return c.modmove(d); }
  cell* cmove(int d) { return createMov(this, d); }
  cell* cmodmove(int d) { return createMov(this, c.fix(d)); }
  cell() {}

  // prevent accidental copying
  cell(const cell&) = delete;
  heptagon& operator=(const cell&) = delete;
  };

/** abbreviations */
typedef walker<heptagon> heptspin;
typedef walker<cell> cellwalker;

/** A structure useful when walking on the cell graph in arbitrary way, 
  * or listing cells in general.
  * Only one celllister may be active at a time, using the stack semantics.
  * Only the most recently created one works; the previous one will resume 
  * working when this one is destroyed.
  */
struct manual_celllister {
  /** list of cells in this list */
  vector<cell*> lst;
  vector<int> tmps;

  /** is the given cell on the list? */
  bool listed(cell *c) {
    return c->listindex >= 0 && c->listindex < isize(lst) && lst[c->listindex] == c;
    }
  
  /** add a cell to the list */
  bool add(cell *c) {
    if(listed(c)) return false;
    tmps.push_back(c->listindex);
    c->listindex = isize(lst);
    lst.push_back(c);
    return true;
    }

  ~manual_celllister() {     
    for(int i=0; i<isize(lst); i++) lst[i]->listindex = tmps[i];
    }  
  };
  
/** automatically generate a list of nearby cells */
struct celllister : manual_celllister {
  vector<int> dists;
  
  void add_at(cell *c, int d) {
    if(add(c)) dists.push_back(d);
    }
  
  /** automatically generate a list of nearby cells
  @param orig where to start
  @param maxdist maximum distance to cover
  @param maxcount maximum number of cells to cover
  @param breakon we are actually looking for this cell, so stop when reaching it
  */
  celllister(cell *orig, int maxdist, int maxcount, cell *breakon) {
    add_at(orig, 0);
    cell *last = orig;
    for(int i=0; i<isize(lst); i++) {
      cell *c = lst[i];
      if(maxdist) forCellCM(c2, c) {
        add_at(c2, dists[i]+1);
        if(c2 == breakon) return;
        }
      if(c == last) {
        if(isize(lst) >= maxcount || dists[i]+1 == maxdist) break;
        last = lst[isize(lst)-1];
        }
      }
    }
  
  /** for a given cell c on the list, return its distance from orig */
  int getdist(cell *c) { return dists[c->listindex]; }
  };

/** translate heptspins to cellwalkers and vice versa */
static const struct cth_t { cth_t() {}} cth;
inline heptspin operator+ (cellwalker cw, cth_t) { return heptspin(cw.at->master, cw.spin * DUALMUL, cw.mirrored); }
inline cellwalker operator+ (heptspin hs, cth_t) { return cellwalker(hs.at->c7, hs.spin / DUALMUL, hs.mirrored); }

#endif

#if HDR
/** a structure for representing movements 
 *  mostly for 'proper' moves where s->move(d) == t,
 *  but also sometimes for other moves
 */

constexpr int STRONGWIND = 99;
constexpr int FALL = 98;
constexpr int NO_SPACE = 97;

namespace whirlwind { cell *jumpDestination(cell*); }

struct movei {
  cell *s;
  cell *t;
  int d;
  bool op() { return s != t; }
  bool proper() const { return d >= 0 && d < s->type && s->move(d) == t; }
  movei(cell *_s, int _d) : s(_s), d(_d) {
    if(d == STRONGWIND) t = whirlwind::jumpDestination(s);
    else if(d == FALL || d == NO_SPACE) t = s;
    else t = s->move(d);
    }
  movei(cell *_s, cell *_t, int _d) : s(_s), t(_t), d(_d) {}
  movei rev() const { return movei(t, s, rev_dir_or(d)); }
  int dir_or(int x) const { return proper() ? d : x; }
  int rev_dir_or(int x) const { return proper() ? s->c.spin(d) : x; }
  bool mirror() { return s->c.mirror(d); }
  };
#endif

EX movei match(cell *f, cell *t) {
  for(int i=0; i<f->type; i++) if(f->move(i) == t) return movei(f, t, i);
  return movei(f, t, -1);
  }

}
