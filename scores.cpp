#ifndef NOSAVE

namespace scores {

vector<score> scores;
score *currentgame;

int scoresort = 2;
int scoredisplay = 1;
int scorefrom = 0;
int scoremode = 0;
bool scorerev = false;

bool scorecompare(const score& s1, const score &s2) {
  return s1.box[scoresort] > s2.box[scoresort];
  }

bool fakescore() {
  return fakebox[scoredisplay];
  }

int colwidth() {
  if(scoredisplay == 0) return 5;
  if(scoredisplay == 1) return 16;
  if(scoredisplay == 5) return 8;
  return 4;
  }

string displayfor(score* S, bool shorten = false) {
  // printf("S=%p, scoredisplay = %d\n", S, scoredisplay);
  if(S == NULL) {
    string str = XLATN(boxname[scoredisplay]);
    if(!shorten) return str;
    if(scoredisplay == 0 || scoredisplay == 65) return XLAT("time");
    if(scoredisplay == 2) return "$$$";
    if(scoredisplay == 3) return XLAT("kills");
    if(scoredisplay == 4) return XLAT("turns");
    if(scoredisplay == 5) return XLAT("cells");
    if(scoredisplay == 67) return XLAT("cheats");
    if(scoredisplay == 66) return XLAT("saves");
    if(scoredisplay == 197) return XLAT("players");
    int i = 0;
    for(int j=0; j<5; j++) if(i < size(str)) getnext(str.c_str(), i);
    return str.substr(0, i);
    }
  if(scoredisplay == 0 || scoredisplay == 65) {
    char buf[20];
    int t = S->box[0];
    if(t >= 3600) 
      snprintf(buf, 20, "%d:%02d:%02d", t/3600, (t/60)%60, t%60);
    else 
      snprintf(buf, 20, "%d:%02d", t/60, t%60);
    return buf;
    }
  if(scoredisplay == 1) {
    time_t tim = S->box[1];
    char buf[128]; strftime(buf, 128, "%c", localtime(&tim));
    return buf;
    }
  return its(S->box[scoredisplay]);
  }

vector<pair<string, int> > pickscore_options;

void sortScores() {
  if(scorerev) reverse(scores.begin(), scores.end());
  else {
    scorerev = true;
    scoresort = scoredisplay; 
    stable_sort(scores.begin(), scores.end(), scorecompare);
    }
  }

void shiftScoreDisplay(int delta) {
  scoredisplay = (scoredisplay + POSSCORE + delta) % POSSCORE, scorerev = false;
  if(fakescore()) shiftScoreDisplay(delta);
  }

int curcol;

vector<int> columns;

bool monsterpage = false;
         
void showPickScores() {

  int d = scoredisplay = columns[curcol];
  
  pickscore_options.clear();

  scorerev = false;

  for(int i=0; i<POSSCORE; i++) {
    scoredisplay = i;
    if(!fakescore()) {
      string s = displayfor(NULL);
      if(mapeditor::hasInfix(s))
        if(monsbox[scoredisplay] == monsterpage)
          pickscore_options.push_back(make_pair(s, i));
      }
    }
  sort(pickscore_options.begin(), pickscore_options.end());
  
  int q = (int) pickscore_options.size();
  int percolumn = vid.yres / (vid.fsize+3) - 4;
  int qcolumns = 1 + (q-1) / percolumn;
  
  for(int i=0; i<q; i++) {
    int x = 16 + (vid.xres * (i/percolumn)) / qcolumns;
    int y = (vid.fsize+3) * (i % percolumn) + vid.fsize*2;
    
    scoredisplay = pickscore_options[i].second;
    if(q <= 9)
      pickscore_options[i].first = pickscore_options[i].first + " [" + its(i+1) + "]";
    if(!fakescore())
      displayButton(x, y, pickscore_options[i].first, 1000+i, 0);
    }
  
  displayButton(vid.xres/2, vid.yres - vid.fsize*2, "kills", 'm', 8);

  scoredisplay = d;

  mouseovers = mapeditor::infix;
  keyhandler = [] (int sym, int uni) {
    if(uni == 'm') monsterpage = !monsterpage; else
    if(uni >= '1' && uni <= '9') uni = uni + 1000 - '1';
    else if(uni >= 1000 && uni < 1000 + size(pickscore_options)) {
      scoredisplay = pickscore_options[uni - 1000].second;
      for(int i=0; i<POSSCORE; i++)
        if(columns[i] == scoredisplay) swap(columns[i], columns[curcol]);
      popScreen();
      }
    else if(mapeditor::editInfix(uni)) ;
    else if(doexiton(sym, uni)) popScreen();
    };
  }

void show() {

  if(columns.size() == 0) for(int i=0; i<POSSCORE; i++) columns.push_back(i);
  int y = vid.fsize * 5/2;
  int bx = vid.fsize;
  getcstat = 0;
  
  displaystr(bx*4, vid.fsize, 0, vid.fsize, "#", forecolor, 16);
  displaystr(bx*8, vid.fsize, 0, vid.fsize, XLAT("ver"), forecolor, 16);
  
  int at = 9;
  for(int i=0; i<POSSCORE; i++) {
    scoredisplay = columns[i];
    if(bx*at > vid.xres) break;
    if(displaystr(bx*at, vid.fsize, 0, vid.fsize, displayfor(NULL, true), i == curcol ? 0xFFD500 : forecolor, 0))
      getcstat = 1000+i;
    at += colwidth();
    }
  
  if(scorefrom < 0) scorefrom = 0;
  int id = 0;
  int omit = scorefrom;
  int rank = 0;
  while(y < (ISMOBILE ? vid.yres - 5*vid.fsize : vid.yres - 2 * vid.fsize)) { 
    if(id >= size(scores)) break;
        
    score& S(scores[id]);
    
    bool wrongtype = false;
    
    wrongtype |= (euclid && (!S.box[116] || S.box[120] != euclidland));
    wrongtype |= (!euclid && S.box[116]);

    wrongtype |= (scoremode == 1 && !S.box[119]);
    wrongtype |= (scoremode != 1 && S.box[119]);
    wrongtype |= (scoremode == 2 && (!S.box[117] || S.box[118] >= PUREHARDCORE_LEVEL));
    
    if(wrongtype) { id++; continue; }

    if(omit) { omit--; rank++; id++; continue; }
    
    bool cur = S.box[MAXBOX-1];
    if(cur) {
      saveBox(); 
      for(int i=0; i<POSSCORE; i++) S.box[i] = savebox[i];
      S.box[0] = S.box[65];
      }
    int col = cur ? 0xFFD500 : 0xC0C0C0;
    

    rank++;
    displaystr(bx*4,  y, 0, vid.fsize, its(rank), col, 16);
    
    displaystr(bx*8,  y, 0, vid.fsize, S.ver, col, 16);

    int at = 9;
    for(int i=0; i<POSSCORE; i++) {
      scoredisplay = columns[i];
      if(bx*at > vid.xres) break;
      at += colwidth();
      if(displaystr(bx*(at-1), y, 0, vid.fsize, displayfor(&S), col, 16))
        getcstat = 1000+i;
      }

    y += vid.fsize*5/4; id++;
    }

  int i0 = vid.yres - vid.fsize;
  int xr = vid.xres / 80;

  string modes = 
    scoremode == 0 ? XLAT(", m - mode: normal") :
    scoremode == 1 ? XLAT(", m - mode: shoot'em up") :
    scoremode == 2 ? XLAT(", m - mode: hardcore only") :
    "?";

  if(euclid) modes += XLAT(" (E:%1)", euclidland);

  displayButton(xr*10, i0, IFM("s - ") + XLAT("sort"), 's', 8);
  displayButton(xr*25, i0, IFM("t - ") + XLAT("choose"), 't', 8);
  displayButton(xr*40, i0, IFM("0 - ") + XLAT("play"), '0', 8);
  displayButton(xr*65, i0, IFM("m - ") + modes.substr(6), 'm', 8);

  keyhandler = [] (int sym, int uni) {
    if(sym == SDLK_LEFT || sym == SDLK_KP4 || sym == 'h' || sym == 'a') {
      if(curcol > 0) curcol--;
      }
    else if(sym == SDLK_RIGHT || sym == SDLK_KP6 || sym == 'l' || sym == 'd') {
      if(curcol < POSSCORE-1) curcol++;
      }
    else if(sym >= 1000 && sym < 1000+POSSCORE)
      curcol = sym - 1000;
    else if(sym == 't') { mapeditor::infix = ""; pushScreen(showPickScores); }
    else if(sym == SDLK_UP || sym == 'k' || sym == 'w')
      scorefrom -= 5;
    else if(sym == SDLK_DOWN || sym == 'j' || sym == 'x')
      scorefrom += 5;
    else if(sym == PSEUDOKEY_WHEELUP)
      scorefrom--;
    else if(sym == PSEUDOKEY_WHEELDOWN)
      scorefrom++;
    else if(sym == 's') sortScores(); 
    else if(sym == 'm') { scoremode++; scoremode %= 3; }
    else if(doexiton(sym, uni)) popScreen();

    static int scoredragy;
    static bool lclicked;
    
    if(mousepressed) {
      if(!lclicked) {
        // scoredragx = mousex;
        scoredragy = mousey;
        }
  
      else {
        while(mousey > scoredragy + vid.fsize) scoredragy += vid.fsize, scorefrom--;
        while(mousey < scoredragy - vid.fsize) scoredragy -= vid.fsize, scorefrom++;
        }

      lclicked = mousepressed;
      }
    };
  }

void load() {
  scores.clear();
  FILE *f = fopen(scorefile, "rt");
  if(!f) {
    printf("Could not open the score file '%s'!\n", scorefile);
    addMessage(s0 + "Could not open the score file: " + scorefile);
    return;
    }
  while(!feof(f)) {
    char buf[120];
    if(fgets(buf, 120, f) == NULL) break;
    if(buf[0] == 'H' && buf[1] == 'y') {
      score sc; bool ok = true;
      sc.box[MAXBOX-1] = 0;
      {if(fscanf(f, "%s", buf) <= 0) break;} sc.ver = buf;
    

      for(int i=0; i<MAXBOX; i++) {
        if(fscanf(f, "%d", &sc.box[i]) <= 0) { boxid = i; break; }
        }
      
      for(int i=boxid; i<MAXBOX; i++) sc.box[i] = 0;

      if(sc.ver >= "4.4") {
        sc.box[0] = sc.box[65];
        // the first executable on Steam included a corruption
        if(sc.box[65] > 1420000000 && sc.box[65] < 1430000000) {
          sc.box[0] = sc.box[65] - sc.box[1];
          sc.box[65] = sc.box[0];
          }
        // do not include saves
        if(sc.box[65 + 4 + itOrbSafety - itOrbLightning]) ok = false;
        }
      else 
        sc.box[0] = sc.box[1] - sc.box[0]; // could not save then
      
      if(sc.box[2] == 0) continue; // do not list zero scores
      
      if(ok && boxid > 20) scores.push_back(sc);
      }
    }

  saveBox();
  score sc; 
  for(int i=0; i<POSSCORE; i++) sc.box[i] = savebox[i];
  sc.box[MAXBOX-1] = 1; sc.ver = "NOW";
  scores.push_back(sc);
  
  fclose(f);
  clearMessages();
  // addMessage(its(size(scores))+" games have been recorded in "+scorefile);
  pushScreen(show);
  boxid = 0; applyBoxes();
  scoresort = 2; reverse(scores.begin(), scores.end());
  scoremode = 0;
  if(shmup::on) scoremode = 1;
  else if(hardcore) scoremode = 2;
  scorefrom = 0;
  stable_sort(scores.begin(), scores.end(), scorecompare);
  }

}

#endif

