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

EX void add_to_changed(struct parameter *f);

EX bool return_false() { return false; }

EX bool use_bool_dialog;

EX string param_esc(string s);

EX void non_editable_pre() { if(game_active) stop_game(); };
EX void non_editable_post() { if(!delayed_start) start_game(); };

#if HDR

using key_type = int;

struct param_exception : hr_exception {
  struct parameter *which;
  param_exception() : hr_exception("param_exception"), which(nullptr) {}
  param_exception(const std::string &s, parameter *p) : hr_exception(s), which(p) {}
  };

struct parameter_names {
  string name;
  string legacy_config_name;
  parameter_names(const string& s) { name = param_esc(s); legacy_config_name = s; }
  parameter_names(const char* s) { name = param_esc(s); legacy_config_name = s; }
  parameter_names(const string& p, const string& s) { name = p; legacy_config_name = s; }
  };

struct parameter : public std::enable_shared_from_this<parameter> {
  function<bool()> restrict;
  string name;
  string legacy_config_name;
  string menu_item_name;
  bool menu_item_name_modified;
  string help_text;
  reaction_t pre_reaction, reaction;
  char default_key;
  bool is_editable;
  bool needs_confirm;
  virtual bool available() { if(restrict) return restrict(); return true; }
  virtual bool affects(void *v) { return false; }
  void show_edit_option() { show_edit_option(default_key); }
  virtual void show_edit_option(key_type key) {
    println(hlog, "warning: no edit option defined for ", name);
    }
  virtual string search_key() { 
    return name + "|" + legacy_config_name + "|" + menu_item_name + "|" + help_text;
    }
  explicit parameter() { restrict = auto_restrict; is_editable = false; needs_confirm = false; }
  void be_non_editable() {
    pre_reaction = non_editable_pre;
    reaction = non_editable_post;
    }
  virtual void check_change() { }
  reaction_t sets;
  parameter *set_sets(const reaction_t& s) { sets = s; return this; }
  parameter *set_extra(const reaction_t& r);
  parameter *set_reaction(const reaction_t& r);
  virtual ~parameter() = default;
  virtual bool load_from_animation(const string& s) {
    load(s); return false;
    }
  virtual void load_as_animation(const string& s) {
    load(s);
    }
  virtual bool anim_unchanged() { return true; }
  virtual void anim_restore() { }
  virtual cld get_cld() { throw param_exception("parameter has no complex value", this); }
  virtual void set_cld_raw(cld x) { throw param_exception("parameter has no complex value", this); }
  virtual void set_cld(cld value) {
    auto bak = get_cld();
    set_cld_raw(value);
    if(value != bak && reaction) reaction();
    }

  virtual string save() = 0;
  virtual void load(const string& s) {
    throw param_exception("parameter cannot be loaded", this);
    }
  virtual bool dosave() = 0;
  virtual void reset() = 0;
  virtual void swap_with(parameter*) = 0;

  virtual shared_ptr<parameter> clone(struct local_parameter_set& lps, void *value) { throw param_exception("not cloneable", this); }

  void setup(const parameter_names& s);
  };

struct local_parameter_set {
  string label;
  local_parameter_set* extends;
  vector<pair<shared_ptr<parameter>, shared_ptr<parameter>>> swaps;
  void pswitch();
  local_parameter_set(string l, local_parameter_set *ext = nullptr) : label(l), extends(ext) {}
  parameter_names mod(parameter *t);
  };
#endif

parameter *parameter::set_extra(const reaction_t& r) {
  auto s = sets; set_sets([s, r] { if(s) s(); dialog::get_di().extra_options = r; }); return this;
  }

parameter *parameter::set_reaction(const reaction_t& r) {
  reaction = r; return this;
  }

#if HDR
using paramlist = map<string, std::shared_ptr<parameter>>;
#endif

EX paramlist params;

EX void show_edit_option_enum(char* value, const string& name, const vector<pair<string, string>>& options, char key, parameter *s);

#if HDR
struct list_parameter : parameter {
  reaction_t manual_reaction;
  virtual int get_value() = 0;
  virtual void set_value(int i) = 0;
  vector<pair<string, string> > options;
  list_parameter* editable(const vector<pair<string, string> >& o, string menu_item_name, char key) {
    is_editable = true;
    options = o;
    this->menu_item_name = menu_item_name;
    menu_item_name_modified = true;
    default_key = key;
    return this;
    }
  void show_edit_option(key_type key) override;
  };

namespace anims {
  extern void animate_parameter(parameter*, string);
  }

template<class T> struct enum_parameter : list_parameter {
  T *value, last_value, dft, anim_value;
  int get_value() override { return (int) *value; }
  hr::function<void(T)> set_value_to;
  void set_value(int i) override { set_value_to((T)i); }
  bool affects(void* v) override { return v == value; }
  virtual void load_from_raw(const string& s) {
    int N = isize(options);
    for(int i=0; i<N; i++) if(appears(options[i].first, s)) {
      set_value_to((T)i);
      return;
      }
    *value = (T) parseint(s);
    }
  void check_change() override {
    if(*value != last_value) {
      last_value = *value;
      add_to_changed(this);
      }
    }

  void load(const string& s) override {
    auto bak = *value;
    load_from_raw(s);
    if(*value != bak && pre_reaction) { swap(*value, bak); pre_reaction(); swap(*value, bak); }
    if(*value != bak && reaction) reaction();
    }
  bool load_from_animation(const string& s) override {
    if(anim_value != *value) return false;
    load(s);
    anim_value = *value;
    return true;
    }
  void load_as_animation(const string& s) override {
    load(s);
    anim_value = *value;
    anims::animate_parameter(this, s);
    }

  enum_parameter<T>* editable(const vector<pair<string, string> >& o, string menu_item_name, char key) {
    list_parameter::editable(o, menu_item_name, key);
    return this;
    }
  enum_parameter<T>* set_need_confirm() {
    needs_confirm = true;
    return this;
    }
  virtual cld get_cld() override { return get_value(); }

  bool dosave() override { return *value != dft; }
  void reset() override { *value = dft; }
  string save() override { return its(int(*value)); }
  shared_ptr<parameter> clone(struct local_parameter_set& lps, void *value) override;
  void swap_with(parameter *s) override { swap(*value, *(((enum_parameter<T>*)s)->value)); }
  };

template<class T> struct val_parameter;

template<class T> struct val_parameter : public parameter {
  T *value, last_value, anim_value, dft;

  bool dosave() override { return *value != dft; }
  void reset() override { *value = dft; }
  bool affects(void* v) override { return v == value; }

  void check_change() override {
    if(*value != last_value) {
      last_value = *value;
      add_to_changed(this);
      }
    }

  bool anim_unchanged() override { return *value == anim_value; }
  void anim_restore() override { *value = anim_value; if(reaction) reaction(); }

  virtual void load_from_raw(const string& s) { throw param_exception("load_from_raw not defined", this); }

  void load(const string& s) override {
    auto bak = *value;
    load_from_raw(s);
    if(*value != bak && pre_reaction) { swap(*value, bak); pre_reaction(); swap(*value, bak); }
    if(*value != bak && reaction) reaction();
    }
  bool load_from_animation(const string& s) override {
    if(anim_value != *value) return false;
    load(s);
    anim_value = *value;
    return true;
    }
  void load_as_animation(const string& s) override {
    load(s);
    anim_value = *value;
    anims::animate_parameter(this, s);
    }
  void swap_with(parameter *s) override { swap(*value, *(((val_parameter<T>*)s)->value)); }  
  };

struct float_parameter : public val_parameter<ld> {
  ld min_value, max_value, step;
  string unit;
  float_parameter *editable(ld min_value, ld max_value, ld step, string menu_item_name, string help_text, char key) {
    is_editable = true;
    this->min_value = min_value;
    this->max_value = max_value;
    this->menu_item_name = menu_item_name;
    menu_item_name_modified = true;
    this->help_text = help_text;
    this->step = step;
    default_key = key;
    return this;
    }
  function<void(float_parameter*)> modify_me;
  float_parameter *modif(const function<void(float_parameter*)>& r) { modify_me = r; return this; }
  void show_edit_option(key_type key) override;
  void load_from_raw(const string& s) override { *value = parseld(s); }
  cld get_cld() override { return *value; }
  void set_cld_raw(cld x) override { *value = real(x); }
  string save() override { return fts(*value, 10); }
  shared_ptr<parameter> clone(struct local_parameter_set& lps, void *value) override;
  };

struct float_parameter_dft : public float_parameter {
  void show_edit_option(key_type key) override;
  function<ld()> get_hint;
  float_parameter_dft* set_hint(const function<ld()>& f) { get_hint = f; return this; }
  };

struct int_parameter : public val_parameter<int> {
  int min_value, max_value;
  ld step;
  function<void(int_parameter*)> modify_me;
  int_parameter *modif(const function<void(int_parameter*)>& r) { modify_me = r; return this; }
  void show_edit_option(key_type key) override;
  int_parameter *editable(int min_value, int max_value, ld step, string menu_item_name, string help_text, char key) {
    this->is_editable = true;
    this->min_value = min_value;
    this->max_value = max_value;
    this->menu_item_name = menu_item_name;
    menu_item_name_modified = true;
    this->help_text = help_text;
    this->step = step;
    default_key = key;
    return this;
    }

  cld get_cld() override { return *value; }

  void load_from_raw(const string& s) override { *value = parseint(s); }
  void set_cld_raw(cld x) override { *value = (int)(real(x) + .5); }

  void check_change() override {
    if(*value != last_value) {
      last_value = *value;
      add_to_changed(this);
      }
    }

  string save() override { return its(*value); }
  shared_ptr<parameter> clone(struct local_parameter_set& lps, void *value) override;
  };

struct string_parameter: public val_parameter<string> {
  reaction_t editor;
  string save() override { return *value; }
  void load_from_raw(const string& s) override { *value = s; }
  void show_edit_option(key_type key) override;
  string_parameter* set_standard_editor(bool direct);
  string_parameter* set_file_editor(string ext);
  string_parameter* editable(string cap, string help, char key ) {
    is_editable = true;
    menu_item_name = cap;
    default_key = key;
    menu_item_name_modified = true;
    help_text = help;
    return this;
    }
  };

struct char_parameter : public val_parameter<char> {
  string save() override { return "\\" + its(*value); }
  void show_edit_option(key_type key) override;
  void load_from_raw(const string& s) override {
    if(s[0] == '\\' && s.size() > 1) *value = parseint(s.substr(1));
    else sscanf(s.c_str(), "%c", value);
    }
  };

struct bool_parameter : public val_parameter<bool> {
  string save() override { return (*value) ? "yes" : "no"; }
  reaction_t switcher;
  bool_parameter* editable(string cap, char key ) {
    is_editable = true;
    menu_item_name = cap; default_key = key;
    menu_item_name_modified = true;
    return this;
    }
  bool_parameter* help(string help) {
    help_text = help;
    return this;
    }
  void show_edit_option(key_type key) override;

  void load_from_raw(const string& s) override {
    if(s == "yes") *value = true;
    else if(s == "no") *value = false;
    else *value = parseint(s);
    }

  cld get_cld() override { return *value; }
  shared_ptr<parameter> clone(struct local_parameter_set& lps, void *value) override;
  };

struct color_parameter : public val_parameter<color_t> {
  bool has_alpha;
  void show_edit_option(key_type key) override;
  color_parameter *editable(string menu_item_name, string help_text, char key) {
    this->is_editable = true;
    this->menu_item_name = menu_item_name;
    menu_item_name_modified = true;
    this->help_text = help_text;
    default_key = key;
    return this;
    }

  cld get_cld() override { return has_alpha ? *value : (*value * 256 + 0xFF); }
  void load_from_raw(const string& s) override { 
     try {
       *value = parsecolor(s, has_alpha);
     } catch(hr_parse_exception& e) {
       sscanf(s.c_str(), "%x", value);
       }
     }
  string save() override { return has_alpha ? itsh8(*value) : itsh6(*value); }
  shared_ptr<parameter> clone(struct local_parameter_set& lps, void *value) override;
  };

/** transmatrix with equality, so we can construct val_parameter<matrix_eq> */
struct matrix_eq : transmatrix {
  bool operator == (const transmatrix& t) const {
    for(int i=0; i<MAXMDIM; i++) for(int j=0; j<MAXMDIM; j++)  if(self[i][j] != t[i][j]) return false;
    return true;
    }
  bool operator != (const transmatrix& t) const {
    return ! (self == t);
    }
  };

struct matrix_parameter : public val_parameter<matrix_eq> {

  void reset() override { *value = dft; }
  bool dosave() override { return !eqmatrix(*value, dft); }

  int dim;
  void show_edit_option(key_type key) override;
  matrix_parameter *editable(string menu_item_name, string help_text, char key) {
    this->is_editable = true;
    this->menu_item_name = menu_item_name;
    menu_item_name_modified = true;
    this->help_text = help_text;
    default_key = key;
    return this;
    }

  string save() override {
    shstream ss;
    print(ss, "matrix ");
    for(int a=0; a<4; a++) for(int b=0; b<4; b++) print(ss, (*value)[a][b], " ");
    return ss.s;
    }
  void load_from_raw(const string& s) override {
    if(s.substr(0, 7) == "matrix ") {
      shstream ss;
      ss.s = s.substr(7);
      for(int a=0; a<4; a++) for(int b=0; b<4; b++) scan(ss, (*value)[a][b]);
      }
    else {
      (transmatrix&)*value = parsematrix(s);
      }
    }
  };

