// Hyperbolic Rogue -- raycaster
// Copyright (C) 2011-2019 Zeno Rogue, see 'hyper.cpp' for details

/** \file raycaster.cpp
 *  \brief A raycaster to draw walls.
 */

#include "hyper.h"

namespace hr {

GLuint txConnections = 0, txWallcolor = 0, txMatrixid = 0, txWallTexture = 0;

#define IN_ODS 0

struct raycaster : glhr::GLprogram {
  GLint uStart, uStartid, uN, uM, uLength, uFovX, uFovY, uIPD;
  GLint tConnections, tWallTexture, tWallcolor, tMatrixid;
  
  raycaster(string vsh, string fsh) : GLprogram(vsh, fsh) {
    println(hlog, "assigning");
    uStart = glGetUniformLocation(_program, "uStart");
    uStartid = glGetUniformLocation(_program, "uStartid");
    uN = glGetUniformLocation(_program, "uN");
    uM = glGetUniformLocation(_program, "uM");
    uLength = glGetUniformLocation(_program, "uLength");
    uFovX = glGetUniformLocation(_program, "uFovX");
    uFovY = glGetUniformLocation(_program, "uFovY");
    uIPD = glGetUniformLocation(_program, "uIPD");
  
    tConnections = glGetUniformLocation(_program, "tConnections");
    tWallTexture = glGetUniformLocation(_program, "tWallTexture");
    tWallcolor = glGetUniformLocation(_program, "tWallcolor");
    tMatrixid = glGetUniformLocation(_program, "tMatrixid");
    }
  };

shared_ptr<raycaster> our_raycaster;

void enable_raycaster() {
  if(!our_raycaster) { 
    string vsh = 
      "#define PI 3.14159265358979324\n"
      "attribute vec4 aPosition;\n"
      "uniform float uFovX, uFovY;\n"
      "varying vec4 at;\n"
      "void main() { \n"
      "  gl_Position = aPosition; at = aPosition; \n"
  #if IN_ODS    
      "  at[0] *= PI; at[1] *= PI; \n"
  #else
      "  at[0] *= uFovX; at[1] *= uFovY; \n"
  #endif
      "  }\n";
  
    string fsh = 
    "varying vec4 at;\n"
    "uniform int uN;\n"
    "uniform int uLength;\n"
    "uniform float uIPD;\n"
    "uniform mat4 uStart;\n"
    "uniform mat4 uM[84];\n"
    "uniform mat4 uTest;\n"
    "uniform float uStartid;\n"
    "uniform sampler1D tConnections;\n"
    "uniform sampler1D tWallcolor;\n"
    "uniform sampler2D tWallTexture;\n"
    "uniform sampler1D tMatrixid;\n"

    "mat4 xpush(float x) { return mat4("
         "cosh(x), 0., 0., sinh(x),\n"
         "0., 1., 0., 0.,\n"
         "0., 0., 1., 0.,\n"
         "sinh(x), 0., 0., cosh(x)"
         ");}\n"

    "mat4 xzspin(float x) { return mat4("
         "cos(x), 0., sin(x), 0.,\n"
         "0., 1., 0., 0.,\n"
         "-sin(x), 0., cos(x), 0.,\n"
         "0., 0., 0., 1."
         ");}\n"
      
    "mat4 yzspin(float x) { return mat4("
         "1., 0., 0., 0.,\n"
         "0., cos(x), sin(x), 0.,\n"
         "0., -sin(x), cos(x), 0.,\n"
         "0., 0., 0., 1."
         ");}\n"
      
    "void main() { \n"
    
    #if IN_ODS
    "  float lambda = at[0];\n" // -PI to PI
    "  float phi;\n"
    "  float eye;\n"
    "  if(at.y < 0.) { phi = at.y + PI/2.; eye = uIPD / 2.; }\n" // right
    "  else { phi = at.y - PI/2.; eye = -uIPD / 2.; }\n"
    "  mat4 vw = uStart * xzspin(-lambda) * xpush(eye) * yzspin(phi);\n"
    "  vec4 at0 = vec4(0., 0., 1., 0.);\n"
    #else
    "  mat4 vw = uStart;\n"
    "  vec4 at0 = at;"
    "  at0.y = -at.y;"
//    "  at0.z = at.y;"
    "  at0.w = 0.;\n"
    "  at0.xyz = at0.xyz / length(at0.xyz);"
    #endif
    "  vec4 position = vw * vec4(0., 0., 0., 1.);"
    "  vec4 tangent = vw * at0;"
    "  float go = 0.;"
    "  float cid = uStartid;"
//    "  int purp = 0;"
    "  for(int iter=0; iter<60; iter++) {"
      "  float dist = 100.;"
      "  int which = -1;"

      "  if(go == 0.) {\n"
      "    float best = position[3];\n"
      "    for(int i=0; i<uN; i++) {\n"
      "      float cand = (uM[i] * position)[3];\n"
      "      if(cand < best - .001) { dist = 0.; best = cand; which = i; }\n"
      "      }\n"
      "    }\n"

      "  if(which == -1) for(int i=0; i<uN; i++) {"
        // position * shd + tangent * chd - uM[i] * position * shd - uMi * tangent * chd
        "    float v = ((position - uM[i] * position)[3] / (uM[i] * tangent - tangent)[3]);\n"
        "    if(v > 1. || v < -1.) continue;\n"
        "    float d = atanh(v);\n"
        "    vec4 next_tangent = position * sinh(d) + tangent * cosh(d);\n"
        "    if(next_tangent[3] < (uM[i] * next_tangent)[3]) continue;\n"
//        "    if(d < 1e-4) continue;\n"
        "    if(d < dist) { dist = d; which = i; }\n"
          "}"
      "  if(which == -1) { gl_FragColor = vec4(0., 0., 0., 1.); return; }"
         // shift d units
      "  if(dist < 0.) { dist = 0.; }\n"
      "  go = go + dist;\n"
      "  float ch = cosh(dist); float sh = sinh(dist);\n"
      "  vec4 v = position * ch + tangent * sh;\n"
      "  tangent = tangent * ch + position * sh;\n"
      "  position = v;"
         // apply wall color
      "  float u = cid + float(which) / float(uLength);\n"
      "  vec4 col = texture1D(tWallcolor, u);\n"
      "  if(col[3] > 0.0) {\n"
      "    vec4 inface = uM[uN+which] * position;\n"
      "    inface = inface / inface.w;"
      "    float bright = texture2D(tWallTexture, (inface.yz + vec2(1.,1.))/2.).r;\n"
      "    col.xyz = col.xyz * bright * max(1.-go/7., 0.5 * exp(-go/4.));\n" // exp(-go/4.);\n"
      "    col.w = 1.;\n"
//      "    if(purp == 1) { col.rgb = (col.rgb + vec3(1., 0., 1.)) / 2; }\n"
      "    gl_FragColor = col;\n"
      "    return;\n"
      "    }\n"
         // next cell
      "  float rot = texture1D(tMatrixid, u).r;\n"
      "  int mid = int(rot * float(uLength));\n"
      "  position = uM[mid] * position;\n"
      "  tangent = uM[mid] * tangent;\n"
      "  cid = texture1D(tConnections, u).r;\n"
      "    }"
      "  gl_FragColor = vec4(0.,0.,0.,1.); \n"
      "  }";
 
    our_raycaster = make_shared<raycaster> (vsh, fsh);
    }
  full_enable(our_raycaster);
  }

void bind_array(vector<float>& v, GLuint t, GLuint& tx, int id) {
  glUniform1i(t, id);
  
  if(tx == 0) glGenTextures(1, &tx);

  glActiveTexture(GL_TEXTURE0 + id);
  glBindTexture(GL_TEXTURE_1D, tx);

  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  
  glTexImage1D(GL_TEXTURE_1D, 0, GL_R32F, isize(v), 0, GL_RED, GL_FLOAT, &v[0]);  
  GLERR("bind_array");
  }
  
void bind_array(vector<array<float, 4>>& v, GLuint t, GLuint& tx, int id) {
  glUniform1i(t, id);

  if(tx == 0) glGenTextures(1, &tx);

  glActiveTexture(GL_TEXTURE0 + id);
  glBindTexture(GL_TEXTURE_1D, tx);

  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  
  glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, isize(v), 0, GL_RGBA, GL_FLOAT, &v[0]);  
  GLERR("bind_array");
  }

