namespace rogue_unlike {

// state manager

struct nop : stater {
  virtual stater& act(const string& s, int& i, int _i) { return self; }
  virtual stater& act(const string& s, ld& d, ld _d) { return self; }
  virtual stater& act(const string& s, bool& b, bool _b) { return self; }
  virtual stater& act(const string& s, string& w, const string& _w) { return self; }
  virtual stater& act(const string& s, color_t& c, color_t _c) { return self; }
  } nop;

struct changeseeker : stater {
  bool changed = false;
  stater& act(const string& s, int& i, int _i) override { if(i != _i) changed = true; return self; }
  stater& act(const string& s, ld& d, ld _d) override { if(d != _d) changed = true; return self; }
  stater& act(const string& s, bool& b, bool _b) override { if(b != _b) changed = true; return self; }
  stater& act(const string& s, string& w, const string& _w) override { if(w != _w) changed = true; return self; }
  stater& act(const string& s, color_t& c, color_t _c) override { if(c != _c) changed = true; return self; }
  };

struct saver : stater {
  fhstream f;
  stater& act(const string& s, int& i, int _i) override { if(i != _i) println(f, s, "=", i); return self; }
  stater& act(const string& s, ld& d, ld _d) override { if(d != _d) println(f, s, "=", d); return self; }
  stater& act(const string& s, bool& b, bool _b) override { if(b != _b) println(f, s, "=", b); return self; }
  stater& act(const string& s, string& w, const string& _w) override { if(w != _w) println(f, s, "=", w); return self; }
  stater& act(const string& s, color_t& c, color_t _c) override { if(c != _c) println(f, s, "=", format("%08X", c)); return self; }
  };

struct loader : stater {
  string name, value;  
  bool loaded = false;
  stater& act(const string& s, int& i, int _i) override { if(s == name) loaded = true, i = atoi(value.c_str()); return self; }
  stater& act(const string& s, ld& d, ld _d) override { if(s == name) loaded = true, d = atof(value.c_str()); return self; }
  stater& act(const string& s, bool& b, bool _b) override { if(s == name) loaded = true, b = atoi(value.c_str()); return self; }
  stater& act(const string& s, string& w, const string& _w) override { if(s == name) loaded = true, w = value; return self; }
  stater& act(const string& s, color_t& c, color_t _c) override { if(s == name) loaded = true, sscanf(value.c_str(), "%08X", &c); return self; }
  };

struct resetter : stater {
  stater& act(const string& s, int& i, int _i) override { i = _i; return self; }
  stater& act(const string& s, ld& d, ld _d) override { d = _d; return self; }
  stater& act(const string& s, bool& b, bool _b) override { b = _b; return self; }
  stater& act(const string& s, string& w, const string& _w) override { w = _w; return self; }
  stater& act(const string& s, color_t& c, color_t _c) override { c = _c; return self; }
  } resetter;

struct fountain_resetter : resetter {
  virtual stater& only_full() { return nop; }
  } fountain_resetter;

}
