// Hyperbolic Rogue language file generator

// Copyright (C) 2011-2018 Zeno Rogue, see 'hyper.cpp' for details

#define GEN_M 0
#define GEN_F 1
#define GEN_N 2
#define GEN_O 3

#include <map>
#include <string>
#include <cstdio>
#include <vector>
#include <cstdlib>
#include <set>

template<class T> int isize(const T& x) { return x.size(); }

#define NUMLAN 7

// language generator

const char *escape(std::string s, const std::string& dft);

template<class T> struct dictionary {
  std::map<std::string, T> m;
  void add(const std::string& s, const T& val) {
    if(m.count(s)) add(s + " [repeat]", val);
    else m[s] = val; 
    }
  T& operator [] (const std::string& s) { return m[s]; }
  int count(const std::string& s) { return m.count(s); }
  void clear() { m.clear(); }
  };

dictionary<std::string> d[NUMLAN];

struct noun {
  int genus;
  std::string nom, nomp, acc, abl;
  };

dictionary<noun> nouns[NUMLAN];

int utfsize(char c) {
  unsigned char cu = c;
  if(cu < 128) return 1;
  if(cu < 224) return 2;
  if(cu < 0xE0) return 3;
  return 4;
  }

void addutftoset(std::set<std::string>& s, std::string& w) {
  size_t i = 0;
  while(i < w.size()) {
    int siz = utfsize(w[i]);
    s.insert(w.substr(i, siz));
    i += siz;
    }
  }

void addutftoset(std::set<std::string>& s, noun& w) {
  addutftoset(s, w.nom);
  addutftoset(s, w.nomp);
  addutftoset(s, w.acc);
  addutftoset(s, w.abl);
  }

template<class T>
void addutftoset(std::set<std::string>& s, dictionary<T>& w) {
  for(auto&& elt : w.m)
    addutftoset(s, elt.second);
  }

std::set<std::string> allchars;

typedef unsigned hashcode;

hashcode hashval;

bool isrepeat(const std::string& s) {
  return s.find(" [repeat]") != std::string::npos;
  }
  
hashcode langhash(const std::string& s) {
  if(isrepeat(s)) {
    return langhash(s.substr(0, s.size() - 9)) + 1;
    }
  hashcode r = 0;
  for(int i=0; i<isize(s); i++) r = hashval * r + s[i];
  return r;
  }

std::map<hashcode, std::string> buildHashTable(std::set<std::string>& s) {
  std::map<hashcode, std::string> res;
  for(auto&& elt : s)
    res[langhash(elt)] = elt;
  return res;
  }

const char *escape(std::string s, const std::string& dft) {
  if(s == "") {
    printf("/*MISSING*/ ");
    s = dft;
    }
  static std::string t;
  t = "\"";
  for(int i=0; i<isize(s); i++)
    if(s[i] == '\\') t += "\\\\";
    else if(s[i] == '\n') t += "\\n";
    else if(s[i] == '\"') t += "\\\"";
    else t += s[i];
  t += "\"";
  return t.c_str();
  }

std::set<std::string> nothe;
std::set<std::string> plural;

void langPL() {
  #define S(a,b) d[1].add(a,b); 
  #define N(a,b,c,d,e,f) \
    {noun n; n.genus = b; n.nom = c; n.nomp = d; n.acc = e; n.abl = f; nouns[1].add(a,n);}
  #include "language-pl.cpp"
  #undef N
  #undef S
  }

void langTR() {
#define S(a,b) d[2].add(a,b);
#define N5(a,b,c,d,e) \
  {noun n; n.genus = b; n.nom = c; n.nomp = d; n.acc = e; n.abl = e; nouns[2].add(a,n);}
#define N(a,b,c,d,e,f) \
  {noun n; n.genus = b; n.nom = c; n.nomp = d; n.acc = e; n.abl = f; nouns[2].add(a,n);}
#include "language-tr.cpp"
#undef N
#undef S
  }

