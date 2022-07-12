// check whether our distance and tallycounter algorithms work correctly

namespace dhrg {

void test_paths(int radius) {
  celllister cl(croot(), radius, 1000000, NULL);
  int N = isize(cl.lst);
  printf("N = %d\n", N);
  vector<mycell*> mycells;
  for(cell *c: cl.lst)
    mycells.push_back(find_mycell_by_path(computePath(c)));
  
  int ctable[MAXDIST];

  for(int u=0; u<MAXDIST; u++)
    ctable[u] = 0;
  
  int errorcount = 0;

  for(int i=0; i<N; i++) {
    cell *c = cl.lst[i];
    mycell *mc = mycells[i];
    cell *c1 = mc->ascell();
    if(c != c1) {
      printf("ascell error %s / %s\n", computePath(c).c_str(), computePath(c1).c_str());
      c->item = itEmerald;
      c1->item = itRuby;
      errorcount++;
      return;
      }
    }
  
  for(int i=0; i<N; i++)
    for(int j=0; j<N; j++) {
      int a = celldistance(cl.lst[i], cl.lst[j]);
      int b = quickdist(mycells[i], mycells[j]);
      if(a != b) {
        printf("celldistance = %d\n", a);
        printf("quickdist = %d\n", b);
        celllister cl2(cl.lst[i], 10, 1500, cl.lst[j]);
        if(cl2.listed(cl.lst[j]))
          printf("actual distance = %d\n", cl2.getdist(cl.lst[j]));
        cl.lst[i]->item = itDiamond;
        cl.lst[j]->item = itSilver;
        errorcount++;
        return;
        }
      else ctable[a]++;
      }
  
  for(mycell* mc1: mycells) for(mycell* mc2: mycells) {
    add_to_set(mc1, 1, 0);
    add_to_tally(mc2, 1, 0);
    // int v = 0;
    if(tally[quickdist(mc1, mc2)] != 1) {
      printf("[%p] [%p]\n", mc1, mc2);
      printf("quickdist = %d\n", quickdist(mc1, mc2));
      for(int i=0; i<MAXDIST; i++) if(tally[i]) printf("in tally = %d\n", i);
      mc1->ascell()->item = itDiamond;
      mc2->ascell()->item = itSilver;
      errorcount++;
      add_to_tally(mc2, -1, 0);
      add_to_set(mc1, -1, 0);
      return;
      }
    add_to_tally(mc2, -1, 0);
    add_to_set(mc1, -1, 0);
    }

  for(mycell* mc: mycells)
    add_to_set(mc, 1, 0);

  for(mycell* mc: mycells)
    add_to_tally(mc, 1, 0);

  for(int u=0; u<MAXDIST; u++) if(ctable[u] || tally[u]) {
    printf("%d: %d/%d\n", u, ctable[u], (int) tally[u]);
    if(ctable[u] != tally[u]) errorcount++;
    }
  
  for(mycell* mc: mycells)
    add_to_tally(mc, -1, 0);

  for(mycell* mc: mycells)
    add_to_set(mc, -1, 0);

  if(errorcount)
    printf("errors found: %d\n", errorcount);
  }

}
