// Hyperbolic Rogue -- spherical spaces
// Copyright (C) 2011-2018 Zeno Rogue, see 'hyper.cpp' for details

/** \file sphere.cpp
 *  \brief spherical spaces
 */

#include "hyper.h"
namespace hr {

// --- spherical geometry ---

EX int spherecells() {
  if(S7 == 5) return (elliptic?6:12);
  if(S7 == 4) return (elliptic?3:6);
  if(S7 == 3 && S3 == 4) return (elliptic?4:8);
  if(S7 == 3) return 4;
  if(S7 == 2) return (elliptic?1:2);
  if(S7 == 1) return 1;
  return 12;
  }

EX vector<int> siblings;
  
struct hrmap_spherical : hrmap_standard {
  heptagon *dodecahedron[12];

  hrmap_spherical() {
    for(int i=0; i<spherecells(); i++) {
      heptagon& h = *(dodecahedron[i] = init_heptagon(S7));
      h.s = hsOrigin;
      h.emeraldval = i;
      h.zebraval = i;
      h.fiftyval = i;
      h.c.fullclear();
      h.fieldval = i;
      if(!IRREGULAR) h.c7 = newCell(S7, &h);
      }
    if(S7 == 5)
      siblings = {1, 0, 10, 4, 3, 8, 9, 11, 5, 6, 2, 7};
    else
      siblings = {1, 0, 3, 2, 5, 4};
    
    if(S7 == 3 && S3 == 4) {
      for(int i=0; i<8; i++) {
        dodecahedron[i]->move(0) = dodecahedron[i^1];
        dodecahedron[i]->c.setspin(0, 0, false);
        dodecahedron[i]->move(1) = dodecahedron[i^2];
        dodecahedron[i]->c.setspin(1, 1, false);
        dodecahedron[i]->move(2) = dodecahedron[i^4];
        dodecahedron[i]->c.setspin(2, 2, false);
        }
      for(int i=0; i<8; i++) {
        int s = (i&1)+((i&2)>>1)+((i&4)>>2);
        if((s&1) == 1) {
          swap(dodecahedron[i]->move(1), dodecahedron[i]->move(2));
          int a = dodecahedron[i]->c.spin(1);
          int b = dodecahedron[i]->c.spin(2);
          dodecahedron[i]->c.setspin(1, b, false);
          dodecahedron[i]->c.setspin(2, a, false);
          dodecahedron[i]->move(1)->c.setspin(b, 1, false);
          dodecahedron[i]->move(2)->c.setspin(a, 2, false);
          }
        }
      for(int i=0; i<8; i++)
      for(int j=0; j<3; j++)
        if(dodecahedron[i]->move(j)->move(dodecahedron[i]->c.spin(j)) != dodecahedron[i])
          println(hlog, "8");
      }
      
    else if(S7 == 4 && elliptic) {
      for(int i=0; i<3; i++) {
        int i1 = (i+1)%3;
        int i2 = (i+2)%3;
        dodecahedron[i]->move(0) = dodecahedron[i1];
        dodecahedron[i]->c.setspin(0, 1, false);
        dodecahedron[i]->move(1) = dodecahedron[i2];
        dodecahedron[i]->c.setspin(1, 0, false);
        dodecahedron[i]->move(2) = dodecahedron[i1];
        dodecahedron[i]->c.setspin(2, 3, true);
        dodecahedron[i]->move(3) = dodecahedron[i2];
        dodecahedron[i]->c.setspin(3, 2, true);
        }
      }
    else for(int i=0; i<S7; i++) {
      dodecahedron[0]->move(i) = dodecahedron[i+1];
      dodecahedron[0]->c.setspin(i, 0, false);
      dodecahedron[i+1]->move(0) = dodecahedron[0];
      dodecahedron[i+1]->c.setspin(0, i, false);
      
      dodecahedron[i+1]->move(1) = dodecahedron[(i+S7-1)%S7+1];
      dodecahedron[i+1]->c.setspin(1, S7-1, false);
      dodecahedron[i+1]->move(S7-1) = dodecahedron[(i+1)%S7+1];
      dodecahedron[i+1]->c.setspin(S7-1, 1, false);
      
      if(S7 == 5 && elliptic) {
        dodecahedron[i+1]->move(2) = dodecahedron[(i+2)%S7+1];
        dodecahedron[i+1]->c.setspin(2, 3, true);
        dodecahedron[i+1]->move(3) = dodecahedron[(i+3)%S7+1];
        dodecahedron[i+1]->c.setspin(3, 2, true);
        }

      else if(S7 == 5) {
        dodecahedron[6]->move(i) = dodecahedron[7+i];
        dodecahedron[6]->c.setspin(i, 0, false);
        dodecahedron[7+i]->move(0) = dodecahedron[6];
        dodecahedron[7+i]->c.setspin(0, i, false);
  
        dodecahedron[i+7]->move(1) = dodecahedron[(i+4)%5+7];
        dodecahedron[i+7]->c.setspin(1, 4, false);
        dodecahedron[i+7]->move(4) = dodecahedron[(i+1)%5+7];
        dodecahedron[i+7]->c.setspin(4, 1, false);
        
        dodecahedron[i+1]->move(2) = dodecahedron[7+(10-i)%5];
        dodecahedron[i+1]->c.setspin(2, 2, false);
        dodecahedron[7+(10-i)%5]->move(2) = dodecahedron[1+i];
        dodecahedron[7+(10-i)%5]->c.setspin(2, 2, false);
  
        dodecahedron[i+1]->move(3) = dodecahedron[7+(9-i)%5];
        dodecahedron[i+1]->c.setspin(3, 3, false);
        dodecahedron[7+(9-i)%5]->move(3) = dodecahedron[i+1];
        dodecahedron[7+(9-i)%5]->c.setspin(3, 3, false);
        }
      if(S7 == 4) {
        dodecahedron[5]->move(3-i) = dodecahedron[i+1];
        dodecahedron[5]->c.setspin(3-i, 2, false);
        dodecahedron[i+1]->move(2) = dodecahedron[5];
        dodecahedron[i+1]->c.setspin(2, 3-i, false);
        }
      }

    #if CAP_IRR
    if(IRREGULAR) {
      irr::link_start(dodecahedron[0]);
      for(int i=0; i<spherecells(); i++)
        for(int j=0; j<S7; j++)
          irr::may_link_next(dodecahedron[i], j);
      }
    #endif
    }

