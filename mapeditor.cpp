// HyperRogue map editor
// Copyright (C) 2011-2016 Zeno Rogue, see 'hyper.cpp' for details

#include <map>
#include <stdint.h>

#if ISWINDOWS
#include "direntx.h"
#include "direntx.c"
#else
#include <dirent.h>
#endif

namespace mapeditor {
  int subcanvas;
  int displaycodes;

  struct editwhat {
    double dist;
    int rotid, symid, pointid;
    bool side;
    cell *c;
    } ew, ewsearch;
  bool autochoose = ISMOBILE;
  
#if CAP_EDIT
  map<int, cell*> modelcell;

  void handleKeyMap(int sym, int uni);
  bool handleKeyFile(int sym, int uni);

  void applyModelcell(cell *c) {
    if(mapeditor::whichPattern == 'H') return;
    if(mapeditor::whichPattern == 'H') return;
    int i = realpattern(c);
    cell *c2 = modelcell[i];
    if(c2) {
      c->wall = c2->wall;
      c->land = c2->land;
      c->monst = c2->monst;
      c->item = c2->item;
      c->landparam = c2->landparam;
      c->wparam = c2->wparam;
      c->mondir = c2->mondir;
      if(c2->mondir != NODIR)
        c->mondir = (c2->mondir - patterndir(c2) + patterndir(c) + 7*6*5) % c->type;
      }
    }
#endif
  }

#if CAP_EDIT
namespace mapstream {
  std::map<cell*, int> cellids;
  vector<cell*> cellbyid;
  vector<char> relspin;
  
  FILE *f;
  
  void addToQueue(cell* c) {
    if(cellids.count(c)) return;
    int numcells = size(cellbyid);
    cellbyid.push_back(c);
    cellids[c] = numcells;
    }
  
  void saveChar(char c) { fwrite(&c, 1, 1, f); }
  template<class T> void save(T& c) { fwrite(&c, sizeof(T), 1, f); }

  char loadChar() { char c; int i=fread(&c, 1, 1, f); if(i!=1) return 0; else return c; }
  template<class T> int load(T& c) { return fread(&c, sizeof(T), 1, f); }
  int32_t loadInt() { int i; if(load(i) < 1) return -1; else return i; }
  
  void savePoint(const hyperpoint& h) {
    for(int i=0; i<3; i++) { double x = h[i]; save(x); }
    }
  
  hyperpoint loadPoint() {
    hyperpoint h;
    for(int i=0; i<3; i++) { double x; load(x); h[i] = x; }
    return h;
    }
  
  bool saveMap(const char *fname) {
    f = fopen(fname, "wb");
    if(!f) return false;
    int32_t i = VERNUM; save(i);
    save(mapeditor::whichPattern);
    addToQueue(cwt.c->master->c7);
    for(int i=0; i<size(cellbyid); i++) {
      cell *c = cellbyid[i];
      if(i) {
        for(int j=0; j<c->type; j++) if(c->mov[j] && cellids.count(c->mov[j]) && 
          cellids[c->mov[j]] < i) {
          int32_t i = cellids[c->mov[j]];
          save(i);
          saveChar(c->spn(j));
          saveChar(j);
          break;
          }
        }
      saveChar(c->land);
      saveChar(c->mondir);
      saveChar(c->monst);
      saveChar(c->wall);
      // saveChar(c->barleft);
      // saveChar(c->barright);
      saveChar(c->item);
      saveChar(c->mpdist);
      // saveChar(c->bardir);
      save(c->wparam); save(c->landparam);
      saveChar(c->stuntime); saveChar(c->hitpoints);
      for(int j=0; j<c->type; j++) {
        cell *c2 = c->mov[j];
        if(c2 && c2->land != laNone) addToQueue(c2);
        }
      }
    int32_t n = -1; save(n);
    int32_t id = cellids.count(cwt.c) ? cellids[cwt.c] : -1;
    save(id);

    for(int i=0; i<USERSHAPEGROUPS; i++) for(int j=0; j<USERSHAPEIDS; j++) {
      usershape *us = usershapes[i][j];
      if(!us) continue;
      
      for(int l=0; l<USERLAYERS; l++) if(size(us->d[l].list)) {
        usershapelayer& ds(us->d[l]);
        save(i); save(j); save(l); save(ds.sym); save(ds.rots); save(ds.color);
        n = size(ds.list); save(n);
        savePoint(ds.shift);
        savePoint(ds.spin);
        for(int i=0; i<size(ds.list); i++) savePoint(ds.list[i]);
        }
      }
    n = -1; save(n);

    fclose(f);
    cellids.clear();
    cellbyid.clear();
    return true;
    }
  
  int fixspin(int rspin, int dir, int t) {
    if(dir >= 0 && dir < t)
      return (dir + rspin) % t;
    else
      return dir;
    }
  
  bool loadMap(const char *fname) {
    f = fopen(fname, "rb");
    if(!f) return false;
    int vernum = loadInt();
    printf("vernum = %d\n", vernum);
    if(vernum >= 7400) load(mapeditor::whichPattern);

    clearMemory();
    initcells();
    if(shmup::on) shmup::init();

    while(true) {
      cell *c;
      int rspin;
      
      if(size(cellbyid) == 0) {
        c = currentmap->gamestart();
        rspin = 0;
        }
      else {
        int32_t parent = loadInt();
        
        if(parent<0 || parent >= size(cellbyid)) break;
        int dir = loadChar();
        cell *c2 = cellbyid[parent];
        dir  = fixspin(dir, relspin[parent], c2->type);
        c = createMov(c2, dir);
        // printf("%p:%d,%d -> %p\n", c2, dir, c);
        
        // spinval becomes xspinval
        rspin = (c2->spn(dir) - loadChar() + 42) % c->type;
        }
      
      cellbyid.push_back(c);
      relspin.push_back(rspin);
      c->land = (eLand) loadChar();
      c->mondir = fixspin(rspin, loadChar(), c->type);
      c->monst = (eMonster) loadChar();
      c->wall = (eWall) loadChar();
      // c->barleft = (eLand) loadChar();
      // c->barright = (eLand) loadChar();
      c->item = (eItem) loadChar();
      c->mpdist = loadChar();
      c->bardir = NOBARRIERS;
      // fixspin(rspin, loadChar(), c->type);
      if(vernum < 7400) {
        load(c->aitmp);
        c->wparam = c->aitmp;
        }
      else load(c->wparam);
      load(c->landparam);
      // backward compatibility
      if(vernum < 7400 && !isIcyLand(c->land)) c->landparam = HEAT(c);
      c->stuntime = loadChar();
      c->hitpoints = loadChar();

      if(mapeditor::whichPattern)
        mapeditor::modelcell[mapeditor::realpattern(c)] = c;
      }
    
    int32_t whereami = loadInt();
    if(whereami >= 0 && whereami < size(cellbyid))
      cwt.c = cellbyid[whereami];
    else cwt.c = currentmap->gamestart();

    for(int i=0; i<size(cellbyid); i++) {
      cell *c = cellbyid[i];
      if(c->bardir != NODIR && c->bardir != NOBARRIERS) 
        extendBarrier(c);
      }

    for(int d=BARLEV-1; d>=0; d--)
    for(int i=0; i<size(cellbyid); i++) {
      cell *c = cellbyid[i];
      if(c->mpdist <= d) 
        for(int j=0; j<c->type; j++) {
          cell *c2 = createMov(c, j);
          setdist(c2, d+1, c);
          }
      }

    cellbyid.clear();
    relspin.clear();

    if(shmup::on) shmup::init();

    timerstart = time(NULL); turncount = 0; 
    sagephase = 0; hardcoreAt = 0;
    timerstopped = false;
    savecount = 0; savetime = 0;
    cheater = 1;
    
    if(vernum >= 7400) while(true) {
      int i = loadInt();
      if(i == -1) break;
      int j = loadInt(), l = loadInt();
      if(i<0 || i >= USERSHAPEGROUPS) break;
      if(j<0 || j >= USERSHAPEIDS) break;
      if(l<0 || l >= USERLAYERS) break;

      initShape(i, j);
      usershapelayer& ds(usershapes[i][j]->d[l]);
      
      load(ds.sym); load(ds.rots); load(ds.color);
      ds.list.clear();
      int siz = loadInt();
      
      ds.shift = loadPoint();
      ds.spin = loadPoint();
      
      for(int i=0; i<siz; i++)
        ds.list.push_back(loadPoint());
      }

    buildpolys();
    bfs();
    restartGraph();
    return true;
    }
  
  }
#endif

namespace mapeditor {

  bool drawplayer = true;
  char whichPattern = 0;
  char whichShape = 0;
  char whichCanvas = 0;

    int nopattern(cell *c) {
        if(isWarped(c) && !euclid) {
            int u = ishept(c)?1:0;
            int qhex = 0;
            for(int v=0; v<c->type; v++) if(c->mov[v] && !isWarped(c->mov[v])) {
                u += 2;
                if(!ishept(c->mov[v])) qhex++;
            }
            if(u == 2 && qhex == 1) return 8;
            if(u == 6 && qhex == 2) return 10;
            return u;
        }
        return ishept(c) ? 1 : ishex1(c) ? 2 : 0; // 0 to 1
    }
    
    bool reflectPatternAt(cell *c, char p = whichPattern) {
        if(p == 'p' && polarb50(c)) return true;
        if(p == 0 && nopattern(c) == 4) {
            int d = patterndir(c);
            return !isWarped(createMov(c, (d+1)%6));
        }
        return false;
    }
    
    int downdir(cell *c, cellfunction *cf = coastvalEdge) {
        cell *c2 = chosenDown(c, 1, 1, cf);
        if(!c2) return 0;
        return neighborId(c, c2);
    }
    
