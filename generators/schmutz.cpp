// Schmutz's Surface generator
// Copyright (C) 2018 Zeno Rogue, see 'hyper.cpp' for details

#include <stdio.h>
#include <vector>
#include <array>
#include <map>
#include <algorithm>

using namespace std;

void dualize(vector<int>& t) {
  int N = t.size();
  /*
  printf("N = %d\n", N);

  printf("pre dual:\n");
  for(int a=0; a<N; a++) {
    printf("%3d, ", t[a]);
    if(a % 12 == 11) printf("\n");    
    }
  
  printf("post dual:\n");
  */

  vector<int> tmap(N, -1);
  vector<int> trmap(N, -1);
  int s = 0;
  for(int i=0; i<N; i++) if(tmap[i] == -1) {
    int j = i;
    for(int k=0; k<12; k++) {
      tmap[j] = s;
      trmap[s] = j;
      s++;
      int oj = j;
      j = t[j];
      j = (j % 3 == 2) ? (j-2) : (j+1);
      // printf("%d -> %d\n", oj, j);
      }
    if(j != i) printf("not equal\n"), exit(1);
    // printf("OK\n");
    }
  // for(int a: tmap) printf("%d ", a); printf("\n");
  // for(int a: trmap) printf("%d ", a); printf("\n");
  vector<int> res(N, -1);
  for(int i=0; i<N; i++) {
    // printf("i=%d\n", i);
    res[i] = tmap[t[trmap[i]]];
    }
  // for(int a: res) printf("%d ", a); printf("\n");
  t = res;

  for(int i=0; i<N; i++) {
    int a = i;
    for(int s=0; s<3; s++) {
      a = res[a];
      a++;
      if(a % 12 == 0) a -= 12;
      }
    if(a != i) { printf("bug\n"); exit(1); }
    }
  }

int main() {

  vector<int> triangles;
  
  for(int a=0; a<12; a++) {
    triangles.emplace_back(3 * ((a+1) % 12) + 1);
    triangles.emplace_back(3 * ((a+11) % 12));
    triangles.emplace_back(36 + (a%4) * 3 + 2 - (a/4));
    }
  
  for(int a=0; a<12; a++)
    triangles.emplace_back(0);

  for(int a=0; a<36; a++) if(triangles[a] >= 36) triangles[triangles[a]] = a;
  
  dualize(triangles);
  
  for(int a=0; a<48; a++) {
    if(a % 12 == 0) printf("\n");
    printf("%3d, ", triangles[a]);
    }
  
  printf("\n");
  
  triangles.clear();

  for(int a=0; a<12; a++) {
    triangles.emplace_back(3 * ((a+1) % 12) + 1);
    triangles.emplace_back(3 * ((a+11) % 12));
    triangles.emplace_back(36 + 3 * a);
    }

  for(int a=0; a<12; a++) {
    triangles.emplace_back(3 * a + 2);
    triangles.emplace_back(36 + (3 * a + 36 - 15 + 2) % 36);
    triangles.emplace_back(36 + (3 * a + 36 + 15 + 1) % 36);
    }
  
  dualize(triangles);

  for(int a=0; a<12*2*3; a++) {
    if(a % 12 == 0) printf("\n");
    printf("%3d, ", triangles[a]);
    }
  
  printf("\n");
  
  return 0;
  }
