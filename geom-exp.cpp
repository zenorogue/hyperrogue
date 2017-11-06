// -- geometry menu --

int eupage = 0;
int euperpage = 21;

string euchelp =
  "If you want to know how much the gameplay is affected by the "
  "hyperbolic geometry in HyperRogue, this mode is for you!\n\n"
  
  "You can try many different geometries here. We start by gluing "
  "n-gons in such a way that k of them meet in every vertex. "
  "Depending on n and k, this either folds into a sphere, unfolds into a plane, "
  "or requires a hyperbolic space. The result may be then 'truncated' by "
  "replacing each vertex by a 2k-gon. Furthermore, you can play "
  "with quotient geometries. For example, the elliptic geometry is "
  "obtained from the sphere by making the antipodes be the same point, "
  "so you return to the same spot (but as a mirror image) after going there. "
  "Have fun experimenting! "
  "Achievements and leaderboards do not work in geometry experiments, "
  "except some specific ones.\n\n"
  "In standard geometry (truncated or not), you can play the full game, but in other geometries "
  "you select a particular land. Lands are unlocked by visiting them in this "
  "session, or permanently by collecting 25 treasure. Try Crossroads in Euclidean "
  "or chaos mode in non-standard non-quotient hyperbolic to visit many lands. "
  "Highlights:\n"
  "* Crystal World and Warped Coast can be understood as extra geometries.\n"
  "* Halloween is specially designed for spherical geometry.\n"
  "* To see the difference, try Hunting Grounds in Euclidean -- it is impossible.\n";

int ewhichscreen = 2;

void showQuotientConfig() {}

void showTorusConfig() {
  cmode = sm::SIDE | sm::TORUSCONFIG;
  gamescreen(2);
  
  dialog::init(XLAT("advanced concfiguration"));
  
  dialog::addSelItem(XLAT("number of cells (n)"), its(torusconfig::newqty), 'n');
  dialog::addSelItem(XLAT("cell bottom-right from 0 (d)"), its(torusconfig::newdy), 'd');
  
  if(torusconfig::newqty % 3)
    dialog::addInfo("best if n is divisible by 3", 0x808080);

  if((torusconfig::newdy + 999999) % 3 != 2)
    dialog::addInfo("best if d+1 is divisible by 3", 0x808080);
  
  dialog::addSelItem(XLAT("scale factor"), fts(vid.scale), 'z');

#if CAP_RUG
  dialog::addBoolItem(XLAT("hypersian rug mode"), (rug::rugged), 'u');
#endif

  dialog::addItem("activate", 'a');
  dialog::addItem("default", 'c');

  keyhandler = [] (int sym, int uni) {
    if(uni == 'n')
      dialog::editNumber(torusconfig::newqty, 0, 1000, 3, torusconfig::def_qty, XLAT("number of cells (n)"), "");
    else if(uni == 'd')
      dialog::editNumber(torusconfig::newdy, -1000, 1000, 3, -torusconfig::def_dy, XLAT("cell bottom-right from 0 (d)"), "");
    else if((uni == 'a' || uni == '\n') && torusconfig::newqty >= 3 && abs(torusconfig::newdy) < torusconfig::newqty ) {
      targetgeometry = gEuclid; restartGame('g');
      torusconfig::qty = torusconfig::newqty;
      torusconfig::dy = torusconfig::newdy;
      targetgeometry = gTorus; restartGame('g');
      }
    else if(uni == 'c') {
      targetgeometry = gEuclid; restartGame('g');
      torusconfig::qty = torusconfig::def_qty;
      torusconfig::dy = torusconfig::def_dy;
      targetgeometry = gTorus; restartGame('g');
      }
    else if(uni == 'z') editScale();
    else if(uni == 'u') rug::select();
    else if(doexiton(sym, uni))
      popScreen();
    };
  
  dialog::display();
  }

