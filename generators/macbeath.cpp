// Fricke-Macbeath Surface generator
// Copyright (C) 2018 Zeno Rogue, see 'hyper.cpp' for details

#include <stdio.h>
#include <vector>
#include <array>
#include <map>

using namespace std;

int isize(const auto x) { return x.size(); }

int mul(int a, int b) { 
  int p = 0;
  for(int i=0; i<3; i++) if((a>>i)&1)
  for(int j=0; j<3; j++) if((b>>j)&1)
    p ^= (1<<(i+j));
  
  for(int z=4; z>=0; z--)
    if(p&(8<<z)) p ^= (11<<z);
    
  return p;
  }

int multable[8][8];

int elements = 0;

vector<array<int, 4>> psl_elements;

int multable_psl[504][504];

map<array<int, 4>, int> back;

int mpow(int a, int n) {
  int z = a;
  while(n > 1) z = multable_psl[a][z], n--;
  return z;
  }

int inset[504];

int main() {
  for(int a=0; a<8; a++) {
    for(int b=0; b<8; b++) printf("%d ", mul(a, b));
    printf("\n");
    }

  for(int a=0; a<8; a++)
    for(int b=0; b<8; b++) 
      multable[a][b] = mul(a, b);

  for(int a=0; a<8; a++)
    for(int b=0; b<8; b++) 
      for(int c=0; c<8; c++)
        for(int d=0; d<8; d++) 
          if((multable[a][d] ^ multable[b][c]) == 1) {
            array<int, 4> arr = { a, b, c, d};
            psl_elements.emplace_back(arr);
            }
          
  printf("elements = %d\n", isize(psl_elements));

  for(int a=0; a<504; a++) back[psl_elements[a]] = a;
  
  for(int a=0; a<504; a++)
  for(int b=0; b<504; b++) {
    auto pa = psl_elements[a];
    auto pb = psl_elements[b];
    array<int, 4> pc;
    for(int s=0; s<4; s++) {
      int s0 = s&2;
      int s1 = s&1;
      pc[s] = (multable[pa[s0]][pb[s1]] ^ multable[pa[s0^1]][pb[s1^2]]);
      }
    multable_psl[a][b] = back[pc];
    }

  int id;
  
  for(int a=0; a<504; a++)
    if(multable_psl[a][a] == a) id = a;
  
  printf("id = %d\n", id);

  int xid = 0;

  for(int R=0; R<504; R++)
  for(int P=0; P<504; P++) {
    if(multable_psl[P][P] != id) continue;
    if(mpow(multable_psl[R][P], 3) != id) continue;
    if(mpow(R, 7) != id) continue;
    xid++;
    vector<int> allels = {id};
    inset[id] = xid;
    for(int i=0; i<isize(allels); i++) {
      for(int m: {P, R}) {
        int im = allels[i];
        int a = multable_psl[im][m];
        // printf("%d x %d = %d\n", im, m, a);
        if(inset[a] != xid) inset[a] = xid, allels.push_back(a);
        }
      }
    printf("R = %d P = %d count = %d\n", R, P, isize(allels));

    array<int, 504> cellid;
    array<int, 504> bycellid;

    for(int a=0; a<504; a++) cellid[a] = 0;

    int ncell = 0;
    for(int a=0; a<504; a++) if(cellid[a] == 0) {
      int b = a;
      for(int s=0; s<7; s++) cellid[b] = ncell + s, bycellid[ncell + s] = b, b = multable_psl[b][R];
      ncell += 7;
      }
    
    printf("ncell = %d\n", ncell);

    for(int a=0; a<72; a++) {
      printf("/* %03d */ ", a);
      for(int b=0; b<7; b++) {
        printf("%d, ", cellid[multable_psl[bycellid[7*a+b]][P]]);
        }
      printf("\n");
      }

    return 0;
    }

  return 0;
  }