    int patterndir(cell *c, char w) {
        switch(w) {
            case 'z': {
                int t = zebra40(c);
                
                if(euclid) return (t*4) % 6;
                
                int t4 = t>>2, tcdir = 0;
                
                if(purehepta) tcdir = t^1;
                
                else if(t4 == 10) tcdir = t-20;
                else if(t4 >= 4 && t4 < 7) tcdir = 40 + (t&3);
                else if(t4 >= 1 && t4 < 4) tcdir = t+12;
                else if(t4 >= 7 && t4 < 10) tcdir = t-24;
                
                for(int i=0; i<c->type; i++) if(c->mov[i] && zebra40(c->mov[i]) == tcdir)
                    return i;
                
                // printf("fail to fintd %d -> %d\n", t, tcdir);
                
                return 0;
            }
                
            case 'f': {
                int t = emeraldval(c);
                if(euclid) return 0;
                int tcdir = 0, tbest = (t&3);
                for(int i=0; i<c->type; i++) {
                    cell *c2 = c->mov[i];
                    if(c2) {
                        int t2 = emeraldval(c2);
                        if((t&3) == (t2&3) && t2 > tbest)
                            tbest = t2, tcdir = i;
                    }
                }
                return tcdir;
            }
                
            case 'p': {
                int tcdir = -1, tbest = -1;
                int pa = polara50(c);
                int pb = polarb50(c);
                for(int i=0; i<c->type; i++) {
                    cell *c2 = c->mov[i];
                    if(c2 && polara50(c2) == pa && polarb50(c2) == pb) {
                        int t2 = fiftyval049(c2);
                        if(t2 > tbest) tbest = t2, tcdir = i;
                    }
                }
                return tcdir;
            }
                
            case 'H':
                return downdir(c);
                
            case 0: {
                if(euclid) return 0;
                int u = nopattern(c);
                
                if(u == 6)
                    for(int i=1; i<c->type; i+=2) if(!isWarped(createMov(c,i)))
                        return i;
                
                if(u == 2 || u == 3 || u == 8)
                    for(int i=0; i<c->type; i++) if(!isWarped(createMov(c,i)))
                        return i;
                
                if(u == 4 || u == 10)
                    for(int i=0; i<c->type; i+=2) if(!isWarped(createMov(c,i)))
                        return i;
                
                if(u == 6)
                    for(int i=1; i<c->type; i+=2) if(!isWarped(createMov(c,i))) 
                        return i;
                
                if(u == 5)
                    for(int i=0; i<c->type; i++) if(!isWarped(createMov(c,(i+3)%7)) && !isWarped(createMov(c,(i+4)%7))) 
                        return i;
                
                if(u == 9)
                    for(int i=0; i<c->type; i++) if(!isWarped(createMov(c,(i+2)%7)) && !isWarped(createMov(c,(i+5)%7))) 
                        return i;
                
                if(u == 7)
                    for(int i=0; i<c->type; i++) if(!isWarped(createMov(c,(i+1)%7)) && !isWarped(createMov(c,(i+6)%7))) 
                        return i;
            }
        }
        return 0;
    }
    
  string infix;
  
  bool hasInfix(const string &s) {
    if(infix == "") return true;
    string t = "";
    for(int i=0; i<size(s); i++) {
      char c = s[i];
      char tt = 0;
      if(c >= 'a' && c <= 'z') tt += c - 32;
      else if(c >= 'A' && c <= 'Z') tt += c;
      else if(c == '@') tt += c;
      if(tt) t += tt;
      }
    return t.find(infix) != string::npos;
    }
  
  bool editInfix(int uni) {
    if(uni >= 'A' && uni <= 'Z') infix += uni;
    else if(uni >= 'a' && uni <= 'z') infix += uni-32;
    else if(infix != "" && uni == 8) infix = infix.substr(0, size(infix)-1);
    else if(infix != "" && uni != 0) infix = "";
    else return false;
    return true;
    }
    
  cell *drawcell;

#if CAP_EDIT
  int paintwhat = 0;
  int painttype = 0;
  int radius = 0;
  string paintwhat_str = "clear monster";
  cell *copywhat = NULL; int copydir; bool copyflip;
  
  bool symRotation, sym01, sym02, sym03;
  int whichpart;
    
  const char *mapeditorhelp = 
    "This mode allows you to edit the map.\n\n"
    "NOTE: Use at your own risk. Combinations which never "
    "appear in the real game may work in an undefined way "
    "(do not work, look strangely, give strange messages, or crash the game).\n\n"
    "To get the most of this editor, "
    "some knowledge of inner workings of HyperRogue is required. "
    "Each cell has four main fields: land type, wall type, monster type, item type. "
    "The same wall type (especially \"none\", \"sea\", or \"bonfire\") may look or "
    "work a bit differently, based on the land it is in. Sometimes an object may "
    "appear twice on the list due to subtle differences (for example, Demons could "
    "move next turn or not).\n\n"
    "Press w, i, l, or m to choose which aspect of cells to change, "
    "then just click on the cells and they will change. Press 'c' while "
    "hovering over a cell to copy that cell, this copies all information about it. "
    "When copying large areas or placing multi-tile monsters, it might be important where "
    "on the cell you are clicking.\n\n"
    "You can also press 0-9 to apply your changes to a greater radius. "
    "This also affects the copy/paste feature, allowing to copy a larger area.\n\n"
    "Press F2 to save the current map (and F3 to load it). If you try this after "
    "a long game of HyperRogue (without using Orbs of Safety), the filesize will "
    "be very large! "
    "Note however that large structures, such as "
    "Great Walls, large circles and horocycles, are destroyed by this.\n\n"
    "Press 'b' to mark cells as boundaries. Such cells, and cells beyond "
    "them, are not copied by the copy/paste feature, nor saved by the "
    "save feature.\n\n";

  const char* patthelp = 
   "Press 'r' to choose a regular pattern. When a pattern is on, "
   "editing a cell automatically edits all cells which are "
   "equivalent according to this pattern. You can choose from "
   "several patterns, and choose which symmetries matter "
   "for equivalence. Also, you can press Space to switch between "
   "the map and graphics editor quickly -- note that editing floors "
   "with the graphics editor also adheres to the pattern.";
  
  string mehelptext() {
    return XLAT(mapeditorhelp) + XLAT(patthelp);
    }

  vector<pair<string, int> > v;
  
  struct undo_info {
    cell *c;
    eWall w;
    eItem i;
    eMonster m;
    eLand l;
    char wparam;
    int32_t lparam;
    char dir;
    };
  
  vector<undo_info> undo;
  
  bool checkEq(undo_info& u) {
    return u.w == u.c->wall && u.i == u.c->item && u.m == u.c->monst && u.l == u.c->land &&
      u.dir == u.c->mondir && u.lparam == u.c->landparam && u.wparam == u.c->wparam;
    }
  
  void saveUndo(cell *c) {
    undo_info u;
    u.c=c; u.w = c->wall; u.i = c->item; u.m = c->monst; u.l = c->land; u.dir = c->mondir;
    u.wparam = c->wparam; u.lparam = c->landparam;
    undo.push_back(u);
    }

  undo_info& lastUndo() { return undo[size(undo)-1]; }
  
  void undoLock() {
    if(!size(undo) || lastUndo().c) {
      undo_info i; i.c = NULL; undo.push_back(i);
      }
    }

  void applyUndo() {
    while(size(undo) && !lastUndo().c) undo.pop_back();
    while(size(undo)) {
      undo_info& i(lastUndo());
      if(!i.c) break;
      i.c->wall = i.w;
      i.c->item = i.i;
      i.c->monst = i.m;
      i.c->land = i.l;
      i.c->mondir = i.dir;
      i.c->wparam = i.wparam; 
      i.c->landparam = i.lparam;
      undo.pop_back();
      }
    }
  
  void checkUndo() {
    if(checkEq(undo[size(undo)-1])) undo.pop_back();
    }
  
  int itc(int k) {
    if(k == 0) return 0;
    if(k == 1) return 0x40;
    if(k == 2) return 0x80;
    if(k == 3) return 0xFF;
    return 0x20;
    }

  bool choosefile = false;
  bool editext = false;

  #define CDIR 0xC0C0C0
  #define CFILE forecolor
  
  bool filecmp(const pair<string,int> &f1, const pair<string,int> &f2) {
    if(f1.first == "../") return true;
    if(f2.first == "../") return false;
    if(f1.second != f2.second)
      return f1.second == CDIR;
    return f1.first < f2.first;
    }
  
  string filecaption, cfileext;
  string *cfileptr;
  
  void drawFileDialog() {
    displayfr(vid.xres/2, 30 + vid.fsize, 2, vid.fsize, 
      filecaption, forecolor, 8);
      
    string& cfile = *cfileptr;

    displayfr(vid.xres/2, 34 + vid.fsize * 2, 2, vid.fsize, 
      cfile, editext ? 0xFF00FF : 0xFFFF00, 8);
    
    displayButton(vid.xres*1/5, 38+vid.fsize * 3, 
      "F2 = save", 2000+SDLK_F2, 8);
    displayButton(vid.xres*2/5, 38+vid.fsize * 3, 
      "F3 = load", 2000+SDLK_F3, 8);
    displayButton(vid.xres*3/5, 38+vid.fsize * 3, 
      "F4 = extension", 2000+SDLK_F4, 8);
    displayButton(vid.xres*4/5, 38+vid.fsize * 3, 
      "Enter = back", 2000+SDLK_RETURN, 8);

    v.clear();
    
    DIR           *d;
    struct dirent *dir;
    
    string where = ".";
    for(int i=0; i<size(cfile); i++)
      if(cfile[i] == '/' || cfile[i] == '\\')
        where = cfile.substr(0, i+1);
    
    d = opendir(where.c_str());
    if (d) {
      while ((dir = readdir(d)) != NULL) {
        string s = dir->d_name;
        if(s != ".." && s[0] == '.') ;
        else if(size(s) > 4 && s.substr(size(s)-4) == cfileext)
          v.push_back(make_pair(s, CFILE));
        else if(dir->d_type & DT_DIR)
          v.push_back(make_pair(s+"/", CDIR));
        }
      closedir(d);
      }
    sort(v.begin(), v.end(), filecmp);

    int q = v.size();
    int percolumn = (vid.yres-38) / (vid.fsize+5) - 4;
    int columns = 1 + (q-1) / percolumn;
      
    for(int i=0; i<q; i++) {
      int x = 16 + (vid.xres * (i/percolumn)) / columns;
      int y = 42 + vid.fsize * 4 + (vid.fsize+5) * (i % percolumn);
        
      displayColorButton(x, y, v[i].first, 1000 + i, 0, 0, v[i].second, 0xFFFF00);
      }

    keyhandler = handleKeyFile;
    }
  
