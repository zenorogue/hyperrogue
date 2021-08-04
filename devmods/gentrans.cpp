#include "../hyper.h"
#include <iostream>
#include <thread>

namespace hr {

char buf[10000000];
int bufpos;

bool chk(const char *s) {
  char *t = buf+bufpos;
  while(*s == *t) s++, t++;
  return *s == 0;
  }

bool eat(const char *s) {
  bool ret = chk(s);
  if(ret) bufpos += strlen(s);
  return ret;
  }

char eatchar() {
  return buf[bufpos++];
  }

set<string> seen;

void check_ex(string s, string help) {
  if(seen.count(s)) return;
  seen.insert(s);
  if(!translation_exists(s)) println(hlog, "S(", as_nice_cstring(s), ", ", help, ")");
  }

void check_nonliteral(string s, string fname, int line) {
  if(seen.count(s)) return;
  seen.insert(s);
  string thu = "", end = "";
  for(char c: s) if(c == '[') break; else thu += c;
  for(char c: s) if(c == ']') end = ""; else end += c;
  
  if(s == "s") ;
  else if(thu == "minetexts")
    for(string m: minetexts) check_ex(m, s);
  else if(thu == "dfnames")
    for(string m: dfnames) check_ex(m, s);
  else if(thu == "ncnames")
    for(string m: ncnames) check_ex(m, s);
  else if(thu == "slides" && end == ".help") ;
    // for(auto sl: tour::slides) check_ex(sl.help);
  else if(thu == "slides" && end == ".name") ;
    // for(auto sl: tour::slides) check_ex(sl.name);
  else if(end == ".menu_displayed_name") ;
    // for(auto g: ginf) check_ex(g.menu_displayed_name);
  else if(end == ".tiling_name") ;
    // for(auto g: ginf) check_ex(g.tiling_name);
  else if(end == ".name_hyperbolic")
    for(auto md: mdinf) check_ex(md.name_hyperbolic, s);
  else if(end == ".name_spherical")
    for(auto md: mdinf) { if(md.name_spherical != md.name_hyperbolic) check_ex(md.name_spherical, s); }
  else if(end == ".name_euclidean")
    for(auto md: mdinf) { if(md.name_euclidean != md.name_hyperbolic && md.name_euclidean != md.name_hyperbolic) check_ex(md.name_euclidean, s); }
  else if(thu == "olrdesc") ;
  else if(thu == "wdmodes") 
    for(string m: wdmodes) check_ex(m, s);
  else if(thu == "axemodes") 
    for(string m: multi::axemodes) check_ex(m, s);
  else if(thu == "axmodes") 
    for(string m: multi::axmodes) check_ex(m, s);
  else if(thu == "mpnames") 
    for(string m: texture::mpnames) check_ex(m, s);
  else if(thu == "axemodes3") 
    for(string m: multi::axemodes3) check_ex(m, s);
  else if(thu == "mdmodes") 
    for(string m: mdmodes) check_ex(m, s);
  else if(thu == "hlmodes") 
    for(string m: hlmodes) check_ex(m, s);
  else if(thu == "current_filter->name") 
    for(auto f: available_filters) check_ex(f->name, s);
  else if(thu == "olrDescriptions")
    for(string m: olrDescriptions) check_ex(m, s);
  else if(thu == "shmupcmdtable") ;
  else if(thu == "stringx") ;
  else if(thu == "lv.msg") ; /* trust */
  else if(thu == "opt") ; /* todo */
  else if(thu == "menu_item_name") ; /* todo */
  else if(s == "winf[w].help") ;
  else if(s == "linf[l].help") ;
  else if(s == "iinf[it].help") ;
  else if(s == "minf[m].help") ;
  else if(among(s, "mapeditorhelp", "patthelp", "drawhelp", "warpdesc", "hyperstone_optional", "irrhelp", "helptext", "inv::helptext", "power_help", "trollhelp2")) ; /* trust */
  else
    println(hlog, "// unrecognized nonliteral: ", s, " in ", fname, ":", line);
  }

void check_xlat_content(string s, string fname, int line) {
  // println(hlog, "// xlat content found: ", s);
  string literal = "";
  string nonliteral = "";
  bool in_quote = false;
  int i = 0;
  while(i < isize(s)) {
    auto chk = [&] (string mname, string mvalue) {
      if(in_quote) return false;
      for(int k=0; k<isize(mname); k++) if(s[i+k] != mname[k]) return false;
      literal += mvalue; i += isize(mname);
      return true;
      };
    if(chk("LEADERFULL", "Steam Leaderboards")) continue;
    if(chk("LEADER", "Steam")) continue;
    if(chk("IMAGEEXT", IMAGEEXT)) continue;
    if(chk("SHMUPTITLE", SHMUPTITLE)) continue;
    if(s[i] == '"') in_quote = !in_quote;
    else if(s[i] == '\\') {
      i++;
      if(s[i] == '"' || s[i] == '\'' || s[i] == '\\')  literal += s[i];
      else if(s[i] == 'n') literal += "\n";
      else println(hlog, "unknown escape: ", s0+s[i]);
      }
    else if(in_quote) literal += s[i];
    else if(among(s[i], ' ', '\n', '\r')) ;
    else nonliteral += s[i];
    i++;
    }
  if(literal != "" && nonliteral != "") {
    println(hlog, "// confused about: ", s, " ", fname, ":", line);
    println(hlog, "literal = ", literal);
    println(hlog, "nonliteral = ", nonliteral);
    }
  else if(nonliteral != "")
    check_nonliteral(nonliteral, fname, line);
  else {
    if(seen.count(literal)) return;
    seen.insert(literal);
    if(!translation_exists(literal)) 
      println(hlog, "S(", s, ", literal in ", fname, ":", line, ")");
    }
  }

void check_file(string s) {
  FILE *f = fopen(s.c_str(), "rt");
  int qty = fread(buf, 1, 10000000, f);
  buf[qty] = 0;
  fclose(f);
  // println(hlog, "// checking file ", s, ", ", qty, " B");
  
  bufpos = 0;
  
  bool in_xlat = false;
  bool in_quote = false;
  string xlat_content;
  
  int linenum = 0;
  
  while(true) {
    if(bufpos == qty) break;
    if(buf[bufpos] == '\n') linenum++;
    if(!in_xlat) {
      if(eat("XLAT(")) {
        in_xlat = true;
        xlat_content = "";
        }
      else 
        eatchar();
      }
    else {
      if(eat("\"")) {
        xlat_content += "\"";
        in_quote = !in_quote;
        }
      else if(in_quote && eat("\\")) {
        xlat_content += "\\";
        xlat_content += eatchar();
        }
      else if(!in_quote && eat("(")) {
        println(hlog, "// found parenthesis inside XLAT: ", xlat_content, " at ", s, ":", linenum);
        }
      else if(!in_quote && eat("?")) {
        println(hlog, "// found '?' inside XLAT: ", xlat_content, " at ", s, ":", linenum);
        }
      else if(!in_quote && (eat(")") || eat(","))) {
        check_xlat_content(xlat_content, s, linenum);
        in_xlat = false;
        }
      else
        xlat_content += eatchar();
      }
    }
  }

void gentrans() {
  DIR           *d;
  struct dirent *dir;
  
  println(hlog, "// checking all the files");

  d = opendir(".");

  if(d) {
    while ((dir = readdir(d)) != NULL) {
      string s = dir->d_name;
      if(s.size() > 4 && s.substr(s.size()-4) == ".cpp")
        check_file(s);
      }
    closedir(d);
    }

  println(hlog, "// checking configurables");
  
  for(auto& fs: params) {
    auto& sett = fs.second;
    if(sett->menu_item_name != sett->config_name)
      check_ex(sett->menu_item_name, "menu_item_name for " + sett->parameter_name);
    check_ex(sett->help_text, "help_text for " + sett->parameter_name);
    auto ls = dynamic_cast<list_setting*> ( (setting*) &*sett);
    if(ls)
      for(auto opt: ls->options) {
        check_ex(opt.first, "option first for " + sett->parameter_name);
        check_ex(opt.second, "option second for " + sett->parameter_name);
        }
    }
  
  exit(0);
  }

auto ar = arg::add3("-gentrans", gentrans);

}
