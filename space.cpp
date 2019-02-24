// Hyperbolic Rogue -- euclidean 3D geometry
// Copyright (C) 2011-2018 Zeno Rogue, see 'hyper.cpp' for details

namespace hr {

#if DIM == 3
namespace space {

  typedef long long coord;
  
  struct hrmap_cube : hrmap {
    map<coord, heptagon*> spacemap;
    map<heptagon*, coord> ispacemap;
    hrmap_cube() {
      getOrigin();
      }
    heptagon *getOrigin() {
      return get_at(0);
      }

    heptagon *get_at(coord at) {
      if(spacemap.count(at)) 
        return spacemap[at];
      else {
        auto h = tailored_alloc<heptagon> (6);
        h->c7 = newCell(6, h);
        h->distance = 0;
        h->cdata = NULL;
        spacemap[at] = h;
        ispacemap[h] = at;
        return h;
        }
      }
    
    heptagon *build(heptagon *parent, int d, coord at) {
      auto h = get_at(at);
      h->c.connect(d^1, parent, d, false);
      return h;
      }
  
    heptagon *createStep(heptagon *parent, int d) {
      int at = ispacemap[parent];
      coord shifttable[6] = { +1, -1, +1000, -1000, +1000000, -1000000 };
      return build(parent, d, at + shifttable[d]);
      }
    };
  
  hrmap_cube* cubemap() {
    return ((hrmap_cube*) currentmap);
    }

  hrmap* new_map() {
    return new hrmap_cube;
    }

  heptagon *createStep(heptagon *parent, int d) {
    return cubemap()->createStep(parent, d);
    }
  
  int getcoord(coord x, int a) { 
    for(int k=0; k<a; k++) { x -= getcoord(x, 0); x /= 1000; }
    x %= 1000; 
    if(x>500) x -= 1000; 
    if(x<-500) x += 500; 
    return x; 
    }

  void draw() {
    dq::visited.clear();
    dq::enqueue(viewctr.at, cview());
    
    while(!dq::drawqueue.empty()) {
      auto& p = dq::drawqueue.front();
      heptagon *h = get<0>(p);
      transmatrix V = get<1>(p);
      dynamicval<ld> b(band_shift, get<2>(p));
      bandfixer bf(V);
      dq::drawqueue.pop();
            
      cell *c = h->c7;
      if(!do_draw(c, V)) continue;
      drawcell(c, V, 0, false);

      for(int i=0; i<6; i++)
        dq::enqueue(h->move(i), V * cpush(i>>1, (i&1) ? -1 : 1));
      }
    }
  
  transmatrix relative_matrix(heptagon *h2, heptagon *h1) {
    auto cm = cubemap();
    coord a = cm->ispacemap[h2] - cm->ispacemap[h1];
    return eupush3(getcoord(a, 0), getcoord(a, 1), getcoord(a, 2));
    }

  int celldistance(cell *c1, cell *c2) {
    auto cm = cubemap();
    coord a = cm->ispacemap[c1->master] - cm->ispacemap[c2->master];
    return getcoord(a, 0) + getcoord(a, 1) + getcoord(a, 2);
    }

  }
#endif
}