void langCZ() {
#define S(a,b) d[3].add(a,b);
#define N(a,b,c,d,e,f) \
  {noun n; n.genus = b; n.nom = c; n.nomp = d; n.acc = e; n.abl = f; nouns[3].add(a,n);}
#include "language-cz.cpp"
#undef N
#undef S
  }

void langRU() {
#define S(a,b) d[4].add(a,b);
#define N(a,b,c,d,e,f) \
  {noun n; n.genus = b; n.nom = c; n.nomp = d; n.acc = e; n.abl = f; nouns[4].add(a,n);}
#include "language-ru.cpp"
#undef N
#undef S
  }

void langDE() {
#define S(a,b) d[5].add(a,b);
#define N(a,b,c,d,e) \
  {noun n; n.genus = b; n.nom = c; n.nomp = d; n.acc = e; n.abl = e; nouns[5].add(a,n);}
#include "language-de.cpp"
#undef N
#undef S
  }

void langPT() {
#define S(a,b) d[6].add(a,b);
#define N(a,b,c,d,e) \
  {noun n; n.genus = b; n.nom = c; n.nomp = d; n.abl = e; nouns[6].add(a,n);}
#include "language-ptbr.cpp"
#undef N
#undef S
  }

int completeness[NUMLAN];

int main() {

  nothe.insert("R'Lyeh");
  nothe.insert("Camelot");
  plural.insert("Crossroads");
  plural.insert("Crossroads II");
  plural.insert("Crossroads III");
  plural.insert("Elemental Planes");
  plural.insert("Crossroads IV");
  plural.insert("Kraken Depths");
  allchars.insert("ᵈ");
  allchars.insert("δ");
  allchars.insert("∞");
  allchars.insert("½");
  allchars.insert("²");
  
  langPL(); langCZ(); langRU();
  langTR(); langDE(); langPT();

  // verify
  std::set<std::string> s;
  for(int i=1; i<NUMLAN; i++) 
    for(auto&& elt : d[i].m)
      s.insert(elt.first);
  
  printf("// DO NOT EDIT -- this file is generated automatically with langen\n\n");

  for(auto&& elt : s) {
    std::string mis = "", mis1 = "";
    for(int i=1; i<NUMLAN; i++) if(d[i].count(elt) == 0) {
      std::string which = d[i]["EN"];
      if(which != "TR" && which != "DE" && which != "PT-BR")
        mis += which;
      else
        mis1 += which;
      }
    if(mis != "" && !isrepeat(elt))
      printf("// #warning Missing [%s/%s]: %s\n", mis.c_str(), mis1.c_str(), escape(elt, "?"));

    if(!isrepeat(elt)) {
      completeness[0]++;
      for(int i=1; i<NUMLAN; i++) if(d[i].count(elt)) completeness[i]++;
      }
    }
  
  s.clear();

  for(int i=1; i<NUMLAN; i++) 
    for(auto&& elt : nouns[i].m)
      s.insert(elt.first);

  for(auto&& elt : s) {
    std::string mis = "", mis1 = "";
    for(int i=1; i<NUMLAN; i++) if(nouns[i].count(elt) == 0) {
      std::string which = d[i]["EN"];
      if(which != "TR" && which != "DE" && which != "PT-BR")
        mis += which;
      else mis1 += which;
      }
    if(mis != "" && !isrepeat(elt))
      printf("// #warning Missing [%s/%s]: %s\n", mis.c_str(), mis1.c_str(), escape(elt, "?"));

    if(!isrepeat(elt)) {
      completeness[0]++;
      for(int i=1; i<NUMLAN; i++) if(nouns[i].count(elt)) completeness[i]++;
      }
    }

  for(int i=1; i<NUMLAN; i++) {
    addutftoset(allchars, d[i]);
    addutftoset(allchars, nouns[i]);
    }

  std::string javastring;
  std::vector<std::string> vchars;
  for(auto&& elt : allchars) {
    if(isize(elt) >= 2) { javastring += elt; vchars.push_back(elt); }
    }
  printf("\n");
  printf("#define NUMEXTRA %zu\n", vchars.size());
  printf("#define NATCHARS {");
  for(auto&& elt : vchars) printf("\"%s\",", elt.c_str());
  printf("};\n");
  printf("const char* natchars[NUMEXTRA] = NATCHARS;");
  printf("//javastring = \"%s\";\n", javastring.c_str());
  
  printf("\nint transcompleteness[NUMLAN] = {");
  for(int i=0; i<NUMLAN; i++) printf("%d, ", completeness[i]);
  printf("};\n");

  for(int i=1; i<NUMLAN; i++) 
    for(auto&& elt : d[i].m)
      s.insert(elt.first);
  
  printf("\n//statistics\n");
  for(auto&& elt : d[1].m)
    d[0][elt.first] = elt.first;
  for(auto&& elt : nouns[1].m) {
    noun n = elt.second;
    n.nom = n.nomp = n.acc = n.abl = elt.first;
    nouns[0][elt.first] = n;
    }

  printf("// total: %5zu nouns, %5zu sentences\n", nouns[1].m.size(), d[1].m.size());

  for(int i=0; i<NUMLAN; i++) {
    size_t bnouns = 0;
    size_t bdict = 0;

    for(auto&& elt : d[i].m)
      bdict += elt.second.size();
    for(auto&& elt : nouns[i].m) {
      const noun& n = elt.second;
      bnouns += n.nom.size();
      bnouns += n.nomp.size();
      bnouns += n.acc.size();
      bnouns += n.abl.size();
      }

    printf("// %s: %5zuB nouns, %5zuB sentences\n",
      d[i]["EN"].c_str(), bnouns, bdict);
    }
  
  std::set<std::string> allsent;
  for(auto&& elt : d[1].m)
    allsent.insert(elt.first);

  std::set<std::string> allnouns;
  for(auto&& elt : nouns[1].m)
    allnouns.insert(elt.first);
  
  std::map<hashcode, std::string> ms, mn;
  
  do {
    hashval = rand();
    printf("// check hash: %x\n", hashval);
    ms = buildHashTable(allsent);
    mn = buildHashTable(allnouns);
    }
  while(ms.size() != allsent.size() || mn.size() != allnouns.size());
  
  printf("hashcode hashval = 0x%x;\n\n", hashval);
  
  printf("sentence all_sentences[] = {\n");
  
  for(auto&& elt : ms) {
    const std::string& s = elt.second;
    if(isrepeat(s)) printf("#if REPEATED\n");    
    printf("  {0x%x, { // %s\n", elt.first, escape(s, s));
    for(int i=1; i<NUMLAN; i++) printf("   %s,\n", escape(d[i][s], s));
    printf("    }},\n");
    if(isrepeat(s)) printf("#endif\n");
    }
  printf("  };\n\n");

  printf("fullnoun all_nouns[] = {\n");
  
  for(auto&& elt : mn) {
    const std::string& s = elt.second;
    if(isrepeat(s)) printf("#if REPEATED\n");
    printf("  {0x%x, %d, { // \"%s\"\n", elt.first,
      (nothe.count(s) ? 1:0) + (plural.count(s) ? 2:0),
      escape(s, s));
    
    for(int i=1; i<NUMLAN; i++) {
      printf("    {%d", nouns[i][s].genus);
      printf(", %s", escape(nouns[i][s].nom, s));
      printf(", %s", escape(nouns[i][s].nomp, s));
      printf(", %s", escape(nouns[i][s].acc, s));
      printf(", %s},\n", escape(nouns[i][s].abl, s));
      }
    
    printf("    }},\n");
    if(isrepeat(s)) printf("#endif\n");
    }

  printf("  };\n");

  }