  void displayFunctionKeys() {
    int fs = min(vid.fsize + 5, vid.yres/26);
    displayButton(8, vid.yres-8-fs*11, XLAT("F1 = help"), SDLK_F1, 0);
    displayButton(8, vid.yres-8-fs*10, XLAT("F2 = save"), SDLK_F2, 0);
    displayButton(8, vid.yres-8-fs*9, XLAT("F3 = load"), SDLK_F3, 0);
    displayButton(8, vid.yres-8-fs*8, XLAT("F4 = file"), SDLK_F3, 0);
    displayButton(8, vid.yres-8-fs*7, XLAT("F5 = restart"), SDLK_F5, 0);
    displayButton(8, vid.yres-8-fs*6, XLAT("F6 = HQ shot"), SDLK_F6, 0);
    displayButton(8, vid.yres-8-fs*5, XLAT("F7 = player on/off"), SDLK_F7, 0);
#if CAP_SVG
    displayButton(8, vid.yres-8-fs*4, XLAT("F8 = SVG shot"), SDLK_F8, 0);
#endif
    displayButton(8, vid.yres-8-fs*3, XLAT("SPACE = map/graphics"), ' ', 0);
    displayButton(8, vid.yres-8-fs*2, XLAT("ESC = return to the game"), SDLK_ESCAPE, 0);
    }

  void vpush(int i, const char *name) {
    string s = XLATN(name);
    if(!hasInfix(s)) return;
    v.push_back(make_pair(s, i));
    }
  
  void showPrePattern() {
    dialog::init("predesigned patterns");
    dialog::addItem(XLAT("Gameboard"), 'g');
    dialog::addItem(XLAT("random colors"), 'r');
    dialog::addItem(XLAT("rainbow landscape"), 'l');
    dialog::addItem(XLAT("dark rainbow landscape"), 'd');
    dialog::addItem(XLAT("football"), 'F');

    dialog::addSelItem(XLAT("emerald pattern"), "emerald", 'e');

    dialog::addSelItem(XLAT("four elements"), "palace", 'b');
    dialog::addSelItem(XLAT("eight domains"), "palace", 'a');

    dialog::addSelItem(XLAT("zebra pattern"), "zebra", 'z');
    dialog::addSelItem(XLAT("four triangles"), "zebra", 't');
    dialog::addSelItem(XLAT("three stripes"), "zebra", 'x');

    dialog::addSelItem(XLAT("random black-and-white"), "current", 'w');

    dialog::addSelItem(XLAT("field pattern C"), "field", 'C');
    dialog::addSelItem(XLAT("field pattern D"), "field", 'D');
    dialog::addSelItem(XLAT("field pattern N"), "field", 'N');
    dialog::addSelItem(XLAT("field pattern S"), "field", 'S');

    dialog::display();
    
    keyhandler = [] (int sym, int uni) {
      dialog::handleNavigation(sym, uni);
      if((uni >= 'a' && uni <= 'z') || (uni >= 'A' && uni <= 'Z')) {
        whichCanvas = uni;
        subcanvas = rand();
        firstland = laCanvas; randomPatternsMode = false;
        restartGame();
        }
      else if(doexiton(sym, uni)) popScreen();
      };    
    }
  
  void showPattern() {
    dialog::init();

    dialog::addBoolItem(XLAT(euclid ? "three colors" : "Emerald Pattern"), (whichPattern == 'f'), 'f');
    dialog::addBoolItem(XLAT("Palace Pattern"), (whichPattern == 'p'), 'p');
    dialog::addBoolItem(XLAT(euclid ? "three colors rotated" : "Zebra Pattern"), (whichPattern == 'z'), 'z');
    dialog::addBoolItem(XLAT("field pattern"), (whichPattern == 'F'), 'F');

    if(whichPattern == 'f') symRotation = true;
    if(whichPattern == 'F') ;
    else if(!euclid) {
      dialog::addBoolItem(XLAT("rotational symmetry"), (symRotation), '0');
      dialog::addBoolItem(XLAT("symmetry 0-1"), (sym01), '1');
      dialog::addBoolItem(XLAT("symmetry 0-2"), (sym02), '2');
      dialog::addBoolItem(XLAT("symmetry 0-3"), (sym03), '3');
      }
    else
      dialog::addBoolItem(XLAT("edit all three colors"), (symRotation), '0');

    dialog::addBoolItem(XLAT("display pattern codes (full)"), (displaycodes == 1), 'd');
    dialog::addBoolItem(XLAT("display pattern codes (simplified)"), (displaycodes == 2), 's');

    dialog::addBoolItem(XLAT("display only hexagons"), (whichShape == '6'), '6');
    dialog::addBoolItem(XLAT("display only heptagons"), (whichShape == '7'), '7');
    dialog::addBoolItem(XLAT("display the triheptagonal grid"), (whichShape == '8'), '8');
    dialog::addItem(XLAT("line patterns"), 'l');

    dialog::addItem(XLAT("predesigned patterns"), 'r');
    dialog::display();
    
    keyhandler = [] (int sym, int uni) {
      dialog::handleNavigation(sym, uni);
      if(uni == 'f' || uni == 'p' || uni == 'z' || uni == 'H' || uni == 'F') {
        if(whichPattern == uni) whichPattern = 0;
        else whichPattern = uni;
        modelcell.clear();
        }
      
      else if(uni == '0') symRotation = !symRotation;
      else if(uni == '1') sym01 = !sym01;
      else if(uni == '2') sym02 = !sym02;
      else if(uni == '3') sym03 = !sym03;
      else if(uni == '6' || uni == '7' || uni == '8') {
        if(whichShape == uni) whichShape = 0;
        else whichShape = uni;
        }
      else if(uni == '3') sym03 = !sym03;
      else if(uni == 'd') displaycodes = displaycodes == 1 ? 0 : 1;
      else if(uni == 's') displaycodes = displaycodes == 2 ? 0 : 2;
      
      else if(uni == 'l')
        pushScreen(linepatterns::showMenu);

      else if(uni == 'r') pushScreen(showPrePattern);
      
      else if(doexiton(sym, uni)) popScreen();
      };
    }
  
  void showList() {
    v.clear();
    if(painttype == 4) painttype = 0;
    switch(painttype) {
      case 0: 
        for(int i=0; i<motypes; i++) {
          eMonster m = eMonster(i);
          if(
            m == moTongue || m == moPlayer || m == moFireball || m == moBullet ||
            m == moFlailBullet || m == moShadow || m == moAirball ||
            m == moWolfMoved || m == moGolemMoved ||
            m == moTameBomberbirdMoved || m == moKnightMoved ||
            m == moDeadBug || m == moLightningBolt || m == moDeadBird ||
            m == moMouseMoved || m == moPrincessMoved || m == moPrincessArmedMoved) ;
          else vpush(i, minf[i].name);
          }
        break;
      case 1:
        for(int i=0; i<ittypes; i++) vpush(i, iinf[i].name);
        break;
      case 2:
        for(int i=0; i<landtypes; i++) vpush(i, linf[i].name);
        break;
      case 3:
        for(int i=0; i<walltypes; i++) if(i != waChasmD) vpush(i, winf[i].name);
        break;
      }
    // sort(v.begin(), v.end());
    
    if(infix != "") mouseovers = infix;
    
    int q = v.size();
    int percolumn = vid.yres / (vid.fsize+5) - 4;
    int columns = 1 + (q-1) / percolumn;
    
    for(int i=0; i<q; i++) {
      int x = 16 + (vid.xres * (i/percolumn)) / columns;
      int y = (vid.fsize+5) * (i % percolumn) + vid.fsize*2;
      
      int actkey = 1000 + i;
      string vv = v[i].first;
      if(i < 9) { vv += ": "; vv += ('1' + i); }
      
      displayButton(x, y, vv, actkey, 0);
      }
    keyhandler = [] (int sym, int uni) {
      if(uni >= '1' && uni <= '9') uni = 1000 + uni - '1';
      if(sym == SDLK_RETURN || sym == SDLK_KP_ENTER || sym == '-' || sym == SDLK_KP_MINUS) uni = 1000;
      for(int z=0; z<size(v); z++) if(1000 + z == uni) {
        paintwhat = v[z].second;
        paintwhat_str = v[z].first;
        mousepressed = false;
        popScreen();
        return;
        }
      if(editInfix(uni)) ;
      else if(doexiton(sym, uni)) popScreen();
      };    
    }
  
  void showMapEditor() {
    cmode = sm::MAP;
    gamescreen(0);
  
    int fs = min(vid.fsize + 5, vid.yres/26);
    
    getcstat = '-';

    displayfr(8, 8 + fs, 2, vid.fsize, paintwhat_str, forecolor, 0);
    displayfr(8, 8+fs*2, 2, vid.fsize, XLAT("use at your own risk!"), 0x800000, 0);

    displayButton(8, 8+fs*4, XLAT("0-9 = radius (%1)", its(radius)), ('0' + (radius+1)%10), 0);
    displayButton(8, 8+fs*5, XLAT("b = boundary"), 'b', 0);
    displayButton(8, 8+fs*6, XLAT("m = monsters"), 'm', 0);
    displayButton(8, 8+fs*7, XLAT("w = walls"), 'w', 0);
    displayButton(8, 8+fs*8, XLAT("i = items"), 'i', 0);
    displayButton(8, 8+fs*9, XLAT("l = lands"), 'l', 0);
    displayfr(8, 8+fs*10, 2, vid.fsize, XLAT("c = copy"), 0xC0C0C0, 0);
    displayButton(8, 8+fs*11, XLAT("u = undo"), 'u', 0);
    if(painttype == 4)
      displayButton(8, 8+fs*12, XLAT("f = flip %1", ONOFF(copyflip)), 'u', 0);
    displayButton(8, 8+fs*13, XLAT("r = regular"), 'r', 0);
    displayButton(8, 8+fs*14, XLAT("p = paint"), 'p', 0);
      
    displayFunctionKeys();
    
    keyhandler = handleKeyMap;
    }
  
  int spillinc() {
    if(radius>=2) return 0;
    if(anyshiftclick) return -1;
    return 1;
    }

  void spillCopy(cell *c1, int d1, cell *c2, int d2, int r) {
    saveUndo(c1);
    int cf = copyflip ? -1 : 1;
    if(c2->land == laNone) return;
    c1->wall = c2->wall;
    c1->item = c2->item;
    c1->land = c2->land;
    c1->monst = c2->monst;
    c1->landparam = c2->landparam;
    c1->wparam = c2->wparam;
    // c1->tmp = c2->tmp;
    if(c2->mondir == NODIR) c1->mondir = NODIR;
    else c1->mondir = (cf * (c2->mondir - d2) + d1 + 42) % c1->type;
    checkUndo();
    
    if(r) for(int i=0; i<c1->type; i++) {
      if(c1 != mouseover && (i == 0 || i == 1 || i == c1->type-1)) continue;
      createMov(c1, i);
      int i0 = (42+cf*i+d1) % c1->type;
      int i1 = (i + d2) % c2->type;
      spillCopy(c1->mov[i0], c1->spn(i0), c2->mov[i1], c2->spn(i1), r-1);
      }
    }
  
