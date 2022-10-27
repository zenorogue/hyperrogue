// Hyperbolic Rogue -- map editor and vector graphics editor
// Copyright (C) 2011-2019 Zeno Rogue, see 'hyper.cpp' for details

/** \file mapedit.cpp
 *  \brief HyperRogue editors (map and vector graphics)
 */

#include "hyper.h"
namespace hr {

EX namespace mapeditor {

  /* changes when the map is changed */
  EX int map_version;

  EX bool drawing_tool;
  EX bool intexture;
  EX bool snapping;
  EX bool building_mode = true;

  #if HDR
  enum eShapegroup { sgPlayer, sgMonster, sgItem, sgFloor, sgWall };
  static const int USERSHAPEGROUPS = 5;
  #endif
  
  EX color_t dtfill = 0;
  EX color_t dtcolor = 0x000000FF;
  EX ld dtwidth = .02;

  /* drawing_tool shapes */
  struct dtshape {
    cell *where;
    color_t col, fill;
    ld lw;
    
    virtual void rotate(const transmatrix& T) = 0;
    virtual void save(hstream& hs) = 0;
    virtual dtshape* load(hstream& hs) = 0;
    virtual void draw(const shiftmatrix& V) = 0;
    virtual ld distance(hyperpoint h) = 0;
    virtual ~dtshape() {}
    };
  
  struct dtline : dtshape {
  
    hyperpoint s, e;

    void rotate(const transmatrix& T) override {
      s = T * s;
      e = T * e;
      }
    void save(hstream& hs) override {
      hs.write<char>(1);
      hs.write(s);
      hs.write(e);
      }
    dtshape *load(hstream& hs) override {
      hs.read(s);
      hs.read(e);
      return this;
      }
    void draw(const shiftmatrix& V) override {
      queueline(V*s, V*e, col, 2 + vid.linequality);
      }
    ld distance(hyperpoint h) override {
      return hdist(s, h) + hdist(e, h) - hdist(s, e);
      }
    };
  
  struct dtcircle : dtshape {
  
    hyperpoint s; ld radius;

    void rotate(const transmatrix& T) override {
      s = T * s;
      }
    void save(hstream& hs) override {
      hs.write<char>(2);
      hs.write(s);
      hs.write(radius);
      }
    dtshape *load(hstream& hs) override {
      hs.read(s);
      hs.read(radius);
      return this;
      }
    void draw(const shiftmatrix& V) override {
      ld len = sin_auto(radius);
      int ll = ceil(360 * len);
      shiftmatrix W = V * rgpushxto0(s);
      for(int i=0; i<=ll; i++)
        curvepoint(xspinpush0(360*degree*i/ll, radius));
      queuecurve(W, col, fill, PPR::LINE);
      }

    ld distance(hyperpoint h) override {
      return abs(hdist(s, h) - radius);
      }
    };

  struct dttext : dtshape {
    hyperpoint where;
    ld size;
    string caption;

    void rotate(const transmatrix& T) override {
      where = T * where;
      }

    void save(hstream& hs) override {
      hs.write<char>(4);
      hs.write(where);
      hs.write(size);
      hs.write(caption);
      }

    dtshape *load(hstream& hs) override {
      hs.read(where);
      hs.read(size);
      hs.read(caption);
      return this;
      }

    void draw(const shiftmatrix& V) override {
      queuestr(V * rgpushxto0(where), size, caption, col);
      }
    
    ld distance(hyperpoint h) override {
      return hdist(h, where);
      }
    };
  
  struct dtfree : dtshape {
  
    vector<hyperpoint> lh;

    void rotate(const transmatrix& T) override {
      for(auto& h: lh) h = T * h;
      }
    void save(hstream& hs) override {
      hs.write<char>(3);
      hs.write(lh);
      }
    dtshape *load(hstream& hs) override {
      hs.read(lh);
      return this;
      }
    void draw(const shiftmatrix& V) override {
      for(auto& p: lh) curvepoint(p);
      queuecurve(V, col, fill, PPR::LINE);
      }    

    ld distance(hyperpoint h) override {
      ld mind = 99;
      for(auto h1: lh) mind = min(mind, hdist(h, h1));
      return mind;
      }
    };
  
  vector<unique_ptr<dtshape>> dtshapes;
  
  EX void clear_dtshapes() { dtshapes.clear(); }
  
  EX shiftpoint full_mouseh() {
    #if CAP_EDIT
    if(GDIM == 3) return find_mouseh3();
    if(snapping) return mouse_snap();
    #endif
    return mouseh;
    }

  EX void draw_dtshapes() {
#if CAP_EDIT
    for(auto& shp: dtshapes) {
      if(shp == nullptr) continue;
      auto& sh = *shp;
      cell*& c = sh.where;
      for(const shiftmatrix& V: current_display->all_drawn_copies[c]) {
        dynamicval<ld> lw(vid.linewidth, vid.linewidth * sh.lw);
        sh.draw(V);
        }
      }

    if(drawing_tool && (cmode & sm::DRAW)) {
      shiftpoint moh = full_mouseh();
      dynamicval<ld> lw(vid.linewidth, vid.linewidth * dtwidth * 100);
      if(holdmouse && mousekey == 'c')
        queue_hcircle(rgpushxto0(lstart), hdist(lstart, moh));
      else if(holdmouse && mousekey == 'l')
        queueline(lstart, moh, dtcolor, 4 + vid.linequality, PPR::LINE);
      else if(!holdmouse) {
        shiftmatrix T = rgpushxto0(moh);
        queueline(T * xpush0(-.1), T * xpush0(.1), dtcolor);
        queueline(T * ypush0(-.1), T * ypush0(.1), dtcolor);
        }
      }
#endif
    }
  
  /** dtshapes takes ownership of sh */
  void dt_add(cell *where, dtshape *sh) {
    sh->where = where;
    sh->col = dtcolor;
    sh->fill = dtfill;
    sh->lw = dtwidth * 100;

    dtshapes.push_back(unique_ptr<dtshape>(sh));
    }
  
  EX void dt_add_line(shiftpoint h1, shiftpoint h2, int maxl) {
    if(hdist(h1, h2) > 1 && maxl > 0) {
      shiftpoint h3 = mid(h1, h2);
      dt_add_line(h1, h3, maxl-1);
      dt_add_line(h3, h2, maxl-1);
      return;
      }
    cell *b = centerover;
    
    auto xh1 = inverse_shift(ggmatrix(b), h1);
    virtualRebase(b, xh1);
    
    auto l = new dtline;
    l->s = xh1;
    l->e = inverse_shift(ggmatrix(b), h2);
    dt_add(b, l);
    }

  EX void dt_add_circle(shiftpoint h1, shiftpoint h2) {
    cell *b = centerover;
    
    auto d = hdist(h1, h2);
    
    auto xh1 = inverse_shift(ggmatrix(b), h1);
    virtualRebase(b, xh1);
    
    auto l = new dtcircle;
    l->s = xh1;
    l->radius = d;
    dt_add(b, l);
    }
  
  EX void dt_add_text(shiftpoint h, ld size, string cap) {
    cell *b = centerover;
    
    auto xh = inverse_shift(ggmatrix(b), h);
    virtualRebase(b, xh);
    
    auto l = new dttext;
    l->where = xh;
    l->size = size;
    l->caption = cap;
    dt_add(b, l);
    l->col >>= 8;
    }
  
  dtshape *load_shape(hstream& hs) {
    char type = hs.get<char>();
    switch(type) {
      case 1:
        return (new dtline)->load(hs);
      case 2:
        return (new dtcircle)->load(hs);
      case 3:
        return (new dtfree)->load(hs);
      case 4:
        return (new dttext)->load(hs);
      default:
        return nullptr;
      }
    }
  
  dtfree *cfree;
  cell *cfree_at;
  
  EX void dt_finish() {
    cfree = nullptr;
    cfree_at = nullptr;
    }
  
  EX void dt_add_free(shiftpoint h) {

    cell *b = centerover;
    shiftmatrix T = rgpushxto0(h);
    auto T1 = inverse_shift(ggmatrix(b), T);
    virtualRebase(b, T1);
    
    if(cfree)
      cfree->lh.push_back(inverse_shift(ggmatrix(cfree_at), tC0(T)));
    
    if(b != cfree_at && !(dtfill && cfree_at)) {
      cfree = new dtfree;
      dt_add(b, cfree);
      cfree->lh.push_back(tC0(T1));
      cfree_at = b;
      }
    }
  
  EX void dt_erase(shiftpoint h) {  
    ld nearest = 1;
    int nearest_id = -1;
    int id = -1;
    for(auto& shp: dtshapes) {
      id++;
      if(shp == nullptr) continue;
      auto& sh = *shp;
      cell*& c = sh.where;
      for(const shiftmatrix& V: current_display->all_drawn_copies[c]) {
        ld dist = sh.distance(inverse_shift(V, h));
        if(dist < nearest) nearest = dist, nearest_id = id;
        }
      }
    if(nearest_id >= 0)
      dtshapes.erase(dtshapes.begin() + nearest_id);
    }
  
  EX shiftpoint lstart;
  EX hyperpoint lstart_rel;
  cell *lstartcell;
  ld front_edit = 0.5;
  enum class eFront { sphere_camera, sphere_center, equidistants, const_x, const_y };
  eFront front_config;
  ld front_step = 0.1;

  #if HDR
  struct editwhat {
    double dist;
    int rotid, symid, pointid;
    bool side;
    cell *c;
    };
  #endif
  EX editwhat ew, ewsearch;
  EX bool autochoose = ISMOBILE;
  
  EX void scaleall(ld z, bool keep_mouse) { 
     
     if(keep_mouse) {
       ld mrx = (.0 + mousex - current_display->xcenter) / vpconf.scale;
       ld mry = (.0 + mousey - current_display->ycenter) / vpconf.scale;
       
       if(vid.xres > vid.yres) {      
         vpconf.xposition += (vpconf.scale - vpconf.scale*z) * mrx / current_display->scrsize;
         vpconf.yposition += (vpconf.scale - vpconf.scale*z) * mry / current_display->scrsize;
         }
       }

     vpconf.scale *= z;
     // printf("scale = " LDF "\n", vpconf.scale);
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
  EX map<int, cell*> modelcell;

  void handleKeyMap(int sym, int uni);

  EX void applyModelcell(cell *c) {
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
        c->mondir = gmod(c2->mondir - si2.dir + si.dir, c->type);
        // todo reflect
        }
      }
    }
#endif
EX }

#if HDR
struct hstream;
struct fhstream;
#endif

