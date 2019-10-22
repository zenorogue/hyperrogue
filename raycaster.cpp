// Hyperbolic Rogue -- raycaster
// Copyright (C) 2011-2019 Zeno Rogue, see 'hyper.cpp' for details

/** \file raycaster.cpp
 *  \brief A raycaster to draw walls.
 */

#include "hyper.h"

namespace hr {

GLuint txConnections = 0, txWallcolor = 0, txMatrixid = 0;

#define IN_ODS 0

struct raycaster : glhr::GLprogram {
  GLint uStart, uStartid, uN, uM, uLength, uFovX, uFovY, uIPD;
  GLint uWallstart, uWallX, uWallY;
  GLint tConnections, tWallcolor, tMatrixid;
  GLint uBinaryWidth;
  
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

    uWallstart = glGetUniformLocation(_program, "uWallstart");
    uWallX = glGetUniformLocation(_program, "uWallX");
    uWallY = glGetUniformLocation(_program, "uWallY");
    
    uBinaryWidth = glGetUniformLocation(_program, "uBinaryWidth");
  
    tConnections = glGetUniformLocation(_program, "tConnections");
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
    "uniform mat4 uM[203];\n"
    "uniform mat4 uTest;\n"
    "uniform float uStartid;\n"
    "uniform sampler1D tConnections;\n"
    "uniform sampler1D tWallcolor;\n"
    "uniform sampler2D tWallTexture;\n"
    "uniform sampler1D tMatrixid;\n"
    "uniform vec4 uWallX[60];\n"
    "uniform vec4 uWallY[60];\n"
    "uniform int uWallstart[16];\n";
    
    if(IN_ODS) fsh += 

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
         ");}\n";    
    
   fsh += 
     "vec2 map_texture(vec4 pos, int which) {\n";
   if(hyperbolic) fsh += 
       "pos /= pos.w;\n";
   fsh += 
       "int s = uWallstart[which];\n"
       "int e = uWallstart[which+1];\n"
       "for(int i=s; i<e; i++) {\n"
         "vec2 v = vec2(dot(uWallX[i], pos), dot(uWallY[i], pos));\n"
         "if(v.x >= 0. && v.y >= 0. && v.x + v.y <= 1.) return v;\n"
         "}\n"
       "return vec2(0.5, 0.5);\n"
       "}\n";
    
   string fmain = "void main() {\n";
    
    if(IN_ODS) fmain +=
    "  float lambda = at[0];\n" // -PI to PI
    "  float phi;\n"
    "  float eye;\n"
    "  if(at.y < 0.) { phi = at.y + PI/2.; eye = uIPD / 2.; }\n" // right
    "  else { phi = at.y - PI/2.; eye = -uIPD / 2.; }\n"
    "  mat4 vw = uStart * xzspin(-lambda) * xpush(eye) * yzspin(phi);\n"
    "  vec4 at0 = vec4(0., 0., 1., 0.);\n";
    
    else fmain += 
    "  mat4 vw = uStart;\n"
    "  vec4 at0 = at;\n"
    "  at0.y = -at.y;\n"
    "  at0.w = 0.;\n"
    "  at0.xyz = at0.xyz / length(at0.xyz);\n";
      
    if(hyperbolic) fsh += "  float len(vec4 x) { return x[3]; }\n";
    else fsh += "  float len(vec4 x) { return length(x.xyz); }\n";
    
    fmain +=     
      "  vec4 position = vw * vec4(0., 0., 0., 1.);\n"
      "  vec4 tangent = vw * at0;\n"
      "  float go = 0.;\n"
      "  float cid = uStartid;\n"
      "  for(int iter=0; iter<600; iter++) {\n";
    
    if(sol) fmain +=
      "  float dist = .01;\n";
    else fmain +=
      "  float dist = 100.;\n";
    
    fmain +=
      "  int which = -1;\n";
      
    if(IN_ODS) fmain += 
      "  if(go == 0.) {\n"
      "    float best = len(position);\n"
      "    for(int i=0; i<uN; i++) {\n"
      "      float cand = len(uM[i] * position);\n"
      "      if(cand < best - .001) { dist = 0.; best = cand; which = i; }\n"
      "      }\n"
      "    }\n";
    
