// This addon is useful when debugging or tuning shaders.
// Press F2, edit the shader files, then press F3 to see them in action.

// F2: write the currently used shaders to files
// F3: replace the currently used shaders with the content of the files

// Might need some editing.

#include "../hyper.h"

namespace hr {

string load_whole(const char *fname) {
  char buf[1000000];
  FILE *f = fopen(fname, "rb");
  int n = fread(buf, 1, 1000000, f);
  buf[n] = 0;
  return buf;
  }

void print_shader(const char *fname, string s) {
  FILE *f = fopen(fname, "wb");
  int indent = 0;
  bool linestart = true;
  for(char c: s) {
    if(c == ' ' && linestart) continue;
    if(c == '\n' && linestart) continue;
    linestart = false;
    fputc(c, f);
    if(c == '{') indent += 2;
    if(c == '}') indent -= 2;
    if(c == ';' || c == '}' || c == '{') {
      fputc('\n', f);
      for(int i=0; i<indent; i++) fputc(' ', f);
      linestart = true;
      }
    }
  fclose(f);
  }

bool es_handleKey(int sym, int uni) {  

  if(sym == SDLK_F2) {
    glhr::be_textured();
    current_display->set_all(0);
    auto p = glhr::get_shaders();
    print_shader("devmods/current.vsh", p.first);
    print_shader("devmods/current.fsh", p.second);
    addMessage("shaders saved");
    return true;
    }

  if(sym == SDLK_F3) {
    glhr::be_textured();
    current_display->set_all(0);
    string vsh = load_whole("devmods/current.vsh");
    string fsh = load_whole("devmods/current.fsh");
    println(hlog, "loaded vsh:\n", vsh);
    glhr::install_shaders(vsh, fsh);
    glhr::be_textured();
    current_display->set_all(0);
    return true;
    }
  
  return false;
  }

auto eshook = 
  addHook(hooks_handleKey, 100, es_handleKey)
+ 0;

}