EX namespace mapstream {
#if CAP_EDIT

  EX std::map<cell*, int> cellids;
  EX vector<cell*> cellbyid;
  EX vector<char> relspin;
  
  void load_drawing_tool(hstream& hs) {
    using namespace mapeditor;
    if(hs.vernum < 0xA82A) return;
    int i = hs.get<int>();
    while(i--) {
      auto sh = load_shape(hs);
      if(!sh) continue;
      hs.read(sh->col);
      hs.read(sh->fill);
      hs.read(sh->lw);
      int id = hs.get<int>();
      sh->where = cellbyid[id];
      sh->rotate(spin(currentmap->spin_angle(sh->where, relspin[id]) - currentmap->spin_angle(sh->where, 0)));
      dtshapes.push_back(unique_ptr<dtshape>(sh));
      }
    }
  
  void save_drawing_tool(hstream& hs) {
    using namespace mapeditor;
    hs.write<int>(isize(dtshapes));
    for(auto& shp: dtshapes) {
      if(shp == nullptr) { hs.write<char>(0); }
      else {
        auto& sh = *shp;
        sh.save(hs);
        hs.write(sh.col);
        hs.write(sh.fill);
        hs.write(sh.lw);
        hs.write(cellids[sh.where]);
        }
      }
    }
  
  
  void addToQueue(cell* c) {
    if(cellids.count(c)) return;
    int numcells = isize(cellbyid);
    cellbyid.push_back(c);
    cellids[c] = numcells;
    }
  
  EX int fixspin(int rspin, int dir, int t, int vernum) {
    if(vernum < 11018 && dir == 14)
      return NODIR;
    else if(vernum < 11018 && dir == 15)
      return NOBARRIERS;
    else if(dir >= 0 && dir < t)
      return (dir + rspin) % t;
    else
      return dir;
    }
#endif
  
  EX void save_geometry(hstream& f) {
    f.write(geometry);
    char nbtype = char(variation);
    f.write(nbtype);
    #if CAP_GP
    if(GOLDBERG || INVERSE) {
      f.write(gp::param.first);
      f.write(gp::param.second);
      }
    #endif
    #if MAXMDIM >= 4
    if(variation == eVariation::coxeter) {
      f.write(reg3::coxeter_param);
      }
    if(is_subcube_based(variation )) {
      f.write(reg3::subcube_count);
      }
    #endif
    #if CAP_FIELD
    if(geometry == gFieldQuotient) {
      using namespace fieldpattern;
      f.write(quotient_field_changed);
      if(quotient_field_changed) {
        decltype(current_extra) cex = WDIM == 3 ? -1 : current_extra;
        f.write(cex);
        if(cex == -1) {
          f.write(fieldpattern::underlying_geometry);
          hwrite_fpattern(f, currfp);
          }
        else {
          f.write(fgeomextras[current_extra].current_prime_id);
          }
        }
      }
    #endif
    #if CAP_CRYSTAL
    if(cryst) {
      f.write(ginf[gCrystal].sides);
      if(ginf[gCrystal].sides == 8)
        f.write(ginf[gCrystal].vertex);
      }
    #endif
    #if CAP_ARCM
    if(geometry == gArchimedean) f.write(arcm::current.symbol);
    #endif
    if(geometry == gArbitrary) {
      f.write<bool>(rulegen::known());
      f.write<bool>(arb::convert::in());
      if(arb::convert::in()) {
        dynamicval<eGeometry> dg(geometry, arb::convert::base_geometry);
        dynamicval<eVariation> dv(variation, arb::convert::base_variation);
        save_geometry(f);
        }
      else
        f.write(arb::current.filename);
      }
    if(geometry == gNil) {
      f.write(S7);
      f.write(nilv::nilperiod);
      }
    #if CAP_SOLV
    if(geometry == gArnoldCat) {
      f.write(asonov::period_xy);
      f.write(asonov::period_z);
      }
    #endif
    if(prod) {
      f.write(hybrid::csteps);
      f.write(product::cspin);
      f.write(product::cmirror);
      f.write(vid.plevel_factor);
      }
    if(rotspace) {
      f.write(hybrid::csteps);
      }
    if(hybri) {
      hybrid::in_underlying_geometry([&] { save_geometry(f); });
      }
    if(fake::in()) {
      f.write(fake::around);
      fake::in_underlying_geometry([&] { save_geometry(f); });
      }
    if(bt::in()) 
      f.write(vid.binary_width);
    if(euc::in()) {
      f.write(euc::eu_input.user_axes);
      f.write(euc::eu_input.twisted);
      }
    f.write(mine_adjacency_rule);
    }
  
  EX void load_geometry(hstream& f) {
    bool was_default = pmodel == default_model();
    auto vernum = f.get_vernum();
    f.read(geometry);
    char nbtype;
    f.read(nbtype);
    variation = eVariation(nbtype);
    #if CAP_GP
    if(GOLDBERG || INVERSE) {
      f.read(gp::param.first);
      f.read(gp::param.second);
      }
    #endif
    #if MAXMDIM >= 4
    if(variation == eVariation::coxeter && vernum >= 0xA908) {
      f.read(reg3::coxeter_param);
      }
    if(is_subcube_based(variation) && vernum >= 0xA908) {
      f.read(reg3::subcube_count);
      }
    #endif
    #if CAP_CRYSTAL
    if(cryst && vernum >= 10504) {
      int sides;
      f.read(sides);
      #if CAP_CRYSTAL
      crystal::set_crystal(sides);
      #endif
      if(sides == 8) {
        int vertices;
        eVariation v = variation;
        f.read(vertices);          
        if(vertices == 3) {
          set_variation(eVariation::bitruncated);
          set_variation(v);
          }
        }
      }
    #endif
    #if CAP_FIELD
    if(geometry == gFieldQuotient) {
      using namespace fieldpattern;
      f.read(quotient_field_changed);
      if(quotient_field_changed) {
        auto cex = current_extra;
        f.read(current_extra);
        if(current_extra == -1) {
          current_extra = cex;
          f.read(geometry);
          check_cgi();
          cgi.require_basics();
          fieldpattern::field_from_current();
          set_geometry(gFieldQuotient);
          hread_fpattern(f, currfp);
          }
        else {
          auto& ge = fgeomextras[current_extra];
          auto& id = ge.current_prime_id;
          f.read(id);
          if(vernum < 0xA80C) switch(ge.base) {
            case gNormal: id++; break;
            case g45: id++; break;
            case g46: id+=2; break;
            case g47: id++; break;
            default: ;
            }
          enableFieldChange();
          }
        }
      }
    #endif
    if(geometry == gArbitrary) {    
      bool rk = vernum >= 0xA905 && f.get<bool>();
      bool ac = vernum >= 0xA905 && f.get<bool>();
      if(ac) {
        load_geometry(f);
        arb::convert::convert();
        arb::convert::activate();
        }
      else {
        string s;
        f.read(s);
        arb::run(s);
        stop_game();
        }
      if(rk) rulegen::prepare_rules();
      }
    #if CAP_ARCM
    if(geometry == gArchimedean) {
      string& symbol = arcm::current.symbol;
      symbol = f.get<string>();
      arcm::current.parse();
      if(arcm::current.errors > 0) {
        printf("Errors! %s\n", arcm::current.errormsg.c_str());
        }
      }
    #endif
    if(geometry == gNil && vernum >= 0xA80C) {
      f.read(S7);
      f.read(nilv::nilperiod);
      nilv::set_flags();
      }
    #if CAP_SOLV
    if(geometry == gArnoldCat && vernum >= 0xA80C) {
      f.read(asonov::period_xy);
      f.read(asonov::period_z);
      asonov::set_flags();
      }
    #endif
    if(geometry == gProduct && vernum >= 0xA80C) {
      f.read(hybrid::csteps);
      if(vernum >= 0xA80D) f.read(product::cspin);
      if(vernum >= 0xA833) f.read(product::cmirror);
      if(vernum >= 0xA908) f.read(vid.plevel_factor);
      }
    if(geometry == gRotSpace && vernum >= 0xA833) {
      f.read(hybrid::csteps);
      }
    if(hybri && vernum >= 0xA80C) {
      auto g = geometry;
      load_geometry(f);
      set_geometry(g);
      }
    if(fake::in()) {
      ld ar;
      f.read(ar);
      fake::around = ar;
      load_geometry(f);
      fake::change_around();
      }
    if(bt::in() && vernum >= 0xA80C) 
      f.read(vid.binary_width);
    if(euc::in() && vernum >= 0xA80D) {
      f.read(euc::eu_input.user_axes);
      f.read(euc::eu_input.twisted);
      }
    if(vernum >= 0xA810)
      f.read(mine_adjacency_rule);
    geometry_settings(was_default);
    }
  
  EX hookset<void(hstream&)> hooks_savemap, hooks_loadmap_old;
  EX hookset<void(hstream&, int)> hooks_loadmap;
  
  EX cell *save_start() {
    return (closed_manifold || euclid || prod || arcm::in() || sol || INVERSE) ? currentmap->gamestart() : cwt.at->master->c7;
    }

#if CAP_EDIT  
  void save_only_map(hstream& f) {
    f.write(patterns::whichPattern);
    save_geometry(f);
    #if CAP_RACING
    if(racing::on) racing::restore_goals();
    #endif
    
    // game settings
    f.write(safety);
    f.write(autocheat);
    f.write(gen_wandering);
    f.write(reptilecheat);
    f.write(timerghost);
    f.write(patterns::canvasback);
    f.write(patterns::whichShape);
    f.write(patterns::subpattern_flags);
    f.write(patterns::whichCanvas);
    f.write(patterns::displaycodes);
    f.write(canvas_default_wall);
    f.write(mapeditor::drawplayer);
    if(patterns::whichCanvas == 'f') f.write(patterns::color_formula);
    f.write(canvasfloor);
    f.write(canvasdark);
    
    {
    int i = ittypes; f.write(i);
    for(int k=0; k<i; k++) f.write(items[k]);
    i = motypes; f.write(i);
    for(int k=0; k<i; k++) f.write(kills[k]); 
    }
    
    addToQueue(save_start());
    #if MAXMDIM >= 4 && CAP_RAY
    if(intra::in) intra::prepare_need_to_save();
    #endif
    for(int i=0; i<isize(cellbyid); i++) {
      cell *c = cellbyid[i];
      if(i) {
        bool ok = false;
        for(int j=0; j<c->type; j++) if(c->move(j) && cellids.count(c->move(j)) && 
          cellids[c->move(j)] < i) {
          int32_t i = cellids[c->move(j)];
          f.write(i);
          f.write_char(c->c.spin(j));
          f.write_char(j);
          ok = true;
          break;
          }
        if(!ok) {
          println(hlog, "parent not found for ", c, "!");
          for(int j=0; j<c->type; j++) println(hlog, j, ": ", c->move(j), "; ", int(cellids.count(c->move(j)) ? cellids[c->move(j)] : -1));
          throw hr_exception("parent not found");
          }
        }
      f.write_char(c->land);
      f.write_char(c->mondir);
      f.write_char(c->monst);
      if(c->monst == moTortoise)
        f.write(tortoise::emap[c] = tortoise::getb(c));
      f.write_char(c->wall);
      if(dice::on(c)) {
        auto& dat = dice::data[c];
        f.write_char(dice::get_die_id(dat.which));
        f.write_char(dat.val);
        f.write_char(dat.dir);
        f.write_char(dat.mirrored);
        }
      f.write_char(c->item);
      if(c->item == itBabyTortoise)
        f.write(tortoise::babymap[c]);
      f.write_char(c->mpdist);
      if(inmirrororwall(c)) {
        f.write_char(c->barleft);
        f.write_char(c->barright);
        f.write_char(c->bardir);
        }
      f.write(c->wparam); f.write(c->landparam);
      f.write_char(c->stuntime); f.write_char(c->hitpoints);
      bool blocked = false;
      #if MAXMDIM >= 4 && CAP_RAY
      if(intra::in && isWall3(c) && !intra::need_to_save.count(c)) blocked = true;
      #endif
      if(!blocked)
      for(int j=0; j<c->type; j++) {
        cell *c2 = c->move(j);
        if(c2 && c2->land != laNone && c2->land != laMemory) addToQueue(c2);
        }
      }
    printf("cells saved = %d\n", isize(cellbyid));
    int32_t n = -1; f.write(n);
    int32_t id = cellids.count(cwt.at) ? cellids[cwt.at] : -1;
    f.write(id);

    if(f.vernum >= 0xA90C) {
      vector<color_t> v;
      for(auto c: walking::colors_of_floors) v.push_back(c);
      f.write(v);
      }
    
    save_drawing_tool(f);

    f.write(vid.always3);
    f.write(mutantphase);
    f.write(rosewave);
    f.write(rosephase);
    f.write(turncount);
    int rms = isize(rosemap); f.write(rms);
    for(auto p: rosemap) f.write(cellids[p.first]), f.write(p.second);
    f.write(multi::players);
    if(multi::players > 1)
      for(int i=0; i<multi::players; i++)
        f.write(cellids[multi::player[i].at]);

    #if MAXMDIM >= 4 && CAP_RAY
    if(intra::in) {
      for(int i=0; i<isize(intra::portals_to_save); i++) {
        auto& p = intra::portals_to_save[i];
        if(cellids.count(p.cw1.at)) {
          f.write<char>(1);
          f.write(i);
          f.write(cellids[p.cw1.at]);
          f.write<char>(p.cw1.spin);
          }
        if(cellids.count(p.cw2.at)) {
          f.write<char>(2);
          f.write(i);
          f.write(cellids[p.cw2.at]);
          f.write<char>(p.cw2.spin);
          }
        }
      f.write<char>(0);
      }
    #endif
      
    if(f.vernum >= 0xA912) {
      #if CAP_RACING
      f.write(racing::on);
      if(racing::on) {
        f.write<int>(isize(racing::track));
        for(auto& t: racing::track) f.write<int>(cellids[t]);
        racing::save_ghosts(f);
        }
      #else
      bool on = false;
      f.write(on);
      #endif
      }

    callhooks(hooks_savemap, f);
    f.write<int>(0);

    cellids.clear();
    cellbyid.clear();
    }
  
  void load_usershapes(hstream& f) {
    if(f.vernum >= 7400) while(true) {
      int i = f.get<int>();
      if(i == -1) break;
      #if CAP_POLY
      int j = f.get<int>(), l = f.get<int>();
      if(i >= 4) i = 3;
      if(i<0 || i >= mapeditor::USERSHAPEGROUPS) break;
      if(l<0 || l >= USERLAYERS) break;

      initShape(i, j);
      usershapelayer& ds(usershapes[i][j]->d[l]);

      if(f.vernum >= 11008) f.read(ds.zlevel);
      
      f.read(ds.sym); f.read(ds.rots); f.read(ds.color);
      ds.list.clear();
      int siz = f.get<int>();
      
      ds.shift = f.get<hyperpoint>();
      ds.spin = f.get<hyperpoint>();
      
      for(int i=0; i<siz; i++)
        ds.list.push_back(f.get<hyperpoint>());
      #else
      printf("cannot read shapes\n"); exit(1);
      #endif
      }    
    }
  
  void load_only_map(hstream& f) {
    stop_game();
    if(f.vernum >= 10420 && f.vernum < 10503) {
      int i;
      f.read(i);
      patterns::whichPattern = patterns::ePattern(i);
      }
    else if(f.vernum >= 7400) f.read(patterns::whichPattern);
    
    if(f.vernum >= 10203) 
      load_geometry(f);
      
    check_cgi();
    cgi.require_basics();

    usershape_changes++;

    initcells();
    if(shmup::on) shmup::init();
    
    if(f.vernum >= 10505) {
      // game settings
      f.read(safety);
      bool b;
      f.read(b); if(b) autocheat = true;
      f.read(gen_wandering);
      f.read(reptilecheat);
      f.read(timerghost);
      f.read(patterns::canvasback);
      f.read(patterns::whichShape);
      f.read(patterns::subpattern_flags);
      f.read(patterns::whichCanvas);
      f.read(patterns::displaycodes);
      if(f.vernum >= 0xA816)
        f.read(canvas_default_wall);
      f.read(mapeditor::drawplayer);
      if(patterns::whichCanvas == 'f') f.read(patterns::color_formula);
      if(f.vernum >= 0xA90D) {
        f.read(canvasfloor);
        f.read(canvasdark);
        }
      
      int i;
      f.read(i); if(i > ittypes || i < 0) throw hstream_exception();
      for(int k=0; k<i; k++) f.read(items[k]);
      f.read(i); if(i > motypes || i < 0) throw hstream_exception();
      for(int k=0; k<i; k++) f.read(kills[k]);    
      }

    int sub = hybri ? 2 : 0;
    while(true) {
      cell *c;
      int rspin;
      
      if(isize(cellbyid) == 0) {
        c = currentmap->gamestart();
        rspin = 0;
        }
      else {
        int32_t parent = f.get<int>();
        
        if(parent<0 || parent >= isize(cellbyid)) break;
        int dir = f.read_char();
        cell *c2 = cellbyid[parent];
        dir = fixspin(relspin[parent], dir, c2->type - sub, f.vernum);
        c = createMov(c2, dir);
        // printf("%p:%d,%d -> %p\n", c2, relspin[parent], dir, c);
        
        // spinval becomes xspinval
        rspin = gmod(c2->c.spin(dir) - f.read_char(), c->type - sub);
        if(GDIM == 3 && rspin && !hybri) {
          println(hlog, "rspin in 3D");
          throw hstream_exception();
          }
        }
      
      cellbyid.push_back(c);
      relspin.push_back(rspin);
      c->land = (eLand) f.read_char();
      c->mondir = fixspin(rspin, f.read_char(), c->type - sub, f.vernum);
      c->monst = (eMonster) f.read_char();
      if(c->monst == moTortoise && f.vernum >= 11001)
        f.read(tortoise::emap[c]);
      c->wall = (eWall) f.read_char();
      if(dice::on(c)) {
        auto& dat = dice::data[c];        
        dat.which = dice::get_by_id(f.read_char());
        dat.val = f.read_char();
        dat.dir = fixspin(rspin, f.read_char(), c->type, f.vernum);
        if(f.vernum >= 0xA902)
          dat.mirrored = f.read_char();
        }
      // c->barleft = (eLand) f.read_char();
      // c->barright = (eLand) f.read_char();
      c->item = (eItem) f.read_char();
      if(c->item == itBabyTortoise && f.vernum >= 11001)
        f.read(tortoise::babymap[c]);
      c->mpdist = f.read_char();
      c->bardir = NOBARRIERS;
      if(inmirrororwall(c) && f.vernum >= 0xA912) {
        c->barleft = (eLand) f.read_char();
        c->barright = (eLand) f.read_char();
        c->bardir = fixspin(rspin, f.read_char(), c->type, f.vernum);
        }
      // fixspin(rspin, f.read_char(), c->type);
      if(f.vernum < 7400) {
        short z;
        f.read(z);
        c->wparam = z;
        }
      else f.read(c->wparam);
      f.read(c->landparam);
      // backward compatibility
      if(f.vernum < 7400 && !isIcyLand(c->land)) c->landparam = HEAT(c);
      c->stuntime = f.read_char();
      c->hitpoints = f.read_char();

      if(patterns::whichPattern)
        mapeditor::modelcell[patterns::getpatterninfo0(c).id] = c;
      }
    
    int32_t whereami = f.get<int>();
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

    relspin.clear();

    if(shmup::on) shmup::init();

    timerstart = time(NULL); turncount = 0; 
    sagephase = 0; hardcoreAt = 0;
    timerstopped = false;
    savecount = 0; savetime = 0;
    cheater = 1;
    
    if(f.vernum >= 0xA90C) {
      vector<color_t> v;
      f.read(v);
      walking::colors_of_floors.clear();
      for(auto c: v) walking::colors_of_floors.insert(c);
      }
    else walking::colors_of_floors.clear();

    load_drawing_tool(f);

    dynamicval<bool> a3(vid.always3, vid.always3);
    if(f.vernum >= 0xA616) { f.read(vid.always3); geom3::apply_always3(); }
    
    if(f.vernum < 0xA61A) load_usershapes(f);

    if(f.vernum >= 11005) {
      f.read(mutantphase);
      f.read(rosewave);
      f.read(rosephase);
      f.read(turncount);
      int i; f.read(i);
      if(i) havewhat |= HF_ROSE;
      while(i--) { 
        int cid; int val; f.read(cid); f.read(val); 
        if(cid >= 0 && cid < isize(cellbyid)) rosemap[cellbyid[cid]] = val; 
        }
      f.read(multi::players);
      if(multi::players > 1)
        for(int i=0; i<multi::players; i++) {
          auto& mp = multi::player[i];
          int whereami = f.get<int>();
          if(whereami >= 0 && whereami < isize(cellbyid))
            mp.at = cellbyid[whereami];
          else
            mp.at = currentmap->gamestart();
          mp.spin = 0,
          mp.mirrored = false;
          }
      }

    #if MAXMDIM >= 4 && CAP_RAY
    if(intra::in) {
      while(true) {
        char k = f.get<char>();
        if(!k) break;
        int i = f.get<int>();
        int id = f.get<int>();
        int spin = f.get<char>();
        auto& p = intra::portals_to_save[i];
        auto& cw = (k==1 ? p.cw1 : p.cw2);
        cw.at = cellbyid[id];
        cw.spin = fixspin(relspin[id], spin, cw.at->type, f.vernum);
        }
      }
    #endif
    
    if(f.vernum >= 0xA912) {
      #if CAP_RACING
      f.read(racing::on);
      if(racing::on) {
        if(!shmup::on) {
          shmup::on = true;
          shmup::init();
          }
        racing::track.resize(f.get<int>());
        for(auto& t: racing::track) t = cellbyid[f.get<int>()];
        racing::load_ghosts(f);
        racing::configure_track(false);
        }
      #else
      bool on;
      f.read(on);
      #endif
      }

    if(f.vernum >= 0xA848) {
      int i;
      f.read(i);
      while(i) {
        callhooks(hooks_loadmap, f, i);
        f.read(i);        
        }
      }
    else
      callhooks(hooks_loadmap_old, f);

    cellbyid.clear();
    restartGraph();
    bfs();
    game_active = true;
    }
  
  void save_usershapes(hstream& f) {
    int32_t n;
    #if CAP_POLY    
    for(int i=0; i<mapeditor::USERSHAPEGROUPS; i++) for(auto usp: usershapes[i]) {
      usershape *us = usp.second;
      if(!us) continue;
      
      for(int l=0; l<USERLAYERS; l++) if(isize(us->d[l].list)) {
        usershapelayer& ds(us->d[l]);
        f.write(i); f.write(usp.first); f.write(l); 
        f.write(ds.zlevel);      
        f.write(ds.sym); f.write(ds.rots); f.write(ds.color);
        n = isize(ds.list); f.write(n);
        f.write(ds.shift);
        f.write(ds.spin);
        for(int i=0; i<isize(ds.list); i++) f.write(ds.list[i]);
        }
      }
    #endif
    n = -1; f.write(n);
    }
  
  EX bool saveMap(const char *fname) {
    fhstream f(fname, "wb");
    if(!f.f) return false;
    saveMap(f);
    return true;
    }

  EX void saveMap(hstream& f) {
    f.write(f.get_vernum());
    f.write(dual::state);
    #if MAXMDIM >= 4 && CAP_RAY
    int q = intra::in ? isize(intra::data) : 0;
    f.write(q);
    #else
    int q = 0;
    #endif
    if(q) {
      #if MAXMDIM >= 4 && CAP_RAY
      intra::prepare_to_save();
      int qp = isize(intra::portals_to_save);
      f.write(qp);
      for(auto& ps: intra::portals_to_save) {
        f.write(ps.spin);
        f.write(ps.mirrored);
        }
      intra::resetter ir;
      for(int i=0; i<q; i++) { intra::switch_to(i); save_only_map(f); }
      #endif
      }
    else {
      // make sure we save in correct order
      if(dual::state) dual::switch_to(1);
      dual::split_or_do([&] { save_only_map(f); });
      }
    save_usershapes(f);
    }
  
  EX bool loadMap(const string& fname) {
    fhstream f(fname, "rb");
    if(!f.f) return false;
    return loadMap(f);
    }
    
  EX bool loadMap(hstream& f) {
    f.read(f.vernum);
    if(f.vernum > 10505 && f.vernum < 11000) 
      f.vernum = 11005;
    auto ds = dual::state;
    if(f.vernum >= 0xA61A)
      f.read(ds);
    else
      ds = 0;
    if(ds == 1 && dual::state == 0) dual::enable();
    if(ds == 0 && dual::state == 1) dual::disable();
    int q = 0;
    if(f.vernum >= 0xA907) {
      f.read(q);
      }
    if(q) {
      int qp;
      f.read(qp);
      #if MAXMDIM >= 4 && CAP_RAY
      intra::portals_to_save.resize(qp);
      for(auto& ps: intra::portals_to_save) {
        f.read(ps.spin);
        f.read(ps.mirrored);
        println(hlog, tie(ps.spin, ps.mirrored));
        }
      for(int i=0; i<q; i++) {
        intra::in = true; /* so that it knows to load portals */
        game_active = false;
        load_only_map(f);
        intra::in = false;
        intra::become();
        }
      intra::start();
      intra::load_saved_portals();
      #endif
      }
    else {
      dual::split_or_do([&] { load_only_map(f); });
      }
    if(dual::state) dual::assign_landsides();
    if(f.vernum >= 0xA61A) 
      load_usershapes(f);
    return true;
    }
  
#endif
EX }