  heptagon *getOrigin() override { return dodecahedron[0]; }

  ~hrmap_spherical() {
    for(int i=0; i<spherecells(); i++) clearHexes(dodecahedron[i]);
    for(int i=0; i<spherecells(); i++) tailored_delete(dodecahedron[i]);
    }    

  void verify() override {
    for(int i=0; i<spherecells(); i++) for(int k=0; k<S7; k++) {
      heptspin hs(dodecahedron[i], k, false);
      heptspin hs2 = hs + wstep + (S7-1) + wstep + (S7-1) + wstep + (S7-1);
      if(S3 == 4) hs2 = hs2 + wstep + (S7-1);
      if(hs2.at != hs.at) printf("error %d,%d\n", i, k);
      }
    for(int i=0; i<spherecells(); i++) verifycells(dodecahedron[i]);
    }
  
  map<cell*, transmatrix> where;
  
  transmatrix get_where(cell *c) {
    if(where.count(c)) return where[c];
    int d = celldist(c);
    if(d == 0) return where[c] = Id;
    else forCellIdCM(c1, i, c) 
      if(celldist(c1) < d) {
        transmatrix T = get_where(c1);
        T = T * iadj(c, i);
        where[c] = T;
        return T;
        }
    return Id;
    }
  
  void on_dim_change() override {
    hrmap_standard::on_dim_change();
    where.clear();
    }

  transmatrix relative_matrixc(cell *c2, cell *c1, const hyperpoint& hint) override {
    transmatrix T = iso_inverse(get_where(c1)) * get_where(c2);
    if(elliptic) fixelliptic(T);
    return T;
    }
  };

EX heptagon *getDodecahedron(int i) {
  if(fake::in()) return FPIU(getDodecahedron(i));
  hrmap_spherical *s = dynamic_cast<hrmap_spherical*> (currentmap);
  if(!s) return NULL;
  return s->dodecahedron[i];
  }

EX hrmap* new_spherical_map() { return new hrmap_spherical; }

}
