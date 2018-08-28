// HyperRogue map editor
// Copyright (C) 2011-2018 Zeno Rogue, see 'hyper.cpp' for details

namespace hr {

namespace mapeditor {

  struct editwhat {
    double dist;
    int rotid, symid, pointid;
    bool side;
    cell *c;
    } ew, ewsearch;
  bool autochoose = ISMOBILE;
  
  void scaleall(ld z) { 
     
     // (mx,my) = (xcb,ycb) + ss * (xpos,ypos) + (mrx,mry) * scale
     
     // (mrx,mry) * (scale-scale') =
     // ss * ((xpos',ypos')-(xpos,ypos))
     
     // mx = xb + ssiz*xpos + mrx * scale
     // mx = xb + ssiz*xpos' + mrx * scale' 
     
     ld mrx = (.0 + mousex - vid.xcenter) / vid.scale;
     ld mry = (.0 + mousey - vid.ycenter) / vid.scale;
     
     if(vid.xres > vid.yres) {      
       vid.xposition += (vid.scale - vid.scale*z) * mrx / vid.scrsize;
       vid.yposition += (vid.scale - vid.scale*z) * mry / vid.scrsize;
       }

     vid.scale *= z;
     // printf("scale = " LDF "\n", vid.scale);
     #if CAP_TEXTURE
     // display(texture::itt);
     texture::config.itt = xyscale(texture::config.itt, 1/z);
     if(false && texture::config.tstate) {
       calcparam();
       texture::config.perform_mapping();
       if(texture::config.tstate == texture::tsAdjusting) 
         texture::config.finish_mapping();
       }
     #endif
     }
  
#if CAP_EDIT
  map<int, cell*> modelcell;

  void handleKeyMap(int sym, int uni);

