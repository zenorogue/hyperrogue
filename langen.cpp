// Hyperbolic Rogue language file generator

// Copyright (C) 2011-2016 Zeno Rogue, see 'hyper.cpp' for details

#define GEN_M 0
#define GEN_F 1
#define GEN_N 2
#define GEN_O 3

#include <map>
#include <string>
#include <stdio.h>
#include <vector>
#include <stdlib.h>
using namespace std;

template<class T> int size(T x) { return x.size(); }

#define NUMLAN 7

// language generator

const char *escape(string s, string dft);

template<class T> struct dictionary {
  map<string, T> m;
  void add(const string& s, const T& val) { 
    if(m.count(s)) add(s + " [repeat]", val);
    else m[s] = val; 
    }
  T& operator [] (const string& s) { return m[s]; }
  int count(const string& s) { return m.count(s); }
  void clear() { m.clear(); }
  };

dictionary<string> d[NUMLAN];

struct noun {
  int genus;
  string nom, nomp, acc, abl;
  };

dictionary<noun> nouns[NUMLAN];

#include <set>

int utfsize(char c) {
  unsigned char cu = c;
  if(cu < 128) return 1;
  if(cu < 224) return 2;
  if(cu < 0xE0) return 3;
  return 4;
  }

void addutftoset(set<string>& s, string& w) {
  int i = 0;
//printf("%s\n", w.c_str());
  while(i < size(w)) {
    int siz = utfsize(w[i]);
    s.insert(w.substr(i, siz));
    i += siz;
    }
  }

void addutftoset(set<string>& s, noun& w) {
  addutftoset(s, w.nom);
  addutftoset(s, w.nomp);
  addutftoset(s, w.acc);
  addutftoset(s, w.abl);
  }

template<class T>
void addutftoset(set<string>& s, dictionary<T>& w) {
  for(typename map<string,T>::iterator it = w.m.begin(); it != w.m.end(); it++)
    addutftoset(s, it->second);
  }

set<string> allchars;

void printletters(dictionary<string>& la, dictionary<noun>& nounla, const char *lang) {
  set<string> s;
  addutftoset(s, la);
  addutftoset(s, nounla);
  addutftoset(allchars, la);
  addutftoset(allchars, nounla);
//printf("%s:", lang);
//for(set<string>::iterator it = s.begin(); it != s.end(); it++)
//  printf(" \"%s\",", it->c_str());
//printf("\n");
  }

typedef unsigned hashcode;

hashcode hashval;

bool isrepeat(const string& s) {
  return s.find(" [repeat]") != string::npos;
  }
  
hashcode langhash(const string& s) {
  if(isrepeat(s)) {
    return langhash(s.substr(0, s.size() - 9)) + 1;
    }
  hashcode r = 0;
  for(int i=0; i<size(s); i++) r = hashval * r + s[i];
  return r;
  }

map<hashcode, string> buildHashTable(set<string>& s) {
  map<hashcode, string> res;
  for(set<string>::iterator it = s.begin(); it != s.end(); it++) 
    res[langhash(*it)] = *it;
  return res;
  }

const char *escape(string s, string dft) {
  if(s == "") {
    printf("/*MISSING*/ ");
    s = dft;
    }
  static string t;
  t = "\"";
  for(int i=0; i<size(s); i++)
    if(s[i] == '\\') t += "\\\\";
    else if(s[i] == '\n') t += "\\n";
    else if(s[i] == '\"') t += "\\\"";
    else t += s[i];
  t += "\"";
  return t.c_str();
  }

set<string> nothe;
set<string> plural;

#ifdef CHECKALL
const char* allstr[] = {
#include "d"
  };
#endif

void setstats(set<string>& s, const char* bn) {
  int tlen=0, tc = 0;
  for(set<string>::iterator it = s.begin(); it != s.end(); it++) 
    tc++, tlen += it->size();
  printf("// %-10s %5d %5d\n", bn, tc, tlen);
  }

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
  
  langPL(); langCZ(); langRU();
  langTR(); langDE(); langPT();

  // verify
  set<string> s;
  for(int i=1; i<NUMLAN; i++) 
    for(map<string,string>::iterator it = d[i].m.begin(); it != d[i].m.end(); it++)
      s.insert(it->first);
  
  printf("// DO NOT EDIT -- this file is generated automatically with langen\n\n");

  for(set<string>::iterator x=s.begin(); x != s.end(); x++) {
    string mis = "", mis1 = "";
    for(int i=1; i<NUMLAN; i++) if(d[i].count(*x) == 0) {
      string which = d[i]["EN"];
      if(which != "TR" && which != "DE" && which != "PT-BR")
        mis += which;
      else
        mis1 += which;
      }
    if(mis != "" && !isrepeat(*x))
      printf("// #warning Missing [%s/%s]: %s\n", mis.c_str(), mis1.c_str(), escape(*x, "?"));

    if(!isrepeat(*x)) {
      completeness[0]++;
      for(int i=1; i<NUMLAN; i++) if(d[i].count(*x)) completeness[i]++;
      }
    }
  
  s.clear();

  for(int i=1; i<NUMLAN; i++) 
    for(map<string,noun>::iterator it = nouns[i].m.begin(); it != nouns[i].m.end(); it++)
      s.insert(it->first);
  
  for(set<string>::iterator x=s.begin(); x != s.end(); x++) {
    string mis = "", mis1 = "";
    for(int i=1; i<NUMLAN; i++) if(nouns[i].count(*x) == 0) {
      string which = d[i]["EN"];
      if(which != "TR" && which != "DE" && which != "PT-BR")
        mis += which;
      else mis1 += which;
      }
    if(mis != "" && !isrepeat(*x))
      printf("// #warning Missing [%s/%s]: %s\n", mis.c_str(), mis1.c_str(), escape(*x, "?"));

    if(!isrepeat(*x)) {
      completeness[0]++;
      for(int i=1; i<NUMLAN; i++) if(nouns[i].count(*x)) completeness[i]++;
      }
    }