  int subpatternEmerald(int i) {
    if(euclid) return (symRotation && (i<3)) ? 0 : i;
    if((sym01?1:0)+(sym02?1:0)+(sym03?1:0) >= 2) i &= ~3;
    if(sym01 && (i&1)) i ^= 1;
    if(sym02 && (i&2)) i ^= 2;
    if(sym03 && (i&2)) i ^= 3;
    return i;
    }
  
  int subpatternZebra(int i) {
    if(euclid) return (symRotation && (i<3)) ? 0 : i;
    i = subpatternEmerald(i);
    if(symRotation) {
      if(i >= 8 && i < 12) i -= 4;
      if(i >= 12 && i < 16) i -= 8;
      if(i >= 20 && i < 24) i -= 4;
      if(i >= 24 && i < 28) i -= 8;
      if(i >= 32 && i < 36) i -= 4;
      if(i >= 36 && i < 40) i -= 8;
      }
    return i;
    }

  int subpatternPalace(int i) {
    if(euclid) return i;
    i = subpatternEmerald(i);
    if(symRotation && i >= 3) i -= ((i/4-1) % 7) * 4;
    return i;
    }
  
  int subpattern(cell *c) {
    switch(whichPattern) { 
      case 'z': 
        return subpatternZebra(zebra40(c)); // 4 to 43
      case 'f':
        return subpatternEmerald(emeraldval(c)); // 44 to 99
      case 'p': {
        int i = fiftyval049(c);
        i *= 4;
        if(polara50(c)) i|=1;
        if(polarb50(c)) i|=2;
        return subpatternPalace(i);
        }
      case 'P':
        return fiftyval(c);
      case 'H':
        return realpattern(c);
      case 'F':
        return realpattern(c);
      }
    return nopattern(c);
    }

  int realpattern(cell *c) {
    switch(whichPattern) { 
      case 'z': 
        return zebra40(c); // 4 to 43
      case 'f':
        return emeraldval(c); // 44 to 99
      case 'p': {
        int i = fiftyval049(c);
        i *= 4;
        if(polara50(c)) i|=1;
        if(polarb50(c)) i|=2;
        return i;
        }
      case 'H': 
        return towerval(c);
      case 'F': {
        pair<int, bool> p = fieldpattern::fieldval(c);
        return 10*p.first + (p.second?6:7);
        }
      }
    return nopattern(c);
    }

  int realpatternsh(cell *c) {
    if(whichPattern == 'F') return nopattern(c);
    else return realpattern(c);
    }

  int cellShapeGroup() {
    if(whichPattern == 'f') return 4;
    if(whichPattern == 'p') return 5;
    if(whichPattern == 'z') return 6;
    if(whichPattern == 'H') return 7;
    return 3;
    }
  
  int drawcellShapeGroup() {
    if(drawcell == cwt.c) return 0;
    if(drawcell->monst) return 1;
    if(drawcell->item) return 2;
    return cellShapeGroup();
    }
  
  int drawcellShapeID() {
    if(drawcell == cwt.c) return vid.cs.charid;
    if(drawcell->monst) return drawcell->monst;
    if(drawcell->item) return drawcell->item;
    return subpattern(drawcell);
    }

  int subpatternShape(int i) {
    if(whichPattern == 'z') return subpatternZebra(i);
    if(whichPattern == 'f') return subpatternEmerald(i);
    if(whichPattern == 'p') return subpatternPalace(i);
    return i;
    }
  
  bool editingShape(int group, int id) {
    if(group != mapeditor::drawcellShapeGroup()) return false;
    if(group < 3) return id == drawcellShapeID();
    return subpatternShape(id) == subpattern(drawcell);
    }
  
  void spill(cell *c, int r, int cdir) {

    if(painttype == 4 && radius) {
      if(mouseover->type != copywhat->type) return;
      if(cdir<0) cdir=0;
      if(mouseover->type == 6 && ((cdir^copydir)&1)) {
        cdir++; cdir %= 6;
        }
      spillCopy(mouseover, cdir, copywhat, copydir, radius);
      }

    saveUndo(c);
    switch(painttype) {
      case 0:
        c->monst = eMonster(paintwhat);
        c->hitpoints = 3;
        c->stuntime = 0;
        c->mondir = cdir;
        
        if((isWorm(c) || isIvy(c) || isMutantIvy(c)) && c->mov[cdir] && 
          !isWorm(c->mov[cdir]) && !isIvy(c->mov[cdir]))
          c->mondir = NODIR;
        break;
      case 1:
        c->item = eItem(paintwhat);
        if(c->item == itBabyTortoise)
          tortoise::babymap[c] = getBits(c) ^ tortoise::getRandomBits();
        break;
      case 2: {
        eLand last = c->land;
        c->land = eLand(paintwhat);
        if(isIcyLand(c) && isIcyLand(last))
           HEAT(c) += spillinc() / 100.;
        else if(last == laDryForest && c->land == laDryForest)
          c->landparam += spillinc();
        else if(last == laOcean && c->land == laOcean)
          c->landparam += spillinc();
        else if(last == laHive && c->land == laHive)
          c->landparam += spillinc();
        else
          c->landparam = 0;
        break;
        }
      case 3: {
        eWall last = c->wall;
        c->wall = eWall(paintwhat);
        
        if(last != c->wall) {
          if(hasTimeout(c))
            c->wparam = 10;
          else if(c->wall == waWaxWall)
            c->landparam = hrand(0xFFFFFF + 1);
          }
        else if(hasTimeout(c))
          c->wparam += spillinc();
        break;
        }
      case 5:
        c->land = laNone;
        c->wall = waNone;
        c->item = itNone;
        c->monst = moNone;
        c->landparam = 0;
        // c->tmp = -1;
        break;
      case 6:
        c->land = laCanvas;
        c->wall = waNone;
        c->landparam = paintwhat >> 8;
        break;
      case 4:
        c->wall = copywhat->wall;
        c->item = copywhat->item;
        c->land = copywhat->land;
        c->monst = copywhat->monst;
        c->landparam = copywhat->landparam;
        c->wparam = copywhat->wparam;
        if(copywhat->mondir == NODIR) c->mondir = NODIR;
        else if(c->mondir == c->type-1) c->mondir = 0;
        else c->mondir++;
        break;
      }
    checkUndo();
    if(r) for(int i=0; i<c->type; i++) spill(createMov(c, i), r-1, c->spn(i));
    }
  
  void allInPattern(cell *c, int r, int cdir) {

    if(!whichPattern) {
      spill(c, r, cdir);
      return;
      }

    vector<cell*> v;
    v.push_back(c);
    sval++;
    c->aitmp = sval;
    
    cdir = cdir - patterndir(c);
    int sp = subpattern(c);
    
    int at = 0;
    while(at < size(v)) {
      cell *c2 = v[at];
      at++;
      
      if(subpattern(c2) == sp) {
        spill(c2, r, fixdir(cdir + patterndir(c2), c));
        modelcell[realpattern(c2)] = c2;
        }
      for(int i=0; i<c2->type; i++) {
        cell *c3 = c2->mov[i];
        if(c3 && !eq(c3->aitmp, sval))
          c3->aitmp = sval, v.push_back(c3);
        }
      }
    }
  
  bool handleKeyFile(int uni, int sym) {
    string& s(*cfileptr);
    int i = size(s) - (editext?0:4);
    if(uni > 2000) sym = uni - 2000;
    if(sym == SDLK_RETURN || sym == SDLK_KP_ENTER || sym == SDLK_ESCAPE) {
      popScreen();
      return true;
      }
    else if(sym == SDLK_F2 || sym == SDLK_F3) {
      popScreen();
      return false;
      }
    else if(sym == SDLK_F4) {
      editext = !editext;
      }
    else if(sym == SDLK_BACKSPACE && i) {
      s.erase(i-1, 1);
      }
    else if(uni >= 32 && uni < 127) {
      s.insert(i, s0 + char(uni));
      }
    else if(uni >= 1000 && uni <= 1000+size(v)) {
      string where = "", what = s, whereparent = "../";
      for(int i=0; i<size(s); i++)
        if(s[i] == '/') {
          if(i >= 2 && s.substr(i-2,3) == "../")
            whereparent = s.substr(0, i+1) + "../";
          else
            whereparent = where;
          where = s.substr(0, i+1), what = s.substr(i+1);
          }
      int i = uni - 1000;
      if(v[i].first == "../") {
        s = whereparent + what;
        }
      else if(v[i].second == CDIR)
        s = where + v[i].first + what;
      else
        s = where + v[i].first;
      }
    return true;
    }
  
