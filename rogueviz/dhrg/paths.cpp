// computePath to convert HyperRogue's cell to DHRG's mycell (mycell.cpp)

namespace dhrg {

// compute the path to the given cell

hrmap *mapptr;

string computePath(cell *c) {
  string s = "";
  int d = celldist(c);
  while(d > 0) {
    char z = '0';
    cell *c1 = ts::left_parent(c, celldist);
    int id = neighborId(c1, c);
    d--;
    if(d == 0) z = '0'+id;
    else while(true) {
      id--;
      if(id<0) id += c1->type;
      if(celldist(createMov(c1, id)) <= d) break;
      if(z >= 'a') { printf("<%d> ", celldist(createMov(c1, id))); }
      z++;
      }
    s += z; c = c1;
    }
  reverse(s.begin(), s.end());
  return s;
  }

int recycle_counter = 0;

void recycle_compute_map() {
  if(mapptr) {
    delete mapptr;
    mapptr = NULL;
    }
  }

string computePath(const transmatrix& T) {
  if(recycle_counter >= 1000)
    recycle_compute_map();
  if(!mapptr) mapptr = bt::in() ? bt::new_map() : new hrmap_hyperbolic;
  recycle_counter++;
  dynamicval<hrmap*> dv (currentmap, mapptr);
  cell *c = mapptr->gamestart();
  hyperpoint T0 = T * C0;
  // call HyperRogue's function virtualRebase: 
  // a point in the hyperbolic plane is given by cell c and point T0 relative to c
  // change c and T0 so that the same point is specified, but with minimal T0
  virtualRebase(c, T0);
  return computePath(c);
  }

}
