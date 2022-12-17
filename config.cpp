// Hyperbolic Rogue -- configuration
// Copyright (C) 2017-2018 Zeno Rogue, see 'hyper.cpp' for details

/** \file config.cpp 
 *  \brief Configuration -- initial settings, saving/loading ini files, menus, etc.
 */

#include "hyper.h"
namespace hr {

#if HDR
enum eCentering { face, edge, vertex };
#endif

EX eCentering centering;

EX function<bool()> auto_restrict;

EX void add_to_changed(struct setting *f);

EX bool return_false() { return false; }

#if HDR
struct supersaver {
  string name;
  virtual string save() = 0;
  virtual void load(const string& s) = 0;
  virtual bool dosave() = 0;
  virtual void reset() = 0;
  virtual bool affects(void* v) { return false; }
  virtual void set_default() = 0;
  virtual ~supersaver() = default;
  };

typedef vector<shared_ptr<supersaver>> saverlist;

extern saverlist savers;

struct setting {
  function<bool()> restrict;
  string parameter_name;
  string config_name;
  string menu_item_name;
  string help_text;
  reaction_t reaction;
  char default_key;
  cld last_value;
  bool is_editable;
  virtual bool available() { if(restrict) return restrict(); return true; }
  virtual bool affects(void *v) { return false; }
  virtual void add_as_saver() {}
  void show_edit_option() { show_edit_option(default_key); }
  virtual void show_edit_option(int key) {
    println(hlog, "default called!"); }
  virtual string search_key() { 
    return parameter_name + "|" + config_name + "|" + menu_item_name + "|" + help_text;
    }
  virtual cld get_cld() = 0;
  explicit setting() { restrict = auto_restrict; is_editable = false; }
  virtual void check_change() {
    cld val = get_cld();
    if(val != last_value) {
      last_value = val;
      add_to_changed(this);
      }
    }
  reaction_t sets;
  setting *set_sets(const reaction_t& s) { sets = s; return this; }
  setting *set_extra(const reaction_t& r);
  setting *set_reaction(const reaction_t& r);
  virtual ~setting() = default;
  virtual void load_from(const string& s) {
    println(hlog, "cannot load this parameter");
    exit(1);
    }
  };
#endif

setting *setting::set_extra(const reaction_t& r) {
  auto s = sets; set_sets([s, r] { if(s) s(); dialog::extra_options = r; }); return this;
  }

setting *setting::set_reaction(const reaction_t& r) {
  reaction = r; return this;
  }

EX map<string, std::unique_ptr<setting>> params;

EX void show_edit_option_enum(char* value, const string& name, const vector<pair<string, string>>& options, char key, setting *s);

#if HDR
struct list_setting : setting {
  virtual int get_value() = 0;
  virtual void set_value(int i) = 0;
  vector<pair<string, string> > options;
  list_setting* editable(const vector<pair<string, string> >& o, string menu_item_name, char key) {
    is_editable = true;
    options = o;
    this->menu_item_name = menu_item_name;
    default_key = key;
    return this;
    }
  void show_edit_option(int key) override;
  };

template<class T> struct enum_setting : list_setting {
  T *value;
  T dft;
  int get_value() override { return (int) *value; }
  void set_value(int i) override { *value = (T) i; }
  bool affects(void* v) override { return v == value; }
  void add_as_saver() override;
  cld get_cld() override { return get_value(); }
  void load_from(const string& s) override {
    *value = (T) parseint(s);
    }
  };

struct float_setting : public setting {
  ld *value;
  ld dft;
  ld min_value, max_value, step;
  string unit;
  float_setting *editable(ld min_value, ld max_value, ld step, string menu_item_name, string help_text, char key) {
    is_editable = true;
    this->min_value = min_value;
    this->max_value = max_value;
    this->menu_item_name = menu_item_name;
    this->help_text = help_text;
    this->step = step;
    default_key = key;
    return this;
    }
  function<void(float_setting*)> modify_me;
  float_setting *modif(const function<void(float_setting*)>& r) { modify_me = r; return this; }
  void add_as_saver() override;
  bool affects(void *v) override { return v == value; }
  void show_edit_option(int key) override;
  cld get_cld() override { return *value; }
  void load_from(const string& s) override;
  };

struct int_setting : public setting {
  int *value;
  int dft;
  int min_value, max_value;
  ld step;
  void add_as_saver() override;
  function<void(int_setting*)> modify_me;
  int_setting *modif(const function<void(int_setting*)>& r) { modify_me = r; return this; }
  bool affects(void *v) override { return v == value; }
  void show_edit_option(int key) override;
  cld get_cld() override { return *value; }
  int_setting *editable(int min_value, int max_value, ld step, string menu_item_name, string help_text, char key) {
    this->is_editable = true;
    this->min_value = min_value;
    this->max_value = max_value;
    this->menu_item_name = menu_item_name;
    this->help_text = help_text;
    this->step = step;
    default_key = key;
    return this;
    }