  void handleKeyMap(int sym, int uni) {
    handlePanning(sym, uni);

    // left-clicks are coded with '-', and right-clicks are coded with sym F1
    if(uni == '-') undoLock();
    if(uni == '-' && mouseover)
      allInPattern(mouseover, radius, neighborId(mouseover, mouseover2));
    
    if(mouseover) for(int i=0; i<mouseover->type; i++) createMov(mouseover, i);
    if(uni == 'u') applyUndo();
    else if(uni == 'v' || sym == SDLK_F10 || sym == SDLK_ESCAPE) popScreen();
    else if(uni >= '0' && uni <= '9') radius = uni - '0';
    else if(uni == 'm') pushScreen(showList), painttype = 0, infix = "";
    else if(uni == 'i') pushScreen(showList), painttype = 1, infix = "";
    else if(uni == 'l') pushScreen(showList), painttype = 2, infix = "";
    else if(uni == 'w') pushScreen(showList), painttype = 3, infix = "";
    else if(uni == 'r') pushScreen(showPattern);
    else if(uni == 't' && mouseover) {
      cwt.c = mouseover; playermoved = true;
      cwt.spin = neighborId(mouseover, mouseover2);
      }
    else if(uni == 'b') painttype = 5, paintwhat_str = XLAT("boundary");
    else if(uni == 'p') {
      painttype = 6;
      paintwhat_str = "paint";
      dialog::openColorDialog((unsigned&)(paintwhat = (painttype ==6 ? paintwhat : 0x808080)));
      }
    else if(sym == SDLK_F2) {
      if(mapstream::saveMap(levelfile.c_str()))
        addMessage(XLAT("Map saved to %1", levelfile));
      else
        addMessage(XLAT("Failed to save map to %1", levelfile));
      }
    else if(sym == SDLK_F5) {
      restartGame();
      }
    else if(sym == SDLK_F3) {
      if(mapstream::loadMap(levelfile.c_str()))
        addMessage(XLAT("Map loaded from %1", levelfile));
      else
        addMessage(XLAT("Failed to load map from %1", levelfile));
      }
    else if(sym == SDLK_F4) {
      cfileptr = &levelfile;
      filecaption = XLAT("level to save/load:");
      cfileext = ".lev";
      pushScreen(drawFileDialog);
      }
#if CAP_SDL    
    else if(sym == SDLK_F6) {
      saveHighQualityShot();
      }
#endif
#if CAP_SVG
    else if(sym == SDLK_F8) {
      svg::render();
      }
#endif
    else if(sym == SDLK_F7) {
      drawplayer = !drawplayer;
      }
    else if(uni == 'c') {
      if(mouseover && mouseover2)
        copydir = neighborId(mouseover, mouseover2);
      if(copydir<0) copydir = 0;
      copyflip = (uni == 'f');
      copywhat = mouseover, painttype = 4;
      paintwhat_str = XLAT("copying");
      }
    else if(uni == 'f') {
      copyflip = !copyflip;
      }
    else if(uni == 'h' || sym == SDLK_F1) 
      gotoHelp(mehelptext());
    else if(uni == ' ') {
      popScreen();
      pushScreen(showDrawEditor);
      initdraw(mouseover ? mouseover : cwt.c);
      }
    }

// VECTOR GRAPHICS EDITOR

  const char* drawhelp = 
   "In this mode you can draw your own player characters, "
   "floors, monsters, and items. Press 'e' while hovering over "
   "an object to edit it. Start drawing shapes with 'n', and "
   "add extra vertices with 'a'. Press 0-9 to draw symmetric "
   "pictures easily. More complex pictures can "
   "be created by using several layers ('l'). See the edges of "
   "the screen for more keys.";

  string drawhelptext() { 
    return XLAT(drawhelp);
    }
      
  int dslayer;
  bool coloring;
  unsigned int colortouse = 0xC0C0C0FFu;

  transmatrix drawtrans, drawtransnew;

  void loadShape(int sg, int id, hpcshape& sh, int d, int layer) {
    initShape(sg, id);
    usershapelayer *dsCur = &usershapes[sg][id]->d[layer];
    dsCur->list.clear();
    dsCur->sym = d==2;
    for(int i=sh.s; i < sh.s + (sh.e-sh.s)/d; i++)
      dsCur->list.push_back(hpc[i]);
    }

  void drawGhosts(cell *c, const transmatrix& V, int ct) {
    }

  hyperpoint ccenter = C0;
  hyperpoint coldcenter = C0;
  
  void drawGrid() {
    for(int d=0; d<84; d++) {
      transmatrix d2 = drawtrans * rgpushxto0(ccenter);
      int lalpha;
      if(d % (84/drawcell->type) == 0)
        lalpha = 0x40;
      else
        lalpha = 0x20;
      int col = darkena(0xC0C0C0, 0, lalpha);
      queueline(d2 * C0, d2 * spin(M_PI*d/42)* xpush(1) * C0, col);
      for(int u=2; u<=20; u++) {
        if(u % 5 == 0) lalpha = 0x40;
        else lalpha = 0x20;
        queueline(
          d2 * spin(M_PI*d/42)* xpush(u/20.) * C0, 
          d2 * spin(M_PI*(d+1)/42)* xpush(u/20.) * C0, 
          darkena(0xC0C0C0, 0, lalpha));
        }
      }
    queueline(drawtrans*ccenter, drawtrans*coldcenter, darkena(0xC0C0C0, 0, 0x20));
    }

  void drawHandleKey(int sym, int uni);

  void showDrawEditor() {
    cmode = sm::DRAW;
    gamescreen(0);
    drawGrid();

    if(!mouseout()) getcstat = '-';

    int sg = drawcellShapeGroup();
    
    string line1, line2;
    
    switch(sg) {
      case 0:
        line1 = XLAT("character");
        line2 = csname(vid.cs);
        break;
      
      case 1:
        line1 = XLAT("monster");
        line2 = XLAT1(minf[drawcell->monst].name);
        break;
        
      case 2:
        line1 = XLAT("item");
        line2 = XLAT1(iinf[drawcell->item].name);
        break;
      
      case 3:
        line1 = XLAT("floor");
        line2 = XLAT(ishept(drawcell) ? "heptagonal" : 
          ishex1(drawcell) ? "hexagonal #1" : "hexagonal");
        break;
      
      default:
        line1 = XLAT("floor/pattern");
        line2 = "#" + its(subpattern(cwt.c));
        break;
      }
    
    usershape *us =usershapes[drawcellShapeGroup()][drawcellShapeID()];
    
    int fs = min(vid.fsize + 5, vid.yres/28);

    // displayButton(8, 8+fs*9, XLAT("l = lands"), 'l', 0);
    displayfr(8, 8+fs, 2, vid.fsize, line1, 0xC0C0C0, 0);
    displayfr(8, 8+fs*2, 2, vid.fsize, line2, 0xC0C0C0, 0);
    displayButton(8, 8+fs*3, XLAT("l = layers: %1", its(dslayer)), 'l', 0);
    if(us && size(us->d[dslayer].list)) {
      usershapelayer& ds(us->d[dslayer]);
      displayButton(8, 8+fs*4, XLAT("1-9 = rotations: %1", its(ds.rots)), '1' + (ds.rots % 9), 0);
      displayButton(8, 8+fs*5, XLAT(ds.sym ? "0 = symmetry" : "0 = asymmetry"), '0', 0);

      displayfr(8, 8+fs*7, 2, vid.fsize, XLAT("%1 vertices", its(size(ds.list))), 0xC0C0C0, 0);
      displaymm('a', 8, 8+fs*8, 2, vid.fsize, XLAT("a = add v"), 0);
      if(autochoose) {
        displaymm('m', 8, 8+fs*9, 2, vid.fsize, XLAT("m = move v"), 0);
        displaymm('d', 8, 8+fs*10, 2, vid.fsize, XLAT("d = delete v"), 0);
        }
      else {
        displayButton(8, 8+fs*9, XLAT("m = move v"), 'm', 0);
        displayButton(8, 8+fs*10, XLAT("d = delete v"), 'd', 0);
        }
      displaymm('c', 8, 8+fs*11, 2, vid.fsize, XLAT(autochoose ? "autochoose" : "c = choose"), 0);
      displayButton(8, 8+fs*12, XLAT("b = switch auto"), 'b', 0);

      displayfr(8, 8+fs*14, 2, vid.fsize, XLAT("t = shift"), 0xC0C0C0, 0);
      displayfr(8, 8+fs*15, 2, vid.fsize, XLAT("y = spin"), 0xC0C0C0, 0);
      displayButton(8, 8+fs*16, XLAT("p = paint"), 'p', 0);

      }
    else {
      displaymm('n', 8, 8+fs*5, 2, vid.fsize, XLAT("'n' to start"), 0);
      if(mousekey == 'a' || mousekey == 'd' || mousekey == 'd' ||
        mousekey == 'c') mousekey = 'n';
      }

    displaymm('g', vid.xres-8, 8+fs*4, 2, vid.fsize, XLAT("g = grid"), 16);
    displayButton(vid.xres-8, 8+fs*3, XLAT("z = zoom in"), 'z', 16);
    displayButton(vid.xres-8, 8+fs*2, XLAT("o = zoom out"), 'o', 16);
    displaymm('e', vid.xres-8, 8+fs, 2, vid.fsize, XLAT("e = edit this"), 16);

    if(!mouseout()) {
      hyperpoint mh = inverse(drawtrans * rgpushxto0(ccenter)) * mouseh;
      displayfr(vid.xres-8, vid.yres-8-fs*6, 2, vid.fsize, XLAT("x: %1", fts4(mh[0])), 0xC0C0C0, 16);
      displayfr(vid.xres-8, vid.yres-8-fs*5, 2, vid.fsize, XLAT("y: %1", fts4(mh[1])), 0xC0C0C0, 16);
      displayfr(vid.xres-8, vid.yres-8-fs*4, 2, vid.fsize, XLAT("z: %1", fts4(mh[2])), 0xC0C0C0, 16);
      displayfr(vid.xres-8, vid.yres-8-fs*2, 2, vid.fsize, XLAT("r: %1", fts4(hdist0(mh))), 0xC0C0C0, 16);
      displayfr(vid.xres-8, vid.yres-8-fs, 2, vid.fsize, XLAT("ϕ: %1°", fts4(-atan2(mh[1], mh[0]) * 360 / 2 / M_PI)), 0xC0C0C0, 16);
      }
    
    displayFunctionKeys();
    
    keyhandler = drawHandleKey;
    }
  
  bool rebuildPolys = false;
  
