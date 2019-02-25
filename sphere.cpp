// Hyperbolic Rogue -- spherical spaces
// Copyright (C) 2011-2018 Zeno Rogue, see 'hyper.cpp' for details

namespace hr {

// --- spherical geometry ---

int spherecells() {
  if(S7 == 5) return (elliptic?6:12);
  if(S7 == 4) return (elliptic?3:6);
  if(S7 == 3 && S3 == 4) return (elliptic?4:8);
  if(S7 == 3) return 4;
  if(S7 == 2) return (elliptic?1:2);
  if(S7 == 1) return 1;
  return 12;
  }

vector<int> siblings;
  
struct hrmap_spherical : hrmap {
  heptagon *dodecahedron[12];
  eVariation mvar;

  hrmap_spherical() {
    mvar = variation;
    for(int i=0; i<spherecells(); i++) {
      heptagon& h = *(dodecahedron[i] = tailored_alloc<heptagon> (S7));
      h.s = hsOrigin;
      h.emeraldval = i;
      h.zebraval = i;
      h.fiftyval = i;
      h.rval0 = h.rval1 = 0;
      h.alt = NULL;
      h.cdata = NULL;
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

  heptagon *getOrigin() { return dodecahedron[0]; }

  ~hrmap_spherical() {
    dynamicval<eVariation> ph(variation, mvar);
    for(int i=0; i<spherecells(); i++) clearHexes(dodecahedron[i]);
    for(int i=0; i<spherecells(); i++) tailored_delete(dodecahedron[i]);
    }    

  void verify() {
    for(int i=0; i<spherecells(); i++) for(int k=0; k<S7; k++) {
      heptspin hs(dodecahedron[i], k, false);
      heptspin hs2 = hs + wstep + (S7-1) + wstep + (S7-1) + wstep + (S7-1);
      if(S3 == 4) hs2 = hs2 + wstep + (S7-1);
      if(hs2.at != hs.at) printf("error %d,%d\n", i, k);
      }
    for(int i=0; i<spherecells(); i++) verifycells(dodecahedron[i]);
    }
  };

heptagon *getDodecahedron(int i) {
  hrmap_spherical *s = dynamic_cast<hrmap_spherical*> (currentmap);
  if(!s) return NULL;
  return s->dodecahedron[i];
  }

namespace sphere3 {

vector<hyperpoint> vertices120;
array<transmatrix, 120> vmatrix120;
vector<int> adj0;
array<array<int, 4>, 120> js;
array<hyperpoint, 60> dodefaces;

hyperpoint zero4;

int root;

ld norm(hyperpoint a, hyperpoint b) {
  ld res = 0;
  for(int i=0; i<4; i++) res += pow(a[i]-b[i], 2);
  return res;
  }

void gen600() {
  vertices120.clear();
  root = 23;
  
  /// coordinates taken from Wikipedia
  
  for(int a=0; a<16; a++) {
    hyperpoint v = zero4;
    for(int i=0; i<4; i++) v[i] = ((a >> i) & 1) ? .5 : -.5;
    vertices120.push_back(v);
    }
  
  for(int i=0; i<4; i++) for(int q: {-1, 1}) {
    hyperpoint v = zero4;
    v[i]=q;
    vertices120.push_back(v);
    }
  
  ld phi = (1 + sqrt(5)) / 2;
  
  array<ld, 4> coo = {1, phi, 1/phi, 0};
  
  // all permutations
  array<int, 4> tab;
  for(int i=0; i<4; i++) tab[i] = i;
  
  do {

    // check the permutation's sign
    auto tabs = tab;
    int inv = 0;

    for(int i=0; i<4; i++) while(tabs[i] != i) {
      swap(tabs[i], tabs[tabs[i]]);
      inv++;
      }
    
    if(inv&1) goto again;
    
    // 8 vertices for each permutation
    
    for(int sg=0; sg<8; sg++) {
      hyperpoint v;
      for(int i=0; i<4; i++) 
        v[i] = (((sg >> tab[i])&1) ? 1 : -1) * coo[tab[i]]/2;
      vertices120.push_back(v);
      }
      
    again: ;
    }
  while(std::next_permutation(tab.begin(), tab.end()));
  
  if(isize(vertices120) != 120) {
    printf("error: wrong number of vertices\n");
    exit(1);
    }
  // we add edges between vertices which are close to each other
  // ((specifically in distance 1/phi/phi)
  
  bool inedge[120][120];
    
  for(int i=0; i<120; i++)
  for(int j=0; j<120; j++) {
    ld d = hdist(vertices120[i], vertices120[j]);
    inedge[i][j] = (i != j) && d < sqrt(.4);
    }
  
  vector<hyperpoint> cellvertices;
  
  for(int i=0; i<120; i++)
    for(int j=0; j<120; j++) if(inedge[i][j])
    for(int k=0; k<120; k++) if(inedge[i][k] && inedge[k][j])
    for(int l=0; l<120; l++) if(inedge[i][l] && inedge[j][l] && inedge[k][l]) {
      array<int, 4> ijkl = {i, j, k, l};
      transmatrix T;
      for(int z=0; z<4; z++) set_column(T, z, vertices120[ijkl[z]]);
      if(det(T) > 0) js[i] = ijkl;
      }
    
  /* transmatrix src;
  for(int z=0; z<4; z++) set_column(src, z, vertices120[js[0][z]]); */

  for(int i=0; i<120; i++)
    for(int z=0; z<4; z++) set_column(vmatrix120[i], z, vertices120[js[i][z]]);

  for(int i=0; i<120; i++) {
    println(hlog, i, ": ", js[i], " -> ", vmatrix120[i]);  
    println(hlog, vmatrix120[i] * hyperpoint(1,0,0,0), " should be ", vertices120[i]);  
    }
  
  adj0.clear();
  for(int i=0; i<120; i++) if(inedge[root][i]) adj0.push_back(i);
  
  using namespace hyperpoint_vec;
  
  int id = 0;
  for(int i=0; i<12; i++) {
    int ot = adj0[i];
    
    vector<int> pentagon;
    for(int j: adj0) if(inedge[ot][j]) pentagon.push_back(j);
    println(hlog, i, ": ", pentagon);
    
    int illegal = -1;
    int at = pentagon[0];
    for(int d=0; d<5; d++) {
      for(int s: pentagon) if(inedge[at][s] && s != illegal) {
        hyperpoint m = vertices120[root] + vertices120[ot] + vertices120[at] + vertices120[s];
        m = mid(m, m);
        println(hlog, id, ": ", m);
        dodefaces[id++] = m;
        illegal = at;
        at = s;
        break;
        }
      }
    }
  
  printf("id = %d\n", id);
  }

struct hrmap_spherical3 : hrmap {
  heptagon* cells[120];

  hrmap_spherical3() {
    gen600();
    for(int i=0; i<120; i++) {
      cells[i] = tailored_alloc<heptagon> (12);
      heptagon& h = *(cells[i]);
      h.s = hsOrigin;
      h.emeraldval = i;
      h.zebraval = i;
      h.fiftyval = i;
      h.rval0 = h.rval1 = 0;
      h.alt = NULL;
      h.cdata = NULL;
      h.c.fullclear();
      h.fieldval = i;
      h.c7 = newCell(12, &h);      
      }

    for(int i=0; i<120; i++) {
      for(int k=0; k<12; k++) {
        hyperpoint which = vmatrix120[i] * inverse(vmatrix120[root]) * vertices120[adj0[k]];
        for(int s=0; s<120; s++) if(hdist(which, vertices120[s]) < 1e-6) {
          cells[i]->move(k) = cells[s];
          println(hlog, i,".",k, " -> ", s, " ; ", js[i], " distance = ", hdist(vertices120[i], vertices120[s]));
          }
        }
      }

    for(int i=0; i<120; i++)
      for(int k=0; k<12; k++) 
        for(int l=0; l<12; l++)
          if(cells[i]->move(k)->move(l) == cells[i])
            cells[i]->c.setspin(k, l, false);
    }

  heptagon *getOrigin() { return cells[root]; }

  ~hrmap_spherical3() {
    for(int i=0; i<120; i++) tailored_delete(cells[i]);
    }    
  };

transmatrix gmatr(heptagon *h) {
  return vmatrix120[h->zebraval] * inverse(vmatrix120[root]);
  }

transmatrix relative_matrix(heptagon *h2, heptagon *h1) {
  return inverse(gmatr(h1)) * gmatr(h2);
  }

void draw() {
  auto m = (hrmap_spherical3*) currentmap;

  int old = viewctr.at->zebraval;

  for(int i=0; i<120; i++)
    drawcell(m->cells[i]->c7, View * relative_matrix(m->cells[i], viewctr.at), 0, false);
  }

void makewax(int x) {
  auto m = (hrmap_spherical3*) currentmap;
  for(int i=0; i<120; i++) m->cells[i]->c7->wall = waNone;
  m->cells[70]->c7->wall = waDune;
  int cols[16] = {0x202020, 0x2020A0, 0x20A020, 0x20A0A0, 0xA02020, 0xA020A0, 0xA0A020, 0xA0A0A0,
    0x606060, 0x6060FF, 0x60FF60, 0x60FFFF, 0xFF6060, 0xFF60FF, 0xFFFF60, 0xFFFFFF };
  if(x) for(int i=0; i<12; i++) {
    m->cells[70]->c7->move(i)->wall = waWaxWall;
    m->cells[70]->c7->move(i)->landparam = cols[i];
    }
  }

#if CAP_COMMANDLINE  
int readArgs() {
  using namespace arg;
           
  if(argis("-wax1")) {
    PHASE(3);
    start_game();
    makewax(1);
    }
  else if(argis("-wax0")) {
    PHASE(3);
    start_game();
    makewax(0);
    }
  else return 1;
  return 0;
  }

auto hook = 
  addHook(hooks_args, 100, readArgs);
#endif

}

}
