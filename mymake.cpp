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
#include <thread>
#include <vector>
#include <sys/stat.h>
#include <unistd.h>
#include <chrono>
#include <future>
#include <functional>

using namespace std;

string opts;

string default_standard = " -std=c++11";
string standard = default_standard;

string preprocessor;
string compiler;
string linker;
string libs;
string default_exec_name;

bool verbose = false;
bool quiet = false;

int batch_size = thread::hardware_concurrency() + 1;
bool mingw64 = false;

int sdlver = 1;

int mysystem(string cmdline) {
  if(verbose) {
    printf("%s\n", cmdline.c_str());
    }
  if (mingw64)
    cmdline = "sh -c '" + cmdline + "'"; // because system(arg) passes arg to cmd.exe on MinGW
  return system(cmdline.c_str());
  }

bool file_exists(string fname) {
  return access(fname.c_str(), F_OK) != -1;
  }

#if defined(MAC)
string os = "mac";
#elif defined(WINDOWS)
string os = "mingw64";
#else
string os = "linux";
#endif

string exec_name = "";

void set_linux() {
  preprocessor = "g++ -E";
  compiler = "g++ -Wall -Wextra -Wno-maybe-uninitialized -Wno-unused-parameter -Wno-implicit-fallthrough -Wno-invalid-offsetof -rdynamic -fdiagnostics-color=always -c";
  linker = "g++ -rdynamic";
  default_exec_name = "hyper";
  if(sdlver == 2) {
    opts = "-DFHS -DLINUX -I/usr/include/SDL2";
    libs = " -lSDL2 -lSDL2_ttf -lSDL2_mixer -lSDL2_gfx -lGLEW -lGL -lpng -rdynamic -lpthread -lz";
    }
  else if(sdlver == 1) {
    opts = "-DFHS -DLINUX -I/usr/include/SDL";
    libs = " -lSDL -lSDL_ttf -lSDL_mixer -lSDL_gfx -lGLEW -lGL -lpng -rdynamic -lpthread -lz";
    }
  else {
    opts = "-DFHS -DLINUX";
    libs = " -rdynamic -lpthread -lz";
    }
  }

void set_mac() {
  preprocessor = "g++ -E";
  compiler = "g++ -W -Wall -Wextra -Wsuggest-override -pedantic -Wno-unused-parameter -Wno-implicit-fallthrough -Wno-invalid-offsetof -c";
  linker = "g++";
  default_exec_name = "hyper";
  opts = "-DMAC -I/usr/local/include";
  libs = " -L/usr/local/lib -framework AppKit -framework OpenGL -lSDL -lSDLMain -lSDL_gfx -lSDL_mixer -lSDL_ttf -lpng -lpthread -lz";
  }

void set_mingw64() {
  mingw64 = true;
  preprocessor = "g++ -E";
  compiler = "g++ -mwindows -W -Wall -Wextra -Wno-unused-parameter -Wno-invalid-offsetof -Wno-implicit-fallthrough -Wno-maybe-uninitialized -c";
  linker = "g++";
  default_exec_name = "hyper";
  opts = "-DWINDOWS -DCAP_GLEW=1 -DCAP_PNG=1";
  libs = " hyper.res -lopengl32 -lSDL -lSDL_gfx -lSDL_mixer -lSDL_ttf -lpthread -lz -lglew32 -lpng";
  setvbuf(stdout, NULL, _IONBF, 0); // MinGW is quirky with output buffering
  }

