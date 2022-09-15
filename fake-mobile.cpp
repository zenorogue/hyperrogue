// Hyperbolic Rogue -- fake mobile target
// Copyright (C) 2011-2018 Zeno Rogue, see 'hyper.cpp' for details

#define ISFAKEMOBILE 1
#define MOBPAR_FORMAL int
#define MOBPAR_ACTUAL 0
#define MIX_MAX_VOLUME 128
#define NOMAIN
#include <string>

namespace hr {
  string scorefile = "fakemobile_score.txt";  
  }

#include <SDL/SDL.h>

#include "hyper.cpp"

#include <SDL/SDL_ttf.h>
#include <SDL/SDL_gfxPrimitives.h>

#undef main

namespace hr {

transmatrix Orient = Id;

int lasttick = 0;

transmatrix getOrientation() { 
  int curtick = SDL_GetTicks();
  ld t = (curtick - lasttick) / 1000.0;
  lasttick = curtick;
  Uint8 *keystate = SDL_GetKeyState(NULL);
  if(keystate[SDLK_LCTRL]) {
    if(keystate['s']) Orient = Orient * cspin(2, 1, t);
    if(keystate['w']) Orient = Orient * cspin(1, 2, t);
    if(keystate['a']) Orient = Orient * cspin(2, 0, t);
    if(keystate['d']) Orient = Orient * cspin(0, 2, t);
    }
  return Orient;
  }

void resetmusic() { printf("resetmusic\n"); }

void playSound(cell *c, const string& fname, int vol) {
  printf("Play sound: %s\n", fname.c_str());
  }

SDL_Surface *s;

int gdpos = 0;

int gdpop() { return graphdata[gdpos++]; }

TTF_Font *font[256];

bool rawdisplaystr(int x, int y, int shift, int size, const char *str, int color, int align) {

  if(strlen(str) == 0) return false;

  if(size <= 0 || size > 255) {
    return false;
    }
  SDL_Color col;
  col.r = (color >> 16) & 255;
  col.g = (color >> 8 ) & 255;
  col.b = (color >> 0 ) & 255;
  
  col.r >>= darken; col.g >>= darken; col.b >>= darken;

  if(!font[size])
    font[size] = TTF_OpenFont("VeraBd.ttf", size);

  SDL_Surface *txt = TTF_RenderText_Solid(font[size], str, col);
  
  if(txt == NULL) return false;

  SDL_Rect rect;

  rect.w = txt->w;
  rect.h = txt->h;

  rect.x = x - rect.w * align / 16;
  rect.y = y - rect.h/2;
  
  bool clicked = (mousex >= rect.x && mousey >= rect.y && mousex <= rect.x+rect.w && mousey <= rect.y+rect.h);
  
  SDL_BlitSurface(txt, NULL, s,&rect); 
  SDL_FreeSurface(txt);
  
  return clicked;
  }

int textwidth(int siz, const string &str) {
  if(isize(str) == 0) return 0;
  
  if(!font[siz]) font[siz] = TTF_OpenFont("VeraBd.ttf", siz);
  
  int w, h;
  TTF_SizeUTF8(font[siz], str.c_str(), &w, &h);
  // printf("width = %d [%d]\n", w, isize(str));
  return w;
  }

char action;

bool currentlyConnecting() { return false; }
bool currentlyConnected() { return false; }
void viewAchievements() { printf("view Achievements\n"); }
void viewLeaderboard(string id) { printf("view Leaderboard :: %s\n", id.c_str()); }
void switchGoogleConnection() { printf("sgc\n"); }

void shareScore(int) {
  printf("share\n");
  }

void openURL() {
  printf("< openURL > \n");
  }

}

using namespace hr;

int main(int argc, char **argv) {

  initAll();
  glhr::init();
  
  vid.xres = 800; vid.yres = 450;
  vid.usingGL = false;
  // 450; vid.yres = 600;

  s= SDL_SetVideoMode(vid.xres, vid.yres, 32, 0);
  
  if(TTF_Init() != 0) {
    printf("Failed to initialize TTF.\n");
    exit(2);
    }

  int mx = 0; int my = 0; bool _clicked = false;
  
  inv::on = true;
  // firstland = laMinefield;
  // activateSafety(laWhirlwind);
  
  items[itGreenStone] = 100;
  // items[itDiamond] = 50;
  // for(int i=1; i<10; i++) kills[i] = 5;
  
  while(true) {
   
    SDL_LockSurface(s);
    memset(s->pixels, 0, vid.xres * vid.yres * 4);
    SDL_UnlockSurface(s);
    
    mousex = mx;
    mousey = my;
    clicked = _clicked;
    
    mobile_draw(MOBPAR_ACTUAL);
    action = 0;
    
    gdpos = 0;
    while(gdpos < isize(graphdata)) {
      switch(gdpop()) {
        case 2: {
          int x = gdpop(), y = gdpop(), al = gdpop();
          int color = gdpop();
          int size = gdpop();
          gdpop();
          int n = gdpop();
          string s;
          for(int i=0; i<n; i++) s += char(gdpop());
          rawdisplaystr(x, y, 0, size, s.c_str(), color, al);
          break;
          }
        
        case 1: {
          int col = gdpop();
          int otl = gdpop();
          int num = gdpop();
          
          Sint16 xpox[6000], xpoy[6000];
          
          // printf("%4d polygon %d\n", gdpos, num);
          for(int i=0; i<num; i++) xpox[i] = gdpop(), xpoy[i] = gdpop();

          filledPolygonColor(s, xpox, xpoy, num, col);
          aapolygonColor(s, xpox, xpoy, num, otl);
          break;
          }

        case 3: {
          int col = gdpop();
          int num = gdpop();

          for(int i=0; i<num; i++) polyx[i] = gdpop(), polyy[i] = gdpop();
          
          for(int i=0; i<num-1; i++)
            aalineColor(s, polyx[i], polyy[i], polyx[i+1], polyy[i+1], col);
          }

        case 4: {
          int col = gdpop();
          int out = gdpop();
          int x = gdpop(), y = gdpop(), rad = gdpop();
          
          aacircleColor(s, x, y, rad, (col << 8) + 0xFF);
          }

        case 5: {
          int out = gdpop();
          /* not implemented */
          }

        }
      }
    SDL_UpdateRect(s, 0, 0, vid.xres, vid.yres);

    SDL_Event ev;
    while(SDL_PollEvent(&ev)) {
    
      if(ev.type == SDL_MOUSEBUTTONDOWN) {
        mx = ev.button.x;
        my = ev.button.y;
        _clicked = true;
        }

      if(ev.type == SDL_MOUSEBUTTONUP) {
        _clicked = false;
        }

      if(ev.type == SDL_MOUSEMOTION) {
        mx = ev.motion.x;
        my = ev.motion.y;
        }
      
      if(ev.type == SDL_KEYDOWN) {
        int sym = ev.key.keysym.sym;
/*        if(sym == '1') {
  printf("Number of cells explored, by distance from the player:\n"); 
  for(int i=0; i<10; i++) printf(" %d", explore[i]); printf("\n");
          return 0;
          }
        else if(sym == '2') {
          items[rand() % ittypes] += 3;
          kills[rand() % motypes] += 3;
          }
        else if(sym == '3') {
          items[itHell] = 0;
          items[itGreenStone] = 100;
          }
        action = sym; */
        if(ev.key.keysym.mod & KMOD_LCTRL) continue;
        mousing = false;
        handlekey(sym, sym);
        }
        
      if(ev.type == SDL_QUIT) {
        SDL_Quit();
        return 0;
        }
      }
    }

  SDL_Quit();
  
  clearMemory();
  }