  void applyModelcell(cell *c) {
    if(patterns::whichPattern == 'H') return;
    auto si = patterns::getpatterninfo0(c);
    cell *c2 = modelcell[si.id];
    if(c2) {
      c->wall = c2->wall;
      c->land = c2->land;
      c->monst = c2->monst;
      c->item = c2->item;
      c->landparam = c2->landparam;
      c->wparam = c2->wparam;
      c->mondir = c2->mondir;
      c->stuntime = c2->stuntime;
      c->hitpoints = c2->hitpoints;
      if(c2->mondir != NODIR) {
        auto si2 = patterns::getpatterninfo0(c2);
        c->mondir = (c2->mondir - si2.dir + si.dir + MODFIXER) % c->type;
        // todo reflect
        }
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
    int numcells = isize(cellbyid);
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
    save(patterns::whichPattern);
    save(geometry);
    char nbtype = nonbitrunc;
    if(gp::on) nbtype = 2;
    save(nbtype);
    if(gp::on) {
      save(gp::param.first);
      save(gp::param.second);
      }
    if(geometry == gTorus) {
      save(torusconfig::qty);
      save(torusconfig::dx);
      save(torusconfig::dy);
      }
    if(geometry == gFieldQuotient) {
      using namespace fieldpattern;
      save(quotient_field_changed);
      if(quotient_field_changed) {
        save(current_extra);
        save(fgeomextras[current_extra].current_prime_id);
        }
      }
    if(geometry == gArchimedean) {
      const string& symbol = arcm::current.symbol;
      char size = isize(symbol);
      save(size);
      for(int i=0; i<size; i++) save(symbol[i]);
      }
    addToQueue((bounded || euclid) ? currentmap->gamestart() : cwt.at->master->c7);
    for(int i=0; i<isize(cellbyid); i++) {
      cell *c = cellbyid[i];
      if(i) {
        for(int j=0; j<c->type; j++) if(c->move(j) && cellids.count(c->move(j)) && 
          cellids[c->move(j)] < i) {
          int32_t i = cellids[c->move(j)];
          save(i);
          saveChar(c->c.spin(j));
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
        cell *c2 = c->move(j);
        if(c2 && c2->land != laNone) addToQueue(c2);
        }
      }
    printf("cells saved = %d\n", isize(cellbyid));
    int32_t n = -1; save(n);
    int32_t id = cellids.count(cwt.at) ? cellids[cwt.at] : -1;
    save(id);

    for(int i=0; i<mapeditor::USERSHAPEGROUPS; i++) for(auto usp: usershapes[i]) {
      usershape *us = usp.second;
      if(!us) continue;
      
      for(int l=0; l<USERLAYERS; l++) if(isize(us->d[l].list)) {
        usershapelayer& ds(us->d[l]);
        save(i); save(usp.first); save(l); save(ds.sym); save(ds.rots); save(ds.color);
        n = isize(ds.list); save(n);
        savePoint(ds.shift);
        savePoint(ds.spin);
        for(int i=0; i<isize(ds.list); i++) savePoint(ds.list[i]);
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
  
  bool loadMap(const string& fname) {
    f = fopen(fname.c_str(), "rb");
    if(!f) return false;
    clearMemory();
    int vernum = loadInt();
    if(vernum >= 7400) load(patterns::whichPattern);
    
    if(vernum >= 10203) {
      load(geometry);
      char nbtype;
      load(nbtype);
      nonbitrunc = !!nbtype;
      gp::on = nbtype == 2;
      if(gp::on) {
        load(gp::param.first);
        load(gp::param.second);
        }
      if(geometry == gTorus) {
        load(torusconfig::qty);
        load(torusconfig::dx);
        load(torusconfig::dy);
        }
      if(geometry == gFieldQuotient) {
        using namespace fieldpattern;
        load(quotient_field_changed);
        if(quotient_field_changed) {
          load(current_extra);
          load(fgeomextras[current_extra].current_prime_id);
          enableFieldChange();
          }
        }
      if(geometry == gArchimedean) {
        string& symbol = arcm::current.symbol;
        char size;
        load(size);
        symbol.resize(size);
        for(int i=0; i<size; i++) load(symbol[i]);
        arcm::current.parse();
        if(arcm::current.errors > 0) {
          printf("Errors! %s\n", arcm::current.errormsg.c_str());
          }
        }
      }
    
    resetGeometry();

    initcells();
    if(shmup::on) shmup::init();

    while(true) {
      cell *c;
      int rspin;
      
      if(isize(cellbyid) == 0) {
        c = currentmap->gamestart();
        rspin = 0;
        }
      else {
        int32_t parent = loadInt();
        
        if(parent<0 || parent >= isize(cellbyid)) break;
        int dir = loadChar();
        cell *c2 = cellbyid[parent];
        dir  = fixspin(dir, relspin[parent], c2->type);
        c = createMov(c2, dir);
        // printf("%p:%d,%d -> %p\n", c2, relspin[parent], dir, c);
        
        // spinval becomes xspinval
        rspin = (c2->c.spin(dir) - loadChar() + MODFIXER) % c->type;
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
        short z;
        load(z);
        c->wparam = z;
        }
      else load(c->wparam);
      load(c->landparam);
      // backward compatibility
      if(vernum < 7400 && !isIcyLand(c->land)) c->landparam = HEAT(c);
      c->stuntime = loadChar();
      c->hitpoints = loadChar();

      if(patterns::whichPattern)
        mapeditor::modelcell[patterns::getpatterninfo0(c).id] = c;
      }
    
    int32_t whereami = loadInt();
    if(whereami >= 0 && whereami < isize(cellbyid))
      cwt.at = cellbyid[whereami];
    else cwt.at = currentmap->gamestart();

    for(int i=0; i<isize(cellbyid); i++) {
      cell *c = cellbyid[i];
      if(c->bardir != NODIR && c->bardir != NOBARRIERS) 
        extendBarrier(c);
      }

    for(int d=BARLEV-1; d>=0; d--)
    for(int i=0; i<isize(cellbyid); i++) {
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
      if(i<0 || i >= mapeditor::USERSHAPEGROUPS) break;
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

  cell *drawcell;

#if CAP_EDIT
  int paintwhat = 0;
  int painttype = 0;
  int radius = 0;
  string paintwhat_str = "clear monster";
  
  cellwalker copysource;
  
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

  undo_info& lastUndo() { return undo[isize(undo)-1]; }
  
  void undoLock() {
    if(!isize(undo) || lastUndo().c) {
      undo_info i; i.c = NULL; undo.push_back(i);
      }
    }

  void applyUndo() {
    while(isize(undo) && !lastUndo().c) undo.pop_back();
    while(isize(undo)) {
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
    if(checkEq(undo[isize(undo)-1])) undo.pop_back();
    }
  
  int itc(int k) {
    if(k == 0) return 0;
    if(k == 1) return 0x40;
    if(k == 2) return 0x80;
    if(k == 3) return 0xFF;
    return 0x20;
    }

  bool choosefile = false;

  void displayFunctionKeys() {
    int fs = min(vid.fsize + 5, vid.yres/26);
    displayButton(8, vid.yres-8-fs*11, XLAT("F1 = help"), SDLK_F1, 0);
    displayButton(8, vid.yres-8-fs*10, XLAT("F2 = save"), SDLK_F2, 0);
    displayButton(8, vid.yres-8-fs*9, XLAT("F3 = load"), SDLK_F3, 0);
    displayButton(8, vid.yres-8-fs*7, XLAT("F5 = restart"), SDLK_F5, 0);
    displayButton(8, vid.yres-8-fs*6, XLAT("F6 = HQ shot"), SDLK_F6, 0);
    displayButton(8, vid.yres-8-fs*5, XLAT("F7 = player on/off"), SDLK_F7, 0);
#if CAP_SVG
    displayButton(8, vid.yres-8-fs*4, XLAT("F8 = SVG shot"), SDLK_F8, 0);
#endif
    displayButton(8, vid.yres-8-fs*3, XLAT("SPACE = map/graphics"), ' ', 0);
    displayButton(8, vid.yres-8-fs*2, XLAT("ESC = return to the game"), SDLK_ESCAPE, 0);
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
      displayButton(8, 8+fs*12, XLAT("f = flip %1", ONOFF(copysource.mirrored)), 'u', 0);
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
  
  eShapegroup drawcellShapeGroup() {
    if(drawcell == cwt.at && drawplayer) return sgPlayer;
    if(drawcell->monst) return sgMonster;
    if(drawcell->item) return sgItem;
    return sgFloor;
    }
  
  int drawcellShapeID() {
    if(drawcell == cwt.at && drawplayer) return vid.cs.charid;
    if(drawcell->monst) return drawcell->monst;
    if(drawcell->item) return drawcell->item;
    auto si = patterns::getpatterninfo0(drawcell);
    return si.id + patterns::subcode(drawcell, si);
    }

  bool editingShape(eShapegroup group, int id) {
    if(group != mapeditor::drawcellShapeGroup()) return false;
    return id == drawcellShapeID();
    }

  void editCell(const pair<cellwalker, cellwalker>& where) {
    cell *c = where.first.at;
    int cdir = where.first.spin;
    saveUndo(c);
    switch(painttype) {
      case 0:
        c->monst = eMonster(paintwhat);
        c->hitpoints = 3;
        c->stuntime = 0;
        c->mondir = cdir;
        
        if((isWorm(c) || isIvy(c) || isMutantIvy(c)) && c->move(cdir) && 
          !isWorm(c->move(cdir)) && !isIvy(c->move(cdir)))
          c->mondir = NODIR;
        
        if(c->monst == moMimic) {
          c->monst = moNone;
          mirror::createMirror(cellwalker(c, cdir, true), 0);
          c->monst = moMimic;
          }
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
        cell *copywhat = where.second.at;
        c->wall = copywhat->wall;
        c->item = copywhat->item;
        c->land = copywhat->land;
        c->monst = copywhat->monst;
        c->landparam = copywhat->landparam;
        c->wparam = copywhat->wparam;
        c->hitpoints = copywhat->hitpoints;
        c->stuntime = copywhat->stuntime; 
        if(copywhat->mondir == NODIR) c->mondir = NODIR;
        else c->mondir = ((where.first.mirrored == where.second.mirrored ? 1 : -1) * (copywhat->mondir - where.second.spin) + cdir + MODFIXER) % c->type;
        break;
      }
    checkUndo();
    }
  
  vector<pair<cellwalker, cellwalker> > spill_list;
  
  void list_spill(cellwalker tgt, cellwalker src, manual_celllister& cl) {
    spill_list.clear(); 
    spill_list.emplace_back(tgt, src);
    int crad = 0, nextstepat = 0;
    for(int i=0; i<isize(spill_list); i++) {
      if(i == nextstepat) {
        crad++; nextstepat = isize(spill_list);
        if(crad > radius) break;
        }
      auto sd = spill_list[i];
      for(int i=0; i<sd.first.at->type; i++) {
        auto sd2 = sd;
        sd2.first = sd2.first + i + wstep;
        if(!cl.add(sd2.first.at)) continue;
        if(sd2.second.at) {
          sd2.second = sd2.second + i + wstep;
          if(sd2.second.at->land == laNone) continue;
          }
        spill_list.push_back(sd2);
        }
      }
    }

  #if CAP_SDL
  void saveHighQualityShotX() {
    static string hqfile = "hqshot.png";
    if(anyshiftclick) 
      saveHighQualityShot();
    else
      dialog::openFileDialog(hqfile, XLAT("F6 = HQ shot"), ".png", [] () {
        saveHighQualityShot(hqfile.c_str());
        return true;
        });
    }
  
  void saveSvgShotX() {
    static string hqfile = "svgshot.svg";
    if(anyshiftclick) 
      svg::render();
    else
      dialog::openFileDialog(hqfile, XLAT("F8 = SVG shot"), ".svg", [] () {
        svg::render(hqfile.c_str());
        return true;
        });
    }
  #endif
  
  void editAt(cellwalker where, manual_celllister& cl) {

    if(painttype == 4 && radius) {
      if(where.at->type != copysource.at->type) return;
      if(where.spin<0) where.spin=0;
      if(!nonbitrunc && !ctof(mouseover) && ((where.spin&1) != (copysource.spin&1)))
        where += 1;
      }
    if(painttype != 4) copysource.at = NULL;
    list_spill(where, copysource, cl);
    
    for(auto& st: spill_list)
      editCell(st);
    }
  
  void allInPattern(cellwalker where) {

    manual_celllister cl;
    if(!patterns::whichPattern) {
      editAt(where, cl);
      return;
      }

    cl.add(where.at);
    
    int at = 0;
    while(at < isize(cl.lst)) {
      cell *c2 = cl.lst[at];
      at++;
      
      forCellEx(c3, c2) cl.add(c3);
      }
    
    auto si = patterns::getpatterninfo0(where.at);
    int cdir = where.spin;
    if(cdir >= 0) cdir = cdir - si.dir;
    
    for(cell* c2: cl.lst) {
      auto si2 = patterns::getpatterninfo0(c2);
      if(si2.id == si.id) {
        editAt(cellwalker(c2, cdir>=0 ? fixdir(cdir + si2.dir, c2) : -1), cl);
        modelcell[si2.id] = c2;
        }
      }
    }
  
  cellwalker mouseover_cw(bool fix) {
    int d = neighborId(mouseover, mouseover2);
    if(d == -1 && fix) d = hrand(mouseover->type);
    return cellwalker(mouseover, d);
    }
  
  void showList() {
    dialog::v.clear();
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
          else if(m == moDragonHead) dialog::vpush(i, "Dragon Head");
          else dialog::vpush(i, minf[i].name);
          }
        break;
      case 1:
        for(int i=0; i<ittypes; i++) dialog::vpush(i, iinf[i].name);
        break;
      case 2:
        for(int i=0; i<landtypes; i++) dialog::vpush(i, linf[i].name);
        break;
      case 3:
        for(int i=0; i<walltypes; i++) if(i != waChasmD) dialog::vpush(i, winf[i].name);
        break;
      }
    // sort(v.begin(), v.end());
    
    if(dialog::infix != "") mouseovers = dialog::infix;
    
    int q = dialog::v.size();
    int percolumn = vid.yres / (vid.fsize+5) - 4;
    int columns = 1 + (q-1) / percolumn;
    
    for(int i=0; i<q; i++) {
      int x = 16 + (vid.xres * (i/percolumn)) / columns;
      int y = (vid.fsize+5) * (i % percolumn) + vid.fsize*2;
      
      int actkey = 1000 + i;
      string vv = dialog::v[i].first;
      if(i < 9) { vv += ": "; vv += ('1' + i); }
      
      displayButton(x, y, vv, actkey, 0);
      }
    keyhandler = [] (int sym, int uni) {
      if(uni >= '1' && uni <= '9') uni = 1000 + uni - '1';
      if(sym == SDLK_RETURN || sym == SDLK_KP_ENTER || sym == '-' || sym == SDLK_KP_MINUS) uni = 1000;
      for(int z=0; z<isize(dialog::v); z++) if(1000 + z == uni) {
        paintwhat = dialog::v[z].second;
        paintwhat_str = dialog::v[z].first;
        mousepressed = false;
        popScreen();
        return;
        }
      if(dialog::editInfix(uni)) ;
      else if(doexiton(sym, uni)) popScreen();
      };    
    }

  void handleKeyMap(int sym, int uni) {
    handlePanning(sym, uni);

    // left-clicks are coded with '-', and right-clicks are coded with sym F1
    if(uni == '-' && !holdmouse) undoLock();
    if(uni == '-' && mouseover) {
      allInPattern(mouseover_cw(false));
      holdmouse = true;
      }
    
    if(mouseover) for(int i=0; i<mouseover->type; i++) createMov(mouseover, i);
    if(uni == 'u') applyUndo();
    else if(uni == 'v' || sym == SDLK_F10 || sym == SDLK_ESCAPE) popScreen();
    else if(uni >= '0' && uni <= '9') radius = uni - '0';
    else if(uni == 'm') pushScreen(showList), painttype = 0, dialog::infix = "";
    else if(uni == 'i') pushScreen(showList), painttype = 1, dialog::infix = "";
    else if(uni == 'l') pushScreen(showList), painttype = 2, dialog::infix = "";
    else if(uni == 'w') pushScreen(showList), painttype = 3, dialog::infix = "";
    else if(uni == 'r') pushScreen(patterns::showPattern);
    else if(uni == 't' && mouseover) {
      playermoved = true;
      cwt = mouseover_cw(true);
      }
    else if(uni == 'b') painttype = 5, paintwhat_str = XLAT("boundary");
    else if(uni == 'p') {
      painttype = 6;
      paintwhat_str = "paint";
      dialog::openColorDialog((unsigned&)(paintwhat = (painttype ==6 ? paintwhat : 0x808080)));
      }
    else if(sym == SDLK_F5) {
      restart_game();
      }
    else if(sym == SDLK_F2) {
      dialog::openFileDialog(levelfile, XLAT("level to save:"), ".lev", [] () {
        if(mapstream::saveMap(levelfile.c_str())) {
          addMessage(XLAT("Map saved to %1", levelfile));
          return true;
          }
        else {
          addMessage(XLAT("Failed to save map to %1", levelfile));
          return false;
          }
        });
      }
    else if(sym == SDLK_F3)
      dialog::openFileDialog(levelfile, XLAT("level to load:"), ".lev", [] () {
        if(mapstream::loadMap(levelfile.c_str())) {
          addMessage(XLAT("Map loaded from %1", levelfile));
          return true;
          }
        else {
          addMessage(XLAT("Failed to load map from %1", levelfile));
          return false;
          }
        });
#if CAP_SDL
    else if(sym == SDLK_F6) {
      saveHighQualityShotX();
      }
#endif
#if CAP_SVG
    else if(sym == SDLK_F8) {
      saveSvgShotX();
      }
#endif
    else if(sym == SDLK_F7) {
      drawplayer = !drawplayer;
      }
    else if(uni == 'c') {
      copysource = mouseover_cw(true);
      painttype = 4;
      paintwhat_str = XLAT("copying");
      }
    else if(uni == 'f') {
      copysource.mirrored = !copysource.mirrored;
      }
    else if(uni == 'h' || sym == SDLK_F1) 
      gotoHelp(mehelptext());
    else if(uni == ' ') {
      popScreen();
      pushScreen(showDrawEditor);
      initdraw(mouseover ? mouseover : cwt.at);
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
  // fake key sent to change the color
  static const int COLORKEY = (-10000); 

  transmatrix drawtrans, drawtransnew;

  void loadShape(int sg, int id, hpcshape& sh, int d, int layer) {
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
  
  unsigned gridcolor = 0xC0C0C040;
  
  void drawGrid() {
    unsigned lightgrid = gridcolor;
    lightgrid -= (lightgrid & 0xFF) / 2;

    transmatrix d2 = drawtrans * rgpushxto0(ccenter) * rspintox(gpushxto0(ccenter) * coldcenter);

    for(int d=0; d<S84; d++) {
      unsigned col = (d % (S84/drawcell->type) == 0) ? gridcolor : lightgrid;
      queueline(d2 * C0, d2 * xspinpush0(M_PI*d/S42, 1), col, 4 + vid.linequality);
      }
    for(int u=2; u<=20; u++) {
      PRING(d) {
        curvepoint(d2 * xspinpush0(M_PI*d/S42, u/20.));
        }
      queuecurve((u%5==0) ? gridcolor : lightgrid, 0, PPR_LINE);
      }
    queueline(drawtrans*ccenter, drawtrans*coldcenter, gridcolor, 4 + vid.linequality);
    }

  void drawHandleKey(int sym, int uni);
  
#if CAP_TEXTURE
  static ld brush_sizes[10] = {
    0.001, 0.002, 0.005, 0.0075, 0.01, 0.015, 0.02, 0.05, 0.075, 0.1};
  
  static unsigned texture_colors[] = {
    11,
    0x000000FF,
    0xFFFFFFFF,
    0xFF0000FF,
    0xFFFF00FF,
    0x00FF00FF,
    0x00FFFFFF,
    0x0000FFFF,
    0xFF00FFFF,
    0xC0C0C0FF,
    0x808080FF,
    0x404040FF,
    0x804000FF
    };
#endif

  bool area_in_pi = false;

  ld compute_area(hpcshape& sh) {
    ld area = 0;
    for(int i=sh.s; i<sh.e-1; i++) {
      hyperpoint h1 = hpc[i];
      hyperpoint h2 = hpc[i+1];
      if(euclid)
        area += (h2[1] + h1[1]) * (h2[0] - h1[0]) / 2;
      else {
        hyperpoint rh2 = gpushxto0(h1) * h2;
        hyperpoint rh1 = gpushxto0(h2) * h1;
        // ld a1 = atan2(h1[1], h1[0]);
        // ld a2 = atan2(h2[1], h2[0]);
        ld b1 = atan2(rh1[1], rh1[0]);
        ld b2 = atan2(rh2[1], rh2[0]);
        // C0 -> H1 -> H2 -> C0
        // at C0: (a1-a2)
        // at H1: (rh2 - a1 - M_PI)
        // at H2: (a2+M_PI - rh1)
        // total: rh2 - rh1
        // ld z = 180 / M_PI;
        ld x = b2 - b1 + M_PI;
        while(x > M_PI) x -= 2 * M_PI;
        while(x < -M_PI) x += 2 * M_PI;
        area += x;
        }
      }
    return area;
    }
    
  void showDrawEditor() {
    cmode = sm::DRAW;
    gamescreen(0);
    drawGrid();
    if(callhandlers(false, hooks_prestats)) return;

    if(!mouseout()) getcstat = '-';
    
    int sg;
    string line1, line2;
  
    usershape *us = NULL;
    
    bool intexture = false;

#if CAP_TEXTURE        
    if(texture::config.tstate == texture::tsActive) {
      sg = 16;
      line1 = "texture";
      line2 = "";
      texture::config.data.update();
      intexture = true;
      }
#else
    if(0);
#endif
    
    else {

      sg = drawcellShapeGroup();
      
      switch(sg) {
        case sgPlayer:
          line1 = XLAT("character");
          line2 = csname(vid.cs);
          break;
        
        case sgMonster:
          line1 = XLAT("monster");
          line2 = XLAT1(minf[drawcell->monst].name);
          break;
          
        case sgItem:
          line1 = XLAT("item");
          line2 = XLAT1(iinf[drawcell->item].name);
          break;
        
        case sgFloor:
          line1 = XLAT("floor");
          line2 = "#" + its(drawcellShapeID());
          /* line2 = XLAT(ishept(drawcell) ? "heptagonal" : 
            ishex1(drawcell) ? "hexagonal #1" : "hexagonal"); */
          break;        
        }
      
      us =usershapes[drawcellShapeGroup()][drawcellShapeID()];
      }
    
    int fs = min(vid.fsize + 5, vid.yres/28);

    // displayButton(8, 8+fs*9, XLAT("l = lands"), 'l', 0);
    displayfr(8, 8+fs, 2, vid.fsize, line1, 0xC0C0C0, 0);
    
    if(!intexture) {
      if(sg == sgFloor)
        displayButton(8, 8+fs*2, line2 + XLAT(" (r = complex tesselations)"), 'r', 0);
      else
        displayfr(8, 8+fs*2, 2, vid.fsize, line2, 0xC0C0C0, 0);
      displayButton(8, 8+fs*3, XLAT("l = layers: %1", its(dslayer)), 'l', 0);
      }

    if(us && isize(us->d[dslayer].list)) {
      usershapelayer& ds(us->d[dslayer]);
      displayButton(8, 8+fs*4, XLAT("1-9 = rotations: %1", its(ds.rots)), '1' + (ds.rots % 9), 0);
      displayButton(8, 8+fs*5, XLAT(ds.sym ? "0 = symmetry" : "0 = asymmetry"), '0', 0);

      displayfr(8, 8+fs*7, 2, vid.fsize, XLAT("%1 vertices", its(isize(ds.list))), 0xC0C0C0, 0);
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
      if(mousekey == 'g')
        displayButton(8, 8+fs*16, XLAT("p = grid color"), 'p', 0);
      else
        displayButton(8, 8+fs*16, XLAT("p = paint"), 'p', 0);

      }
#if CAP_TEXTURE
    else if(texture::config.tstate == texture::tsActive) {
      displayButton(8, 8+fs*2, XLAT(texture::texturesym ? "0 = symmetry" : "0 = asymmetry"), '0', 0);
      if(mousekey == 'g')
        displayButton(8, 8+fs*16, XLAT("p = grid color"), 'p', 0);
      else
        displayButton(8, 8+fs*16, XLAT("p = color"), 'p', 0);
      displayButton(8, 8+fs*4, XLAT("b = brush size: %1", fts4(texture::penwidth)), 'b', 0);
      displayButton(8, 8+fs*5, XLAT("u = undo"), 'u', 0);
      displaymm('d', 8, 8+fs*7, 2, vid.fsize, XLAT("d = draw"), 0);
      displaymm('l', 8, 8+fs*8, 2, vid.fsize, XLAT("l = line"), 0);
      displaymm('c', 8, 8+fs*9, 2, vid.fsize, XLAT("c = circle"), 0);
      int s = isize(texture::config.data.pixels_to_draw);
      if(s) displaymm(0, 8, 8+fs*11, 2, vid.fsize, its(s), 0);
      }
#endif
    else {
      displaymm('n', 8, 8+fs*5, 2, vid.fsize, XLAT("'n' to start"), 0);
      displaymm('u', 8, 8+fs*6, 2, vid.fsize, XLAT("'u' to load current"), 0);
      if(mousekey == 'a' || mousekey == 'd' || mousekey == 'd' ||
        mousekey == 'c') mousekey = 'n';
      }

    displaymm('g', vid.xres-8, 8+fs*4, 2, vid.fsize, XLAT("g = grid"), 16);

#if CAP_TEXTURE    
    if(intexture) for(int i=0; i<10; i++) {
      if(8 + fs * (6+i) < vid.yres - 8 - fs * 7)
        displayColorButton(vid.xres-8, 8+fs*(6+i), "###", 1000 + i, 16, 1, dialog::displaycolor(texture_colors[i+1]));

      if(displayfr(vid.xres-8 - fs * 3, 8+fs*(6+i), 0, vid.fsize, its(i+1), texture::penwidth == brush_sizes[i] ? 0xFF8000 : 0xC0C0C0, 16))
        getcstat = 2000+i;
      }

    if(texture::config.tstate != texture::tsActive)
      displaymm('e', vid.xres-8, 8+fs, 2, vid.fsize, XLAT("e = edit this"), 16);
#endif

    if(!mouseout()) {
      transmatrix T = inverse(drawtrans * rgpushxto0(ccenter));
      hyperpoint mh = spintox(gpushxto0(ccenter) * coldcenter) * T * mouseh;
      displayfr(vid.xres-8, vid.yres-8-fs*6, 2, vid.fsize, XLAT("x: %1", fts4(mh[0])), 0xC0C0C0, 16);
      displayfr(vid.xres-8, vid.yres-8-fs*5, 2, vid.fsize, XLAT("y: %1", fts4(mh[1])), 0xC0C0C0, 16);
      displayfr(vid.xres-8, vid.yres-8-fs*4, 2, vid.fsize, XLAT("z: %1", fts4(mh[2])), 0xC0C0C0, 16);
      displayfr(vid.xres-8, vid.yres-8-fs*2, 2, vid.fsize, XLAT("r: %1", fts4(hdist0(mh))), 0xC0C0C0, 16);
      displayfr(vid.xres-8, vid.yres-8-fs, 2, vid.fsize, XLAT("ϕ: %1°", fts4(-atan2(mh[1], mh[0]) * 360 / 2 / M_PI)), 0xC0C0C0, 16);
      }

    if(us) {
      auto& sh = us->d[dslayer].sh;
      if(sh.e >= sh.s + 3)
        displayButton(vid.xres-8, vid.yres-8-fs*8, XLAT("area: %1", area_in_pi ? fts4(compute_area(sh) / M_PI) + "π" : fts4(compute_area(sh))), 'w', 16);
      }

    
    displayFunctionKeys();
    
    keyhandler = drawHandleKey;
    }
  
  bool rebuildPolys = false;

  void loadShapes(int sg, int id) {
    delete usershapes[sg][id];
    usershapes[sg][id] = NULL;

    initquickqueue();
    
    dynamicval<bool> ws(mmspatial, false);
    
    if(sg == 0) {
      multi::cpid = id, drawMonsterType(moPlayer, drawcell, Id, 0xC0C0C0, 0);
      }
    else if(sg == 1) {
      drawMonsterType(eMonster(id), drawcell, Id, minf[id].color, 0);
      }
    else if(sg == 2) {
      drawItemType(eItem(id), drawcell, Id, iinf[id].color, 0, false);
      }
    else {
      draw_qfi(drawcell, Id, 0, PPR_FLOOR);
      }

    sortquickqueue();
      
    int layer = 0;
    
    initShape(sg, id);
    
    for(int i=0; i<isize(ptds); i++) { 
      auto& ptd = ptds[i];
      if(ptd.kind != pkPoly) continue;
      
      auto& p = ptd.u.poly;
      int cnt = p.cnt;
      
      usershapelayer *dsCur = &usershapes[sg][id]->d[layer];
      dsCur->list.clear();
      dsCur->color = ptd.col;
      dsCur->sym = false;
      dsCur->rots = 1;
      
      for(auto& v: symmetriesAt)
        if(v[0] == p.offset) {
          dsCur->rots = v[1];
          dsCur->sym = v[2] == 2;
          }
        
      int d = dsCur->rots * (dsCur->sym ? 2 : 1);
      
      for(int i=0; i < cnt/d; i++)
        dsCur->list.push_back(p.V * glhr::gltopoint((*p.tab)[i+p.offset]));
      
      layer++;      
      if(layer == USERLAYERS) break;
      }
    rebuildPolys = true;
    }
    
  void applyToShape(int sg, int id, int uni, hyperpoint mh) {
    bool haveshape = usershapes[sg][id];
    bool xnew = false;
    
    if(uni == '-') uni = mousekey;
    
    if(!haveshape) {
      if(uni == 'n')
        initShape(sg, id);
      else if(uni == 'u') ;
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

    if(uni == 'u') 
      loadShapes(sg, id);

    if(uni == 'a' && haveshape) {
      mh = spin(2*M_PI*-ew.rotid/dsCur->rots) * mh;
      if(ew.symid) mh = Mirror * mh;
    
      if(ew.pointid < 0 || ew.pointid >= isize(dsCur->list)) 
        ew.pointid = isize(dsCur->list)-1, ew.side = 1;

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

      if(i < 0 || i >= isize(dsCur->list)) return;

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
      if(err != 1) printf("Warning: read error\n");
      h[i] = d;
      }
    return h;
    }
  
  string drawHelpLine() {
    return XLAT("vector graphics editor");
    }
  
  bool onelayeronly;
  
  bool loadPicFile(const string& s) {
    FILE *f = fopen(picfile.c_str(), "rt");
    if(!f) {
      addMessage(XLAT("Failed to load pictures from %1", picfile));
      return false;
      }
    int err;
    char buf[200];
    if(!fgets(buf, 200, f)) { 
      addMessage(XLAT("Failed to load pictures from %1", picfile));
      fclose(f); return false; 
      }
    int vernum; err = fscanf(f, "%x", &vernum);
    printf("vernum = %x\n", vernum);

    if(vernum >= 0xA0A0) {
      int tg, wp;
      int nt;
      hr::ignore(fscanf(f, "%d%d%d%d\n", &tg, &nt, &wp, &patterns::subpattern_flags));
      patterns::whichPattern = patterns::ePattern(wp);
      if(tg != geometry) { targetgeometry = eGeometry(tg); stop_game_and_switch_mode(rg::geometry); }
      if(bool(nt) != nonbitrunc) stop_game_and_switch_mode(rg::bitrunc);
      start_game();
      }

    while(true) {
      int i, j, l, sym, rots, color, siz;
      err = fscanf(f, "%d%d%d%d%d%x%d", &i, &j, &l, &sym, &rots, &color, &siz);
      if(i == -1 || err < 6) break;
      if(siz < 0 || siz > 1000) break;
      
      if(i >= 4) {
        if(i < 8) patterns::whichPattern = patterns::ePattern("xxxxfpzH"[i]);
        patterns::subpattern_flags = 0;
        i = 3;
        }

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
    return true;
    }
  
  bool savePicFile(const string& s) {
    FILE *f = fopen(picfile.c_str(), "wt");
    if(!f) {
      addMessage(XLAT("Failed to save pictures to %1", picfile));
      return false;
      }
    fprintf(f, "HyperRogue saved picture\n");
    fprintf(f, "%x\n", VERNUM_HEX);
    if(VERNUM_HEX >= 0xA0A0)
      fprintf(f, "%d %d %d %d\n", geometry, nonbitrunc, patterns::whichPattern, patterns::subpattern_flags);
    for(int i=0; i<USERSHAPEGROUPS; i++) for(auto usp: usershapes[i]) {
      usershape *us = usp.second;
      if(!us) continue;
      
      for(int l=0; l<USERLAYERS; l++) if(isize(us->d[l].list)) {
        usershapelayer& ds(us->d[l]);
        fprintf(f, "\n%d %d %d %d %d %6x %d\n", 
          i, usp.first, l, ds.sym, ds.rots, ds.color, int(isize(ds.list)));
        writeHyperpoint(f, ds.shift);
        writeHyperpoint(f, ds.spin);
        fprintf(f,"\n");
        for(int i=0; i<isize(ds.list); i++)
          writeHyperpoint(f, ds.list[i]);
        }
      }
    fprintf(f, "\n-1\n");
    fclose(f);
    addMessage(XLAT("Pictures saved to %1", picfile));
    return true;
    }

  hyperpoint lstart;
  cell *lstartcell;

  void drawHandleKey(int sym, int uni) {

    handlePanning(sym, uni);
  
    if(uni == SETMOUSEKEY) {
       if(mousekey == newmousekey)
         mousekey = '-';
       else
         mousekey = newmousekey;
       }
    
    if(uni == 'w') area_in_pi = !area_in_pi;

    if(uni == 'r') {
      pushScreen(patterns::showPattern);
      if(drawplayer) 
        addMessage(XLAT("Hint: use F7 to edit floor under the player"));
      }
    
    hyperpoint mh = inverse(drawtrans) * mouseh;

    bool clickused = false;
    
    if((uni == 'p' && mousekey == 'g') || (uni == 'g' && coldcenter == ccenter && ccenter == mh)) {
      static unsigned grid_colors[] = {
        8,
        0x00000040,
        0xFFFFFF40,
        0xFF000040,
        0x0000F040,
        0x00000080,
        0xFFFFFF80,
        0xFF000080,
        0x0000F080,
        };
      dialog::openColorDialog(gridcolor, grid_colors);
      clickused = true;
      }
    
    char mkuni = uni == '-' ? mousekey : uni;
    
    if(mkuni == 'g') 
      coldcenter = ccenter, ccenter = mh, clickused = true;
    
    if(uni == 'd' || uni == 'l' || uni == 'c')
      mousekey = uni;

    if(uni == ' ' && (cheater || autocheat)) {
      popScreen();
      pushScreen(showMapEditor);
      }

    if(sym == SDLK_F7) {
      drawplayer = !drawplayer;
      }

#if CAP_SDL    
    if(sym == SDLK_F6) {
      saveHighQualityShotX();
      }
#endif

#if CAP_SVG    
    if(sym == SDLK_F8) {
      saveSvgShotX();
      }
#endif
    
    if(sym == SDLK_ESCAPE) popScreen();

    if(sym == SDLK_F1) {
      gotoHelp(drawhelptext());
      }

    if(sym == SDLK_F10) popScreen();

#if CAP_TEXTURE
    if(texture::config.tstate == texture::tsActive) {
    
      int tcolor = (texture::config.paint_color >> 8) | ((texture::config.paint_color & 0xFF) << 24);
      
      if(uni == '-' && !clickused) {
        if(mousekey == 'l' || mousekey == 'c') {
          if(!holdmouse) lstart = mouseh, lstartcell = mouseover, holdmouse = true;
          }
        else {
          if(!holdmouse) texture::config.data.undoLock();
          texture::drawPixel(mouseover, mouseh, tcolor);
          holdmouse = true; lstartcell = NULL;
          }
        }
      
      if(sym == PSEUDOKEY_RELEASE) {
        printf("release\n");
        if(mousekey == 'l') { 
          texture::config.data.undoLock();
          texture::where = mouseover;
          texture::drawPixel(mouseover, mouseh, tcolor);
          texture::drawLine(mouseh, lstart, tcolor);
          lstartcell = NULL;
          }
        if(mousekey == 'c') { 
          texture::config.data.undoLock();
          ld rad = hdist(lstart, mouseh);
          int circp = int(1 + 3 * (circlelength(rad) / texture::penwidth));
          if(circp > 1000) circp = 1000;
          transmatrix T = rgpushxto0(lstart);
          texture::where = lstartcell;
          for(int i=0; i<circp; i++)
            texture::drawPixel(T * xspinpush0(2 * M_PI * i / circp, rad), tcolor);
          lstartcell = NULL;
          }
        }
      
      if(uni >= 1000 && uni < 1010)
        texture::config.paint_color = texture_colors[uni - 1000 + 1];

      if(uni >= 2000 && uni < 2010)
        texture::penwidth = brush_sizes[uni - 2000];

      if(uni == '0')
        texture::texturesym = !texture::texturesym;

      if(uni == 'u') {
        texture::config.data.undo();
        }        

      if(uni == 'p') {
        if(!clickused)
          dialog::openColorDialog(texture::config.paint_color, texture_colors);
        }

      if(uni == 'b') 
        dialog::editNumber(texture::penwidth, 0, 0.1, 0.005, 0.02, XLAT("brush size"), XLAT("brush size"));
      }
#else
    (void)clickused;
    if(0);
#endif    

    else {
      dslayer %= USERLAYERS;

      applyToShape(drawcellShapeGroup(), drawcellShapeID(), uni, mh);

      if(uni == 'e' || (uni == '-' && mousekey == 'e')) {
        initdraw(mouseover ? mouseover : cwt.at);
        }
      if(uni == 'l') { dslayer++; dslayer %= USERLAYERS; }
      if(uni == 'L') { dslayer--; if(dslayer < 0) dslayer += USERLAYERS; }
      if(uni == 'l' - 96) onelayeronly = !onelayeronly;
    
      if(uni == 'c') ew = ewsearch;
      if(uni == 'b') autochoose = !autochoose;
      
      if(uni == 'S') {
        for(int i=0; i<USERSHAPEGROUPS; i++) for(auto usp: usershapes[i]) {
          auto us = usp.second;
          if(!us) continue;
          
          for(int l=0; l<USERLAYERS; l++) if(isize(us->d[l].list)) {
            usershapelayer& ds(us->d[l]);
            printf("// %d %d %d [%06X]\n", i, usp.first, l, ds.color);
            printf(" ID, %d, %d, ", us->d[l].rots, us->d[l].sym?2:1); 
            for(int i=0; i<isize(us->d[l].list); i++) 
              printf("%lf,%lf, ", double(us->d[l].list[i][0]), double(us->d[l].list[i][1]));
            printf("\n");
            }
          }
        }
  
      if(uni == 'p') {
        dialog::openColorDialog(colortouse);
        dialog::reaction = [] () {
          drawHandleKey(COLORKEY, COLORKEY);
          };
        }

      if(sym == SDLK_F2) 
        dialog::openFileDialog(picfile, XLAT("pics to save:"), ".pic", 
          [] () {
            return savePicFile(picfile);
            });
      
      if(sym == SDLK_F3) 
        dialog::openFileDialog(picfile, XLAT("pics to load:"), ".pic", 
          [] () {
            return loadPicFile(picfile);
            });

      if(sym == SDLK_F5) {
        for(int i=0; i<USERSHAPEGROUPS; i++) {
          for(auto us: usershapes[i])
            if(us.second) delete us.second;
          usershapes[i].clear();
          }
        }
      }

    if(rebuildPolys)
      buildpolys(), rebuildPolys = false;
    }

  auto hooks = addHook(clearmemory, 0, [] () {
    if(mapeditor::painttype == 4) 
      mapeditor::painttype = 0, mapeditor::paintwhat = 0,
      mapeditor::paintwhat_str = "clear monster";
    mapeditor::copysource.at = NULL;
    mapeditor::undo.clear();
    if(!cheater) patterns::displaycodes = false;
    if(!cheater) patterns::whichShape = 0;
    modelcell.clear();
    }) + 
  addHook(hooks_removecells, 0, [] () {
    modelcell.clear();
    set_if_removed(mapeditor::copysource.at, NULL);
    });;;  
#endif

  void initdraw(cell *c) {
    mapeditor::drawcell = c;
    ew.c = c;
    ew.rotid = 0;
    ew.symid = 0;
    ew.pointid = -1;
    ew.side = 0;
    ewsearch = ew;
    }
  
  transmatrix textrans;

#if CAP_TEXTURE
  void queue_hcircle(transmatrix Ctr, ld radius) {
    vector<hyperpoint> pts;
    int circp = int(6 * pow(2, vid.linequality));
    if(radius > 0.04) circp *= 2;
    if(radius > .1) circp *= 2; 
    
    for(int j=0; j<circp; j++)
      pts.push_back(Ctr * xspinpush0(M_PI*j*2/circp, radius));
    for(int j=0; j<circp; j++) curvepoint(pts[j]);
    curvepoint(pts[0]);
    queuecurve(texture::config.paint_color, 0, PPR_LINE);
    }
#endif

  bool haveUserShape(eShapegroup group, int id) {
  #if !CAP_EDIT
    return false;
  #else
    return usershapes[group].count(id) && usershapes[group][id];
  #endif
    }

  bool drawUserShape(const transmatrix& V, eShapegroup group, int id, int color, cell *c, PPR prio) {
  #if !CAP_EDIT
    return false;
  #else
  
    // floors handled separately
    if(c && c == drawcell && editingShape(group, id) && group != sgFloor)
      drawtrans = V;

    usershape *us = usershapes[group][id];
    if(us) {  
      for(int i=0; i<USERLAYERS; i++) {
        if(i != dslayer && onelayeronly) continue;
        usershapelayer& ds(us->d[i]);
        hpcshape& sh(ds.sh);
    
        if(sh.s != sh.e) 
          queuepolyat(V, sh, ds.color ? ds.color : color, prio);
        }
      }
  
    if(cmode & sm::DRAW) {

#if CAP_TEXTURE    
      if(texture::config.tstate == texture::tsActive && lmouseover && !mouseout() && (lstartcell || !holdmouse) && c) {
        cell *ls = lstartcell ? lstartcell : lmouseover;
        auto sio = patterns::getpatterninfo0(ls);
        auto sih = patterns::getpatterninfo0(c);
        
        if(sio.id == sih.id) {
          if(c == ls)
            textrans = inverse(V * applyPatterndir(ls, sio));
          
          transmatrix mh = textrans * rgpushxto0(mouseh);
          transmatrix ml = textrans * rgpushxto0(lstart);
          
          for(int j=0; j<=texture::texturesym; j++)
          for(int i=0; i<c->type; i += sih.symmetries) {
            transmatrix M2 = V * applyPatterndir(c, sih) * spin(2*M_PI*i/c->type);
            if(j) M2 = M2 * Mirror;
            switch(holdmouse ? mousekey : 'd') {
              case 'c':
                queue_hcircle(M2 * ml, hdist(lstart, mouseh));
                break;
              case 'l':
                queueline(M2 * mh * C0, M2 * ml * C0, texture::config.paint_color, 4 + vid.linequality, PPR_LINE);
                break;
              default:
                queue_hcircle(M2 * mh, texture::penwidth);
              }                
            }
          }
        }
#endif

      if(mapeditor::editingShape(group, id)) {
  
        /* for(int a=0; a<isize(ds.list); a++) {
          hyperpoint P2 = V * ds.list[a];
    
          int xc, yc, sc;
          getcoord(P2, xc, yc, sc);
          queuechr(xc, yc, sc, 10, 'x', 
            a == 0 ? 0x00FF00 : 
            a == isize(ds.list)-1 ? 0xFF0000 :
            0xFFFF00);
          } */
        
        if(!us) return false;
     
        usershapelayer &ds(us->d[mapeditor::dslayer]);
        
        hyperpoint mh = inverse(mapeditor::drawtrans) * mouseh;
    
        for(int a=0; a<ds.rots; a++) 
        for(int b=0; b<(ds.sym?2:1); b++) {
    
          if(mouseout()) break;
    
          hyperpoint P2 = V * spin(2*M_PI*a/ds.rots) * (b?Mirror*mh:mh);
        
          queuechr(P2, 10, 'x', 0xFF00FF);
          }
        
        if(isize(ds.list) == 0) return us;
        
        hyperpoint Plast = V * spin(-2*M_PI/ds.rots) * (ds.sym?Mirror*ds.list[0]:ds.list[isize(ds.list)-1]);
        int state = 0;
        int gstate = 0;
        double dist2 = 0;
        hyperpoint lpsm;
        
        for(int a=0; a<ds.rots; a++) 
        for(int b=0; b<(ds.sym?2:1); b++) {
        
          hyperpoint mh2 = spin(2*M_PI*-ew.rotid/ds.rots) * mh;
          if(ew.symid) mh2 = Mirror * mh2;
          hyperpoint pseudomouse = V * spin(2*M_PI*a/ds.rots) * mirrorif(mh2, b);      
        
          for(int t=0; t<isize(ds.list); t++) {
            int ti = b ? isize(ds.list)-1-t : t;
  
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
                if(ew.side == (b==1)) queueline(pseudomouse, Plast, 0x90000080);
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
      
      }
  
    return us;
  #endif
    }

  }

#if CAP_EDIT
string levelfile = "hyperrogue.lev";
const char *loadlevel = NULL;
string picfile = "hyperrogue.pic";

#if CAP_COMMANDLINE

int read_editor_args() {
  using namespace arg;
  if(argis("-lev")) { shift(); levelfile = args(); }
  else if(argis("-pic")) { shift(); picfile = args(); }
  else if(argis("-load")) { PHASE(3); shift(); mapstream::loadMap(args()); }
  else if(argis("-picload")) { PHASE(3); shift(); mapeditor::loadPicFile(args()); }
  else if(argis("-noplayer")) mapeditor::drawplayer = !mapeditor::drawplayer;
  else if(argis("-canvas")) {
    PHASEFROM(2);
    stop_game();
    firstland = specialland = laCanvas;
    shift();
    if(args() == "i") canvas_invisible = !canvas_invisible;
    else if(args().size() == 1) patterns::whichCanvas = args()[0];
    else patterns::canvasback = arghex();
    stop_game_and_switch_mode(rg::nothing);
    }
  else return 1;
  return 0;
  }

auto ah_editor = addHook(hooks_args, 0, read_editor_args);
#endif
#endif
}