  void applyToShape(int sg, int id, int uni, hyperpoint mh) {
    bool haveshape = usershapes[sg][id];
    bool xnew = false;
    
    if(uni == '-') uni = mousekey;
    
    if(!haveshape) {
      if(uni == 'n' || uni == 'u')
        initShape(sg, id);
      else if(uni >= '0' && uni <= '9') {
        initShape(sg, id);
        xnew = true;
        }
      else 
        return;
      }

    usershapelayer *dsCur = &usershapes[sg][id]->d[dslayer];

    if(uni == 'n' || xnew) {
      dsCur->list.clear();
      dsCur->list.push_back(mh);
      rebuildPolys = true;
      }

    if(uni == 'a' && haveshape) {
      mh = spin(2*M_PI*-ew.rotid/dsCur->rots) * mh;
      if(ew.symid) mh = Mirror * mh;
    
      if(ew.pointid < 0 || ew.pointid >= size(dsCur->list)) 
        ew.pointid = size(dsCur->list)-1, ew.side = 1;

      dsCur->list.insert(dsCur->list.begin()+ew.pointid+(ew.side?1:0), mh);
      if(ew.side) ew.pointid++;
      rebuildPolys = true;
      }
    
    if(uni == 'D') {
      delete usershapes[sg][id];
      usershapes[sg][id] = NULL;
      }

    if(uni == 'm' || uni == 'd') {

      int i = ew.pointid;

      if(i < 0 || i >= size(dsCur->list)) return;

      mh = spin(2*M_PI*-ew.rotid/dsCur->rots) * mh;
      if(ew.symid) mh = Mirror * mh;

      if(uni == 'm' || uni == 'M') 
        dsCur->list[i] = mh;
      if(uni == 'd' || uni == 'b') {
        dsCur->list.erase(dsCur->list.begin() + i);
        if(ew.side == 1 && ew.pointid >= i) ew.pointid--;
        if(ew.side == 0 && ew.pointid > i) ew.pointid--;
        }
      rebuildPolys = true;
      }
    
    if(uni == 'T') {
      /* loadShape(sg, id, shFemaleBody, 1, 1);
      loadShape(sg, id, shPKnife, 1, 2);
      loadShape(sg, id, shFemaleDress, 1, 3);
      loadShape(sg, id, shPrincessDress, 1, 4);
      loadShape(sg, id, shBeautyHair, 1, 5);
      loadShape(sg, id, shPFace, 1, 6);
      loadShape(sg, id, shFlowerHair, 1, 7); */

      // loadShape(sg, id, shPBody, 1, 0);
      // loadShape(sg, id, shPHead, 1, 1);

      /* loadShape(sg, id, shReptileFrontFoot, 1, 0);
      loadShape(sg, id, shReptileRearFoot, 1, 1);
      loadShape(sg, id, shReptileFrontLeg, 1, 2);
      loadShape(sg, id, shReptileRearLeg, 1, 3);
      loadShape(sg, id, shReptileBody, 2, 4);
      loadShape(sg, id, shReptileHead, 2, 5);
      loadShape(sg, id, shReptileTail, 2, 6); */

      // loadShape(sg, id, shTrylobite, 2, 0);
      
      for(int i=0; i<8; i++)
        loadShape(sg, id, shWave[i][0], 1, i);

      /* loadShape(sg, id, shYeti, 2, 0);
      loadShape(sg, id, shHumanFoot, 1, 1); */

      /* loadShape(sg, id, shYeti, 1, 2);
      loadShape(sg, id, shRatHead, 1, 3);
      loadShape(sg, id, shRatTail, 1, 1);
      loadShape(sg, id, shWolf1, 1, 4);
      loadShape(sg, id, shWolf2, 1, 5);
      loadShape(sg, id, shRatCape1, 1, 7);
      loadShape(sg, id, shRatCape2, 1, 6); */

//    loadShape(sg, id, shTortoise[0][0], 1, 0);
/*      loadShape(sg, id, shTentacleX, 1, 0);
      loadShape(sg, id, shTentacle, 1, 1);
      loadShape(sg, id, shJoint, 1, 2); */
      
      /* loadShape(3, 0, shTurtleFloor[0], 12, 0);
      loadShape(3, 1, shTurtleFloor[1], 14, 0); */

      // loadShape(sg, id, shDragonSegment, 2, 0);
      // loadShape(sg, id, shEyes, 2, 2);
      
      // loadShape(sg, id, shFamiliarHead, 2, 0);
      rebuildPolys = true;
      }

    if(uni == 'K') {
      if(vid.cs.charid >= 4) {
        loadShape(sg, id, shCatBody, 2, 0);
        loadShape(sg, id, shCatHead, 2, 1);
        }
      else {
        if(!(vid.cs.charid&1)) loadShape(sg, id, shPBody, 2, 0);
        else loadShape(sg, id, shFemaleBody, 2, 0);
  
        loadShape(sg, id, shPSword, 1, 1);
  
        if(vid.cs.charid&1)
          loadShape(sg, id, shFemaleDress, 2, 2);

        /* if(vid.cs.charid&1)
          loadShape(sg, id, shPrincessDress, 1, 3);
        else
          loadShape(sg, id, shPrinceDress, 2, 3); */
        
        loadShape(sg, id, shRatCape2, 1, 3);
      
        if(vid.cs.charid&1)
          loadShape(sg, id, shFemaleHair, 2, 4);
        else
          loadShape(sg, id, shPHead, 2, 4);
        
        loadShape(sg, id, shPFace, 2, 5); 
        }
      
      // loadShape(sg, id, shWolf, 2, dslayer);
      
      rebuildPolys = true;
      }

    if(uni == '+') dsCur->rots++;

    if(uni >= '1' && uni <= '9') {
      dsCur->rots = uni - '0';
      if(dsCur->rots == 9) dsCur->rots = 21;
      rebuildPolys = true;
      }
    if(uni == '0') {
      dsCur->sym = !dsCur->sym;
      rebuildPolys = true;
      }

    if(uni == 't') {
      dsCur->shift = mh;
      rebuildPolys = true;
      }
    if(uni == 'y') {
      dsCur->spin = mh;
      rebuildPolys = true;
      }

#define COLORKEY (-10000)    
    if(uni == COLORKEY) dsCur->color = colortouse;
    }

  void writeHyperpoint(FILE *f, hyperpoint h) {
    for(int i=0; i<3; i++) fprintf(f, "%lf ", double(h[i]));
    fprintf(f, "\n");
    }
  
  hyperpoint readHyperpoint(FILE *f) {
    hyperpoint h;
    for(int i=0; i<3; i++) {
      double d;
      int err = fscanf(f, "%lf", &d);
      if(err) printf("Warning: read error\n");
      h[i] = d;
      }
    return h;
    }
  
  string drawHelpLine() {
    return XLAT("vector graphics editor");
    }
  
  bool onelayeronly;
  
  void drawHandleKey(int sym, int uni) {

    handlePanning(sym, uni);
  
    if(uni == SETMOUSEKEY) mousekey = newmousekey;
  
    dslayer %= USERLAYERS;
    hyperpoint mh = inverse(drawtrans) * mouseh;
    
    int sg = drawcellShapeGroup();
    
    for(int i=0; i<USERSHAPEIDS; i++) if(editingShape(sg, i))
      applyToShape(sg, i, uni, mh);
      
    if(uni == 'e' || (uni == '-' && mousekey == 'e')) {
      initdraw(mouseover ? mouseover : cwt.c);
      }
    if(uni == 'l') { dslayer++; dslayer %= USERLAYERS; }
    if(uni == 'L') { dslayer--; if(dslayer < 0) dslayer += USERLAYERS; }
    if(uni == 'l' - 96) onelayeronly = !onelayeronly;
    
    if(uni == 'g') coldcenter = ccenter, ccenter = mh;
    if(uni == 'c') ew = ewsearch;
    if(uni == 'b') autochoose = !autochoose;

    
    if(uni == 'S') {
      for(int i=0; i<USERSHAPEGROUPS; i++) for(int j=0; j<USERSHAPEIDS; j++) {
        usershape *us = usershapes[i][j];
        if(!us) continue;
        
        for(int l=0; l<USERLAYERS; l++) if(size(us->d[l].list)) {
          usershapelayer& ds(us->d[l]);
          printf("// %d %d %d [%06X]\n", i, j, l, ds.color);
          printf(" ID, %d, %d, ", us->d[l].rots, us->d[l].sym?2:1); 
          for(int i=0; i<size(us->d[l].list); i++) 
            printf("%lf,%lf, ", double(us->d[l].list[i][0]), double(us->d[l].list[i][1]));
          printf("\n");
          }
        }
      }

    if(uni == 'z') vid.scale *= 2;
    if(uni == 'o') vid.scale /= 2;

    if(uni == ' ' && cheater) {
      popScreen();
      pushScreen(showMapEditor);
      }

    if(uni == 'p')
      dialog::openColorDialog(colortouse);

    if(sym == SDLK_F4) {
      filecaption = XLAT("pics to save/load:");
      cfileptr = &picfile;
      cfileext = ".pic";
      pushScreen(drawFileDialog);
      return;
      }

    if(sym == SDLK_F2) {
      FILE *f = fopen(picfile.c_str(), "wt");
      if(!f) {
        addMessage(XLAT("Failed to save pictures to %1", picfile));
        return;
        }
      fprintf(f, "HyperRogue saved picture\n");
      fprintf(f, "%x\n", VERNUM_HEX);
      for(int i=0; i<USERSHAPEGROUPS; i++) for(int j=0; j<USERSHAPEIDS; j++) {
        usershape *us = usershapes[i][j];
        if(!us) continue;
        
        for(int l=0; l<USERLAYERS; l++) if(size(us->d[l].list)) {
          usershapelayer& ds(us->d[l]);
          fprintf(f, "\n%d %d %d %d %d %6x %d\n", 
            i, j, l, ds.sym, ds.rots, ds.color, int(size(ds.list)));
          writeHyperpoint(f, ds.shift);
          writeHyperpoint(f, ds.spin);
          fprintf(f,"\n");
          for(int i=0; i<size(ds.list); i++)
            writeHyperpoint(f, ds.list[i]);
          }
        }
      fprintf(f, "\n-1\n");
      fclose(f);
      addMessage(XLAT("Pictures saved to %1", picfile));
      }
    
    if(sym == SDLK_F3) {
      FILE *f = fopen(picfile.c_str(), "rt");
      if(!f) {
        addMessage(XLAT("Failed to load pictures from %1", picfile));
        return;
        }
      int err;
      char buf[200];
      if(!fgets(buf, 200, f)) { 
        addMessage(XLAT("Failed to load pictures from %1", picfile));
        fclose(f); return; 
        }
      int vernum; err = fscanf(f, "%x", &vernum);
      printf("vernum = %x\n", vernum);
      while(true) {
        int i, j, l, sym, rots, color, siz;
        err = fscanf(f, "%d%d%d%d%d%x%d", &i, &j, &l, &sym, &rots, &color, &siz);
        if(i == -1 || err < 6) break;
        if(siz < 0 || siz > 1000) break;
        initShape(i, j);
        usershapelayer& ds(usershapes[i][j]->d[l]);
        ds.shift = readHyperpoint(f);
        ds.spin = readHyperpoint(f);
        ds.list.clear();
        for(int i=0; i<siz; i++) {
          ds.list.push_back(readHyperpoint(f));
          writeHyperpoint(stdout, ds.list[i]);
          }
        ds.sym = sym;
        ds.rots = rots;
        ds.color = color;
        }
      fclose(f);
      addMessage(XLAT("Pictures loaded from %1", picfile));
      
      buildpolys();
      }
    
    if(sym == SDLK_F7) {
      drawplayer = !drawplayer;
      }

#if CAP_SDL    
    if(sym == SDLK_F6) {
      saveHighQualityShot();
      }
#endif

#if CAP_SVG    
    if(sym == SDLK_F8) {
      svg::render();
      }
#endif
    
    if(sym == SDLK_F5) {
      for(int i=0; i<USERSHAPEGROUPS; i++)
      for(int j=0; j<USERSHAPEIDS; j++)
        if(usershapes[i][j]) delete usershapes[i][j];
      }
    
    if(sym == SDLK_ESCAPE) popScreen();

    if(sym == SDLK_F1) {
      gotoHelp(drawhelptext());
      }

    if(sym == SDLK_F10) popScreen();

    if(rebuildPolys)
      buildpolys(), rebuildPolys = false;
    }

