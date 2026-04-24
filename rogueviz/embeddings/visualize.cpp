namespace dhrg {
  void show_likelihood();
  void launch_dhrg();
  extern std::vector<struct mycell*> vertices;
  }

namespace rogueviz {

namespace embeddings {

void show_embedding_info() {
  cmode = sm::SIDE | sm::DIALOG_STRICT_X | sm::PANNING;
  gamescreen();

  dialog::init("embedding info");
  auto vp = [] (string s, char k, string t) { dialog::addSelItem(s,t,k); };

  if(!eval.current) {
    vp("evaluate", 'e', "...");
    dialog::add_action(full_evaluation);
    }
  else {
    // println(hlog, make_tuple(int(eval.li.N), int(eval.li.M), int(eval.li.N1), eval.li.loglik, eval.li.control));
    vp("nodes", 'n', its(eval.li.N));
    vp("edges", 'm', its(eval.li.M));
    vp("embedding radius", 'r', its(eval.maxradius));
    dialog::addBoolItem("symmetric", eval.li.symmetric, 's');
    auto NX = bestll2(eval.li.M, eval.li.N1);
    vp("normalized loglikelihood", 'x', fts(1 - eval.li.loglik / NX));
    vp("icv", 'x', fts(-NX / (-NX + eval.li.control)));

    vp("mAP", 'M', fts(eval.rank.map / eval.rank.n));
    vp("MeanRank", 'R', fts(eval.rank.ranks / eval.rank.rby));

    auto &r = eval.routing;

    vp("greedy success", 'A', fts(r.suc / r.tot));
    vp("greedy stretch", 'A', fts(r.routedist / r.suc));
    vp("greedy efficiency", 'A', fts(r.eff / r.tot));

    vp("modded success", 'A', fts(r.msuc / r.tot));
    vp("modded stretch", 'A', fts(r.mroutedist / r.msuc));
    vp("modded efficiency", 'A', fts(r.meff / r.tot));
    }

  if(WDIM == 2) {
    if(!isize(dhrg::vertices)) {  
      dialog::addItem("DHRG: convert", 'd');
      dialog::add_action(dhrg::launch_dhrg);
      }
    else { 
      dialog::addItem("DHRG: examine", 'd');
      dialog::add_action_push(dhrg::show_likelihood);
      }
    }

  dialog::addBack();
  dialog::display();

  keyhandler = [] (int sym, int uni) {

    handlePanning(sym, uni);
    dialog::handleNavigation(sym, uni);    
    if(doexiton(sym, uni)) popScreen();
    };
  }

int a1 = addHook(rogueviz::hooks_rvmenu, 100, [] {
    if(current) {
      dialog::addItem("evaluate embedding", 'e');
      dialog::add_action_push(show_embedding_info);
      }
    });
}}