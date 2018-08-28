// Hyperbolic Rogue -- initialization, and stuff related to mobiles
// Copyright (C) 2011-2018 Zeno Rogue, see 'hyper.cpp' for details

#include "compileunits.h"

namespace hr {

#if CU_INIT
int fontscale = 100;

#if ISANDROID
FILE *debfile;
#endif

FILE *debugfile;
int debugflags;

string s0;

bool fixseed = false;
int startseed = 0;

eLand firstland0;

void initAll() {
  showstartmenu = true;
  ca::init();
#if CAP_COMMANDLINE
  arg::read(1);
#endif
  srand(time(NULL));
  shrand(fixseed ? startseed : time(NULL));

  achievement_init(); // not in ANDROID

  firstland0 = firstland;
  
  // initlanguage();
  initgraph();
#if CAP_SAVE
  loadsave();
  if(IRREGULAR) irr::auto_creator();
#endif
  start_game();
  
  shmup::safety = safety;

  if(!shmup::on) {
    restoreGolems(items[itOrbLife], moGolem); items[itOrbLife] = 0;
    restoreGolems(items[itOrbFriend], moTameBomberbird); items[itOrbFriend] = 0;
    restoreGolems(kills[moPrincessMoved], moPrincess, princess::saveHP); kills[moPrincessMoved] = 0;
    restoreGolems(kills[moPrincessArmedMoved], moPrincessArmed, princess::saveArmedHP); kills[moPrincessArmedMoved] = 0;
    }
  
  firstland = firstland0;
  polygonal::solve();
  }

void finishAll() {
  achievement_final(!items[itOrbSafety]);
  
#if CAP_SAVE
  saveStats();
#endif
  offscreen.clear();
  clearMemory();
#if ISMOBILE==0
  cleargraph();
#endif
  
  achievement_close();  
  }

#if ISANDROID
string buildScoreDescription() {
  string s;
  time_t timer;
  timer = time(NULL);
  char buf[128]; strftime(buf, 128, "%c", localtime(&timer));
  char buf2[128];
  
  s += XLAT("HyperRogue for Android");
  s += " ( " VER "), http://www.roguetemple.com/z/hyper/\n";
  s += XLAT("Date: %1 time: %2 s ", buf, getgametime_s());
  s += XLAT("distance: %1\n", its(celldist(cwt.at)));
  // s += buf2;
  if(cheater) s += XLAT("Cheats: ") + its(cheater) + "\n";
  s += XLAT("Score: ") + its(gold());

  for(int i=0; i<ittypes; i++) if(items[i]) {
    string t = XLATN(iinf[i].name);
    sprintf(buf2, " %s (%d)", t.c_str(), items[i]);
    s += buf2;
    }
  s += "\n";
  s += XLAT("Kills: ") + its(tkills());
  for(int i=1; i<motypes; i++) if(kills[i]) {
    string t = XLATN(minf[i].name);
    sprintf(buf2, " %s (%d)", t.c_str(), kills[i]);
    s += buf2;
    }
  s += "\n";

  for(int i=0; i<gamelog.size(); i++) if(gamelog[i].msg != "") s += gamelog[i].msg + "\n";
  
  return s;
  }
#endif

#if ISMOBILE==1

int andmode;

bool lclicked = false, clicked = false;
string lmouseovers;
bool inmenu = false;

bool longclick;
bool verylongclick;

void handleScoreClick();
void openURL();
void displayTexts();
void controlMusic(int ticks);

void showHelp(MOBPAR_FORMAL, string nhelp) {
  gotoHelp(nhelp);
  }

bool useRangedOrb;

void handleclick(MOBPAR_FORMAL) {

    if(handleCompass()) return;

    if(buttonclicked || mouseout()) {

      bool statkeys = andmode == 0 && !shmup::on && isize(screens) == 1;
      
      if(statkeys && getcstat == 'g') {
        movepcto(MD_DROP);
        getcstat = 0;
        }
      
      else if(statkeys && getcstat == 'i') {
        if(inv::on) pushScreen(inv::show);
        getcstat = 0;
        }
      
      else if(statkeys && getcstat == 't') {
        if(playermoved && items[itStrongWind]) {
          cell *c = whirlwind::jumpDestination(cwt.at);
          if(c) centerover.at = c, centerover.spin = 0;
          }
        targetRangedOrb(centerover.at, roKeyboard);
        getcstat = 0;
        }

      else if(getcstat != SDLK_F1 && getcstat != 'i' && getcstat != 't') {
        int px = mousex < vid.xcenter ? 0 : 1;
        int py = mousey < vid.ycenter ? 0 : 1;
        
        if(isize(screens) == 1) {
          if(px == 0 && py == 1) {
            if(andmode == 0 && shmup::on) ; 
            else andmode = 10;
            }
          if(px == 1 && py == 1) {
            if(andmode == 0 && shmup::on) ; // just fire, do not change modes
            else {
              if(andmode == 1) {
                ors::reset();
                centerpc(INF);
                View = Id;
                viewctr.at = cwt.at->master;
                }
              andmode = 11;
              }
            }
          if(px == 0 && py == 0) andmode = 22;
          if(px == 1 && py == 0) andmode = 13;
          }
        }
      
      else {
        if(andmode == 0 && help != "@") {
          addMessage(mouseovers);
          showHelp(MOBPAR_ACTUAL, help);
	  andmode = 10;
          getcstat = 0;
          return;
          }
        }
      }
    
    if(andmode == 0 && isize(screens) == 1 && !mouseout()) {

      bool forcetarget = longclick;
      
      if(mouseover && targetclick && targetRangedOrb(mouseover, forcetarget ? roMouseForce : roMouse)) {
        ;
        }
      else if(!forcetarget) movepcto(mousedest);
      }

    if(andmode == 10) {
      if(!playerfound) {
        centerpc(INF);
        View = Id;
        viewctr.at = cwt.at->master;
        }
      playermoved = true;
      }
      
    if(andmode >= 10) andmode -= 10;

  if(andmode == 3) {
    if(tour::on)
      showMissionScreen();
    else
      pushScreen(showMainMenu);
    andmode = 0;
    }
  }

int touchedAt;

#if CAP_ANDROIDSHARE
void shareScore(MOBPAR_FORMAL);
#endif

void mobile_draw(MOBPAR_FORMAL) {

  optimizeview();

  int lastt = ticks; ticks = SDL_GetTicks();
  if(lastt > ticks) lastt = ticks;
  int tdiff = ticks - lastt;

  ors::check_orientation();

  if(playermoved && vid.sspeed > -4.99)
    centerpc(tdiff / 1000.0 * exp(vid.sspeed));

  if(shmup::on && (andmode == 0 || andmode == 10) && isize(screens) == 1) 
    shmup::turn(tdiff);
    
  safety = false;
  vid.fsize = (min(vid.xres, vid.yres) * fontscale + 50) / 3200;
  
  mouseoh = mouseh;
  gtouched = mousepressed = clicked;

  longclick = lclicked && ticks > touchedAt + 500;
  verylongclick = longclick && ticks > touchedAt + 1000;

  useRangedOrb = 
    longclick || (!(vid.shifttarget & 2) && haveRangedOrb() && lmouseover && lmouseover->cpdist > 1);

  targetclick = ((vid.shifttarget & 2) && !shmup::on) ? longclick : true;

  if(shmup::on) {
    using namespace shmupballs;
    if(hypot(mousex - xfire, mousey - yb) < rad) targetclick = false;
    if(hypot(mousex - xmove, mousey - yb) < rad) targetclick = false;
    }
  
  if(cmode & sm::NORMAL) {
    lmouseover = (gtouched && lclicked) ? mouseover : NULL;
    if(!shmup::on && !useRangedOrb && vid.mobilecompasssize) {
      using namespace shmupballs;
      int dx = mousex - xmove;
      int dy = mousey - yb;
      int h = hypot(dx, dy);
      if(h < rad) {
        if(h < rad*SKIPFAC) { lmouseover = cwt.at; mousedest.d = -1; }
        else {
          double d = vid.revcontrol ? -1 : 1;
          mouseh = hpxy(dx * d / rad, dy * d / rad);
          calcMousedest();          
          }
        }
      }
    if(andmode == 0 && !useRangedOrb && gtouched && lclicked) {
      lmouseover = mousedest.d >= 0 ? cwt.at->modmove(cwt.spin + mousedest.d) : cwt.at;
      }
    }

  #if CAP_RUG
  if(rug::rugged)
    mouseh = rug::gethyper(mousex, mousey);
  else
  #endif
    mouseh = gethyper(mousex, mousey);

  inmenu = isize(screens) > 1;

  if(!inmenu && stereo::mode == stereo::sLR && ors::mode)
    mousex = vid.xres/2, mousey = vid.yres/2, mouseh = sphereflip * C0;

//  if(debfile) fprintf(debfile, "d1\n"), fflush(debfile);
  frames++;
  if(conformal::on) conformal::apply();

  if(ticks > lastt) tortoise::updateVals(ticks - lastt);

  if(clicked && !lclicked) touchedAt = ticks;  

#if CAP_XGD
  graphdata.clear();
#endif
  getcstat = 0; shiftmul = 1; getcshift = 1;
  drawscreen();
  shiftmul = getcshift;
  calcMousedest();

  if(lclicked && !clicked && !inmenu) handleclick(MOBPAR_ACTUAL);

  if(inmenu && !clicked && !lclicked) inmenu = false;
  
  bool keyreact = lclicked && !clicked;

#if CAP_MENUSCALING  
  if(true) {
    using namespace dialog::zoom;
    if(zoomoff || !(cmode & sm::ZOOMABLE)) nozoom();
    else if(clicked && !lclicked) initzoom();
    else if(!clicked && zoomf > 1) stopzoom();
    }
#endif
  
  if(inslider) keyreact = true;

#if CAP_ANDROIDSHARE
  if(getcstat == 's'-96 && keyreact) {
    popScreenAll();
    shareScore(MOBPAR_ACTUAL);
    }
#endif

  bool normal_reaction = !inmenu;

  if(normal_reaction && stereo::mode == stereo::sLR) {
    normal_reaction = false;
    if(lclicked && !clicked) {
      if(rug::rugged)
        rug::select();
      else if(ors::mode && !verylongclick)
        normal_reaction = true;
      else
        pushScreen(showStereo);
      }
    }

  if(andmode == 2 && isize(screens) != 1) andmode = 12;

  if((cmode & sm::NORMAL) && getcstat == '-')
    getcstat = 0;

  if(keyreact) {
    handlekey(getcstat, getcstat);
    }

  #if ISIOS
  displayTexts();
  #endif

  if(clicked && lclicked && andmode == 1 && normal_reaction) {
    if(!mouseout2() && mouseoh[2] < 50 && mouseh[2] < 50 && !rug::rugged) {
      panning(mouseoh, mouseh);
      }
    }
  
  static int lticks_rug;
  
#if CAP_RUG
  if(andmode == 1 && normal_reaction && rug::rugged && clicked)
    rug::move_forward((ticks - lticks_rug) / 2500.);
#endif
  
  lticks_rug = ticks;

  if(andmode == 1 && lclicked && !clicked && normal_reaction && mouseover)
    performMarkCommand(mouseover);

  if(clicked && andmode == 2 && (mouseover != lmouseover || mouseovers != lmouseovers) && normal_reaction) {
    addMessage(mouseovers);
    lmouseovers = mouseovers;
    }

  if(andmode == 10 && clicked != lclicked) andmode = 0;
  if(andmode == 20 && clicked != lclicked) andmode = 10;

  if(andmode == 2 && lclicked && !clicked) { 
    showHelp(MOBPAR_ACTUAL, help);
    }

  else if(andmode == 4) {
    achievement_final(false);
    }
    
  if(clicked && andmode == 12 && (mouseover != lmouseover || mouseovers != lmouseovers)) {
    addMessage(mouseovers);
    lmouseovers = mouseovers;
    }

  if(clicked != lclicked)
    flashMessages();

  // END
  lclicked = clicked;  

#if ISIOS
  controlMusic(ticks - lastt);
  #endif
  }

#endif

#if !CAP_AUDIO
void playSound(cell*, const string &s, int vol) { printf("play sound: %s vol %d\n", s.c_str(), vol); }
#endif

#endif

}