struct custom_parameter : public parameter {
  cld last_value, anim_value;
  function<void(key_type)> custom_viewer;
  function<cld()> custom_value;
  function<bool(void*)> custom_affect;
  function<void(const string&)> custom_load;
  function<string()> custom_save;
  function<bool()> custom_do_save;
  reaction_t custom_reset;
  function<shared_ptr<parameter>(struct local_parameter_set& lps, void *value)> custom_clone;

  virtual shared_ptr<parameter> clone(struct local_parameter_set& lps, void *value) override {
    if(custom_clone) return custom_clone(lps, value);
    return parameter::clone(lps, value);
    }

  void show_edit_option(key_type key) override { custom_viewer(key); }
  bool affects(void *v) override { return custom_affect(v); }
  void check_change() override {
    if(custom_value() != last_value) {
      last_value = custom_value();
      add_to_changed(this);
      }
    }
  virtual void load_from_raw(const string& s) {
    if(!custom_load) {
      throw param_exception("parameter cannot be loaded", this);
      }
    custom_load(s);
    }
  void load(const string& s) override {
    auto bak = get_cld();
    load_from_raw(s);
    if(bak != get_cld() && reaction) reaction();
    }
  bool load_from_animation(const string& s) override {
    if(anim_value != get_cld()) return false;
    load(s);
    anim_value = get_cld();
    return true;
    }
  void load_as_animation(const string& s) override {
    load(s);
    anim_value = get_cld();
    anims::animate_parameter(this, s);
    }

  virtual cld get_cld() override { return custom_value(); }
  virtual string save() override { if(custom_save) return custom_save(); else return "not saveable"; }
  virtual bool dosave() override { if(custom_do_save) return custom_do_save(); else return false; }
  virtual void reset() override { if(custom_reset) custom_reset(); }
  virtual void swap_with(parameter*) override {}
  };
#endif

void non_editable() {
  dialog::addHelp("Warning: editing this value through this menu may not work correctly");
  }

void float_parameter::show_edit_option(key_type key) {
  if(modify_me) modify_me(this);
  dialog::addSelItem(XLAT(menu_item_name), fts(*value) + unit, key);
  if(*value == use_the_default_value) dialog::lastItem().value = XLAT("default");
  dialog::add_action([this] () {
    add_to_changed(this);
    dialog::editNumber(*value, min_value, max_value, step, dft, XLAT(menu_item_name), help_text); 
    if(sets) sets();
    if(reaction) dialog::get_di().reaction = reaction;
    if(!is_editable) dialog::get_di().extra_options = non_editable;
    });
  }

void float_parameter_dft::show_edit_option(key_type key) {
  if(modify_me) modify_me(this);
  dialog::addSelItem(XLAT(menu_item_name), fts(*value) + unit, key);
  if(*value == use_the_default_value) dialog::lastItem().value = XLAT("default: ") + fts(get_hint());
  dialog::add_action([this] () {
    add_to_changed(this);
    if(*value == use_the_default_value) *value = get_hint();
    dialog::editNumber(*value, min_value, max_value, step, dft, XLAT(menu_item_name), help_text);
    if(sets) sets();
    if(reaction) dialog::get_di().reaction = reaction;
    if(!is_editable) dialog::get_di().extra_options = non_editable;
    auto eo = dialog::get_di().extra_options;
    dialog::get_di().extra_options = [eo, this] {
      dialog::addSelItem(XLAT("use the default value"), "", 'D');
      dialog::add_action([this] { *value = use_the_default_value; });
      if(eo) eo();
      };
    });
  }

void int_parameter::show_edit_option(key_type key) {
  if(modify_me) modify_me(this);
  dialog::addSelItem(XLAT(menu_item_name), its(*value), key);
  dialog::add_action([this] () {
    add_to_changed(this);
    dialog::editNumber(*value, min_value, max_value, step, dft, XLAT(menu_item_name), help_text); 
    if(sets) sets();
    if(reaction) dialog::get_di().reaction = reaction;
    if(!is_editable) dialog::get_di().extra_options = non_editable;
    });
  }

void bool_parameter::show_edit_option(key_type key) {
  dialog::addBoolItem(XLAT(menu_item_name), *value, key);
  if(is_highlight(dialog::items.back()) && help_text != menu_item_name) mouseovers = XLAT(help_text);
  dialog::add_action([this] () {
    if(use_bool_dialog || hiliteclick) {
      auto& bd = dialog::editBool(*value, dft, XLAT(menu_item_name), XLAT(help_text), switcher);
      if(sets) sets();
      if(reaction) bd.reaction = reaction;
      if(!is_editable) bd.extra_options = non_editable;
      }
    else {
      add_to_changed(this);
      switcher(); if(sets) sets();
      if(reaction) reaction();
      }
    });
  }

void color_parameter::show_edit_option(key_type key) {
  dialog::addColorItem(XLAT(menu_item_name), has_alpha ? *value : addalpha(*value), key);
  dialog::add_action([this] () {
    dialog::openColorDialog(*value);
    dialog::colorAlpha = has_alpha;
    dialog::get_di().dialogflags |= sm::SIDE;
    });
  }

void matrix_parameter::show_edit_option(key_type key) {
  dialog::addMatrixItem(XLAT(menu_item_name), *value, key, dim);
  dialog::add_action([this] () {
    dialog::editMatrix(*value, XLAT(menu_item_name), help_text, dim);
    if(sets) sets();
    if(reaction) dialog::get_di().reaction = reaction;
    });
  }

void char_parameter::show_edit_option(key_type key) {
  string s = s0; s += *(value);
  dialog::addSelItem(XLAT(menu_item_name), s, key);
  }

void string_parameter::show_edit_option(key_type key) {
  dialog::addSelItem(XLAT(menu_item_name), *value, key);
  if(!editor) {
    if(is_highlight(dialog::items.back())) mouseovers = XLAT("not editable");
    }
  else dialog::add_action(editor);
  }

string_parameter* string_parameter::set_standard_editor(bool direct) {
  shared_ptr<string> bak = make_shared<string>();
  editor = [this, bak, direct] {
    *bak = *value;
    dialog::edit_string(direct ? *value : *bak, menu_item_name, help_text);
    dialog::get_di().reaction = [direct, this, bak] {
      if(!direct) if(pre_reaction) pre_reaction();
      if(!direct) *value = *bak;
      if(reaction) reaction();
      };
    if(sets) sets();
    };
  return this;
  }

string_parameter* string_parameter::set_file_editor(string ext) {
  shared_ptr<string> bak = make_shared<string>();
  editor = [this, bak, ext] {
    *bak = *value;
    dialog::openFileDialog(*bak, menu_item_name, ext, [this, bak] {
      if(pre_reaction) pre_reaction();
      *value = *bak;
      if(reaction) reaction();
      return true;
      });
    };
  return this;
  }

void parameter::setup(const parameter_names& n) {
  name = n.name;
  legacy_config_name = n.legacy_config_name;
  menu_item_name = n.legacy_config_name;
  menu_item_name_modified = false;
  if(params.count(name)) println(hlog, "ERROR: repeated parameter name ", name);
  params[name] = shared_from_this();
  }

EX shared_ptr<float_parameter> param_f(ld& val, const parameter_names& n, ld dft) {
  shared_ptr<float_parameter> u ( new float_parameter );
  u->setup(n);
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
  return u;
  }

