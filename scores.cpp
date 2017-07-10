#ifndef NOSAVE
vector<score> scores;

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

string displayfor(score* S) {
  // printf("S=%p, scoredisplay = %d\n", S, scoredisplay);
  if(S == NULL) {
    return XLATN(boxname[scoredisplay]);
    }
  if(scoredisplay == 0) {
    char buf[10];
    snprintf(buf, 10, "%d:%02d", S->box[0]/60, S->box[0]%60);
    return buf;
    }
  if(scoredisplay == 1) {
    time_t tim = S->box[1];
    char buf[128]; strftime(buf, 128, "%c", localtime(&tim));
    return buf;
    }
  return its(S->box[scoredisplay]);
  }

void loadScores() {
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
      if(ok && boxid > 20) scores.push_back(sc);
      }
    }
  fclose(f);
  addMessage(its(size(scores))+" games have been recorded in "+scorefile);
  pushScreen(showScores);
  boxid = 0; applyBoxes();
  scoresort = 2; reverse(scores.begin(), scores.end());
  scoremode = 0;
  if(shmup::on) scoremode = 1;
  else if(hardcore) scoremode = 2;
  scorefrom = 0;
  stable_sort(scores.begin(), scores.end(), scorecompare);
  #ifdef MOBILE
  extern int andmode;
  andmode = 2;
  #endif
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

void showScores() {
  int y = vid.fsize * 7/2;
  int bx = vid.fsize;
  getcstat = 1;
  
  string modes = 
    scoremode == 0 ? XLAT(", m - mode: normal") :
    scoremode == 1 ? XLAT(", m - mode: shoot'em up") :
    scoremode == 2 ? XLAT(", m - mode: hardcore only") :
    "?";

  if(euclid) modes += XLAT(" (E:%1)", euclidland);


  mouseovers = XLAT("t/left/right - change display, up/down - scroll, s - sort by") + modes;

  displaystr(bx*4, vid.fsize*2, 0, vid.fsize, "#", forecolor, 16);
  displaystr(bx*8, vid.fsize*2, 0, vid.fsize, "$$$", forecolor, 16);
  displaystr(bx*12, vid.fsize*2, 0, vid.fsize, XLAT("kills"), forecolor, 16);
  displaystr(bx*18, vid.fsize*2, 0, vid.fsize, XLAT("time"), forecolor, 16);
  displaystr(bx*22, vid.fsize*2, 0, vid.fsize, XLAT("ver"), forecolor, 16);
  displaystr(bx*23, vid.fsize*2, 0, vid.fsize, displayfor(NULL), forecolor, 0);
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

    char buf[16];
    
    rank++; sprintf(buf, "%d", rank);
    displaystr(bx*4,  y, 0, vid.fsize, buf, 0xC0C0C0, 16);
    
    sprintf(buf, "%d", S.box[2]);
    displaystr(bx*8,  y, 0, vid.fsize, buf, 0xC0C0C0, 16);
    
    sprintf(buf, "%d", S.box[3]);
    displaystr(bx*12,  y, 0, vid.fsize, buf, 0xC0C0C0, 16);

    sprintf(buf, "%d:%02d", S.box[0]/60, S.box[0] % 60);
    displaystr(bx*18,  y, 0, vid.fsize, buf, 0xC0C0C0, 16);

    displaystr(bx*22,  y, 0, vid.fsize, S.ver, 0xC0C0C0, 16);

    displaystr(bx*23, y, 0, vid.fsize, displayfor(&S), 0xC0C0C0, 0);
    
    y += vid.fsize*5/4; id++;
    }

#ifdef MOBILE
  buttonclicked = false;
  displayabutton(-1, +1, XLAT("SORT"), BTON);
  displayabutton( 0, +1, XLAT("PICK"), BTON);
  displayabutton(+1, +1, XLAT("PLAY"), BTON);
#endif

  keyhandler = [] (int sym, int uni) {
  #ifndef MOBILE
    if(sym == SDLK_LEFT || sym == SDLK_KP4 || sym == 'h' || sym == 'a')
      shiftScoreDisplay(-1);
    else if(sym == SDLK_RIGHT || sym == SDLK_KP6 || sym == 'l' || sym == 'd')
      shiftScoreDisplay(1);
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
  #else
    static int scoredragx, scoredragy;
    extern bool clicked, lclicked;
    extern int andmode;
    
    if(andmode) { 
      if(!clicked && !lclicked) {
        andmode = 0;
        scoredragx = mousex;
        scoredragy = mousey;
        }
      }
  
    else {
    
      if(clicked && !lclicked)
        scoredragx = mousex, scoredragy = mousey;
    
      else if(lclicked && !clicked) {
        if(mousey > vid.ycenter - 2 * vid.fsize) {
          if(mousex < vid.xcenter*2/3) sortScores();
          else if(mousex < vid.xcenter*4/3)
            cmode = emPickScores;
          else andmode = 0, popScreen();
          }
        }
      
      else if(clicked && lclicked) {
    //  if(mousex > scoredragx + 80) scoredragx += 80, shiftScoreDisplay(1); 
    //  if(mousex < scoredragx - 80) scoredragx -= 80, shiftScoreDisplay(-1); 
        while(mousey > scoredragy + vid.fsize) scoredragy += vid.fsize, scorefrom--;
        while(mousey < scoredragy - vid.fsize) scoredragy -= vid.fsize, scorefrom++;
        }
      }
  #endif
    };
  }

bool monsterpage = false;
         
void showPickScores() {

  getcstat = '0';  
  int d = scoredisplay;
  
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
  int columns = 1 + (q-1) / percolumn;
  
  for(int i=0; i<q; i++) {
    int x = 16 + (vid.xres * (i/percolumn)) / columns;
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
    extern int andmode;
    andmode = 2;
    if(uni == 'm') monsterpage = !monsterpage; else
    if(uni >= '1' && uni <= '9') uni = uni + 1000 - '1';
    else if(uni >= 1000 && uni < 1000 + size(pickscore_options)) { 
      scoredisplay = pickscore_options[uni - 1000].second;
      popScreen();
      }
    else if(mapeditor::editInfix(uni)) ;
    else if(doexiton(sym, uni)) popScreen();
    };
  }
#endif

