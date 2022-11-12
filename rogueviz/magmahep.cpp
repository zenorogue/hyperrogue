#include "../hyper.h"

// usage: -viz -geo 1 -canvas i -zoom .2 -shot-1000 -magmashape N
// where N = 1, 2, 3, 4

namespace hr {

int magmav = 7;

/* vertices of the Magma's heptagon */
vector<hyperpoint> vertices;

hyperpoint hcenter;

/* how are all Magma's heptagons transformed and colored */
vector<pair<transmatrix, color_t>> heps;

int magmashape = 0;

int magmacount = 1000;

int magmalong = 10;

bool magmadebug = false;

/* transformation from the original to the next heptagon; 
   edge a of the original heptagon matches edge b of the next heptagon */

EX transmatrix get_adj(int a, int b) {
  
  hyperpoint vl = vertices[a];
  hyperpoint vr = vertices[a+1];
  hyperpoint vm = mid(vl, vr);
      
  transmatrix rm = gpushxto0(vm);
  
  hyperpoint xvl = vertices[b];
  hyperpoint xvr = vertices[b+1];
  hyperpoint xvm = mid(xvl, xvr);
  
  transmatrix xrm = gpushxto0(xvm);
  
  transmatrix Res = rgpushxto0(vm) * rspintox(rm*vr);
    
  Res = Res * spintox(xrm*xvl) * xrm;
  
  return Res;
  }

pair<int, int> hash(hyperpoint h) {
  return {int(h[0] * 1000000 + .5), int(h[1] * 1000000 + .5)};
  }

void make() {

  int& v = magmav;
  
  /* compute the vertices */
  vertices.resize(magmav+1);
  for(int i=0; i<=magmav; i++)
    vertices[i] = spin(TAU*(i+(v-7)/4.)/v) * xpush0(1);
  ld xx = vertices[2][0];
  
  int down = v/2 + 2;
  
  for(int k=3; k<down; k++)
    vertices[k][0] = 2 * xx - vertices[k][0];
  
  hcenter = Hypc;
  for(int i=0; i<magmav; i++) hcenter += vertices[i];
  hcenter = normalize(hcenter);
  
  vertices.resize(magmav);
  for(int i=0; i<magmav; i++) vertices.push_back(vertices[i]);
  for(int i=0; i<magmav; i++) vertices.push_back(vertices[i]);
  
  map<pair<int, int>, int> counts;
  
  int big = v - 2;
  
  auto rehash = [&] (const transmatrix& T) {
    for(int a=0; a<v; a++) {
      auto hashval = hash(T * vertices[a]);
      if(a == 2 || a == down)
        counts[hashval]++;
      else if(a > 2 && a < down)
        counts[hashval] += 2*v - big;
      else 
        counts[hashval] += big;
      }
    };

  heps.emplace_back(Id, 0xFFFFFFFF);
  
  auto advance = [&] (int i, int j, bool mirror = false) {
    transmatrix T = heps.back().first * get_adj(i, j);
    if(mirror) T = T * MirrorY;   
    heps.emplace_back(T, heps.back().second ^ 0xFFFF00);
    };
  
  /* create the core */
  
  int last = v-1;
  int t = 3;

  switch(magmashape) {
  
    case 1:
      for(int i=1; i<2*v; i++)
        advance(0, t);
      break;

    case 2:
      for(int i=0; i<t+1; i++)
        advance(t, 0);
      break;
    
    case 3:
      for(int a=0; a<2; a++) {
        advance(t-1, 0);
        for(int i=a; i<v+1; i++)
          advance(t, 0);
        }
      break;
    
    case 4: {     
      advance(t, 1);
      for(int a=0; a<v-3; a++)
        advance(t, 0);
      advance(t, 1);
      for(int a=0; a<v-4; a++)
        advance(t, 0);
      break;
      }
    
    case 5:
      for(int a=0; a<v-1; a++) {
        advance(t, 0);
        }
      for(int b=0; b<magmalong; b++) {
        advance(t, last);
        advance(t, 0);
        }
  
      for(int a=0; a<v; a++) {
        advance(t, 0);
        }
  
      for(int b=0; b<magmalong; b++) {
        advance(t, last);
        advance(t, 0);
        }
      break;
    }
  
  /* center the core */

  hyperpoint center = Hypc;
  for(auto& h: heps) for(int i=0; i<v; i++)
    center += h.first * vertices[i];
  
  center = normalize(center);
  
  for(auto& h: heps) h.first = gpushxto0(center) * h.first;
  
  counts.clear();
  for(auto& h: heps) rehash(h.first);
  
  /* grow */

  for(int a=0; a<magmacount; a++) {
    hyperpoint p = heps.back().first * vertices[2];
    
    int total = counts[hash(p)];
    
      println(hlog, "total ", total);
    if(total == 2*v) 
      advance(down, down, true);

    else if(total == 2 * v - big)
      advance(t, last);
    else if(total < 2 * v)
      advance(t, 0);
    else
      break;

    rehash(heps.back().first);
    }

  }

void draw_at(transmatrix T, color_t col, int id) {
  for(int i=0; i<=magmav; i++) 
    curvepoint(T * vertices[i]);
  queuecurve(0xFF, col, PPR::LINE);
  if(magmadebug) {
    for(int i=0; i<magmav; i++) {
      hyperpoint h = mid(vertices[i], vertices[i+1]);
      h += spin90() * (vertices[i+1] - vertices[i]) * .05;
      queuestr(T * rgpushxto0(h), 0.4, its(i), 0x80);
      }
    queuestr(T * rgpushxto0(hcenter), 0.4, "#"+its(id), 0x80);
    }

  }

void draw_magma() {
  if(heps.empty()) make();
  transmatrix V = ggmatrix(currentmap->gamestart());
  int id = 0;
  for(auto h: heps)
    draw_at(V * h.first, h.second, id++);
  }

int readArgs() {
  using namespace arg;
           
  if(0) ;
  else if(argis("-magmashape")) {    
    shift(); magmashape = argi();
    }

  else if(argis("-magmav")) {    
    shift(); magmav = argi();
    }

  else if(argis("-magmacount")) {    
    shift(); magmacount = argi();
    }

  else if(argis("-magmalong")) {    
    shift(); magmalong = argi();
    }

  else if(argis("-magmadebug")) {
    shift(); magmadebug = argi();
    }

  else return 1;
  return 0;
  }


auto msc = addHook(hooks_frame, 100, draw_magma)
 + addHook(hooks_args, 100, readArgs);

}


