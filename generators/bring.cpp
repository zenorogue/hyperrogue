// Bring's Surface generator
// Copyright (C) 2018 Zeno Rogue, see 'hyper.cpp' for details

#include <stdio.h>
#include <vector>
#include <array>
#include <map>
#include <algorithm>

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

int elements = 0;

vector<array<int, 5>> s5_elements;

map<array<int, 5>, int> back;

int multable[504][504];

int mpow(int a, int n) {
  int z = a;
  while(n > 1) z = multable[a][z], n--;
  return z;
  }

int inset[504];

int main() {
  array<int, 5> ar;
  for(int i=0; i<5; i++) ar[i] = i;
  
  do {
    back[ar] = isize(s5_elements);
    s5_elements.push_back(ar);
    }
  while(next_permutation(ar.begin(), ar.end()));
  
  for(int a=0; a<120; a++)
    for(int b=0; b<120; b++) {
      array<int, 5> res;
      for(int i=0; i<5; i++)
        res[i] = s5_elements[a] [ s5_elements[b][i] ];
      multable[a][b] = back[res];
      }
          
  int id;
  
  for(int a=0; a<120; a++)
    if(multable[a][a] == a) id = a;
  
  printf("id = %d\n", id);

  int xid = 0;
  
  int sols = 0;

  for(int R=0; R<120; R++)
  for(int P=0; P<120; P++) {
    if(P == 0 && mpow(R, 5) == id) printf("deg5 R\n");
    if(multable[P][P] != id) continue;
    if(mpow(multable[R][P], 4) != id) continue;
    if(mpow(R, 5) != id) continue;
    xid++;
    vector<int> allels = {id};
    inset[id] = xid;
    for(int i=0; i<isize(allels); i++) {
      for(int m: {P, R}) {
        int im = allels[i];
        int a = multable[im][m];
        // printf("%d x %d = %d\n", im, m, a);
        if(inset[a] != xid) inset[a] = xid, allels.push_back(a);
        }
      }
    if(isize(allels) != 120) continue;
    printf("R = %d P = %d count = %d\n", R, P, isize(allels));
    for(int a: s5_elements[R]) printf("%d ", a); printf("// ");
    for(int a: s5_elements[P]) printf("%d ", a); printf("\n");
    sols++;
    
    array<int, 120> cellid;
    array<int, 120> bycellid;

    for(int a=0; a<120; a++) cellid[a] = 0;

    int ncell = 0;
    for(int a=0; a<120; a++) if(cellid[a] == 0) {
      int b = a;
      for(int s=0; s<5; s++) cellid[b] = ncell + s, bycellid[ncell + s] = b, b = multable[b][R];
      ncell += 5;
      }
    
    printf("ncell = %d\n", ncell);

    for(int a=0; a<24; a++) {
      printf("/* %03d */ ", a);
      for(int b=0; b<5; b++) {
        printf("%d, ", cellid[multable[bycellid[5*a+b]][P]]);
        }
      printf("\n");
      }

    return 0;
    }
  
  printf("solutions = %d\n", sols);

  return 0;
  }
