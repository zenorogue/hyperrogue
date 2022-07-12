// test the conjecture comparing triangulation distances and hyperbolic distances

namespace dhrg {

ld stats[32][3], wstats[32][3];
  
hyperpoint celltopoint(cell *c) {
  return tC0(calc_relative_matrix(c, croot(), C0));
  }

void do_analyze_grid(int maxv) {
  cell *root = croot();
  celllister cl(root, 32, maxv, NULL);
  // if this works too slow, use a smaller number
  // (you can also use a larger number if you have time of course)

  // int rot = 0;
  
  vector<ld> distances[128];
  
  for(cell *c: cl.lst) {
    hyperpoint h = celltopoint(c);
    ld dd = hdist0(h);
    int d = celldist(c);
    stats[d][0] ++;
    stats[d][1] += dd;
    stats[d][2] += dd*dd;
    distances[d].push_back(dd);

    if(d>0) {
      ld alpha[2];
      int qalpha = 0;
      forCellCM(c2, c) if(celldist(c2) == d) {
        hyperpoint h1 = celltopoint(c2);
        alpha[qalpha++] = atan2(h1[0], h1[1]);
        }
      if(qalpha != 2) printf("Error: qalpha = %d\n", qalpha);
      ld df = alpha[0] - alpha[1];
      if(df<0) df = -df;
      while(df > 2*M_PI) df -= 2*M_PI;
      while(df > M_PI) df = 2*M_PI - df;
      df /= 4*M_PI;
      wstats[d][0] += df;
      if(d==2) printf("df == %" PLDF " dd = %" PLDF "\n", df, dd);
      wstats[d][1] += df*dd;
      wstats[d][2] += df*dd*dd;
      }
    }
  
  println(hlog, "log(gamma) = ", log(cgi.expansion->get_growth()));
  
  ld lE, dif, lwE;
  for(int d=0; d<32; d++) if(stats[d][0]) {
    int q = stats[d][0];
    if(q != cgi.expansion->get_descendants(d).approx_int()) continue; 
    ld E = stats[d][1] / q;
    ld E2 = stats[d][2] / q;
    ld Vr = E2 - E * E;
    if(Vr < 0) Vr = 0;
    dif = E- lE; lE = E;
    ld Vd = d > 1 ? Vr/(d-1) : 0;
    
    ld wE = wstats[d][1];
    ld wE2 = wstats[d][2];
    ld wVr = wE2 - wE * wE;

    print(hlog, format("d=%2d: q = %8d E = %12.8" PLDF " dif = %12.8" PLDF " Vr = %12.8" PLDF " Vr/(d-1)=%12.8" PLDF,
      d, q, E, dif, Vr, Vd));

    if(0) print(hlog, format(" | <%" PLDF "> ex = %12.8" PLDF " d.ex = %12.8" PLDF " Vr = %12.8" PLDF, wstats[d][0], wE, wE - lwE, wVr));
    
    ld Sigma = sqrt(Vr);
    sort(distances[d].begin(), distances[d].end());
    if(Sigma) for(int u=1; u<8; u++)
      print(hlog, format(" %8.5" PLDF, (distances[d][u * isize(distances[d]) / 8] - E) / Sigma));
    
    println(hlog);
    lwE = wE;
    }
  }

}
