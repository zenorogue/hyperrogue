// Hyperbolic Rogue -- Circuit Land
// Copyright (C) 2011-2026 Zeno Rogue, see 'hyper.cpp' for details

/** \file circuit.cpp
 *  \brief Circuit Land: AND/NAND/OR logic puzzles guarded by hyperbugs.
 *
 *  A depth-n circuit is a binary tree of AND/NAND/OR gates. Every root-to-leaf
 *  path passes through exactly n gates, so the tree has 2^n inputs total. The
 *  seed sits above the root gate; its value is whatever the root gate outputs.
 *
 *  Circuits grow only on hexagonal cells (as in the Clearing's mutant ivy).
 *  In HR's tiling each hex has just 3 hex-neighbors, so a pure "complete
 *  binary tree of gates" doesn't fit past depth 3. To reach deeper puzzles
 *  the ivy also lays down wire cells: wherever a cell only has room for one
 *  child, we place a wire that carries the signal outward until the geometry
 *  opens up and the next gate can branch. Wires don't consume a gate level,
 *  so each root-to-leaf path still hits exactly n gates -- but the physical
 *  paths can be of different lengths.
 *
 *  Wall types:
 *    waCircuitSeed  -- the root; treasure appears here when the circuit fires
 *    waCircuitAND   -- gate: TRUE iff both children are TRUE
 *    waCircuitNAND  -- gate: TRUE iff NOT (both children TRUE)
 *    waCircuitOR    -- gate: TRUE iff at least one child is TRUE
 *    waCircuitInput -- leaf input; passable; TRUE iff a Dead Orb sits here
 *
 *  Storage:
 *    c->wall      -- wall type above (or waNone outside the circuit)
 *    c->landflags -- direction on c pointing toward its parent (255 at seed).
 *                    We can't use c->mondir here because monster movement
 *                    overwrites mondir on passable cells (waCircuitInput can
 *                    host a hyperbug walking through), which would sever the
 *                    tree at that input.
 *    c->wparam    -- level number (0 = seed, 1 = root gate, ..., n+1 = inputs)
 *
 *  Trivially-satisfied circuits (e.g. a lone NAND with two blank inputs, which
 *  evaluates to TRUE with zero orbs placed) dissolve immediately at plant
 *  time -- the player just finds a loose Circuit Seed.
 */