    if(sol) {
      fsh += "uniform float uBinaryWidth;\n";

      fsh += 
        "vec4 christoffel(vec4 pos, vec4 vel, vec4 tra) {\n"
        "  return vec4(-vel.z*tra.x - vel.x*tra.z, vel.z*tra.y + vel.y * tra.z, vel.x*tra.x * exp(2.*pos.z) - vel.y * tra.y * exp(-2.*pos.z), 0.);\n"
        "  }\n";

      fsh +=
        "vec4 get_pos(vec4 pos, vec4 tan) {\n"
          "vec4 acc = christoffel(pos, tan, tan);\n"
          "vec4 pos2 = pos + tan / 2.;\n"
          "vec4 tan2 = tan + acc / 2.;\n"
          "vec4 acc2 = christoffel(pos2, tan2, tan2);\n"
          "return pos + tan + acc2 / 2.;\n"
          "}\n";
        
      fsh +=
        "float hp_distance(vec4 pos, vec4 tan, int axis, float value) {\n"
        "  if(value > 0. && pos[axis] > value) return 0.;\n"
        "  if(value < 0. && pos[axis] < value) return 0.;\n"
        "  if(tan[axis] == 0.) return .5;\n"
        "  float approx = (value - pos[axis]) / tan[axis];\n"
        "  for(int it=0; it<3; it++) {\n"
        "    if(approx <= 0. || approx  > .5) return .5;\n"
        "    vec4 npos = get_pos(pos, tan * approx);\n"
        "    approx = approx * (value - pos[axis]) / (npos[axis] - pos[axis]);\n"
        "    }\n"
        "  if(approx < .001) approx = .001;\n"
        "  return approx;\n"
        "  }\n";

      fmain +=
      "  float d;\n"
      "  d = hp_distance(position, tangent, 0, uBinaryWidth);\n"
      "  if(d < dist) { dist = d; which = 0; }\n"
      "  d = hp_distance(position, tangent, 0, -uBinaryWidth);\n"
      "  if(d < dist) { dist = d; which = 4; }\n"
      "  d = hp_distance(position, tangent, 1, uBinaryWidth);\n"
      "  if(d < dist) { dist = d; which = 1; }\n"
      "  d = hp_distance(position, tangent, 1, -uBinaryWidth);\n"
      "  if(d < dist) { dist = d; which = 5; }\n"
      "  d = hp_distance(position, tangent, 2, log(2.)/2.);\n"
      "  if(d < dist) { dist = d; which = 2; }\n"
      "  d = hp_distance(position, tangent, 2, -log(2.)/2.);\n"
      "  if(d < dist) { dist = d; which = 6; }\n";
      }
    
    else {
    
      fmain +=
        "  if(which == -1) for(int i=0; i<uN; i++) {\n";
      
      if(hyperbolic) fmain +=
          "    float v = ((position - uM[i] * position)[3] / (uM[i] * tangent - tangent)[3]);\n"
          "    if(v > 1. || v < -1.) continue;\n"
          "    float d = atanh(v);\n"
          "    vec4 next_tangent = position * sinh(d) + tangent * cosh(d);\n"
          "    if(next_tangent[3] < (uM[i] * next_tangent)[3]) continue;\n";
      else fmain += 
          "    float deno = dot(position, tangent) - dot(uM[i]*position, uM[i]*tangent);\n"
          "    if(deno < 1e-6  && deno > -1e-6) continue;\n"
          "    float d = (dot(uM[i]*position, uM[i]*position) - dot(position, position)) / 2. / deno;\n"
          "    if(d < 0.) continue;\n"
          "    vec4 next_position = position + d * tangent;\n"
          "    if(dot(next_position, tangent) < dot(uM[i]*next_position, uM[i]*tangent)) continue;\n";
  
      fmain += 
          "  if(d < dist) { dist = d; which = i; }\n"
            "}\n";
      }
        
    fmain += 
      "  if(dist < 0.) { dist = 0.; }\n";
    
    fmain +=
      "  if(which == -1 && dist == 0.) { gl_FragColor = vec4(0., 0., 0., 1.); return; }";
    
    // shift d units
    
    fmain += "  go = go + dist;\n";
    
    if(hyperbolic) fmain += 
      "  float ch = cosh(dist); float sh = sinh(dist);\n"
      "  vec4 v = position * ch + tangent * sh;\n"
      "  tangent = tangent * ch + position * sh;\n"
      "  position = v;\n";
    else if(sol) {
      fmain +=
        "vec4 acc = christoffel(position, tangent, tangent);\n"
        "vec4 pos2 = position + tangent * dist / 2.;\n"
        "vec4 tan2 = tangent + acc * dist / 2.;\n"
        "vec4 acc2 = christoffel(pos2, tan2, tan2);\n"
        "position = position + tangent * dist + acc2 / 2. * dist * dist;\n"
        "tangent = tangent + acc * dist;\n";
      }
    else fmain += 
      "position = position + tangent * dist;\n";
    
