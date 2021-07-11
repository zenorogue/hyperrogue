// Hyperbolic Rogue -- infinite-order tessellations
// Copyright (C) 2011-2019 Zeno Rogue, see 'hyper.cpp' for details

/** \file inforder3.cpp
 *  \brief infinite-order tessellations
 *
 *  very simple
 */

#include "hyper.h"

namespace hr {

EX namespace inforder {

  EX bool in() { return S3 >= OINF; }

  EX bool mixed() { return cgflags & qINFMIXED; }
  
  EX int alt_degree;

  struct hrmap_inforder : hrmap_hyperbolic {

    heptagon *create_step(heptagon *h, int direction) override {
      int deg = h->type;
      if(mixed()) deg = 7 - deg;
      auto h1 = init_heptagon(deg);
      bool par = h->s == hsA && direction == 0;
      h->c.connect(direction, h1, par ? 1 + hrand(2) : 0, false);

      h1->s = hsA;
      h1->distance = h->distance + (par ? -1 : 1);
      h1->c7 = newCell(deg, h1);

      return h1;
      }

    };
  
  EX hrmap* new_map() { return new hrmap_inforder; }  
  
  EX int celldistance(cell *c1, cell *c2) {
    int d = 0;
    while(true) {
      if(c1 == c2) return d;
      else if(c1->master->distance >= c2->master->distance) c1 = c1->move(0), d++;
      else c2 = c2->move(0), d++;
      }
    }
  
  EX }


}
