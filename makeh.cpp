// generate autohdr.h based on the `EX` and `#if HDR` in *.cpp files
#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set>

using namespace std;

int indent = 2;

string ind() { string s; for(int i=0; i<indent; i++) s += ' '; return s; }

string which_file;

vector<string> if_stack;
int ifs_level;

void mark_file() {
  if(which_file != "") {
    cout << "\n" << ind() << "// implemented in: " << which_file << "\n\n";
    which_file = "";
    }
  while(ifs_level < (int) if_stack.size())
    cout << ind() << if_stack[ifs_level++] << "\n";
  }

int in_hdr;

set<string> seen;

int lineid;

void gen(string sf) {
  if(seen.count(sf)) return;
  seen.insert(sf);
  which_file = sf; lineid = 1;
  ifstream in(sf);
  string s;
  while(getline(in, s)) {
    lineid++;
    while(s != "" && s[0] == ' ') s = s.substr(1);
    while(s.back() == 10 || s.back() == 13) s = s.substr(0, s.size() - 1);
    if(in_hdr) {
      if(s == "#endif")
        in_hdr--;
      if(s.substr(0, 3) == "#if")
        in_hdr++;
      if(in_hdr)
        cout << ind() << s << "\n";
      continue;
      }
    if(s == "#if HDR") {
      mark_file();
      cout << "#line " << lineid << " \"" << sf << "\"\n";
      in_hdr = true;
      continue;
      }
    if(s == "#if CU_INIT") {
      if_stack.push_back("#if 1");
      continue;
      }
    if(s.substr(0, 3) == "#if" || s.substr(0, 4) == "# if") {
      if_stack.push_back(s);
      }
    if(s.substr(0, 6) == "#endif") {
      if(if_stack.empty()) { cerr << "if_stack error " << which_file << ", " << s << "\n"; exit(1); }
      if_stack.pop_back();
      while(ifs_level > (int) if_stack.size())
        cout << ind() << "#endif\n", ifs_level--;
      }
    if(s.substr(0, 4) == "EX }") {
      mark_file();
      cout << ind() << "}\n";
      indent -= 2;
      }
    else if(s.substr(0, 3) == "EX ") {
      string t = s.substr(3);
      if(t.substr(0, 10) == "namespace ") {
        mark_file();
        cout << ind() << t << "\n";
        indent += 2;
        }
      else {
        mark_file();
        cout << "#line " << lineid << " \"" << sf << "\"\n";
        for(int i=0;; i++) {
          if(i == int(t.size())) { cerr << "Error: unrecognizable EX: " << s << "\n"; }
          else if(t[i] == '{') {
            while(i && t[i-1] == ' ') i--;
            cout << ind() << t.substr(0, i) << ";\n";
            break;
            }
          else if(t[i] == ';') {
            cout << ind() << "extern " << t << "\n";
            break;
            }
          else if(t[i] == '=') {
            while(i && t[i-1] == ' ') i--;
            cout << ind() << "extern " << t.substr(0, i) << ";\n";
            break;
            }
          }
        }
      }
    }
  
  while(ifs_level > (int) if_stack.size())
    cout << ind() << "#endif\n", ifs_level--;

  while(indent > 2) {
    cout << ind() << "}\n";
    indent -= 2;
    }
  }

int main(int argc, char ** argv) {
  printf("// This file is generated automatically by makeh.cpp.\n\nnamespace hr {\n");
  indent = 2;
  
  for(int i=1; i<argc; i++) 
    gen(argv[i]);
  
  printf("  }\n");
  }