EX shared_ptr<float_parameter_dft> param_fd(ld& val, const parameter_names& n, ld dft IS(use_the_default_value) ) {
  shared_ptr<float_parameter_dft> u ( new float_parameter_dft );
  u->setup(n);
  u->value = &val;
  u->last_value = dft;
  u->min_value = -100;
  u->max_value = +100;
  u->step = 1;
  u->dft = dft;
  val = dft;
  return u;
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

EX shared_ptr<int_parameter> param_i(int& val, const parameter_names& n, int dft) {
  shared_ptr<int_parameter> u ( new int_parameter );
  u->setup(n);
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
  return u;
  }

EX shared_ptr<int_parameter> param_i(int& val, const parameter_names& n) { return param_i(val, n, val); }

EX shared_ptr<bool_parameter> param_b(bool& val, const parameter_names& n, bool dft) {
  shared_ptr<bool_parameter> u ( new bool_parameter );
  u->setup(n);
  u->value = &val;
  u->last_value = dft;
  u->dft = dft;
  u->switcher = [&val] { val = !val; };
  val = dft;
  return u;
  }

EX shared_ptr<color_parameter> param_color(color_t& val, const parameter_names& n, bool has_alpha, color_t dft) {
  shared_ptr<color_parameter> u ( new color_parameter );
  u->setup(n);
  u->value = &val;
  u->last_value = dft;
  u->dft = dft;
  u->has_alpha = has_alpha;
  val = dft;
  return u;
  }

EX shared_ptr<matrix_parameter> param_matrix(transmatrix& val0, const parameter_names& n, int dim) {
  matrix_eq& val = (matrix_eq&) val0;
  shared_ptr<matrix_parameter> u ( new matrix_parameter );
  u->setup(n);
  u->value = &val;
  u->last_value = val;
  u->dft = val;
  u->dim = dim;
  return u;
  }

EX shared_ptr<char_parameter> param_char(char& val, const parameter_names& n, char dft) {
  shared_ptr<char_parameter> u ( new char_parameter );
  u->setup(n);
  u->value = &val;
  u->last_value = dft;
  u->dft = dft;
  val = dft;
  return u;
  }

EX shared_ptr<string_parameter> param_str(string& val, const parameter_names& n, const string dft) {
  shared_ptr<string_parameter> u ( new string_parameter );
  u->setup(n);
  u->value = &val;
  u->last_value = val;
  u->dft = dft;
  val = dft;
  return u;
  }

EX shared_ptr<string_parameter> param_str(string& val, const parameter_names& n) { return param_str(val, n, val); }

EX shared_ptr<color_parameter> param_color(color_t& val, const parameter_names& n, bool has_alpha) { return param_color(val, n, has_alpha, val); }

EX shared_ptr<bool_parameter> param_b(bool& val, const parameter_names& n) { return param_b(val, n, val); }

#if HDR
template<class T> shared_ptr<enum_parameter<T>> param_enum(T& val, const parameter_names& n, T dft) {
  shared_ptr<enum_parameter<T>> u ( new enum_parameter<T> );
  u->setup(n);
  u->value = &val;
  u->dft = dft;
  val = dft;
  u->last_value = dft;
  auto f = &*u;
  u->set_value_to = [f] (T val) { *f->value = val; };
  return u;
  }

template<class T> shared_ptr<enum_parameter<T>> param_enum(T& val, const parameter_names& n) { return param_enum(val, n, val); }

template<class T> shared_ptr<parameter> enum_parameter<T>::clone(struct local_parameter_set& lps, void *value) { return param_enum(*(T*) value, lps.mod(this), *(T*) value); }
#endif

EX shared_ptr<float_parameter> param_f(ld& val, const parameter_names& n) {
  return param_f(val, n, val);
  }

parameter_names local_parameter_set::mod(parameter *t) {
  return parameter_names(label + t->name, label + t->legacy_config_name);
  }

shared_ptr<parameter> int_parameter::clone(struct local_parameter_set& lps, void *value) {
  auto val = (int*) value; 
  return param_i(*val, lps.mod(this), *val);
  }

shared_ptr<parameter> bool_parameter::clone(struct local_parameter_set& lps, void *value) {
  auto val = (bool*) value;
  return param_b(*val, lps.mod(this), *val);
  }

shared_ptr<parameter> color_parameter::clone(struct local_parameter_set& lps, void *value) {
  auto val = (color_t*) value;
  return param_color(*val, lps.mod(this), has_alpha, *val);
  }

shared_ptr<parameter> float_parameter::clone(struct local_parameter_set& lps, void *value) {
  auto val = (ld*) value;
  return param_f(*val, lps.mod(this), *val);
  }

#if HDR
template<class T>
shared_ptr<custom_parameter> param_custom_int(T& val, const parameter_names& n, function<void(key_type)> menuitem, char key) {
  shared_ptr<custom_parameter> u ( new custom_parameter );
  u->setup(n);
  int dft = (int) val;
  u->last_value = dft;
  u->custom_viewer = menuitem;
  u->custom_value = [&val] () { return (int) val; };
  u->custom_affect = [&val] (void *v) { return &val == v; };
  u->custom_load = [&val] (const string& s) { val = (T) parseint(s); };
  u->custom_save = [&val] { return its(int(val)); };
  u->custom_do_save = [dft, &val] { return int(val) != dft; };
  u->custom_clone = [u] (struct local_parameter_set& lps, void *value) { auto val = (int*) value; return param_i(*val, lps.mod(&*u), *val); };
  u->custom_reset = [dft, &val] { val = (T) dft; };
  u->default_key = key;
  u->is_editable = true;
  return u;
  }
#endif

EX shared_ptr<custom_parameter> param_custom_ld(ld& val, const parameter_names& n, function<void(key_type)> menuitem, char key) {
  shared_ptr<custom_parameter> u ( new custom_parameter );
  u->setup(n);
  ld dft = val;
  u->last_value = dft;
  u->custom_viewer = menuitem;
  u->custom_value = [&val] () { return val; };
  u->custom_affect = [&val] (void *v) { return &val == v; };
  u->custom_load = [&val] (const string& s) { val = parseld(s); };
  u->custom_save = [&val] { return fts(val, 10); };
  u->custom_do_save = [dft, &val] { return val != dft; };
  u->custom_clone = [u] (struct local_parameter_set& lps, void *value) { auto val = (ld*) value; return param_f(*val, lps.mod(&*u), *val); };
  u->custom_reset = [dft, &val] { val = dft; };

  u->default_key = key;
  u->is_editable = true;
  return u;
  }

EX shared_ptr<custom_parameter> param_colortable(colortable& val, const parameter_names& n) {
  shared_ptr<custom_parameter> u ( new custom_parameter );
  u->setup(n);
  colortable dft = val;
  u->last_value = -1;
  u->custom_viewer = [] (char key) {};
  u->custom_value = [] () { return -1; };
  u->custom_affect = [&val] (void *v) { return &val == v; };
  u->custom_load = [&val] (const string& s) {
    val = parsecolortable(s);
    };
  u->custom_save = [&val] {
    bool first = true;
    string str;
    for(auto v: val) { if(first) first = false; else str += ","; str += itsh(v); }
    return str;
    };
  u->custom_do_save = [dft, &val] { return val != dft; };
  u->custom_clone = [u] (struct local_parameter_set& lps, void *value) { auto val = (colortable*) value; return param_colortable(*val, lps.mod(&*u)); };
  u->custom_reset = [dft, &val] { val = dft; };

  u->default_key = 0;
  u->is_editable = true;
  return u;
  }

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

struct charstyle_prebow {
  int charid;
  color_t skincolor, haircolor, dresscolor, swordcolor, dresscolor2, uicolor, eyecolor;
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
    cs.swordcolor = cs.eyecolor = cs.bowcolor = cs.bowcolor2 = cso.swordcolor;
    if(cs.charid < 4) cs.eyecolor = 0;
    cs.dresscolor2 = cso.dresscolor2;
    cs.uicolor = cso.uicolor;
    cs.lefthanded = cso.lefthanded;
    }
  else if(hs.get_vernum() < 0xA938) {
    charstyle_prebow cso;
    hread_raw(hs, cso);
    cs.charid = cso.charid;
    cs.skincolor = cso.skincolor;
    cs.haircolor = cso.haircolor;
    cs.dresscolor = cso.dresscolor;
    cs.eyecolor = cso.eyecolor;
    cs.swordcolor = cs.bowcolor = cs.bowcolor2 = cso.swordcolor;
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

EX void paramset(charstyle& cs, string s) {
  param_i(cs.charid, s + ".charid");
  param_color(cs.skincolor, s + ".skincolor", true, cs.skincolor);
  param_color(cs.eyecolor, s + ".eyecolor", true, cs.eyecolor);
  param_color(cs.bowcolor, s + ".bowcolor", true, cs.bowcolor);
  param_color(cs.bowcolor2, s + ".bowcolor2", true, cs.bowcolor2);
  param_color(cs.haircolor, s + ".haircolor", true, cs.haircolor);
  param_color(cs.dresscolor, s + ".dresscolor", true, cs.dresscolor);
  param_color(cs.swordcolor, s + ".swordcolor", true, cs.swordcolor);
  param_color(cs.dresscolor2, s + ".dresscolor2", true, cs.dresscolor2);
  param_color(cs.uicolor, s + ".uicolor", true, cs.uicolor);
  param_b(cs.lefthanded, s + ".lefthanded");
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
  cs.bowcolor   = 0x603000FF;
  cs.bowcolor2  = 0xFFD500FF;
  cs.lefthanded = false;
  }

EX purehookset hooks_configfile;

EX ld mapfontscale = 100;

EX void font_reaction() {
  if(among(font_id, 5, 6)) {
    int fid = font_id;
    font_id = last_font_id;
    dialog::openFileDialog(font_filenames[fid], XLAT("font to use:"), fid == 5 ? ".ttf" : ".otf", [fid] () {
      font_id = fid; return true;
      });
    }
  }

EX void initConfig() {
  
  // basic config
  param_i(vid.flashtime, "flashtime", 8);

  param_i(vid.msglimit, "message limit", 5);
  param_i(vid.timeformat, "message log time format", 0);
  
  param_b(resizable, "resizable", true)
  -> editable("resizable window", 'r')
  -> help("This lets your operating system resize the window.");

  param_b(no_find_player, "no_find_player")
  -> help("Do not show the 'find player' button even if the player seems to be lost.");
  param_b(game_keys_scroll, "game_keys_scroll")
  -> editable("pure exploration (game keys scroll)", 'P')
  -> help("Enables easier control of the camera for watching visualizations (when you are not playing the game).");
  param_b(reg3::cubes_reg3, "cubes_reg3");
  param_f(linepatterns::tree_starter, "tree_starter")
  -> editable(0, 1, 0.05, "tree-drawing parameter", "How much of edges to draw for tree patterns (to show how the tree edges are oriented).", 't');

  // param_char(patterns::whichCanvas, "whichCanvas", 0); %TODO
  param_f(ccolor::rwalls, "randomwalls");
  param_b(patterns::innerwalls, "innerwalls")
  -> help("In 3D modes, display all the walls, including the walls between adjacent wall tiles.");

  param_b(use_bool_dialog, "use_bool_dialog")
  ->editable("on/off dialog/help", 'B')
  ->help(
     "Enable this for a full dialog when editing some on/off settings (otherwise, the dialog is not shown, we just switch). "
     "This lets you see an explanation of what the setting does. "
     "You can also press ALT while changing such settings.");

  param_b(vid.grid, "grid");
  param_b(models::desitter_projections, "desitter_projections", false);
  param_b(nonisotropic_weird_transforms, "nonisotropic_weird_transforms", false);

  param_b(arb::apeirogon_consistent_coloring, "apeirogon_consistent_coloring", true)
  -> editable("apeirogon_consistent_coloring", 'c')
  -> help("In arbitrary tilings, apeirogons are internally represented as multiple tiles. This option ensures that all subtiles have the same color.");
  param_b(arb::apeirogon_hide_grid_edges, "apeirogon_hide_grid_edges", true)
  -> editable("apeirogon_hide_grid_edges", 'h')
  -> help("In arbitrary tilings, apeirogons are internally represented as multiple tiles. This option hides the subtile edges.");
  param_b(arb::apeirogon_simplified_display, "apeirogon_simplified_display", false)
  -> editable("simplified display of apeirogons", 'f')
  -> help("Connect the ends of the apeirogon segment with the boundary point using straight lines. This should be faster and, in most cases, actually more correct.");
  param_b(arb::convert::minimize_on_convert, "tes_minimize_on_convert", false)
  -> editable("consider all symmetries when converting", 'm');
  param_b(arb::convert::reverse_order, "tes_reverse_order", false)
  -> editable("tes reverse order on convert", 'r');

  param_b(display_yasc_codes, "yasc", false)
  -> editable("YASC codes", 'Y')
  -> help("YASC codes: Sides-Entity-Restrict-Threat-Wall");

  param_b(vid.relative_font, "relative_font", true)
  -> editable("set relative font size", 'r')
  -> help("Font size is set as a relation to screen size.")
  -> set_reaction(compute_fsize);
  param_i(vid.fontscale, "fontscale", 100)
  -> editable(25, 400, 10, "font scale", "", 'b')
  -> set_reaction(compute_fsize)
  -> set_sets([] { dialog::bound_low(0); });

  param_f(mapfontscale, "mapfontscale", 100)
  -> editable(-400, 400, 10, "map font scale",
      "This affects the size of the characters on the ASCII map. This includes ASCII walls/monster display mode, the minimap, minefield values, and various debug features.", 'B')
  ->set_extra([] {
    dialog::get_di().extra_options = [] () { draw_radar(true); };
    });

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
  param_i(vid.steamscore, "scores to Steam", 1);
  initcs(vid.cs); paramset(vid.cs, "single");
  param_b(vid.samegender, "princess choice", false);
  param_i(vid.language, "language", -1);  
  param_enum(font_id, "font_id", 0)
  ->editable(font_names, "select font", 'f')
  ->manual_reaction = font_reaction;
  param_str(font_filenames[5], "ttf_font");
  param_str(font_filenames[6], "otf_font");
  param_b(vid.drawmousecircle, "mouse circle", ISMOBILE || ISPANDORA);
  param_b(vid.revcontrol, "reverse control", false);
  #if CAP_AUDIO
  param_i(musicvolume, "music volume")
  ->editable(0, 128, 10, "background music volume", "", 'b')
  ->set_sets(sets_music_volume);
  #endif
  #if CAP_SDLAUDIO
  param_b(music_out_of_focus, "music out of focus", false);
  #endif
  #if CAP_AUDIO
  param_i(effvolume, "sound effect volume")
  ->editable(0, 128, 10, "sound effects volume", "", 'e')
  ->set_sets(sets_sfx_volume);
  #endif

  param_enum(vid.faraway_highlight, parameter_names("faraway_highlight", "highlight faraway monsters"), tlNoThreat)
    ->editable({{"off", ""}, {"spam", ""}, {"normal monsters", ""}, {"high-threat monsters only", ""}}, "highlight faraway monsters", 'h');

  param_i(vid.faraway_highlight_color, "faraway_highlight_color", 50)
  -> editable(0, 100, 10, "faraway highlight color", "0 = monster color, 100 = red-light oscillation", 'c');

  param_enum(glyphsortorder, parameter_names("glyph_sort", "glyph sort order"), glyphsortorder)
    ->editable({
      {"first on top", ""},
      {"first on bottom", ""},
      {"last on top", ""},
      {"last on bottom", ""},
      {"by land", ""},
      {"by number", ""}
      }, "inventory/kill sorting", 'k');

  param_enum(vid.orbmode, parameter_names("orb_mode", "orb display mode"), 2)
    ->editable({
      {"plain", ""},
      {"types", ""},
      {"icons", ""},
      }, "orb display mode", 'o');

  param_b(less_in_landscape, "less_in_landscape", false)
  ->editable("less items/kills in landscape", 'L')
  -> help("If set, only the important items and kills will be shown")
  -> set_reaction([] { vid.killreduction = 0; });

  param_b(less_in_portrait, "less_in_portrait", false)
  ->editable("less items/kills in portrait", 'P')
  -> help("If set, only the important items and kills will be shown")
  -> set_reaction([] { vid.killreduction = 0; });
  
  // basic graphics
  
  param_b(vid.wantGL, "usingGL", true)
  ->editable("openGL mode", 'o');
  
  param_i(vid.want_antialias, "antialias", AA_NOGL | AA_FONT | (ISWEB ? AA_MULTI : AA_LINES) | AA_VERSION);
  param_b(vid.fineline, "fineline", true)->help("Disable this to make all line widths 1.");
  param_f(vid.linewidth, "linewidth", 1);
  param_f(precise_width, "precisewidth", .5);
  param_i(perfect_linewidth, "perfect_linewidth", 1);
  param_f(linepatterns::width, parameter_names("lpwidth", "pattern-linewidth"), 1);
  param_b(fat_edges, "fat-edges")->help("When this setting is ON, grid lines and various line patterns are drawn as pipes.");
  param_f(vid.sspeed, parameter_names("sspeed", "scrollingspeed"), 0);
  param_f(vid.mspeed, parameter_names("mspeed", "movement speed"), 1);
  param_f(vid.ispeed, parameter_names("ispeed", "idle speed"), 1);
  param_i(vid.aurastr, "aura strength", ISMOBILE ? 0 : 128);
  param_i(vid.aurasmoothen, "aura smoothen", 5);
  param_enum(vid.graphglyph, parameter_names("graphglyph", "graphical items/kills"), 1)
  -> editable({{"letters", ""}, {"auto", ""}, {"images", ""}}, "inventory/kill mode", 'd');

  param_i(min_cells_drawn, "min_cells_drawn");

  param_str(menu_format, "menu_format", "")
  ->set_standard_editor(true)
  ->editable("menu line format",
     "Displays an arbitrary text instead of menu. "
     "You can use e.g. $(turncount) or $(gametime,2) "
     "to display the values of parameters and statistics.",
     'T')
  ->set_extra([] {
    dialog::addSelItem(XLAT("default"), "", SDLK_F1);
    dialog::add_action([] { menu_format = ""; popScreen(); });
    dialog::addSelItem(XLAT("show turn count"), "", SDLK_F2);
    dialog::add_action([] { menu_format = "t:$(turncount)"; popScreen(); });
    dialog::addSelItem(XLAT("testing"), eval_programmable_string(menu_format), 0);
    });
  param_i(menu_darkening, "menu_darkening", 2)
  -> editable(0, 8, 1, "menu map darkening", "A larger number means darker game map in the background. Set to 8 to disable the background.", 'd')
  -> set_sets([] { dialog::bound_low(0); dialog::bound_up(8); dialog::get_di().dialogflags |= sm::DARKEN; });
  param_b(centered_menus, "centered_menus", false)
  -> editable("centered menus in widescreen", 'c');

  param_b(startanims::enabled, "startanim", true)
  -> editable("start animations", 's');

  param_b(vid.flasheffects, "flasheffects", 1)
  -> editable("flashing effects", 'h')
  -> help("Disable if you are photosensitive. Replaces flashing effects such as Orb of Storms lightning with slow, adjustable animations.");

  vid.binary_width = 1;
  param_custom_ld(vid.binary_width, "binary tiling width", menuitem_binary_width, 'v');

  param_b(fake::multiple_special_draw, "fake_multiple", true);

  param_f(hat::hat_param, "hat_param", 1)
  -> editable(0, 2, 0.1, "hat/spectre/turtle parameter",
    "Apeirodic hat tiling based on: https://arxiv.org/pdf/2303.10798.pdf\n\n"
    "This controls the parameter discussed in Section 6. Parameter p is Tile(p, (2-p)√3), scaled so that the area is the same for every p.\n\n"
    "Aperiodic spectre tiling based on: https://arxiv.org/abs/2305.17743\n\n"
    "In the spectre tiling, set the parameter to 'spectre' value to make all tiles have the same shape."
    ,
    'v'
    )
  -> set_extra([] {
      dialog::addSelItem(XLAT("chevron (periodic)"), "0", 'C');
      dialog::add_action([] { dialog::get_ne().s = "0"; dialog::get_ne().apply_edit(); });
      dialog::addSelItem(XLAT("hat"), "1", 'H');
      dialog::add_action([] { dialog::get_ne().s = "1"; dialog::get_ne().apply_edit(); });
      dialog::addSelItem(XLAT("spectre"), "3-√3", 'T');
      dialog::add_action([] { dialog::get_ne().s = "3 - sqrt(3)"; dialog::get_ne().apply_edit(); });
      dialog::addSelItem(XLAT("turtle"), "1.5", 'T');
      dialog::add_action([] { dialog::get_ne().s = "1.5"; dialog::get_ne().apply_edit(); });
      dialog::addSelItem(XLAT("comma (periodic)"), "2", ',');
      dialog::add_action([] { dialog::get_ne().s = "2"; dialog::get_ne().apply_edit(); });
      })
  -> set_reaction(hat::reshape);

  param_f(hat::hat_param_imag, "hat_param_imag", 0)
  -> editable(0, 2, 0.1, "hat parameter (imaginary)",
    "Imaginary part of the hat parameter. This corresponds to the usual interpretation of complex numbers in Euclidean planar geometry: rather than shortened or lengthened, the edges are moved in the other dimension.", 'v'
    )
  -> set_reaction(hat::reshape);

  param_b(vid.particles, "extra effects", 1)
  -> help("Disable if you do not want particle effects and similar.");
  param_i(vid.framelimit, "frame limit", 999);

  #if !ISMOBWEB
  param_b(vid.want_vsync, "vsync", true)
  -> help("Disable if you want to see the actual framerate rendered by the engine.")
  ->editable("vsync", 'v');
  #endif
  
  param_b(vid.want_fullscreen, "fullscreen", false)
  ->editable("fullscreen mode", 'f');
  param_b(vid.change_fullscr, "fullscreen_change", false)
  ->editable("use specific fullscreen resolution", 'g');
  param_b(vid.relative_window_size, "window_relative", true)
  ->editable("specify relative window size", 'g');

  param_custom_int(vid.xres, "xres", [] (key_type ch) {}, 0)->restrict = return_false;
  param_custom_int(vid.yres, "yres", [] (key_type ch) {}, 0)->restrict = return_false;
  
  param_i(vid.fullscreen_x, "fullscreen_x", 1280)
  -> editable(640, 3840, 640, "fullscreen resolution to use (X)", "", 'x')
  -> set_sets([] { dialog::bound_low(640); dialog::get_di().reaction_final = do_request_resolution_change; });
  
  param_i(vid.fullscreen_y, "fullscreen_y", 1024)
  -> editable(480, 2160, 480, "fullscreen resolution to use (Y)", "", 'x')
  -> set_sets([] { dialog::bound_low(480); dialog::get_di().reaction_final = do_request_resolution_change; });

  param_i(vid.window_x, "window_x", 1280)
  -> editable(160, 3840, 160, "window resolution to use (X)", "", 'x')
  -> set_sets([] { dialog::bound_low(160); dialog::get_di().reaction_final = do_request_resolution_change; });

  param_i(vid.window_y, "window_y", 1024)
  -> editable(120, 2160, 120, "window resolution to use (Y)", "", 'x')
  -> set_sets([] { dialog::bound_low(120); dialog::get_di().reaction_final = do_request_resolution_change; });

  param_f(vid.window_rel_x, "window_rel_x", .9)
  -> editable(.1, 1, .1, "screen size percentage to use (X)", "", 'x')
  -> set_sets([] { dialog::bound_low(.1); dialog::get_di().reaction_final = do_request_resolution_change; });

  param_f(vid.window_rel_y, "window_rel_y", .9)
  -> editable(.1, 1, .1, "screen size percentage to use (Y)", "", 'x')
  -> set_sets([] { dialog::bound_low(.1); dialog::get_di().reaction_final = do_request_resolution_change; });

  param_b(vid.darkhepta, "mark heptagons", false);
  
  param_b(logfog, "logfog", false);

  for(auto& lp: linepatterns::patterns) {
    param_color(lp->color, "lpcolor-" + lp->lpname, true, lp->color);
    param_f(lp->multiplier, "lpwidth-" + lp->lpname);
    }
  
  // special graphics

  param_i(vid.monmode, "monster display mode", DEFAULT_MONMODE);
  param_i(vid.wallmode, "wall display mode", DEFAULT_WALLMODE);
  param_i(vid.highlightmode, "highlightmode");

  param_b(vid.always3, "3D always", false)->switcher = geom3::switch_fpp;

  param_f(geom3::euclid_embed_scale, "euclid_embed_scale")
  -> editable(0, 2, 0.05, "Euclidean embedding scale", "How to scale the Euclidean map, relatively to the 3D absolute unit.", 'X')
  -> set_sets([] { dialog::bound_low(0.05); })
  -> set_reaction(geom3::apply_settings_light);

  param_f(geom3::euclid_embed_scale_y, "euclid_embed_scale_y")
  -> editable(0, 2, 0.05, "Euclidean embedding scale Y/X", "This scaling factor affects only the Y coordinate.", 'Y')
  -> set_sets([] { dialog::bound_low(0.05); })
  -> set_reaction(geom3::apply_settings_light);

  param_f(geom3::euclid_embed_rotate, "euclid_embed_rotate")
  -> editable(0, 360, 15, "Euclidean embedding rotation", "How to rotate the Euclidean embedding, in degrees.", 'F')
  -> set_reaction(geom3::apply_settings_light);

  param_enum(embedded_shift_method_choice, "embedded_shift_method", smcBoth)
  -> editable({
    {"geodesic", "always move on geodesics"},
    {"keep levels", "keep the vertical angle of the camera"},
    {"mixed", "on geodesics when moving camera manually, keep level when auto-centering"}
    }, "view shift for embedded planes", 'H');

  param_b(geom3::auto_configure, "auto_configure_3d")
  -> editable("set 3D settings automatically", 'A');

  param_b(geom3::inverted_embedding, "inverted_3d", false)
  -> editable("invert convex/concave", 'I')
  -> set_reaction(geom3::apply_settings_full);

  param_b(geom3::flat_embedding, "flat_3d", false)
  -> editable("flat, not equidistant", 'F')
  -> set_reaction(geom3::apply_settings_full);

  param_enum(geom3::spatial_embedding, "spatial_embedding", geom3::seDefault)
  ->editable(geom3::spatial_embedding_options, "3D embedding method", 'E')
  ->set_reaction(geom3::apply_settings_full);
  
  param_b(memory_saving_mode, "memory_saving_mode", (ISMOBILE || ISPANDORA || ISWEB) ? 1 : 0);
  param_i(reserve_limit, "memory_reserve", 128);
  param_b(show_memory_warning, "show_memory_warning");

  param_b(rug::renderonce, "rug-renderonce");
  param_b(rug::rendernogl, "rug-rendernogl");
  param_i(rug::texturesize, "rug-texturesize");
#if CAP_RUG
  param_f(rug::model_distance, "rug-model-distance");
#endif

  param_b(vid.backeffects, "background particle effects", (ISMOBILE || ISPANDORA) ? false : true)
  -> help("Background particle effects, e.g., in the Blizzard.");
  // control
  
  param_enum(joy_init, "joyinit", jiFast)
    ->editable({{"off", "do not use joysticks"}, {"fast", "do not wait until the joysticks are initialized"}, {"wait", "wait until the joysticks are initialized"}}, "joystick initialization", 'j');
  param_i(vid.joyvalue, "vid.joyvalue", 4800);
  param_i(vid.joyvalue2, "vid.joyvalue2", 5600);
  param_i(vid.joysmooth, "vid.joysmooth", 200);
  param_i(vid.joypanthreshold, "vid.joypanthreshold", 2500);
  param_f(vid.joypanspeed, "vid.joypanspeed", ISPANDORA ? 0.0001 : 0);
  param_b(autojoy, "autojoy");
    
  vid.killreduction = 0;
  
  param_b(vid.skipstart, "skip the start menu", false);
  param_b(vid.quickmouse, "quick mouse", !ISPANDORA)
  -> help("Buttons activate when they are pressed (by clicking), not when they are released.");
  
  // colors

  param_f(crosshair_size, "size:crosshair")
  ->set_extra(draw_crosshair);
  param_color(crosshair_color, "color:crosshair", true, crosshair_color)
  ->set_extra(draw_crosshair);
  
  param_b(mapeditor::drawplayer, "drawplayer");

  param_color(backcolor, "color:background", false);
  param_color(forecolor, "color:foreground", false);
  param_color(bordcolor, "color:borders", false);
  param_color(ringcolor, "color:ring", true);
  param_f(vid.multiplier_ring, parameter_names("mring", "mult:ring"), 1);
  param_color(modelcolor, "color:model", true);
  param_color(periodcolor, "color:period", true);
  param_color(stdgridcolor, "color:stdgrid", true);
  param_f(vid.multiplier_grid, parameter_names("mgrid", "mult:grid"), 1);
  param_color(dialog::dialogcolor, "color:dialog", false);
  for(auto p: ccolor::all)
    param_colortable(p->ctab, s0+"canvas:"+p->name);
  param_colortable(distcolors, "distance");
  param_colortable(minecolors, "mines");
  #if CAP_COMPLEX2
  param_colortable(brownian::colors, "color:brown");
  #endif

  param_colortable(prairie_colors, "color:prairie");
  param_colortable(mountain_colors, "color:mountain");
  param_colortable(tower_colors, "color:tower");
  param_colortable(westwall_colors, "color:freefall");
  param_colortable(endorian_colors, "color:endorian");
  param_colortable(canopy_colors, "color:canopy");
  param_colortable(camelot_cheat_colors, "color:camelotcheat");

  for(int i=0; i<motypes; i++)
    param_color(minf[i].color, "color:monster:" + its(i), false);
  for(int i=0; i<ittypes; i++)
    param_color(iinf[i].color, "color:item:" + its(i), false);
  for(int i=0; i<landtypes; i++)
    param_color(floorcolors[i], "color:land:" + its(i), false);
  for(int i=0; i<walltypes; i++)
    param_color(winf[i].color, "color:wall:" + its(i), false);

  // modes
    
  param_b(shmup::on, "mode-shmup", false)->be_non_editable();
  param_b(hardcore, "mode-hardcore", false)->set_reaction([] { hardcore = !hardcore; switchHardcore_quiet(); });
  param_enum(land_structure, "mode-chaos", lsNiceWalls)->be_non_editable();
  #if CAP_INV
  param_b(inv::on, "mode-Orb Strategy")->be_non_editable();
  #endif
  param_b(peace::on, "mode-peace")->be_non_editable();
  param_b(peace::otherpuzzles, "mode-peace-submode")->be_non_editable();
  param_enum(specialland, parameter_names("specialland", "land for special modes"), specialland)->be_non_editable();
  
  param_b(viewdists, "expansion mode");
  param_f(backbrightness, parameter_names("back", "brightness behind sphere"));
  param_b(auto_extend, "expansion_auto_extend")
  -> editable("extend automatically", 'E');

  param_f(vid.ipd, parameter_names("ipd", "interpupilar-distance"), 0.05);
  param_f(vid.lr_eyewidth, parameter_names("lr", "eyewidth-lr"), 0.5);
  param_f(vid.anaglyph_eyewidth, parameter_names("anaglyph", "eyewidth-anaglyph"), 0.1);
  param_f(vid.fov, parameter_names("fov", "field-of-vision"), 90);
  param_i(vid.desaturate, "desaturate", 0);
  
  param_enum(vid.stereo_mode, "stereo-mode", vid.stereo_mode)
    ->editable({
    {"OFF", "linear perspective"}, {"anaglyph", "for red-cyan glasses"}, {"side-by-side", "for mobile VR"},
    {"ODS", "for rendering 360° VR videos (implemented only in raycaster and some other parts)"},
    {"Panini", "Projection believed to be used by Italian painters. Allows very high FOV angles while rendering more straight lines as straight than the stereographic projection."},
    {"stereographic", "Stereographic projection allows very high FOV angles."},
    {"equirectangular", "for rendering 360° videos (implemented only in raycaster)"},
    {"cylindrical", "full vertical (not implemented in raycaster)"}
    }, "stereo/high-FOV mode", 'm')
  ->set_reaction(reset_all_shaders);

  param_f(vid.plevel_factor, "plevel_factor", 0.7);

  param_b(nohud, "no-hud", false);
  param_b(nomap, "nomap", false);
  param_b(nofps, "no-fps", false);
  
  #if CAP_IRR
  param_f(irr::density, "irregular-density", 2);
  param_i(irr::cellcount, "irregular-cellcount", 150);
  param_f(irr::quality, "irregular-quality", .2);
  param_i(irr::place_attempts, "irregular-place", 10);
  param_i(irr::rearrange_max_attempts, "irregular-rearrange-max", 50);
  param_i(irr::rearrange_less, "irregular-rearrangeless", 10);
  #endif
  
  param_i(vid.linequality, "line quality", 0);
  
  #if CAP_FILES && CAP_SHOT && CAP_ANIMATIONS
  param_str(anims::animfile, "animation file format")->set_file_editor(".png");
  #endif

  #if CAP_RUG
  param_f(rug::move_on_touch, "rug move on touch");
  #endif
  
  #if CAP_CRYSTAL
  param_f(crystal::compass_probability, parameter_names("cprob", "compass-probability"));
  param_b(crystal::view_coordinates, "crystal-coordinates");
  #endif
  
#if CAP_TEXTURE  
  param_b(texture::texture_aura, "texture-aura", false);
#endif

  param_f(vid.smart_range_detail, "smart-range-detail", 8)
  ->editable(1, 50, 1, "minimum visible cell in pixels", "", 'd')
  ->set_extra([] { add_cells_drawn('C'); });

  param_f(vid.smart_range_detail_3, "smart-range-detail-3", 30)
  ->editable(1, 50, 1, "minimum visible cell in pixels", "", 'd')
  ->set_extra([] { add_cells_drawn('C'); });

  param_b(vid.smart_area_based, "smart-area-based", false);
  param_i(vid.cells_drawn_limit, "limit on cells drawn", 10000);
  param_i(vid.cells_generated_limit, "limit on cells generated", 250);

  param_enum(diskshape, "disk_shape", dshTiles)
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
  param_f(sn::solrange_xy, "solrange-xy");
  param_f(sn::solrange_z, "solrange-z");
  #endif
  param_i(slr::shader_iterations, "slr-steps");
  param_f(slr::range_xy, "slr-range-xy");
  param_f(slr::range_z, "slr-range-z");

  param_f(arcm::euclidean_edge_length, "arcm-euclid-length");
  
  #if CAP_ARCM
  auto arcms = param_str(arcm::current.symbol, "arcm-symbol", "4^5");
  arcms->editor = [] { pushScreen(arcm::show); arcm::init_symbol_edit(); };
  arcms->pre_reaction = non_editable_pre;
  arcms->reaction = [] { if(!arcm::load_symbol(arcm::current.symbol, false)) throw hr_parse_exception("wrong Archimedean symbol"); non_editable_post(); };
  #endif
  param_enum(hybrid::underlying, "product_underlying", hybrid::underlying)->be_non_editable();
  
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
    if(i < gBinary3) param_f(sightranges[i], "sight-g" + its(i));
    }
  
  ld bonus = 0;
  ld emul = 1;
  
  param_b(dialog::onscreen_keyboard, "onscreen_keyboard")
  ->editable("onscreen keyboard", 'k');
  
  param_b(context_fog, "coolfog");

  param_f(sightranges[gBinary3], "sight-binary3", 3.1 + bonus);
  param_f(sightranges[gCubeTiling], "sight-cubes", 10);
  param_f(sightranges[gCell120], "sight-120cell", TAU);
  param_f(sightranges[gECell120], "sight-120cell-elliptic", M_PI);
  param_f(sightranges[gRhombic3], "sight-rhombic", 10.5 * emul);
  param_f(sightranges[gBitrunc3], "sight-bitrunc", 12 * emul);
  param_f(sightranges[gSpace534], "sight-534", 4 + bonus);
  param_f(sightranges[gSpace435], "sight-435", 3.8 + bonus);

  param_f(sightranges[gCell5], "sight-5cell", TAU);
  param_f(sightranges[gCell8], "sight-8cell", TAU);
  param_f(sightranges[gECell8], "sight-8cell-elliptic", M_PI);
  param_f(sightranges[gCell16], "sight-16cell", TAU);
  param_f(sightranges[gECell16], "sight-16cell-elliptic", M_PI);
  param_f(sightranges[gCell24], "sight-24cell", TAU);
  param_f(sightranges[gECell24], "sight-24cell-elliptic", M_PI);
  param_f(sightranges[gCell600], "sight-600cell", TAU);
  param_f(sightranges[gECell600], "sight-600cell-elliptic", M_PI);
  param_f(sightranges[gHoroTris], "sight-horotris", 2.9 + bonus);
  param_f(sightranges[gHoroRec], "sight-hororec", 2.2 + bonus);
  param_f(sightranges[gHoroHex], "sight-horohex", 2.75 + bonus);

  param_f(sightranges[gKiteDart3], "sight-kd3", 2.25 + bonus);
  
  param_f(sightranges[gField435], "sight-field435", 4 + bonus);
  param_f(sightranges[gField534], "sight-field534", 3.8 + bonus);
  param_f(sightranges[gSol], "sight-sol");
  param_f(sightranges[gNil], "sight-nil", 6.5 + bonus);
  param_f(sightranges[gNIH], "sight-nih");
  param_f(sightranges[gSolN], "sight-solnih");

  param_f(sightranges[gCrystal344], "sight-crystal344", 2.5); /* assume raycasting */
  param_f(sightranges[gSpace344], "sight-344", 4.5);
  param_f(sightranges[gSpace336], "sight-336", 4);

  param_b(vid.sloppy_3d, "sloppy3d", true)
  ->editable("sloppy range checking", 's')
  ->help("Do not draw if their distance is greater than the sight range (although some points might be closer). This is faster.");

  param_i(vid.texture_step, "wall-quality", 4);
  add_texture_params();
  
  param_b(smooth_scrolling, "smooth-scrolling", false)
  ->editable("smooth scrolling", 'c');
  param_f(mouseaim_sensitivity, "mouseaim_sensitivity", 0.01);

  param_b(vid.consider_shader_projection, "shader-projection", true)
  ->editable("use GPU to compute projections", 'G')
  ->help("Computing projections using a GPU (vertex shader) is faster, but sometimes, some projections or their minor details are not available.");

  param_b(semidirect_rendering, "semidirect_rendering", false)
  ->editable("semidirect_rendering (perspective on GPU)", 'k');

  param_i(forced_center_down, "forced_center_down")
  -> editable(0, 100, 10, "forced center down", "make the center not the actual screen center", 'd');
  
  param_b(tortoise::shading_enabled, "tortoise_shading", true);

  param_f(bounded_mine_percentage, "bounded_mine_freq")
  -> editable(0, 1, 0.01, "fraction of mine in bounded minefield", "", '%')
  -> set_reaction([] {
    if(game_active) { stop_game(); start_game(); }
    });

  param_enum(nisot::geodesic_movement, "solv_geodesic_movement", true)
  -> editable({{"Lie group", "light, camera, and objects move in lines of constant direction, in the Lie group sense"}, {"geodesics", "light, camera, and objects always take the shortest path"}}, "straight lines", 'G')
  -> set_reaction([] {
    if(pmodel == mdLiePerspective && nisot::geodesic_movement) pmodel = hyperbolic ? mdPerspective : mdGeodesic;
    if(among(pmodel, mdGeodesic, mdPerspective) && !nisot::geodesic_movement) pmodel = mdLiePerspective;
    });

  param_i(s2xe::qrings, "s2xe-rings");
  param_f(hybrid::underlying_scale, "rots-underlying-scale");
  param_b(hybrid::underlying_as_pc, "underlying_as_pc")
  -> editable("draw PC on the underlying map", 'P');
  
  param_b(vid.bubbles_special, "bubbles-special", 1);
  param_b(vid.bubbles_threshold, "bubbles-threshold", 1);
  param_b(vid.bubbles_all, "bubbles-all", 0);

#if CAP_SHMUP  
  multi::initConfig();
#endif

  param_i(asonov::period_xy, "asonov:period_xy");
  param_i(asonov::period_z, "asonov:period_z");
  param_i(nilv::nilperiod[0], "nilperiod_x");
  param_i(nilv::nilperiod[1], "nilperiod_y");
  param_i(nilv::nilperiod[2], "nilperiod_z");
  
  param_enum(neon_mode, "neon_mode", neon_mode)
    ->editable(
        {{"OFF", ""}, {"standard", ""}, {"no boundary mode", ""}, {"neon mode II", ""}, {"illustration mode", ""}}, 
        "neon mode", 'M'
        );

  param_enum(bow::weapon, "pc_class", bow::weapon)
    -> editable({{"blade", "Standard Rogue weapon. Bump into a monster to hit. Most monsters attack you the same way."},
      {"crossbow", "Hits all monsters in a straight line, but slow to reload. Press 'f' or click the crossbow icon to target."}},
      "weapon selection", 'w')
    -> set_need_confirm()
    -> set_value_to = [] (bow::eWeapon wpn) { bool b = game_active; if(wpn != bow::weapon) stop_game(); bow::weapon = wpn;
      peace::on = false; if(dual::state) dual::disable(); if(multi::players > 1 && !shmup::on) multi::players = 1;
      if(b) start_game();
      };
  param_enum(bow::style, "bow_style", bow::style)
    -> editable({{"bull line", "Can go in either direction on odd shapes. 3 turns to reload."},
      {"geodesic", "Graph geodesic: any sequence of tiles is OK as long as there are no shortcuts. 4 turns to reload."},
      {"geometric", "Approximations of geometric straight lines."}},
      "crossbow straight line style", 'l')
    -> set_need_confirm()
    -> set_value_to = [] (bow::eCrossbowStyle s) { bool b = game_active; if(s != bow::style) stop_game(); bow::style = s; if(b) start_game(); };
  param_b(bow::bump_to_shoot, "bump_to_shoot", true)->editable("bump to shoot", 'b');
  param_enum(bow::mouse_fire_mode, "mouse_fire_mode", bow::mfmPriority)
     ->editable({{"explicit", "You need to click crossbow or be close to fire."},
      {"priority", "Click on a faraway monster to fire if possible, or move if not."},
      {"always", "Clicking on a faraway monster always means an attempt to fire."}},
      "mouse auto-fire mode", 'm');

  param_enum(vid.msgleft, "message_style", 2)
    -> editable({{"centered", ""}, {"left-aligned", ""}, {"line-broken", ""}}, "message style", 'a');

  param_enum(neon_nofill, "neon_nofill", neon_nofill);
  param_b(noshadow, "noshadow");
  param_b(bright, "bright");
  param_b(cblind, "cblind");
  
  param_i(berger_limit, "berger_limit");
  
  param_enum(centering, "centering", centering);
  
  param_f(camera_speed, parameter_names("camspd", "camera-speed"), 1);
  param_f(camera_rot_speed, parameter_names("camrot", "camera-rot-speed"), 1);
  param_f(third_person_rotation, "third_person_rotation", 0);

  param_f(vid.stereo_param, "stereo_param", 0.9)
  ->editable(-1, 1, 0.9, "stereographic/Panini parameter", "1 for full stereographic/Panini projection. Lower values reduce the effect.\n\n"
        "HyperRogue uses "
        "a quick implementation, so parameter values too close to 1 may "
        "be buggy (outside of raycasting); try e.g. 0.9 instead.", 'd')
  ->set_reaction(reset_all_shaders);

  callhooks(hooks_configfile);
  
  #if CAP_SHOT
  param_f(levellines, "levellines", 0);
  #endif

#if CAP_CONFIG
  for(auto s: params) s.second->reset();
#endif

  param_custom_int(sightrange_bonus, "sightrange_bonus", menuitem_sightrange_bonus, 'r');
  param_custom_int(vid.use_smart_range, "sightrange_style", menuitem_sightrange_style, 's');
  
  param_custom_int(gp::param.first, "Goldberg x", menuitem_change_variation, 0);
  param_custom_int(gp::param.second, "Goldberg y", menuitem_change_variation, 0);
  param_custom_int(variation, parameter_names("variation", "mode-variation"), menuitem_change_variation, 'v')
  ->help_text = "variation|dual|bitruncated";
  param_custom_int(geometry, parameter_names("geometry", "mode-geometry"), menuitem_change_geometry, 0)
  ->help_text = "hyperbolic|spherical|Euclidean";
  
  param_i(stamplen, "stamplen");
  param_f(anims::period, "animperiod");

  param_b(use_custom_land_list, "customland_use")->be_non_editable();
  for(int i=0; i<landtypes; i++) {
    custom_land_list[i] = true;
    custom_land_treasure[i] = 100;
    custom_land_difficulty[i] = 100;
    custom_land_wandering[i] = 100;
    param_b(custom_land_list[i], "customland" + its(i) + "i", true)->be_non_editable();
    param_i(custom_land_treasure[i], "customland" + its(i) + "t", 100)->be_non_editable();
    param_i(custom_land_difficulty[i], "customland" + its(i) + "d", 100)->be_non_editable();
    param_i(custom_land_wandering[i], "customland" + its(i) + "w", 100)->be_non_editable();
    param_i(custom_land_ptm_runs[i], "customland" + its(i) + "r", 100)->be_non_editable();
    param_i(custom_land_ptm_mult[i], "customland" + its(i) + "m", 100)->be_non_editable();
    }
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
  for(auto s: params)
    if(s.second->legacy_config_name.substr(0,5) != "mode-")
      s.second->reset();
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
  
  for(auto s: params) if(s.second->dosave())
    fprintf(f, "%s=%s\n", s.second->name.c_str(), s.second->save().c_str());
  
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

map<string, shared_ptr<parameter> > allconfigs;

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
  dynamicval<bool> ds(delayed_start, true);
  for(auto& c: params) allconfigs[c.second->name] = allconfigs[c.second->legacy_config_name] = c.second;
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
  cgi.require_basics();
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

EX void menuitem_sightrange_bonus(key_type c) {
  dialog::addSelItem(XLAT("sight range bonus"), its(sightrange_bonus), c);
  dialog::add_action([]{
    dialog::editNumber(sightrange_bonus, -5, allowIncreasedSight() ? 3 : 0, 1, 0, XLAT("sight range"), 
      XLAT("Roughly 42% cells are on the edge of your sight range. Reducing "
      "the sight range makes HyperRogue work faster, but also makes "
      "the game effectively harder."));
    dialog::get_di().reaction = doOvergenerate;
    dialog::bound_low(1-getDistLimit());
    dialog::bound_up(allowIncreasedSight() ? euclid ? 99 : gp::dist_2() * 5 : 0);
    });
  }

EX void edit_sightrange_3d(char key, bool fog) {
  dialog::addSelItem(fog ? XLAT("3D sight range for the fog effect") : ("3D sight range"), fts(sightranges[geometry]), key);
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
    dialog::get_di().extra_options = [] { add_cells_drawn('C'); };
    });
  }