  void load_from(const string& s) override {
    *value = parseint(s);
    }
  };

struct bool_setting : public setting {
  bool *value;
  bool dft;
  void add_as_saver() override;
  reaction_t switcher;
  bool_setting* editable(string cap, char key ) {
    is_editable = true;
    menu_item_name = cap; default_key = key; return this; 
    } 
  bool affects(void *v) override { return v == value; }
  void show_edit_option(int key) override;
  cld get_cld() override { return *value ? 1 : 0; }
  void load_from(const string& s) override {
    *value = parseint(s);
    }
  };

struct custom_setting : public setting {  
  function<void(char)> custom_viewer;
  function<cld()> custom_value;
  function<bool(void*)> custom_affect;
  void show_edit_option(int key) override { custom_viewer(key); }
  cld get_cld() override { return custom_value(); }
  bool affects(void *v) override { return custom_affect(v); }
  };
  
#if CAP_CONFIG

template<class T> struct dsaver : supersaver {
  T& val;
  T dft;
  bool dosave() override { return val != dft; }
  void reset() override { val = dft; }
  explicit dsaver(T& val) : val(val) { }
  bool affects(void* v) override { return v == &val; }
  void set_default() override { dft = val; }
  };

template<class T> struct saver : dsaver<T> {};

template<class T, class U, class V> void addsaver(T& i, U name, V dft) {
  auto s = make_shared<saver<T>> (i);
  s->dft = dft;
  s->name = name;
  savers.push_back(s);
  }

template<class T> void addsaver(T& i, string name) {
  addsaver(i, name, i);
  }

template<class T> void removesaver(T& val) {
  for(int i=0; i<isize(savers); i++)
    if(savers[i]->affects(&val))
      savers.erase(savers.begin() + i);
  }

template<class T> void set_saver_default(T& val) {
  for(auto sav: savers)
    if(sav->affects(&val))
      sav->set_default();
  }

template<class T> struct saverenum : supersaver {
  T& val;
  T dft;
  explicit saverenum(T& v) : val(v) { }
  bool dosave() override { return val != dft; }
  void reset() override { val = dft; }
  string save() override { return its(int(val)); }
  void load(const string& s) override { val = (T) atoi(s.c_str()); }
  bool affects(void* v) override { return v == &val; }
  void set_default() override { dft = val; }
  };

template<class T, class U> void addsaverenum(T& i, U name, T dft) {
  auto s = make_shared<saverenum<T>> (i);
  s->dft = dft;
  s->name = name;
  savers.push_back(s);
  }

template<class T, class U> void addsaverenum(T& i, U name) {
  addsaverenum(i, name, i);
  }

template<> struct saver<int> : dsaver<int> {
  explicit saver(int& val) : dsaver<int>(val) { }
  string save() override { return its(val); }
  void load(const string& s) override { val = atoi(s.c_str()); }
  };

template<> struct saver<char> : dsaver<char> {
  explicit saver(char& val) : dsaver<char>(val) { }
  string save() override { return its(val); }
  void load(const string& s) override { val = atoi(s.c_str()); }
  };

template<> struct saver<bool> : dsaver<bool> {
  explicit saver(bool& val) : dsaver<bool>(val) { }
  string save() override { return val ? "yes" : "no"; }
  void load(const string& s) override { val = isize(s) && s[0] == 'y'; }
  };

template<> struct saver<unsigned> : dsaver<unsigned> {
  explicit saver(unsigned& val) : dsaver<unsigned>(val) { }
  string save() override { return itsh(val); }
  void load(const string& s) override { val = (unsigned) strtoll(s.c_str(), NULL, 16); }
  };

template<> struct saver<string> : dsaver<string> {
  explicit saver(string& val) : dsaver<string>(val) { }
  string save() override { return val; }
  void load(const string& s) override { val = s; }
  };

template<> struct saver<ld> : dsaver<ld> {
  explicit saver(ld& val) : dsaver<ld>(val) { }
  string save() override { return fts(val, 10); }
  void load(const string& s) override {
    if(s == "0.0000000000e+000") ; // ignore!
    else val = atof(s.c_str()); 
    }
  };
#endif
#endif

void float_setting::add_as_saver() { 
#if CAP_CONFIG
  addsaver(*value, config_name, dft);
#endif
  }

void int_setting::add_as_saver() { 
#if CAP_CONFIG
  addsaver(*value, config_name, dft);
#endif
  }

void bool_setting::add_as_saver() { 
#if CAP_CONFIG
  addsaver(*value, config_name, dft);
#endif
  }

void float_setting::load_from(const string& s) {
  *value = parseld(s);
  anims::animate_parameter(*value, s, reaction);
  if(reaction) reaction();
  }

void non_editable() {
  dialog::addHelp("Warning: editing this value through this menu may not work correctly");
  }

void float_setting::show_edit_option(int key) {
  if(modify_me) modify_me(this);
  dialog::addSelItem(XLAT(menu_item_name), fts(*value) + unit, key);
  dialog::add_action([this] () {
    add_to_changed(this);
    dialog::editNumber(*value, min_value, max_value, step, dft, XLAT(menu_item_name), help_text); 
    if(sets) sets();
    if(reaction) dialog::reaction = reaction;
    if(!is_editable) dialog::extra_options = non_editable;
    });
  }

void int_setting::show_edit_option(int key) {
  if(modify_me) modify_me(this);
  dialog::addSelItem(XLAT(menu_item_name), its(*value), key);
  dialog::add_action([this] () {
    add_to_changed(this);
    dialog::editNumber(*value, min_value, max_value, step, dft, XLAT(menu_item_name), help_text); 
    if(sets) sets();
    if(reaction) dialog::reaction = reaction;
    if(!is_editable) dialog::extra_options = non_editable;
    });
  }

void bool_setting::show_edit_option(int key) {
  dialog::addBoolItem(XLAT(menu_item_name), *value, key);
  dialog::add_action([this] () {
    add_to_changed(this);
    switcher(); if(sets) sets();
    if(reaction) reaction();
    });
  }

EX float_setting *param_f(ld& val, const string p, const string s, ld dft) {
  unique_ptr<float_setting> u ( new float_setting );
  u->parameter_name = p;
  u->config_name = s;
  u->menu_item_name = s;
  u->value = &val;
  u->last_value = dft;
  if(dft == 0) {
    u->min_value = -100;
    u->max_value = +100;
    }
  else {
    u->min_value = 0;
    u->max_value = 2 * dft;
    }
  u->step = dft / 10;
  u->dft = dft;
  val = dft;
  u->add_as_saver();
  auto f = &*u;
  params[u->parameter_name] = std::move(u);
  return f;
  }

EX string param_esc(string s) {
  string out;
  for(char c: s)
    if(c == ' ' || c == '-' || c == ':')
      out += '_';
    else
      out += c;
  return out;
  }

EX int_setting *param_i(int& val, const string s, int dft) {
  unique_ptr<int_setting> u ( new int_setting );
  u->parameter_name = param_esc(s);
  u->config_name = s;
  u->menu_item_name = s;
  u->value = &val;
  u->last_value = dft;
  u->dft = dft;
  val = dft;
  if(dft == 0) {
    u->min_value = -100;
    u->max_value = +100;
    }
  else {
    u->min_value = 0;
    u->max_value = 2 * dft;
    }
  u->add_as_saver();
  auto f = &*u;
  params[u->parameter_name] = std::move(u);
  return f;
  }

EX int_setting *param_i(int& val, const string s) { return param_i(val, s, val); }

EX bool_setting *param_b(bool& val, const string s, bool dft) {
  unique_ptr<bool_setting> u ( new bool_setting );
  u->parameter_name = param_esc(s);
  u->config_name = s;
  u->menu_item_name = s;
  u->value = &val;
  u->last_value = dft;
  u->dft = dft;
  u->switcher = [&val] { val = !val; };
  val = dft;
  u->add_as_saver();
  auto f = &*u;
  params[u->parameter_name] = std::move(u);
  return f;
  }

EX bool_setting *param_b(bool& val, const string s) { return param_b(val, s, val); }

#if HDR
template<class T> void enum_setting<T>::add_as_saver() { 
#if CAP_CONFIG
  addsaverenum(*value, config_name, dft);
#endif
  }

template<class T> enum_setting<T> *param_enum(T& val, const string p, const string s, T dft) {
  unique_ptr<enum_setting<T>> u ( new enum_setting<T> );
  u->parameter_name = p;
  u->config_name = s;
  u->menu_item_name = s;
  u->value = &val;
  u->dft = dft;
  val = dft;
  u->last_value = u->get_cld();
  u->add_as_saver();
  auto f = &*u;
  params[p] = std::move(u);
  return f;
  }
#endif

EX float_setting* param_f(ld& val, const string s) {
  return param_f(val, param_esc(s), s, val);
  }

EX float_setting* param_f(ld& val, const string p, const string s) {
  return param_f(val, p, s, val);
  }

EX float_setting* param_f(ld& val, const string s, ld dft) {
  return param_f(val, param_esc(s), s, dft);
  }

#if HDR
template<class T>
custom_setting* param_custom(T& val, const string& s, function<void(char)> menuitem, char key) {
  unique_ptr<custom_setting> u ( new custom_setting );
  u->parameter_name = param_esc(s);
  u->config_name = s;
  u->menu_item_name = s;
  u->last_value = (int) val;
  u->custom_viewer = menuitem;
  u->custom_value = [&val] () { return (int) val; };
  u->custom_affect = [&val] (void *v) { return &val == v; };
  u->default_key = key;
  u->is_editable = true;
  auto f = &*u;
  params[s] = std::move(u);
  return f;  
  }
#endif

EX ld bounded_mine_percentage = 0.1;
EX int bounded_mine_quantity, bounded_mine_max;

EX const char *conffile = "hyperrogue.ini";

/* extra space if more geometries are added */
EX array<ld, gGUARD+64> sightranges;

EX bool logfog;

EX videopar vid;

#define DEFAULT_WALLMODE (ISMOBILE ? 3 : 5)
#define DEFAULT_MONMODE  (ISMOBILE ? 2 : 4)

EX void android_settings_changed() {
  #if ISANDROID
  settingsChanged = true;
  #endif
  }

extern color_t floorcolors[landtypes];

EX charstyle& getcs(int id IS(multi::cpid)) {
  if(multi::players>1 && id >= 0 && id < multi::players)
    return multi::scs[id];
  else
    return vid.cs;
  }

struct charstyle_old {
  int charid;
  color_t skincolor, haircolor, dresscolor, swordcolor, dresscolor2, uicolor;
  bool lefthanded;
  };

EX void hread(hstream& hs, charstyle& cs) {
  // before 0xA61A there was no eyecolor
  if(hs.get_vernum() < 0xA61A) {
    charstyle_old cso;
    hread_raw(hs, cso);
    cs.charid = cso.charid;
    cs.skincolor = cso.skincolor;
    cs.haircolor = cso.haircolor;
    cs.dresscolor = cso.dresscolor;
    cs.swordcolor = cs.eyecolor = cso.swordcolor;
    if(cs.charid < 4) cs.eyecolor = 0;
    cs.dresscolor2 = cso.dresscolor2;
    cs.uicolor = cso.uicolor;
    cs.lefthanded = cso.lefthanded;    
    }
  else hread_raw(hs, cs);
  }

EX void hwrite(hstream& hs, const charstyle& cs) {
  hwrite_raw(hs, cs);
  }

// void hread(hstream& hs, charstyle& cs) { hread_raw(hs, cs); }
// void hwrite(hstream& hs, const charstyle& cs) { hwrite_raw(hs, cs); }

EX string csnameid(int id) {
  if(id == 0) return XLAT("male");
  if(id == 1) return XLAT("female");
  if(id == 2) return XLAT("Prince");
  if(id == 3) return XLAT("Princess");
  if(id == 4 || id == 5) return XLAT("cat");
  if(id == 6 || id == 7) return XLAT("dog");
  if(id == 8 || id == 9) return XLATN("Familiar");
  return XLAT("none");
  }

EX string csname(charstyle& cs) {
  return csnameid(cs.charid);
  }

EX int playergender() {
  return (getcs().charid >= 0 && (getcs().charid&1)) ? GEN_F : GEN_M; 
  }
EX int princessgender() {
  int g = playergender();
  if(vid.samegender) return g;
  return g == GEN_M ? GEN_F : GEN_M;
  }

EX int default_language;

EX int lang() { 
  if(vid.language >= 0)
    return vid.language; 
  return default_language;
  }

EX bool autojoy = true;

#if CAP_CONFIG
saverlist savers;
#endif

#if !CAP_CONFIG
template<class T, class U, class V> void addsaver(T& i, U name, V dft) {
  i = dft;
  }

template<class T, class U> void addsaver(T& i, U name) {}
template<class T, class U> void addsaverenum(T& i, U name) {}
template<class T, class U> void addsaverenum(T& i, U name, T dft) { i = dft; }
#endif

EX void addsaver(charstyle& cs, string s) {
  addsaver(cs.charid, s + ".charid");
  addsaver(cs.skincolor, s + ".skincolor");
  addsaver(cs.eyecolor, s + ".eyecolor");
  addsaver(cs.haircolor, s + ".haircolor");
  addsaver(cs.dresscolor, s + ".dresscolor");
  addsaver(cs.swordcolor, s + ".swordcolor");
  addsaver(cs.dresscolor2, s + ".dresscolor2");
  addsaver(cs.uicolor, s + ".uicolor");
  addsaver(cs.lefthanded, s + ".lefthanded");
  }
  
// R:239, G:208, B:207 

unsigned int skincolors[]  = { 7, 0xD0D0D0FF, 0xEFD0C9FF, 0xC77A58FF, 0xA58869FF, 0x602010FF, 0xFFDCB1FF, 0xEDE4C8FF };
unsigned int haircolors[]  = { 8, 0x686868FF, 0x8C684AFF, 0xF2E1AEFF, 0xB55239FF, 0xFFFFFFFF, 0x804000FF, 0x502810FF, 0x301800FF };
unsigned int dresscolors[] = { 6, 0xC00000FF, 0x00C000FF, 0x0000C0FF, 0xC0C000FF, 0xC0C0C0FF, 0x202020FF };
unsigned int dresscolors2[] = { 7, 0x8080FFC0, 0x80FF80C0, 0xFF8080C0, 0xFFFF80C0, 0xFF80FFC0, 0x80FFFFC0, 0xFFFFFF80 };
unsigned int swordcolors[] = { 6, 0xC0C0C0FF, 0xFFFFFFFF, 0xFFC0C0FF, 0xC0C0FFFF, 0x808080FF, 0x202020FF };
unsigned int eyecolors[] = { 4, 0x00C000FF, 0x0000C0FF, 0xC00000FF, 0xC0C000FF, 0x804010FF, 0x00C000FF };

EX void initcs(charstyle &cs) {
  cs.charid     = 0;
  cs.skincolor  = 0xD0D0D0FF;
  cs.haircolor  = 0x686868FF;
  cs.dresscolor = 0xC00000FF;
  cs.swordcolor = 0xD0D0D0FF;
  cs.dresscolor2= 0x8080FFC0;
  cs.uicolor    = 0xFF0000FF;
  cs.eyecolor   = 0x603000FF;
  cs.lefthanded = false;
  }

EX void savecolortable(colortable& ct, string name) {
  for(int i=0; i<isize(ct); i++)
    addsaver(ct[i], "color:" + name + ":" + its(i));
  }

EX purehookset hooks_configfile;

EX void initConfig() {
  
  // basic config
  param_i(vid.flashtime, "flashtime", 8);
  param_enum(vid.msgleft, "message_style", "message style", 2)
    -> editable({{"centered", ""}, {"left-aligned", ""}, {"line-broken", ""}}, "message style", 'a');

  param_i(vid.msglimit, "message limit", 5);
  param_i(vid.timeformat, "message log time format", 0);
  
  param_b(resizable, "resizable", true)
  -> editable("resizable window", 'r');

  param_b(no_find_player, "no_find_player");
  param_b(game_keys_scroll, "game_keys_scroll")
  -> editable("pure exploration (game keys scroll)", 'P');
  param_b(reg3::cubes_reg3, "cubes_reg3");
  param_f(linepatterns::tree_starter, "tree_starter")
  -> editable(0, 1, 0.05, "tree-drawing parameter", "How much of edges to draw for tree patterns (to show how the tree edges are oriented).", 't');

  param_b(arb::apeirogon_consistent_coloring, "apeirogon_consistent_coloring", true)
  -> editable("apeirogon_consistent_coloring", 'c');
  param_b(arb::apeirogon_hide_grid_edges, "apeirogon_hide_grid_edges", true)
  -> editable("apeirogon_hide_grid_edges", 'h');
  param_b(arb::apeirogon_simplified_display, "apeirogon_simplified_display", false)
  -> editable("simplified display of apeirogons", 'f');
  param_b(arb::convert::minimize_on_convert, "tes_minimize_on_convert", false)
  -> editable("consider all symmetries when converting", 'm');
  param_b(arb::convert::reverse_order, "tes_reverse_order", false)
  -> editable("tes reverse order on convert", 'r');

  param_b(display_yasc_codes, "yasc", false)
  -> editable("YASC codes", 'Y')
  -> set_reaction([] { 
    addMessage(XLAT("YASC codes: Sides-Entity-Restrict-Threat-Wall"));  
    });

  param_b(vid.relative_font, "relative_font", true)
  -> editable("set relative font size", 'r')
  -> set_reaction(compute_fsize);
  param_i(vid.fontscale, "fontscale", 100)
  -> editable(25, 400, 10, "font scale", "", 'b')
  -> set_reaction(compute_fsize)
  -> set_sets([] { dialog::bound_low(0); });

  param_i(vid.abs_fsize, "fsize", 12)
  -> editable(1, 72, 1, "font size", "", 'b')
  -> set_reaction(compute_fsize)
  -> set_sets([] { dialog::bound_low(0); });

  param_i(vid.mobilecompasssize, "mobile compass size", 0); // ISMOBILE || ISPANDORA ? 30 : 0);
  param_i(vid.radarsize, "radarsize size", 120);
  param_f(vid.radarrange, "radarrange", 2.5);
  param_i(vid.axes, "movement help", 1);
  param_b(vid.axes3, "movement help3", true);
  param_i(vid.shifttarget, "shift-targetting", 2);
  addsaver(vid.steamscore, "scores to Steam", 1);
  initcs(vid.cs); addsaver(vid.cs, "single");
  param_b(vid.samegender, "princess choice", false);
  addsaver(vid.language, "language", -1);  
  param_b(vid.drawmousecircle, "mouse circle", ISMOBILE || ISPANDORA);
  param_b(vid.revcontrol, "reverse control", false);
  #if CAP_AUDIO
  param_i(musicvolume, "music volume")
  ->editable(0, 128, 10, "background music volume", "", 'b')
  ->set_sets(sets_music_volume);
  #endif
  #if CAP_SDLAUDIO
  addsaver(music_out_of_focus, "music out of focus", false);
  #endif
  #if CAP_AUDIO
  param_i(effvolume, "sound effect volume")
  ->editable(0, 128, 10, "sound effects volume", "", 'e')
  ->set_sets(sets_sfx_volume);
  #endif

  param_enum(vid.faraway_highlight, "faraway_highlight", "highlight faraway monsters", tlNoThreat)
    ->editable({{"off", ""}, {"spam", ""}, {"normal monsters", ""}, {"high-threat monsters only", ""}}, "highlight faraway monsters", 'h');

  param_i(vid.faraway_highlight_color, "faraway_highlight_color", 50)
  -> editable(0, 100, 10, "faraway highlight color", "0 = monster color, 100 = red-light oscillation", 'c');

  param_enum(glyphsortorder, "glyph_sort", "glyph sort order", glyphsortorder)
    ->editable({
      {"first on top", ""},
      {"first on bottom", ""},
      {"last on top", ""},
      {"last on bottom", ""},
      {"by land", ""},
      {"by number", ""}
      }, "inventory/kill sorting", 'k');

  param_enum(vid.orbmode, "orb_mode", "orb display mode", 2)
    ->editable({
      {"plain", ""},
      {"types", ""},
      {"icons", ""},
      }, "orb display mode", 'o');

  param_b(less_in_landscape, "less_in_landscape", false)
  ->editable("less items/kills in landscape", 'L')
  -> set_sets([] { dialog::reaction_final = [] { println(hlog, "Reset"); vid.killreduction = 0; }; });

  param_b(less_in_portrait, "less_in_portrait", false)
  ->editable("less items/kills in portrait", 'P')
  -> set_sets([] { dialog::reaction_final = [] { println(hlog, "Reset"); vid.killreduction = 0; }; });
  
  // basic graphics
  
  param_b(vid.wantGL, "usingGL", true)
  ->editable("openGL mode", 'o');
  
  addsaver(vid.want_antialias, "antialias", AA_NOGL | AA_FONT | (ISWEB ? AA_MULTI : AA_LINES) | AA_VERSION);
  param_b(vid.fineline, "fineline", true);
  param_f(vid.linewidth, "linewidth", 1);
  addsaver(precise_width, "precisewidth", .5);
  param_i(perfect_linewidth, "perfect_linewidth", 1);
  param_f(linepatterns::width, "lpwidth", "pattern-linewidth", 1);
  addsaver(fat_edges, "fat-edges");
  param_f(vid.sspeed, "sspeed", "scrollingspeed", 0);
  param_f(vid.mspeed, "mspeed", "movement speed", 1);
  param_f(vid.ispeed, "ispeed", "idle speed", 1);
  addsaver(vid.aurastr, "aura strength", ISMOBILE ? 0 : 128);
  addsaver(vid.aurasmoothen, "aura smoothen", 5);
  param_enum(vid.graphglyph, "graphglyph", "graphical items/kills", 1)
  -> editable({{"letters", ""}, {"auto", ""}, {"images", ""}}, "inventory/kill mode", 'd');

  param_i(menu_darkening, "menu_darkening", 2)
  -> editable(0, 8, 1, "menu map darkening", "A larger number means darker game map in the background. Set to 8 to disable the background.", 'd')
  -> set_sets([] { dialog::bound_low(0); dialog::bound_up(8); dialog::dialogflags |= sm::DARKEN; });
  param_b(centered_menus, "centered_menus", false)
  -> editable("centered menus in widescreen", 'c');

  param_b(startanims::enabled, "startanim", true)
  -> editable("start animations", 's');

  addsaver(vid.flasheffects, "flasheffects", 1);

  param_f(vid.binary_width, "bwidth", "binary-tiling-width", 1);
  param_custom(vid.binary_width, "binary tiling width", menuitem_binary_width, 'v');
 
  addsaver(vid.particles, "extra effects", 1);
  param_i(vid.framelimit, "frame limit", 999);

  #if !ISMOBWEB
  param_b(vid.want_vsync, "vsync", true)
  ->editable("vsync", 'v');
  #endif
  
  param_b(vid.want_fullscreen, "fullscreen", false)
  ->editable("fullscreen mode", 'f');
  param_b(vid.change_fullscr, "fullscreen_change", false)
  ->editable("use specific fullscreen resolution", 'g');
  param_b(vid.relative_window_size, "window_relative", true)
  ->editable("specify relative window size", 'g');

  param_custom(vid.xres, "xres", [] (char ch) {}, 0)->restrict = return_false;
  param_custom(vid.yres, "yres", [] (char ch) {}, 0)->restrict = return_false;
  
  param_i(vid.fullscreen_x, "fullscreen_x", 1280)
  -> editable(640, 3840, 640, "fullscreen resolution to use (X)", "", 'x')
  -> set_sets([] { dialog::bound_low(640); dialog::reaction_final = do_request_resolution_change; });
  
  param_i(vid.fullscreen_y, "fullscreen_y", 1024)
  -> editable(480, 2160, 480, "fullscreen resolution to use (Y)", "", 'x')
  -> set_sets([] { dialog::bound_low(480); dialog::reaction_final = do_request_resolution_change; });

  param_i(vid.window_x, "window_x", 1280)
  -> editable(160, 3840, 160, "window resolution to use (X)", "", 'x')
  -> set_sets([] { dialog::bound_low(160); dialog::reaction_final = do_request_resolution_change; });

  param_i(vid.window_y, "window_y", 1024)
  -> editable(120, 2160, 120, "window resolution to use (Y)", "", 'x')
  -> set_sets([] { dialog::bound_low(120); dialog::reaction_final = do_request_resolution_change; });

  param_f(vid.window_rel_x, "window_rel_x", .9)
  -> editable(.1, 1, .1, "screen size percentage to use (X)", "", 'x')
  -> set_sets([] { dialog::bound_low(.1); dialog::reaction_final = do_request_resolution_change; });

  param_f(vid.window_rel_y, "window_rel_y", .9)
  -> editable(.1, 1, .1, "screen size percentage to use (Y)", "", 'x')
  -> set_sets([] { dialog::bound_low(.1); dialog::reaction_final = do_request_resolution_change; });

  param_b(vid.darkhepta, "mark heptagons", false);
  
  param_b(logfog, "logfog", false);

  for(auto& lp: linepatterns::patterns) {
    addsaver(lp->color, "lpcolor-" + lp->lpname);
    addsaver(lp->multiplier, "lpwidth-" + lp->lpname);
    }
  
  // special graphics

  addsaver(vid.monmode, "monster display mode", DEFAULT_MONMODE);
  addsaver(vid.wallmode, "wall display mode", DEFAULT_WALLMODE);
  addsaver(vid.highlightmode, "highlightmode");

  addsaver(vid.always3, "3D always", false);

  param_f(geom3::euclid_embed_scale, "euclid_embed_scale", "euclid_embed_scale")
  -> editable(0, 2, 0.05, "Euclidean embedding scale", "How to scale the Euclidean map, relatively to the 3D absolute unit.", 'F')
  -> set_sets([] { dialog::bound_low(0.05); })
  -> set_reaction([] { if(vid.always3) { geom3::switch_fpp(); geom3::switch_fpp(); } });

  param_enum(embedded_shift_method_choice, "embedded_shift_method", "embedded_shift_method", smcBoth)
  -> editable({
    {"geodesic", "always move on geodesics"},
    {"keep levels", "keep the vertical angle of the camera"},
    {"mixed", "on geodesics when moving camera manually, keep level when auto-centering"}
    }, "view shift for embedded planes", 'H');

  param_b(geom3::auto_configure, "auto_configure_3d", "auto_configure_3d")
  -> editable("set 3D settings automatically", 'A');

  param_b(geom3::inverted_embedding, "inverted_3d", false)
  -> editable("invert convex/concave", 'I')
  -> set_reaction([] { if(vid.always3) { geom3::switch_fpp(); geom3::switch_fpp(); } });

  param_b(geom3::flat_embedding, "flat_3d", false)
  -> editable("flat, not equidistant", 'F')
  -> set_reaction([] { if(vid.always3) { geom3::switch_fpp(); geom3::switch_fpp(); } });

  param_enum(geom3::spatial_embedding, "spatial_embedding", "spatial_embedding", geom3::seDefault)
  ->editable(geom3::spatial_embedding_options, "3D embedding method", 'E')
  ->set_reaction([] {
    if(vid.always3) {
      geom3::switch_fpp();
      geom3::switch_fpp();
      delete_sky();
      // not sure why this is needed...
      resetGL();
      }
    });
  
  param_b(memory_saving_mode, "memory_saving_mode", (ISMOBILE || ISPANDORA || ISWEB) ? 1 : 0);
  param_i(reserve_limit, "memory_reserve", 128);
  addsaver(show_memory_warning, "show_memory_warning");

  addsaver(rug::renderonce, "rug-renderonce");
  addsaver(rug::rendernogl, "rug-rendernogl");
  addsaver(rug::texturesize, "rug-texturesize");
#if CAP_RUG
  param_f(rug::model_distance, "rug_model_distance", "rug-model-distance");
#endif

  param_b(vid.backeffects, "background particle effects", (ISMOBILE || ISPANDORA) ? false : true);
  // control
  
  param_i(vid.joyvalue, "vid.joyvalue", 4800);
  param_i(vid.joyvalue2, "vid.joyvalue2", 5600);
  param_i(vid.joysmooth, "vid.joysmooth", 200);
  param_i(vid.joypanthreshold, "vid.joypanthreshold", 2500);
  param_f(vid.joypanspeed, "vid.joypanspeed", ISPANDORA ? 0.0001 : 0);
  addsaver(autojoy, "autojoy");
    
  vid.killreduction = 0;
  
  param_b(vid.skipstart, "skip the start menu", false);
  param_b(vid.quickmouse, "quick mouse", !ISPANDORA);
  
  // colors

  param_f(crosshair_size, "size:crosshair");
  addsaver(crosshair_color, "color:crosshair");
  
  addsaver(backcolor, "color:background");
  addsaver(forecolor, "color:foreground");
  addsaver(bordcolor, "color:borders");
  addsaver(ringcolor, "color:ring");
  param_f(vid.multiplier_ring, "mring", "mult:ring", 1);
  addsaver(modelcolor, "color:model");
  addsaver(periodcolor, "color:period");
  addsaver(stdgridcolor, "color:stdgrid"); 
  param_f(vid.multiplier_grid, "mgrid", "mult:grid", 1);
  addsaver(dialog::dialogcolor, "color:dialog");
  for(auto& p: colortables)
    savecolortable(p.second, s0+"canvas"+p.first);
  savecolortable(distcolors, "distance");
  savecolortable(minecolors, "mines");
  #if CAP_COMPLEX2
  savecolortable(brownian::colors, "color:brown");
  #endif
  
  for(int i=0; i<motypes; i++)
    addsaver(minf[i].color, "color:monster:" + its(i));
  for(int i=0; i<ittypes; i++)
    addsaver(iinf[i].color, "color:item:" + its(i));
  for(int i=0; i<landtypes; i++)
    addsaver(floorcolors[i], "color:land:" + its(i));
  for(int i=0; i<walltypes; i++)
    addsaver(winf[i].color, "color:wall:" + its(i));

  // modes
    
  addsaverenum(geometry, "mode-geometry");
  addsaver(shmup::on, "mode-shmup", false);
  addsaver(hardcore, "mode-hardcore", false);
  addsaverenum(land_structure, "mode-chaos");
  #if CAP_INV
  addsaver(inv::on, "mode-Orb Strategy");
  #endif
  addsaverenum(variation, "mode-variation", eVariation::bitruncated);
  addsaver(peace::on, "mode-peace");
  addsaver(peace::otherpuzzles, "mode-peace-submode");
  addsaverenum(specialland, "land for special modes");
  
  addsaver(viewdists, "expansion mode");
  param_f(backbrightness, "back", "brightness behind sphere");
  param_b(auto_extend, "expansion_auto_extend")
  -> editable("extend automatically", 'E');

  param_f(vid.ipd, "ipd", "interpupilar-distance", 0.05);
  param_f(vid.lr_eyewidth, "lr", "eyewidth-lr", 0.5);
  param_f(vid.anaglyph_eyewidth, "anaglyph", "eyewidth-anaglyph", 0.1);
  param_f(vid.fov, "fov", "field-of-vision", 90);
  addsaver(vid.desaturate, "desaturate", 0);
  
  param_enum(vid.stereo_mode, "stereo_mode", "stereo-mode", vid.stereo_mode)
    ->editable({{"OFF", ""}, {"anaglyph", ""}, {"side-by-side", ""}
    #if CAP_ODS
    , {"ODS", ""}
    #endif
    }, "stereo mode", 'm');

  param_f(vid.plevel_factor, "plevel_factor", 0.7);

  #if CAP_GP
  addsaver(gp::param.first, "goldberg-x", gp::param.first);
  addsaver(gp::param.second, "goldberg-y", gp::param.second);
  #endif
  
  param_b(nohud, "no-hud", false);
  param_b(nomap, "nomap", false);
  param_b(nofps, "no-fps", false);
  
  #if CAP_IRR
  addsaver(irr::density, "irregular-density", 2);
  addsaver(irr::cellcount, "irregular-cellcount", 150);
  addsaver(irr::quality, "irregular-quality", .2);
  addsaver(irr::place_attempts, "irregular-place", 10);
  addsaver(irr::rearrange_max_attempts, "irregular-rearrange-max", 50);
  addsaver(irr::rearrange_less, "irregular-rearrangeless", 10);
  #endif
  
  param_i(vid.linequality, "line quality", 0);
  
  #if CAP_FILES && CAP_SHOT && CAP_ANIMATIONS
  addsaver(anims::animfile, "animation file format");
  #endif

  #if CAP_RUG
  addsaver(rug::move_on_touch, "rug move on touch");
  #endif
  
  #if CAP_CRYSTAL
  param_f(crystal::compass_probability, "cprob", "compass-probability");
  addsaver(crystal::view_coordinates, "crystal-coordinates");
  #endif
  
#if CAP_TEXTURE  
  param_b(texture::texture_aura, "texture-aura", false);
#endif

  addsaver(vid.use_smart_range, "smart-range", 0);
  param_f(vid.smart_range_detail, "smart-range-detail", 8)
  ->editable(1, 50, 1, "minimum visible cell in pixels", "", 'd');

  param_f(vid.smart_range_detail_3, "smart-range-detail-3", 30)
  ->editable(1, 50, 1, "minimum visible cell in pixels", "", 'd');

  param_b(vid.smart_area_based, "smart-area-based", false);
  param_i(vid.cells_drawn_limit, "limit on cells drawn", 10000);
  param_i(vid.cells_generated_limit, "limit on cells generated", 250);

  param_enum(diskshape, "disk_shape", "disk_shape", dshTiles)
    ->editable({{"distance in tiles", ""}, {"distance in vertices", ""}, {"geometric distance", ""}
    }, "disk shape", 'S')
  ->set_reaction([] { if(game_active) { stop_game(); start_game(); } });

  param_i(req_disksize, "disk_size")
  ->editable(10, 100000, 10, "disk size", "Play on a disk. Enables the special game rules for small bounded spaces (especially relevant for e.g. Minefield and Halloween). The number given is the number of tiles to use; it is not used exactly, actually the smallest disk above this size is used. Set to 0 to disable.", 'd')
  ->set_sets([] { dialog::bound_low(0); })
  ->set_reaction([] { if(game_active) { stop_game(); start_game(); } })
  ->set_extra([] {
    add_edit(diskshape);
    });
  
  #if CAP_SOLV
  addsaver(sn::solrange_xy, "solrange-xy");
  addsaver(sn::solrange_z, "solrange-z");
  #endif
  param_i(slr::shader_iterations, "slr-steps");
  param_f(slr::range_xy, "slr-range-xy");
  param_f(slr::range_z, "slr-range-z");

  param_f(arcm::euclidean_edge_length, "arcm-euclid-length");
  
  #if CAP_ARCM
  addsaver(arcm::current.symbol, "arcm-symbol", "4^5");
  #endif
  addsaverenum(hybrid::underlying, "product-underlying");
  
  for(int i=0; i<isize(ginf); i++) {
    if(ginf[i].flags & qELLIPTIC)
      sightranges[i] = M_PI;
    else if(ginf[i].cclass == gcSphere)
      sightranges[i] = TAU;
    else if(ginf[i].cclass == gcEuclid)
      sightranges[i] = 10;
    else if(ginf[i].cclass == gcSL2)
      sightranges[i] = 4.5;
    else if(ginf[i].cclass == gcHyperbolic && ginf[i].g.gameplay_dimension == 2)
      sightranges[i] = 4.5;
    else
      sightranges[i] = 5;
    sightranges[gArchimedean] = 10;
    if(i < gBinary3) addsaver(sightranges[i], "sight-g" + its(i));
    }
  
  ld bonus = 0;
  ld emul = 1;
  
  param_b(dialog::onscreen_keyboard, "onscreen_keyboard")
  ->editable("onscreen keyboard", 'k');
  
  param_b(context_fog, "coolfog");

  addsaver(sightranges[gBinary3], "sight-binary3", 3.1 + bonus);
  addsaver(sightranges[gCubeTiling], "sight-cubes", 10);
  addsaver(sightranges[gCell120], "sight-120cell", TAU);
  addsaver(sightranges[gECell120], "sight-120cell-elliptic", M_PI);
  addsaver(sightranges[gRhombic3], "sight-rhombic", 10.5 * emul);
  addsaver(sightranges[gBitrunc3], "sight-bitrunc", 12 * emul);
  addsaver(sightranges[gSpace534], "sight-534", 4 + bonus);
  addsaver(sightranges[gSpace435], "sight-435", 3.8 + bonus);

  addsaver(sightranges[gCell5], "sight-5cell", TAU);
  addsaver(sightranges[gCell8], "sight-8cell", TAU);
  addsaver(sightranges[gECell8], "sight-8cell-elliptic", M_PI);
  addsaver(sightranges[gCell16], "sight-16cell", TAU);
  addsaver(sightranges[gECell16], "sight-16cell-elliptic", M_PI);
  addsaver(sightranges[gCell24], "sight-24cell", TAU);
  addsaver(sightranges[gECell24], "sight-24cell-elliptic", M_PI);
  addsaver(sightranges[gCell600], "sight-600cell", TAU);
  addsaver(sightranges[gECell600], "sight-600cell-elliptic", M_PI);
  addsaver(sightranges[gHoroTris], "sight-horotris", 2.9 + bonus);
  addsaver(sightranges[gHoroRec], "sight-hororec", 2.2 + bonus);
  addsaver(sightranges[gHoroHex], "sight-horohex", 2.75 + bonus);

  addsaver(sightranges[gKiteDart3], "sight-kd3", 2.25 + bonus);
  
  addsaver(sightranges[gField435], "sight-field435", 4 + bonus);
  addsaver(sightranges[gField534], "sight-field534", 3.8 + bonus);
  addsaver(sightranges[gSol], "sight-sol");
  addsaver(sightranges[gNil], "sight-nil", 6.5 + bonus);
  addsaver(sightranges[gNIH], "sight-nih");
  addsaver(sightranges[gSolN], "sight-solnih");

  addsaver(sightranges[gCrystal344], "sight-crystal344", 2.5); /* assume raycasting */
  addsaver(sightranges[gSpace344], "sight-344", 4.5);
  addsaver(sightranges[gSpace336], "sight-336", 4);

  param_b(vid.sloppy_3d, "sloppy3d", true);

  param_i(vid.texture_step, "wall-quality", 4);
  
  param_b(smooth_scrolling, "smooth-scrolling", false);
  addsaver(mouseaim_sensitivity, "mouseaim_sensitivity", 0.01);

  param_b(vid.consider_shader_projection, "shader-projection", true);
  
  param_b(tortoise::shading_enabled, "tortoise_shading", true);

  addsaver(bounded_mine_percentage, "bounded_mine_percentage");

  param_b(nisot::geodesic_movement, "solv_geodesic_movement", true);

  addsaver(s2xe::qrings, "s2xe-rings");
  addsaver(rots::underlying_scale, "rots-underlying-scale");
  
  param_b(vid.bubbles_special, "bubbles-special", 1);
  param_b(vid.bubbles_threshold, "bubbles-threshold", 1);
  param_b(vid.bubbles_all, "bubbles-all", 0);

#if CAP_SHMUP  
  multi::initConfig();
#endif

  addsaver(asonov::period_xy, "asonov:period_xy");
  addsaver(asonov::period_z, "asonov:period_z");
  addsaver(nilv::nilperiod[0], "nilperiod_x");
  addsaver(nilv::nilperiod[1], "nilperiod_y");
  addsaver(nilv::nilperiod[2], "nilperiod_z");
  
  param_enum(neon_mode, "neon_mode", "neon_mode", neon_mode)
    ->editable(
        {{"OFF", ""}, {"standard", ""}, {"no boundary mode", ""}, {"neon mode II", ""}, {"illustration mode", ""}}, 
        "neon mode", 'M'
        );

  addsaverenum(neon_nofill, "neon_nofill");
  param_b(noshadow, "noshadow");
  param_b(bright, "bright");
  param_b(cblind, "cblind");
  
  addsaver(berger_limit, "berger_limit");
  
  addsaverenum(centering, "centering");
  
  param_f(camera_speed, "camspd", "camera-speed", 1);
  param_f(camera_rot_speed, "camrot", "camera-rot-speed", 1);
  param_f(third_person_rotation, "third_person_rotation", 0);

  param_f(panini_alpha, "panini_alpha", 0);
  param_f(stereo_alpha, "stereo_alpha", 0);

  callhooks(hooks_configfile);
  
  #if CAP_SHOT
  param_f(levellines, "levellines", 0);
  #endif

#if CAP_CONFIG
  for(auto s: savers) s->reset();
#endif

  param_custom(sightrange_bonus, "sightrange_bonus", menuitem_sightrange_bonus, 'r');
  param_custom(vid.use_smart_range, "sightrange_style", menuitem_sightrange_style, 's');
  
  param_custom(gp::param.first, "Goldberg x", menuitem_change_variation, 0);
  param_custom(gp::param.second, "Goldberg y", menuitem_change_variation, 0);
  param_custom(variation, "variation", menuitem_change_variation, 'v')
  ->help_text = "variation|dual|bitruncated";
  param_custom(geometry, "geometry", menuitem_change_geometry, 0)
  ->help_text = "hyperbolic|spherical|Euclidean";
  
  param_i(stamplen, "stamplen");
  param_f(anims::period, "animperiod");
  }

EX bool inSpecialMode() {
  return !ls::nice_walls() || ineligible_starting_land || !BITRUNCATED || peace::on || 
  #if CAP_TOUR
    tour::on ||
  #endif
    yendor::on || tactic::on || randomPatternsMode ||
    geometry != gNormal || pmodel != mdDisk || pconf.alpha != 1 || pconf.scale != 1 || 
    rug::rugged || vid.monmode != DEFAULT_MONMODE ||
    vid.wallmode != DEFAULT_WALLMODE;
  }

EX bool have_current_settings() {
  int modecount = 0;
  if(inv::on) modecount++;
  if(shmup::on) modecount += 10;
#if CAP_TOUR
  if(tour::on) modecount += 10;
#endif
  if(!ls::nice_walls()) modecount += 10;
  if(!BITRUNCATED) modecount += 10;
  if(peace::on) modecount += 10;
  if(yendor::on) modecount += 10;
  if(tactic::on) modecount += 10;
  if(randomPatternsMode) modecount += 10;
  if(geometry != gNormal) modecount += 10;

  if(modecount > 1)
    return true;
  
  return false;
  }

EX bool have_current_graph_settings() {
  if(pconf.xposition || pconf.yposition || pconf.alpha != 1 || pconf.scale != 1)
    return true;
  if(pmodel != mdDisk || vid.monmode != DEFAULT_MONMODE || vid.wallmode != DEFAULT_WALLMODE)
    return true;
  if(firstland != laIce || multi::players != 1 || rug::rugged)
    return true;
  
  return false;
  }

EX void reset_graph_settings() {
  pmodel = mdDisk; pconf.alpha = 1; pconf.scale = 1;
  pconf.xposition = pconf.yposition = 0;
  #if CAP_RUG
  if(rug::rugged) rug::close();
  #endif

  vid.monmode = DEFAULT_MONMODE;
  vid.wallmode = DEFAULT_WALLMODE;
  }

EX void resetModes(char leave IS('c')) {
  while(game_active || gamestack::pushed()) {
    if(game_active) stop_game();
    if(gamestack::pushed()) gamestack::pop();
    }
  if(shmup::on != (leave == rg::shmup)) stop_game_and_switch_mode(rg::shmup);
  if(inv::on != (leave == rg::inv)) stop_game_and_switch_mode(rg::inv);

  /* we do this twice to make sure that stop_game_and_switch_mode switches to the correct land_structure */
  for(int i=0; i<2; i++) {
    if(leave == rg::chaos && !ls::std_chaos()) stop_game_and_switch_mode(rg::chaos);
    if(leave != rg::chaos && !ls::nice_walls()) stop_game_and_switch_mode(rg::chaos);
    }

  if((!!dual::state) != (leave == rg::dualmode)) stop_game_and_switch_mode(rg::dualmode);

  if(peace::on != (leave == rg::peace)) stop_game_and_switch_mode(rg::peace);
#if CAP_TOUR
  if(tour::on != (leave == rg::tour)) stop_game_and_switch_mode(rg::tour);
#endif
  if(yendor::on != (leave == rg::yendor)) stop_game_and_switch_mode(rg::yendor);
  if(tactic::on != (leave == rg::tactic)) stop_game_and_switch_mode(rg::tactic);
  if(randomPatternsMode != (leave == rg::randpattern)) stop_game_and_switch_mode(rg::randpattern);
  if(multi::players != 1) {
    stop_game_and_switch_mode(); multi::players = 1;
    }
  if(firstland != laIce || specialland != laIce) {
    stop_game();
    firstland = laIce; specialland = laIce; stop_game_and_switch_mode();
    }

  set_geometry(gNormal);
  set_variation(leave == rg::heptagons ? eVariation::pure : eVariation::bitruncated);
  
  start_game();
  }

#if CAP_CONFIG  
EX void resetConfig() {
  dynamicval<int> rx(vid.xres, 0);
  dynamicval<int> ry(vid.yres, 0);
  dynamicval<int> rf(vid.fsize, 0);
  dynamicval<bool> rfs(vid.full, false);
  for(auto s: savers) 
    if(s->name.substr(0,5) != "mode-")
      s->reset();
  }
#endif

#if CAP_CONFIG
EX void saveConfig() {
  DEBB(DF_INIT, ("save config\n"));
  FILE *f = fopen(conffile, "wt");
  if(!f) {
    addMessage(s0 + "Could not open the config file: " + conffile);
    return;
    }
  
  {
  int pt_depth = 0, pt_camera = 0, pt_alpha = 0;
  if(vid.tc_depth > vid.tc_camera) pt_depth++;
  if(vid.tc_depth < vid.tc_camera) pt_camera++;
  if(vid.tc_depth > vid.tc_alpha ) pt_depth++;
  if(vid.tc_depth < vid.tc_alpha ) pt_alpha ++;
  if(vid.tc_alpha > vid.tc_camera) pt_alpha++;
  if(vid.tc_alpha < vid.tc_camera) pt_camera++;
  vid.tc_alpha = pt_alpha;
  vid.tc_camera = pt_camera;
  vid.tc_depth = pt_depth;
  }
  
  for(auto s: savers) if(s->dosave())
    fprintf(f, "%s=%s\n", s->name.c_str(), s->save().c_str());
  
  fclose(f);
#if !ISMOBILE
  addMessage(s0 + "Configuration saved to: " + conffile);
#else
  addMessage(s0 + "Configuration saved");
#endif
  }

void readf(FILE *f, ld& x) {
  double fl = x; 
  hr::ignore(fscanf(f, "%lf", &fl));
  x = fl;
  }

map<string, shared_ptr<supersaver> > allconfigs;

EX void parseline(const string& str) {
  if(str[0] == '#') return;
  for(int i=0; i<isize(str); i++) if(str[i] == '=') {
    string cname = str.substr(0, i);
    if(!allconfigs.count(cname)) {
      printf("Warning: unknown config variable: %s\n", str.c_str());
      return;
      }
    auto sav = allconfigs[cname];
    sav->load(str.substr(i+1));
    return;
    }
  printf("Warning: config line without equality sign: %s\n", str.c_str());
  }

EX void loadNewConfig(FILE *f) {
  for(auto& c: savers) allconfigs[c->name] = c;
  string rd;
  while(true) {
    int c = fgetc(f);
    if(c == -1) break;
    if(c == 10 || c == 13) {
      if(rd != "") parseline(rd);
      rd = "";
      }
    else rd += c;
    }
  allconfigs.clear();
  }

EX void loadConfig() {
 
  DEBB(DF_INIT, ("load config"));
  vid.xres = 9999; vid.yres = 9999; vid.framelimit = 999;
  FILE *f = fopen(conffile, "rt");
  if(f) {
    int err;
    int fs;
    err=fscanf(f, "%d%d%d%d", &vid.xres, &vid.yres, &fs, &vid.fsize);
    if(err != 4) 
      loadNewConfig(f);
    else {
      vid.full = fs;
      #if CAP_LEGACY
      loadOldConfig(f);
      #endif
      }
  
    fclose(f);
    DEBB(DF_INIT, ("Loaded configuration: %s\n", conffile));
    }

  geom3::apply_always3();
  polygonal::solve();
  check_cgi();
  cgi.prepare_basics();
  }
#endif

EX void add_cells_drawn(char c IS('C')) {
  dialog::addSelItem(XLAT("cells drawn"), (noclipped ? its(cells_drawn) + " (" + its(noclipped) + ")" : its(cells_drawn)) + " / " + its(vid.cells_drawn_limit), c);
  dialog::add_action([] () { 
    dialog::editNumber(vid.cells_drawn_limit, 100, 1000000, log(10), 10000, XLAT("limit on cells drawn"), 
      XLAT("This limit exists to protect the engine from freezing when too many cells would be drawn according to the current options.")
      );
    dialog::scaleLog();
    });
  if(WDIM == 3 || vid.use_smart_range == 2) {
    dialog::addSelItem(XLAT("limit generated cells per frame"), its(vid.cells_generated_limit), 'L');
    dialog::add_action([] () { 
      dialog::editNumber(vid.cells_generated_limit, 1, 1000, log(10), 25, XLAT("limit generated cells per frame"), 
        XLAT("In the 3D mode, lowering this value may help if the game lags while exploring new areas.")
        );
      });
    }
  }

string solhelp() {
#if CAP_SOLV
  return XLAT(
    "Solv (aka Sol) is a 3D space where directions work in different ways. It is described by the following metric:\n"
    "ds² = (eᶻdx)² + (e⁻ᶻdy)² + dz²\n\n"
    "You are currently displaying Solv in the perspective projection based on native geodesics. You can control how "
    "the fog effects depends on the geodesic distance, and how far object in X/Y/Z coordinates are rendered."
    );
#else
  return "";
#endif
  }

EX void menuitem_sightrange_bonus(char c) {
  dialog::addSelItem(XLAT("sight range bonus"), its(sightrange_bonus), c);
  dialog::add_action([]{
    dialog::editNumber(sightrange_bonus, -5, allowIncreasedSight() ? 3 : 0, 1, 0, XLAT("sight range"), 
      XLAT("Roughly 42% cells are on the edge of your sight range. Reducing "
      "the sight range makes HyperRogue work faster, but also makes "
      "the game effectively harder."));
    dialog::reaction = doOvergenerate;
    dialog::bound_low(1-getDistLimit());
    dialog::bound_up(allowIncreasedSight() ? euclid ? 99 : gp::dist_2() * 5 : 0);
    });
  }

EX void edit_sightrange() {
  #if CAP_RUG
  USING_NATIVE_GEOMETRY_IN_RUG;
  #endif
  cmode = sm::SIDE;
  gamescreen();
  dialog::init("sight range settings");
  add_edit(vid.use_smart_range);
  if(vid.use_smart_range)
    add_edit(WDIM == 2 ? vid.smart_range_detail : vid.smart_range_detail_3);
  else {
    if(WDIM == 2) {
      add_edit(sightrange_bonus);
      if(GDIM == 3) {
        dialog::addSelItem(XLAT("3D sight range for the fog effect"), fts(sightranges[geometry]), 'r');
        dialog::add_action([] {
          dialog::editNumber(sightranges[geometry], 0, TAU, 0.5, M_PI, XLAT("fog effect"), "");
          });
        }
      }
    if(WDIM == 3) {
      dialog::addSelItem(XLAT("3D sight range"), fts(sightranges[geometry]), 'r');
      dialog::add_action([] {
      dialog::editNumber(sightranges[geometry], 0, TAU, 0.5, M_PI, XLAT("3D sight range"),
        XLAT(
          "Sight range for 3D geometries is specified in the absolute units. This value also affects the fog effect.\n\n"
          "In spherical geometries, the sight range of 2π will let you see things behind you as if they were in front of you, "
          "and the sight range of π (or more) will let you see things on the antipodal point just as if they were close to you.\n\n"
          "In hyperbolic geometries, the number of cells to render depends exponentially on the sight range. More cells to drawn "
          "reduces the performance.\n\n"
          "Sight range affects the gameplay, and monsters act iff they are visible. Monster generation takes this into account."
          )
        );
        });
      }
    }
  #if CAP_SOLV
  if(models::is_perspective(pmodel) && sol) {
    dialog::addSelItem(XLAT("max difference in X/Y coordinates"), fts(sn::solrange_xy), 'x');
    dialog::add_action([] {
      dialog::editNumber(sn::solrange_xy, 0.01, 200, 0.1, 50, XLAT("max difference in X/Y coordinates"), solhelp()), dialog::scaleLog();
      });
    dialog::addSelItem(XLAT("max difference in Z coordinate"), fts(sn::solrange_z), 'z');
    dialog::add_action([] {
      dialog::editNumber(sn::solrange_z, 0, 20, 0.1, 6, XLAT("max difference in Z coordinates"), solhelp());
      });
    }
  #endif
  if(models::is_perspective(pmodel) && sl2) {
    dialog::addSelItem(XLAT("max difference in X/Y coordinates"), fts(slr::range_xy), 'x');
    dialog::add_action([] {
      dialog::editNumber(slr::range_xy, 0, 10, 0.5, 4, XLAT("max difference in X/Y coordinates"), "");
      });
    dialog::addSelItem(XLAT("max difference in Z coordinate"), fts(slr::range_z), 'x');
    dialog::add_action([] {
      dialog::editNumber(slr::range_xy, 0, 10, 0.5, 4, XLAT("max difference in Z coordinate"), "");
      });
    dialog::addSelItem(XLAT("shader_iterations"), its(slr::shader_iterations), 'z');
    dialog::add_action([] {
      dialog::editNumber(slr::shader_iterations, 0, 50, 1, 10, "", "");
      });
    }
  if(vid.use_smart_range && WDIM == 2) {
    dialog::addBoolItem_action(XLAT("area-based range"), vid.smart_area_based, 'a');
    }
  if(vid.use_smart_range == 0 && allowChangeRange() && WDIM == 2) {
    dialog::addSelItem(XLAT("generation range bonus"), its(genrange_bonus), 'o');
    dialog::add_action([] () { genrange_bonus = sightrange_bonus; doOvergenerate(); });
    dialog::addSelItem(XLAT("game range bonus"), its(gamerange_bonus), 's');
    dialog::add_action([] () { gamerange_bonus = sightrange_bonus; doOvergenerate(); });
    }
  if(WDIM == 3 && !vid.use_smart_range) {
    dialog::addBoolItem_action(XLAT("sloppy range checking"), vid.sloppy_3d, 's');
    }
  if(GDIM == 3 && !vid.use_smart_range) {
    dialog::addSelItem(XLAT("limit generation"), fts(extra_generation_distance), 'e');
    dialog::add_action([] {
      dialog::editNumber(extra_generation_distance, 0, 999, 0.5, 999, XLAT("limit generation"), 
        "Cells over this distance will not be generated, but they will be drawn if they are already generated and in the sight range."
        );
      });
    }
  add_cells_drawn('c');
  dialog::display();
  }

EX void menuitem_sightrange_style(char c IS('c')) {
  dialog::addSelItem(XLAT("draw range based on"), 
    vid.use_smart_range == 0 ? XLAT("distance") :
    vid.use_smart_range == 1 ? XLAT("size (no gen)") :
    XLAT("size"),
    c
    );
  dialog::add_action_push([] {
    dialog::init(XLAT("draw range based on"));
    dialog::addBoolItem(XLAT("draw range based on distance"), vid.use_smart_range == 0, 'd');
    dialog::add_action([] () { vid.use_smart_range = 0; popScreen(); edit_sightrange(); });
    if(WDIM == 2 && allowIncreasedSight()) {
      dialog::addBoolItem(XLAT("draw based on size in the projection (no generation)"), vid.use_smart_range == 1, 'n');
      dialog::add_action([] () { vid.use_smart_range = 1; popScreen(); edit_sightrange(); });
      }
    if(allowChangeRange() && allowIncreasedSight()) {
      dialog::addBoolItem(XLAT("draw based on size in the projection (generation)"), vid.use_smart_range == 2, 'g');
      dialog::add_action([] () { vid.use_smart_range = 2; popScreen(); edit_sightrange(); });
      }
    if(!allowChangeRange() || !allowIncreasedSight()) {
      dialog::addItem(XLAT("enable the cheat mode for additional options"), 'X');
      dialog::add_action(enable_cheat);
      }
    dialog::display();
    });
  }

EX void menuitem_sightrange(char c IS('c')) {
  #if CAP_SOLV
  if(pmodel == mdGeodesic && sol)
    dialog::addSelItem(XLAT("sight range settings"), fts(sn::solrange_xy) + "x" + fts(sn::solrange_z), c);
  else
  #endif
  if(vid.use_smart_range)
    dialog::addSelItem(XLAT("sight range settings"), fts(WDIM == 3 ? vid.smart_range_detail_3 : vid.smart_range_detail) + " px", c);
  else if(WDIM == 3)
    dialog::addSelItem(XLAT("sight range settings"), fts(sightranges[geometry]) + "au", c);
  else
    dialog::addSelItem(XLAT("sight range settings"), format("%+d", sightrange_bonus), c);
  dialog::add_action_push(edit_sightrange);
  }

EX void sets_sfx_volume() {
#if CAP_AUDIO
  dialog::dialogflags = sm::NOSCR;
  #if ISANDROID
  dialog::reaction = [] () {
    settingsChanged = true;
    };
  #endif
  dialog::bound_low(0);
  dialog::bound_up(MIX_MAX_VOLUME);
#endif
  }

EX void sets_music_volume() {
#if CAP_AUDIO
  dialog::dialogflags = sm::NOSCR;
  dialog::reaction = [] () {
    #if CAP_SDLAUDIO
    Mix_VolumeMusic(musicvolume);
    #endif
    #if ISANDROID
    settingsChanged = true;
    #endif
    };
  dialog::bound_low(0);
  dialog::bound_up(MIX_MAX_VOLUME);
  #if CAP_SDLAUDIO
  dialog::extra_options = [] {
    dialog::addBoolItem_action(XLAT("play music when out of focus"), music_out_of_focus, 'A');
    };
  #endif
#endif
  }

EX void showSpecialEffects() {
  cmode = vid.xres > vid.yres * 1.4 ? sm::SIDE : sm::MAYDARK;
  gamescreen();
  dialog::init(XLAT("extra graphical effects"));

  dialog::addBoolItem_action(XLAT("particles on attack"), (vid.particles), 'p');
  dialog::addBoolItem_action(XLAT("floating bubbles: special"), vid.bubbles_special, 's');
  dialog::addBoolItem_action(XLAT("floating bubbles: treasure thresholds"), vid.bubbles_threshold, 't');
  dialog::addBoolItem_action(XLAT("floating bubbles: all treasures"), vid.bubbles_all, 'a');
  dialog::addBoolItem_action(XLAT("background particle effects"), (vid.backeffects), 'b');

  dialog::addBreak(50);
  dialog::addBack();
  dialog::display();
  }

EX void show_vector_settings() {
  cmode = vid.xres > vid.yres * 1.4 ? sm::SIDE : sm::MAYDARK;
  gamescreen();
  dialog::init(XLAT("vector settings"));

  dialog::addSelItem(XLAT("line width"), fts(vid.linewidth), 'w');
  dialog::add_action([] {
     dialog::editNumber(vid.linewidth, 0, 10, 0.1, 1, XLAT("line width"), 
       vid.usingGL ? "" : XLAT("Line width setting is only taken into account in OpenGL."));
     });

  dialog::addSelItem(XLAT("line quality"), its(vid.linequality), 'l');
  dialog::add_action([] {
    dialog::editNumber(vid.linequality, -3, 5, 1, 1, XLAT("line quality"), 
      XLAT("Higher numbers make the curved lines smoother, but reduce the performance."));
    });

  dialog::addBoolItem("perfect width", perfect_linewidth == 2, 'p');
  if(perfect_linewidth == 1) 
    dialog::lastItem().value = XLAT("shots only");
  dialog::add_action([] { perfect_linewidth = (1 + perfect_linewidth) % 3; });

  dialog::addBoolItem_action("finer lines at the boundary", vid.fineline, 'O');

  if(vid.fineline) {
    dialog::addSelItem("variable width", fts(precise_width), 'm');
    dialog::add_action([] () {
      dialog::editNumber(precise_width, 0, 2, 0.1, 0.5, 
        XLAT("variable width"), XLAT("lines longer than this value will be split into shorter lines, with width computed separately for each of them.")
        );
      });
    }
  else dialog::addBreak(100);
  
  add_edit(neon_mode);        
  dialog::addBreak(100);
  dialog::addInfo(XLAT("hint: press Alt while testing modes"));
  dialog::addBreak(100);
  dialog::addBoolItem_action(XLAT("disable shadows"), noshadow, 'f');
  dialog::addBoolItem_action(XLAT("bright mode"), bright, 'g');
  dialog::addBoolItem_action(XLAT("colorblind simulation"), cblind, 'h');

  dialog::addBoolItem_action(XLAT("no fill in neon mode"), neon_nofill, 'n');

  dialog::addBreak(50);
  dialog::addBack();
  dialog::display();
  }

EX void showGraphConfig() {
  cmode = vid.xres > vid.yres * 1.4 ? sm::SIDE : sm::MAYDARK;
  gamescreen();

  dialog::init(XLAT("graphics configuration"));
  
#if !ISIOS && !ISWEB
  add_edit(vid.want_fullscreen);
  
  #if !ISANDROID && !ISFAKEMOBILE
  if(vid.want_fullscreen) {
    add_edit(vid.change_fullscr);
    if(vid.change_fullscr)
      add_edit(vid.fullscreen_x), add_edit(vid.fullscreen_y);
    else
      dialog::addBreak(200);
    }
  else {
    add_edit(vid.relative_window_size);
    if(vid.relative_window_size)
      add_edit(vid.window_rel_x), add_edit(vid.window_rel_y);
    else
      add_edit(vid.window_x), add_edit(vid.window_y);
    }
  #endif
#endif

  #if CAP_GLORNOT
  add_edit(vid.wantGL);  
  #endif

  #if !ISIOS && !ISANDROID && !ISFAKEMOBILE
  if(!vid.usingGL) {
    dialog::addBoolItem(XLAT("anti-aliasing"), vid.want_antialias & AA_NOGL, 'O');
    dialog::add_action([] {
      if(!vid.usingGL)
        vid.want_antialias ^= AA_NOGL | AA_FONT;
      });
    }
  else {
    dialog::addSelItem(XLAT("anti-aliasing"), 
      (vid.want_antialias & AA_POLY) ? "polygons" :
      (vid.want_antialias & AA_LINES) ? "lines" :
      (vid.want_antialias & AA_MULTI) ? "multisampling" :
      "NO", 'O');
    dialog::add_action([] {
      if(vid.want_antialias & AA_MULTI)
        vid.want_antialias ^= AA_MULTI;
      else if(vid.want_antialias & AA_POLY)
        vid.want_antialias ^= AA_POLY | AA_LINES | AA_MULTI;
      else if(vid.want_antialias & AA_LINES) 
        vid.want_antialias |= AA_POLY;
      else 
        vid.want_antialias |= AA_LINES;
      });
    }
  #endif

  #if !ISIOS && !ISANDROID && !ISFAKEMOBILE
  if(vid.usingGL) {
    if(vrhr::active())
      dialog::addInfo(XLAT("(vsync disabled in VR)"));
    else
      add_edit(vid.want_vsync);
    }
  else
    dialog::addBreak(100);
  #endif

  if(need_to_apply_screen_settings()) {
    dialog::addItem(XLAT("apply changes"), 'A');
    dialog::add_action(apply_screen_settings);
    dialog::addBreak(100);
    }
  else
    dialog::addBreak(200);  

  add_edit(vid.relative_font);
  if(vid.relative_font) 
    add_edit(vid.fontscale);
  else
    add_edit(vid.abs_fsize);

  dialog::addSelItem(XLAT("vector settings"), XLAT("width") + " " + fts(vid.linewidth), 'w');
  dialog::add_action_push(show_vector_settings);
  
  #if CAP_FRAMELIMIT
  dialog::addSelItem(XLAT("framerate limit"), its(vid.framelimit), 'l');
  if(getcstat == 'l') 
    mouseovers = XLAT("Reduce the framerate limit to conserve CPU energy");
  #endif
  
  dialog::addSelItem(XLAT("scrolling speed"), fts(vid.sspeed), 'a');

  dialog::addSelItem(XLAT("camera movement speed"), fts(camera_speed), 'c');
  dialog::add_action([] { 
    dialog::editNumber(camera_speed, -10, 10, 0.1, 1, XLAT("camera movement speed"), 
      "This affects:\n\nin 2D: scrolling with arrow keys and Wheel Up\n\nin 3D: camera movement with Home/End."
      );
    });
  dialog::addSelItem(XLAT("camera rotation speed"), fts(camera_rot_speed), 'r');
  dialog::add_action([] { 
    dialog::editNumber(camera_rot_speed, -10, 10, 0.1, 1, XLAT("camera rotation speed"), 
      "This affects view rotation with Page Up/Down, and in 3D, camera rotation with arrow keys or mouse."
      );
    });
    
  dialog::addSelItem(XLAT("movement animation speed"), fts(vid.mspeed), 'm');
  
  dialog::addSelItem(XLAT("idle animation speed"), fts(vid.ispeed), 'i');
  dialog::add_action([] {
    dialog::editNumber(vid.ispeed, 0, 4, 0.1, 1, 
      XLAT("idle animation speed"),
      "0 = disable\n\nThis affects non-movement animations such as orb effects, item rotation, and more."
      );
    });

  dialog::addBoolItem_action(XLAT("flashing effects"), (vid.flasheffects), 'h');
  if(getcstat == 'h') 
    mouseovers = XLAT("Disable if you are photosensitive. Replaces flashing effects such as Orb of Storms lightning with slow, adjustable animations.");

  dialog::addItem(XLAT("extra graphical effects"), 'u');

  dialog::addBreak(50);
  dialog::addBack();
  dialog::display();

  keyhandler = [] (int sym, int uni) {
    dialog::handleNavigation(sym, uni);
  
    char xuni = uni | 96;
  
    if((uni >= 32 && uni < 64) || uni == 'L' || uni == 'C') xuni = uni;
    
    if(xuni == 'u') pushScreen(showSpecialEffects);

    else if(xuni == 'a') dialog::editNumber(vid.sspeed, -5, 5, 1, 0, 
      XLAT("scrolling speed"),
      XLAT("+5 = center instantly, -5 = do not center the map")
      + "\n\n" +
      XLAT("press Space or Home to center on the PC"));
  
    else if(xuni == 'm') dialog::editNumber(vid.mspeed, -5, 5, 1, 0, 
      XLAT("movement animation speed"),
      XLAT("+5 = move instantly"));
  
  #if CAP_FRAMELIMIT    
    else if(xuni == 'l') {
      dialog::editNumber(vid.framelimit, 5, 300, 10, 300, XLAT("framerate limit"), "");
      dialog::bound_low(5);
      }
  #endif
      
    else if(xuni =='p') 
      vid.backeffects = !vid.backeffects;
      
    else if(doexiton(sym, uni)) popScreen();
    };
  }
  
EX void edit_whatever(char type, int index) {
  if(type == 'f') {
    dialog::editNumber(whatever[index], -10, 10, 1, 0, XLAT("whatever"), 
      "f:" + its(index));
    }
  else {
    dialog::editNumber(whateveri[index], -10, 10, 1, 0, XLAT("whatever"), 
      "i:" + its(index));
    }
  dialog::extra_options = [type, index] {
    dialog::addItem(XLAT("integer"), 'X');
    dialog::add_action( [index] { popScreen(); edit_whatever('i', index); });
    dialog::addItem(XLAT("float"), 'Y');
    dialog::add_action( [index] { popScreen(); edit_whatever('f', index); });
    for(int x=0; x<8; x++) {
      dialog::addSelItem(its(x), type == 'i' ? its(whateveri[x]) : fts(whatever[x]), 'A' + x);
      dialog::add_action([type,x] { popScreen(); edit_whatever(type, x); });
      }
    };
  }

EX void configureOther() {
  cmode = sm::SIDE | sm::MAYDARK;
  gamescreen();

  dialog::init(XLAT("other settings"));

#if ISSTEAM
  dialog::addBoolItem(XLAT("send scores to Steam leaderboards"), (vid.steamscore&1), 'x');
  dialog::add_action([] {vid.steamscore = vid.steamscore^1; });
#endif

  dialog::addBoolItem_action(XLAT("skip the start menu"), vid.skipstart, 'm');
  
  dialog::addItem(XLAT("memory configuration"), 'y');
  dialog::add_action_push(show_memory_menu);

  // dialog::addBoolItem_action(XLAT("forget faraway cells"), memory_saving_mode, 'y');
  
#if CAP_AUDIO
  add_edit(musicvolume);
  add_edit(effvolume);
#endif

  menuitem_sightrange('r');

  add_edit(vid.faraway_highlight);
  add_edit(vid.faraway_highlight_color);
  
#ifdef WHATEVER
  dialog::addSelItem(XLAT("whatever"), fts(whatever[0]), 'j');
  dialog::add_action([] { edit_whatever('f', 0); });
#endif

  add_edit(savefile_selection);
  
  dialog::addBreak(50);
  dialog::addBack();
  
  dialog::display();
  }

EX void configureInterface() {
  cmode = sm::SIDE | sm::MAYDARK;
  gamescreen();
  dialog::init(XLAT("interface"));

#if CAP_TRANS
  dialog::addSelItem(XLAT("language"), XLAT("EN"), 'l');
  dialog::add_action_push(selectLanguageScreen);
#endif

  dialog::addSelItem(XLAT("player character"), numplayers() > 1 ? "" : csname(vid.cs), 'g');
  dialog::add_action_push(showCustomizeChar);
  if(getcstat == 'g') mouseovers = XLAT("Affects looks and grammar");

  dialog::addSelItem(XLAT("message flash time"), its(vid.flashtime), 't');
  dialog::add_action([] {
    dialog::editNumber(vid.flashtime, 0, 64, 1, 8, XLAT("message flash time"),
      XLAT("How long should the messages stay on the screen."));
    dialog::bound_low(0);
    });

  dialog::addSelItem(XLAT("limit messages shown"), its(vid.msglimit), 'z');
  dialog::add_action([] {
    dialog::editNumber(vid.msglimit, 0, 64, 1, 5, XLAT("limit messages shown"),
      XLAT("Maximum number of messages on screen."));
    dialog::bound_low(0);
    });
  
  add_edit(vid.msgleft);
  
  add_edit(glyphsortorder);
  add_edit(vid.graphglyph);
  add_edit(less_in_landscape);
  add_edit(less_in_portrait);

  add_edit(display_yasc_codes);
  add_edit(vid.orbmode);

  dialog::addSelItem(XLAT("draw crosshair"), crosshair_size > 0 ? fts(crosshair_size) : ONOFF(false), 'x');
  dialog::add_action([] () { 
    dialog::editNumber(crosshair_size, 0, 100, 1, 10, XLAT("crosshair size"), XLAT(
      "Display a targetting reticle in the center of the screen. Might be useful when exploring 3D modes, "
      "as it precisely shows the direction we are going. However, the option is available in all modes."
      ));
    dialog::bound_low(0);
    dialog::extra_options = [] {
      dialog::addColorItem(XLAT("crosshair color"), crosshair_color, 'X');
      dialog::add_action([] { dialog::openColorDialog(crosshair_color); });
      };
    });

  add_edit(menu_darkening);
  add_edit(centered_menus);
  add_edit(startanims::enabled);
   
  dialog::addBreak(50);
  dialog::addBack();
  
  dialog::display();
  }

#if CAP_SDLJOY
EX void showJoyConfig() {
  cmode = sm::SIDE | sm::MAYDARK;
  gamescreen();

  dialog::init(XLAT("joystick configuration"));
  
  dialog::addSelItem(XLAT("first joystick position (movement)"), its(joyx)+","+its(joyy), 0);
  dialog::addSelItem(XLAT("second joystick position (panning)"), its(panjoyx)+","+its(panjoyy), 0);
  
  dialog::addSelItem(XLAT("joystick mode"), autojoy ? XLAT("automatic") : XLAT("manual"), 'p');
  if(getcstat == 'p') {
    if(autojoy) 
      mouseovers = XLAT("joystick mode: automatic (release the joystick to move)");
    if(!autojoy) 
      mouseovers = XLAT("joystick mode: manual (press a button to move)");
    }
    
  dialog::addSelItem(XLAT("first joystick: movement threshold"), its(vid.joyvalue), 'a');
  dialog::addSelItem(XLAT("first joystick: execute movement threshold"), its(vid.joyvalue2), 'b');
  dialog::addSelItem(XLAT("second joystick: pan threshold"), its(vid.joypanthreshold), 'c');
  dialog::addSelItem(XLAT("second joystick: panning speed"), fts(vid.joypanspeed * 1000), 'd');
  dialog::addSelItem(XLAT("smoothen"), its(vid.joysmooth) + " ms", 'e');

  dialog::addBreak(50);
  dialog::addBack();
  dialog::display();
  
  keyhandler = [] (int sym, int uni) {
    dialog::handleNavigation(sym, uni);
    if(uni == 'p') autojoy = !autojoy;
    else if(uni == 'a') {
      dialog::editNumber(vid.joyvalue, 0, 32768, 100, 4800, XLAT("first joystick: movement threshold"), "");
      dialog::bound_low(0);
      }
    else if(uni == 'b') {
      dialog::editNumber(vid.joyvalue2, 0, 32768, 100, 5600, XLAT("first joystick: execute movement threshold"), "");
      dialog::bound_low(0);
      }
    else if(uni == 'c') {
      dialog::editNumber(vid.joypanthreshold, 0, 32768, 100, 2500, XLAT("second joystick: pan threshold"), "");
      dialog::bound_low(0);
      }
    else if(uni == 'd')
      dialog::editNumber(vid.joypanspeed, 0, 1e-2, 1e-5, 1e-4, XLAT("second joystick: panning speed"), "");
    else if(uni == 'e')
      dialog::editNumber(vid.joypanspeed, 0, 2000, 20, 200, XLAT("smoothen"), "large values help if the joystick is imprecise");
  
    else if(doexiton(sym, uni)) popScreen();
    };
  }
#endif

EX void projectionDialog() {
  vid.tc_alpha = ticks;
  dialog::editNumber(vpconf.alpha, -5, 5, .1, 1,
    XLAT("projection distance"),
    XLAT("HyperRogue uses the Minkowski hyperboloid model internally. "
    "Klein and Poincaré models can be obtained by perspective, "
    "and the Gans model is obtained by orthogonal projection. "
    "See also the conformal mode (in the special modes menu) "
    "for more models."));
  dialog::extra_options = [] () {
    dialog::addBreak(100);
    if(GDIM == 2) dialog::addHelp(XLAT(
      "If we are viewing an equidistant g absolute units below a plane, "
      "from a point c absolute units above the plane, this corresponds "
      "to viewing a Minkowski hyperboloid from a point "
      "tanh(g)/tanh(c) units below the center. This in turn corresponds to "
      "the Poincaré model for g=c, and Klein-Beltrami model for g=0."));
    dialog::addSelItem(sphere ? "stereographic" : "Poincaré model", "1", 'P');
    dialog::add_action([] () { *dialog::ne.editwhat = 1; vpconf.scale = 1; dialog::ne.s = "1"; });
    dialog::addSelItem(sphere ? "gnomonic" : "Klein model", "0", 'K');
    dialog::add_action([] () { *dialog::ne.editwhat = 0; vpconf.scale = 1; dialog::ne.s = "0"; });
    if(hyperbolic) {
      dialog::addSelItem("inverted Poincaré model", "-1", 'I');
      dialog::add_action([] () { *dialog::ne.editwhat = -1; vpconf.scale = 1; dialog::ne.s = "-1"; });
      }
    dialog::addItem(sphere ? "orthographic" : "Gans model", 'O');
    dialog::add_action([] () { vpconf.alpha = vpconf.scale = 999; dialog::ne.s = dialog::disp(vpconf.alpha); });
    dialog::addItem(sphere ? "towards orthographic" : "towards Gans model", 'T');
    dialog::add_action([] () { double d = 1.1; vpconf.alpha *= d; vpconf.scale *= d; dialog::ne.s = dialog::disp(vpconf.alpha); });
    };
  }

EX void menuitem_projection_distance(char key) {
  dialog::addSelItem(XLAT("projection distance"), fts(vpconf.alpha) + " (" + current_proj_name() + ")", key);
  dialog::add_action(projectionDialog);
  }

EX void explain_detail() {
  dialog::addHelp(XLAT(
    "Objects at distance less than %1 absolute units "
    "from the center will be displayed with high "
    "detail, and at distance at least %2 with low detail.",
    fts(vid.highdetail), fts(vid.middetail)
    ));
  }

EX ld max_fov_angle() {
  auto& p = panini_alpha ? panini_alpha : stereo_alpha;
  if(p >= 1 || p <= -1) return 360;
  return acos(-p) * 2 / degree;
  }

EX void add_edit_fov(char key IS('f'), bool pop IS(false)) {

  string sfov = fts(vid.fov) + "°";
  if(panini_alpha || stereo_alpha) {
    sfov += " / " + fts(max_fov_angle()) + "°";
    }
  dialog::addSelItem(XLAT("field of view"), sfov, key);
  dialog::add_action([=] {
    if(pop) popScreen();
    dialog::editNumber(vid.fov, 1, max_fov_angle(), 1, 90, "field of view", 
      XLAT(
        "Horizontal field of view, in angles. "
        "This affects the Hypersian Rug mode (even when stereo is OFF) "
        "and non-disk models.") + "\n\n" +
      XLAT(
        "Must be less than %1°. Panini projection can be used to get higher values.",
        fts(max_fov_angle())
        )
        );
    dialog::bound_low(1e-8);
    dialog::bound_up(max_fov_angle() - 0.01);
    string quick = 
      XLAT(
        "HyperRogue uses "
        "a quick implementation, so parameter values too close to 1 may "
        "be buggy (outside of raycasting); try e.g. 0.9 instead."
        );
    dialog::extra_options = [quick] {
      dialog::addSelItem(XLAT("Panini projection"), fts(panini_alpha), 'P');
      dialog::add_action([quick] {
        popScreen();
        dialog::editNumber(panini_alpha, 0, 1, 0.1, 0, "Panini parameter", 
          XLAT(
            "The Panini projection is an alternative perspective projection "
            "which allows very wide field-of-view values.\n\n") + quick
            );
        #if CAP_GL
        dialog::reaction = reset_all_shaders;
        #endif
        dialog::extra_options = [] {
          add_edit_fov('F', true);
          };
        });
      dialog::addSelItem(XLAT("spherical perspective projection"), fts(stereo_alpha), 'S');
      dialog::add_action([quick] {
        popScreen();
        dialog::editNumber(stereo_alpha, 0, 1, 0.1, 0, "spherical perspective parameter", 
          XLAT(
            "Set to 1 to get stereographic projection, "
            "which allows very wide field-of-view values.\n\n") + quick
            );
        #if CAP_GL
        dialog::reaction = reset_all_shaders;
        #endif
        dialog::extra_options = [] {
          add_edit_fov('F', true);
          };
        });
      };
    });
  }

bool supported_ods() {
  if(!CAP_ODS) return false;
  return rug::rugged || (hyperbolic && GDIM == 3);
  }

EX void showStereo() {
  cmode = sm::SIDE | sm::MAYDARK;
  gamescreen();
  dialog::init(XLAT("stereo vision config"));

  add_edit(vid.stereo_mode);
  
  dialog::addSelItem(XLAT("pupillary distance"), fts(vid.ipd), 'e');
  
  switch(vid.stereo_mode) {
    case sAnaglyph:
      dialog::addSelItem(XLAT("distance between images"), fts(vid.anaglyph_eyewidth), 'd');
      break;
    case sLR:
      dialog::addSelItem(XLAT("distance between images"), fts(vid.lr_eyewidth), 'd');
      break;
    default:
      dialog::addBreak(100);
      break;
    }
  
  dialog::addSelItem(XLAT("desaturate colors"), its(vid.desaturate)+"%", 'c');
  dialog::add_action([] {
    dialog::editNumber(vid.desaturate, 0, 100, 10, 0, XLAT("desaturate colors"),
      XLAT("Make the game colors less saturated. This is useful in the anaglyph mode.")
      );    
    });
  
  add_edit_fov('f');

  dialog::addBack();
  dialog::display();

  keyhandler = [] (int sym, int uni) {
    dialog::handleNavigation(sym, uni);
    
    string help3 = XLAT(
      "This allows you to view the world of HyperRogue in three dimensions. "
      "Best used with the Hypersian Rug mode. When used in the disk model, "
      "this lets you look at the Minkowski hyperboloid (which means the "
      "depth of terrain features is actually reversed). It also works with non-disk models, "
      "from the conformal menu."
       ) + " " + XLAT(
       "Currently, red-cyan anaglyph glasses and mobile VR googles are supported."
        ) + "\n\n";

    if(uni == 'm') {
      vid.stereo_mode = eStereo((1 + vid.stereo_mode) % 4);
      if(vid.stereo_mode == sODS && !supported_ods()) vid.stereo_mode = sOFF;
      }
    
    else if(uni == 'e') 
      dialog::editNumber(vid.ipd, -10, 10, 0.01, 0, XLAT("pupillary distance"),
        help3 + 
        XLAT("The distance between your eyes in the represented 3D object. This is given in absolute units.")
        ), dialog::scaleSinh100();
      
    else if(uni == 'd' && vid.stereo_mode == sAnaglyph)
      dialog::editNumber(vid.anaglyph_eyewidth, -1, 1, 0.01, 0, XLAT("distance between images"),
        help3 +
        XLAT("The distance between your eyes. 1 is the width of the screen."));

    else if(uni == 'd' && vid.stereo_mode == sLR)
      dialog::editNumber(vid.lr_eyewidth, -1, 1, 0.01, 0, XLAT("distance between images"),
        help3 +
        XLAT("The distance between your eyes. 1 is the width of the screen."));
      
    else if(doexiton(sym, uni)) popScreen();
    };
  }

EX void add_edit_wall_quality(char c) {
  dialog::addSelItem(XLAT("wall quality"), its(vid.texture_step), c);
  dialog::add_action([] {
    dialog::editNumber(vid.texture_step, 1, 16, 1, 1, XLAT("wall quality"), 
      XLAT(
      "Controls the number of triangles used for wall surfaces. "
      "Higher numbers reduce the performance. "
      "This has a strong effect when the walls are curved indeed "
      "(floors in 2D geometries, honeycombs based on horospheres, and projections other than native perspective), "
      "but otherwise, usually it can be set to 1 without significant adverse effects other "
      "than slightly incorrect texturing."
      )
      );
    dialog::bound_low(1);
    dialog::bound_up(128);
    dialog::reaction = [] {
      #if MAXMDIM >= 4
      if(floor_textures) {
        delete floor_textures;
        floor_textures = NULL;
        }
      #endif
      };
    });
  }

EX void edit_levellines(char c) {
  if(levellines)
    dialog::addSelItem(XLAT("level lines"), fts(levellines), c);
  else
    dialog::addBoolItem(XLAT("level lines"), false, c);
  dialog::add_action([] {
    dialog::editNumber(levellines, 0, 100, 0.5, 0, XLAT("level lines"), 
      XLAT(
        "This feature superimposes level lines on the rendered screen. These lines depend on the Z coordinate. In 3D hyperbolic the Z coordinate is taken from the Klein model. "
        "Level lines can be used to observe the curvature: circles correspond to positive curvature, while hyperbolas correspond to negative. See e.g. the Hypersian Rug mode.")
      );
    dialog::reaction = ray::reset_raycaster;
    dialog::extra_options = [] {
      dialog::addBoolItem(XLAT("disable textures"), disable_texture, 'T');
      dialog::add_action([] { ray::reset_raycaster(); disable_texture = !disable_texture; });
      dialog::addItem(XLAT("disable level lines"), 'D');
      dialog::add_action([] { ray::reset_raycaster(); levellines = 0; popScreen(); });
      };
    dialog::bound_low(0);
    });
  }

EX geom3::eSpatialEmbedding shown_spatial_embedding() {
  if(GDIM == 2) return geom3::seNone;
  return geom3::spatial_embedding;
  }

EX bool in_tpp() { return pmodel == mdDisk && pconf.camera_angle; }

EX void display_embedded_errors() {
  if(meuclid && among(geom3::spatial_embedding, geom3::seNil, geom3::seSol, geom3::seSolN, geom3::seNIH) && (!among(geometry, gEuclid, gEuclidSquare) || !PURE))
    dialog::addInfo(XLAT("error: works only in PURE Euclidean regular square or hex tiling"), 0xC00000);
  }

EX void show_spatial_embedding() {
  cmode = sm::SIDE | sm::MAYDARK | sm::CENTER | sm::PANNING | sm::SHOWCURSOR;
  gamescreen();
  dialog::init(XLAT("3D styles"));
  auto emb = shown_spatial_embedding();
  add_edit(geom3::auto_configure);

  dialog::addBreak(100);

  auto &seo = geom3::spatial_embedding_options;

  for(int i=0; i<isize(seo); i++) {
    auto se = geom3::eSpatialEmbedding(i);
    dialog::addBoolItem(XLAT(seo[i].first), emb == i, 'a' + i);
    dialog::add_action([se, emb] {
      if(GDIM == 3) { if(geom3::auto_configure) geom3::switch_fpp(); else geom3::switch_always3(); }
      if(in_tpp()) geom3::switch_tpp();
      if(se != geom3::seNone) {
        geom3::spatial_embedding = se;
        if(geom3::auto_configure) geom3::switch_fpp(); else geom3::switch_always3();
        delete_sky();
        resetGL();
        }
      });
    }

  dialog::addBreak(100);
  dialog::addHelp(XLAT(seo[emb].second));
  display_embedded_errors();
  dialog::addBreak(100);

  if(geom3::auto_configure) {
    if(emb == geom3::seNone) {
      dialog::addBoolItem(XLAT("third-person perspective"), in_tpp(), 'T');
      dialog::add_action(geom3::switch_tpp);
      dialog::addBreak(100);
      }
    else {
      if(geom3::supports_flat()) add_edit(geom3::flat_embedding);
      else dialog::addBreak(100);
      if(geom3::supports_invert()) add_edit(geom3::inverted_embedding);
      else dialog::addBreak(100);
      }
    }

  dialog::addBreak(100);
  dialog::addBack();

  dialog::display();
  }

EX void show3D() {
  cmode = sm::SIDE | sm::MAYDARK;
  gamescreen();
  dialog::init(XLAT("3D configuration"));

#if MAXMDIM >=4
  if(WDIM == 2) {
    dialog::addSelItem("3D style", geom3::spatial_embedding_options[shown_spatial_embedding()].first, 'E');
    dialog::add_action_push(show_spatial_embedding);

    display_embedded_errors();
    dialog::addBreak(50);
    }
#endif

  if(vid.use_smart_range == 0 && GDIM == 2) {
    add_edit(vid.highdetail);
    add_edit(vid.middetail);
    dialog::addBreak(50);
    }
  
  if(WDIM == 2) {
    if(geom3::euc_in_noniso()) add_edit(geom3::euclid_embed_scale);
    add_edit(embedded_shift_method_choice);
    add_edit(vid.camera);
    if(GDIM == 3)
      add_edit(vid.eye);

    add_edit(vid.depth);
      
    if(GDIM == 2) {
      dialog::addSelItem(XLAT("Projection at the ground level"), fts(pconf.alpha), 'p');
      dialog::add_action(projectionDialog);
      }
    else if(!in_perspective())
      dialog::addSelItem(XLAT("projection distance"), fts(pconf.alpha), 'p');
    
    dialog::addBreak(50);
    add_edit(vid.wall_height);
    
    add_edit(vid.rock_wall_ratio);
    add_edit(vid.human_wall_ratio);
    add_edit(vid.lake_top);
    add_edit(vid.lake_bottom);
    if(scale_used())
      add_edit(vid.creature_scale);
    }
  else {
    add_edit(vid.creature_scale);
    add_edit(vid.height_width);
    menuitem_sightrange('s');
    }

  dialog::addBreak(50);
  add_edit(vid.yshift);
  if(GDIM == 3) {
    dialog::addSelItem(XLAT("mouse aiming sensitivity"), fts(mouseaim_sensitivity), 'a');
    dialog::add_action([] () { 
      dialog::editNumber(mouseaim_sensitivity, -1, 1, 0.002, 0.01, XLAT("mouse aiming sensitivity"), "set to 0 to disable");
      });
    }
  if(true) {
    dialog::addSelItem(XLAT("camera rotation"), fts(vpconf.camera_angle), 'S');
    dialog::add_action([] {
      dialog::editNumber(vpconf.camera_angle, -180, 180, 5, 0, XLAT("camera rotation"), 
        XLAT("Rotate the camera. Can be used to obtain a first person perspective, "
        "or third person perspective when combined with Y shift.")
        );
      dialog::extra_options = [] {
        dialog::addBoolItem(XLAT("render behind the camera"), vpconf.back_and_front, 'R');
        dialog::add_action([] { vpconf.back_and_front = !vpconf.back_and_front; });
        };
      });
    }
  if(GDIM == 2) {
    dialog::addSelItem(XLAT("fixed facing"), vid.fixed_facing ? fts(vid.fixed_facing_dir) : XLAT("OFF"), 'f');
    dialog::add_action([] () { vid.fixed_facing = !vid.fixed_facing; 
      if(vid.fixed_facing) {
        dialog::editNumber(vid.fixed_facing_dir, 0, 360, 15, 90, "", "");
        dialog::dialogflags |= sm::CENTER;
        }
      });
    }

  if(mproduct || embedded_plane)
    dialog::addBoolItem_action(XLAT("fixed Y/Z rotation"), vid.fixed_yz, 'Z');

  if(WDIM == 2 && GDIM == 3) {
    add_edit(vid.pseudohedral);
    // add_edit(vid.depth_bonus);
    }

  if(true) {
    dialog::addBreak(50);
    dialog::addSelItem(XLAT("projection"), current_proj_name(), 'M');
    dialog::add_action_push(models::model_menu);  
    }
  #if MAXMDIM >= 4
  if(GDIM == 3) add_edit_fov('f');
  if(GDIM == 3) {
    dialog::addSelItem(XLAT("radar size"), fts(vid.radarsize), 'r');
    dialog::add_action([] () {
      dialog::editNumber(vid.radarsize, 0, 360, 15, 90, "", XLAT("set to 0 to disable"));
      dialog::extra_options = [] () { draw_radar(true); };
      });
    }
  
  if(WDIM == 3 && sphere && stretch::factor) {
    dialog::addItem(XLAT("Berger sphere limit"), berger_limit);
    dialog::add_action([] () {
      dialog::editNumber(berger_limit, 0, 10, 1, 2, "", 
        XLAT("Primitive-based rendering of Berger sphere is currently very slow and low quality. "
          "Here you can choose how many images to draw.")
        );
      });
    }
  
  #if CAP_RAY
  if(GDIM == 3) {
    dialog::addItem(XLAT("configure raycasting"), 'A');
    dialog::add_action_push(ray::configure);
    }
  #endif
  
  edit_levellines('L');
  
  if(WDIM == 3 || (GDIM == 3 && meuclid)) {
    dialog::addSelItem(XLAT("radar range"), fts(vid.radarrange), 'R');
    dialog::add_action([] () {
      dialog::editNumber(vid.radarrange, 0, 10, 0.5, 2, "", XLAT(""));
      dialog::extra_options = [] () { draw_radar(true); };
      });
    }
  if(GDIM == 3) add_edit_wall_quality('W');
  #endif
  
  #if CAP_RUG
  if(rug::rugged) {
    dialog::addBoolItem_action(XLAT("3D monsters/walls on the surface"), rug::spatial_rug, 'S');
    }
  #endif

  if(0);
  #if CAP_RUG
  else if(rug::rugged && !rug::spatial_rug)
    dialog::addBreak(100);
  #endif
  else if(GDIM == 2 && non_spatial_model())
    dialog::addInfo(XLAT("no 3D effects available in this projection"), 0xC00000);
  else if(GDIM == 2 && !spatial_graphics)
    dialog::addInfo(XLAT("set 3D monsters or walls in basic config first"));
  else if(geom3::invalid != "")
    dialog::addInfo(XLAT("error: ")+geom3::invalid, 0xC00000);
  else
    dialog::addInfo(XLAT("parameters set correctly"));
  dialog::addBreak(50);
  dialog::addItem(XLAT("stereo vision config"), 'e');
  dialog::add_action_push(showStereo);
  
  #if CAP_VR
  dialog::addBoolItem(XLAT("VR settings"), vrhr::active(), 'v');
  dialog::add_action_push(vrhr::show_vr_settings);
  #endif

  dialog::addBack();
  dialog::display();
  }

EX int config3 = addHook(hooks_configfile, 100, [] {
  param_f(vid.eye, "eyelevel", 0)
    ->editable(-5, 5, .1, "eye level", "", 'E')
    ->set_extra([] {
      dialog::dialogflags |= sm::CENTER;
      vid.tc_depth = ticks;
    
      dialog::addHelp(XLAT("In the FPP mode, the camera will be set at this altitude (before applying shifts)."));

      dialog::addBoolItem(XLAT("auto-adjust to eyes on the player model"), vid.auto_eye, 'O');
      dialog::reaction = [] { vid.auto_eye = false; };
      dialog::add_action([] () {
        vid.auto_eye = !vid.auto_eye;
        geom3::do_auto_eye();
        });
      });
  
  addsaver(vid.auto_eye, "auto-eyelevel", false);

  param_f(vid.creature_scale, "creature_scale", "3d-creaturescale", 1)
    ->editable(0, 1, .1, "Creature scale", "", 'C');
  param_f(vid.height_width, "heiwi", "3d-heightwidth", 1.5)
    ->editable(0, 1, .1, "Height to width", "", 'h');
  param_f(vid.yshift, "yshift", "Y shift", 0)
    ->editable(0, 1, .1, "Y shift", "Don't center on the player character.", 'y')
    ->set_extra([] {
      if(WDIM == 3 && pmodel == mdPerspective) 
        dialog::addBoolItem_action(XLAT("reduce if walls on the way"), vid.use_wall_radar, 'R');
      });
  addsaver(vid.use_wall_radar, "wallradar", true);
  addsaver(vid.fixed_facing, "fixed facing", 0);
  addsaver(vid.fixed_facing_dir, "fixed facing dir", 90);
  param_b(vid.fixed_yz, "fixed YZ", true);
  param_b(frustum_culling, "frustum_culling");
  param_b(numerical_minefield, "numerical_minefield")
  ->editable("display mine counts numerically", 'n');
  param_b(dont_display_minecount, "dont_display_minecount");
  param_b(draw_sky, "draw sky", true);
  param_f(linepatterns::parallel_count, "parallel_count")
    ->editable(0, 24, 1, "number of parallels drawn", "", 'n');
  param_f(linepatterns::parallel_max, "parallel_max")
    ->editable(0, TAU, 15*degree, "last parallel drawn", "", 'n');
  param_f(vid.depth_bonus, "depth_bonus", 0)
    ->editable(-5, 5, .1, "depth bonus in pseudohedral", "", 'b');
  param_b(vid.pseudohedral, "pseudohedral", false)
    ->editable("make the tiles flat", 'p');
  param_f(vid.depth, "depth", "3D depth", 1)
    ->editable(0, 5, .1, "Ground level below the plane", "", 'd')
    ->set_extra([] {
        vid.tc_depth = ticks;
        help = XLAT(
          "Ground level is actually an equidistant surface, "
          "%1 absolute units below the plane P. "
          "Theoretically, this value affects the world -- "
          "for example, eagles could fly %2 times faster by "
          "flying above the ground level, on the plane P -- "
          "but the actual game mechanics are not affected. ", fts(vid.depth), fts(cosh(vid.depth)));        
        if(GDIM == 2)
          help += XLAT(
            "(Distances reported by the vector graphics editor "
            "are not about points on the ground level, but "
            "about the matching points on the plane P -- "
            "divide them by the factor above to get actual "
            "distances.)"
            );
        if(GDIM == 3 && pmodel == mdPerspective && !euclid) {
          ld current_camera_level = hdist0(tC0(current_display->radar_transform));
          help += "\n\n";
          if(abs(current_camera_level) < 1e-6)
            help += XLAT(
              "The camera is currently exactly on the plane P. "
              "The horizon is seen as a straight line."
              );
          else help += XLAT(
              "The camera is currently %1 units above the plane P. "
              "This makes you see the floor level as in general perspective projection "
              "with parameter %2.", fts(current_camera_level), fts(tan_auto(vid.depth) / tan_auto(current_camera_level)));
          }
        dialog::addHelp(help);
        });
  param_f(vid.camera, "camera", "3D camera level", 1)
    ->editable(0, 5, .1, "", "", 'c')
    ->modif([] (float_setting* x) { x->menu_item_name = (GDIM == 2 ? "Camera level above the plane" : "Z shift"); })
    ->set_extra([] {    
       vid.tc_camera = ticks;
       if(GDIM == 2)
       dialog::addHelp(XLAT(
         "Camera is placed %1 absolute units above a plane P in a three-dimensional "
         "world. Ground level is actually an equidistant surface, %2 absolute units "
         "below the plane P. The plane P (as well as the ground level or any "
         "other equidistant surface below it) is viewed at an angle of %3 "
         "(the tangent of the angle between the point in "
         "the center of your vision and a faraway location is 1/cosh(c) = %4).",
         fts(vid.camera),
         fts(vid.depth),
         fts(atan(1/cosh(vid.camera))*2/degree),
         fts(1/cosh(vid.camera))));
       if(GDIM == 3) 
         dialog::addHelp(XLAT("Look from behind."));
       if(GDIM == 3 && pmodel == mdPerspective) 
         dialog::addBoolItem_action(XLAT("reduce if walls on the way"), vid.use_wall_radar, 'R');
       });
  param_f(vid.wall_height, "wall_height", "3D wall height", .3)
    ->editable(0, 1, .1, "Height of walls", "", 'w')
    ->set_extra([] () {
        dialog::addHelp(GDIM == 3 ? "" : XLAT(
          "The height of walls, in absolute units. For the current values of g and c, "
          "wall height of %1 absolute units corresponds to projection value of %2.",
          fts(geom3::actual_wall_height()), fts(geom3::factor_to_projection(cgi.WALL))));
        dialog::addBoolItem(XLAT("auto-adjust in Goldberg grids"), vid.gp_autoscale_heights, 'O');
        dialog::add_action([] () {
          vid.gp_autoscale_heights = !vid.gp_autoscale_heights;
          });
        });
  param_f(vid.rock_wall_ratio, "rock_wall_ratio", "3D rock-wall ratio", .9)
    ->editable(0, 1, .1, "Rock-III to wall ratio", "", 'r')
    ->set_extra([] { dialog::addHelp(XLAT(
        "The ratio of Rock III to walls is %1, so Rock III are %2 absolute units high. "
        "Length of paths on the Rock III level is %3 of the corresponding length on the "
        "ground level.",
        fts(vid.rock_wall_ratio), fts(vid.wall_height * vid.rock_wall_ratio),
        fts(cosh(vid.depth - vid.wall_height * vid.rock_wall_ratio) / cosh(vid.depth))));
        });
  param_f(vid.human_wall_ratio, "human_wall_ratio", "3D human-wall ratio", .7)
    ->editable(0, 1, .1, "Human to wall ratio", "", 'h')
    ->set_extra([] { dialog::addHelp(XLAT(
        "Humans are %1 "
        "absolute units high. Your head travels %2 times the distance travelled by your "
        "feet.",
        fts(vid.wall_height * vid.human_wall_ratio),
        fts(cosh(vid.depth - vid.wall_height * vid.human_wall_ratio) / cosh(vid.depth)))
        );
        });
  param_f(vid.lake_top, "lake_top", "3D lake top", .25)
    ->editable(0, 1, .1, "Level of water surface", "", 'l');
  param_f(vid.lake_bottom, "lake_bottom", "3D lake bottom", .9)
    ->editable(0, 1, .1, "Level of water bottom", "", 'k');
  addsaver(vid.tc_depth, "3D TC depth", 1);
  addsaver(vid.tc_camera, "3D TC camera", 2);
  addsaver(vid.tc_alpha, "3D TC alpha", 3);
  param_f(vid.highdetail, "highdetail", "3D highdetail", 8)
    ->editable(0, 5, .5, "High detail range", "", 'n')
    ->set_extra(explain_detail)
    ->set_reaction([] {
      if(vid.highdetail > vid.middetail) vid.middetail = vid.highdetail;
      });  
  param_f(vid.middetail, "middetail", "3D middetail", 8)
    ->editable(0, 5, .5, "Mid detail range", "", 'm')
    ->set_extra(explain_detail)
    ->set_reaction([] {
      if(vid.highdetail > vid.middetail) vid.highdetail = vid.middetail;
      });
  param_i(debug_tiles, "debug_tiles")->editable(0, 2, 1, 
    "display tile debug values",
    "Display cell type IDs, as well as vertex and edge identifiers.\n\n"
    "Setting 1 uses the internal shape IDs, while setting 2 in tes files uses "
    "the original IDs in case if extra tile types were added to "
    "separate mirror images or different football types.", 'd');
  param_f(global_boundary_ratio, "global_boundary_ratio")
  ->editable(0, 5, 0.1, "Width of cell boundaries",
    "How wide should the cell boundaries be.", 'b');
  addsaver(vid.gp_autoscale_heights, "3D Goldberg autoscaling", true);  
  addsaver(scorefile, "savefile");
  param_b(savefile_selection, "savefile_selection")
  -> editable("select the score/save file on startup", 's')
  -> set_reaction([] {
    if(savefile_selection)
      addMessage(XLAT("Save the config and restart to select another score/save file."));
    else if(scorefile == "")
      addMessage(XLAT("Save the config to always play without recording your progress."));
    else
      addMessage(XLAT("Save the config to always use %1.", scorefile));
    });
  });

EX void switchcolor(unsigned int& c, unsigned int* cs) {
  dialog::openColorDialog(c, cs);
  }


double cc_footphase;
int lmousex, lmousey;

EX void showCustomizeChar() {

  cc_footphase += hypot(mousex - lmousex, mousey - lmousey);
  lmousex = mousex; lmousey = mousey;

  cmode = sm::SIDE | sm::MAYDARK;
  gamescreen();
  dialog::init(XLAT("Customize character"));
  
  if(shmup::on || multi::players) multi::cpid = multi::cpid_edit % multi::players;
  charstyle& cs = getcs();
  
  dialog::addSelItem(XLAT("character"), csname(cs), 'g');
  dialog::addColorItem(XLAT("skin color"), cs.skincolor, 's');
  dialog::addColorItem(XLAT("eye color"), cs.eyecolor, 'e');
  dialog::addColorItem(XLAT("weapon color"), cs.swordcolor, 'w');
  dialog::addColorItem(XLAT("hair color"), cs.haircolor, 'h');
  
  if(cs.charid >= 1) dialog::addColorItem(XLAT("dress color"), cs.dresscolor, 'd');
  else dialog::addBreak(100);
  if(cs.charid == 3) dialog::addColorItem(XLAT("dress color II"), cs.dresscolor2, 'f');
  else dialog::addBreak(100);
  
  dialog::addColorItem(XLAT("movement color"), cs.uicolor, 'u');

  if(!shmup::on && multi::players == 1) dialog::addSelItem(XLAT("save whom"), XLAT1(minf[moPrincess].name), 'p');
  
  if(numplayers() > 1) dialog::addSelItem(XLAT("player"), its(multi::cpid+1), 'a');

  dialog::addBoolItem(XLAT("left-handed"), cs.lefthanded, 'l');
  
  dialog::addBreak(50);
  dialog::addBack();
  dialog::display();
  
  int firsty = dialog::items[0].position / 2;
  int scale = firsty - 2 * vid.fsize;
  
  flat_model_enabler fme;

  initquickqueue();
  transmatrix V = atscreenpos(vid.xres/2, firsty, scale);
  
  double alpha = atan2(mousex - vid.xres/2, mousey - firsty) - 90._deg;
  V = V * spin(alpha);
  drawMonsterType(moPlayer, NULL, shiftless(V), 0, cc_footphase / scale, NOCOLOR);
  quickqueue();
  
  keyhandler = [] (int sym, int uni) {
    dialog::handleNavigation(sym, uni);
  
    if(shmup::on || multi::players) multi::cpid = multi::cpid_edit % multi::players;
    charstyle& cs = getcs();
    bool cat = cs.charid >= 4;
    if(uni == 'a') { multi::cpid_edit++; multi::cpid_edit %= 60; }
    else if(uni == 'g') {
      cs.charid++;
      if(cs.charid == 2 && !princess::everSaved && !autocheat) cs.charid = 4;
      cs.charid %= 10;
      }
    else if(uni == 'p') vid.samegender = !vid.samegender;
    else if(uni == 's') switchcolor(cs.skincolor, cat ? haircolors : skincolors);
    else if(uni == 'h') switchcolor(cs.haircolor, haircolors);
    else if(uni == 'w') switchcolor(cs.swordcolor, swordcolors);
    else if(uni == 'd') switchcolor(cs.dresscolor, cat ? haircolors : dresscolors);
    else if(uni == 'f') switchcolor(cs.dresscolor2, dresscolors2);
    else if(uni == 'u') switchcolor(cs.uicolor, eyecolors);
    else if(uni == 'e') switchcolor(cs.eyecolor, eyecolors);
    else if(uni == 'l') cs.lefthanded = !cs.lefthanded;
    else if(doexiton(sym, uni)) popScreen();
    };
  }

EX void refresh_canvas() {
  manual_celllister cl;
  cl.add(cwt.at);
    
  int at = 0;
  while(at < isize(cl.lst)) {
    cell *c2 = cl.lst[at];
    c2->landparam = patterns::generateCanvas(c2);
    at++;
    
    forCellEx(c3, c2) cl.add(c3);
    }
  }

EX color_t addalpha(color_t c) { return (c << 8) | 0xFF; }

EX void edit_color_table(colortable& ct, const reaction_t& r IS(reaction_t()), bool has_bit IS(false)) {
  cmode = sm::SIDE;
  gamescreen();
  dialog::init(XLAT("colors & aura"));
  
  for(int i=0; i<isize(ct); i++) {
    dialog::addColorItem(its(i), addalpha(ct[i]), 'a'+i);
    if(WDIM == 3 && has_bit && !(ct[i] & 0x1000000)) dialog::lastItem().value = XLAT("(no wall)");
    dialog::add_action([i, &ct, r, has_bit] () { 
      if(WDIM == 3 && has_bit) {
        ct[i] ^= 0x1000000;
        if(!(ct[i] & 0x1000000)) return;
        }
      dialog::openColorDialog(ct[i]); 
      dialog::reaction = r; 
      dialog::colorAlpha = false;
      dialog::dialogflags |= sm::SIDE;
      });
    }

  dialog::addItem("add a color", 'A');
  dialog::add_action([&ct, r] {
    ct.push_back(rand() & 0x1FFFFFF);
    r();
    });

  if(isize(ct) > 2) {
    dialog::addItem("delete a color", 'D');
    dialog::add_action([&ct, r] {
      ct.pop_back();
      r();
      });
    }

  dialog::addBack();
  dialog::display();
  }

EX void show_color_dialog() {
  cmode = sm::SIDE | sm::DIALOG_STRICT_X;
  getcstat = '-';
  gamescreen();
  dialog::init(XLAT("colors & aura"));

  dialog::addColorItem(XLAT("background"), addalpha(backcolor), 'b');
  dialog::add_action([] () { dialog::openColorDialog(backcolor); dialog::colorAlpha = false; dialog::dialogflags |= sm::SIDE; });
  
  if(WDIM == 2 && GDIM == 3 && hyperbolic)
    dialog::addBoolItem_action(XLAT("cool fog effect"), context_fog, 'B');

  dialog::addColorItem(XLAT("foreground"), addalpha(forecolor), 'f');
  dialog::add_action([] () { dialog::openColorDialog(forecolor); dialog::colorAlpha = false; dialog::dialogflags |= sm::SIDE; });

  dialog::addColorItem(XLAT("borders"), addalpha(bordcolor), 'o');
  dialog::add_action([] () { dialog::openColorDialog(bordcolor); dialog::colorAlpha = false; dialog::dialogflags |= sm::SIDE; });

  dialog::addColorItem(XLAT("projection boundary"), ringcolor, 'r');
  dialog::add_action([] () { dialog::openColorDialog(ringcolor); dialog::dialogflags |= sm::SIDE; });

  dialog::addSelItem(XLAT("boundary width multiplier"), fts(vid.multiplier_ring), 'R');
  dialog::add_action([] () { dialog::editNumber(vid.multiplier_ring, 0, 10, 1, 1, XLAT("boundary width multiplier"), ""); });

  dialog::addColorItem(XLAT("projection background"), modelcolor, 'c');
  dialog::add_action([] () { dialog::openColorDialog(modelcolor); dialog::dialogflags |= sm::SIDE; });

  dialog::addColorItem(XLAT("standard grid color"), stdgridcolor, 'g');
  dialog::add_action([] () { vid.grid = true; dialog::openColorDialog(stdgridcolor); dialog::dialogflags |= sm::SIDE; });
  
  dialog::addSelItem(XLAT("grid width multiplier"), fts(vid.multiplier_grid), 'G');
  dialog::add_action([] () { dialog::editNumber(vid.multiplier_grid, 0, 10, 1, 1, XLAT("grid width multiplier"), ""); });

  dialog::addSelItem(XLAT("brightness behind the sphere"), fts(backbrightness), 'i');
  dialog::add_action([] () { dialog::editNumber(backbrightness, 0, 1, .01, 0.25, XLAT("brightness behind the sphere"), 
    XLAT("In the orthogonal projection, objects on the other side of the sphere are drawn darker.")); dialog::bound_low(0); });

  dialog::addColorItem(XLAT("projection period"), periodcolor, 'p');
  dialog::add_action([] () { dialog::openColorDialog(periodcolor); dialog::dialogflags |= sm::SIDE; });

  dialog::addColorItem(XLAT("dialogs"), addalpha(dialog::dialogcolor), 'd');
  dialog::add_action([] () { dialog::openColorDialog(dialog::dialogcolor); dialog::colorAlpha = false; dialog::dialogflags |= sm::SIDE; });

  dialog::addBreak(50);
  if(specialland == laCanvas && colortables.count(patterns::whichCanvas)) {
    dialog::addItem(XLAT("pattern colors"), 'P');
    dialog::add_action_push([] { edit_color_table(colortables[patterns::whichCanvas], refresh_canvas, true); });

    if(patterns::whichCanvas == 'R') {
      dialog::addItem(XLAT("unreversed colors"), 'U');
      dialog::add_action_push([] { edit_color_table(colortables['A'], refresh_canvas, true); });
      }
    }
 
  if(cwt.at->land == laMinefield) {
    dialog::addItem(XLAT("minefield colors"), 'm');
    dialog::add_action_push([] { edit_color_table(minecolors); });
    }
  
  if(viewdists) {
    dialog::addItem(XLAT("distance colors"), 'd');
    dialog::add_action_push([] () {edit_color_table(distcolors); });
    }
  
  #if CAP_CRYSTAL
  if(cryst && cheater) {
    dialog::addItem(XLAT("crystal coordinate colors"), 'C');
    dialog::add_action([] () { crystal::view_coordinates = true; pushScreen([] () { edit_color_table(crystal::coordcolors); });});
    }
  #endif

  if(cwt.at->land == laTortoise) {
    dialog::addBoolItem_action(XLAT("Galápagos shading"), tortoise::shading_enabled, 'T');
    }

  dialog::addInfo(XLAT("colors of some game objects can be edited by clicking them."));
  
  dialog::addBreak(50);

  dialog::addSelItem(XLAT("aura brightness"), its(vid.aurastr), 'a');
  dialog::add_action([] () { dialog::editNumber(vid.aurastr, 0, 256, 10, 128, XLAT("aura brightness"), ""); dialog::bound_low(0); });

  dialog::addSelItem(XLAT("aura smoothening factor"), its(vid.aurasmoothen), 's');
  dialog::add_action([] () { dialog::editNumber(vid.aurasmoothen, 1, 180, 1, 5, XLAT("aura smoothening factor"), ""); dialog::bound_low(1); });  

  dialog::addBreak(50);
  dialog::addBack();
  dialog::display();

  keyhandler = [] (int sym, int uni) {
    if(uni == '-') {
      cell *c = mouseover;
      if(!c) return;
      else if(c == cwt.at) {
        pushScreen(showCustomizeChar);
        return;
        }
      else if(c->monst) 
        dialog::openColorDialog(minf[c->monst].color);
      else if(c->item) 
        dialog::openColorDialog(iinf[c->item].color);
      else if(c->wall) 
        dialog::openColorDialog(winf[c->wall == waMineMine ? waMineUnknown : c->wall].color);
      #if CAP_COMPLEX2
      else if(c->land == laBrownian) 
        dialog::openColorDialog(brownian::get_color_edit(c->landparam));
      #endif
      else 
        dialog::openColorDialog(floorcolors[c->land]);
      dialog::colorAlpha = false;
      dialog::dialogflags |= sm::SIDE;
      return;
      }
    else dialog::handleNavigation(sym, uni);
    if(doexiton(sym, uni)) popScreen();
    };
  }

#if CAP_CONFIG
EX void resetConfigMenu() {
  dialog::init(XLAT("reset all configuration"));
  dialog::addInfo("Are you sure?");
  dialog::addItem("yes, and delete the config file", 'd');
  dialog::addItem("yes", 'y');
  dialog::addItem("cancel", 'n');
  dialog::addItem("reset the special game modes", 'r');
  dialog::display();
  keyhandler = [] (int sym, int uni) {
    dialog::handleNavigation(sym, uni);

    if(uni == 'd') { 
      resetConfig();
      unlink(conffile);
      popScreen();
      }
    else if(uni == 'y') {
      printf("resetting config\n");
      resetConfig();
      printf("config reset\n");
      popScreen();
      }
    else if(uni == 'r') 
      resetModes();
    else if(uni == 'n' || doexiton(sym, uni)) 
      popScreen();
    
    };
  }
#endif

#if CAP_TRANS
EX void selectLanguageScreen() {
  cmode = sm::SIDE | sm::MAYDARK;
  gamescreen();
  dialog::init("select language"); // intentionally not translated

  int v = vid.language;  
  dynamicval<int> d(vid.language, -1);
  
  for(int i=0; i<NUMLAN; i++) {
    vid.language = i;
    dialog::addSelItem(XLAT("EN"), its(100 * transcompleteness[i] / transcompleteness[0]) + "%", 'a'+i);
    }
  
  dialog::addBreak(50);
  vid.language = -1;
  dialog::addBoolItem(XLAT("default") + ": " + XLAT("EN"), v == -1, '0');
  dialog::addBack();

  dialog::addBreak(50);

  vid.language = v;
  if(lang() >= 1)
    dialog::addHelp(XLAT("add credits for your translation here"));
  else
    dialog::addHelp(XLAT("original language"));

  if(lang() != 0) {
    string tw = "";
    string s = XLAT("TRANSLATIONWARNING");
    if(s != "" && s != "TRANSLATIONWARNING") tw += s;
    s = XLAT("TRANSLATIONWARNING2");
    if(s != "" && s != "TRANSLATIONWARNING2") { if(tw != "") tw += " "; tw += s; }
    if(tw != "") {
      dialog::addHelp(tw);
      dialog::lastItem().color = 0xFF0000;
      }
    }

  dialog::display();
  
  keyhandler = []   (int sym, int uni) {
    dialog::handleNavigation(sym, uni);
    
    if(uni == '0') {
      vid.language = -1;
      android_settings_changed();
      }

    else if(uni >= 'a' && uni < 'a'+NUMLAN) {
      vid.language = uni - 'a';
      android_settings_changed();
      }
    
    else if(doexiton(sym, uni))
      popScreen();
    };
  }
#endif

EX void configureMouse() {
  cmode = sm::SIDE | sm::MAYDARK;
  gamescreen();
  dialog::init(XLAT("mouse & touchscreen"));

  dialog::addBoolItem_action(XLAT("reverse pointer control"), (vid.revcontrol), 'r');
  
  dialog::addBoolItem_action(XLAT("draw circle around the target"), (vid.drawmousecircle), 'd');
  
  if(GDIM == 3) {
    dialog::addBoolItem_action(XLAT("highlight the cell forward"), vid.axes3, 'f');
    }
  
#if ISMOBILE
  dialog::addBoolItem(XLAT("targetting ranged Orbs long-click only"), (vid.shifttarget&2), 'i');
#else
  dialog::addBoolItem(XLAT("targetting ranged Orbs Shift+click only"), (vid.shifttarget&1), 'i');
#endif
  dialog::add_action([] {vid.shifttarget = vid.shifttarget^3; });    

  #if !ISMOBILE
  dialog::addBoolItem_action(XLAT("quick mouse"), vid.quickmouse, 'M');
  #endif

  dialog::addSelItem(XLAT("move by clicking on compass"), its(vid.mobilecompasssize), 'C');
  dialog::add_action([] {
    dialog::editNumber(vid.mobilecompasssize, 0, 100, 10, 20, XLAT("compass size"), XLAT("0 to disable"));
    // we need to check the moves
    dialog::reaction = checkmove;
    dialog::bound_low(0);
    });

  #if CAP_ORIENTATION
  if(GDIM == 2) {
    dialog::addSelItem(XLAT("scrolling by device rotation"), ors::choices[ors::mode], '1');  
    dialog::add_action_push(ors::show);
    }
  #endif

  dialog::addBack();
  dialog::display();
  }

vector<setting*> last_changed;

EX void add_to_changed(setting *f) {
  auto orig_f = f;
  for(int i=0; i<isize(last_changed); i++) {
    if(last_changed[i] == f)
      return;
    swap(last_changed[i], f);
    if(f == orig_f) return;
    }
  last_changed.push_back(f);
  }

EX setting *find_edit(void *val) {
  for(auto& fs: params) {
    fs.second->check_change();
    if(fs.second->affects(val))
      return &*fs.second;
    }
  return nullptr;
  }

EX void add_edit_ptr(void *val) {
  int found = 0;
  for(auto& fs: params) {
    fs.second->check_change();
    if(fs.second->affects(val))
      fs.second->show_edit_option(), found++;
    }
  if(found != 1) println(hlog, "found = ", found);
  }

#if HDR
template<class T> void add_edit(T& val) {
  add_edit_ptr(&val);
  }
#endif

EX void find_setting() {
  cmode = sm::SIDE | sm::MAYDARK;
  gamescreen();

  dialog::init(XLAT("find a setting"));
  if(dialog::infix != "") mouseovers = dialog::infix;

  dialog::start_list(900, 900, '1');

  int found = 0;

  for(auto& p: params) {
    auto& fs = p.second;
    string key = fs->search_key();
    if(fs->available() && dialog::hasInfix(key)) {
      fs->show_edit_option(dialog::list_fake_key++);
      found++;
      }
    }

  dialog::end_list();

  dialog::addBreak(100);
  dialog::addInfo(XLAT("press letters to search"));
  dialog::addSelItem(XLAT("matching items"), its(found), 0);
  dialog::display();

  keyhandler = [] (int sym, int uni) {
    dialog::handleNavigation(sym, uni);
    if(dialog::editInfix(uni)) dialog::list_skip = 0;
    else if(doexiton(sym, uni)) popScreen();
    };
  }

EX void edit_all_settings() {
  cmode = sm::SIDE | sm::MAYDARK;
  gamescreen();
  dialog::init(XLAT("recently changed settings"));

  for(auto &fs: params) fs.second->check_change();

  dialog::start_list(1000, 1000, 'a');
  for(auto l: last_changed) 
    if(l->available())
      l->show_edit_option(dialog::list_fake_key++);
  dialog::end_list();

  dialog::addBreak(100);
  dialog::addItem(XLAT("find a setting"), '/');
  dialog::add_action_push(find_setting);
  dialog::addBack();
  dialog::display();
  }

void list_setting::show_edit_option(int key) {
  string opt = options[get_value()].first;
  dialog::addSelItem(XLAT(menu_item_name), XLAT(opt), key);
  dialog::add_action_push([this] {
    add_to_changed(this);
    cmode = sm::SIDE | sm::MAYDARK;
    gamescreen();
    dialog::init(XLAT(menu_item_name));
    dialog::addBreak(100);
    int q = isize(options);

    int need_list = q > 15 ? 2 : q > 10 ? 1 : 0;

    if(need_list >= 2) dialog::start_list(1500, 1500, 'a');
    for(int i=0; i<q; i++) {
      dialog::addBoolItem(XLAT(options[i].first), get_value() == i, need_list >= 2 ? dialog::list_fake_key++ : 'a' + i);
      dialog::add_action([this, i, need_list] { set_value(i); if(reaction) reaction(); if(need_list == 0) popScreen(); });
      if(need_list == 0 && options[i].second != "") {
        dialog::addBreak(100);
        dialog::addHelp(XLAT(options[i].second));
        dialog::addBreak(100);
        }
      }
    if(need_list >= 2) dialog::end_list();
    dialog::addBreak(100);

    if(need_list >= 1 && options[get_value()].second != "") {
      dialog::addHelp(XLAT(options[get_value()].second));
      dialog::addBreak(100);
      }
    dialog::addBack();
    dialog::display();
    });
  }

EX void showSettings() {
  cmode = sm::SIDE | sm::MAYDARK;
  gamescreen();
  dialog::init(XLAT("settings"));

  dialog::addItem(XLAT("interface"), 'i');
  dialog::add_action_push(configureInterface);

  dialog::addItem(XLAT("general graphics"), 'g');
  dialog::add_action_push(showGraphConfig);

  dialog::addItem(XLAT("3D configuration"), '9');
  dialog::add_action_push(show3D);

  dialog::addItem(XLAT("quick options"), 'q');
  dialog::add_action_push(showGraphQuickKeys);

  dialog::addItem(XLAT("models & projections"), 'p');
  dialog::add_action_push(models::quick_model);

  dialog::addItem(XLAT("colors & aura"), 'c');
  dialog::add_action_push(show_color_dialog);

#if CAP_SHMUP && !ISMOBILE
  dialog::addSelItem(XLAT("keyboard & joysticks"), "", 'k');
  dialog::add_action(multi::configure);
#endif

  dialog::addSelItem(XLAT("mouse & touchscreen"), "", 'm');
  dialog::add_action_push(configureMouse);

  dialog::addItem(XLAT("other settings"), 'o');
  dialog::add_action_push(configureOther);
  
  dialog::addBreak(100);

#if CAP_CONFIG
  dialog::addItem(XLAT("find a setting"), '/');
  dialog::add_action_push(edit_all_settings);

  dialog::addItem(XLAT("save the current config"), 's');
  dialog::add_action(saveConfig);

  dialog::addItem(XLAT("reset all configuration"), 'R');
  dialog::add_action_push(resetConfigMenu);
#endif  
  
  if(getcstat == 's') mouseovers = XLAT("Config file: %1", conffile);
  
  dialog::addBack();
  dialog::display();
  }

#if CAP_COMMANDLINE

EX int read_color_args() {
  using namespace arg;

  if(argis("-back")) {
    PHASEFROM(2); shift(); backcolor = argcolor(24);
    }
  else if(argis("-fillmodel")) {
    PHASEFROM(2); shift(); modelcolor = argcolor(32);
    }
  else if(argis("-apeirocolor")) {
    PHASEFROM(2); shift(); patterns::apeirogonal_color = argcolor(32);
    }
  else if(argis("-ring")) {
    PHASEFROM(2); shift(); ringcolor = argcolor(32);
    }
  else if(argis("-ringw")) {
    PHASEFROM(2); shift_arg_formula(vid.multiplier_ring);
    }
  else if(argis("-stdgrid")) {
    PHASEFROM(2); shift(); stdgridcolor = argcolor(32);
    }
  else if(argis("-gridw")) {
    PHASEFROM(2); shift_arg_formula(vid.multiplier_grid);
    }
  else if(argis("-period")) {
    PHASEFROM(2); shift(); periodcolor = argcolor(32);
    }
  else if(argis("-crosshair")) {
    PHASEFROM(2); shift(); crosshair_color = argcolor(32);
    shift_arg_formula(crosshair_size);
    }
  else if(argis("-borders")) {
    PHASEFROM(2); shift(); bordcolor = argcolor(24);
    }
  else if(argis("-fore")) {
    PHASEFROM(2); shift(); forecolor = argcolor(24);
    }
  else if(argis("-title")) {
    PHASEFROM(2); shift(); titlecolor = argcolor(24);
    }
  else if(argis("-dialog")) {
    PHASEFROM(2); shift(); dialog::dialogcolor = argcolor(24);
    }
  else if(argis("-d:color"))
    launch_dialog(show_color_dialog);
  else return 1;
  return 0;
  }

EX int read_config_args() {
  using namespace arg;

  if(argis("-c")) { PHASE(1); shift(); conffile = argcs(); }
// change the configuration from the command line
  else if(argis("-aa")) { PHASEFROM(2); shift(); vid.want_antialias = argi(); apply_screen_settings(); }
  else if(argis("-lw")) { PHASEFROM(2); shift_arg_formula(vid.linewidth); }
  else if(argis("-wm")) { PHASEFROM(2); shift(); vid.wallmode = argi(); }
  else if(argis("-mm")) { PHASEFROM(2); shift(); vid.monmode = argi(); }

  else if(argis("-noshadow")) { noshadow = true; }
  else if(argis("-bright")) { bright = true; }
  else if(argis("-gridon")) { vid.grid = true; }
  else if(argis("-gridoff")) { vid.grid = false; }

// non-configurable options
  else if(argis("-vsync_off")) {
    vid.want_vsync = false;
    apply_screen_settings();
    }
  else if(argis("-aura")) {
    PHASEFROM(2);
    shift(); vid.aurastr = argi();
    shift(); vid.aurasmoothen = argi();
    }
  else if(argis("-nofps")) {
    PHASEFROM(2);
    nofps = true;
    }
  else if(argis("-nohud")) {
    PHASEFROM(2);
    nohud = true;
    }
  else if(argis("-nomenu")) {
    PHASEFROM(2);
    nomenukey = true;
    }
  else if(argis("-nomsg")) {
    PHASEFROM(2);
    nomsg = true;
    }
#if MAXMDIM >= 4
  else if(argis("-switch-fpp")) {
    PHASEFROM(2);
    geom3::switch_fpp();
    }
#endif
  else if(argis("-switch-tpp")) {
    PHASEFROM(2);
    geom3::switch_tpp();
    }
#if MAXMDIM >= 4
  else if(argis("-switch-3d")) {
    PHASEFROM(2);
    geom3::switch_always3();
    }
#endif
  else if(argis("-nohelp")) {
    PHASEFROM(2);
    nohelp = true;
    }
  else if(argis("-dont_face_pc")) {
    PHASEFROM(2);
    dont_face_pc = true;
    }

#if CAP_TRANS
  else if(argis("-lang")) { 
    PHASEFROM(2); shift(); vid.language = argi();
    }
#endif
  else if(argis("-vlq")) { 
    PHASEFROM(2); shift(); vid.linequality = argi();
    }
  else if(argis("-fov")) { 
    PHASEFROM(2); shift_arg_formula(vid.fov);
    }
  else if(argis("-r")) { 
    PHASEFROM(2);
    shift(); 
    if(vid.want_fullscreen) {
      int clWidth=0, clHeight=0, clFont=0;
      sscanf(argcs(), "%dx%dx%d", &clWidth, &clHeight, &clFont);
      vid.change_fullscr = clWidth;
      if(clWidth) vid.fullscreen_x = clWidth;
      if(clHeight) vid.fullscreen_y = clHeight;
      if(clFont) vid.abs_fsize = clFont, vid.relative_font = true;
      }
    else if(args().find(".") != string::npos) {
      vid.relative_window_size = true;
      ld dWidth=0, dHeight=0;
      sscanf(argcs(), "%lfx%lf", &dWidth, &dHeight);
      if(dWidth) vid.window_rel_x = dWidth;
      if(dHeight) vid.window_rel_y = dHeight;
      }
    else {
      vid.want_fullscreen = false;
      vid.relative_window_size = false;
      int clFont=0;
      sscanf(argcs(), "%dx%dx%d", &vid.window_x, &vid.window_y, &clFont);
      if(clFont) vid.abs_fsize = clFont, vid.relative_font = true;
      }
    }    
  else if(argis("-msm")) {
    PHASEFROM(2); memory_saving_mode = true;
    }
  else if(argis("-mrsv")) {
    PHASEFROM(2); shift(); reserve_limit = argi(); apply_memory_reserve();
    }
  else if(argis("-yca")) {
    PHASEFROM(2); 
    shift_arg_formula(vid.yshift);
    shift_arg_formula(pconf.camera_angle);
    }
  else if(argis("-pside")) {
    PHASEFROM(2); 
    permaside = true;
    }
  else if(argis("-xy")) {
    PHASEFROM(2); 
    shift_arg_formula(pconf.xposition);
    shift_arg_formula(pconf.yposition);
    }
  else if(argis("-fixdir")) {
    PHASEFROM(2); 
    vid.fixed_facing = true;
    shift_arg_formula(vid.fixed_facing_dir);
    }
  else if(argis("-fixdiroff")) {
    PHASEFROM(2); 
    vid.fixed_facing = false;
    }
  else if(argis("-msmoff")) {
    PHASEFROM(2); memory_saving_mode = false;
    }
  else if(argis("-levellines")) {
    PHASEFROM(2); shift_arg_formula(levellines);
    }
  else if(argis("-level-notexture")) {
    PHASEFROM(2); disable_texture = true;
    }
  else if(argis("-level-texture")) {
    PHASEFROM(2); disable_texture = false;
    }
  else if(argis("-msens")) {
    PHASEFROM(2); shift_arg_formula(mouseaim_sensitivity);
    }
  TOGGLE('o', vid.wantGL, { vid.wantGL = !vid.wantGL; apply_screen_settings();})
  TOGGLE('f', vid.want_fullscreen, { vid.want_fullscreen = !vid.want_fullscreen; apply_screen_settings(); })
  else if(argis("-noshaders")) {
    PHASE(1);
    glhr::noshaders = true; 
    }
  else if(argis("-d:sight")) {
    PHASEFROM(2); launch_dialog(); edit_sightrange();
    }
  else if(argis("-d:char")) {
    PHASEFROM(2); launch_dialog(showCustomizeChar);
    }
  else if(argis("-d:3")) {
    PHASEFROM(2); launch_dialog(show3D);
    }
  else if(argis("-d:stereo")) {
    PHASEFROM(2); launch_dialog(showStereo);
    }
  else if(argis("-d:iface")) {
    PHASEFROM(2); launch_dialog(configureInterface);
    }
  else if(argis("-d:graph")) {
    PHASEFROM(2); launch_dialog(showGraphConfig);
    }
  else if(argis("-tstep")) {
    PHASEFROM(2); shift(); vid.texture_step = argi();
    }
  else if(argis("-csc")) {
    PHASEFROM(2); shift_arg_formula(vid.creature_scale);
    }
  else if(argis("-neon")) {
    PHASEFROM(2);
    shift(); neon_mode = eNeon(argi());
    }
  else if(argis("-dmc")) {
    PHASEFROM(2);
    shift(); vid.drawmousecircle = argi();
    }
  else if(argis("-smooths")) {
    PHASEFROM(2);
    shift(); smooth_scrolling = argi();
    }
  else if(argis("-via-shader")) {
    PHASEFROM(2);
    shift(); vid.consider_shader_projection = argi();
    }
  else if(argis("-neonnf")) {
    PHASEFROM(2);
    shift(); neon_nofill = argi();
    }
  else if(argis("-precw")) {
    PHASEFROM(2);
    shift_arg_formula(precise_width);
    }
  else if(argis("-d:all")) {
    PHASEFROM(2); launch_dialog(edit_all_settings);
    }
  else if(argis("-d:find")) {
    PHASEFROM(2); launch_dialog(find_setting);
    }
  else if(argis("-char")) {
    auto& cs = vid.cs;
    shift();
    string s = args();
    if(s == "dodek") {
      cs.charid = 4;
      cs.lefthanded = false;
      cs.skincolor = 0x202020FF;
      cs.eyecolor = 0x20C000FF;
      cs.haircolor = 0x202020FF;
      cs.dresscolor =0x424242FF;
      cs.swordcolor = 0xF73333FF;      
      }
    else if(s == "rudy") {
      cs.charid = 4;
      cs.lefthanded = false;
      cs.skincolor = 0xA44139FF;
      cs.eyecolor = 0xD59533FF;
      cs.haircolor = 0xC6634AFF;
      cs.dresscolor =0xC6634AFF;
      cs.swordcolor = 0x3CBB33FF;      
      }
    else if(s == "running") {
      cs.charid = 6;
      cs.lefthanded = false;
      cs.skincolor = 0xFFFFFFFF;
      cs.eyecolor = 0xFF;
      cs.haircolor = 0xFFFFFFFF;
      cs.dresscolor =0xFFFFFFFF;
      cs.swordcolor = 0xFF0000FF;
      }
    else if(s == "princess") {
      cs.charid = 3;
      cs.lefthanded = true;
      cs.skincolor  = 0xEFD0C9FF;
      cs.haircolor  = 0x301800FF;
      cs.eyecolor   = 0xC000FF;
      cs.dresscolor = 0x408040FF;
      cs.swordcolor = 0xFFFFFFFF;
      }
    else if(s == "worker") {
      cs.charid = 2;
      cs.skincolor = 0xC77A58FF;
      cs.haircolor = 0x502810FF;
      cs.dresscolor = 0xC0C000FF;
      cs.eyecolor = 0x500040FF;
      cs.swordcolor = 0x808080FF;
      }
    else {
      cs.charid = argi();
      cs.lefthanded = cs.charid >= 10;
      cs.charid %= 10;
      }
    }
  else return 1;
  return 0;
  }

EX int read_param_args() {
  const string& s = arg::args();
  auto pos = s.find("=");
  if(pos == string::npos) return 1;
  string name = s.substr(0, pos);
  string value = s.substr(pos+1);
  PHASEFROM(2);
  if(!params.count(name))  {
    println(hlog, "parameter unknown: ", name);
    exit(1);
    }
  params[name]->load_from(value);
  return 0;
  }

// mode changes:

EX int read_gamemode_args() {
  using namespace arg;

  if(argis("-P")) { 
    PHASE(2); shift(); 
    stop_game_and_switch_mode(rg::nothing);
    multi::players = argi();
    }
  TOGGLE('S', shmup::on, stop_game_and_switch_mode(rg::shmup))
  TOGGLE('H', hardcore, switchHardcore())
  TOGGLE('R', randomPatternsMode, stop_game_and_switch_mode(rg::randpattern))
  TOGGLE('i', inv::on, stop_game_and_switch_mode(rg::inv))
  
  else return 1;
  return 0;
  }

auto ah_config = 
  addHook(hooks_args, 0, read_config_args) + 
  addHook(hooks_args, 0, read_param_args) + 
  addHook(hooks_args, 0, read_gamemode_args) + addHook(hooks_args, 0, read_color_args);
#endif

}