EX namespace mapeditor {

  EX bool drawplayer = true;

  EX cell *drawcell;

#if CAP_EDIT
  int paintwhat = 0;
  int paintwhat_alt_wall = 0;
  int painttype = 0;
  int paintstatueid = 0;
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
  
  int editor_fsize() {
    return min(vid.fsize + 5, (vid.yres - 16) /32 );
    }

  void displayFunctionKeys() {
    int fs = editor_fsize();
    displayButton(8, vid.yres-8-fs*11, XLAT("F1 = help"), SDLK_F1, 0);
    displayButton(8, vid.yres-8-fs*10, XLAT("F2 = save"), SDLK_F2, 0);
    displayButton(8, vid.yres-8-fs*9, XLAT("F3 = load"), SDLK_F3, 0);
    displayButton(8, vid.yres-8-fs*7, drawing_tool ? XLAT("F5 = clear") : XLAT("F5 = restart"), SDLK_F5, 0);
    #if CAP_SHOT
    displayButton(8, vid.yres-8-fs*6, XLAT("F6 = HQ shot"), SDLK_F6, 0);
    #endif
    displayButton(8, vid.yres-8-fs*5, XLAT("F7 = player on/off"), SDLK_F7, 0);
    displayButton(8, vid.yres-8-fs*3, XLAT("SPACE = map/graphics"), ' ', 0);
    displayButton(8, vid.yres-8-fs*2, XLAT("ESC = return to the game"), SDLK_ESCAPE, 0);
    }
  
  EX set<cell*> affected;
  EX set<int> affected_id;

  EX void showMapEditor() {
    cmode = sm::MAP | sm::PANNING;
    if(building_mode) {
      if(anyshiftclick) cmode |= sm::EDIT_INSIDE_WALLS;
      else cmode |= sm::EDIT_BEFORE_WALLS;
      }
    gamescreen();
  
    int fs = editor_fsize();
    
    affected.clear();
    affected_id.clear();
    if(lmouseover) {
      celllister cl(lmouseover, radius, 10000, nullptr);
      for(cell *c: cl.lst) affected.insert(c), affected_id.insert(patterns::getpatterninfo0(c).id);
      }
    
    getcstat = '-';

    if(anyshiftclick) {
      displayfr(8, 8 + fs, 2, vid.fsize,
        (painttype == 6 && (GDIM == 3)) ? "wall" :
        painttype == 3 ? XLATN(winf[paintwhat_alt_wall].name) : "clear",
        forecolor, 0);
      }
    else
      displayfr(8, 8 + fs, 2, vid.fsize, paintwhat_str, forecolor, 0);
    displayfr(8, 8+fs*2, 2, vid.fsize, XLAT("use at your own risk!"), 0x800000, 0);

    displayButton(8, 8+fs*4, XLAT("0-9 = radius (%1)", its(radius)), ('0' + (radius+1)%10), 0);
    displayButton(8, 8+fs*5, XLAT("b = boundary"), 'b', 0);
    displayButton(8, 8+fs*6, XLAT("m = monsters"), 'm', 0);
    displayButton(8, 8+fs*7, XLAT("w = walls"), 'w', 0);
    displayButton(8, 8+fs*8, XLAT("i = items"), 'i', 0);
    displayButton(8, 8+fs*9, XLAT("l = lands"), 'l', 0);
    displayButton(8, 8+fs*10, XLAT("c = copy"), 'c', 0);
    displayButton(8, 8+fs*11, XLAT("u = undo"), 'u', 0);
    if(painttype == 4)
      displayButton(8, 8+fs*12, XLAT("f = flip %1", ONOFF(copysource.mirrored)), 'u', 0);
    displayButton(8, 8+fs*13, XLAT("r = regular"), 'r', 0);
    displayButton(8, 8+fs*14, XLAT("p = paint"), 'p', 0);
    if(painttype == 3)
      displayButton(8, 8+fs*15, XLAT("z = set Shift+click"), 'z', 0);
    if(WDIM == 3)
      displayButton(8, 8+fs*16, XLAT("B = build on walls ") + ONOFF(building_mode), 'B', 0);

    displayFunctionKeys();
    displayButton(8, vid.yres-8-fs*4, XLAT("F8 = settings"), SDLK_F8, 0);
    
    keyhandler = handleKeyMap;
    }
  
  int spillinc() {
    if(radius>=2) return 0;
    if(anyshiftclick) return -1;
    return 1;
    }
  
  EX eShapegroup drawcellShapeGroup() {
    if(drawcell == cwt.at && drawplayer) return sgPlayer;
    if(drawcell->wall == waEditStatue) return sgWall;
    if(drawcell->monst) return sgMonster;
    if(drawcell->item) return sgItem;
    return sgFloor;
    }
  
  EX int drawcellShapeID() {
    if(drawcell == cwt.at && drawplayer) return vid.cs.charid;
    if(drawcell->wall == waEditStatue) return drawcell->wparam;
    if(drawcell->monst) return drawcell->monst;
    if(drawcell->item) return drawcell->item;
    auto si = patterns::getpatterninfo0(drawcell);
    return si.id;
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
      case 0: {
        if(anyshiftclick) { c->monst = moNone; mirror::destroyKilled(); break; }
        eMonster last = c->monst;
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

        if(c->monst ==moTortoise && last == moTortoise) {
          cell *c1 = c;
          for(int i=0; i<100; i++) c1 = c1->cmove(hrand(c1->type));
          tortoise::emap[c] = tortoise::getRandomBits();
          }
        
        if(isDie(c->monst)) {
          if(!dice::generate_random(c)) c->monst = moNone;
          }

        mirror::destroyKilled();
        break;
        }
      case 1: {
        if(anyshiftclick) { c->item = itNone; break; }
        eItem last = c->item;
        c->item = eItem(paintwhat);
        if(c->item == itBabyTortoise)
          tortoise::babymap[c] = getBits(c) ^ (last == itBabyTortoise ? tortoise::getRandomBits() : 0);
        break;
        }
      case 2: {
        if(anyshiftclick) { c->land = laNone; c->wall = waNone; map_version++; break; }
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
        c->wall = eWall(anyshiftclick ? paintwhat_alt_wall : paintwhat);
        map_version++;
        
        if(last != c->wall) {
          if(hasTimeout(c))
            c->wparam = 10;
          else if(c->wall == waWaxWall)
            c->landparam = hrand(0xFFFFFF + 1);
          }
        else if(hasTimeout(c))
          c->wparam += spillinc();
        
        if(c->wall == waEditStatue) {
          c->wparam = paintstatueid;
          c->mondir = cdir;
          }

        if(isDie(c->wall)) {
          if(!dice::generate_random(c)) c->wall = waNone;
          }

        break;
        }
      case 5:
        map_version++;
        c->land = laNone;
        c->wall = waNone;
        c->item = itNone;
        c->monst = moNone;
        c->landparam = 0;
        // c->tmp = -1;
        break;
      case 6:
        map_version++;
        c->land = laCanvas;
        c->wall = ((GDIM == 3) ^ anyshiftclick) ? waWaxWall : waNone;
        c->landparam = paintwhat >> 8;
        break;
      case 4: {
        map_version++;
        cell *copywhat = where.second.at;
        c->wall = copywhat->wall;
        c->item = copywhat->item;
        c->land = copywhat->land;
        c->monst = copywhat->monst;
        c->landparam = copywhat->landparam;
        c->wparam = copywhat->wparam;
        c->hitpoints = copywhat->hitpoints;
        c->stuntime = copywhat->stuntime; 
        if(dice::on(c)) dice::data[c] = dice::data[copywhat];
        if(copywhat->mondir == NODIR) c->mondir = NODIR;
        else c->mondir = gmod((where.first.mirrored == where.second.mirrored ? 1 : -1) * (copywhat->mondir - where.second.spin) + cdir, c->type);
        break;
        }
      case 7:
        if(c) {
          copysource = c;
          painttype = 4;
          paintwhat_str = XLAT("copying");
          }
        break;
      }
    checkUndo();
    }
  
  vector<pair<cellwalker, cellwalker> > spill_list;
  
  void list_spill(cellwalker tgt, cellwalker src, manual_celllister& cl) {
    spill_list.clear(); 
    spill_list.emplace_back(tgt, src);
    if(painttype == 7) return;
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

  void editAt(cellwalker where, manual_celllister& cl) {

    if(painttype == 4 && radius) {
      if(where.at->type != copysource.at->type) return;
      if(where.spin<0) where.spin=0;
      if(BITRUNCATED && !ctof(mouseover) && ((where.spin&1) != (copysource.spin&1)))
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
        editAt(cellwalker(c2, cdir>=0 ? cdir + si2.dir : -1), cl);
        modelcell[si2.id] = c2;
        }
      }
    }
  
  cellwalker mouseover_cw(bool fix) {
    int d = neighborId(mouseover, mouseover2);
    if(d == -1 && fix) d = hrand(mouseover->type);
    return cellwalker(mouseover, d);
    }

  void save_level() {
    #if ISWEB
    mapstream::saveMap("web.lev");
    offer_download("web.lev", "mime/type");
    #else
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
    #endif
    }

  void load_level() {
    #if ISWEB
    offer_choose_file([] {
      mapstream::loadMap("data.txt");
      });
    #else
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
    #endif
    }
  
  void showList() {
    string caption;
    dialog::v.clear();
    if(painttype == 4) painttype = 0;
    switch(painttype) {
      case 0: 
        caption = "monsters";
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
        caption = "items";
        for(int i=0; i<ittypes; i++) dialog::vpush(i, iinf[i].name);
        break;
      case 2:
        caption = "lands";
        for(int i=0; i<landtypes; i++) dialog::vpush(i, linf[i].name);
        break;
      case 3:
        caption = "walls";
        for(int i=0; i<walltypes; i++) if(i != waChasmD) dialog::vpush(i, winf[i].name);
        break;
      }
    // sort(v.begin(), v.end());
    
    if(dialog::infix != "") mouseovers = dialog::infix;
    
    cmode = 0;
    gamescreen();
    dialog::init(caption);
    if(dialog::infix != "") mouseovers = dialog::infix;
    dialog::addBreak(50);
    dialog::start_list(900, 900, '1');
    
    for(auto& vi: dialog::v) {
      dialog::addItem(vi.first, dialog::list_fake_key++);
      dialog::add_action([&vi] {
        paintwhat = vi.second;
        paintwhat_str = vi.first;

        mousepressed = false;
        popScreen();

        if(painttype == 3 && paintwhat == waEditStatue)
          dialog::editNumber(paintstatueid, 0, 127, 1, 1, XLAT1("editable statue"), 
            XLAT("These statues are designed to have their graphics edited in the Vector Graphics Editor. Each number has its own, separate graphics.")
            );
        });
      }

    dialog::end_list();
    dialog::addBreak(50);
    dialog::addInfo(XLAT("press letters to search"));
    dialog::addBack();
    dialog::display();

    keyhandler = [] (int sym, int uni) {
      dialog::handleNavigation(sym, uni);
      if(dialog::editInfix(uni)) dialog::list_skip = 0;
      else if(doexiton(sym, uni)) popScreen();
      };    
    }

  void handleKeyMap(int sym, int uni) {
    handlePanning(sym, uni);

    // left-clicks are coded with '-', and right-clicks are coded with sym F1
    if(uni == '-' && !holdmouse) undoLock();
    if(uni == '-' && mouseover) {
      allInPattern(mouseover_cw(false));
      if(!(GDIM == 3 && building_mode))
        holdmouse = true;
      }
    
    if(mouseover) for(int i=0; i<mouseover->type; i++) createMov(mouseover, i);
    if(uni == 'u') applyUndo();
    else if(uni == 'v' || sym == SDLK_F10 || sym == SDLK_ESCAPE) popScreen();
    else if(uni == 'A') popScreen();
    else if(uni >= '0' && uni <= '9') radius = uni - '0';
    else if(uni == 'm') pushScreen(showList), painttype = 0, dialog::infix = "";
    else if(uni == 'i') pushScreen(showList), painttype = 1, dialog::infix = "";
    else if(uni == 'l') pushScreen(showList), painttype = 2, dialog::infix = "";
    else if(uni == 'w') pushScreen(showList), painttype = 3, dialog::infix = "";
    else if(uni == 'z' && painttype == 3) paintwhat_alt_wall = paintwhat;
    else if(uni == 'B') building_mode = !building_mode;
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
    else if(uni == 'G') 
      push_debug_screen();
    else if(sym == SDLK_F5) {
      dialog::push_confirm_dialog([] { restart_game(); }, XLAT("Are you sure you want to clear the map?"));
      }
    else if(sym == SDLK_F2) save_level();
    else if(sym == SDLK_F3) load_level();
#if CAP_SHOT
    else if(sym == SDLK_F6) {
      pushScreen(shot::menu);
      }
#endif
    else if(sym == SDLK_F7) {
      drawplayer = !drawplayer;
      }
    else if(sym == SDLK_F8) {
      pushScreen(map_settings);
      }
    else if(uni == 'c' && mouseover) {
      copysource = mouseover_cw(true);
      painttype = 4;
      paintwhat_str = XLAT("copying");
      }
    else if(uni == 'c') {
      painttype = 7;
      paintwhat_str = XLAT("select area to copy");
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
  color_t colortouse = 0xC0C0C0FFu;
  // fake key sent to change the color
  static const int COLORKEY = (-10000); 

  EX shiftmatrix drawtrans, drawtransnew;

  #if CAP_POLY
  void loadShape(int sg, int id, hpcshape& sh, int d, int layer) {
    usershapelayer *dsCur = &usershapes[sg][id]->d[layer];
    dsCur->list.clear();
    dsCur->sym = d==2;
    for(int i=sh.s; i < sh.s + (sh.e-sh.s)/d; i++)
      dsCur->list.push_back(cgi.hpc[i]);
    }
  #endif

  EX void drawGhosts(cell *c, const shiftmatrix& V, int ct) {
    if(!(cmode & sm::MAP)) return;
    if(darken != 0) return;
    if(GDIM == 2 && mouseout()) return;
    if(patterns::whichPattern) {
      if(!affected_id.count(patterns::getpatterninfo0(c).id)) return;
      }
    else {
      if(!affected.count(c)) return;
      }
    queuecircleat1(c, V, .78, 0x00FFFFFF);
    }

  hyperpoint ccenter = C02;
  hyperpoint coldcenter = C02;
  
  unsigned gridcolor = 0xC0C0C040;
  
  shiftpoint in_front_dist(ld d) {

    ld ys = current_display->xsize/2;
    double mx = current_display->tanfov * (mousex - current_display->xcenter)/ys;
    double my = current_display->tanfov * (mousey - current_display->ycenter)/ys/pconf.stretch;
    hyperpoint tgt = point3(mx, my, 1);
    tgt *= d / hypot_d(3, tgt);

    return shiftless(direct_exp(lp_iapply(tgt))); /* todo direct_shift */
    }
  
  EX shiftpoint find_mouseh3() {
    if(vrhr::active())
      return mouseh;
    if(front_config == eFront::sphere_camera)
      return in_front_dist(front_edit);
    ld step = 0.01;
    ld cdist = 0;
    
    auto idt = z_inverse(unshift(drawtrans));

    auto qu = [&] (ld d) {
      ld d1 = front_edit;
      shiftpoint h1 = in_front_dist(d);
      if(front_config == eFront::sphere_center) 
        d1 = geo_dist(drawtrans * C0, h1);
      if(front_config == eFront::equidistants) {
        hyperpoint h = idt * unshift(in_front_dist(d));
        d1 = asin_auto(h[2]);
        }
      if(front_config == eFront::const_x) {
        hyperpoint h = idt * unshift(in_front_dist(d));
        d1 = asin_auto(h[0]);
        }
      if(front_config == eFront::const_y) {
        hyperpoint h = idt * unshift(in_front_dist(d));
        d1 = asin_auto(h[1]);
        }
      return pow(d1 - front_edit, 2);
      };
    
    ld bq = qu(cdist);
    while(abs(step) > 1e-10) {
      ld cq = qu(cdist + step);
      if(cq < bq) cdist += step, bq = cq;
      else step *= -.5;
      }
    return in_front_dist(cdist);
    }
  
  int parallels = 12, meridians = 6;
  ld equi_range = 1;
    
  EX void drawGrid() {
    if(!drawcell) drawcell = cwt.at;
    color_t lightgrid = gridcolor;
    lightgrid -= (lightgrid & 0xFF) / 2;
    shiftmatrix d2 = drawtrans * rgpushxto0(ccenter) * rspintox(gpushxto0(ccenter) * coldcenter);

    if(GDIM == 3) {
      queuecircleat(mapeditor::drawcell, 1, 0x80D080FF);
      color_t cols[4] = { 0x80D080FF, 0x80D080FF, 0xFFFFFF40, 0x00000040 };
      if(true) {
        shiftmatrix t = rgpushxto0(find_mouseh3());
        for(int i=0; i<4; i++)
          queueline(t * cpush0(i&1, 0.1), t * cpush0(i&1, -0.1), cols[i], -1, i < 2 ? PPR::LINE : PPR::SUPERLINE);
        }
      if(front_config == eFront::sphere_center) for(int i=0; i<4; i+=2) {
        auto pt = [&] (ld a, ld b) {
          return direct_exp(spin(a*degree) * cspin(0, 2, b*degree) * xtangent(front_edit));
          };
        for(int ai=0; ai<parallels; ai++) {
          ld a = ai * 360 / parallels;
          for(int b=-90; b<90; b+=5) curvepoint(pt(a,b));
          queuecurve(d2, cols[i + ((ai*4) % parallels != 0)], 0, i < 2 ? PPR::LINE : PPR::SUPERLINE);
          }
        for(int bi=1-meridians; bi<meridians; bi++) {
          ld b = 90 * bi / meridians;
          for(int a=0; a<=360; a+=5) curvepoint(pt(a, b));
          queuecurve(d2, cols[i + (bi != 0)], 0, i < 2 ? PPR::LINE : PPR::SUPERLINE);
          }
        }
      transmatrix T;
      if(front_config == eFront::equidistants) T = Id;
      else if(front_config == eFront::const_x) T = cspin(2, 0, M_PI/2);
      else if(front_config == eFront::const_y) T = cspin(2, 1, M_PI/2);
      else return;
      for(int i=0; i<4; i+=2) {
        for(int u=2; u<=20; u++) {
          PRING(d) {
            curvepoint(T * xspinpush(M_PI*d/cgi.S42, u/20.) * zpush0(front_edit));
            }
          queuecurve(d2, cols[i + (u%5 != 0)], 0, i < 2 ? PPR::LINE : PPR::SUPERLINE);
          }
        for(int d=0; d<cgi.S84; d++) {
          for(int u=0; u<=20; u++) curvepoint(T * xspinpush(M_PI*d/cgi.S42, u/20.) * zpush(front_edit) * C0);
          queuecurve(d2, cols[i + (d % (cgi.S84/drawcell->type) != 0)], 0, i < 2 ? PPR::LINE : PPR::SUPERLINE);
          }
        }
      return;
      }

    for(int d=0; d<cgi.S84; d++) {
      unsigned col = (d % (cgi.S84/drawcell->type) == 0) ? gridcolor : lightgrid;
      queueline(d2 * C0, d2 * xspinpush0(M_PI*d/cgi.S42, 1), col, 4 + vid.linequality);
      }
    for(int u=2; u<=20; u++) {
      PRING(d) {
        curvepoint(xspinpush0(M_PI*d/cgi.S42, u/20.));
        }
      queuecurve(d2, (u%5==0) ? gridcolor : lightgrid, 0, PPR::LINE);
      }
    queueline(drawtrans*ccenter, drawtrans*coldcenter, gridcolor, 4 + vid.linequality);

    if(snapping && !mouseout())
      queuestr(mouse_snap(), 10, "x", 0xC040C0);
    }

  void drawHandleKey(int sym, int uni);
  
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

  bool area_in_pi = false;

  ld compute_area(hpcshape& sh) {
    ld area = 0;
    for(int i=sh.s; i<sh.e-1; i++) {
      hyperpoint h1 = cgi.hpc[i];
      hyperpoint h2 = cgi.hpc[i+1];
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
        // ld z = degree;
        ld x = b2 - b1 + M_PI;
        while(x > M_PI) x -= 2 * M_PI;
        while(x < -M_PI) x += 2 * M_PI;
        area += x;
        }
      }
    return area;
    }

#define EDITING_TRIANGLES (GDIM == 3)

  EX shiftpoint mouse_snap() {
    ld xdist = HUGE_VAL;
    shiftpoint resh;
    auto snap_to = [&] (shiftpoint h) {
      ld dist = hdist(h, mouseh);
      if(dist < xdist) xdist = dist, resh = h;
      };
    for(auto& p: gmatrix) {
      cell *c = p.first;
      auto& T = p.second;
      snap_to(T * C0);
      for(int i=0; i<c->type; i++) {
        hyperpoint h1 = get_corner_position(c, i);
        hyperpoint h2 = get_corner_position(c, (i+1) % c->type);
        snap_to(T * h1);
        snap_to(T * mid(h1, h2));
        }
      }
    return resh;
    }

  EX void showDrawEditor() {
#if CAP_POLY
    cmode = sm::DRAW | sm::PANNING;
    gamescreen();
    drawGrid();
    if(callhandlers(false, hooks_prestats)) return;

    if(!mouseout()) getcstat = '-';
    
    int sg;
    string line1, line2;
  
    usershape *us = NULL;

#if CAP_TEXTURE
    if(texture::config.tstate != texture::tsActive && intexture) {
      intexture = false; drawing_tool = true;
      }
#endif
    
    bool freedraw = drawing_tool || intexture;    

#if CAP_TEXTURE        
    if(intexture) {
      sg = 16;
      line1 = "texture";
      line2 = "";
      texture::config.data.update();
      freedraw = true;
      drawing_tool = false;
      }
#endif

    if(!freedraw) {

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
          line1 = GDIM == 3 ? XLAT("pick something") : XLAT("floor");
          line2 = "#" + its(drawcellShapeID());
          break;        

        case sgWall:
          line1 = XLAT("statue");
          line2 = "#" + its(drawcellShapeID());
          break;        
        }
      
      us =usershapes[drawcellShapeGroup()][drawcellShapeID()];
      }
    
    int fs = editor_fsize();

    // displayButton(8, 8+fs*9, XLAT("l = lands"), 'l', 0);
    displayfr(8, 8+fs, 2, vid.fsize, line1, 0xC0C0C0, 0);
    
    if(!freedraw) {
      if(sg == sgFloor)
        displayButton(8, 8+fs*2, line2 + XLAT(" (r = complex tesselations)"), 'r', 0);
      else
        displayfr(8, 8+fs*2, 2, vid.fsize, line2, 0xC0C0C0, 0);
      displayButton(8, 8+fs*3, GDIM == 3 ? XLAT("l = color group: %1", its(dslayer)) : XLAT("l = layers: %1", its(dslayer)), 'l', 0);
      }

    if(us && isize(us->d[dslayer].list)) {
      usershapelayer& ds(us->d[dslayer]);
      if(!EDITING_TRIANGLES) {
        displayButton(8, 8+fs*4, XLAT("1-9 = rotations: %1", its(ds.rots)), '1' + (ds.rots % 9), 0);
        displayButton(8, 8+fs*5, ds.sym ? XLAT("0 = symmetry") : XLAT("0 = asymmetry"), '0', 0);
        }

      displayfr(8, 8+fs*7, 2, vid.fsize, XLAT("%1 vertices", its(isize(ds.list))), 0xC0C0C0, 0);
      displaymm('a', 8, 8+fs*8, 2, vid.fsize, XLAT("a = add v"), 0);
      if(!EDITING_TRIANGLES) {
        if(autochoose) {
          displaymm('m', 8, 8+fs*9, 2, vid.fsize, XLAT("m = move v"), 0);
          displaymm('d', 8, 8+fs*10, 2, vid.fsize, XLAT("d = delete v"), 0);
          }
        else {
          displayButton(8, 8+fs*9, XLAT("m = move v"), 'm', 0);
          displayButton(8, 8+fs*10, XLAT("d = delete v"), 'd', 0);
          }
        displaymm('c', 8, 8+fs*11, 2, vid.fsize, autochoose ? XLAT("autochoose") : XLAT("c = choose"), 0);
        displayButton(8, 8+fs*12, XLAT("b = switch auto"), 'b', 0);
        }
      else {
        displayfr(8, 8+fs*9, 2, vid.fsize, XLAT("c = reuse"), 0xC0C0C0, 0);
        displayfr(8, 8+fs*10, 2, vid.fsize, XLAT("d = delete"), 0xC0C0C0, 0);
        }

      if(GDIM == 2) {
        displayfr(8, 8+fs*14, 2, vid.fsize, XLAT("t = shift"), 0xC0C0C0, 0);
        displayfr(8, 8+fs*15, 2, vid.fsize, XLAT("y = spin"), 0xC0C0C0, 0);
        }
      if(mousekey == 'g')
        displayButton(8, 8+fs*16, XLAT("p = grid color"), 'p', 0);
      else
        displayButton(8, 8+fs*16, XLAT("p = paint"), 'p', 0);
      if(GDIM == 2) 
        displayfr(8, 8+fs*17, 2, vid.fsize, XLAT("z = z-level"), 0xC0C0C0, 0);
      if(GDIM == 2)
        displayButton(8, 8+fs*18, XLAT("S = snap (%1)", ONOFF(snapping)), 'S', 0);
      }
#if CAP_TEXTURE
    else if(freedraw) {
      displayButton(8, 8+fs*2, texture::texturesym ? XLAT("0 = symmetry") : XLAT("0 = asymmetry"), '0', 0);
      if(mousekey == 'g')
        displayButton(8, 8+fs*16, XLAT("p = grid color"), 'p', 0);
      else
        displayButton(8, 8+fs*16, XLAT("p = color"), 'p', 0);
      if(drawing_tool)
        displayButton(8, 8+fs*17, XLAT("f = fill") + (dtfill ? " (on)" : " (off)"), 'f', 0);
      displayButton(8, 8+fs*4, XLAT("b = brush size: %1", fts(dtwidth)), 'b', 0);
      displayButton(8, 8+fs*5, XLAT("u = undo"), 'u', 0);
      displaymm('d', 8, 8+fs*7, 2, vid.fsize, XLAT("d = draw"), 0);
      displaymm('l', 8, 8+fs*8, 2, vid.fsize, XLAT("l = line"), 0);
      displaymm('c', 8, 8+fs*9, 2, vid.fsize, XLAT("c = circle"), 0);
      if(drawing_tool)
        displaymm('T', 8, 8+fs*10, 2, vid.fsize, XLAT("T = text"), 0);
      if(drawing_tool)
        displaymm('e', 8, 8+fs*11, 2, vid.fsize, XLAT("e = erase"), 0);
      int s = isize(texture::config.data.pixels_to_draw);
      if(s) displaymm(0, 8, 8+fs*12, 2, vid.fsize, its(s), 0);
      }
#endif
    else {
      displaymm('n', 8, 8+fs*5, 2, vid.fsize, XLAT("'n' to start"), 0);
      displaymm('u', 8, 8+fs*6, 2, vid.fsize, XLAT("'u' to load current"), 0);
      if(mousekey == 'a' || mousekey == 'd' || mousekey == 'd' ||
        mousekey == 'c') mousekey = 'n';
      if(GDIM == 2)
        displayButton(8, 8+fs*7, XLAT("S = snap (%1)", ONOFF(snapping)), 'S', 0);
      }
    
    if(GDIM == 3)
      displayfr(8, 8+fs*19, 2, vid.fsize, (front_config == eFront::sphere_camera ? XLAT("z = camera") : front_config == eFront::sphere_center ? XLAT("z = spheres") : 
        nonisotropic && front_config == eFront::equidistants ? XLAT("Z =") :
        nonisotropic && front_config == eFront::const_x ? XLAT("X =") :
        nonisotropic && front_config == eFront::const_y ? XLAT("Y =") :
        XLAT("z = equi")) + " " + fts(front_edit), 0xC0C0C0, 0);

    displaymm('g', vid.xres-8, 8+fs*4, 2, vid.fsize, XLAT("g = grid"), 16);

#if CAP_TEXTURE    
    if(freedraw) for(int i=0; i<10; i++) {
      if(8 + fs * (6+i) < vid.yres - 8 - fs * 7)
        displayColorButton(vid.xres-8, 8+fs*(6+i), "###", 1000 + i, 16, 1, dialog::displaycolor(texture_colors[i+1]));

      if(displayfr(vid.xres-8 - fs * 3, 8+fs*(6+i), 0, vid.fsize, its(i+1), dtwidth == brush_sizes[i] ? 0xFF8000 : 0xC0C0C0, 16))
        getcstat = 2000+i;
      }

    if(!freedraw)
      displaymm('e', vid.xres-8, 8+fs, 2, vid.fsize, XLAT("e = edit this"), 16);
#endif

    int prec = snapping ? 15 : 4;

    if(!mouseout()) {
      shiftpoint h1 = drawtrans * ccenter;
      shiftpoint h2 = drawtrans * coldcenter;
      transmatrix T = gpushxto0(unshift(h1));
      T = spintox(T*unshift(h2)) * T;

      shiftpoint mh1 = full_mouseh();
      hyperpoint mh = T * unshift(mh1);

      displayfr(vid.xres-8, vid.yres-8-fs*7, 2, vid.fsize, XLAT("x: %1", fts(mh[0],prec)), 0xC0C0C0, 16);
      displayfr(vid.xres-8, vid.yres-8-fs*6, 2, vid.fsize, XLAT("y: %1", fts(mh[1],prec)), 0xC0C0C0, 16);
      displayfr(vid.xres-8, vid.yres-8-fs*5, 2, vid.fsize, XLAT("z: %1", fts(mh[2],prec)), 0xC0C0C0, 16);
      if(MDIM == 4)
        displayfr(vid.xres-8, vid.yres-8-fs*4, 2, vid.fsize, XLAT("w: %1", fts(mh[3],prec)), 0xC0C0C0, 16);
      mh = inverse_exp(shiftless(mh));
      displayfr(vid.xres-8, vid.yres-8-fs*3, 2, vid.fsize, XLAT("r: %1", fts(hypot_d(3, mh),prec)), 0xC0C0C0, 16);
      if(GDIM == 3) {
        displayfr(vid.xres-8, vid.yres-8-fs, 2, vid.fsize, XLAT("ϕ: %1°", fts(-atan2(mh[2], hypot_d(2, mh)) / degree,prec)), 0xC0C0C0, 16);
        displayfr(vid.xres-8, vid.yres-8-fs*2, 2, vid.fsize, XLAT("λ: %1°", fts(-atan2(mh[1], mh[0]) / degree,prec)), 0xC0C0C0, 16);
        }
      else {
        displayfr(vid.xres-8, vid.yres-8-fs*2, 2, vid.fsize, XLAT("ϕ: %1°", fts(-atan2(mh[1], mh[0]) / degree,prec)), 0xC0C0C0, 16);
        }
      }

    if(us) {
      cgi.require_usershapes();
      auto& sh = cgi.ushr[&us->d[dslayer]];
      if(sh.e >= sh.s + 3)
        displayButton(vid.xres-8, vid.yres-8-fs*8, XLAT("area: %1", area_in_pi ? fts(compute_area(sh) / M_PI, 4) + "π" : fts(compute_area(sh), prec)), 'w', 16);
      }

    
    displayFunctionKeys();
    
    keyhandler = drawHandleKey;
#else
    popScreen();
#endif
    }
  