  auto hooks = addHook(clearmemory, 0, [] () {
    if(mapeditor::painttype == 4) 
      mapeditor::painttype = 0, mapeditor::paintwhat = 0,
      mapeditor::paintwhat_str = "clear monster";
    mapeditor::copywhat = NULL;
    mapeditor::undo.clear();
    if(!cheater) mapeditor::displaycodes = 0;
    if(!cheater) mapeditor::whichShape = 0;
    modelcell.clear();
    });  
#endif

  int canvasback = linf[laCanvas].color >> 2;

  int generateCanvas(cell *c) {
    if(whichCanvas == 'C') {
      using namespace fieldpattern;
      int z = fp43.getdist(fieldval(c), make_pair(0,false));
      if(z < fp43.circrad) return 0x00C000;
      int z2 = fp43.getdist(fieldval(c), make_pair(fp43.otherpole,false));
      if(z2 < fp43.disthep[fp43.otherpole] - fp43.circrad)
        return 0x3000;
      return 0x6000;
      }
    if(whichCanvas == 'D') {
      using namespace fieldpattern;
      int z = fp43.getdist(fieldval(c), make_pair(0,false));
      return 255 * (fp43.maxdist+1-z) / fp43.maxdist;
      }
    if(whichCanvas == 'N') {
      using namespace fieldpattern;
      int z = fp43.getdist(fieldval(c), make_pair(0,false));
      int z2 = fp43.getdist(fieldval(c), make_pair(fp43.otherpole,false));
      if(z < z2) return 0x00C000;
      if(z > z2) return 0xC00000;
      return 0xCCCC00;
      }
    if(whichCanvas == 'S') {
      return 0x3F1F0F * fieldpattern::subval(c).second + 0x000080;
      }
    if(whichCanvas == 'g')
      return canvasback;
    if(whichCanvas == 'r')
      return hrand(0xFFFFFF + 1);
    if(whichCanvas == 'e') {
      static unsigned int fcol[4] = { 0x404040, 0x800000, 0x008000, 0x000080 };
      int fv = emeraldval(c);
      return fcol[fv&3];
      }
    if(whichCanvas == 'a') {
      static unsigned int fcol8[8] = { 
        0x800000,
        0x503000,
        0x206000,
        0x007010,
        0x004040,
        0x001070,
        0x200060,
        0x500030
        };
        
      if(c->wall == waNone) {
        int col = fcol8[land50(c)];
        if(polara50(c)) col += 0x181818;
        return col;
        }
      }
    if(whichCanvas == 'b') {
      static unsigned int fcol[4] = { 0x404040, 0x800000, 0x008000, 0x000080 };
      return fcol[polara50(c) + 2 * polarb50(c)];
      }
    if(whichCanvas == 'z') {
      static unsigned int fcol[4] = { 0xC0C0C0, 0xE0E0E0, 0x404040, 0x606060 };
      int fv = zebra40(c);
      return fcol[fv&3];
      }
    if(whichCanvas == 't') {
      static unsigned int fcol[4] = { 0x804040, 0x408040, 0x404080, 0x808040 };
      int fv = zebra40(c);
      if(fv/4 == 4 || fv/4 == 6 || fv/4 == 5 || fv/4 == 10) fv ^= 2;
      return fcol[fv&3];
      }
    if(whichCanvas == 'x') {
      static unsigned int fcol[4] = { 0xC0C0C0, 0x800000, 0x008000, 0x000080 };
      return fcol[zebra3(c)];
      }
    if(whichCanvas == 'w') {
      static unsigned int fcol[2] = { 0x303030, 0xC0C0C0 };
      return fcol[randpattern(c, subcanvas) ? 1 : 0];
      }
    if(whichCanvas == 'l') {
      int col[4];
      bool err = false;
      for(int j=0; j<4; j++) {
        col[j] = getCdata(c, j);
        col[j] *= 3;
        col[j] %= 240;
        if(col[j] > 120) col[j] = 240 - col[j];
        if(col[j] < -120) col[j] = -240 - col[j];
        }
      return (0x808080 + col[0] + (col[1] << 8) + (col[2] << 16)) >> (err?2:0);
      }
    if(whichCanvas == 'd') {
      int col[4];
      bool err = false;
      for(int j=0; j<4; j++) {
        col[j] = getCdata(c, j);
        col[j] *= 6;
        col[j] %= 240;
        if(col[j] > 120) col[j] = 240 - col[j];
        if(col[j] < -120) col[j] = -240 - col[j];
        }
      col[0] /= 8;
      col[1] /= 8;
      col[2] /= 8;
      return (0x101010 + col[0] + (col[1] << 8) + (col[2] << 16)) >> (err?2:0);
      }
    if(whichCanvas == 'h') {
      int col[4];
      bool err = false;
      for(int j=0; j<4; j++) {
        col[j] = getCdata(c, j);
        col[j] *= 6;
        col[j] %= 240;
        if(col[j] > 120) col[j] = 240 - col[j];
        if(col[j] < -120) col[j] = -240 - col[j];
        }
      col[0] /= 4;
      col[1] /= 4;
      col[2] /= 4;
      return (0x202020 + col[0] + (col[1] << 8) + (col[2] << 16)) >> (err?2:0);
      }
    if(whichCanvas == 'F') {
      return ishept(c) ? 0x202020 : 0xC0C0C0;
      }
    return canvasback;
    }

  void initdraw(cell *c) {
    mapeditor::drawcell = c;
    ew.c = c;
    ew.rotid = 0;
    ew.symid = 0;
    ew.pointid = -1;
    ew.side = 0;
    ewsearch = ew;
    }
    
  bool drawUserShape(transmatrix V, int group, int id, int color, cell *c) {
  #if !CAP_EDIT
    return false;
  #else
  
    usershape *us = usershapes[group][id];
    if(us) {  
      for(int i=0; i<USERLAYERS; i++) {
        if(i != dslayer && onelayeronly) continue;
        usershapelayer& ds(us->d[i]);
        hpcshape& sh(ds.sh);
    
        if(sh.s != sh.e) 
          queuepoly(V, sh, ds.color ? ds.color : color);
        }
      }
  
    if((cmode & sm::DRAW) && mapeditor::editingShape(group, id)) {
  
      /* for(int a=0; a<size(ds.list); a++) {
        hyperpoint P2 = V * ds.list[a];
  
        int xc, yc, sc;
        getcoord(P2, xc, yc, sc);
        queuechr(xc, yc, sc, 10, 'x', 
          a == 0 ? 0x00FF00 : 
          a == size(ds.list)-1 ? 0xFF0000 :
          0xFFFF00);
        } */
      
      if(c == ew.c) mapeditor::drawtrans = V;
      
      if(!us) return false;
   
      usershapelayer &ds(us->d[mapeditor::dslayer]);
      
      hyperpoint mh = inverse(mapeditor::drawtrans) * mouseh;
  
      for(int a=0; a<ds.rots; a++) 
      for(int b=0; b<(ds.sym?2:1); b++) {
  
        if(mouseout()) break;
  
        hyperpoint P2 = V * spin(2*M_PI*a/ds.rots) * (b?Mirror*mh:mh);
      
        queuechr(P2, 10, 'x', 0xFF00FF);
        }
      
      if(size(ds.list) == 0) return us;
      
      hyperpoint Plast = V * spin(-2*M_PI/ds.rots) * (ds.sym?Mirror*ds.list[0]:ds.list[size(ds.list)-1]);
      int state = 0;
      int gstate = 0;
      double dist2 = 0;
      hyperpoint lpsm;
      
      for(int a=0; a<ds.rots; a++) 
      for(int b=0; b<(ds.sym?2:1); b++) {
      
        hyperpoint mh2 = spin(2*M_PI*-ew.rotid/ds.rots) * mh;
        if(ew.symid) mh2 = Mirror * mh2;
        hyperpoint pseudomouse = V * spin(2*M_PI*a/ds.rots) * mirrorif(mh2, b);      
      
        for(int t=0; t<size(ds.list); t++) {
          int ti = b ? size(ds.list)-1-t : t;

          hyperpoint P2 = V * spin(2*M_PI*a/ds.rots) * mirrorif(ds.list[ti], b);
          
          if(!mouseout()) {
            double d = hdist(mouseh, P2);
            if(d < ewsearch.dist)
              ewsearch.dist = d,
              ewsearch.rotid = a,
              ewsearch.symid = b,
              ewsearch.pointid = ti,
              ewsearch.c = c,
              ewsearch.side = b,
              state = 1,
              dist2 = d + hdist(mouseh, Plast) - hdist(P2, Plast);
          
            else if(state == 1) {
              double dist3 = d + hdist(mouseh, Plast) - hdist(P2, Plast);
              if(dist3 < dist2) 
                ewsearch.side = !ewsearch.side;
              state = 2;
              }
            }
          
          queuechr(P2, 10, 'o', 
            0xC000C0);
          
          if(!mouseout()) {
            if(gstate == 1) queueline(lpsm, P2, 0x90000080), gstate = 0;
            if(ti == ew.pointid) {
              queueline(pseudomouse, P2, 0xF0000080);
              if(ew.side == b) queueline(pseudomouse, Plast, 0x90000080);
              else gstate = 1, lpsm = pseudomouse;
              }
            }
  
          Plast = P2;           
          }
  
        }
      
      if(gstate == 1) queueline(lpsm, V * ds.list[0], 0x90000080), gstate = 0;
      if(state == 1) {
        hyperpoint P2 = V * ds.list[0];
        if(hdist(mouseh, P2) + hdist(mouseh, Plast) - hdist(P2, Plast) < dist2) 
          ewsearch.side = 1;
        }
      
      }
  
    return us;
  #endif
    }
  
  }

namespace linepatterns {

