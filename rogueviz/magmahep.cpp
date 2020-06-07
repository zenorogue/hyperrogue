#include "../hyper.h"

// usage: -viz -geo 1 -canvas i -zoom .2 -shot-1000 -magmashape N
// where N = 1, 2, 3, 4

namespace hr {

vector<pair<transmatrix, color_t>> heps;

array<hyperpoint, 8> vertices;

int magmashape = 0;

int magmacount = 1000;

int magmalong = 10;

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

void make() {
  for(int i=0; i<=7; i++)
    vertices[i] = spin(2*M_PI*i/7) * xpush0(1);
  ld xx = vertices[2][0];
  vertices[3][0] = 2 * xx - vertices[3][0];
  vertices[4][0] = 2 * xx - vertices[4][0];
  
  heps.emplace_back(gpushxto0(vertices[2]), 0xFFFFFFFF);
  
  switch(magmashape) {
  
    case 1:
      for(int i=0; i<13; i++)
        heps.emplace_back(heps.back().first * get_adj(0, 3), heps.back().second ^ 0xFFFF00);
      break;

    case 2:
      for(int i=0; i<8; i++)
        heps.emplace_back(heps.back().first * get_adj(3, 0), heps.back().second ^ 0xFFFF00);
      heps.emplace_back(heps.back().first * get_adj(3, 6), heps.back().second ^ 0xFFFF00);
      break;
    
    case 3:
      for(int a=0; a<2; a++) {
        heps.emplace_back(heps.back().first * get_adj(2, 0), heps.back().second ^ 0xFFFF00);
        for(int i=a; i<8; i++)
          heps.emplace_back(heps.back().first * get_adj(3, 0), heps.back().second ^ 0xFFFF00);
        }
      break;
    
    case 4:
      for(int a=0; a<4; a++)
        heps.emplace_back(heps.back().first * get_adj(3, 0), heps.back().second ^ 0xFFFF00);
      heps.emplace_back(heps.back().first * get_adj(3, 1), heps.back().second ^ 0xFFFF00);
      for(int a=0; a<4; a++)
        heps.emplace_back(heps.back().first * get_adj(3, 0), heps.back().second ^ 0xFFFF00);
      break;
    
    case 5:
      for(int a=0; a<6; a++) {
        heps.emplace_back(heps.back().first * get_adj(3, 0), heps.back().second ^ 0xFFFF00);
        }
      for(int b=0; b<magmalong; b++) {
        heps.emplace_back(heps.back().first * get_adj(3, 6), heps.back().second ^ 0xFFFF00);
        heps.emplace_back(heps.back().first * get_adj(3, 0), heps.back().second ^ 0xFFFF00);
        }
  
      for(int a=0; a<7; a++) {
        heps.emplace_back(heps.back().first * get_adj(3, 0), heps.back().second ^ 0xFFFF00);
        }
  
      for(int b=0; b<magmalong; b++) {
        heps.emplace_back(heps.back().first * get_adj(3, 6), heps.back().second ^ 0xFFFF00);
        heps.emplace_back(heps.back().first * get_adj(3, 0), heps.back().second ^ 0xFFFF00);
        }
      break;
    }

  if(1) {
  
    for(int a=0; a<magmacount; a++) {
      hyperpoint p = heps.back().first * vertices[2];
      int small = 0;      
      int big = 0;
      int inner = 0;
      for(auto h: heps)
        for(int a=0; a<7; a++)
          if(hdist(h.first * vertices[a], p) < .001) {
            if(a == 2 || a == 5)
              small ++;
            else if(a == 3 || a == 4)
              inner ++;
            else
              big++;
            }
          
      if(small == 14 && inner == 0 && big == 0) 
        heps.emplace_back(heps.back().first * get_adj(5, 5) * MirrorY, heps.back().second ^ 0xFFFF00);

      else if(small == 4 && big == 2 && inner == 0) 
        heps.emplace_back(heps.back().first * get_adj(5, 5) * MirrorY, heps.back().second ^ 0xFFFF00);

      else if(small == 9 && big == 1 && inner == 0)
        heps.emplace_back(heps.back().first * get_adj(5, 5) * MirrorY, heps.back().second ^ 0xFFFF00);

      else if(inner == 1 && small == 5 && big == 0)
        heps.emplace_back(heps.back().first * get_adj(5, 5) * MirrorY, heps.back().second ^ 0xFFFF00);
      
      else if(small == 4 && big == 1 && inner == 0)
        heps.emplace_back(heps.back().first * get_adj(3, 6), heps.back().second ^ 0xFFFF00);
      else if(big == 1 && small < 4 && inner == 0)
        heps.emplace_back(heps.back().first * get_adj(3, 0), heps.back().second ^ 0xFFFF00);
      else if(big == 2 && small < 4 && inner == 0)
        heps.emplace_back(heps.back().first * get_adj(3, 0), heps.back().second ^ 0xFFFF00);
      
      else {
        println(hlog, "big = ", big, "small = ", small);
        }
      }
    }

  }

void draw_at(transmatrix T, color_t col) {
  for(int i=0; i<=7; i++) 
    curvepoint(T * vertices[i]);
  queuecurve(0xFF, col, PPR::LINE);
  for(int i=0; i<7; i++) {
    hyperpoint h = mid(vertices[i], vertices[i+1]);
    h += spin(M_PI/2) * (vertices[i+1] - vertices[i]) * (among(i, 2, 3, 4) ? 1 : 1) * .05;
    queuestr(T * rgpushxto0(h), 0.4, its(i), 0x80);
    }

  }

void draw_magma() {
  if(heps.empty()) make();
  transmatrix V = ggmatrix(currentmap->gamestart());
  for(auto h: heps)
    draw_at(V * h.first, h.second);
  }

int readArgs() {
  using namespace arg;
           
  if(0) ;
  else if(argis("-magmashape")) {    
    shift(); magmashape = argi();
    }

  else if(argis("-magmacount")) {    
    shift(); magmacount = argi();
    }

  else if(argis("-magmalong")) {    
    shift(); magmalong = argi();
    }

  else return 1;
  return 0;
  }


auto msc = addHook(hooks_frame, 100, draw_magma)
 + addHook(hooks_args, 100, readArgs);

}