#include "hyper.h"
#include <unordered_set>
namespace hr {

EX namespace circuit {

  /** Cells whose lightning flash we want to keep re-asserting until the
   *  player takes their next turn. `flash_turn` records the turncount at
   *  which the flash was armed; when turncount advances past that, the
   *  flash expires and the cells go back to normal. */
  EX vector<cell*> flash_cells;
  EX int flash_turn = -1;

  /** Called each frame: if we're still on the same turn the flash was armed
   *  on, force ligon back on for every tree cell and refresh the timer so
   *  the arcs render at full brightness. Otherwise the flash expires. */
  EX void maintain_flash() {
    if(flash_cells.empty()) return;
    // The drop action itself bumps turncount by one, so flash_turn (captured
    // in check()) is already one behind the "resting" turncount even before
    // the player moves again. Only expire once turncount has advanced past
    // that resting value -- i.e. on the player's NEXT action.
    if(turncount > flash_turn + 1) { flash_cells.clear(); return; }
    for(cell *c : flash_cells) if(c) c->ligon = 1;
    lightat = ticks;
    }

  auto flash_hook = addHook(hooks_frame, 100, maintain_flash);

  /** Tree depth (levels of gates): items[itCircuitSeed]/3 + 2, capped at 6.
   *  At depth 6 the tree has 63 gates and 64 inputs -- already a huge puzzle;
   *  going deeper both slows backtracking down noticeably and doesn't reliably
   *  fit in the geometry anyway. */
  EX int target_depth() {
    int d = items[itCircuitSeed] / 3 + 2;
    if(d > 6) d = 6;
    return d;
    }

  /** Is this cell part of some circuit? */
  EX bool in_circuit(cell *c) {
    if(!c) return false;
    if(c->land != laCircuit) return false;
    eWall w = c->wall;
    return w == waCircuitSeed || w == waCircuitAND || w == waCircuitNAND
        || w == waCircuitOR || w == waCircuitWire || w == waCircuitInput;
    }

  static eWall random_gate() {
    int r = hrand(3);
    if(r == 0) return waCircuitAND;
    if(r == 1) return waCircuitNAND;
    return waCircuitOR;
    }

  // Small helper: track the set of cells claimed by the current build alongside
  // the ordered vector we need for rollback. The set gives O(1) membership,
  // which matters for deep trees where linear scans over `claimed` dominate.
  struct claim_bag {
    vector<cell*> order;
    std::unordered_set<cell*> set;
    void push(cell *c) { order.push_back(c); set.insert(c); }
    void pop() { set.erase(order.back()); order.pop_back(); }
    bool has(cell *c) const { return set.count(c) != 0; }
    size_t size() const { return order.size(); }
    cell *back() const { return order.back(); }
    };

  /** Direction on `c` pointing back to its parent, or -1 if none / invalid. */
  static int parent_dir(cell *c) {
    int d = c->landflags;
    if(d < 0 || d >= c->type) return -1;
    return d;
    }

  /** Walk up via landflags until we hit the seed. */
  EX cell *find_seed(cell *c) {
    if(!in_circuit(c)) return nullptr;
    for(int steps=0; steps<128; steps++) {
      if(c->wall == waCircuitSeed) return c;
      int d = parent_dir(c);
      if(d < 0) return nullptr;
      cell *p = c->move(d);
      if(!p) return nullptr;
      c = p;
      }
    return nullptr;
    }

  /** Recursively evaluate cell c. Wires (legacy) pass their single child
   *  through; gates AND/NAND/OR combine two children; the seed passes its
   *  single child through; input wires read c->item. */
  static int eval(cell *c, int fuel) {
    if(!c || fuel <= 0) return 0;
    if(c->wall == waCircuitInput)
      return (c->item == itGreenStone) ? 1 : 0;

    int child_vals[16], nchildren = 0;
    forCellIdEx(n, i, c) {
      if(!in_circuit(n)) continue;
      if(n->wall == waCircuitSeed) continue;
      int d = parent_dir(n);
      if(d < 0) continue;
      if(n->move(d) != c) continue;
      if(nchildren < 16) child_vals[nchildren++] = eval(n, fuel - 1);
      }

    if(c->wall == waCircuitSeed || c->wall == waCircuitWire)
      return nchildren >= 1 ? child_vals[0] : 0;

    if(c->wall == waCircuitAND || c->wall == waCircuitNAND || c->wall == waCircuitOR) {
      int a = nchildren >= 1 ? child_vals[0] : 0;
      int b = nchildren >= 2 ? child_vals[1] : 0;
      if(c->wall == waCircuitAND)  return a && b;
      if(c->wall == waCircuitNAND) return !(a && b);
      return a || b;
      }

    return 0;
    }

  /** BFS-collect every cell in the circuit rooted at seed. */
  static void collect_tree(cell *seed, vector<cell*>& out) {
    out.clear();
    if(!seed || seed->wall != waCircuitSeed) return;
    out.push_back(seed);
    for(size_t idx=0; idx<out.size(); idx++) {
      cell *c = out[idx];
      forCellEx(n, c) {
        if(!in_circuit(n)) continue;
        if(n->wall == waCircuitSeed) continue;
        int d = parent_dir(n);
        if(d < 0) continue;
        if(n->move(d) != c) continue;
        out.push_back(n);
        }
      if(out.size() > 1024) break;
      }
    }

  /** Roll back cells appended to `claimed` after position `keep`. */
  static void unclaim_since(claim_bag& claimed, size_t keep) {
    while(claimed.size() > keep) {
      cell *cc = claimed.back();
      cc->wall = waNone;
      cc->landflags = 0;
      cc->wparam = 0;
      claimed.pop();
      }
    }

  /** Enumerate the hex-neighbors of `parent` that could be claimed as part of
   *  the current tree. `back_dir` is the direction from `parent` toward its
   *  own parent (-1 if none). */
  static int find_candidates(cell *parent, int back_dir, const claim_bag& claimed,
                             int cand[], int cap) {
    int ncand = 0;
    for(int i=0; i<parent->type; i++) {
      if(i == back_dir) continue;
      cell *n = createMov(parent, i);
      if(!n) continue;
      if(pseudohept(n)) continue;
      if(n->land != laCircuit && n->land != laNone) continue;
      if(n->wall != waNone) continue;
      if(n->item != itNone) continue;
      if(n->monst != moNone) continue;
      if(claimed.has(n)) continue;
      bool bad = false;
      forCellEx(nn, n) {
        if(!nn) continue;
        if(claimed.has(nn)) continue;
        if(in_circuit(nn)) { bad = true; break; }
        }
      if(bad) continue;
      cand[ncand++] = i;
      if(ncand >= cap) break;
      }
    return ncand;
    }

  /** Total cell budget for a single build to keep runaway wire chains bounded. */
  static const size_t TREE_CELL_BUDGET = 1024;

  /** Grow a subtree starting at direction `dir` from `parent`. The new cell
   *  becomes a gate (2 children) if it has two candidate hex-neighbors, or a
   *  wire (1 child) if it has just one -- the wire propagates outward until
   *  the geometry opens up and another gate can be placed. `remaining_gates`
   *  counts how many more GATES must still appear on each root-to-leaf path
   *  from this new cell downwards; a wire does not decrement it. When it
   *  reaches 0 the cell is placed as an input leaf. */
  static bool place_and_grow(cell *parent, int dir, int remaining_gates, claim_bag& claimed) {
    if(claimed.size() >= TREE_CELL_BUDGET) return false;

    cell *c = parent->move(dir);
    size_t start = claimed.size();

    c->land = laCircuit;
    c->landflags = parent->c.spin(dir);
    c->wparam = parent->wparam + 1;
    claimed.push(c);

    // Leaf: no more gates required, this cell is an input.
    if(remaining_gates == 0) {
      c->wall = waCircuitInput;
      return true;
      }

    // Look at what this cell could branch/pass-through into.
    int cand[16];
    int ncand = find_candidates(c, c->landflags, claimed, cand, 16);
    if(ncand == 0) {
      // Dead end -- can't reach any more cells from here, so we can't finish
      // the required chain of gates.
      unclaim_since(claimed, start);
      return false;
      }

    const int MAX_RETRIES = 4;

    // If we have room to branch, place a gate.
    if(ncand >= 2) {
      c->wall = random_gate();
      for(int retry = 0; retry < MAX_RETRIES; retry++) {
        for(int i=ncand-1; i>0; i--) {
          int j = hrand(i+1);
          int t = cand[i]; cand[i] = cand[j]; cand[j] = t;
          }
        size_t gate_mark = claimed.size();
        if(place_and_grow(c, cand[0], remaining_gates - 1, claimed)
        && place_and_grow(c, cand[1], remaining_gates - 1, claimed))
          return true;
        unclaim_since(claimed, gate_mark);
        }
      // Fall through and try again as a wire -- it may be that a straight
      // extension gets us into a spot with more room downstream.
      }

    // Wire: single-child pass-through. Does not consume a gate level.
    c->wall = waCircuitWire;
    for(int retry = 0; retry < MAX_RETRIES; retry++) {
      for(int i=ncand-1; i>0; i--) {
        int j = hrand(i+1);
        int t = cand[i]; cand[i] = cand[j]; cand[j] = t;
        }
      size_t wire_mark = claimed.size();
      if(place_and_grow(c, cand[0], remaining_gates, claimed)) return true;
      unclaim_since(claimed, wire_mark);
      }

    // Couldn't route through here at all -- give up on this cell.
    unclaim_since(claimed, start);
    return false;
    }

  /** Attempt to grow a tree rooted at `seed` where each root-to-leaf path
   *  passes through exactly `n_gate_levels` gates. Wires may appear anywhere
   *  along a path to bridge tight geometry. */
  static bool build_tree(cell *seed, int n_gate_levels, claim_bag& claimed) {
    seed->wall = waCircuitSeed;
    seed->landflags = 255;
    seed->wparam = 0;
    claimed.push(seed);

    // Seed has 1 child. That child (or a wire chain leading to one) will be
    // the first gate on the path to any input.
    int cand[16];
    int ncand = find_candidates(seed, -1, claimed, cand, 16);
    if(ncand < 1) return false;

    const int MAX_RETRIES = 4;
    for(int retry = 0; retry < MAX_RETRIES; retry++) {
      for(int i=ncand-1; i>0; i--) {
        int j = hrand(i+1);
        int t = cand[i]; cand[i] = cand[j]; cand[j] = t;
        }
      size_t mark = claimed.size();
      if(place_and_grow(seed, cand[0], n_gate_levels, claimed)) return true;
      unclaim_since(claimed, mark);
      }

    return false;
    }

  /** Try to plant a new circuit rooted at cell c. Tries the target depth
   *  first; if the tree won't fit geometrically, falls back to shallower
   *  depths down to 1. At each depth, retries the random gate assignment
   *  a few times so we don't produce a circuit that's already satisfied
   *  with zero orbs placed (e.g. a lone NAND). */
  EX void try_plant(cell *c) {
    if(!c) return;
    if(c->land != laCircuit) return;
    if(pseudohept(c)) return;
    if(c->wall != waNone) return;
    if(c->item != itNone) return;
    if(c->monst != moNone) return;

    int max_depth = target_depth();
    if(max_depth < 1) max_depth = 1;

    // Require enough clearance for the tree we WANT to grow before we try:
    // scan cells within `max_depth + 1` steps of c and refuse if any of them
    // already belongs to another circuit. Otherwise deep trees would try to
    // plant right next to shallower ones and fall back to a smaller depth.
    {
      vector<cell*> frontier;
      frontier.push_back(c);
      std::unordered_set<cell*> visited;
      visited.insert(c);
      int reach = max_depth + 1;
      for(int step=0; step<reach; step++) {
        vector<cell*> next;
        for(cell *cc : frontier) forCellEx(n, cc) {
          if(!n) continue;
          if(visited.count(n)) continue;
          if(in_circuit(n)) return;   // another circuit within our footprint
          visited.insert(n);
          next.push_back(n);
          }
        frontier.swap(next);
        }
      }

    for(int depth = max_depth; depth >= 1; depth--) {
      for(int retry = 0; retry < 8; retry++) {
        claim_bag claimed;
        if(build_tree(c, depth, claimed)) {
          // Reject trivially-satisfied circuits (e.g. lone NAND).
          if(eval(c, 128) != 1) return;
          }
        // Rollback everything the failed build touched.
        unclaim_since(claimed, 0);
        }
      }
    }

  /** Called after the player drops or picks up an orb. If the touched cell is
   *  part of a circuit and the circuit now evaluates to TRUE, dissolve the
   *  gates and drop the seed on the root. */
  EX void check(cell *center) {
    if(!center || center->land != laCircuit) return;
    if(!in_circuit(center)) return;
    cell *seed = find_seed(center);
    if(!seed) return;
    if(eval(seed, 128) != 1) return;

    vector<cell*> cells;
    collect_tree(seed, cells);
    for(cell *c : cells) {
      if(c == seed) {
        c->wall = waNone;
        c->item = itCircuitSeed;
        }
      else {
        c->wall = waNone;
        }
      c->landflags = 0;
      c->wparam = 0;
      }
    // Arm the lightning flash: each frame between now and the next player
    // turn, arm_flash() reasserts ligon on these cells and refreshes lightat,
    // so the arcs stay bright until the player moves (same feel as Storms).
    flash_cells = cells;
    flash_turn = turncount;
    for(cell *c : cells) c->ligon = 1;
    drawLightning();
    addMessage(XLAT("The circuit burns out and drops its seed!"));
    }

EX }

}
