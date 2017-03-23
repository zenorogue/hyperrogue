// Hyperbolic Rogue language file generator

// Copyright (C) 2011-2016 Zeno Rogue, see 'hyper.cpp' for details

#define GEN_M 0
#define GEN_F 1
#define GEN_N 2
#define GEN_O 3

using namespace std;
#include <map>
#include <string>
#include <stdio.h>
#include <vector>
#include <stdlib.h>

template<class T> int size(T x) { return x.size(); }

#define NUMLAN 6

// language generator

template<class T> struct dictionary {
  map<string, T> m;
  void add(const string& s, const T& val) { m[s] = val; }
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

void addutftoset(set<string>& s, string& w) {
  int i = 0;
//printf("%s\n", w.c_str());
  while(i < size(w)) {
  
    if(((signed char)(w[i])) < 0) {
      string z = w.substr(i, 2);
//    printf("Insert: %s [%02x%02x]\n", z.c_str(), w[i], w[i+1]);
      s.insert(w.substr(i, 2));
      i += 2;
      }
    else {
      s.insert(w.substr(i, 1));
      i++;
      }
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

hashcode langhash(const string& s) {
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

int main() {

  nothe.insert("R'Lyeh");
  nothe.insert("Camelot");
  plural.insert("Crossroads");
  plural.insert("Crossroads II");
  plural.insert("Crossroads III");
  plural.insert("Elemental Planes");
  plural.insert("Crossroads IV");
  plural.insert("Kraken Depths");
  
#define S(a,b) d[1].add(a,b); 
#define N(a,b,c,d,e,f) \
  {noun n; n.genus = b; n.nom = c; n.nomp = d; n.acc = e; n.abl = f; nouns[1].add(a,n);}
#include "language-pl.cpp"
#undef N
#undef S

#define S(a,b) d[2].add(a,b);
#define N5(a,b,c,d,e) \
  {noun n; n.genus = b; n.nom = c; n.nomp = d; n.acc = e; n.abl = e; nouns[2].add(a,n);}
#define N(a,b,c,d,e,f) \
  {noun n; n.genus = b; n.nom = c; n.nomp = d; n.acc = e; n.abl = f; nouns[2].add(a,n);}
#include "language-tr.cpp"
#undef N
#undef S

#define S(a,b) d[3].add(a,b);
#define N(a,b,c,d,e,f) \
  {noun n; n.genus = b; n.nom = c; n.nomp = d; n.acc = e; n.abl = f; nouns[3].add(a,n);}
#include "language-cz.cpp"
#undef N
#undef S

#define S(a,b) d[4].add(a,b);
#define N(a,b,c,d,e,f) \
  {noun n; n.genus = b; n.nom = c; n.nomp = d; n.acc = e; n.abl = f; nouns[4].add(a,n);}
#include "language-ru.cpp"
#undef N
#undef S

#define S(a,b) d[5].add(a,b);
#define N(a,b,c,d,e) \
  {noun n; n.genus = b; n.nom = c; n.nomp = d; n.acc = e; n.abl = e; nouns[5].add(a,n);}
#include "language-de.cpp"
#undef N
#undef S

  // verify
  set<string> s;
  for(int i=1; i<NUMLAN; i++) 
    for(map<string,string>::iterator it = d[i].m.begin(); it != d[i].m.end(); it++)
      s.insert(it->first);
  
  for(set<string>::iterator x=s.begin(); x != s.end(); x++) {
    string mis = "";
    for(int i=1; i<NUMLAN; i++) if(d[i].count(*x) == 0)
      mis += d[i]["EN"];
    if(mis != "")
      printf("// #warning Missing [%s]: %s\n", mis.c_str(), escape(*x, "?"));
    }
  
  s.clear();

  for(int i=1; i<NUMLAN; i++) 
    for(map<string,noun>::iterator it = nouns[i].m.begin(); it != nouns[i].m.end(); it++)
      s.insert(it->first);
  
  for(set<string>::iterator x=s.begin(); x != s.end(); x++) {
    string mis = "";
    for(int i=1; i<NUMLAN; i++) if(nouns[i].count(*x) == 0)
      mis += d[i]["EN"];
    if(mis != "")
      printf("// #warning Missing [%s]: %s\n", mis.c_str(), escape(*x, "?"));
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
    if(size(*it) == 2) { javastring += (*it); vchars.push_back(*it); c++; }
    }
  printf("// DO NOT EDIT -- this file is generated automatically with langen\n");
  printf("\n");
  printf("#define NUMEXTRA %d\n", c);
  printf("const char* natchars[NUMEXTRA] = {");
  for(int i=0; i<c; i++) printf("\"%s\",", vchars[i].c_str());
  printf("};\n");
  printf("//javastring = \"%s\";\n", javastring.c_str());

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
  
  printf("sentence all_sentences[%d] = {\n", size(allsent));
  
  for(map<hashcode,string>::iterator it = ms.begin(); it != ms.end(); it++) {
    string s = it->second;
    printf("  {0x%x, { // %s\n", it->first, escape(s, s));
    for(int i=1; i<NUMLAN; i++) printf("   %s,\n", escape(d[i][s], s));
    printf("    }},\n");
    }
  printf("  };\n\n");

  printf("fullnoun all_nouns[%d] = {\n", size(allnouns));
  
  for(map<hashcode,string>::iterator it = mn.begin(); it != mn.end(); it++) {
    string s = it->second;
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
    }

  printf("  };\n");

  }