#ifdef CHECKALL
  for(int i=1; i<NUMLAN; i++) 
    for(map<string,string>::iterator it = d[i].m.begin(); it != d[i].m.end(); it++)
      s.insert(it->first);

  int ca = sizeof(allstr) / sizeof(char*);
  for(int i=0; i<ca; i++) if(!s.count(allstr[i])) {
    printf("#warning GO %s\n", escape(allstr[i], "?"));
    }

  for(set<string>::iterator x=s.begin(); x != s.end(); x++) {
    bool b = false;
    for(int i=0; i<ca; i++) if(allstr[i] == *x) b = true;
    if(!b) printf("#warning TO %s\n", escape(*x, "?"));
    }
#endif  

  for(int i=1; i<NUMLAN; i++) {
    printletters(d[i], nouns[i], "SOMETHING");
    }

  int c =0;
  string javastring;
  vector<string> vchars;
//printf("ALL:");
  for(set<string>::iterator it = allchars.begin(); it != allchars.end(); it++) {
//  printf(" \"%s\",", it->c_str());
    if(size(*it) >= 2) { javastring += (*it); vchars.push_back(*it); c++; }
    }
  printf("\n");
  printf("#define NUMEXTRA %d\n", c);
  printf("#define NATCHARS {");
  for(int i=0; i<c; i++) printf("\"%s\",", vchars[i].c_str());
  printf("};\n");
  printf("const char* natchars[NUMEXTRA] = NATCHARS;");
  printf("//javastring = \"%s\";\n", javastring.c_str());
  
  printf("\nint transcompleteness[NUMLAN] = {");
  for(int i=0; i<NUMLAN; i++) printf("%d, ", completeness[i]);
  printf("};\n");

  for(int i=1; i<NUMLAN; i++) 
    for(map<string,string>::iterator it = d[i].m.begin(); it != d[i].m.end(); it++)
      s.insert(it->first);
  
  printf("\n//statistics\n");
  for(map<string, string>::iterator it = d[1].m.begin(); it != d[1].m.end(); it++)
    d[0][it->first] = it->first;
  for(map<string, noun>::iterator it = nouns[1].m.begin(); it != nouns[1].m.end(); it++) {
    noun n = it->second;
    n.nom = n.nomp = n.acc = n.abl = it->first;
    nouns[0][it->first] = n;
    }
  
  printf("// total: %5d nouns, %5d sentences\n", int(nouns[1].m.size()), int(d[1].m.size()));

  for(int i=0; i<NUMLAN; i++) {
    int bnouns = 0;
    int dict = 0;

    for(map<string, string>::iterator it = d[i].m.begin(); it != d[i].m.end(); it++)
      dict += it->second.size();
    for(map<string, noun>::iterator it = nouns[i].m.begin(); it != nouns[i].m.end(); it++) {
      noun& n = it->second;
      bnouns += n.nom.size();
      bnouns += n.nomp.size();
      bnouns += n.acc.size();
      bnouns += n.abl.size();
      }

    printf("// %s: %5dB nouns, %5dB sentences\n",
      d[i]["EN"].c_str(), bnouns, dict);
    }
  
  set<string> allsent;
  for(map<string, string>::iterator it = d[1].m.begin(); it != d[1].m.end(); it++)
    allsent.insert(it->first);

  set<string> allnouns;
  for(map<string, noun>::iterator it = nouns[1].m.begin(); it != nouns[1].m.end(); it++)
    allnouns.insert(it->first);
  
  map<hashcode, string> ms, mn;
  
  do {
    hashval = rand();
    printf("// check hash: %x\n", hashval);
    ms = buildHashTable(allsent);
    mn = buildHashTable(allnouns);
    }
  while(size(ms) != size(allsent) || size(mn) != size(allnouns));
  
  printf("hashcode hashval = 0x%x;\n\n", hashval);
  
  printf("sentence all_sentences[] = {\n", size(allsent));
  
  for(map<hashcode,string>::iterator it = ms.begin(); it != ms.end(); it++) {
    string s = it->second;
    if(isrepeat(s)) printf("#if REPEATED\n");    
    printf("  {0x%x, { // %s\n", it->first, escape(s, s));
    for(int i=1; i<NUMLAN; i++) printf("   %s,\n", escape(d[i][s], s));
    printf("    }},\n");
    if(isrepeat(s)) printf("#endif\n");
    }
  printf("  };\n\n");

  printf("fullnoun all_nouns[] = {\n", size(allnouns));
  
  for(map<hashcode,string>::iterator it = mn.begin(); it != mn.end(); it++) {
    string s = it->second;
    if(isrepeat(s)) printf("#if REPEATED\n");
    printf("  {0x%x, %d, { // \"%s\"\n", it->first, 
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

