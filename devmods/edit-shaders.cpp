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

  using namespace glhr;
  if(sym == SDLK_F2) {
    glhr::be_textured();
    current_display->set_all(0);
    auto p = current_glprogram;
    print_shader("devmods/current.vsh", p->_vsh);
    print_shader("devmods/current.fsh", p->_fsh);
    addMessage("shaders saved");
    return true;
    }

  if(sym == SDLK_F4) {
    println(hlog, "shader program = ", glhr::current_glprogram->_program);
    solv_all = !solv_all;
    println(hlog, "now solv_all = ", solv_all);
    return true;
    }

  /*if(sym == SDLK_F5) {
    drawthemap();
    for(auto& p: ptds) {
      auto s = dynamic_cast<dqi_poly*> (&*p);
      if(!s) continue;
      for(int i=0; i<s->cnt; i+=3) {
        array<hyperpoint, 3> ps;
        for(int k=0; k<3; k++) ps[k] = s->V * gltopoint((*(s->tab))[i+k]);
        array<hyperpoint, 3> ie;
        for(int k=0; k<3; k++) ie[k] = inverse_exp(ps[k]);
        ld iz = 99, az = -99;
        for(int k=0; k<3; k++) {
          if(ie[k][2] < iz) iz = ie[k][2];
          if(ie[k][2] > az) az = ie[k][2];
          }
        if(iz < 0 && az > 0)
          println(hlog, tie(iz, az), ps);
        }
      }
    
    return true;
    } */

  if(sym == SDLK_F5) {
    /*
    for(ld x=1; x<30; x+=.2)
    for(ld y=1; y<30; y+=.2)
    for(ld z=0; z<=20.1; z+=.02) {
      hyperpoint p = point31(x, y, z);
      transmatrix T = rgpushxto0(p);
      auto ie = inverse_exp(p);
      for(ld ax=-0.2; ax<=0.21; ax+=0.2)
      for(ld ay=-0.2; ay<=0.21; ay+=0.2)
      for(ld az=-0.2; az<=0.21; az+=0.2) {
        hyperpoint p1 = T * point31(ax, ay, az);
        auto ie1 = inverse_exp(p1);
        if(ie[2] * ie1[2] < 0 && abs(ie[2]) > .5 && abs(ie1[2]) > .5)
          println(hlog, lalign(30, kz(p)), ": ", lalign(30, kz(ie)), " ", lalign(30, kz(p1)), ": ", lalign(30, kz(ie1)), " @ ",
            make_tuple(sn::x_to_ix(p[0]), sn::x_to_ix(p[1]), sn::z_to_iz(p[2])));
        }
      }
    */
    return true;
    }

  if(sym == SDLK_F3 && !solv_all) {
    glhr::be_textured();
    current_display->set_all(0);
    string vsh = load_whole("devmods/current.vsh");
    string fsh = load_whole("devmods/current.fsh");
    println(hlog, "loaded vsh:\n", vsh);
    auto p = make_shared<glhr::GLprogram>(vsh, fsh);
    println(hlog, "compiled program: ", p->_program);
    
    auto old = current_glprogram;
    p->shader_flags = old->shader_flags;
    
    auto replace = [&] (shared_ptr<GLprogram>& s) {
      if(s == old) s = p;
      };
    
    for(auto& pp: compiled_programs) replace(pp.second);
    for(auto& pp: matched_programs) replace(pp.second);

    return true;
    }
  
  return false;
  }

auto eshook = 
  addHook(hooks_handleKey, 100, es_handleKey)
+ 0;

}