#if CAP_POLY
  void loadShapes(int sg, int id) {
    delete usershapes[sg][id];
    usershapes[sg][id] = NULL;

    initquickqueue();
    
    dynamicval<bool> ws(mmspatial, false);
    
    auto sId = shiftless(Id);
    if(sg == 0) {
      multi::cpid = id, drawMonsterType(moPlayer, drawcell, sId, 0xC0C0C0, 0, 0xC0C0C0);
      }
    else if(sg == 1) {
      drawMonsterType(eMonster(id), drawcell, sId, minf[id].color, 0, minf[id].color);
      }
    else if(sg == 2) {
      drawItemType(eItem(id), drawcell, sId, iinf[id].color, 0, false);
      }
    else {
      draw_qfi(drawcell, sId, 0, PPR::FLOOR);
      }

    sortquickqueue();
      
    int layer = 0;
    
    initShape(sg, id);
    
    for(int i=0; i<isize(ptds); i++) { 
      auto pp = dynamic_cast<dqi_poly*> (&*ptds[i]);
      if(!pp) continue;
      auto& ptd = *pp;
      
      int cnt = ptd.cnt;
      
      usershapelayer *dsCur = &usershapes[sg][id]->d[layer];
      dsCur->list.clear();
      dsCur->color = ptd.color;
      dsCur->sym = false;
      dsCur->rots = 1;
      dsCur->zlevel = 0;
      
      for(auto& v: cgi.symmetriesAt)
        if(v[0] == ptd.offset) {
          dsCur->rots = v[1];
          dsCur->sym = v[2] == 2;
          }
        
      int d = dsCur->rots * (dsCur->sym ? 2 : 1);
      
      for(int i=0; i < cnt/d; i++)
        dsCur->list.push_back(unshift(ptd.V) * glhr::gltopoint((*ptd.tab)[i+ptd.offset]));
      
      layer++;      
      if(layer == USERLAYERS) break;
      }
    usershape_changes++;
    }

  void applyToShape(int sg, int id, int uni, hyperpoint mh) {
    bool haveshape = usershapes[sg][id];
    bool xnew = false;
    
    if(uni == '-') uni = mousekey;
    
    if(!haveshape) {
      if(uni == 'n')
        initShape(sg, id);
      else if(uni == 'u') ;
      else if(uni == 'S') { snapping = !snapping; return; }
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
      usershape_changes++;
      }

    if(uni == 'u') 
      loadShapes(sg, id);
    
    if(uni == 'S') snapping = !snapping;

    if(uni == 'z' && haveshape && GDIM == 2)
      dialog::editNumber(dsCur->zlevel, -10, +10, 0.1, 0, XLAT("z-level"),
        XLAT("Changing the z-level will make this layer affected by the parallax effect."));

    if(EDITING_TRIANGLES) {
      if(uni == 'a') {
        dsCur->list.push_back(mh);
        uni = 0;
        usershape_changes++;
        }
      else if(uni == 'c' || uni == 'd' || uni == 'm') {
        hyperpoint best = mh;
        hyperpoint onscr;
        applymodel(drawtrans * mh, onscr);
        println(hlog, "onscr = ", onscr);
        ld dist = HUGE_VAL;
        for(auto& layer: usershapes[sg][id]->d)
        for(const hyperpoint& h: layer.list) {
          hyperpoint h1;
          applymodel(drawtrans * h, h1);
          ld d = sqhypot_d(2, h1 - onscr);
          if(d < dist) dist = d, best = h;
          }
        if(uni == 'c') dsCur->list.push_back(best);
        else if(uni == 'd') {
          vector<hyperpoint> oldlist = std::move(dsCur->list);
          dsCur->list.clear();
          int i;
          for(i=0; i<isize(oldlist); i+=3)
            if(oldlist[i] != best && oldlist[i+1] != best && oldlist[i+2] != best)
              dsCur->list.push_back(oldlist[i]),
              dsCur->list.push_back(oldlist[i+1]),
              dsCur->list.push_back(oldlist[i+2]);
          for(; i<isize(oldlist); i++)
            if(oldlist[i] != best)
              dsCur->list.push_back(oldlist[i]);
          }
        usershape_changes++;
        uni = 0;
        }
      else if(uni == COLORKEY) dsCur->color = colortouse;
      else if(uni != 'D') uni = 0;
      }
    
    if(uni == 'a' && haveshape) {
      mh = spin(2*M_PI*-ew.rotid/dsCur->rots) * mh;
      if(ew.symid) mh = Mirror * mh;
    
      if(ew.pointid < 0 || ew.pointid >= isize(dsCur->list)) 
        ew.pointid = isize(dsCur->list)-1, ew.side = 1;

      dsCur->list.insert(dsCur->list.begin()+ew.pointid+(ew.side?1:0), mh);
      if(ew.side) ew.pointid++;
      usershape_changes++;
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
      usershape_changes++;
      }
    
    if(uni == 'K') {
      if(vid.cs.charid >= 4) {
        loadShape(sg, id, cgi.shCatBody, 2, 0);
        loadShape(sg, id, cgi.shCatHead, 2, 1);
        }
      else {
        if(!(vid.cs.charid&1)) loadShape(sg, id, cgi.shPBody, 2, 0);
        else loadShape(sg, id, cgi.shFemaleBody, 2, 0);
  
        loadShape(sg, id, cgi.shPSword, 1, 1);
  
        if(vid.cs.charid&1)
          loadShape(sg, id, cgi.shFemaleDress, 2, 2);

        /* if(vid.cs.charid&1)
          loadShape(sg, id, cgi.shPrincessDress, 1, 3);
        else
          loadShape(sg, id, cgi.shPrinceDress, 2, 3); */
        
        loadShape(sg, id, cgi.shRatCape2, 1, 3);
      
        if(vid.cs.charid&1)
          loadShape(sg, id, cgi.shFemaleHair, 2, 4);
        else
          loadShape(sg, id, cgi.shPHead, 2, 4);
        
        loadShape(sg, id, cgi.shPFace, 2, 5); 
        }
      
      // loadShape(sg, id, cgi.shWolf, 2, dslayer);
      
      usershape_changes++;
      }

    if(uni == '+') dsCur->rots++;

    if(uni >= '1' && uni <= '9') {
      dsCur->rots = uni - '0';
      if(dsCur->rots == 9) dsCur->rots = 21;
      usershape_changes++;
      }
    if(uni == '0') {
      dsCur->sym = !dsCur->sym;
      usershape_changes++;
      }

    if(uni == 't') {
      dsCur->shift = mh;
      usershape_changes++;
      }
    if(uni == 'y') {
      dsCur->spin = mh;
      usershape_changes++;
      }

    if(uni == COLORKEY) dsCur->color = colortouse;
    }

  void writeHyperpoint(hstream& f, hyperpoint h) {
    println(f, spaced_of(&h[0], MDIM));
    }
  
  hyperpoint readHyperpoint(fhstream& f) {
    hyperpoint h;
    for(int i=0; i<MDIM; i++) scan(f, h[i]);
    return h;
    }
  
  string drawHelpLine() {
    return XLAT("vector graphics editor");
    }
  
  bool onelayeronly;
  
  bool loadPicFile(const string& s) {
    fhstream f(picfile, "rt");
    if(!f.f) {
      addMessage(XLAT("Failed to load pictures from %1", picfile));
      return false;
      }
    scanline(f);
    scan(f, f.vernum);
    printf("vernum = %x\n", f.vernum);
    if(f.vernum == 0) {
      addMessage(XLAT("Failed to load pictures from %1", picfile));
      return false;
      }

    if(f.vernum >= 0xA0A0) {
      int tg, wp;
      int nt;
      scan(f, tg, nt, wp, patterns::subpattern_flags);
      patterns::whichPattern = patterns::ePattern(wp);
      set_geometry(eGeometry(tg));
      set_variation(eVariation(nt));
      start_game();
      }

    while(true) {
      int i, j, l, sym, rots, siz;
      color_t color;
      if(!scan(f, i, j, l, sym, rots, color, siz)) break;
      if(i == -1) break;
      if(siz < 0 || siz > 1000) break;
      
      if(i >= 4) {
        if(i < 8) patterns::whichPattern = patterns::ePattern("xxxxfpzH"[i]);
        patterns::subpattern_flags = 0;
        i = 3;
        }

      initShape(i, j);
      usershapelayer& ds(usershapes[i][j]->d[l]);
      if(f.vernum >= 0xA608) scan(f, ds.zlevel);
      ds.shift = readHyperpoint(f);
      ds.spin = readHyperpoint(f);
      ds.list.clear();
      for(int i=0; i<siz; i++) {
        ds.list.push_back(readHyperpoint(f));
        writeHyperpoint(hlog, ds.list[i]);
        }
      ds.sym = sym;
      ds.rots = rots;
      ds.color = color;
      }
    addMessage(XLAT("Pictures loaded from %1", picfile));
    
    usershape_changes++;
    return true;
    }
  
  bool savePicFile(const string& s) {
    fhstream f(picfile, "wt");
    if(!f.f) {
      addMessage(XLAT("Failed to save pictures to %1", picfile));
      return false;
      }
    println(f, "HyperRogue saved picture");
    println(f, f.vernum);
    if(f.vernum >= 0xA0A0)
      println(f, spaced(geometry, int(variation), patterns::whichPattern, patterns::subpattern_flags));
    for(int i=0; i<USERSHAPEGROUPS; i++) for(auto usp: usershapes[i]) {
      usershape *us = usp.second;
      if(!us) continue;
      
      for(int l=0; l<USERLAYERS; l++) if(isize(us->d[l].list)) {
        usershapelayer& ds(us->d[l]);
        println(f, spaced(i, usp.first, l, ds.sym, ds.rots, ds.color, int(isize(ds.list))));
        print(f, spaced(ds.zlevel), " ");
        writeHyperpoint(f, ds.shift);
        writeHyperpoint(f, ds.spin);
        println(f);
        for(int i=0; i<isize(ds.list); i++)
          writeHyperpoint(f, ds.list[i]);
        }
      }
    println(f, "-1");
    addMessage(XLAT("Pictures saved to %1", picfile));
    return true;
    }

  void drawHandleKey(int sym, int uni) {

    if(uni == PSEUDOKEY_WHEELUP && GDIM == 3 && front_step) {
      front_edit += front_step * shiftmul; return;
      }

    if(uni == PSEUDOKEY_WHEELDOWN && GDIM == 3 && front_step) {
      front_edit -= front_step * shiftmul; return;
      }

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
    
    shiftpoint mh = full_mouseh();
    hyperpoint mh1 = inverse_shift(drawtrans, mh);

    bool clickused = false;
    
    if((uni == 'p' && mousekey == 'g') || (uni == 'g' && coldcenter == ccenter && ccenter == mh1)) {
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
      coldcenter = ccenter, ccenter = mh1, clickused = true;
    
    if(uni == 'd' || uni == 'l' || uni == 'c' || uni == 'e' || uni == 'T')
      mousekey = uni;

    if(drawing_tool) {
      if(sym == SDLK_F2) save_level();
      if(sym == SDLK_F3) load_level();
      if(sym == SDLK_F5) {
        dialog::push_confirm_dialog([] {
          stop_game();
          enable_canvas();
          canvas_default_wall = waInvisibleFloor;
          patterns::whichCanvas = 'g';
          patterns::canvasback = 0xFFFFFF;
          dtcolor = (forecolor << 8) | 255;
          drawplayer = false;
          vid.use_smart_range = 2;
          start_game();
          },
          XLAT("Are you sure you want to restart? This will let you draw on a blank screen.")
          );
        }
      }

    if(uni == ' ' && (cheater || autocheat)) {
      drawing_tool = !drawing_tool;
      if(!drawing_tool) {
        popScreen();
        pushScreen(showMapEditor);
        }
      }

    if(uni == 'z' && GDIM == 3) {
      dialog::editNumber(front_edit, 0, 5, 0.1, 0.5, XLAT("z-level"), "");
      dialog::extra_options = [] () {
        dialog::addBoolItem(XLAT("The distance from the camera to added points."), front_config == eFront::sphere_camera, 'A');
        dialog::add_action([] { front_config = eFront::sphere_camera; });
        dialog::addBoolItem(XLAT("place points at fixed radius"), front_config == eFront::sphere_center, 'B');
        dialog::add_action([] { front_config = eFront::sphere_center; });
        dialog::addBoolItem(nonisotropic ? XLAT("place points on surfaces of const Z") : XLAT("place points on equidistant surfaces"), front_config == eFront::equidistants, 'C');
        dialog::add_action([] { front_config = eFront::equidistants; });
        if(nonisotropic) {
          dialog::addBoolItem(XLAT("place points on surfaces of const X"), front_config == eFront::const_x, 'D');
          dialog::add_action([] { front_config = eFront::const_x; });
          dialog::addBoolItem(XLAT("place points on surfaces of const Y"), front_config == eFront::const_y, 'E');
          dialog::add_action([] { front_config = eFront::const_y; });
          }
        dialog::addSelItem(XLAT("mousewheel step"), fts(front_step), 'S');
        dialog::add_action([] {
          popScreen();
          dialog::editNumber(front_step, -10, 10, 0.1, 0.1, XLAT("mousewheel step"), "hint: shift for finer steps");
          });
        if(front_config == eFront::sphere_center) {
          dialog::addSelItem(XLAT("parallels to draw"), its(parallels), 'P');
          dialog::add_action([] {
            popScreen();
            dialog::editNumber(parallels, 0, 72, 1, 12, XLAT("parallels to draw"), "");
            });
          dialog::addSelItem(XLAT("meridians to draw"), its(meridians), 'M');
          dialog::add_action([] {
            popScreen();
            dialog::editNumber(meridians, 0, 72, 1, 12, XLAT("meridians to draw"), "");
            });
          }
        else if(front_config != eFront::sphere_camera) {
          dialog::addSelItem(XLAT("range of grid to draw"), fts(equi_range), 'R');
          dialog::add_action([] {
            popScreen();
            dialog::editNumber(equi_range, 0, 5, 0.1, 1, XLAT("range of grid to draw"), "");
            });
          }
        };
      }
    
    if(sym == SDLK_F7) {
      drawplayer = !drawplayer;
      }

#if CAP_SHOT
    else if(sym == SDLK_F6) {
      pushScreen(shot::menu);
      }
#endif

    if(sym == SDLK_ESCAPE) popScreen();

    if(sym == SDLK_F1) {
      gotoHelp(drawhelptext());
      }

    if(sym == SDLK_F10) popScreen();
    else if(uni == 'A') popScreen();

    (void)clickused;
    
    
    bool freedraw = drawing_tool || intexture;

    if(freedraw) {
      if(lstartcell) lstart = ggmatrix(lstartcell) * lstart_rel;

#if CAP_TEXTURE    
      int tcolor = (dtcolor >> 8) | ((dtcolor & 0xFF) << 24);
#endif
      
      if(uni == '-' && !clickused) {
        if(mousekey == 'e') {
          dt_erase(mh);
          clickused = true;
          }
        else if(mousekey == 'l' || mousekey == 'c' || mousekey == 'T') {
          if(!holdmouse) lstart = mh, lstartcell = mouseover, lstart_rel = inverse_shift(ggmatrix(mouseover), lstart), holdmouse = true;
          }
#if CAP_TEXTURE
        else if(intexture) {
          if(!holdmouse) texture::config.data.undoLock();
          texture::drawPixel(mouseover, mh, tcolor);
          holdmouse = true; lstartcell = NULL;
          }
#endif
        else {
          dt_add_free(mh);
          holdmouse = true;
          }
        }
      
      if(sym == PSEUDOKEY_RELEASE) {
        printf("release\n");
#if CAP_TEXTURE
        if(mousekey == 'l' && intexture) { 
          texture::config.data.undoLock();
          texture::where = mouseover;
          texture::drawPixel(mouseover, mh, tcolor);
          texture::drawLine(mh, lstart, tcolor);
          lstartcell = NULL;
          }
        else 
#endif
        if(mousekey == 'l') {
          dt_add_line(mh, lstart, 10);
          lstartcell = NULL;
          }
#if CAP_TEXTURE
        else if(mousekey == 'c' && intexture) { 
          texture::config.data.undoLock();
          ld rad = hdist(lstart, mh);
          int circp = int(1 + 3 * (circlelength(rad) / dtwidth));
          if(circp > 1000) circp = 1000;
          shiftmatrix T = rgpushxto0(lstart);
          texture::where = lstartcell;
          for(int i=0; i<circp; i++)
            texture::drawPixel(T * xspinpush0(2 * M_PI * i / circp, rad), tcolor);
          lstartcell = NULL;
          }
#endif          
        else if(mousekey == 'c') {
          dt_add_circle(lstart, mh);
          lstartcell = NULL;
          }
        else if(mousekey == 'T') {
          static string text = "";
          dialog::edit_string(text, "", "");
          shiftpoint h = mh;
          dialog::reaction_final = [h] {
            if(text != "")
              dt_add_text(h, dtwidth * 50, text);
            };
          lstartcell = nullptr;          
          }
        else 
          dt_finish();
        }
      
      if(uni >= 1000 && uni < 1010)
        dtcolor = texture_colors[uni - 1000 + 1];

      if(uni >= 2000 && uni < 2010)
        dtwidth = brush_sizes[uni - 2000];

#if CAP_TEXTURE
      if(uni == '0')
        texture::texturesym = !texture::texturesym;

      if(uni == 'u') {
        texture::config.data.undo();
        }        
#endif
      
      if(uni == 'p') {
        if(!clickused)
          dialog::openColorDialog(dtcolor, texture_colors);
        }

      if(uni == 'f') {
        if(dtfill == dtcolor)
          dtfill = 0;
        else
          dtfill = dtcolor;
        }

      if(uni == 'b') 
        dialog::editNumber(dtwidth, 0, 0.1, 0.005, 0.02, XLAT("brush size"), XLAT("brush size"));
      }

    else {
      dslayer %= USERLAYERS;

      applyToShape(drawcellShapeGroup(), drawcellShapeID(), uni, mh1);

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
            println(hlog, spaced("//", i, usp.first, l, "[", ds.color, double(ds.zlevel), "]"));
            print(hlog, " ID, ", us->d[l].rots, ", ", us->d[l].sym?2:1, ", "); 
            for(int i=0; i<isize(us->d[l].list); i++) {
              for(int d=0; d<GDIM; d++) print(hlog, fts(us->d[l].list[i][d]), ", ");
              print(hlog, " ");
              }
            println(hlog);
            }
          }

        for(int i=0; i<USERSHAPEGROUPS; i++) for(auto usp: usershapes[i]) {
          auto us = usp.second;
          if(!us) continue;

          for(int l=0; l<USERLAYERS; l++) if(isize(us->d[l].list)) {
            usershapelayer& ds(us->d[l]);
            println(hlog, spaced("//", i, usp.first, l, "[", ds.color, double(ds.zlevel), "]"));
            print(hlog, "{");

            for(int r=0; r<us->d[l].rots; r++) {
              for(int i=0; i<isize(us->d[l].list); i++) {
                hyperpoint h = us->d[l].list[i];
                h = spin(360 * degree * r / us->d[l].rots) * h;
                for(int d=0; d<GDIM; d++) print(hlog, fts(h[d]), ", ");
                }
              if(us->d[l].sym) for(int i=isize(us->d[l].list)-1; i>=0; i--) {
                hyperpoint h = us->d[l].list[i];
                h[1] = -h[1];
                h = spin(360 * degree * r / us->d[l].rots) * h;
                for(int d=0; d<GDIM; d++) print(hlog, fts(h[d]), ", ");
                }
              }
            println(hlog, "}, ");
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
    }
#endif    

  auto hooks = addHook(hooks_clearmemory, 0, [] () {
    if(mapeditor::painttype == 4) 
      mapeditor::painttype = 0, mapeditor::paintwhat = 0,
      mapeditor::paintwhat_str = "clear monster";
    mapeditor::copysource.at = NULL;
    mapeditor::undo.clear();
    if(!cheater) patterns::displaycodes = false;
    if(!cheater) patterns::whichShape = 0;
    modelcell.clear();
    mapeditor::dtshapes.clear();
    dt_finish();
    drawcell = nullptr;
    }) + 
  addHook(hooks_removecells, 0, [] () {
    modelcell.clear();
    set_if_removed(mapeditor::copysource.at, NULL);
    });
#endif

  EX void initdraw(cell *c) {
    #if CAP_EDIT
    mapeditor::drawcell = c;
    ew.c = c;
    ew.rotid = 0;
    ew.symid = 0;
    ew.pointid = -1;
    ew.side = 0;
    ewsearch = ew;
    ccenter = coldcenter = C0;
    #endif
    }
  
  transmatrix textrans;

  EX void queue_hcircle(shiftmatrix Ctr, ld radius) {
    vector<hyperpoint> pts;
    int circp = int(6 * pow(2, vid.linequality));
    if(radius > 0.04) circp *= 2;
    if(radius > .1) circp *= 2; 
    
    for(int j=0; j<circp; j++)
      pts.push_back(xspinpush0(M_PI*j*2/circp, radius));
    for(int j=0; j<circp; j++) curvepoint(pts[j]);
    curvepoint(pts[0]);
    queuecurve(Ctr, dtcolor, 0, PPR::LINE);
    }

#if CAP_POLY
  EX bool haveUserShape(eShapegroup group, int id) {
  #if !CAP_EDIT
    return false;
  #else
    return usershapes[group].count(id) && usershapes[group][id];
  #endif
    }
#endif
  
#if CAP_TEXTURE      
  EX void draw_texture_ghosts(cell *c, const shiftmatrix& V) {
    if(!c) return;
    if(holdmouse && !lstartcell) return;
    cell *ls = lstartcell ? lstartcell : lmouseover;     
    if(!ls) return;
     
    auto sio = patterns::getpatterninfo0(ls);
    auto sih = patterns::getpatterninfo0(c);
    
    if(sio.id == sih.id) {
      if(c == ls)
        textrans = z_inverse(V.T * applyPatterndir(ls, sio));
      
      transmatrix mh = textrans * rgpushxto0(unshift(mouseh, V.shift));
      transmatrix ml = textrans * rgpushxto0(unshift(lstart, V.shift));

      for(int j=0; j<=texture::texturesym; j++)
      for(int i=0; i<c->type; i += sih.symmetries) {
        shiftmatrix M2 = V * applyPatterndir(c, sih) * spin(2*M_PI*i/c->type);
        if(j) M2 = M2 * Mirror;
        switch(holdmouse ? mousekey : 'd') {
          case 'c':
            queue_hcircle(M2 * ml, hdist(lstart, mouseh));
            break;
          case 'l':
            queueline(M2 * mh * C0, M2 * ml * C0, dtcolor, 4 + vid.linequality, PPR::LINE);
            break;
          default:
            queue_hcircle(M2 * mh, dtwidth);
          }                
        }
      }
    }
#endif

#if CAP_POLY
  EX bool drawUserShape(const shiftmatrix& V, eShapegroup group, int id, color_t color, cell *c, PPR prio IS(PPR::DEFAULT)) {
  #if !CAP_EDIT
    return false;
  #else
  
    // floors handled separately
    if(c && c == drawcell && editingShape(group, id) && group != sgFloor)
      drawtrans = V;

    usershape *us = usershapes[group][id];
    if(us) {  
      cgi.require_usershapes();
      for(int i=0; i<USERLAYERS; i++) {
        if(i != dslayer && onelayeronly) continue;
        usershapelayer& ds(us->d[i]);
        hpcshape& sh(cgi.ushr[&ds]);
    
        if(sh.s != sh.e) {
          auto& last = queuepolyat(mmscale(V, GDIM == 3 ? 0 : geom3::lev_to_factor(ds.zlevel)), sh, ds.color ? ds.color : color, prio);
          if(GDIM == 3) {
            last.tinf = &user_triangles_texture;
            last.offset_texture = ds.texture_offset;
            }
          }
        }
      }
  
    if(cmode & sm::DRAW) {

      if(c == drawcell && EDITING_TRIANGLES && mapeditor::editingShape(group, id)) {
        if(!us) return false;
        usershapelayer &ds(us->d[mapeditor::dslayer]);
        for(int i=0; i<isize(ds.list); i++) {
          int j = (i%3 == 2 ? i-2 : i+1);
          if(j < isize(ds.list))
            queueline(V * ds.list[i], V * ds.list[j], 0xFF00FFFF, -1, PPR::SUPERLINE);
          queuestr(V * ds.list[i], 10, "x", 0xFF00FF);
          }
        }

      if(mapeditor::editingShape(group, id) && !EDITING_TRIANGLES) {
  
        /* for(int a=0; a<isize(ds.list); a++) {
          hyperpoint P2 = V * ds.list[a];
    
          int xc, yc, sc;
          getcoord(P2, xc, yc, sc);
          queuestr(xc, yc, sc, 10, "x", 
            a == 0 ? 0x00FF00 : 
            a == isize(ds.list)-1 ? 0xFF0000 :
            0xFFFF00);
          } */
        
        if(!us) return false;
     
        usershapelayer &ds(us->d[mapeditor::dslayer]);
        
        shiftpoint moh = full_mouseh();
        
        hyperpoint mh = inverse_shift(mapeditor::drawtrans, moh);
    
        for(int a=0; a<ds.rots; a++) 
        for(int b=0; b<(ds.sym?2:1); b++) {
    
          if(mouseout()) break;
    
          shiftpoint P2 = V * spin(2*M_PI*a/ds.rots) * (b?Mirror*mh:mh);
        
          queuestr(P2, 10, "x", 0xFF00FF);
          }
        
        if(isize(ds.list) == 0) return us;
        
        shiftpoint Plast = V * spin(-2*M_PI/ds.rots) * (ds.sym?Mirror*ds.list[0]:ds.list[isize(ds.list)-1]);
        int state = 0;
        int gstate = 0;
        double dist2 = 0;
        shiftpoint lpsm;
        
        for(int a=0; a<ds.rots; a++) 
        for(int b=0; b<(ds.sym?2:1); b++) {
        
          hyperpoint mh2 = spin(2*M_PI*-ew.rotid/ds.rots) * mh;
          if(ew.symid) mh2 = Mirror * mh2;
          shiftpoint pseudomouse = V * spin(2*M_PI*a/ds.rots) * mirrorif(mh2, b);      
        
          for(int t=0; t<isize(ds.list); t++) {
            int ti = b ? isize(ds.list)-1-t : t;
  
            shiftpoint P2 = V * spin(2*M_PI*a/ds.rots) * mirrorif(ds.list[ti], b);
            
            if(!mouseout()) {
              double d = hdist(moh, P2);
              if(d < ewsearch.dist)
                ewsearch.dist = d,
                ewsearch.rotid = a,
                ewsearch.symid = b,
                ewsearch.pointid = ti,
                ewsearch.c = c,
                ewsearch.side = b,
                state = 1,
                dist2 = d + hdist(moh, Plast) - hdist(P2, Plast);
            
              else if(state == 1) {
                double dist3 = d + hdist(moh, Plast) - hdist(P2, Plast);
                if(dist3 < dist2) 
                  ewsearch.side = !ewsearch.side;
                state = 2;
                }
              }
            
            queuestr(P2, 10, "o", 
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
          shiftpoint P2 = V * ds.list[0];
          if(hdist(moh, P2) + hdist(moh, Plast) - hdist(P2, Plast) < dist2) 
            ewsearch.side = 1;
          }
        }
      
      }
  
    return us;
  #endif
    }
#endif


  EX string canvasFloorName(int id) {
    if(id>=0 && id<caflEND)
      return XLAT(canvasFloorNames[id]);
    return its(id);
    }
  string allCanvasFloorNames() {
    string ret;
    for(int i=0; i<caflEND; i++) {
      ret += its(i) + ":" + canvasFloorName(i) + "  ";
      }
    return ret;
    }

  EX void map_settings() {
    cmode = sm::SIDE | sm::MAYDARK;
    gamescreen();
  
    dialog::init(XLAT("Map settings"));
  
    dialog::addBoolItem_action_neg(XLAT("disable wandering monsters"), gen_wandering, 'w');

    if(gen_wandering) {
      dialog::addBoolItem_action_neg(XLAT("disable ghost timer"), timerghost, 'g');
      }
    else dialog::addBreak(100);

    dialog::addBoolItem_action(XLAT("simple pattern generation"), reptilecheat, 'p');
    dialog::addInfo(XLAT("(e.g. pure Reptile pattern)"));

    dialog::addBoolItem_action(XLAT("safety generation"), safety, 's');
    dialog::addInfo(XLAT("(no treasure, no dangers)"));

    dialog::addBoolItem(XLAT("god mode"), autocheat, 'G');
    dialog::add_action([] () { autocheat = true; });
    dialog::addInfo(XLAT("(unlock all, allow cheats, normal character display, cannot be turned off!)"));

    add_edit(game_keys_scroll);
    dialog::addInfo(XLAT("hint: shift+A to enter the map editor"));
    
    if(WDIM == 3 && !intra::in) {
      dialog::addBoolItem(XLAT("become a portal map"), intra::in, 'm');
      dialog::add_action_push(intra::become_menu);
      }

    if(WDIM == 3 && intra::in) {
      dialog::addItem(XLAT("manage portals"), 'm');
      dialog::add_action_push(intra::show_portals);
      }

    dialog::addItem(XLAT("change the pattern/color of new Canvas cells"), 'c');
    dialog::add_action_push(patterns::showPrePatternNoninstant);

    dialog::addItem(XLAT("configure WFC"), 'W');
    dialog::add_action_push(wfc::wfc_menu);

    dialog::addItem(XLAT("edit cell values"), 'G');
    dialog::add_action(push_debug_screen);

    dialog::addSelItem(XLAT("canvas floor shape"), canvasFloorName(canvasfloor), 'S');
    dialog::add_action([] {
      dialog::editNumber(canvasfloor, 0, caflEND - 1, 1, 0, XLAT("canvas floor shape"),allCanvasFloorNames());
      });

    dialog::addSelItem(XLAT("canvas darkness"), its(canvasdark), 'd');
    dialog::add_action([] {
      dialog::editNumber(canvasdark, 0, 2, 1, 0, XLAT("canvas darkness"),
      "0: no darkening (bright mode, canvas, reptiles, etc)\n"
      "1: light darkening (r'lyeh, palace, dragon chasms, etc)\n"
      "2: normal darkening (default, most lands)");
      });
    
    dialog::addBack();
    dialog::display();
    }
EX }

#if CAP_EDIT
EX string levelfile = "hyperrogue.lev";
EX const char *loadlevel = NULL;
EX string picfile = "hyperrogue.pic";

#if CAP_COMMANDLINE

int read_editor_args() {
  using namespace arg;
  if(argis("-lev")) { shift(); levelfile = args(); }
  else if(argis("-pic")) { shift(); picfile = args(); }
  else if(argis("-load")) { PHASE(3); shift(); mapstream::loadMap(args()); }
  else if(argis("-save")) { PHASE(3); shift(); mapstream::saveMap(args().c_str()); }
  else if(argis("-d:draw")) { PHASE(3); 
    #if CAP_EDIT
    start_game();
    mapeditor::drawing_tool = true;
    mapeditor::initdraw(cwt.at);
    launch_dialog(mapeditor::showDrawEditor);
    #endif
    }
  #if CAP_POLY
  else if(argis("-dred")) {
    PHASEFROM(2); 
    mapeditor::dtcolor = 0xFF0000FF;
    mapeditor::dtwidth = 0.1;
    }
  else if(argis("-picload")) { PHASE(3); shift(); mapeditor::loadPicFile(args()); }
  #endif
  else return 1;
  return 0;
  }

auto ah_editor = addHook(hooks_args, 0, read_editor_args);
#endif
#endif
}