bool made_texture;

void make_walltexture(GLuint t, GLuint& tx, int id) {
  if(made_texture) return;
  made_texture = true;
  glUniform1i(our_raycaster->tWallTexture, id);
  GLERR("pre walltexture");

  if(tx == 0) glGenTextures(1, &tx);
  GLERR("gentexture");

  glActiveTexture(GL_TEXTURE0 + id);
  GLERR("walltexture b");

  glBindTexture(GL_TEXTURE_2D, tx);
  GLERR("walltexture c");

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  GLERR("walltexture a");
  
  constexpr int dsize = 1024;
  
  float data[dsize][dsize];
  
  for(int y=0; y<dsize; y++)
  for(int x=0; x<dsize; x++) {
    hyperpoint h = point2(x - dsize/2 + .5, y - dsize/2 + .5) / (dsize / 2);
    // ld maxd = -1;
    ld maxd = 100;
    int q = reg3::face;
    for(int s=0; s<q; s++) {
      maxd = min(maxd, h[0]);
      h = spin(360 * degree / q) * h;
      }
    maxd = -maxd;
    maxd *= 2.4;
    if(maxd > .9) maxd = (10 - 10 * maxd) * .9;
    else maxd = 0.8 + 0.1 * max(sin(maxd*50), sin(atan2(h[1], h[0]) * 30 + M_PI/4));
    data[y][x] = maxd;
    }
  
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, dsize, dsize, 0, GL_RED, GL_FLOAT, &data[0][0]);
  GLERR("walltexture");
  }
  
