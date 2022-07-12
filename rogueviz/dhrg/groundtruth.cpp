
//	$(VARIANT) -nogui -dhrg embedded-graphs/facebook_combined_result -contll -iterate 99 -contll -esave > $@

namespace dhrg {

bool is(char *where, const char *what) {
  while(*where && *what && *where == *what) where++, what++;
  return !*what;
  }

void ground_truth_test(string s) {

  logistic cont;

  vector<ld> logliks;
  vector<string> reps;

  auto report = [&] (string s, ld val) {
    logliks.push_back(val);
    reps.push_back(s);
    println(hlog, "REPORT ", s, " = ", val);
    };

  if(1) {
    FILE *f = fopen(("embout/" + s).c_str(), "rb");
    char buf[999999];
    int siz = fread(buf, 1, 999999, f);
    
    for(int i=0; i<siz; i++) if(is(buf+i, "Embedded Log-likelihood: "))
      report("bfkl", atof(buf+i+25));

    fclose(f);
    }
if(1) {
  dhrg_init(); read_graph_full("data/simg-" + s);


  origcoords();
  build_disttable();

  cont.setRT(graph_R, graph_T);

  report("gz", loglik_cont(cont));
  fix_logistic_parameters(cont, loglik_cont, "lcont", 1e-3);
  report("grc", loglik_cont(cont));

  cellcoords();
  build_disttable();

  report("gco", loglikopt());
  report("gcm", loglikopt_mono());
  report("gcrt", loglik_logistic());

  cont.setRT(graph_R, graph_T);
  report("gcz", loglik_cont(cont));
  fix_logistic_parameters(cont, loglik_cont, "lcont", 1e-3);
  report("gcrc", loglik_cont(cont));

  embedder_loop(20);

  report("geo", loglikopt());
  report("gem", loglikopt_mono());
  report("gert", loglik_logistic());

  cellcoords();
  build_disttable();

  cont.setRT(graph_R, graph_T);
  fix_logistic_parameters(cont, loglik_cont, "lcont", 1e-3);  
  report("gerc", loglik_cont(cont));

  delete mroot;
  mroot = NULL;
  segmentcount = 0;
  for(int i=0; i<MAXDIST; i++) tally[i] = 0;
  for(int i=0; i<MAXDIST; i++) edgetally[i] = 0;
  vertices.clear();
  rogueviz::close();
}

  dhrg_init(); read_graph_full("data/sime-" + s);
  origcoords();
  build_disttable();
  cont.setRT(graph_R, graph_T);
  report("ez", loglik_cont(cont));
  fix_logistic_parameters(cont, loglik_cont, "lcont", 1e-3);
  report("erc", loglik_cont(cont));

  report("eco", loglikopt());
  report("ecm", loglikopt_mono());
  report("ecrt", loglik_logistic());

  cellcoords();
  build_disttable();

  cont.setRT(graph_R, graph_T);
  report("ecz", loglik_cont(cont));
  fix_logistic_parameters(cont, loglik_cont, "lcont", 1e-3);
  report("ecrc", loglik_cont(cont));

  long long a = SDL_GetTicks();
  embedder_loop(20);
  long long v = SDL_GetTicks();
  long long tim = v - a;

  report("eeo", loglikopt());
  report("eem", loglikopt_mono());
  report("eert", loglik_logistic());

  cellcoords();
  build_disttable();

  cont.setRT(graph_R, graph_T);
  fix_logistic_parameters(cont, loglik_cont, "lcont", 1e-3);  
  report("eerc", loglik_cont(cont));

  print(hlog, "HDR;", separated(";", reps), ";TIME;N;GROWTH\n");
  print(hlog, "RES'", separated(";", logliks), ";", int(tim), ";", N, ";", cgi.expansion->get_growth());
  }

}