/* cross-compile Linux to Windows (tested on Archlinux) */
void set_mingw64_cross() {
  preprocessor = "x86_64-w64-mingw32-g++ -E";
  compiler = "x86_64-w64-mingw32-g++ -mwindows -W -Wall -Wextra -Wno-unused-parameter -Wno-invalid-offsetof -Wno-implicit-fallthrough -Wno-maybe-uninitialized -c";
  linker = "x86_64-w64-mingw32-g++";
  default_exec_name = "hyper.exe";
  opts = "-DWINDOWS -DGLEW_STATIC -DUSE_STDFUNCTION=1 -DCAP_PNG=1";
  if(sdlver == 1) opts += " -I /usr/x86_64-w64-mingw32/include/SDL/";
  if(sdlver == 2) opts += " -I /usr/x86_64-w64-mingw32/include/SDL2/";
  libs = " hyper64.res -static-libgcc -lopengl32";
  if(sdlver == 1) libs += " -lSDL -lSDL_gfx -lSDL_mixer -lSDL_ttf";
  if(sdlver == 2) libs += " -lSDL2 -lSDL2_gfx -lSDL2_mixer -lSDL2_ttf";
  libs += " -lpthread -lz -lglew32 -lpng";
  setvbuf(stdout, NULL, _IONBF, 0); // MinGW is quirky with output buffering
  if(!file_exists("hyper64.res"))
    mysystem("x86_64-w64-mingw32-windres hyper.rc -O coff -o hyper64.res");
  }

void set_web() {
  preprocessor = "/usr/lib/emscripten/em++ -E";
  compiler = "/usr/lib/emscripten/em++ -c";
  default_standard = standard = " -std=c++17";
  opts = "-DISWEB=1";
  linker = 
    "/usr/lib/emscripten/em++ -s USE_ZLIB=1 -s DISABLE_DEPRECATED_FIND_EVENT_TARGET_BEHAVIOR=0 -s TOTAL_MEMORY=512MB "
    "-s EXTRA_EXPORTED_RUNTIME_METHODS='[\"FS\",\"ccall\"]' "
    "-s EXPORTED_FUNCTIONS=\"['_main', '_use_file']\" "
    "-s DISABLE_EXCEPTION_CATCHING=0";
  default_exec_name = "mhyper.html";
  libs = "";
  }

void set_os(string o) {
  os = o;
  if(os == "linux") set_linux();
  else if(os == "mac") set_mac();
  else if(os == "mingw64") set_mingw64();
  else if(os == "mingw64-cross") set_mingw64_cross();
  else if(os == "web") set_web();
  else {
    fprintf(stderr, "unknown OS");
    exit(1);
    }
  }

vector<string> modules;
vector<string> hidden_dependencies;

time_t get_file_time(const string s) {
  struct stat attr;
  if(stat(s.c_str(), &attr)) return 0;
  time_t res = attr.st_mtime;

  for(auto& hd: hidden_dependencies) if(s.substr(0, hd.size()) == hd) {
    int pos = 0;
    vector<int> slashes = {0};
    int numslash = 0;
    for(char c: s) { pos++; if(c == '/') slashes.push_back(pos), numslash++; }
    ifstream ifs(s);
    string s1;
    while(getline(ifs, s1)) {
      if(s1.substr(0, 10) == "#include \"") {
        string t = s1.substr(10);
        t = t.substr(0, t.find("\""));
        int qdot = 0;
        while(t.substr(0, 3) == "../" && qdot < numslash) qdot++, t = t.substr(3);
        string u = s.substr(0, slashes[numslash - qdot]) + t;
        res = max(res, get_file_time(u));
        }
      }
    }
  return res;
  }

int optimized = 0;

string obj_dir = "mymake_files";
string setdir = "../";