EX void edit_sightrange() {
  cmode = sm::SIDE;
  gamescreen();
  dialog::init("sight range settings");
  add_edit(vid.use_smart_range);
  int wdim = WDIM;
  #if CAP_RUG
  USING_NATIVE_GEOMETRY_IN_RUG;
  #endif
  if(vid.use_smart_range) {
    add_edit(wdim == 2 ? vid.smart_range_detail : vid.smart_range_detail_3);
    if(GDIM == 3) edit_sightrange_3d('r', true);
    }
  else {
    if(wdim == 2) {
      add_edit(sightrange_bonus);
      if(GDIM == 3) edit_sightrange_3d('r', true);
      }
    if(wdim == 3) edit_sightrange_3d('r', false);
    }
  #if CAP_SOLV
  if(models::is_perspective(pmodel) && sol) {
    dialog::addSelItem(XLAT("max difference in X/Y coordinates"), fts(sn::solrange_xy), 'x');
    dialog::add_action([] {
      dialog::editNumber(sn::solrange_xy, 0.01, 200, 0.1, 50, XLAT("max difference in X/Y coordinates"), solhelp()), dialog::scaleLog();
      dialog::get_di().extra_options = [] { add_cells_drawn('C'); };
      });
    dialog::addSelItem(XLAT("max difference in Z coordinate"), fts(sn::solrange_z), 'z');
    dialog::add_action([] {
      dialog::editNumber(sn::solrange_z, 0, 20, 0.1, 6, XLAT("max difference in Z coordinates"), solhelp());
      dialog::get_di().extra_options = [] { add_cells_drawn('C'); };
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
  if(vid.use_smart_range && wdim == 2) {
    dialog::addBoolItem_action(XLAT("area-based range"), vid.smart_area_based, 'a');
    }
  if(vid.use_smart_range == 0 && allowChangeRange() && wdim == 2) {
    dialog::addSelItem(XLAT("generation range bonus"), its(genrange_bonus), 'o');
    dialog::add_action([] () { genrange_bonus = sightrange_bonus; doOvergenerate(); });
    dialog::addSelItem(XLAT("game range bonus"), its(gamerange_bonus), 's');
    dialog::add_action([] () { gamerange_bonus = sightrange_bonus; doOvergenerate(); });
    }
  if(wdim == 3 && !vid.use_smart_range) {
    add_edit(vid.sloppy_3d);
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

EX void menuitem_sightrange_style(key_type c IS('c')) {
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

EX void menuitem_sightrange(key_type c IS('c')) {
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
    dialog::addSelItem(XLAT("sight range settings"), hr::format("%+d", sightrange_bonus), c);
  dialog::add_action_push(edit_sightrange);
  }

EX void sets_sfx_volume() {
#if CAP_AUDIO
  dialog::get_di().dialogflags = sm::NOSCR;
  #if ISANDROID
  dialog::get_di().reaction = [] () {
    settingsChanged = true;
    };
  #endif
  dialog::bound_low(0);
  dialog::bound_up(MIX_MAX_VOLUME);
#endif
  }

EX void sets_music_volume() {
#if CAP_AUDIO
  dialog::get_di().dialogflags = sm::NOSCR;
  dialog::get_di().reaction = [] () {
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
  dialog::get_di().extra_options = [] {
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
  add_edit(mapfontscale);

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

  add_edit(vid.flasheffects);

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
  dialog::get_di().extra_options = [type, index] {
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

  add_edit(font_id);

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

  add_edit(nohelp);
  
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
    dialog::get_di().extra_options = [] {
      draw_crosshair();
      dialog::addColorItem(XLAT("crosshair color"), crosshair_color, 'X');
      dialog::add_action([] {
        dialog::openColorDialog(crosshair_color);
        dialog::get_di().extra_options = draw_crosshair;
        });
      };
    });

  add_edit(menu_format);
  add_edit(menu_darkening);
  add_edit(centered_menus);
  add_edit(startanims::enabled);
  add_edit(use_bool_dialog);
   
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

  add_edit(joy_init);

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
  dialog::get_di().extra_options = [] () {
    dialog::addBreak(100);
    if(GDIM == 2) dialog::addHelp(XLAT(
      "If we are viewing an equidistant g absolute units below a plane, "
      "from a point c absolute units above the plane, this corresponds "
      "to viewing a Minkowski hyperboloid from a point "
      "tanh(g)/tanh(c) units below the center. This in turn corresponds to "
      "the Poincaré model for g=c, and Klein-Beltrami model for g=0."));
    dialog::addSelItem(sphere ? "stereographic" : "Poincaré model", "1", 'P');
    dialog::add_action([] () { *dialog::get_ne().editwhat = 1; vpconf.scale = 1; dialog::get_ne().s = "1"; });
    dialog::addSelItem(sphere ? "gnomonic" : "Klein model", "0", 'K');
    dialog::add_action([] () { *dialog::get_ne().editwhat = 0; vpconf.scale = 1; dialog::get_ne().s = "0"; });
    if(hyperbolic) {
      dialog::addSelItem("inverted Poincaré model", "-1", 'I');
      dialog::add_action([] () { *dialog::get_ne().editwhat = -1; vpconf.scale = 1; dialog::get_ne().s = "-1"; });
      }
    dialog::addItem(sphere ? "orthographic" : "Gans model", 'O');
    dialog::add_action([] () { vpconf.alpha = vpconf.scale = 999; dialog::get_ne().reset_str(); });
    dialog::addItem(sphere ? "towards orthographic" : "towards Gans model", 'T');
    dialog::add_action([] () { double d = 1.1; vpconf.alpha *= d; vpconf.scale *= d; dialog::get_ne().reset_str(); });
    };
  }

EX void menuitem_projection_distance(key_type key) {
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
  auto p = get_stereo_param();
  if(p >= 1 || p <= -1) return 360;
  return acos(-p) * 2 / degree;
  }

EX void edit_fov_screen() {
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
  dialog::get_di().extra_options = [] {
    auto ptr = dynamic_cast<dialog::number_dialog*> (screens.back().target_base());
    if(ptr && ptr->vmax != max_fov_angle()) { popScreen(); edit_fov_screen(); return; }
    add_edit(vid.stereo_mode, 'M');
    if(among(vid.stereo_mode, sPanini, sStereographic)) {
      add_edit(vid.stereo_param, 'P');
      }
    };
  }

EX void add_edit_fov(char key IS('f')) {

  string sfov = fts(vid.fov) + "°";
  if(get_stereo_param()) {
    sfov += " / " + fts(max_fov_angle()) + "°";
    }
  dialog::addSelItem(XLAT("field of view"), sfov, key);
  dialog::add_action(edit_fov_screen);
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
    case sPanini: case sStereographic:
      add_edit(vid.stereo_param);
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
    dialog::get_di().reaction = [] {
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
    dialog::get_di().reaction = ray::reset_raycaster;
    dialog::get_di().extra_options = [] {
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

EX bool in_tpp() { return pmodel == mdDisk && !models::camera_straight; }

EX void display_embedded_errors() {
  using namespace geom3;
  auto eucs = [] {
    dialog::addItem(XLAT("set square tiling"), 'A'); dialog::add_action([] { dialog::do_if_confirmed( [] { stop_game(); set_geometry(gEuclidSquare); set_variation(eVariation::pure); start_game(); });});
    dialog::addItem(XLAT("set hex tiling"), 'B'); dialog::add_action([] { dialog::do_if_confirmed( [] { stop_game(); set_geometry(gEuclid); set_variation(eVariation::pure); start_game(); });});
    };
  if(among(spatial_embedding, seNil, seProductH, seProductS, seCliffordTorus, seSL2) && (!among(geometry, gEuclid, gEuclidSquare) || !PURE)) {
    dialog::addInfo(XLAT("error: currently works only in PURE Euclidean regular square or hex tiling"), 0xC00000);
    eucs();
    return;
    }
  if(among(spatial_embedding, seSol, seSolN, seNIH) && (!bt::in() && !among(geometry, gEuclid, gEuclidSquare))) {
    dialog::addInfo(XLAT("error: currently works only in pure Euclidean, or binary tiling and similar"), 0xC00000);
    eucs();
    dialog::addItem(XLAT("set binary tiling variant"), 'C'); dialog::add_action([] { dialog::do_if_confirmed( [] { stop_game(); set_geometry(gBinaryTiling); geom3::switch_fpp(); geom3::switch_fpp(); start_game(); }); });
    dialog::addItem(XLAT("set ternary tiling"), 'D'); dialog::add_action([] { dialog::do_if_confirmed( [] { stop_game(); set_geometry(gTernary); geom3::switch_fpp(); geom3::switch_fpp(); start_game(); }); });
    dialog::addItem(XLAT("set binary tiling"), 'E'); dialog::add_action([] { dialog::do_if_confirmed( [] { stop_game(); set_geometry(gBinary4); geom3::switch_fpp(); geom3::switch_fpp(); start_game(); }); });
    return;
    }
  if(shmup::on && cgi.emb->no_spin()) {
    dialog::addInfo(XLAT("error: this embedding does not work in shmup"), 0xC00000);
    return;
    }
  if(meuclid && spatial_embedding == seCliffordTorus) {
    if(!clifford_torus_valid()) {
      dialog::addInfo(XLAT("error: this method works only in rectangular torus"), 0xC00000);
      dialog::addItem(XLAT("set 20x20 torus"), 'A'); dialog::add_action([] { dialog::do_if_confirmed( [] { stop_game();
        auto& T0 = euc::eu_input.user_axes;
        T0[0][0] = T0[1][1] = 20;
        T0[0][1] = 0;
        T0[1][0] = geometry == gEuclid ? 10 : 0;
        euc::eu_input.twisted = false;
        euc::build_torus3();
        geom3::apply_settings_full(); start_game(); }); });
      return;
      }
    }
  if(meuclid && spatial_embedding == seProductS) {
    #if CAP_RUG
    rug::clifford_torus ct;
    bool err = sqhypot_d(2, ct.xh) < 1e-3 && sqhypot_d(2, ct.yh) < 1e-3;
    if(err) {
      dialog::addInfo(XLAT("error: this method works only in cylinder"), 0xC00000);
      dialog::addItem(XLAT("set cylinder"), 'A'); dialog::add_action([] { dialog::do_if_confirmed( [] { stop_game();
        auto& T0 = euc::eu_input.user_axes;
        T0[0][0] = 10;
        T0[0][1] = T0[1][0] = T0[1][1] = 0;
        euc::eu_input.twisted = false;
        euc::build_torus3();
        geom3::apply_settings_full(); start_game(); }); });
      return;
      }
    #else
    dialog::addInfo(XLAT("error: not supported"), 0xC00000);
    #endif
    }
  if(msphere && !among(spatial_embedding, seNone, seDefault, seLowerCurvature, seMuchLowerCurvature, seProduct, seProductS)) {
    dialog::addInfo(XLAT("error: this method does not work in spherical geometry"), 0xC00000);
    return;
    }
  if(mhyperbolic && !among(spatial_embedding, seNone, seDefault, seLowerCurvature, seMuchLowerCurvature, seProduct, seProductH, seSol, seSolN, seNIH)) {
    dialog::addInfo(XLAT("error: this method does not work in hyperbolic geometry"), 0xC00000);
    return;
    }
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
    dialog::add_action([se] { invoke_embed(se); });
    string s = why_wrong(se);
    if(s != "")
      dialog::items.back().value = (emb == i ? ONOFF(true) : XLAT("needs")) + s;
    }

  dialog::addBreak(100);
  dialog::addHelp(XLAT(seo[emb].second));
  display_embedded_errors();
  dialog::addBreak(100);

  if(geom3::auto_configure) {
    if(emb == geom3::seNone) {
      dialog::addBoolItem(XLAT("third-person perspective"), in_tpp(), 'T');
      dialog::add_action(geom3::switch_tpp);
      #if CAP_RUG
      dialog::addBoolItem(XLAT("Hypersian Rug"), rug::rugged, 'u');
      dialog::add_action([] {
        if(in_tpp()) geom3::switch_tpp();
        if(!rug::rugged) {
          pconf.alpha = 1, pconf.scale = 1; if(!rug::rugged) rug::init();
          }
        else rug::close();
        });
      #endif
      dialog::addBreak(100);
      }
    else {
      if(geom3::supports_flat()) add_edit(geom3::flat_embedding);
      else dialog::addBreak(100);
      if(geom3::supports_invert()) add_edit(geom3::inverted_embedding);
      else dialog::addBreak(100);
      }
    }

  dialog::addSelItem(XLAT("reset view"), embedded_plane && isize(current_display->radarpoints) == 0 ? XLAT("(fix errors)") : !cells_drawn ? XLAT("(fix errors)") : "", ' ');
  dialog::add_action([] {
    if(rug::rug_control())
      rug::reset_view();
    else
      fullcenter();
    });

  dialog::addBreak(100);
  dialog::addBack();

  dialog::display();
  }

EX void show3D_height_details() {
  cmode = sm::SIDE | sm::MAYDARK;
  gamescreen();
  dialog::init(XLAT("3D detailed settings"));

  add_edit(vid.wall_height);

  dialog::addBreak(50);

  add_edit(vid.rock_wall_ratio);
  add_edit(vid.human_wall_ratio);
  add_edit(vid.lake_top);
  add_edit(vid.lake_shallow);
  add_edit(vid.lake_bottom);

  dialog::addBreak(50);

  if(embedded_plane) {
    add_edit(auto_remove_roofs);
    add_edit(vid.wall_height2);
    add_edit(vid.wall_height3);
    add_edit(draw_sky);
    add_edit(vid.lowsky_height);
    add_edit(vid.sky_height);
    add_edit(vid.star_height);
    add_edit(vid.infdeep_height);
    add_edit(vid.sun_size);
    add_edit(vid.star_size);
    #if MAXMDIM >= 4
    add_edit(star_prob);
    add_edit(vid.height_limits);
    if(euclid && msphere) add_edit(use_euclidean_infinity);
    #endif

    dialog::addBreak(100);
    dialog::addHelp(lalign(0, "absolute altitudes:\n\n"
      "depth ", cgi.INFDEEP,
      " water ", tie(cgi.BOTTOM, cgi.SHALLOW, cgi.LAKE),
      " floor ", cgi.FLOOR,
      " eye ", vid.eye,
      " walls ", tie(cgi.WALL, cgi.HIGH, cgi.HIGH2),
      " star ", cgi.STAR,
      " sky ", cgi.SKY,
      "\n\n",
      "recommended: ", cgi.emb->height_limit(-1), " to ", cgi.emb->height_limit(1)
      ));
    }
  else dialog::addInfo(XLAT("more options in 3D engine"));

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
    dialog::addSelItem(XLAT("3D style"), XLAT(geom3::spatial_embedding_options[shown_spatial_embedding()].first), 'E');
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
    if(cgi.emb->is_euc_scalable()) {
      add_edit(geom3::euclid_embed_scale);
      add_edit(geom3::euclid_embed_scale_y);
      add_edit(geom3::euclid_embed_rotate);
      }
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
    dialog::addSelItem(XLAT("3D detailed settings"), "", 'D');
    dialog::add_action_push(show3D_height_details);
    
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
  if(true) add_edit(vpconf.cam());
  if(GDIM == 2) {
    dialog::addSelItem(XLAT("fixed facing"), vid.fixed_facing ? fts(vid.fixed_facing_dir) : XLAT("OFF"), 'f');
    dialog::add_action([] () { vid.fixed_facing = !vid.fixed_facing; 
      if(vid.fixed_facing) {
        dialog::editNumber(vid.fixed_facing_dir, 0, 360, 15, 90, "", "");
        dialog::get_di().dialogflags |= sm::CENTER;
        }
      });
    }

  if(has_fixed_yz())
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
  #if CAP_RUG
  if(GDIM == 2) {
    dialog::addItem(XLAT("configure Hypersian Rug"), 'u');
    dialog::add_action_push(rug::show);
    }
  #endif

  #if MAXMDIM >= 4
  if(GDIM == 3) add_edit_fov('f');
  if(GDIM == 3) {
    dialog::addSelItem(XLAT("radar size"), fts(vid.radarsize), 'r');
    dialog::add_action([] () {
      dialog::editNumber(vid.radarsize, 0, 360, 15, 90, "", XLAT("set to 0 to disable"));
      dialog::get_di().extra_options = [] () { draw_radar(true); };
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
      dialog::get_di().extra_options = [] () { draw_radar(true); };
      });
    }
  if(GDIM == 3) {
    add_edit_wall_quality('W');
    dialog::addItem(XLAT("wall/floor texture settings"), 'X');
    dialog::add_action_push(edit_texture_params);
    }
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

#if HDR
namespace ccolor { struct data; }
#endif

EX shared_ptr<custom_parameter> param_ccolor(ccolor::data*& val, const parameter_names& n) {
  shared_ptr<custom_parameter> u ( new custom_parameter );
  u->setup(n);
  u->custom_viewer = [] (char key) {};
  u->custom_value = [&val] { for(int i=0; i<isize(ccolor::all); i++) if(ccolor::all[i] == val) return i; return -1; };
  u->last_value = u->custom_value();
  u->custom_affect = [&val] (void *v) { return &val == v; };
  u->custom_load = [&val] (const string& s) { for(auto c: ccolor::all) if(c->name == s) val = c; };
  u->custom_save = [&val] { return val->name; };
  u->custom_clone = [u] (struct local_parameter_set& lps, void *value) { auto val = (ccolor::data**) value; return param_ccolor(*val, lps.mod(&*u)); };
  return u;
  }

EX int config3 = addHook(hooks_configfile, 100, [] {
  param_f(vid.eye, "eyelevel", 0)
    ->editable(-5, 5, .1, "eye level", "", 'E')
    ->set_extra([] {
      dialog::get_di().dialogflags |= sm::CENTER;
      vid.tc_camera = ticks;
    
      dialog::addHelp(XLAT("In the FPP mode, the camera will be set at this altitude (before applying shifts)."));

      dialog::addBoolItem(XLAT("auto-adjust to eyes on the player model"), vid.auto_eye, 'O');
      dialog::get_di().reaction = [] { vid.auto_eye = false; };
      dialog::add_action([] () {
        vid.auto_eye = !vid.auto_eye;
        geom3::do_auto_eye();
        });
      });
  
  param_b(vid.auto_eye, "auto-eyelevel", false);

  param_b(nomenukey, "nomenukey");
  param_b(showstartmenu, "showstartmenu");
  param_b(draw_centerover, "draw_centerover");

  param_enum(nohelp, "help_messages", 0)
  -> editable({
    {"all", "all context help/welcome messages"},
    {"none", "no context help/welcome messages"},
    {"automatic", "I know I can press F1 for help"},
    }, "context help", 'H');

  param_f(vid.creature_scale, parameter_names("creature_scale", "3d-creaturescale"), 1)
    ->editable(0, 1, .1, "Creature scale", "", 'C')
    ->set_extra([] { dialog::addInfo(XLAT("changing this during shmup is counted as cheating")); })
    ->set_reaction([] { if(shmup::on) cheater++; });
  param_f(vid.height_width, parameter_names("heiwi", "3d-heightwidth"), 1.5)
    ->editable(0, 1, .1, "Height to width", "", 'h');
  param_f(vid.yshift, parameter_names("yshift", "Y shift"), 0)
    ->editable(0, 1, .1, "Y shift", "Don't center on the player character.", 'y')
    ->set_extra([] {
      if(WDIM == 3 && pmodel == mdPerspective) 
        dialog::addBoolItem_action(XLAT("reduce if walls on the way"), vid.use_wall_radar, 'R');
      });
  param_b(vid.use_wall_radar, "wallradar", true);
  param_b(vid.fixed_facing, "fixed facing", 0);
  param_f(vid.fixed_facing_dir, "fixed facing dir", 90);
  param_b(vid.fixed_yz, "fixed YZ", true);
  param_b(frustum_culling, "frustum_culling");
  param_b(numerical_minefield, "numerical_minefield")
  -> editable("toggle numerical display", 'n');
  param_b(mine_hollow, "mine_hollow")
  -> editable("hollow mine symbols", 'h');
  param_b(mine_markers, "mine_markers")
  -> editable("markers on possible mines", 'm');
  param_i(mine_opacity, "minefield opacity", 255)
   ->editable(0, 255, 51, "opacity of undiscovered minefield", "3D modes only\n\n0 = invisible, 255 = fully opaque", 'o');
  param_enum(mine_zero_display, "minefield_zero", 1)
  ->editable({{"OFF", "never display zeros"}, {"3D", "only in 3D modes"}, {"ON", "always display zeros"}}, "display zeros in minefield", 'z');
  param_b(dont_display_minecount, "dont_display_minecount");
  #if MAXMDIM >= 4
  param_enum(draw_sky, "draw_sky", skyAutomatic)
  -> editable({{"NO", "do not draw sky"}, {"automatic", ""}, {"skybox", "works only in Euclidean"}, {"always", "might be glitched in some settings"}}, "sky rendering", 's');
  param_b(use_euclidean_infinity, "use_euclidean_infinity", true)
  -> editable("infinite sky", 'i');
  #endif
  param_f(linepatterns::parallel_count, "parallel_count")
    ->editable(0, 24, 1, "number of parallels drawn", "", 'n');
  param_f(linepatterns::parallel_max, "parallel_max")
    ->editable(0, TAU, 15*degree, "last parallel drawn", "", 'n');
  param_f(linepatterns::mp_ori, "mp_ori")
    ->editable(0, TAU, 15*degree, "parallel/meridian orientation", "", 'n');
  param_f(linepatterns::meridian_max, "meridian_max");
  param_f(linepatterns::meridian_count, "meridian_count");
  param_f(linepatterns::meridian_length, "meridian_length");
  param_f(linepatterns::meridian_prec, "meridian_prec");
  param_f(linepatterns::meridian_prec2, "meridian_prec2");

  param_f(linepatterns::dual_length, "dual_length");
  param_matrix(linepatterns::dual_angle.v2, "dual_angle", 2);
  param_matrix(linepatterns::dual_angle.v3, "dual_angle3", 3);

  param_f(twopoint_xscale, "twopoint_xscale");
  param_i(twopoint_xshape, "twopoint_xshape");
  param_f(twopoint_xwidth, "twopoint_xwidth");
  param_f(periodwidth, "periodwidth", 1);

  param_b(draw_plain_floors, "draw_plain_floors", false)
  ->editable("draw plain floors in 3D", 'p');
  param_i(default_flooralpha, "floor_alpha")
  ->editable(0, 255, 15, "floor alpha", "255 = opaque", 'a');

  param_f(vid.depth_bonus, "depth_bonus", 0)
    ->editable(-5, 5, .1, "depth bonus in pseudohedral", "", 'b');
  param_enum(vid.pseudohedral, "pseudohedral", phOFF)
    ->editable(
    {{"OFF", "the tiles are curved"},
    {"inscribed", "the tiles are inscribed"},
    {"circumscribed", "the tiles are circumscribed"}},
    "make the tiles flat", 'p');
  param_f(vid.depth, parameter_names("depth", "3D depth"), 1)
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
        })
    ->set_reaction([] {
        bool b = vid.tc_alpha < vid.tc_camera;
        if(vid.tc_alpha >= vid.tc_depth) vid.tc_alpha = vid.depth - 1;
        if(vid.tc_camera >= vid.tc_depth) vid.tc_camera = vid.depth - 1;
        if(vid.tc_alpha == vid.tc_camera) (b ? vid.tc_alpha : vid.tc_camera)--;
        geom3::apply_settings_light();
        });
  param_f(vid.camera, parameter_names("camera", "3D camera level"), 1)
    ->editable(0, 5, .1, "", "", 'c')
    ->modif([] (float_parameter* x) { x->menu_item_name = (GDIM == 2 ? "Camera level above the plane" : "Z shift"); })
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
  param_f(vid.wall_height, parameter_names("wall_height", "3D wall height"), .3)
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
        })
    ->set_reaction(geom3::apply_settings_light);
  param_f(vid.rock_wall_ratio, parameter_names("rock_wall_ratio", "3D rock-wall ratio"), .9)
    ->editable(0, 1, .1, "Rock-III to wall ratio", "", 'r')
    ->set_extra([] { dialog::addHelp(XLAT(
        "The ratio of Rock III to walls is %1, so Rock III are %2 absolute units high. "
        "Length of paths on the Rock III level is %3 of the corresponding length on the "
        "ground level.",
        fts(vid.rock_wall_ratio), fts(vid.wall_height * vid.rock_wall_ratio),
        fts(cosh(vid.depth - vid.wall_height * vid.rock_wall_ratio) / cosh(vid.depth))));
        });
  param_f(vid.human_wall_ratio, parameter_names("human_wall_ratio", "3D human-wall ratio"), .7)
    ->editable(0, 1, .1, "Human to wall ratio", "", 'h')
    ->set_extra([] { dialog::addHelp(XLAT(
        "Humans are %1 "
        "absolute units high. Your head travels %2 times the distance travelled by your "
        "feet.",
        fts(vid.wall_height * vid.human_wall_ratio),
        fts(cosh(vid.depth - vid.wall_height * vid.human_wall_ratio) / cosh(vid.depth)))
        );
        });
  string unitwarn =
    "The unit this value is given in is wall height. "
    "Note that, in exponentially expanding spaces, too high values could cause rendering issues. So "
    "if you want infinity, values of 5 or similar should be used -- there is no visible difference "
    "from infinity and glitches are avoided.";
  param_f(vid.lake_top, parameter_names("lake_top", "3D lake top"), .25 / 0.3)
    ->editable(0, 1, .1, "Level of water surface", unitwarn, 'l');
  param_f(vid.lake_shallow, parameter_names("lake_shallow", "3D lake shallow"), .4 / 0.3)
    ->editable(0, 1, .1, "Level of shallow water", unitwarn, 's');
  param_f(vid.lake_bottom, parameter_names("lake_bottom", "3D lake bottom"), .9 / 0.3)
    ->editable(0, 1, .1, "Level of water bottom", unitwarn, 'k');
  param_f(vid.wall_height2, "wall_height2", 2)
    ->editable(0, 5, .1, "ratio of high walls to normal walls", unitwarn, '2');
  param_f(vid.wall_height3, "wall_height3", 3)
    ->editable(0, 5, .1, "ratio of very high walls to normal walls", unitwarn, '3');
  param_f(vid.lowsky_height, "lowsky_height", 2)
    ->editable(0, 5, .1, "sky fake height", "Sky is rendered at the distance computed based on "
      "the sky height, which might be beyond the range visible in fog. To prevent this, "
      "the intensity of the fog effect depends on the value here rather than the actual distance. "
      "Stars are affected similarly.", '4');
  #if MAXMDIM >= 4
  param_fd(vid.sky_height, "sky_height")
    ->set_hint([] { return geom3::to_wh(cgi.SKY); })
    ->editable(0, 10, .1, "altitude of the sky", unitwarn, '5')
    ->set_reaction(delete_sky);
  #endif
  param_fd(vid.star_height, "star_height")
    ->set_hint([] { return geom3::to_wh(cgi.STAR); })
    ->editable(0, 10, .1, "altitude of the stars", unitwarn, '6');
  param_fd(vid.infdeep_height, "infdeep_height")
    ->set_hint([] { return geom3::to_wh(cgi.INFDEEP); })
    ->editable(0, 10, .1, "infinite depth", unitwarn, '7');
  param_f(vid.sun_size, "sun_size", 8)
    ->editable(0, 10, .1, "sun size (relative to item sizes)", "", '8');
  param_f(vid.star_size, "star_size", 0.75)
    ->editable(0, 10, .1, "night star size (relative to item sizes)", "", '9');
  #if MAXMDIM >= 4
  param_f(star_prob, "star_prob", 0.3)
    ->editable(0, 1, .01, "star probability", "probability of star per tile", '*');
  #endif
  param_b(vid.height_limits, "height_limits", true)
    ->editable("prevent exceeding recommended altitudes", 'l');
  param_b(auto_remove_roofs, "auto_remove_roofs", true)
    ->editable("do not render higher levels if camera too high", 'r');
  param_i(vid.tc_depth, "3D TC depth", 1);
  param_i(vid.tc_camera, "3D TC camera", 2);
  param_i(vid.tc_alpha, "3D TC alpha", 3);
  param_f(vid.highdetail, parameter_names("highdetail", "3D highdetail"), 8)
    ->editable(0, 5, .5, "High detail range", "", 'n')
    ->set_extra(explain_detail)
    ->set_reaction([] {
      if(vid.highdetail > vid.middetail) vid.middetail = vid.highdetail;
      });  
  param_f(vid.middetail, parameter_names("middetail", "3D middetail"), 8)
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
  param_b(debug_voronoi, "debug_voronoi")->editable(
    "display Voronoi tie debug values", 'd');
  param_i(horodisk_from, "horodisk_from", -2)->editable(-10, 10, 1,
    "land size in horodisk mode",
    "Set this to -2 to get perfect horodisks. Smaller values yield less dense horodisks, and "
    "larger values might produce horodisks with errors or crashing into each other.", 'H');
  param_i(randomwalk_size, "randomwalk_size", 10)->editable(2, 100, 1,
    "land size in randomwalk mode",
    "The average size of a land in randomwalk mode.", 'R')
  ->set_reaction([] { if(game_active) { stop_game(); start_game(); } });
  param_i(landscape_div, "landscape_div")->editable(1, 100, 1,
    "land size in landscape structure",
    "Each cell gets three coordinates, each of which change smoothly, using the same method as used for the generation of landscapes e.g. in Dragon Chasms. "
    "Then, we find a cell of the bitruncated cubic honeycomb at these cordinates, and this cell determines which land it is. The bigger the value, the larger the lands.", 'R')
  ->set_sets([] { dialog::bound_low(1); })
  ->set_reaction([] { if(game_active) { stop_game(); start_game(); } });

  param_i(curse_percentage, "curse_percentage")->editable(0, 100, 1,
    "curse percentage",
    "The percentage of towers in Cursed Walls mode to be manned by Canyon Hags", 'R')
  ->set_reaction([] { if(game_active) { stop_game(); start_game(); } });

  param_f(global_boundary_ratio, "global_boundary_ratio")
  ->editable(0, 5, 0.1, "Width of cell boundaries",
    "How wide should the cell boundaries be.", '0');
  param_b(vid.gp_autoscale_heights, "3D Goldberg autoscaling", true);
  auto scf = param_str(scorefile, "savefile");
  scf->be_non_editable(); scf->reaction = [] { if(save_loaded) exit(1); };
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

  param_ccolor(ccolor::which, "pattern");
  param_b(ccolor::live_canvas, "live_canvas")
  -> editable("apply color/pattern changes to canvas automatically", 'l');
  param_str(ccolor::color_formula, "color_formula")
  -> editor = [] { ccolor::config_formula(false); };
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
  if(bow::crossbow_mode()) {
    dialog::addColorItem(XLAT("bow color"), cs.bowcolor, 'b');
    dialog::addColorItem(XLAT("bowstring color"), cs.bowcolor2, 'c');
    }
  
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
    else if(uni == 'b') switchcolor(cs.bowcolor, swordcolors);
    else if(uni == 'c') switchcolor(cs.bowcolor2, eyecolors);
    else if(doexiton(sym, uni)) popScreen();
    };
  }

EX void refresh_canvas() { ccolor::live_canvas = true; }

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
      if(r) dialog::get_di().reaction = r;
      dialog::colorAlpha = false;
      dialog::get_di().dialogflags |= sm::SIDE;
      });
    }

  dialog::addItem("add a color", 'A');
  dialog::add_action([&ct, r] {
    ct.push_back(rand() & 0x1FFFFFF);
    if(r) r();
    });

  if(isize(ct) > 2) {
    dialog::addItem("delete a color", 'D');
    dialog::add_action([&ct, r] {
      ct.pop_back();
      if(r) r();
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
  dialog::add_action([] () { dialog::openColorDialog(backcolor); dialog::colorAlpha = false; dialog::get_di().dialogflags |= sm::SIDE; });
  
  if(WDIM == 2 && GDIM == 3 && hyperbolic)
    dialog::addBoolItem_action(XLAT("cool fog effect"), context_fog, 'B');

  dialog::addColorItem(XLAT("foreground"), addalpha(forecolor), 'f');
  dialog::add_action([] () { dialog::openColorDialog(forecolor); dialog::colorAlpha = false; dialog::get_di().dialogflags |= sm::SIDE; });

  dialog::addColorItem(XLAT("borders"), addalpha(bordcolor), 'o');
  dialog::add_action([] () { dialog::openColorDialog(bordcolor); dialog::colorAlpha = false; dialog::get_di().dialogflags |= sm::SIDE; });

  dialog::addColorItem(XLAT("projection boundary"), ringcolor, 'r');
  dialog::add_action([] () { dialog::openColorDialog(ringcolor); dialog::get_di().dialogflags |= sm::SIDE; });

  dialog::addSelItem(XLAT("boundary width multiplier"), fts(vid.multiplier_ring), 'R');
  dialog::add_action([] () { dialog::editNumber(vid.multiplier_ring, 0, 10, 1, 1, XLAT("boundary width multiplier"), ""); });

  dialog::addColorItem(XLAT("projection background"), modelcolor, 'c');
  dialog::add_action([] () { dialog::openColorDialog(modelcolor); dialog::get_di().dialogflags |= sm::SIDE; });

  dialog::addColorItem(XLAT("standard grid color"), stdgridcolor, 'g');
  dialog::add_action([] () { vid.grid = true; dialog::openColorDialog(stdgridcolor); dialog::get_di().dialogflags |= sm::SIDE; });
  
  dialog::addSelItem(XLAT("grid width multiplier"), fts(vid.multiplier_grid), 'G');
  dialog::add_action([] () { dialog::editNumber(vid.multiplier_grid, 0, 10, 1, 1, XLAT("grid width multiplier"), ""); });

  dialog::addSelItem(XLAT("brightness behind the sphere"), fts(backbrightness), 'i');
  dialog::add_action([] () { dialog::editNumber(backbrightness, 0, 1, .01, 0.25, XLAT("brightness behind the sphere"), 
    XLAT("In the orthogonal projection, objects on the other side of the sphere are drawn darker.")); dialog::bound_low(0); });

  dialog::addColorItem(XLAT("projection period"), periodcolor, 'p');
  dialog::add_action([] () { dialog::openColorDialog(periodcolor); dialog::get_di().dialogflags |= sm::SIDE; });

  dialog::addColorItem(XLAT("dialogs"), addalpha(dialog::dialogcolor), 'd');
  dialog::add_action([] () { dialog::openColorDialog(dialog::dialogcolor); dialog::colorAlpha = false; dialog::get_di().dialogflags |= sm::SIDE; });

  dialog::addBreak(50);
  if(specialland == laCanvas && ccolor::which->ctab.size()) {
    dialog::addItem(XLAT("pattern colors"), 'P');
    dialog::add_action_push([] { edit_color_table(ccolor::which->ctab, refresh_canvas, true); });

    if(ccolor::which == &ccolor::shape_mirror) {
      dialog::addItem(XLAT("unreversed colors"), 'U');
      dialog::add_action_push([] { edit_color_table(ccolor::shape.ctab, refresh_canvas, true); });
      }

    add_edit(ccolor::live_canvas);
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
      else if(auto tab = special_colortable_for(c)) { pushScreen([tab] { edit_color_table(*tab); }); return; }
      else if(c->wall) 
        dialog::openColorDialog(winf[c->wall == waMineMine ? waMineUnknown : c->wall].color);
      #if CAP_COMPLEX2
      else if(c->land == laBrownian) 
        dialog::openColorDialog(brownian::get_color_edit(c->landparam));
      #endif
      else 
        dialog::openColorDialog(floorcolors[c->land]);
      dialog::colorAlpha = false;
      dialog::get_di().dialogflags |= sm::SIDE;
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
    dialog::get_di().reaction = checkmove;
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

vector<parameter*> last_changed;

EX void add_to_changed(parameter *f) {
  auto orig_f = f;
  for(int i=0; i<isize(last_changed); i++) {
    if(last_changed[i] == f)
      return;
    swap(last_changed[i], f);
    if(f == orig_f) return;
    }
  last_changed.push_back(f);
  }

EX parameter *find_edit(void *val) {
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

EX void add_edit_ptr(void *val, char key) {
  int found = 0;
  for(auto& fs: params) {
    fs.second->check_change();
    if(fs.second->affects(val))
      fs.second->show_edit_option(key), found++;
    }
  if(found != 1) println(hlog, "found = ", found);
  }

#if HDR
template<class T> void add_edit(T& val) {
  add_edit_ptr(&val);
  }

template<class T> void add_edit(T& val, char key) {
  add_edit_ptr(&val, key);
  }
#endif

EX void find_parameter() {
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

EX void edit_all_parameters() {
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
  dialog::add_action_push(find_parameter);
  dialog::addBack();
  dialog::display();
  }

void list_parameter::show_edit_option(key_type key) {
  string opt;
  if(get_value() < 0 || get_value() >= isize(options)) opt = its(get_value());
  else opt = options[get_value()].first;
  dialog::addSelItem(XLAT(menu_item_name), XLAT(opt), key);
  reaction_t screen = [this] {
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
      auto action = [this, i, need_list] { set_value(i); if(need_list == 0) popScreen(); if(manual_reaction) manual_reaction(); if(reaction) reaction(); };
      if(needs_confirm)
        dialog::add_action_confirmed(action);
      else
        dialog::add_action(action);
      if(need_list == 0 && options[i].second != "") {
        dialog::addBreak(100);
        dialog::addHelp(XLAT(options[i].second));
        dialog::addBreak(100);
        }
      }
    if(need_list >= 2) dialog::end_list();
    dialog::addBreak(100);

    if(need_list >= 1 && options[get_value()].second != "") {
      string text = options[get_value()].second;
      dialog::addHelp(XLAT(text));
      dialog::addBreak(100);
      }
    dialog::addBack();
    dialog::display();
    };
  dialog::add_action_push(screen);
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
  dialog::add_action_push(edit_all_parameters);

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
    PHASEFROM(2); shift(); ccolor::apeirogonal_color = argcolor(32);
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
    PHASEFROM(2); launch_dialog(edit_all_parameters);
    }
  else if(argis("-d:find")) {
    PHASEFROM(2); launch_dialog(find_parameter);
    }
  else if(argis("-d:param")) {
    PHASEFROM(2);
    shift();
    string s = args();
    cmode |= sm::SIDE;
    for(auto& fs: params) if(fs.first == s) {
      dialog::items.clear();
      dialog::key_actions.clear();
      fs.second->show_edit_option();
      for(auto p: dialog::key_actions) { p.second(); return 0; }
      println(hlog, "no key action");
      return 0;
      }
    println(hlog, "unknown param to edit: ", s);
    }
  else if(argis("-char")) {
    auto& cs = vid.cs;
    shift();
    string s = args();
    set_char_by_name(cs, s);
    }
  else return 1;
  return 0;
  }

EX void set_char_by_name(charstyle& cs, const string& s) {
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
    cs.charid = atoi(s.c_str());
    cs.lefthanded = cs.charid >= 10;
    cs.charid %= 10;
    }
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
  params[name]->load_as_animation(value);
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

/* local parameter, for another game */

local_parameter_set* current_lps;

void local_parameter_set::pswitch() {
  if(extends) extends->pswitch();
  for(auto s: swaps) {
    s.first->swap_with(&*s.second);
    swap(s.first->name, s.second->name);
    }
  }

EX void lps_enable(local_parameter_set *lps) {
  if(lps == current_lps) return;
  if(current_lps) current_lps->pswitch();
  current_lps = lps;
  if(current_lps) current_lps->pswitch();
  }

#if HDR
//template<class T> vector<std::unique_ptr<T>> lps_of_type;
extern vector<void*> lps_of_type;

template<class T, class U> void lps_add(local_parameter_set& lps, T&val, U nvalue) {
  int found = 0;
  for(auto& fs: params) {
    if(fs.second->affects(&val)) {
      found++;
      T* nv = new T(nvalue);
      lps_of_type.emplace_back(nv);
      println(hlog, lps.label, " found saver: ", fs.second->name);
      auto fs2 = fs.second->clone(lps, nv);
      lps.swaps.emplace_back(fs.second, fs2);
      return;
      }
    }
  if(found != 1) println(hlog, lps.label, " saver not found");
  }
#endif

vector<void*> lps_of_type;

}
