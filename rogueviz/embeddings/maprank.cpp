// compute mAP/MeanRank rankings, for general continuous distances

namespace rogueviz {

namespace embeddings {

void continuous_ranks() {
  ld tot_ranks = 0, tot_rby = 0, tot_map = 0, tot_n = 0;

  int N = isize(rogueviz::vdata);

  if(1) {
    progressbar pb(N/threads, "continuous ranks");

    std::mutex lock; 
    parallelize(N, [&] (int a, int b) {
      ld ltot_ranks = 0, ltot_rby = 0, ltot_map = 0, ltot_n = 0;
      for(int i=a; i<b; i++) {
        int qty = isize(directed_edges[i]);
        if(!qty) continue;
        vector<pair<ld, int>> dists(N);
        // println(hlog, i, ": ", dists);
        for(int j=0; j<N; j++) dists[j] = { current->distance(i, j), 0};

        for(auto e: directed_edges[i]) dists[e].second = 1;

/*      for(auto& d: dists) if(d.second == 0) d.first += 0.5;
        vector<int> ba(MAXDIST, 0), bb(MAXDIST, 0);
        for(auto& d: dists) { if(d.second == 1) ba[d.first]++; bb[d.first]++; } */

/*    bb[0]--; print(hlog, i, ": "); for(int d=0; d<MAXDIST; d++) if(bb[d]) print(hlog, d, " : ", ba[d],"/",bb[d], " ");
    println(hlog); */

        // dists[i].first -= 2;
        sort(dists.begin(), dists.end());

        vector<int> ranks;
        int seen = 0;
        int bad = 0;
        ld for_map = 0;
        // note: i itself is in first position
        for(int j=0; seen<qty; j++)
          if(dists[j].second) {
            ltot_ranks += bad;
            ranks.push_back(bad+seen);
            // if(bad+seen+1 <= qty) for_map++; // 
            for_map += (seen+1.) / (bad+seen);
            // printf("%d/%d ", seen+1, bad+seen);
            seen++;
            }
          else
            bad++;
        if(a == 0) pb++;
        ltot_rby += qty;
        ltot_map += for_map / qty;
// println(hlog, "contribution: ", for_map / qty);
        ltot_n++;
        }
      std::unique_lock<std::mutex> lk(lock);
      tot_rby += ltot_rby;
      tot_map += ltot_map;
      tot_ranks += ltot_ranks;
      tot_n += ltot_n;
      return 0;
      });
    }
  println(hlog, "MeanRank = ", tot_ranks / tot_rby, " MAP = ", tot_map / tot_n);
  println(hlog, "data: ", tie(tot_rby, tot_map, tot_ranks, tot_n, N));
  }


}}
