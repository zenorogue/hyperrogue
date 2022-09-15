// Hyperbolic Rogue -- local highscore lists
// Copyright (C) 2011-2018 Zeno Rogue, see 'hyper.cpp' for details

/** \file scores.cpp
 *  \brief local highscore lists
 */

#include "hyper.h"
#if CAP_SAVE

namespace hr { namespace scores {

vector<score> scores;
score *currentgame;

int scorefrom = 0;
bool scorerev = false;

string csub(const string& str, int q) {
  int i = 0;
  for(int j=0; j<q && i<isize(str); j++) getnext(str.c_str(), i);
  return str.substr(0, i);
  }

int colwidth(int scoredisplay) {
  if(scoredisplay == 0) return 5;
  if(scoredisplay == 1) return 16;
  if(scoredisplay == 5) return 8;
  if(scoredisplay == POSSCORE) return 8;
  return 4;
  }

bool isHardcore(score *S) {
  return S->box[117] && S->box[118] < PUREHARDCORE_LEVEL;
  }

int modediff(score *S) {
  int diff = 0;
  eGeometry g = (eGeometry) S->box[116]; 
  if(S->box[306] != inv::on) diff += 4;
  if(S->box[238]) g = gSphere;
  if(S->box[239]) g = gElliptic;
  if(max(S->box[197], 1) != multi::players) diff += 8;
  
  eVariation savevar = 
    S->box[341] ? eVariation::goldberg :
    S->box[344] ? eVariation::irregular :
    eVariation(S->box[186]);

  if(variation != savevar) diff += 16;
  if(GOLDBERG && savevar == eVariation::goldberg && (S->box[342] != gp::param.first || S->box[343] != gp::param.second))
    diff += 16;
  if(S->box[196] != (int) land_structure) diff += 32;
  if(S->box[119] != shmup::on) diff += 64;
  if(pureHardcore() && !isHardcore(S)) diff += 128;
  if(g != gNormal && S->box[120] != specialland) 
    diff += 256;
  if(g != geometry) {
    diff += 512;
    }
  return -diff;
  }
    
string modedesc(score *S) {
  eGeometry g = (eGeometry) S->box[116]; 
  if(S->box[238]) g = gSphere;
  if(S->box[239]) g = gElliptic;
  string s = ginf[g].shortname;
  if(g != gNormal) s += " " + csub(XLATT1((eLand) S->box[120]), 3);
  if(S->box[341]) s += "/GP(" + its(S->box[342])+","+its(S->box[343])+")";
  else if(S->box[186]) s += "/7";
  if(S->box[196]) s += "/C";
  if(S->box[119]) s += "/s";
  if(S->box[197] > 1) s += "/P" + its(S->box[197]);
  if(S->box[306]) s += "/i";
  if(isHardcore(S)) s += "/h";
  return s;
  }

string displayfor(int scoredisplay, score* S, bool shorten = false) {
  // printf("S=%p, scoredisplay = %d\n", S, scoredisplay);
  if(S == NULL) {
    if(scoredisplay == POSSCORE) return "mode";
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
    return csub(str, 5);
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
  if(scoredisplay == POSSCORE) return modedesc(S);
  if(scoredisplay == 1) {
    time_t tim = S->box[1];
    char buf[128]; strftime(buf, 128, "%c", localtime(&tim));
    return buf;
    }
  return its(S->box[scoredisplay]);
  }

vector<pair<string, int> > pickscore_options;

int curcol;

vector<int> columns;

bool monsterpage = false;
         
void showPickScores() {

  pickscore_options.clear();

  scorerev = false;

  for(int i=0; i<=POSSCORE; i++) {
    int scoredisplay = i;
    if(!fakebox[scoredisplay]) {
      string s = displayfor(scoredisplay, NULL);
      if(dialog::hasInfix(s))
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
    
    int scoredisplay = pickscore_options[i].second;
    if(q <= 9)
      pickscore_options[i].first = pickscore_options[i].first + " [" + its(i+1) + "]";
    if(!fakebox[scoredisplay])
      displayButton(x, y, pickscore_options[i].first, 1000+i, 0);
    }
  
  displayButton(vid.xres/2, vid.yres - vid.fsize*2, "kills", '/', 8);

  mouseovers = dialog::infix;
  keyhandler = [] (int sym, int uni) {
    if(uni == '/' && dialog::infix == "") monsterpage = !monsterpage; else
    if(uni >= '1' && uni <= '9') uni = uni + 1000 - '1';
    else if(uni >= 1000 && uni < 1000 + isize(pickscore_options)) {
      int scoredisplay = pickscore_options[uni - 1000].second;
      for(int i=0; i<=POSSCORE; i++)
        if(columns[i] == scoredisplay) swap(columns[i], columns[curcol]);
      popScreen();
      }
    else if(dialog::editInfix(uni)) ;
    else if(doexiton(sym, uni)) popScreen();
    };
  }

void show() {

  if(columns.size() == 0) {
    columns.push_back(POSSCORE);
    for(int i=0; i<POSSCORE; i++) columns.push_back(i);
    }
  int y = vid.fsize * 5/2;
  int bx = vid.fsize;
  getcstat = 0;
  
  displaystr(bx*4, vid.fsize, 0, vid.fsize, "#", forecolor, 16);
  displaystr(bx*8, vid.fsize, 0, vid.fsize, XLAT("ver"), forecolor, 16);
  
  int at = 9;
  for(int i=0; i<=POSSCORE; i++) {
    int c = columns[i];
    if(bx*at > vid.xres) break;
    if(displaystr(bx*at, vid.fsize, 0, vid.fsize, displayfor(c, NULL, true), i == curcol ? 0xFFD500 : forecolor, 0))
      getcstat = 1000+i;
    at += colwidth(c);
    }
  
  if(scorefrom < 0) scorefrom = 0;
  int id = 0;
  int omit = scorefrom;
  int rank = 0;
  while(y < (ISMOBILE ? vid.yres - 5*vid.fsize : vid.yres - 2 * vid.fsize)) { 
    if(id >= isize(scores)) break;
        
    score& S(scores[id]);
    
    if(omit) { omit--; rank++; id++; continue; }
    
    bool cur = S.box[MAXBOX-1];
    if(cur) {
      saveBox(); 
      for(int i=0; i<POSSCORE; i++) S.box[i] = save.box[i];
      S.box[0] = S.box[65];
      }
    color_t col = cur ? 0xFFD500 : 0xC0C0C0;
    

    rank++;
    displaystr(bx*4,  y, 0, vid.fsize, its(rank), col, 16);
    
    displaystr(bx*8,  y, 0, vid.fsize, S.ver, col, 16);

    int at = 9;
    for(int i=0; i<=POSSCORE; i++) {
      int c = columns[i];
      if(bx*at > vid.xres) break;
      at += colwidth(c);
      if(displaystr(bx*(at-1), y, 0, vid.fsize, displayfor(c, &S), col, 16))
        getcstat = 1000+i;
      }

    y += vid.fsize*5/4; id++;
    }

  int i0 = vid.yres - vid.fsize;
  int xr = vid.xres / 80;

  displayButton(xr*10, i0, IFM("s - ") + XLAT("sort"), 's', 8);
  displayButton(xr*30, i0, IFM("t - ") + XLAT("choose"), 't', 8);
  displayButton(xr*50, i0, IFM(dialog::keyname(SDLK_ESCAPE) + " - ") + XLAT("go back"), '0', 8);

  keyhandler = [] (int sym, int uni) {
    if(DKEY == SDLK_LEFT || uni == 'h' || uni == 'a') {
      scorerev = false;
      if(curcol > 0) curcol--;
      }
    else if(DKEY == SDLK_RIGHT || uni == 'l' || uni == 'd') {
      scorerev = false;
      if(curcol < POSSCORE) curcol++;
      }
    else if(sym >= 1000 && sym <= 1000+POSSCORE) {
      scorerev = false;
      curcol = sym - 1000;
      }
    else if(uni == 't') { dialog::infix = ""; pushScreen(showPickScores); }
    else if(DKEY == SDLK_UP || uni == 'k' || uni == 'w')
      scorefrom -= 5;
    else if(DKEY == SDLK_DOWN || uni == 'j' || uni == 'x')
      scorefrom += 5;
    else if(sym == PSEUDOKEY_WHEELUP)
      scorefrom--;
    else if(sym == PSEUDOKEY_WHEELDOWN)
      scorefrom++;
    else if(uni == 's') {
      if(scorerev) reverse(scores.begin(), scores.end());
      else {
        scorerev = true;
        stable_sort(scores.begin(), scores.end(), [] (const score& s1, const score &s2) {
          return s1.box[columns[curcol]] > s2.box[columns[curcol]];
          });
        }
      }
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
  if(scorefile == "") return;
  scores.clear();
  FILE *f = fopen(scorefile.c_str(), "rt");
  if(!f) {
    printf("Could not open the score file '%s'!\n", scorefile.c_str());
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

      if(!verless(sc.ver, "4.4")) {
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
      sc.box[POSSCORE] = modediff(&sc);
      
      if(ok && boxid > 20) scores.push_back(sc);
      }
    }

  saveBox();
  score sc; 
  for(int i=0; i<POSSCORE; i++) sc.box[i] = save.box[i];
  sc.box[POSSCORE] = 0;
  sc.box[MAXBOX-1] = 1; sc.ver = "NOW";
  scores.push_back(sc);
  
  fclose(f);
  clearMessages();
  // addMessage(its(isize(scores))+" games have been recorded in "+scorefile);
  pushScreen(show);
  boxid = 0; applyBoxes();
  reverse(scores.begin(), scores.end());
  scorefrom = 0;
  stable_sort(scores.begin(), scores.end(), [] (const score& s1, const score& s2) {
    return tie(s1.box[POSSCORE], s1.box[2]) > tie(s2.box[POSSCORE], s2.box[2]);
    });
  }

}}

#endif

