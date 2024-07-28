#ifndef _STATISTICS_CPP_
#define _STATISTICS_CPP_

#include <vector>
#include <cstdio>
#include <cmath>
#include <array>

namespace stats {

using namespace std;

typedef double val;

struct inverse_error {};

template<size_t N> array<array<val, N>, N> invert(const array<array<val, N>, N>& T) {
  int iN = N;
  auto T1 = T, T2 = T;
  for(int y=0; y<iN; y++)
  for(int x=0; x<iN; x++)
    T2[y][x] = (x==y);

  for(int a=0; a<iN; a++) {
    int best = a;
    
    for(int b=a+1; b<iN; b++)
      if(abs(T1[b][a]) > abs(T1[best][a]))
        best = b;

    int b = best;

    if(b != a)
      for(int c=0; c<iN; c++) 
        swap(T1[b][c], T1[a][c]), swap(T2[b][c], T2[a][c]);

    if(abs(T1[a][a]) < 1e-6) throw inverse_error();
    for(int b=a+1; b<iN; b++) {
      val co = -T1[b][a] / T1[a][a];
      for(int c=0; c<iN; c++) T1[b][c] += T1[a][c] * co, T2[b][c] += T2[a][c] * co;
      }
    }
  
  for(int a=N-1; a>=0; a--) {
    for(int b=0; b<a; b++) {
      val co = -T1[b][a] / T1[a][a];
      for(int c=0; c<iN; c++) T1[b][c] += T1[a][c] * co, T2[b][c] += T2[a][c] * co;
      }
    val co = 1 / T1[a][a];
    for(int c=0; c<iN; c++) T1[a][c] *= co, T2[a][c] *= co;
    }
  return T2;
  }

template<size_t N> struct leastsquare_solution : public array<val, N> {
  val operator() (const array<val, N> X) {
    int iN = N;
    val res = 0;
    for(int j=0; j<iN; j++) res += X[j] * (*this)[j];
    return res;
    }
  };

template<size_t N> struct leastsquare_solver {
  array<array<val, N>, N> toinvert;
  array<val, N> Xty;
  static constexpr int iN = N;

  leastsquare_solver() {
    for(int y=0; y<iN; y++) Xty[y] = 0;
    for(int y=0; y<iN; y++)
    for(int x=0; x<iN; x++)
      toinvert[y][x] = 0;
    }
  
  void add_data(const array<val, N> X, val y) {
    for(int j=0; j<iN; j++)
    for(int k=0; k<iN; k++)
      toinvert[j][k] += X[j] * X[k];
    for(int j=0; j<iN; j++)
      Xty[j] += X[j] * y;
    }
  
  void operator += (const leastsquare_solver<iN> other) {
    for(int j=0; j<iN; j++)
    for(int k=0; k<iN; k++)
      toinvert[j][k] += other.toinvert[j][k];
    for(int j=0; j<iN; j++)
      Xty[j] += other.Xty[j];
    }
  
  leastsquare_solution<iN> solve() {
    auto res = invert(toinvert);
    leastsquare_solution<iN> s;
    for(int i=0; i<iN; i++) {
      s[i] = 0;
      for(int j=0; j<iN; j++)
        s[i] += res[i][j] * Xty[j];
      }
    return s;
    }
  };

template<int dim1, int dim2> double small_kendall(const vector<pair<int, int>>& allp) {
  int maxo = 0, maxe = 0;
  for(const auto& a: allp) maxo = max(maxo, a.first), maxe = max(maxe, a.second);
  maxo++; maxe++;

  if(maxo >= dim1 || maxe >= dim2)
    throw hr::hr_exception("small_kendall limit exceeded");
  int cnt[dim1][dim2];
  for(int a=0; a<maxo; a++)
  for(int b=0; b<maxe; b++)
    cnt[a][b] = 0;
  for(const auto& a: allp) cnt[a.first][a.second]++;

  // int i1 = 0, i2 = 0;
  int K = hr::isize(allp);
  // allp.emplace_back(maxo, maxe);
  vector<int> counts(maxe, 0);
  vector<int> totals(maxe);
  double tau = 0;
  for(int i=0; i<maxo; i++) {
    totals[0] = 0;
    for(int ii=1; ii<maxe; ii++)
      totals[0] -= counts[ii];
    for(int ii=1; ii<maxe; ii++)
      totals[ii] = totals[ii-1] + counts[ii] + counts[ii-1];

    for(int b=0; b<maxe; b++) {
      tau += totals[b] * 1. * cnt[i][b];
      counts[b] += cnt[i][b];
      }
    }
  double par = (K * (K-1.) / 2);
  return tau / par;
  }


}
#endif
