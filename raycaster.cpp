// Hyperbolic Rogue -- raycaster
// Copyright (C) 2011-2019 Zeno Rogue, see 'hyper.cpp' for details

/** \file raycaster.cpp
 *  \brief A raycaster to draw walls.
 */

#include "hyper.h"

namespace hr {

EX namespace ray {

#if MAXMDIM >= 4

/** texture IDs */
GLuint txConnections = 0, txWallcolor = 0, txTextureMap = 0;

EX bool in_use;
EX bool comparison_mode;

/** 0 - never use, 2 - always use, 1 = smart selection */
EX int want_use = 1;

EX ld exp_start = 1, exp_decay_exp = 4, exp_decay_poly = 10;

EX ld maxstep_sol = .02;
EX ld maxstep_nil = .1;
EX ld minstep = .001;

EX ld reflect_val = 0;

static const int NO_LIMIT = 999999;

EX ld hard_limit = NO_LIMIT;

EX int max_iter_sol = 600, max_iter_iso = 60;

EX int max_cells = 2048;
EX bool rays_generate = true;

EX ld& exp_decay_current() {
  return (solnih || hyperbolic) ? exp_decay_exp : exp_decay_poly;
  }

EX int& max_iter_current() {
  if(nonisotropic) return max_iter_sol;
  else return max_iter_iso;
  }

ld& maxstep_current() {
  if(solnih) return maxstep_sol;
  else return maxstep_nil;
  }

#define IN_ODS 0

eGeometry last_geometry;

/** is the raycaster available? */
EX bool available() {
  if(noGUI) return false;
  if(!vid.usingGL) return false;
  if(WDIM == 2) return false;
  if(hyperbolic && pmodel == mdPerspective && !penrose)
    return true;
  if((solnih || nil) && pmodel == mdGeodesic)
    return true;
  if(euclid && pmodel == mdPerspective && !binarytiling)
    return true;
  if(prod && PURE)
    return true;
  return false;
  }

/** do we want to use the raycaster? */
EX bool requested() {
  if(!want_use) return false;
  #if CAP_TEXTURE
  if(texture::config.tstate == texture::tsActive) return false;
  #endif
  if(!available()) return false;
  if(want_use == 2) return true;
  return racing::on || quotient;
  }

struct raycaster : glhr::GLprogram {
  GLint uStart, uStartid, uM, uLength, uFovX, uFovY, uIPD;
  GLint uWallstart, uWallX, uWallY;
  GLint tConnections, tWallcolor, tTextureMap;
  GLint uBinaryWidth, uPLevel, uLP, uStraighten, uReflectX, uReflectY;
  GLint uLinearSightRange, uExpStart, uExpDecay;
  GLint uBLevel;
  
  raycaster(string vsh, string fsh) : GLprogram(vsh, fsh) {
    println(hlog, "assigning");
    uStart = glGetUniformLocation(_program, "uStart");
    uStartid = glGetUniformLocation(_program, "uStartid");
    uM = glGetUniformLocation(_program, "uM");
    uLength = glGetUniformLocation(_program, "uLength");
    uFovX = glGetUniformLocation(_program, "uFovX");
    uFovY = glGetUniformLocation(_program, "uFovY");
    uIPD = glGetUniformLocation(_program, "uIPD");

    uWallstart = glGetUniformLocation(_program, "uWallstart");
    uWallX = glGetUniformLocation(_program, "uWallX");
    uWallY = glGetUniformLocation(_program, "uWallY");
    
    uBinaryWidth = glGetUniformLocation(_program, "uBinaryWidth");
    uStraighten =  glGetUniformLocation(_program, "uStraighten");
    uPLevel = glGetUniformLocation(_program, "uPLevel");
    uLP = glGetUniformLocation(_program, "uLP");
    uReflectX = glGetUniformLocation(_program, "uReflectX");
    uReflectY = glGetUniformLocation(_program, "uReflectY");

    uLinearSightRange = glGetUniformLocation(_program, "uLinearSightRange");
    uExpDecay = glGetUniformLocation(_program, "uExpDecay");
    uExpStart = glGetUniformLocation(_program, "uExpStart");

    uBLevel = glGetUniformLocation(_program, "uBLevel");
  
    tConnections = glGetUniformLocation(_program, "tConnections");
    tWallcolor = glGetUniformLocation(_program, "tWallcolor");
    tTextureMap = glGetUniformLocation(_program, "tTextureMap");
    }
  };

shared_ptr<raycaster> our_raycaster;

EX void reset_raycaster() { our_raycaster = nullptr; };

int deg;

void enable_raycaster() {
  if(geometry != last_geometry) reset_raycaster();
  last_geometry = geometry;
  deg = S7; if(prod) deg += 2;
  if(!our_raycaster) { 
    bool asonov = hr::asonov::in();
    bool use_reflect = reflect_val && !nil && !levellines;

    string vsh = 
      "attribute vec4 aPosition;\n"
      "uniform float uFovX, uFovY;\n"
      "varying vec4 at;\n"
      "void main() { \n"
      "  gl_Position = aPosition; at = aPosition;\n"
  #if IN_ODS    
      "  at[0] *= PI; at[1] *= PI; \n"
  #else
      "  at[0] *= uFovX; at[1] *= uFovY; \n"
  #endif
      "  }\n";
  
    string rays = its(isize(cgi.raywall));
  
    string fsh = 
    "varying vec4 at;\n"
    "uniform int uLength;\n"
    "uniform float uIPD;\n"
    "uniform mat4 uStart;\n"
    "uniform mat4 uM[84];\n"
    "uniform mat4 uTest;\n"
    "uniform vec2 uStartid;\n"
    "uniform sampler2D tConnections;\n"
    "uniform sampler2D tWallcolor;\n"
    "uniform sampler2D tTexture;\n"
    "uniform sampler2D tTextureMap;\n"
    "uniform vec4 uWallX["+rays+"];\n"
    "uniform vec4 uWallY["+rays+"];\n"
    "uniform vec4 uFogColor;\n"
    "uniform int uWallstart["+its(deg+1)+"];\n"
    "uniform float uLinearSightRange, uExpStart, uExpDecay;\n";
    
    if(prod) fsh += 
      "uniform float uPLevel;\n"
      "uniform mat4 uLP;\n";
    
    int flat1 = 0, flat2 = S7;
    
    if(hyperbolic && binarytiling) {
      fsh += "uniform float uBLevel;\n";
      flat1 = binary::dirs_outer();
      flat2 -= binary::dirs_inner();
      }

    if(IN_ODS || hyperbolic) fsh += 

    "mat4 xpush(float x) { return mat4("
         "cosh(x), 0., 0., sinh(x),\n"
         "0., 1., 0., 0.,\n"
         "0., 0., 1., 0.,\n"
         "sinh(x), 0., 0., cosh(x)"
         ");}\n";
    
    if(IN_ODS) fsh += 

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
   if(nil) fsh += "if(which == 2 || which == 5) pos.z = 0.;\n";
   else if(hyperbolic && binarytiling) fsh += 
       "pos = vec4(-log(pos.w-pos.x), pos.y, pos.z, 1);\n"
       "pos.yz *= exp(pos.x);\n";
   else if(hyperbolic) fsh += 
       "pos /= pos.w;\n";
   else if(prod) fsh +=
     "pos = vec4(pos.x/pos.z, pos.y/pos.z, pos.w, 0);\n";
   
   fsh += 
       "int s = uWallstart[which];\n"
       "int e = uWallstart[which+1];\n"
       "for(int ix=0; ix<16; ix++) {\n"
         "int i = s+ix; if(i >= e) break;\n"
         "vec2 v = vec2(dot(uWallX[i], pos), dot(uWallY[i], pos));\n"
         "if(v.x >= 0. && v.y >= 0. && v.x + v.y <= 1.) return vec2(v.x+v.y, v.x-v.y);\n"
         "}\n"
       "return vec2(1, 1);\n"
       "}\n";
    
   string fmain = "void main() {\n";
   
   if(use_reflect) fmain += "  bool depthtoset = true;\n";
    
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
    "  gl_FragColor = vec4(0,0,0,1);\n"
    "  float left = 1.;\n"
    "  at0.y = -at.y;\n"
    "  at0.w = 0.;\n"
    "  at0.xyz = at0.xyz / length(at0.xyz);\n";
      
    if(hyperbolic) fsh += "  float len(vec4 x) { return x[3]; }\n";
    else fsh += "  float len(vec4 x) { return length(x.xyz); }\n";
    
    if(nonisotropic) fmain += 
      "  const float maxstep = " + fts(maxstep_current()) + ";\n"
      "  const float minstep = " + fts(minstep) + ";\n"
      "  float next = maxstep;\n";
    
    if(prod) {
      string sgn=in_h2xe() ? "-" : "+";
      fmain +=     
      "  vec4 position = vw * vec4(0., 0., 1., 0.);\n"
      "  vec4 at1 = uLP * at0;\n"
      "  float zpos = log(position.z*position.z"+sgn+"position.x*position.x"+sgn+"position.y*position.y)/2.;\n"
      "  position *= exp(-zpos);\n"
      "  float zspeed = at1.z;\n"
      "  float xspeed = length(at1.xy);\n"
      "  vec4 tangent = vw * exp(-zpos) * vec4(at1.xy, 0, 0) / xspeed;\n";
      }
    else fmain +=
      "  vec4 position = vw * vec4(0., 0., 0., 1.);\n"
      "  vec4 tangent = vw * at0;\n";
    
    fmain +=     
      "  float go = 0.;\n"
      "  vec2 cid = uStartid;\n"
      "  for(int iter=0; iter<" + its(max_iter_current()) + "; iter++) {\n";
    
    fmain +=
      "  float dist = 100.;\n";
    
    fmain +=
      "  int which = -1;\n";
      
    if(IN_ODS) fmain += 
      "  if(go == 0.) {\n"
      "    float best = len(position);\n"
      "    for(int i=0; i<"+its(S7)+"; i++) {\n"
      "      float cand = len(uM[i] * position);\n"
      "      if(cand < best - .001) { dist = 0.; best = cand; which = i; }\n"
      "      }\n"
      "    }\n";
    
    if(!nonisotropic) {
    
      fmain +=
        "  if(which == -1) {\n";
      
      fmain += "for(int i="+its(flat1)+"; i<"+its(flat2)+"; i++) {\n";
      
      if(in_h2xe()) fmain +=
          "    float v = ((position - uM[i] * position)[2] / (uM[i] * tangent - tangent)[2]);\n"
          "    if(v > 1. || v < -1.) continue;\n"
          "    float d = atanh(v);\n"
          "    vec4 next_tangent = position * sinh(d) + tangent * cosh(d);\n"
          "    if(next_tangent[2] < (uM[i] * next_tangent)[2]) continue;\n"
          "    d /= xspeed;\n";
      else if(in_s2xe()) fmain +=
          "    float v = ((position - uM[i] * position)[2] / (uM[i] * tangent - tangent)[2]);\n"
          "    float d = atan(v);\n"
          "    vec4 next_tangent = tangent * cos(d) - position * sin(d);\n"
          "    if(next_tangent[2] > (uM[i] * next_tangent)[2]) continue;\n"
          "    d /= xspeed;\n";
      else if(hyperbolic) fmain +=
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
    
      // 20: get to horosphere +uBLevel (take smaller root)
      // 21: get to horosphere -uBLevel (take larger root)
                          
      if(hyperbolic && binarytiling) {
        fmain += 
          "for(int i=20; i<22; i++) {\n"
            "float sgn = i == 20 ? -1. : 1.;\n"
            "vec4 zpos = xpush(uBLevel*sgn) * position;\n"
            "vec4 ztan = xpush(uBLevel*sgn) * tangent;\n"
            "float Mp = zpos.w - zpos.x;\n"
            "float Mt = ztan.w - ztan.x;\n"
            "float a = (Mp*Mp-Mt*Mt);\n"
            "float b = Mp/a;\n"
            "float c = (1.+Mt*Mt) / a;\n"
            "if(b*b < c) continue;\n"
            "if(sgn < 0. && Mt > 0.) continue;\n"
            "float zsgn = (Mt > 0. ? -sgn : sgn);\n"
            "float u = sqrt(b*b-c)*zsgn + b;\n"
            "float v = -(Mp*u-1.) / Mt;\n"
            "float d = asinh(v);\n"
            "if(d < 0. && abs(log(position.w*position.w-position.x*position.x)) < uBLevel) continue;\n"
            "if(d < dist) { dist = d; which = i; }\n"
            "}\n";
        }
          
      if(prod) fmain += 
        "if(zspeed > 0.) { float d = (uPLevel - zpos) / zspeed; if(d < dist) { dist = d; which = "+its(S7)+"+1; }}\n"
        "if(zspeed < 0.) { float d = (-uPLevel - zpos) / zspeed; if(d < dist) { dist = d; which = "+its(S7)+"; }}\n";
      
      fmain += "}\n";

      fmain += 
        "  if(dist < 0.) { dist = 0.; }\n";
      
      fmain +=
        "  if(which == -1 && dist == 0.) return;";    
      }
        
    // shift d units
    if(use_reflect) fmain += 
      "bool reflect = false;\n";
      
    if(in_h2xe()) fmain +=
      "  float ch = cosh(dist*xspeed); float sh = sinh(dist*xspeed);\n"
      "  vec4 v = position * ch + tangent * sh;\n"
      "  tangent = tangent * ch + position * sh;\n"
      "  position = v;\n"
      "  zpos += dist * zspeed;\n";
    else if(in_s2xe()) fmain +=
      "  float ch = cos(dist*xspeed); float sh = sin(dist*xspeed);\n"
      "  vec4 v = position * ch + tangent * sh;\n"
      "  tangent = tangent * ch - position * sh;\n"
      "  position = v;\n"
      "  zpos += dist * zspeed;\n";
    else if(hyperbolic) fmain += 
      "  float ch = cosh(dist); float sh = sinh(dist);\n"
      "  vec4 v = position * ch + tangent * sh;\n"
      "  tangent = tangent * ch + position * sh;\n"
      "  position = v;\n";
    else if(nonisotropic) {
    
      if(sol && nih) fsh += 
        "vec4 christoffel(vec4 pos, vec4 vel, vec4 tra) {\n"
        "  return vec4(-(vel.z*tra.x + vel.x*tra.z)*log(2.), (vel.z*tra.y + vel.y * tra.z)*log(3.), vel.x*tra.x * exp(2.*log(2.)*pos.z)*log(2.) - vel.y * tra.y * exp(-2.*log(3.)*pos.z)*log(3.), 0.);\n"
        "  }\n";
      else if(nih) fsh += 
        "vec4 christoffel(vec4 pos, vec4 vel, vec4 tra) {\n"
        "  return vec4((vel.z*tra.x + vel.x*tra.z)*log(2.), (vel.z*tra.y + vel.y * tra.z)*log(3.), -vel.x*tra.x * exp(-2.*log(2.)*pos.z)*log(2.) - vel.y * tra.y * exp(-2.*log(3.)*pos.z)*log(3.), 0.);\n"
        "  }\n";
      else if(sol) fsh += 
        "vec4 christoffel(vec4 pos, vec4 vel, vec4 tra) {\n"
        "  return vec4(-vel.z*tra.x - vel.x*tra.z, vel.z*tra.y + vel.y * tra.z, vel.x*tra.x * exp(2.*pos.z) - vel.y * tra.y * exp(-2.*pos.z), 0.);\n"
        "  }\n";
      else fsh +=
        "vec4 christoffel(vec4 pos, vec4 vel, vec4 tra) {\n"
        "  float x = pos.x;\n"
        "  return vec4(x*vel.y*tra.y - 0.5*dot(vel.yz,tra.zy), -.5*x*dot(vel.yx,tra.xy) + .5 * dot(vel.zx,tra.xz), -.5*(x*x-1.)*dot(vel.yx,tra.xy)+.5*x*dot(vel.zx,tra.xz), 0.);\n"
//        "  return vec4(0.,0.,0.,0.);\n"
        "  }\n";
      
      if(solnih && !asonov) fsh += "uniform float uBinaryWidth;\n";
      
      fmain += 
        "  dist = next < minstep ? 2.*next : next;\n";

      if(nil) fsh += 
        "vec4 translate(vec4 a, vec4 b) {\n"
          "return vec4(a[0] + b[0], a[1] + b[1], a[2] + b[2] + a[0] * b[1], b[3]);\n"
          "}\n"
        "vec4 translatev(vec4 a, vec4 t) {\n"
          "return vec4(t[0], t[1], t[2] + a[0] * t[1], 0.);\n"
          "}\n"
        "vec4 itranslate(vec4 a, vec4 b) {\n"
          "return vec4(-a[0] + b[0], -a[1] + b[1], -a[2] + b[2] - a[0] * (b[1]-a[1]), b[3]);\n"
          "}\n"
        "vec4 itranslatev(vec4 a, vec4 t) {\n"
          "return vec4(t[0], t[1], t[2] - a[0] * t[1], 0.);\n"
          "}\n";
                
      if(nil) fmain += "tangent = translate(position, itranslate(position, tangent));\n";
      
      if(solnih) fmain +=
        "vec4 acc = christoffel(position, tangent, tangent);\n"
        "vec4 pos2 = position + tangent * dist / 2.;\n"
        "vec4 tan2 = tangent + acc * dist / 2.;\n"
        "vec4 acc2 = christoffel(pos2, tan2, tan2);\n"
        "vec4 nposition = position + tangent * dist + acc2 / 2. * dist * dist;\n";
      
      if(nil) {
        fmain +=
          "vec4 xp, xt;\n"
          "vec4 back = itranslatev(position, tangent);\n"
          "if(back.x == 0. && back.y == 0.) {\n"
          "  xp = vec4(0., 0., back.z*dist, 1.);\n"
          "  xt = back;\n"
          "  }\n"
          "else if(abs(back.z) == 0.) {\n"
          "  xp = vec4(back.x*dist, back.y*dist, back.x*back.y*dist*dist/2., 1.);\n"
          "  xt = vec4(back.x, back.y, dist*back.x*back.y, 0.);\n"
          "  }\n"
          "else if(abs(back.z) < 1e-1) {\n"
// we use the midpoint method here, because the formulas below cause glitches due to float precision
          "  vec4 acc = christoffel(vec4(0,0,0,1), back, back);\n"
          "  vec4 pos2 = back * dist / 2.;\n"
          "  vec4 tan2 = back + acc * dist / 2.;\n"
          "  vec4 acc2 = christoffel(pos2, tan2, tan2);\n"
          "  xp = vec4(0,0,0,1) + back * dist + acc2 / 2. * dist * dist;\n"
          "  xt = back + acc * dist;\n"
          "  }\n"
          "else {\n"
          "  float alpha = atan2(back.y, back.x);\n"
          "  float w = back.z * dist;\n"
          "  float c = length(back.xy) / back.z;\n"
          "  xp = vec4(2.*c*sin(w/2.) * cos(w/2.+alpha), 2.*c*sin(w/2.)*sin(w/2.+alpha), w*(1.+(c*c/2.)*((1.-sin(w)/w)+(1.-cos(w))/w * sin(w+2.*alpha))), 1.);\n"
          "  xt = back.z * vec4("
               "c*cos(alpha+w),"
               "c*sin(alpha+w),"
               "1. + c*c*2.*sin(w/2.)*sin(alpha+w)*cos(alpha+w/2.),"
               "0.);\n"
          "  }\n"
          "vec4 nposition = translate(position, xp);\n";
        }
      
      if(nil) fmain +=
        "float rz = (abs(nposition.x) > abs(nposition.y) ?  -nposition.x*nposition.y : 0.) + nposition.z;\n";
      
      if(asonov) {
        fsh += "uniform mat4 uStraighten;\n";
        fmain += "vec4 sp = uStraighten * nposition;\n";
        }

      fmain +=
        "if(next >= minstep) {\n";
      
      if(asonov) fmain +=
          "if(abs(sp.x) > 1. || abs(sp.y) > 1. || abs(sp.z) > 1.) {\n";      
      else if(nih) fmain +=
          "if(abs(nposition.x) > uBinaryWidth || abs(nposition.y) > uBinaryWidth || abs(nposition.z) > .5) {\n";
      else if(sol) fmain +=
          "if(abs(nposition.x) > uBinaryWidth || abs(nposition.y) > uBinaryWidth || abs(nposition.z) > log(2.)/2.) {\n";
      else fmain +=
          "if(abs(nposition.x) > .5 || abs(nposition.y) > .5 || abs(rz) > .5) {\n";
      
      fmain +=
            "next = dist / 2.; continue;\n"
            "}\n"
          "if(next < maxstep) next = next / 2.;\n"
          "}\n"
        "else {\n";
      
      if(solnih) {
        if(asonov) fmain +=
          "if(sp.x > 1.) which = 4;\n"
          "if(sp.y > 1.) which = 5;\n"
          "if(sp.x <-1.) which = 10;\n"
          "if(sp.y <-1.) which = 11;\n"
          "if(sp.z > 1.) {\n"
            "float best = 999.;\n"
            "for(int i=0; i<4; i++) {\n"
              "float cand = len(uStraighten * uM[i] * position);\n"
              "if(cand < best) { best = cand; which = i;}\n"
              "}\n"
            "}\n"
          "if(sp.z < -1.) {\n"
            "float best = 999.;\n"
            "for(int i=6; i<10; i++) {\n"
              "float cand = len(uStraighten * uM[i] * position);\n"
              "if(cand < best) { best = cand; which = i;}\n"
              "}\n"
            "}\n";
        else if(sol && !nih) fmain +=
          "if(nposition.x > uBinaryWidth) which = 0;\n"
          "if(nposition.x <-uBinaryWidth) which = 4;\n"
          "if(nposition.y > uBinaryWidth) which = 1;\n"
          "if(nposition.y <-uBinaryWidth) which = 5;\n";
        if(nih) fmain += 
          "if(nposition.x > uBinaryWidth) which = 0;\n"
          "if(nposition.x <-uBinaryWidth) which = 2;\n"
          "if(nposition.y > uBinaryWidth) which = 1;\n"
          "if(nposition.y <-uBinaryWidth) which = 3;\n";
        if(sol && nih) fmain += 
          "if(nposition.z > .5) which = nposition.x > 0. ? 5 : 4;\n"
          "if(nposition.z <-.5) which = nposition.y > uBinaryWidth/3. ? 8 : nposition.y < -uBinaryWidth/3. ? 6 : 7;\n";
        if(nih && !sol) fmain += 
          "if(nposition.z > .5) which = 4;\n"
          "if(nposition.z < -.5) which = (nposition.y > uBinaryWidth/3. ? 9 : nposition.y < -uBinaryWidth/3. ? 5 : 7) + (nposition.x>0.?1:0);\n";
        if(sol && !nih && !asonov) fmain += 
          "if(nposition.z > log(2.)/2.) which = nposition.x > 0. ? 3 : 2;\n"
          "if(nposition.z <-log(2.)/2.) which = nposition.y > 0. ? 7 : 6;\n";
        }
      else fmain +=
          "if(nposition.x > .5) which = 3;\n"
          "if(nposition.x <-.5) which = 0;\n"
          "if(nposition.y > .5) which = 4;\n"
          "if(nposition.y <-.5) which = 1;\n"
          "if(rz > .5) which = 5;\n"
          "if(rz <-.5) which = 2;\n";
      
      fmain += 
          "next = maxstep;\n"
          "}\n";
      
      if(nil) fmain +=
        "tangent = translatev(position, xt);\n";

      fmain +=
        "position = nposition;\n";
      
      if(!nil) fmain +=
        "tangent = tangent + acc * dist;\n";
      }
    else fmain += 
      "position = position + tangent * dist;\n";
      
    #if CAP_FIX_RAYCAST
    if(hyperbolic) fmain +=
      "position /= sqrt(position.w*position.w - dot(position.xyz, position.xyz));\n"
      "tangent -= dot(vec4(-position.xyz, position.w), tangent) * position;\n"
      "tangent /= sqrt(dot(tangent.xyz, tangent.xyz) - tangent.w*tangent.w);\n";
    #endif
    
    if(hyperbolic && binarytiling) {
      fmain += 
        "if(which == 20) {\n"
        "  float best = 999.;\n"
        "  for(int i="+its(flat2)+"; i<"+its(S7)+"; i++) {\n"
          "  float cand = len(uM[i] * position);\n"
          "  if(cand < best) { best = cand; which = i; }\n"
          "  }\n"
          "}\n"
        "if(which == 21) {\n"
          "float best = 999.;\n"
          "for(int i=0; i<"+its(flat1)+"; i++) {\n"
          "  float cand = len(uM[i] * position);\n"
          "  if(cand < best) { best = cand; which = i; }\n"
          "  }\n"
//          "gl_FragColor = vec4(.5 + .5 * sin((go+dist)*100.), 1, float(which)/3., 1); return;\n"
          "}\n";
      }
    
    fmain += "  go = go + dist;\n";

    fmain += "if(which == -1) continue;\n";
        
    if(prod) fmain += "position.w = -zpos;\n";
    
    // apply wall color
    fmain +=
      "  vec2 u = cid + vec2(float(which) / float(uLength), 0);\n"
      "  vec4 col = texture2D(tWallcolor, u);\n"
      "  if(col[3] > 0.0) {\n";

    if(hard_limit < NO_LIMIT)
      fmain += "    if(go > float(" + fts(hard_limit) + ")) { gl_FragDepth = 1.; return; }\n";
    
    if(!(levellines && disable_texture)) fmain +=
      "    vec2 inface = map_texture(position, which);\n"
      "    vec3 tmap = texture2D(tTextureMap, u).rgb;\n"
      "    if(tmap.z == 0.) col.xyz *= min(1., (1.-inface.x)/ tmap.x);\n"
      "    else {\n"
      "      vec2 inface2 = tmap.xy + tmap.z * inface;\n"
      "      col.xyz *= texture2D(tTexture, inface2).rgb;\n"
      "      }\n";

    fmain +=
      "    float d = max(1. - go / uLinearSightRange, uExpStart * exp(-go / uExpDecay));\n"
      "    col.xyz = col.xyz * d + uFogColor.xyz * (1.-d);\n";
    
    if(nil) fmain +=
      "    if(abs(abs(position.x)-abs(position.y)) < .005) col.xyz /= 2.;\n";
    
    if(use_reflect) fmain +=
      "  if(col.w == 1.) {\n"
      "    col.w = float("+fts(1-reflect_val)+");\n"
      "    reflect = true;\n"
      "    }\n";
    
    ld vnear = glhr::vnear_default;
    ld vfar = glhr::vfar_default;

    fmain +=
      "    gl_FragColor.xyz += left * col.xyz * col.w;\n";

    if(use_reflect) fmain +=
      "    if(reflect && depthtoset) {\n";
    else fmain +=
      "    if(col.w == 1.) {\n";
    
    if(hyperbolic) fmain +=
      "      float z = at0.z * sinh(go);\n"
      "      float w = 1.;\n";
    else fmain +=
      "      float z = at0.z * go;\n"
      "      float w = 1.;\n";

    if(levellines) {
      if(hyperbolic) 
        fmain += "gl_FragColor.xyz *= 0.5 + 0.5 * cos(z/cosh(go) * uLevelLines * 2. * PI);\n";
      else
        fmain += "gl_FragColor.xyz *= 0.5 + 0.5 * cos(z * uLevelLines * 2. * PI);\n";
      fsh += "uniform float uLevelLines;\n";
      }

    fmain +=    
      "      gl_FragDepth = (-float("+fts(vnear+vfar)+")+w*float("+fts(2*vnear*vfar)+")/z)/float("+fts(vnear-vfar)+");\n"
      "      gl_FragDepth = (gl_FragDepth + 1.) / 2.;\n";
    
    if(!use_reflect) fmain +=
      "      return;\n";
    else fmain +=
      "      depthtoset = false;\n";

    fmain +=    
      "      }\n"
      "    left *= (1. - col.w);\n"
      "    }\n";

    if(use_reflect) {
      if(prod) fmain += "if(reflect && which >= "+its(S7)+") { zspeed = -zspeed; continue; }\n";
      if(hyperbolic && binarytiling) fmain +=
        "if(reflect && (which < "+its(flat1)+" || which >= "+its(flat2)+")) {\n"
        "  float x = -log(position.w - position.x);\n"
        "  vec4 xtan = xpush(-x) * tangent;\n"
        "  float diag = (position.y*position.y+position.z*position.z)/2.;\n"
        "  vec4 normal = vec4(1.-diag, -position.y, -position.z, -diag);\n"
        "  float mdot = dot(xtan.xyz, normal.xyz) - xtan.w * normal.w;\n"
        "  xtan = xtan - normal * mdot * 2.;\n"
        "  tangent = xpush(x) * xtan;\n"
        "  continue;\n"
        "  }\n";
      if(asonov) {
        fmain += 
          "  if(reflect) {\n"
          "    if(which == 4 || which == 10) tangent = refl(tangent, position.z, uReflectX);\n"
          "    else if(which == 5 || which == 11) tangent = refl(tangent, position.z, uReflectY);\n"
          "    else tangent.z = -tangent.z;\n"
          "    }\n";
        fsh += 
          "uniform vec4 uReflectX, uReflectY;\n"
          "vec4 refl(vec4 t, float z, vec4 r) {\n"
            "t.x *= exp(z); t.y /= exp(z);\n"
            "t -= dot(t, r) * r;\n"
            "t.x /= exp(z); t.y *= exp(z);\n"
            "return t;\n"
            "}\n";           
        }
      else if(sol && !nih && !asonov) fmain += 
        "  if(reflect) {\n"
        "    if(which == 0 || which == 4) tangent.x = -tangent.x;\n"
        "    else if(which == 1 || which == 5) tangent.y = -tangent.y;\n"
        "    else tangent.z = -tangent.z;\n"
        "    continue;\n"
        "    }\n";
      else if(nih) fmain += 
        "  if(reflect) {\n"
        "    if(which == 0 || which == 2) tangent.x = -tangent.x;\n"
        "    else if(which == 1 || which == 3) tangent.y = -tangent.y;\n"
        "    else tangent.z = -tangent.z;\n"
        "    continue;\n"
        "    }\n";
      else fmain += 
        "  if(reflect) {\n"
        "    tangent = uM["+its(deg)+"+which] * tangent;\n"
        "    continue;\n"
        "    }\n";
      }
    
    // next cell
    fmain += 
      "  vec4 connection = texture2D(tConnections, u);\n"
      "  cid = connection.xy;\n";
    
    if(prod) fmain +=
      "  if(which == "+its(S7)+") { zpos += uPLevel+uPLevel; continue; }\n"
      "  if(which == "+its(S7)+"+1) { zpos -= uPLevel+uPLevel; continue; }\n";
    
    fmain +=
      "  int mid = int(connection.z * 1024.);\n"
      "  position = uM[mid] * uM[which] * position;\n"
      "  tangent = uM[mid] * uM[which] *  tangent;\n";
    
    fmain += 
      "  }\n"
      "  gl_FragColor.xyz += left * uFogColor.xyz;\n";

    if(use_reflect) fmain +=
      "  if(depthtoset) gl_FragDepth = 1.;\n";
    else fmain +=
      "  gl_FragDepth = 1.;\n";

    fmain += 
      "  }";

    fsh += fmain;    
 
    our_raycaster = make_shared<raycaster> (vsh, fsh);
    }
  full_enable(our_raycaster);
  }

int length, per_row, rows;

void bind_array(vector<array<float, 4>>& v, GLint t, GLuint& tx, int id) {
  if(t == -1) println(hlog, "bind to nothing");
  glUniform1i(t, id);

  if(tx == 0) glGenTextures(1, &tx);

  glActiveTexture(GL_TEXTURE0 + id);
  glBindTexture(GL_TEXTURE_2D, tx);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  
  glTexImage2D(GL_TEXTURE_2D, 0, 0x8814 /* GL_RGBA32F */, length, isize(v)/length, 0, GL_RGBA, GL_FLOAT, &v[0]);  
  GLERR("bind_array");
  }

void uniform2(GLint id, array<float, 2> fl) {
  glUniform2f(id, fl[0], fl[1]);
  }

array<float, 2> enc(int i, int a) { 
  array<float, 2> res;
  res[0] = ((i%per_row) * deg + a + .5) / length;
  res[1] = ((i / per_row) + .5) / rows;
  return res;
  };

color_t color_out_of_range = 0xFF0080FF;

EX void cast() {
  enable_raycaster();
  
  if(comparison_mode) 
    glColorMask( GL_TRUE,GL_FALSE,GL_FALSE,GL_TRUE );

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
  
  deg = S7;
  if(prod) deg += 2;
  
  length = 4096;
  per_row = length / deg;
  
  vector<cell*> lst;

  cell *cs = viewcenter();

  transmatrix T = cview();
  if(nonisotropic) T = nisot::local_perspective * T;
  if(prod) T = actualV(viewctr, T);
  T = inverse(T);

  virtualRebase(cs, T, true);
  
  if(true) {
    manual_celllister cl;
    cl.add(cs);
    bool optimize = !isWall3(cs);
    for(int i=0; i<isize(cl.lst); i++) {
      cell *c = cl.lst[i];
      if(racing::on && i > 0 && c->wall == waBarrier) continue;
      if(optimize && isWall3(c)) continue;
      forCellCM(c2, c) {
        if(rays_generate) setdist(c2, 7, c);
        cl.add(c2);
        if(isize(cl.lst) >= max_cells) goto finish;
        }
      }
    finish:
    lst = cl.lst;
    }
  
  rows = next_p2((isize(lst)+per_row-1) / per_row);
  
  map<cell*, int> ids;
  for(int i=0; i<isize(lst); i++) ids[lst[i]] = i;

  glUniform1i(o->uLength, length);
  GLERR("uniform length");
  
  glUniformMatrix4fv(o->uStart, 1, 0, glhr::tmtogl_transpose3(T).as_array());
  if(o->uLP != -1) glUniformMatrix4fv(o->uLP, 1, 0, glhr::tmtogl_transpose3(inverse(nisot::local_perspective)).as_array());
  GLERR("uniform start");
  uniform2(o->uStartid, enc(ids[cs], 0));
  GLERR("uniform startid");
  glUniform1f(o->uIPD, vid.ipd);
  GLERR("uniform IPD");
  
  vector<transmatrix> ms;
  for(int j=0; j<S7; j++) ms.push_back(
    asonov::in() ? asonov::adjmatrix((j+S7/2)%S7) :
    nil ? nilv::adjmatrix((j+S7/2)%S7) :
    prod ? currentmap->relative_matrix(cwt.at, cwt.at->cmove(j), Hypc) : currentmap->relative_matrix(cwt.at->master, cwt.at->cmove(j)->master));
  if(prod) ms.push_back(Id);
  if(prod) ms.push_back(Id);
  
  if(!sol && !nil && reflect_val) {
    for(int j=0; j<S7; j++) {
      transmatrix T = inverse(ms[j]);
      hyperpoint h = tC0(T);
      ld d = hdist0(h);
      transmatrix U = rspintox(h) * xpush(d/2) * MirrorX * xpush(-d/2) * spintox(h);
      ms.push_back(U);
      }
    }
  
  vector<array<float, 4>> connections(length * rows);
  vector<array<float, 4>> wallcolor(length * rows);
  vector<array<float, 4>> texturemap(length * rows);

  if(1) for(cell *c: lst) {
    int id = ids[c];
    forCellIdEx(c1, i, c) { 
      int u = (id/per_row*length) + (id%per_row * deg) + i;
      if(!ids.count(c1)) {
        wallcolor[u] = glhr::acolor(color_out_of_range | 0xFF);
        texturemap[u] = glhr::makevertex(0.1,0,0);
        continue;
        }
      auto code = enc(ids[c1], 0);
      connections[u][0] = code[0];
      connections[u][1] = code[1];
      if(isWall3(c1)) {
        celldrawer dd;
        dd.cw.at = c1;
        dd.setcolors();
        transmatrix Vf;
        dd.set_land_floor(Vf);
        color_t wcol = darkena(dd.wcol, 0, 0xFF);
        int dv = get_darkval(c1, c->c.spin(i));
        float p = 1 - dv / 16.;
        wallcolor[u] = glhr::acolor(wcol);
        for(int a: {0,1,2}) wallcolor[u][a] *= p;
        if(qfi.fshape) {
          texturemap[u] = floor_texture_map[qfi.fshape->id];
          }
        else
          texturemap[u] = glhr::makevertex(0.1,0,0);
        }
      else {
        color_t col = transcolor(c, c1, winf[c->wall].color) | transcolor(c1, c, winf[c1->wall].color);
        if(col == 0)
          wallcolor[u] = glhr::acolor(0);
        else {
          int dv = get_darkval(c1, c->c.spin(i));
          float p = 1 - dv / 16.;
          wallcolor[u] = glhr::acolor(col);
          for(int a: {0,1,2}) wallcolor[u][a] *= p;
          texturemap[u] = glhr::makevertex(0.001,0,0);
          }
        }
      
      if(prod && i >= S7) {
        connections[u][2] = (S7+.5) / 1024.;
        continue;
        }
      transmatrix T = (asonov::in() || nil) ? Id : (prod ? currentmap->relative_matrix(c, c1, C0) : currentmap->relative_matrix(c->master, c1->master)) * inverse(ms[i]);
      for(int k=0; k<=isize(ms); k++) {
        if(k < isize(ms) && !eqmatrix(ms[k], T)) continue;
        if(k == isize(ms)) ms.push_back(T);
        connections[u][2] = (k+.5) / 1024.;
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

  if(o->uLevelLines != -1)
    glUniform1f(o->uLevelLines, levellines);
  if(o->uBinaryWidth != -1)
    glUniform1f(o->uBinaryWidth, vid.binary_width/2 * (nih?1:log(2)));
  if(o->uStraighten != -1) {
    glUniformMatrix4fv(o->uStraighten, 1, 0, glhr::tmtogl_transpose(asonov::straighten).as_array());
    }
  if(o->uReflectX != -1) {
    auto h = glhr::pointtogl(tangent_length(spin(90*degree) * asonov::ty, 2));
    glUniform4fv(o->uReflectX, 1, &h[0]);
    h = glhr::pointtogl(tangent_length(spin(90*degree) * asonov::tx, 2));
    glUniform4fv(o->uReflectY, 1, &h[0]);
    }
  if(o->uPLevel != -1)
    glUniform1f(o->uPLevel, cgi.plevel / 2);
  if(o->uBLevel != -1)
    glUniform1f(o->uBLevel, log(binary::expansion()) / 2);
  
  glUniform1f(o->uLinearSightRange, sightranges[geometry]);
  glUniform1f(o->uExpDecay, exp_decay_current());
  glUniform1f(o->uExpStart, exp_start);


  vector<glhr::glmatrix> gms;
  for(auto& m: ms) gms.push_back(glhr::tmtogl_transpose3(m));
  glUniformMatrix4fv(o->uM, isize(gms), 0, gms[0].as_array());

  bind_array(wallcolor, o->tWallcolor, txWallcolor, 4);
  bind_array(connections, o->tConnections, txConnections, 3);
  bind_array(texturemap, o->tTextureMap, txTextureMap, 5);
  
  auto cols = glhr::acolor(darkena(backcolor, 0, 0xFF));
  glUniform4f(o->uFogColor, cols[0], cols[1], cols[2], cols[3]);

  glVertexAttribPointer(hr::aPosition, 4, GL_FLOAT, GL_FALSE, sizeof(glvertex), &screen[0]);
  if(ray::comparison_mode)
    glhr::set_depthtest(false);
  else {
    glhr::set_depthtest(true);
    glhr::set_depthwrite(true);
    }
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glActiveTexture(GL_TEXTURE0 + 0);
  glBindTexture(GL_TEXTURE_2D, floor_textures->renderedTexture);

  glDrawArrays(GL_TRIANGLES, 0, 6);
  GLERR("finish");
  }

EX void configure() {
  cmode = sm::SIDE | sm::MAYDARK;
  gamescreen(0);
  dialog::init(XLAT("raycasting configuration"));
  
  dialog::addBoolItem(XLAT("available in current geometry"), available(), 0);
  
  dialog::addBoolItem(XLAT("use raycasting?"), want_use == 2 ? true : in_use, 'u');
  if(want_use == 1) dialog::lastItem().value = XLAT("SMART");
  
  dialog::add_action([] {
    want_use++; want_use %= 3;
    });

  dialog::addBoolItem_action(XLAT("comparison mode"), comparison_mode, 'c');
  
  dialog::addSelItem(XLAT("exponential range"), fts(exp_decay_current()), 'r');
  dialog::add_action([&] {
    dialog::editNumber(exp_decay_current(), 0, 40, 0.25, 5, XLAT("exponential range"), 
      XLAT("brightness formula: max(1-d/sightrange, s*exp(-d/r))")
      );
    });

  dialog::addSelItem(XLAT("exponential start"), fts(exp_start), 's');
  dialog::add_action([&] {
    dialog::editNumber(exp_start, 0, 1, 0.1, 1, XLAT("exponential start"), 
      XLAT("brightness formula: max(1-d/sightrange, s*exp(-d/r))\n")
      );
    });

  if(hard_limit < NO_LIMIT)
    dialog::addSelItem(XLAT("hard limit"), fts(hard_limit), 'H');
  else
    dialog::addBoolItem(XLAT("hard limit"), false, 'H');
  dialog::add_action([&] {
    if(hard_limit >= NO_LIMIT) hard_limit = 10;
    dialog::editNumber(hard_limit, 0, 100, 1, 10, XLAT("hard limit"), "");
    dialog::reaction = reset_raycaster;
    dialog::extra_options = [] {
      dialog::addItem("no limit", 'N');
      dialog::add_action([] { hard_limit = NO_LIMIT; reset_raycaster(); });
      };
    });
  
  if(!nil) {
    dialog::addSelItem(XLAT("reflective walls"), fts(reflect_val), 'R');
    dialog::add_action([&] {
      dialog::editNumber(reflect_val, 0, 1, 0.1, 0, XLAT("reflective walls"), "");
      dialog::reaction = reset_raycaster;
      });
    }

  if(nonisotropic) {
    dialog::addSelItem(XLAT("max step"), fts(maxstep_current()), 'x');
    dialog::add_action([] {
      dialog::editNumber(maxstep_current(), 1e-6, 1, 0.1, sol ? 0.03 : 0.1, XLAT("max step"), "affects the precision of solving the geodesic equation in Solv");
      dialog::scaleLog();
      dialog::bound_low(1e-9);
      dialog::reaction = reset_raycaster;
      });

    dialog::addSelItem(XLAT("min step"), fts(minstep), 'n');
    dialog::add_action([] {
      dialog::editNumber(minstep, 1e-6, 1, 0.1, 0.001, XLAT("min step"), "how precisely should we find out when do cross the cell boundary");
      dialog::scaleLog();
      dialog::bound_low(1e-9);
      dialog::reaction = reset_raycaster;
      });
    }
  
  dialog::addSelItem(XLAT("iterations"), its(max_iter_current()), 's');
  dialog::add_action([&] {
    dialog::editNumber(max_iter_current(), 0, 600, 1, 60, XLAT("iterations"), "in H3/H2xE/E3 this is the number of cell boundaries; in nonisotropic, the number of simulation steps");
    dialog::reaction = reset_raycaster;
    });

  dialog::addSelItem(XLAT("max cells"), its(max_cells), 's');
  dialog::add_action([&] {
    dialog::editNumber(max_cells, 16, 131072, 0.1, 4096, XLAT("max cells"), "");
    dialog::scaleLog();
    dialog::extra_options = [] {
      dialog::addBoolItem_action("generate", rays_generate, 'G');
      dialog::addColorItem(XLAT("out-of-range color"), color_out_of_range, 'X');
      dialog::add_action([] { 
        dialog::openColorDialog(color_out_of_range); 
        dialog::dialogflags |= sm::SIDE;
        });
      };
    });

  edit_levellines('L');
  
  dialog::addBack();
  dialog::display();
  }

#if CAP_COMMANDLINE  
int readArgs() {
  using namespace arg;
           
  if(0) ;
  else if(argis("-ray-do")) {
    PHASEFROM(2);
    want_use = 2;
    }
  else if(argis("-ray-dont")) {
    PHASEFROM(2);
    want_use = 0;
    }
  else if(argis("-ray-smart")) {
    PHASEFROM(2);
    want_use = 1;
    }
  else if(argis("-ray-out")) {
    PHASEFROM(2); shift(); color_out_of_range = arghex();
    }
  else if(argis("-ray-comp")) {
    PHASEFROM(2);
    comparison_mode = true;
    }
  else if(argis("-ray-cells")) {
    PHASEFROM(2); shift();
    rays_generate = true;
    max_cells = argi();
    }
  else if(argis("-ray-reflect")) {
    PHASEFROM(2); 
    shift_arg_formula(reflect_val);
    }
  else if(argis("-ray-cells-no")) {
    PHASEFROM(2); shift();
    rays_generate = false;
    max_cells = argi();
    }
  else return 1;
  return 0;
  }

auto hook = addHook(hooks_args, 100, readArgs);
#endif
#endif

#if MAXMDIM == 3
EX always_false in_use;
EX always_false comparison_mode;
EX void reset_raycaster() { }
EX void cast() { }
#endif
EX }
}
