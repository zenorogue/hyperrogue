// HyperRogue map editor
// Copyright (C) 2011-2016 Zeno Rogue, see 'hyper.cpp' for details

#include <map>
#include <stdint.h>

#ifdef WINDOWS
#include "direntx.h"
#include "direntx.c"
#else
#include <dirent.h>
#endif

namespace mapeditor {
  int subcanvas;
#ifndef NOEDIT
  map<int, cell*> modelcell;
  
  void clearModelCells() {
    modelcell.clear();
    }
  
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

#ifndef NOEDIT
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
    cleargraphmemory();
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
    
#ifndef NOEDIT
  int paintwhat = 0;
  int painttype = 0;
  int radius = 0;
  string paintwhat_str = "clear monster";
  cell *copywhat = NULL; int copydir; bool copyflip;
  
  int subscreen; //0=normal, 1=config, 2=patterns, 3=predesigned

  bool symRotation, sym01, sym02, sym03;
  int displaycodes;
  int whichpart;
  
  cell *drawcell;
  
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
  #define CFILE 0xFFFFFF
  
  bool filecmp(const pair<string,int> &f1, const pair<string,int> &f2) {
    if(f1.first == "../") return true;
    if(f2.first == "../") return false;
    if(f1.second != f2.second)
      return f1.second == CDIR;
    return f1.first < f2.first;
    }
  
  void drawFileDialog() {
    displayfr(vid.xres/2, 30 + vid.fsize, 2, vid.fsize, 
      XLAT(cmode == emDraw ? "pics to save/load:" : "level to save/load:"), 0xFFFFFF, 8);
      
    string cfile = cmode == emDraw ? picfile : levelfile;
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
        else if(size(s) > 4 && s.substr(size(s)-4) == (cmode == emDraw ? ".pic" : ".lev"))
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
    }
  
  void displayFunctionKeys() {
    int fs = vid.fsize + 5;
    displayButton(8, vid.yres-8-fs*11, XLAT("F1 = help"), SDLK_F1, 0);
    displayButton(8, vid.yres-8-fs*10, XLAT("F2 = save"), SDLK_F2, 0);
    displayButton(8, vid.yres-8-fs*9, XLAT("F3 = load"), SDLK_F3, 0);
    displayButton(8, vid.yres-8-fs*8, XLAT("F4 = file"), SDLK_F3, 0);
    displayButton(8, vid.yres-8-fs*7, XLAT("F5 = restart"), SDLK_F5, 0);
    displayButton(8, vid.yres-8-fs*6, XLAT("F6 = HQ shot"), SDLK_F6, 0);
    displayButton(8, vid.yres-8-fs*5, XLAT("F7 = player on/off"), SDLK_F7, 0);
    displayButton(8, vid.yres-8-fs*4, XLAT("F8 = SVG shot"), SDLK_F8, 0);
    displayButton(8, vid.yres-8-fs*3, XLAT("SPACE = map/graphics"), ' ', 0);
    displayButton(8, vid.yres-8-fs*2, XLAT("ESC = return to the game"), SDLK_ESCAPE, 0);
    }

  void vpush(int i, const char *name) {
    string s = XLATN(name);
    if(!hasInfix(s)) return;
    v.push_back(make_pair(s, i));
    }
  