EX void do_raycast() {
  enable_raycaster();

  auto& o = our_raycaster;
  
  make_walltexture(o->tWallTexture, txWallTexture, 6);

  vector<glvertex> screen = {
    glhr::makevertex(-1, -1, 1),
    glhr::makevertex(-1, +1, 1),
    glhr::makevertex(+1, -1, 1),
    glhr::makevertex(-1, +1, 1),
    glhr::makevertex(+1, -1, 1),
    glhr::makevertex(+1, +1, 1)
    };
  
  auto& cd = current_display;
  glUniform1f(o->uFovX, cd->tanfov);
  glUniform1f(o->uFovY, cd->tanfov * cd->ysize / cd->xsize);

  celllister cl(viewctr.at->c7, 10, 3000, NULL);
  
  vector<cell*> lst = cl.lst;
  
  map<cell*, int> ids;
  for(int i=0; i<isize(lst); i++) ids[lst[i]] = i;

  int length = next_p2(isize(lst) * S7);
  glUniform1i(o->uLength, length);
  GLERR("uniform length");
  auto enc = [&] (int i) { return float((i+.5) / length); };
  
  // for(auto &m: reg3::spins) println(hlog, m);
  
  glUniformMatrix4fv(o->uStart, 1, 0, glhr::tmtogl_transpose(inverse(View)).as_array());
  GLERR("uniform start");
  glUniform1i(o->uN, S7);
  GLERR("uniform N");
  glUniform1f(o->uStartid, enc(ids[viewctr.at->c7] * S7));
  GLERR("uniform startid");
  glUniform1f(o->uIPD, vid.ipd);
  GLERR("uniform IPD");

  vector<transmatrix> ms;
  for(int j=0; j<S7; j++) ms.push_back(inverse(reg3::adjmoves[j]));
  for(int j=0; j<S7; j++) ms.push_back(inverse(reg3::spins[j]));
  
  vector<float> connections(length);
  vector<array<float, 4>> wallcolor(length);
  vector<float> matrixid(length);
  
  if(1) for(cell *c: lst) {
    int id = ids[c];
    forCellIdEx(c1, i, c) { 
      int u = id * S7 + i;
      if(!ids.count(c1)) {
        wallcolor[u] = glhr::acolor(0xFF);
        continue;
        }
      connections[u] = enc(ids[c1] * S7);
      if(isWall3(c1)) {
        color_t wcol;
        color_t fcol;
        setcolors(c1, wcol, fcol);
        wcol = darkena(wcol, 0, 0xFF);
        wallcolor[u] = glhr::acolor(wcol);
        }
      else
        wallcolor[u] = glhr::acolor(0);

      transmatrix T = currentmap->relative_matrix(c->master, c1->master);
      for(int k=0; k<=isize(ms); k++) {
        if(k < isize(ms) && !eqmatrix(ms[k], T)) continue;
        if(k == isize(ms)) ms.push_back(T);
        matrixid[u] = enc(k);
        break;
        }
      }
    }

  vector<glhr::glmatrix> gms;
  for(auto& m: ms) gms.push_back(glhr::tmtogl_transpose(m));
  glUniformMatrix4fv(o->uM, isize(gms), 0, gms[0].as_array());

  bind_array(wallcolor, o->tWallcolor, txWallcolor, 4);
  bind_array(connections, o->tConnections, txConnections, 3);
  bind_array(matrixid, o->tMatrixid, txMatrixid, 5);
  
  glVertexAttribPointer(hr::aPosition, 4, GL_FLOAT, GL_FALSE, sizeof(glvertex), &screen[0]);
  glhr::set_depthtest(false);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glDrawArrays(GL_TRIANGLES, 0, 6);
  GLERR("finish");

  glActiveTexture(GL_TEXTURE0 + 0);
  }

}
