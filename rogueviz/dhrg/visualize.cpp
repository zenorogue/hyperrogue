// a RogueViz dialog to manipulate DHRG embeddings

int held_id = -1;

void show_likelihood() {
  cmode = sm::SIDE | sm::MAYDARK | sm::DIALOG_STRICT_X | sm::PANNING;
  gamescreen();

  dialog::init("DHRG information");
  
  ll bonus_tally[MAXDIST], bonus_edgetally[MAXDIST];

  if(held_id >= 0) {
    for(int i=0; i<MAXDIST; i++)
      bonus_tally[i] = tally[i],
      bonus_edgetally[i] = edgetally[i];
    auto& mc = vertices[held_id];
    add_to_set(mc, -1, 0);
    add_to_tally(mc, -1, 0);
    tallyedgesof(held_id, -1, mc);
    for(int i=0; i<MAXDIST; i++)
      bonus_tally[i] -= tally[i],
      bonus_edgetally[i] -= edgetally[i];
    tallyedgesof(held_id, +1, mc);
    add_to_tally(mc, +1, 0);    
    add_to_set(mc, +1, 0);
    }
  else 
    for(int i=0; i<MAXDIST; i++) bonus_tally[i] = 0;

  for(int u=0; u<MAXDIST; u++) if(tally[u] || bonus_tally[u]) {
    char buf[20];
    sprintf(buf, "%.6lf", lc_type == 'R' ? current_logistic.yes(u) : double(edgetally[u] * 1. / tally[u]));    
    string s = its(u);
    if(isize(s) == 1) s = "0" + s;
    s += ": " + its(edgetally[u]) + " / " + its(tally[u]);
    if(bonus_tally[u]) s += " [" + its(bonus_edgetally[u]) + "/" + its(bonus_tally[u]) + "]";
    dialog::addSelItem(s, buf, 0);
    }
  
  char letters[3] = {'O', 'R', 'M'};
  string lltypes[3] = {"opt", "logistic", "mono."};
  string clltype = "?";
  char nextletter;
  for(int i=0; i<3; i++) if(lc_type == letters[i])
    clltype = lltypes[i], nextletter = letters[(i+1)%3];
  
  getcstat = '-';

  dialog::addBreak(50);
  dialog::addSelItem("loglikelihood (" + clltype + ")", fts(loglik_chosen()), 'l');
  dialog::add_action([nextletter] () { lc_type = nextletter; });
  
  dialog::addSelItem("iterations of local search", its(iterations), 'i');
  dialog::add_action([] () { 
    embedder_loop(1); 
    ts_vertices = ts_rbase;
    place_rogueviz_vertices();
    if(!stored) rogueviz::storeall(), stored = true;
    else shmup::fixStorage();
    });
  
  dialog::addBack();
  dialog::display();

  if(held_id >= 0) {
    if(!mousepressed) held_id = -1;
    else if(mouseover) {    
      rogueviz::vdata[held_id].m->base = mouseover;
      shmup::fixStorage();
      dhrg::fixedges(); 

      auto& mc = vertices[held_id];
      tallyedgesof(held_id, -1, mc);
      add_to_set(mc, -1, 0);
      add_to_tally(mc, -1, 0);

      mc = find_mycell_by_path(computePath(rogueviz::vdata[held_id].m->base));

      tallyedgesof(held_id, 1, mc);
      add_to_tally(mc, 1, 0);
      add_to_set(mc, 1, 0);
      }
    }
  
  keyhandler = [] (int sym, int uni) {

    handlePanning(sym, uni);
    if(uni == '-' && held_id == -1) {
      for(int i=0; i<N; i++) if(rogueviz::vdata[i].m->base == mouseover)
        held_id = i;
      return;
      }

    dialog::handleNavigation(sym, uni);    
    if(doexiton(sym, uni)) popScreen();
    };
  }

bool dhrg_animate(int sym, int uni) {
  if((cmode & sm::NORMAL) && uni == '/') {
    clearMessages();

    if(ts_rbase > ts_vertices) {
      dhrg_init(); graph_from_rv(); 
      ts_vertices = ts_rbase;
      place_rogueviz_vertices();
      if(!stored) rogueviz::storeall(), stored = true;
      else shmup::fixStorage();
      }    
    pushScreen(show_likelihood);

    return true;
    }
  return false;
  }
