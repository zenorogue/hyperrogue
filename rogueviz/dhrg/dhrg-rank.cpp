namespace dhrg {

ll rtally[MAXDIST];
int redgetally[MAXDIST];

using rogueviz::embeddings::directed_edges;


void tallydiredgesof(int i, int delta, mycell *mc) {
  for(auto j: directed_edges[i]) {
    whichedgetally[quickdist(mc, vertices[j], 0)] += delta;
    }
  }

void dhrg_ranks() {

  int N = isize(rogueviz::vdata);
  /*
  print(hlog, "check tally = ");
  for(int j=0; j<MAXDIST; j++)
    if(edgetally[j] || tally[j])
      print(hlog, j, ":", int(edgetally[j]), "/", int(tally[j]));
  println(hlog);
  */

  dynamicval<ll*> dt(whichtally, rtally);
  dynamicval<int*> det(whichedgetally, redgetally);

  ld meanrank = 0;
  ld meanrank_opti = 0;
  int tgood = 0;
  rtally[0]--; // do not count self
  ld maprank = 0;
  ld maprank_opti = 0;
  
  progressbar pb(N, "compute_ranks");
  for(int i=0; i<N; i++) {
    mycell *mc = vertices[i];
    tallydiredgesof(i, 1, mc);
    add_to_tally(mc, 1, 0);

    if(i < -10) {
      print(hlog, "tallies for i=", i, " =");
      for(int j=0; j<MAXDIST; j++)
        if(redgetally[j] || rtally[j])
          print(hlog, " ", j, ":", int(redgetally[j]), "/", int(rtally[j]));
      println(hlog);
      }

    // print(hlog, "LINE:");

    int pgood = 0;
    ld bad = 0;
    ld ap = 0;
    ld ap_opti = 0;
    ld pall = 0;
    // ld lastbad = 0;

    for(int j=0; j<MAXDIST; j++) {
      ld good = redgetally[j];
      ld all = rtally[j];
      ld err = all - good;

      /* for(int k=0; k<good; k++) {
        lastbad = bad;
        print(hlog, " ", pgood+bad+k+1);
        } */

      meanrank_opti += bad * good;
      bad += err / 2.;
      meanrank += bad * good;
      bad += err / 2.;

      for(int k=1; k<=good; k++) {
        ap_opti += (pgood+k) / (pall+k);
        }

      for(int k=0; k<good; k++) {
        pgood++, pall++;
        pall += err/2 / good;
        ap += pgood / pall;
        pall += err/2 / good;
        }
      if(!good) pall += err;

      }

    // if(lastbad > 30) print(hlog, " ** BAD");
    tgood += pgood;

    // print(hlog, " -> ", meanrank/(tgood+pgood), " (pgood ", pgood, ") (tgood ", tgood, ")");
    // println(hlog);

    if(pgood) maprank += ap / pgood;
    if(pgood) maprank_opti += ap_opti / pgood;
    tallydiredgesof(i, -1, mc);
    add_to_tally(mc, -1, 0);
    
    if(i < -10) {
      println(hlog, meanrank, "/", tgood, " ", ap, "/", pgood, " ", ap_opti, "/", pgood);
      }
    pb++;
    }

  rtally[0]++;
  print(hlog, "tallies for nothing =");
  for(int j=0; j<MAXDIST; j++)
    if(redgetally[j] || rtally[j])
      print(hlog, " ", j, ":", int(redgetally[j]), "/", int(rtally[j]));
  println(hlog);

  int tot_n = 0;
  for(int i=0; i<N; i++)
    if(isize(directed_edges[i]))
      tot_n++;

  println(hlog, "meanrank = ", meanrank / tgood, " MAP = ", maprank / tot_n, " meanrank_opti = ", meanrank_opti / tgood, " MAP_opti = ", maprank_opti / tot_n);
  }

}