    fmain += "if(which == -1) continue;\n";
    
    if(sol) {
      fmain += 
        "if(which == 2 && position.x > 0.) which = 3;\n"
        "if(which == 6 && position.y > 0.) which = 7;\n";
      }

    // apply wall color
    fmain +=
      "  float u = cid + float(which) / float(uLength);\n"
      "  vec4 col = texture1D(tWallcolor, u);\n"
      "  if(col[3] > 0.0) {\n"
      "    vec2 inface = map_texture(position, which);\n"
      "    float bright = min(1., 10. * (1.-inface.x-inface.y));\n" // texture2D(tWallTexture, (inface.yz + vec2(1.,1.))/2.).r;\n"
//      "    col.xyz = col.xyz * bright * max(1.-go/7., 0.5 * exp(-go/4.));\n" // exp(-go/4.);\n"
      "    col.xyz = col.xyz * bright * exp(- go / 20.);\n"
//      "    col.xyz = col.xyz * bright * max(1.-go/7., 0.5 * exp(-go/4.));\n" // exp(-go/4.);\n"
      "    col.w = 1.;\n"
//      "    if(purp == 1) { col.rgb = (col.rgb + vec3(1., 0., 1.)) / 2; }\n"
      "    gl_FragColor = col;\n"
      "    return;\n"
      "    }\n";

    // next cell
    fmain += 
      "  float rot = texture1D(tMatrixid, u).r;\n"
      "  int mid = int(rot * float(uLength));\n"
      "  position = uM[mid] * position;\n"
      "  tangent = uM[mid] * tangent;\n"
      "  cid = texture1D(tConnections, u).r;\n";
    
    fmain += 
      "  }"
      "  gl_FragColor = vec4(0.,0.,0.,1.); \n"
      "  }";

    fsh += fmain;    
 
    our_raycaster = make_shared<raycaster> (vsh, fsh);
    }
  full_enable(our_raycaster);
  }

void bind_array(vector<float>& v, GLint t, GLuint& tx, int id) {
  if(t == -1) println(hlog, "bind to nothing");
  glUniform1i(t, id);
  
  if(tx == 0) glGenTextures(1, &tx);

  glActiveTexture(GL_TEXTURE0 + id);
  glBindTexture(GL_TEXTURE_1D, tx);

  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  
  glTexImage1D(GL_TEXTURE_1D, 0, GL_R32F, isize(v), 0, GL_RED, GL_FLOAT, &v[0]);  
  }
  
void bind_array(vector<array<float, 4>>& v, GLint t, GLuint& tx, int id) {
  if(t == -1) println(hlog, "bind to nothing");
  glUniform1i(t, id);

  if(tx == 0) glGenTextures(1, &tx);

  glActiveTexture(GL_TEXTURE0 + id);
  glBindTexture(GL_TEXTURE_1D, tx);

  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  
  glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, isize(v), 0, GL_RGBA, GL_FLOAT, &v[0]);  
  GLERR("bind_array");
  }

EX void do_raycast() {
  enable_raycaster();

  auto& o = our_raycaster;
  
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
  
  int maxl = 32768 / S7;
  if(isize(lst) > maxl) lst.resize(maxl);
  
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
  if(euclid || sol)
    for(int j=0; j<S7; j++) ms.push_back(currentmap->relative_matrix(cwt.at->master, cwt.at->cmove(j)->master));
  else
    for(int j=0; j<S7; j++) inverse(reg3::adjmoves[j]);

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

  vector<GLint> wallstart;
  for(auto i: cgi.wallstart) wallstart.push_back(i);
  glUniform1iv(o->uWallstart, isize(wallstart), &wallstart[0]);  
  
  vector<glvertex> wallx, wally;
  for(auto& m: cgi.raywall) {
    wallx.push_back(glhr::pointtogl(m[0]));
    wally.push_back(glhr::pointtogl(m[1]));
    }
  
  glUniform4fv(o->uWallX, isize(wallx), &wallx[0][0]);
  glUniform4fv(o->uWallY, isize(wally), &wally[0][0]);

  if(o->uBinaryWidth != -1)
    glUniform1f(o->uBinaryWidth, vid.binary_width * log(2) / 2);

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