int main(int argc, char **argv) {
  set_os(os);
  int retval = 0; // for storing return values of some function calls
  for(string fname: {"Makefile.loc", "Makefile"})
    if(file_exists(fname)) {
      retval = mysystem("make -f " + fname + " language-data.cpp autohdr.h");
      if (retval) { printf("error during preparation!\n"); exit(retval); }
      break;
      }
  for(int i=1; i<argc; i++) {
    string s = argv[i];
    if(s.substr(0, 2) == "-D") {
      opts += " " + s;
      obj_dir += "/";
      setdir += "../";
      for(char c: s) 
        if(!isalnum(c)) obj_dir += "_"; 
        else obj_dir += c;      
      }
    else if(s == "-v") {
      verbose = true;
      }
    else if(s == "-q") {
      quiet = true;
      }
    else if(s == "-mingw64") {
      set_os("mingw64");
      obj_dir += "/mingw64";
      setdir += "../";
      }
    else if(s == "-mingw64-cross") {
      set_os("mingw64-cross");
      obj_dir += "/mingw64";
      setdir += "../";
      }
    else if(s == "-mac") {
      set_os("mac");
      obj_dir += "/mac";
      setdir += "../";
      }
    else if(s == "-linux") {
      set_os("linux");
      obj_dir += "/linux";
      setdir += "../";
      }
    else if(s == "-web") {
      set_os("web");
      modules.push_back("hyperweb");
      obj_dir += "/web";
      setdir += "../";
      }
    else if(s == "-sdl1") {
      sdlver = 1;
      set_os(os);
      obj_dir += "/sdl1";
      setdir += "../";
      opts += " -DCAP_SDL2=0";
      }
    else if(s == "-sdl0") {
      sdlver = 0;
      set_os(os);
      obj_dir += "/sdl0";
      setdir += "../";
      opts += " -DCAP_SDL=0 -DCAP_GL=0 -DCAP_PNG=0";
      }
    else if(s == "-sdl2") {
      sdlver = 2;
      set_os(os);
      obj_dir += "/sdl2";
      setdir += "../";
      opts += " -DCAP_SDL2=1";
      }
    else if(s.substr(0, 2) == "-f" || s.substr(0, 2) == "-m") {
      opts += " " + s;
      obj_dir += "/";
      setdir += "../";
      for(char c: s) 
        if(!isalnum(c)) obj_dir += "_"; 
        else obj_dir += c;
      compiler += " " + s;
      linker += " " + s;
      }
    else if(s == "-o") {
      exec_name = argv[i+1];
      i++;
      }
    else if(s == "-shared") {
      obj_dir += "/shared";
      setdir += "../";
      opts += " -fPIC -DNOMAIN";
      linker += " -shared -fPIC";
      default_exec_name = "libhyper.so";
      }
    else if(s == "-O2")
      optimized = 2, compiler += " -O2", obj_dir += "/O2", setdir += "../";
    else if(s == "-O3")
      optimized = 3, compiler += " -O3", obj_dir += "/O3", setdir += "../";
    else if(s == "-Werror")
      compiler += " -Werror", obj_dir += "/Werror", setdir += "../";
    else if(s.substr(0, 4) == "-std")
      standard = s;
    else if(s.substr(0, 2) == "-l")
      linker += " " + s;
    else if(s.substr(0, 2) == "-j")
      batch_size = stoi(s.substr(2));
    else if(s == "-I") {
      opts += " " + s + " " + argv[i+1];
      i++;
      }
    else if(s.substr(0, 2) == "-I")
      opts += " " + s;
    else if(s == "-vr") {
      obj_dir += "/vr";
      setdir += "../";
      if(os == "mingw64" || os == "mingw64-cross") {
        linker += " for-win64/mingw64/bin/libopenvr_api.dll";
        opts += " -DCAP_VR=1 -I./for-win64/mingw64/include/openvr/";
        }
      else {
        linker += " -lopenvr_api";
        opts += " -DCAP_VR=1 -I/usr/include/openvr/";
        }
      }
    else if(s == "-rv") {
      
      if(standard == default_standard) {
        standard = "-std=c++17";
        }
      ifstream ifs("rogueviz/rogueviz-all.cpp");
      string s;
      while(getline(ifs, s)) {
        if(s.substr(0, 10) == "#include \"") {
          string t = s.substr(10);
          t = t.substr(0, t.find(".cpp\""));
          modules.push_back("rogueviz/" + t);
          }
        if(s.substr(0, 24) == "// hidden dependencies: ") {
          while(s.back() == 10 || s.back() == 13) s.pop_back();
          hidden_dependencies.push_back(s.substr(24));
          }
        }
      }
    else {
      if(s.size() >= 5 && s.substr(s.size() - 4) == ".cpp")
        s = s.substr(0, s.size() - 4);
      modules.push_back(s);
      }
    }
  if(!optimized)
    compiler += " -g3";
  preprocessor += " " + standard;
  compiler += " " + standard;
  ifstream fs("hyper.cpp");

  retval = mysystem("mkdir -p " + obj_dir);
  if (retval) { printf("unable to create output directory!\n"); exit(retval); }

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
      if(iext != "cpp") printf("unknown extension: %s\n", iext.c_str());
      fsm << "INCLUDE(\"" << t << "\")\n";
      continue;
      }
    fsm << s << "\n";
    }
  fsm.close();
  
  if(!quiet) printf("preprocessing...\n");
  if(mysystem(preprocessor + " " + opts + " "+obj_dir+"/hyper.cpp -o "+obj_dir+"/hyper.E")) { printf("preprocessing error\n"); exit(1); }
  
  if(true) {
    ifstream fs2(obj_dir+"/hyper.E");
    while(getline(fs2, s)) {
      if(s.substr(0, 7) == "INCLUDE") {
        s = s.substr(9);
        s = s.substr(0,s.size() - 2);
        modules.push_back(s);
        }
      }
    }
  
  if(sdlver) modules.push_back("savepng");

  if(get_file_time(obj_dir + "/hyper.o") < get_file_time("hyper.cpp")) {
    if(!quiet) printf("compiling hyper...\n");
    if(mysystem(compiler + " -DREM " + opts + " " + obj_dir + "/hyper.cpp -c -o " + obj_dir + "/hyper.o")) { printf("error\n"); exit(1); }
    }
  
  string allobj = " " + obj_dir + "/hyper.o";

  printf("compiling modules using batch size of %d:\n", batch_size);

  int id = 0;
  vector<pair<int, function<int(void)>>> tasks;
  for(string m: modules) {
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
    if(src == "language.cpp") {
      src_time = max(src_time, get_file_time("language-data.cpp"));
      }
    if(src_time > obj_time) {
      string cmdline = compiler + " " + opts + " " + src + " -o " + obj;
      pair<int, function<int(void)>> task(id, [cmdline]() { return mysystem(cmdline); });
      tasks.push_back(task);
      }
    else {
      if(!quiet) printf("ok: %s\n", m.c_str());
      }
    allobj += " ";
    allobj += obj;
    id++;
    }

  chrono::milliseconds quantum(40);
  vector<future<int>> workers(batch_size);

  int tasks_amt = tasks.size();
  int tasks_taken = 0, tasks_done = 0;
  bool finished = tasks.empty();

  while (!finished) {
  for (auto & worker : workers) {
    if (worker.valid()) {
      if (worker.wait_for(chrono::seconds(0)) != future_status::ready) continue;
      else {
        int res = worker.get();
        if (res) { printf("compilation error!\n"); exit(1); }
        ++tasks_done;
        }
      }
    if (tasks_taken < tasks_amt) {
      auto task = tasks[tasks_taken];
      int mid = task.first;
      function<int(void)> do_work = task.second;
      if(!quiet)
        printf("compiling %s... [%d/%d]\n", modules[mid].c_str(), tasks_taken+1, tasks_amt);
      worker = async(launch::async, do_work);
      ++tasks_taken;
      }
    else if (tasks_done == tasks_amt) { finished = true; break; }
    } this_thread::sleep_for(quantum); }

  if (mingw64) {
    retval = mysystem("windres hyper.rc -O coff -o hyper.res");
    if (retval) { printf("windres error!\n"); exit(retval); }
    }

  printf("linking...\n");
  if(exec_name == "") exec_name = default_exec_name;
  retval = mysystem(linker + " -o " + exec_name + allobj + libs);
  if (retval) { printf("linking error!\n"); exit(retval); }
  return 0;
  }