void showEuclideanMenu() {
  cmode = sm::SIDE;
  gamescreen(0);  
  if(cheater) for(int i=0; i<landtypes; i++) landvisited[i] = true;
  for(int i=0; i<landtypes; i++)
    if(hiitemsMax(treasureType(eLand(i))) >= 25) landvisited[i] = true;
  landvisited[laCrossroads] = true;
  landvisited[laIce] = true;
  landvisited[laHunting] = true;
  landvisited[laMirrorOld] = true;
  landvisited[laPrincessQuest] = cheater || princess::everSaved;
  landvisited[laWildWest] = true;
  landvisited[laHalloween] = true;
  landvisited[laWarpCoast] = true;
  landvisited[laGraveyard] = true;
  landvisited[laDual] = true;
  landvisited[laCA] = true;
  // for(int i=2; i<lt; i++) landvisited[i] = true;
  
  if(geometry == gNormal || ewhichscreen == 2) {
    dialog::init(XLAT("experiment with geometry"));
    int ts = ginf[geometry].sides;
    int tv = ginf[geometry].vertex;
    int tq = ginf[geometry].quotientstyle;
    int nom = (nontruncated ? tv : tv+ts) * ((tq & qELLIP) ? 2 : 4);
    int denom = (2*ts + 2*tv - ts * tv);
    
    dialog::addSelItem(XLAT("land"), XLAT1(linf[specialland].name), '5');
    dialog::addBreak(50);

    for(int i=0; i<gGUARD; i++)
      dialog::addBoolItem(XLAT(ginf[i].name), geometry == i, 'a'+i);
    
    dialog::addBreak(50);
  
    if(ts == 6 && tv == 3)
      dialog::addSelItem(XLAT("truncated"), XLAT("does not matter"), 't');
    else
      dialog::addBoolItem(XLAT("truncated"), !nontruncated, 't');
  
    dialog::addBreak(50);
  
    int worldsize = denom ? nom/denom : 0;
    if(tq & qTORUS) worldsize = torusconfig::qty;
    if(tq & qZEBRA) worldsize = nontruncated ? 12 : 40;
    if(tq & qFIELD) {
      worldsize = size(currfp.matrices) / ts;
      if(!nontruncated) worldsize = ((ts+tv)*worldsize) / tv;
      }
  
    dialog::addSelItem(XLAT("sides per face"), its(ts), 0);
    dialog::addSelItem(XLAT("faces per vertex"), its(tv), 0);
  
    string qstring = "none";
    if(tq & qZEBRA) qstring = "zebra";
  
    else if(tq & qFIELD) qstring = "field";
  
    else if(tq & qELLIP) qstring = "torus";
  
    else if(tq & qTORUS) qstring = "torus";
  
    dialog::addSelItem(XLAT("quotient space"), XLAT(qstring), 0);
  
    dialog::addSelItem(XLAT("size of the world"), 
      worldsize < 0 ? "exp(∞)*" + (nom%denom ? its(nom)+"/"+its(-denom) : its(-worldsize)): 
      worldsize == 0 ? "∞" :
      its(worldsize),
      '3');
    
    switch(ginf[geometry].cclass) {
      case 0:
        dialog::addSelItem(XLAT("Curvature"), XLAT("hyperbolic"), 0);
        break;
      
      case 1: 
        dialog::addSelItem(XLAT("Curvature"), XLAT("flat"), 0);
        break;
      
      case 2:
        dialog::addSelItem(XLAT("Curvature"), XLAT("spherical"), 0);
        break;
      }
    
    if(sphere) 
      dialog::addBoolItem(XLAT("stereographic/orthogonal"), vid.alpha>10, '1');
    else
      dialog::addBoolItem(XLAT("Poincaré/Klein"), vid.alpha>.5, '1');
    if(torus || quotient == 2)
      dialog::addItem(XLAT("advanced parameters"), '4');  
    dialog::addItem(XLAT("help"), SDLK_F1);  
    dialog::addItem(XLAT("back"), '0');  
    dialog::display();
  
    keyhandler = [] (int sym, int uni) {
      dialog::handleNavigation(sym, uni);
      if(uni >= 'a' && uni < 'a'+gGUARD) {
        targetgeometry = eGeometry(uni - 'a');
        restartGame(geometry == targetgeometry ? 0 : 'g');
        pushScreen(showEuclideanMenu);
        }
      else if(uni == 't') {
        if(!euclid) {
          restartGame('7');
          pushScreen(showEuclideanMenu);
          }
        }
      else if(uni == '2' || sym == SDLK_F1) gotoHelp(euchelp);
      else if(uni == '3') { viewdists = !viewdists; if(viewdists) popScreenAll(); }
      else if(uni == '1' && !euclid) {
        if(sphere) {
          if(vid.alpha < 10) { vid.alpha = 999; vid.scale = 998; }
          else {vid.alpha = 1; vid.scale = .4; }
          }
        else {
          if(vid.alpha > .5) { vid.alpha = 0; vid.scale = 1; }
          else {vid.alpha = 1; vid.scale = 1; }
          }
        }
      else if(uni == '5')
        ewhichscreen ^= 3;
      else if(uni == '4') {
        if(torus) 
          torusconfig::newdy = torusconfig::dy,
          torusconfig::newqty = torusconfig::qty,
          pushScreen(showTorusConfig);
        if(quotient==2) pushScreen(showQuotientConfig);
        }
      else if(doexiton(sym, uni))
        popScreen();
      };
    }
  else {
    dialog::init(XLAT("experiment with geometry"));
    string truncatenames[2] = {" (t)", " (n)"};
  
    dialog::addSelItem(XLAT("geometry"), XLAT(ginf[geometry].name) + XLAT(truncatenames[nontruncated]), '5');
    dialog::addBreak(50);
    
    generateLandList(isLandValid);
    
    for(int i=0; i<euperpage; i++) {
      if(euperpage * eupage + i >= size(landlist)) { dialog::addBreak(100); break; }
      eLand l = landlist[euperpage * eupage + i];
      char ch;
      if(i < 26) ch = 'a' + i;
      else ch = 'A' + (i-26);
      string validclasses[4] = {"", " (½)", "", " (!)"};
      string s = XLAT1(linf[l].name);

      if(landvisited[l]) {
        dialog::addBoolItem(s, l == specialland, ch);
        }
      else {
        dialog::addSelItem(s, XLAT("(locked)"), ch);
        }
      
      dialog::lastItem().color = linf[l].color;
      dialog::lastItem().value += validclasses[isLandValid(l)];
      }
    dialog::addBreak(50);
    if(chaosUnlocked && !quotient && !euclid && !sphere)
      dialog::addItem(XLAT("Chaos mode"), '1');
    dialog::addItem(XLAT("next page"), '-');
    
    dialog::addItem(XLAT("help"), SDLK_F1);  
    dialog::addItem(XLAT("back"), '0');
    dialog::display();
  
    keyhandler = [] (int sym, int uni) {
      dialog::handleNavigation(sym, uni);
      int lid;
      if(uni >= 'a' && uni <= 'z') lid = uni - 'a';
      else if(uni >= 'A' && uni <= 'Z') lid = 26 + uni - 'A';
      else lid = -1;
      
      if(lid >= 0) lid += euperpage * eupage;
      
      if(uni == '5') 
        ewhichscreen ^= 3;
      else if(uni == '-' || uni == PSEUDOKEY_WHEELUP || uni == PSEUDOKEY_WHEELDOWN) {
        eupage++;
        if(eupage * euperpage >= size(landlist)) eupage = 0;
        }
      else if(uni == '1') {
        if(chaosUnlocked) {
          restartGame('C');
          pushScreen(showEuclideanMenu);
          }
        }
      else if(lid >= 0 && lid < size(landlist)) {
        eLand nland = landlist[lid];
        if(landvisited[nland]) {
          specialland = nland;
          restartGame(tactic::on ? 't' : 0);
          pushScreen(showEuclideanMenu);
          }
        }
      else if(uni == '2' || sym == SDLK_F1) gotoHelp(euchelp);
      else if(doexiton(sym, uni)) popScreen();
      };
    }
  
  }

void runGeometryExperiments() {
  specialland = getLandForList(cwt.c);
  pushScreen(showEuclideanMenu);
  }

