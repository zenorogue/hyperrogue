// HyperRogue: alternative build system
// This reads the file 'hyper.cpp' and compiles the cpp files it includes into separate object files, and then links them.
// Tested in Linux, should work in other systems with some changes.

// Options:
//   -O2 -- optimize
//   -O3 -- optimize
//   -D... -- change compilation flags
//   [file.cpp] -- add a module to the build (e.g. ./mymake rogueviz)

#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <sys/stat.h>
#include <unistd.h>

using namespace std;

string opts = "-DFHS -DLINUX -DWHATEVER -I/usr/include/SDL";

string standard = " -std=c++11";

string preprocessor = 
  "g++ " + opts + " -E";

string compiler = 
  "g++ " + opts + " -Wall -Wextra -Wno-maybe-uninitialized -Wno-missing-field-initializers -Wno-unused-parameter -Wno-implicit-fallthrough -rdynamic -fdiagnostics-color=always -c";

string linker = 
  "g++ -rdynamic -o hyper";
 
string libs = " savepng-loc.o -lSDL -lSDL_ttf -lSDL_mixer -lSDL_gfx -lGLEW -lGL -lpng -rdynamic -lpthread";

vector<string> modules;

time_t get_file_time(const string s) {
  struct stat attr;
  if(stat(s.c_str(), &attr)) return 0;
  return attr.st_mtime;
  }

int optimized = 0;

string obj_dir = "mymake_files";
string setdir = "../";

int system(string cmdline) {
  return system(cmdline.c_str());
  }

bool file_exists(string fname) {
  return access(fname.c_str(), F_OK) != -1;
  }
  
int main(int argc, char **argv) {
  for(string fname: {"Makefile.loc", "Makefile.simple", "Makefile"})
    if(file_exists(fname))
      system("make -f " + fname + " language-data.cpp autohdr.h savepng-loc.o");
  for(int i=1; i<argc; i++) {
    string s = argv[i];
    if(s.substr(0, 2) == "-D") {
      opts += " " + s;
      obj_dir += "/";
      setdir += "../";
      for(char c: s) 
        if(c == '=' || c == '-' || c == '/') obj_dir += "_"; 
        else obj_dir += c;      
      }
    else if(s == "-O2")
      optimized = 2, compiler += " -O2", obj_dir += "/O2", setdir += "../";
    else if(s == "-O3")
      optimized = 3, compiler += " -O3", obj_dir += "/O3", setdir += "../";
    else if(s.substr(0, 4) == "-std")
      standard = s;
    else if(s.substr(0, 2) == "-l")
      linker += " " + s;
    else if(s == "-I") {
      linker += " " + s + " " + argv[i+1];
      i++;
      }
    else if(s.substr(0, 2) == "-I")
      linker += " " + s;
    else modules.push_back(s);
    }
  if(!optimized)
    compiler += " -g3";
  compiler += " " + standard;
  ifstream fs("hyper.cpp");

  system("mkdir -p " + obj_dir);

  ofstream fsm(obj_dir + "/hyper.cpp");
  fsm << "#if REM\n#define INCLUDE(x)\n#endif\n";
  string s;
  while(getline(fs, s)) {
    if(s.substr(0, 8) == "#include") {
      string t;
      bool in = false;
      bool ext = false;
      string iext = "";
      for(char c: s) if(c == '"') in = !in; else if(!in) ; else if(c == '.') ext = !ext; else if(!ext) t += c; else iext += c;
      if(iext == "h") { fsm << "#include \"" + setdir + "hyper.h\"\n"; continue; }
      if(iext != "cpp") printf("unknown extension: %s\n", iext);
      fsm << "INCLUDE(___" << t << ")\n";
      continue;
      }
    fsm << s << "\n";
    }
  fsm.close();
  
  printf("preprocessing...\n");
  if(system(preprocessor + " "+obj_dir+"/hyper.cpp -o "+obj_dir+"/hyper.E")) { printf("preprocessing error\n"); exit(1); }
  
  if(true) {
    ifstream fs2(obj_dir+"/hyper.E");
    while(getline(fs2, s)) {
      if(s.substr(0, 7) == "INCLUDE") {
        s = s.substr(11);
        s = s.substr(0,s.size() - 1);
        modules.push_back(s);
        }
      }
    }

  if(get_file_time(obj_dir + "/hyper.o") < get_file_time("hyper.cpp")) {
    printf("compiling hyper...\n");
    if(system(compiler + " -DREM " + obj_dir + "/hyper.cpp -c -o " + obj_dir + "/hyper.o")) { printf("error\n"); exit(1); }
    }
  
  string allobj = " " + obj_dir + "/hyper.o";
  
  int id = 0;
  for(string m: modules) {
    id++;
    string src = m + ".cpp";
    string m2 = m;
    for(char& c: m2) if(c == '/') c = '_';
    string obj = obj_dir + "/" + m2 + ".o";
    time_t src_time = get_file_time(src);
    if(!src_time) { 
      printf("file not found: %s\n", src.c_str());
      exit(1);
      }
    time_t obj_time = get_file_time(obj);
    if(src_time > obj_time) {
      printf("compiling %s... [%d/%d]\n", m.c_str(), id, int(modules.size()));
      if(system(compiler + " " + src + " -o " + obj)) { printf("error\n"); exit(1); }
      }
    else {
      printf("ok: %s\n", m.c_str());
      }
    allobj += " ";
    allobj += obj;
    }
  
  printf("linking...\n");
  system(linker + allobj + libs);
  return 0;
  }