  void showMapEditor() {
  
    if(choosefile) { drawFileDialog(); return; }
  
    if(subscreen == 2) {
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

      dialog::addItem(XLAT("predesigned patterns"), 'r');
      dialog::display();
      }
    else if(subscreen == 3) {
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
      }
    else if(subscreen == 1 && painttype == 6) 
      dialog::drawColorDialog(paintwhat);
    else if(subscreen == 1) {
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
      }
    else {
      /* displayfr(vid.xres/2, vid.ycenter - vid.radius * 3/4 - vid.fsize*3/2, 2,
        vid.fsize, 
        XLAT("MAP EDITOR: ") + paintwhat_str, 
        0xFFFFFF, 8); */
        
      int fs = vid.fsize + 5;
      
      getcstat = '-';

      displayfr(8, 8 + fs, 2, vid.fsize, paintwhat_str, 0xFFFFFF, 0);
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
      }
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
  
  bool handleKeyFile(int uni, int& sym) {
    string& s(cmode == emDraw ? picfile : levelfile);
    int i = size(s) - (editext?0:4);
    if(uni > 2000) sym = uni - 2000;
    if(sym == SDLK_RETURN || sym == SDLK_ESCAPE) {
      choosefile = false;
      return true;
      }
    else if(sym == SDLK_F2 || sym == SDLK_F3) {
      choosefile = false;
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
  
  void handleKey(int sym, int uni) {
    if(choosefile && handleKeyFile(sym, uni)) ;
    else if(subscreen == 1 && painttype == 6) {
      paintwhat_str = "paint";
      int v = dialog::handleKeyColor(sym, uni, paintwhat);
      if(v == 1) subscreen = 0;
      if(v == 2) cmode = emNormal;
      }
    else if(subscreen == 1) {
      if(uni >= '1' && uni <= '9') uni = 1000 + uni - '1';
      if(sym == SDLK_RETURN || sym == '-') uni = 1000;
      for(int z=0; z<size(v); z++) if(1000 + z == uni) {
        paintwhat = v[z].second;
        paintwhat_str = v[z].first;
        subscreen = 0;
        mousepressed = false;
        }
      if(editInfix(uni)) ;
      else if(subscreen == 1 && uni != 0) cmode = emNormal;      
      }
    else if(subscreen == 3) {
      dialog::handleNavigation(sym, uni);
      if((uni >= 'a' && uni <= 'z') || (uni >= 'A' && uni <= 'Z')) {
        whichCanvas = uni;
        subcanvas = rand();
        firstland = laCanvas; randomPatternsMode = false;
        restartGame(); subscreen = 0;
        }
      else if(uni != 0) subscreen = 0;
      }
    else if(subscreen == 2) {
      dialog::handleNavigation(sym, uni);
      if(uni == 'f' || uni == 'p' || uni == 'z' || uni == 'H' || uni == 'F') {
        if(whichPattern == uni) whichPattern = 0;
        else whichPattern = uni;
        clearModelCells();
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
      
      else if(uni == 'r') 
        subscreen = 3;
      
      else if(uni != 0) subscreen = 0;
      }
    else {
      // left-clicks are coded with '-', and right-clicks are coded with sym F1
      if(uni == '-') undoLock();
      if(mousepressed && mouseover && sym != SDLK_F1)
        allInPattern(mouseover, radius, neighborId(mouseover, mouseover2));
      
      if(mouseover) for(int i=0; i<mouseover->type; i++) createMov(mouseover, i);
      if(uni == 'u') applyUndo();
      else if(uni == 'v' || sym == SDLK_F10 || sym == SDLK_ESCAPE) cmode = emNormal;
      else if(uni >= '0' && uni <= '9') radius = uni - '0';
      else if(uni == 'm') subscreen = 1, painttype = 0, infix = "";
      else if(uni == 'i') subscreen = 1, painttype = 1, infix = "";
      else if(uni == 'l') subscreen = 1, painttype = 2, infix = "";
      else if(uni == 'w') subscreen = 1, painttype = 3, infix = "";
      else if(uni == 'r') subscreen = 2;
      else if(uni == 't' && mouseover) {
        cwt.c = mouseover; playermoved = true;
        cwt.spin = neighborId(mouseover, mouseover2);
        }
      else if(uni == 'b') painttype = 5, paintwhat_str = XLAT("boundary");
      else if(uni == 'p')
        subscreen = 1, paintwhat = (painttype ==6 ? paintwhat : 0x808080), painttype = 6;
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
      else if(sym == SDLK_F4)
        choosefile = true;
      else if(sym == SDLK_F6) {
        saveHighQualityShot();
        }
      else if(sym == SDLK_F8) {
        svg::render();
        }
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
      else if(uni == 'h' || sym == SDLK_F1) {
        lastmode = cmode;
        cmode = emHelp;
        help = mehelptext();
        }
      else if(uni == ' ') {
        cmode = emDraw;
        drawcell = mouseover ? mouseover : cwt.c;
        }
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
    return XLAT(mapeditor::drawhelp);
    }
      
  int dslayer;
  bool coloring;
  int colortouse = 0xC0C0C0FF;

  transmatrix drawtrans;

  void loadShape(int sg, int id, hpcshape& sh, int d, int layer) {
    initShape(sg, id);
    usershapelayer *dsCur = &usershapes[sg][id]->d[layer];
    dsCur->list.clear();
    dsCur->sym = d==2;
    for(int i=sh.s; i < sh.s + (sh.e-sh.s)/d; i++)
      dsCur->list.push_back(hpc[i]);
    }

  void drawGhosts(cell *c, const transmatrix& V, int ct) {
/*  if(cmode == emDraw && cwt.c->type == 6 && ct == 6) for(int a=0; a<dsCur->rots; a++) {

      transmatrix V2 = V * spin(M_PI + 2*M_PI*a/dsCur->rots);

      if(outofmap(mouseh)) break;

      hyperpoint P2 = V2 * inverse(cwtV) * mouseh;
    
      int xc, yc, sc;
      getcoord(P2, xc, yc, sc);
      displaychr(xc, yc, sc, 10, 'x', 0xFF);

      if(crad > 0 && c->cpdist <= 3) {
        lalpha = 0x80;
        transmatrix movtocc = V2 * inverse(cwtV) * rgpushxto0(ccenter);
        for(int d=0; d<84; d++) 
          drawline(movtocc * ddi(d+1, crad) * C0, movtocc * ddi(d, crad) * C0, 0xC00000);
        lalpha = 0xFF;
        }
      } */
    }

  hyperpoint ccenter = C0;
  hyperpoint coldcenter = C0;
  
  void drawGrid() {
    if(cmode == emDraw && !inHighQual) {

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
  
      int sg = drawcellShapeGroup();
      
      for(int i=0; i<USERSHAPEIDS; i++) if(editingShape(sg, i) && usershapes[sg][i]) {
  
        usershapelayer &ds(usershapes[sg][i]->d[mapeditor::dslayer]);
  
        for(int a=0; a<size(ds.list); a++) {
          hyperpoint P2 = drawtrans * ds.list[a];
    
          queuechr(P2, 10, 'x', 
            darkena(a == 0 ? 0x00FF00 : 
            a == size(ds.list)-1 ? 0xFF0000 :
            0xFFFF00, 0, 0xFF));
          }
        }
      }
    }

  void showDrawEditor() {

    if(coloring) {
      dialog::drawColorDialog(colortouse);
      return;
      }

    if(choosefile) { drawFileDialog(); return; }
  
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
    
    int fs = vid.fsize + 5;

    // displayButton(8, 8+fs*9, XLAT("l = lands"), 'l', 0);
    displayfr(8, 8+fs, 2, vid.fsize, line1, 0xC0C0C0, 0);
    displayfr(8, 8+fs*2, 2, vid.fsize, line2, 0xC0C0C0, 0);
    displayButton(8, 8+fs*3, XLAT("l = layers: %1", its(dslayer)), 'l', 0);
    if(us) {
      usershapelayer& ds(us->d[dslayer]);
      displayButton(8, 8+fs*4, XLAT("1-9 = rotations: %1", its(ds.rots)), '1' + (ds.rots % 9), 0);
      displayButton(8, 8+fs*5, XLAT(ds.sym ? "0 = symmetry" : "0 = asymmetry"), '0', 0);

      displayfr(8, 8+fs*7, 2, vid.fsize, XLAT("%1 vertices", its(size(ds.list))), 0xC0C0C0, 0);
      displayfr(8, 8+fs*8, 2, vid.fsize, XLAT("a = add v"), 0xC0C0C0, 0);
      displayfr(8, 8+fs*9, 2, vid.fsize, XLAT("m = move v"), 0xC0C0C0, 0);
      displayfr(8, 8+fs*10, 2, vid.fsize, XLAT("d = delete v"), 0xC0C0C0, 0);
      displayfr(8, 8+fs*11, 2, vid.fsize, XLAT("c = readd v"), 0xC0C0C0, 0);
      displayfr(8, 8+fs*12, 2, vid.fsize, XLAT("b = reorder"), 0xC0C0C0, 0);

      displayfr(8, 8+fs*14, 2, vid.fsize, XLAT("t = shift"), 0xC0C0C0, 0);
      displayfr(8, 8+fs*15, 2, vid.fsize, XLAT("y = spin"), 0xC0C0C0, 0);
      displayButton(8, 8+fs*16, XLAT("p = paint"), 'p', 0);

      }
    else {
      displayfr(8, 8+fs*5, 2, vid.fsize, XLAT("'n' to start"), 0xC0C0C0, 0);
      }

    displayfr(vid.xres-8, 8+fs*4, 2, vid.fsize, XLAT("g = grid"), 0xC0C0C0, 16);
    displayButton(vid.xres-8, 8+fs*3, XLAT("z = zoom in"), 'z', 16);
    displayButton(vid.xres-8, 8+fs*2, XLAT("o = zoom out"), 'o', 16);
    displayfr(vid.xres-8, 8+fs, 2, vid.fsize, XLAT("e = edit this"), 0xC0C0C0, 16);

    if(!outofmap(mouseh)) {
      hyperpoint mh = inverse(drawtrans * rgpushxto0(ccenter)) * mouseh;
      displayfr(vid.xres-8, vid.yres-8-fs*6, 2, vid.fsize, XLAT("x: %1", fts4(mh[0])), 0xC0C0C0, 16);
      displayfr(vid.xres-8, vid.yres-8-fs*5, 2, vid.fsize, XLAT("y: %1", fts4(mh[1])), 0xC0C0C0, 16);
      displayfr(vid.xres-8, vid.yres-8-fs*4, 2, vid.fsize, XLAT("z: %1", fts4(mh[2])), 0xC0C0C0, 16);
      displayfr(vid.xres-8, vid.yres-8-fs*2, 2, vid.fsize, XLAT("r: %1", fts4(hdist0(mh))), 0xC0C0C0, 16);
      displayfr(vid.xres-8, vid.yres-8-fs, 2, vid.fsize, XLAT("ϕ: %1°", fts4(-atan2(mh[1], mh[0]) * 360 / 2 / M_PI)), 0xC0C0C0, 16);
      }
    
    displayFunctionKeys();
    }
  
  bool rebuildPolys = false;
  
  void applyToShape(int sg, int id, int uni, hyperpoint mh) {
    bool haveshape = usershapes[sg][id];
    bool xnew = false;
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
      dsCur->list.push_back(mh);
      rebuildPolys = true;
      }
    
    if(uni == 'D') {
      delete usershapes[sg][id];
      usershapes[sg][id] = NULL;
      }

    if(uni == 'm' || uni == 'd' || uni == 'c' || uni == 'b') {
      int i = 0;
      if(size(dsCur->list) < 1) return;
      for(int j=1; j<size(dsCur->list); j++) 
        if(intval(mh, dsCur->list[j]) < intval(mh, dsCur->list[i]))
          i = j;
      if(uni == 'm') 
        dsCur->list[i] = mh;
      if(uni == 'd')
        dsCur->list.erase(dsCur->list.begin() + i);
      if(uni == 'c')
        dsCur->list.push_back(dsCur->list[i]);
      if(uni == 'b') {
        while(i) {
          dsCur->list.push_back(dsCur->list[0]);
          dsCur->list.erase(dsCur->list.begin());
          i--;
          }
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

      loadShape(sg, id, shTrylobite, 2, 0);

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
  
  void drawHandleKey(int sym, int uni) {

    if(choosefile && handleKeyFile(sym, uni)) return;
  
    if(coloring) {
      int v = dialog::handleKeyColor(sym, uni, colortouse);
      if(v == 2) { coloring = false; return; }
      else if(v == 1) { coloring = false; uni = COLORKEY; }
      else return;
      }
  
    dslayer %= USERLAYERS;
    hyperpoint mh = inverse(drawtrans) * mouseh;
    
    int sg = drawcellShapeGroup();
    
    for(int i=0; i<USERSHAPEIDS; i++) if(editingShape(sg, i))
      applyToShape(sg, i, uni, mh);
      
    if(uni == 'e') {
      drawcell = mouseover ? mouseover : cwt.c;
      }
    if(uni == 'l') { dslayer++; dslayer %= USERLAYERS; }
    if(uni == 'L') { dslayer--; if(dslayer < 0) dslayer += USERLAYERS; }
    
    if(uni == 'g') coldcenter = ccenter, ccenter = mh;
    
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

    if(uni == ' ' && cheater) cmode = emMapEditor;

    if(uni == 'p') coloring = true;

    if(sym == SDLK_F4) choosefile = true;

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
    
    if(sym == SDLK_F6) {
      saveHighQualityShot();
      }
    
    if(sym == SDLK_F8) {
      svg::render();
      }
    
    if(sym == SDLK_F5) {
      for(int i=0; i<USERSHAPEGROUPS; i++)
      for(int j=0; j<USERSHAPEIDS; j++)
        if(usershapes[i][j]) delete usershapes[i][j];
      }
    
    if(sym == SDLK_ESCAPE) cmode = emNormal;

    if(sym == SDLK_F1) {
      lastmode = cmode;
      cmode = emHelp;
      sym = 0;
      help = drawhelptext();
      }

    if(sym == SDLK_F10) cmode = emNormal;

    if(rebuildPolys)
      buildpolys(), rebuildPolys = false;
    }
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
        col[j] *= 3;
        col[j] %= 240;
        if(col[j] > 120) col[j] = 240 - col[j];
        if(col[j] < -120) col[j] = -240 - col[j];
        }
      col[0] /= 8;
      col[1] /= 8;
      col[2] /= 8;
      return (0x101010 + col[0] + (col[1] << 8) + (col[2] << 16)) >> (err?2:0);
      }
    if(whichCanvas == 'F') {
      return ishept(c) ? 0x202020 : 0xC0C0C0;
      }
    return canvasback;
    }
  }