  int lessalpha(int col, int m) {
    part(col, 0) /= m;
    return col;
    }
  
  int lessalphaif(int col, bool b) {
    return b?lessalpha(col, 4):col;
    }
    
  int lessalphaif(int col, bool b1, bool b2) {
    if(b1) col = lessalpha(col, 2);
    if(b2) col = lessalpha(col, 2);
    return col;
    }
    
  struct {
    int id;
    const char *lpname;
    unsigned int color;
    } patterns[] = {

    {patTriNet, "triangle grid: not rings", 0xFFFFFF00},
    {patTriRings, "triangle grid: rings", 0xFFFFFF00},
    {patHepta, "heptagonal grid", 0x0000C000},
    {patRhomb, "rhombic tesselation", 0x0000C000},
    {patTrihepta, "triheptagonal tesselation", 0x0000C000},
    {patNormal, "normal tesselation", 0x0000C000},
    {patBigTriangles, "big triangular grid", 0x00606000},
    {patBigRings, "big triangles: rings", 0x0000C000},
    
    {patTree, "underlying tree", 0x00d0d000},
    {patAltTree, "circle/horocycle tree", 0xd000d000},

    {patZebraTriangles, "zebra triangles", 0x40FF4000},
    {patZebraLines, "zebra lines", 0xFF000000},
    {patVine, "vineyard pattern", 0x8438A400},
    {patPalacelike, "firewall lines", 0xFF400000},
    {patPalace, "firewall lines: Palace", 0xFFD50000},
    {patPower, "firewall lines: Power", 0xFFFF0000},
    {0, NULL, 0}
    };

  void clearAll() {
    for(int k=0; patterns[k].lpname; k++) patterns[k].color &= ~255;
    }

  bool any() {
    for(int k=0; patterns[k].lpname; k++) if(patterns[k].color & 255) return true;
    return false;
    }

  void setColor(ePattern id, int col) {
    for(int k=0; patterns[k].lpname; k++)
      if(patterns[k].id == id) patterns[k].color = col;
    }
  
  void switchAlpha(ePattern id, int col) {
    for(int k=0; patterns[k].lpname; k++)
      if(patterns[k].id == id) patterns[k].color ^= col;
    }

  void drawPattern(int id, int col, cell *c, const transmatrix& V) {

    switch(id) {

#define col1 \
lessalphaif(col, behindsphere(V))

#define col2 \
lessalphaif(col, behindsphere(V), behindsphere(gmatrix[c2]))

      case patZebraTriangles:
        if(zebra40(c) / 4 == 10) {
          bool all = true;
          hyperpoint tri[3];
          for(int i=0; i<3; i++) {
            cell *c2 = createMov(c, i*2);
            if(!gmatrix.count(c2)) all = false;
            else tri[i] = tC0(gmatrix[c2]);
            }
          
          if(all) for(int i=0; i<3; i++)
            queueline(tri[i], tri[(i+1)%3], col, 3);
          }
        break;
      
      case patZebraLines:
        if(!pseudohept(c)) for(int i=0; i<c->type; i+=2) {
          cell *c2 = createMov(c, i);
          int fv1 = zebra40(c);
          if(fv1/4 == 4 || fv1/4 == 6 || fv1/4 == 5 || fv1/4 == 10) fv1 ^= 2;
          int fv2 = zebra40(c2);
          if(fv2/4 == 4 || fv2/4 == 6 || fv2/4 == 5 || fv2/4 == 10) fv2 ^= 2;
          if((fv1&1) == (fv2&1)) continue;
          
          double x = sphere?.3651:euclid?.2611:.2849;

          queueline(V * ddspin(c,i,-S14) * xpush0(x), 
            V * ddspin(c,i,+S14) * xpush0(x), 
            col, 1);
          }
        break;
      
      case patNormal: {
        double x = sphere?.401:euclid?.3 : .328;
        if(euclid || !pseudohept(c)) for(int t=0; t<c->type; t++) 
          if(euclid ? c->mov[t]<c : (((t^1)&1) || c->mov[t] < c))
            queueline(V * ddspin(c,t,-S7) * xpush0(x), 
                V * ddspin(c,t,+S7) * xpush0(x), 
                col1, 1);
        break;
        }
      
      case patTrihepta:
        if(!pseudohept(c)) for(int i=0; i<6; i++) {
          cell *c2 = c->mov[i];
          if(!c2 || !pseudohept(c2)) continue;
          double x = sphere?.3651:euclid?.2611:.2849;
          queueline(V * ddspin(c,i,-S14) * xpush0(x), 
            V * ddspin(c,i,+S14) * xpush0(x), 
            col2, 1);
          }
        break;
      
      case patTriNet:
        forCellEx(c2, c) if(c2 > c) if(gmatrix.count(c2)) if(celldist(c) != celldist(c2)) {
          queueline(tC0(V), gmatrix[c2]*C0, 
            darkena(backcolor ^ 0xFFFFFF, 0, col2),
            2);
          }
        break;

      case patTriRings:
        forCellEx(c2, c) if(c2 > c) if(gmatrix.count(c2) && celldist(c) == celldist(c2)) 
          queueline(tC0(V), gmatrix[c2]*C0, 
            darkena(backcolor ^ 0xFFFFFF, 0, col2),
            2);
        break;

      case patHepta:
        forCellEx(c2, c) if(c2 > c) if(gmatrix.count(c2) && pseudohept(c) == pseudohept(c2)) 
          queueline(tC0(V), gmatrix[c2]*C0, 
            darkena(backcolor ^ 0xFFFFFF, 0, col2),
            2);
        break;

      case patRhomb:
        forCellEx(c2, c) if(c2 > c) if(gmatrix.count(c2) && pseudohept(c) != pseudohept(c2)) 
          queueline(tC0(V), gmatrix[c2]*C0, 
            darkena(backcolor ^ 0xFFFFFF, 0, col2),
            2);
        break;
      
      case patPalace: {
        int a = polarb50(c);
        if(pseudohept(c)) for(int i=0; i<7; i++) {
            cell *c1 = createMov(c, (i+3) % 7);
            cell *c2 = createMov(c, (i+4) % 7);
            if(polarb50(c1) != a && polarb50(c2) != a)
                queueline(V * ddspin(c,i,84*5/14) * xpush0(tessf/2),
                          V * ddspin(c,i,84*9/14) * xpush0(tessf/2),
                                    col, 1);
            }
        break;
        }
      
      case patPalacelike:
        if(pseudohept(c)) for(int i=0; i<7; i++) 
          queueline(V * ddspin(c,i,84*5/14) * xpush0(tessf/2),
                    V * ddspin(c,i,84*9/14) * xpush0(tessf/2),
                              col1, 1);
        break;
      
      case patBigTriangles: {
        if(pseudohept(c) && !euclid) for(int i=0; i<S7; i++) 
          if(c->master->move[i] < c->master) {
            queueline(tC0(V), V*xspinpush0((purehepta?M_PI:0) -2*M_PI*i/S7, tessf), col1, 2);
            }
        break;
        }
        
      case patBigRings: {
        if(pseudohept(c) && !euclid) for(int i=0; i<S7; i++) 
          if(c->master->move[i] && c->master->move[i] < c->master && c->master->move[i]->dm4 == c->master->dm4) {
            cell *c2 = c->master->move[i]->c7;
            queueline(tC0(V), V*xspinpush0((purehepta?M_PI:0) -2*M_PI*i/S7, tessf), col2, 2);
            }
        break;
        }
        
      case patTree:
        if(c->type != 6 && !euclid) 
          queueline(tC0(V), V*ddi0(purehepta?S42:0, tessf), col1, 2);
        break;
      
      case patAltTree:
        if(c->type != 6 && !euclid && c->master->alt) {
          for(int i=0; i<S7; i++)
            if(c->master->move[i] && c->master->move[i]->alt == c->master->alt->move[0])
              queueline(tC0(V), V*xspinpush0((purehepta?M_PI:0) -2*M_PI*i/S7, tessf), col, 2);
          }
        break;
      
      case patVine: {
        int p = emeraldval(c);
        double hdist = hdist0(heptmove[0] * heptmove[2] * C0);
        if(pseudohept(c) && (p/4 == 10 || p/4 == 8))
        for(int i=0; i<S7; i++) if(c->mov[i] && emeraldval(c->mov[i]) == p-4) {
          queueline(tC0(V), V*tC0(heptmove[i]), col, 2);
          queueline(tC0(V), V*tC0(spin(-i * ALPHA) * xpush(-hdist/2)), col, 2);
          }
        break;
        }
      
      case patPower: {
        int a = emeraldval(c);
        if(pseudohept(c) && a/4 == 8) for(int i=0; i<7; i++) {
            heptagon *h1 = c->master->move[(i+1)%7];
            heptagon *h2 = c->master->move[(i+6)%7];
            if(!h1 || !h2) continue;
            if(emeraldval(h1->c7)/4 == 8 && emeraldval(h2->c7)/4 == 8)
                queueline(V * ddspin(c,i,84*5/14) * xpush0(tessf/2),
                          V * ddspin(c,i,84*9/14) * xpush0(tessf/2),
                                    col, 1);
            }
        break;
        }
      }
    }  

  void drawAll() {

    if(any()) for(map<cell*, transmatrix>::iterator it = gmatrix.begin(); it != gmatrix.end(); it++) {
      cell *c = it->first;
      transmatrix& V = it->second;
      
      for(int k=0; patterns[k].lpname; k++) {
        int col = patterns[k].color;
        if(!(col & 255)) continue;
        int id = patterns[k].id;
        
        drawPattern(id, col, c, V);
        }
      }
    }
#undef col1
#undef col2
  
  int numpat = 0;
  
  void showMenu() {
    dialog::init(XLAT("line patterns"));
    
    for(numpat=0; patterns[numpat].lpname; numpat++)
      dialog::addColorItem(XLAT(patterns[numpat].lpname), patterns[numpat].color, 'a'+numpat);
  
    dialog::addBreak(50);
    dialog::addItem(XLAT("go back"), ' ');
    
    dialog::addBreak(50);
    dialog::addInfo("change the alpha parameter to show the lines");
  
    dialog::display();
    
    keyhandler = [] (int sym, int uni) {
      dialog::handleNavigation(sym, uni);
      if(uni >= 'a' && uni < 'a' + numpat)
        dialog::openColorDialog(patterns[uni - 'a'].color, NULL);
      else if(doexiton(sym,uni)) popScreen();
      }; 
    }
  
  };
