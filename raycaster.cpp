// Hyperbolic Rogue -- raycaster
// Copyright (C) 2011-2019 Zeno Rogue, see 'hyper.cpp' for details

/** \file raycaster.cpp
 *  \brief A raycaster to draw walls.
 */

#include "hyper.h"

namespace hr {

/** \brief raycaster */
EX namespace ray {

#if CAP_RAY

/** texture IDs */
GLuint txConnections = 0, txWallcolor = 0, txTextureMap = 0, txVolumetric = 0, txM = 0, txWall = 0, txPortalConnections = 0;

EX bool in_use;
EX bool comparison_mode;

/** 0 - never use, 2 - always use, 1 = smart selection */
EX int want_use = 1;

/** generate the map for raycasting just once */
EX bool fixed_map = false;

EX ld exp_start = 1;
EX ld exp_decay_exp = 4;
EX ld exp_decay_poly = 10;

#ifdef GLES_ONLY
const int gms_limit = 16; /* enough for Bringris -- need to do better */
#else
const int gms_limit = 110;
#endif

EX int gms_array_size = 16;

EX ld maxstep_sol = .05;
EX ld maxstep_nil = .1;
EX ld maxstep_pro = .5;
EX ld maxstep_intra = .05;
EX ld minstep = .001;

EX ld reflect_val = 0;

static const int NO_LIMIT = 999999;

EX ld hard_limit = NO_LIMIT;

EX int max_iter_intra = 600;
EX int max_iter_sol = 600;
EX int max_iter_iso = 60;
EX int max_iter_eyes = 200;

EX int max_cells = 2048;
EX bool rays_generate = true;

EX ld& exp_decay_current() {
  if(intra::in) return exp_decay_exp;
  if(fake::in()) return *FPIU(&exp_decay_current());
  return (sn::in() || hyperbolic || sl2) ? exp_decay_exp : exp_decay_poly;
  }

EX int& max_iter_current() {
  if(intra::in) return max_iter_intra;
  if(nonisotropic || stretch::in()) return max_iter_sol;
  else if(is_eyes()) return max_iter_eyes;
  else return max_iter_iso;
  }

EX bool is_eyes() {
  #if CAP_VR
  return vrhr::active() && vrhr::eyes == vrhr::eEyes::equidistant;
  #else
  return false;
  #endif
  }

EX bool is_stepbased() {
  return nonisotropic || stretch::in() || is_eyes();
  }

EX bool horos() {
  return (hyperbolic || in_h2xe()) && bt::in();
  }

ld& maxstep_current() {
  if(intra::in) return maxstep_intra;
  if(sn::in() || stretch::in()) return maxstep_sol;
  #if CAP_VR
  if(vrhr::active() && vrhr::eyes == vrhr::eEyes::equidistant)
    return maxstep_pro;
  #endif
  return maxstep_nil;
  }

#define IN_ODS 0

eGeometry last_geometry;

vector<pair<int, cell*>> used_sample_list() {
  if(intra::in) return intra::full_sample_list;
  return hybrid::gen_sample_list();
  }

bool need_many_cell_types() {
  return isize(used_sample_list()) > 2;
  }

/** is the raycaster available? */
EX bool available() {
  #if CAP_VR
  /* would need a completely different implementation */
  if(vrhr::active() && vrhr::eyes == vrhr::eEyes::equidistant) {
    if(reflect_val) return false;
    if(sol || stretch::in() || sl2) return false;
    }
  #endif
  if(noGUI) return false;
  if(!vid.usingGL) return false;
  if(GDIM == 2) return false;
  if(WDIM == 2 && (kite::in() || bt::in())) return false;
  #ifdef GLES_ONLY
  if(need_many_cell_types()) return false;
  if(!euclid && !gproduct && !nil) return false;
  #endif
  if(hyperbolic && pmodel == mdPerspective && !kite::in())
    return true;
  if(sphere && pmodel == mdPerspective && !rotspace)
    return true;
  if(nil && S7 == 8)
    return false;
  if((sn::in() || nil || sl2) && pmodel == mdGeodesic)
    return true;
  if(euclid && pmodel == mdPerspective && !bt::in())
    return true;
  if(gproduct)
    return true;
  if(pmodel == mdPerspective && stretch::in())
    return true;
  return false;
  }

/** do we want to use the raycaster? */
EX bool requested() {
  if(cgflags & qRAYONLY) return true;
  if(!want_use) return false;
  if(intra::in) return true;
  if(stretch::in() && sphere) return true;
  #if CAP_TEXTURE
  if(texture::config.tstate == texture::tsActive) return false;
  #endif
  if(!available()) return false;
  if(want_use == 2) return true;
  if(rotspace) return false; // not very good
  if(WDIM == 2) return false; // not very good
  return racing::on || quotient || fake::in();
  }

#if HDR
struct raycaster : glhr::GLprogram {
  GLint uStart, uStartid, uM, uLength, uIPD;
  GLint uWallstart, uWallX, uWallY;
  GLint tConnections, tWallcolor, tTextureMap, tVolumetric;
  GLint uBinaryWidth, uPLevel, uLP, uStraighten, uReflectX, uReflectY;
  GLint uLinearSightRange, uExpStart, uExpDecay;
  GLint uBLevel;
  GLint uWallOffset, uSides;
  GLint uITOA, uATOI;
  GLint uToOrig, uFromOrig;
  GLint uProjection;
  GLint uEyeShift, uAbsUnit;
  GLint uMirrorShift;
  
  GLint tM, uInvLengthM;
  GLint tWall, uInvLengthWall;
  
  GLint tPortalConnections;
  raycaster(string vsh, string fsh);
  };
#endif

#ifdef GLES_ONLY
bool m_via_texture = false;
bool wall_via_texture = false;
static const bool can_via_texture = false; /* textures are not precise enough ): */
#else
bool m_via_texture = true;
bool wall_via_texture = true;
static const bool can_via_texture = true;
#endif


raycaster::raycaster(string vsh, string fsh) : GLprogram(vsh, fsh) {

    /* need to set shader_flags to 0 so that attributes are not enabled */
    shader_flags = 0;

    uStart = glGetUniformLocation(_program, "uStart");
    uStartid = glGetUniformLocation(_program, "uStartid");
    uM = glGetUniformLocation(_program, "uM");
    uLength = glGetUniformLocation(_program, "uLength");
    uProjection = glGetUniformLocation(_program, "uProjection");
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
    uMirrorShift = glGetUniformLocation(_program, "uMirrorShift");

    uLinearSightRange = glGetUniformLocation(_program, "uLinearSightRange");
    uExpDecay = glGetUniformLocation(_program, "uExpDecay");
    uExpStart = glGetUniformLocation(_program, "uExpStart");

    uBLevel = glGetUniformLocation(_program, "uBLevel");
  
    tConnections = glGetUniformLocation(_program, "tConnections");
    tWallcolor = glGetUniformLocation(_program, "tWallcolor");
    tTextureMap = glGetUniformLocation(_program, "tTextureMap");
    tVolumetric = glGetUniformLocation(_program, "tVolumetric");

    tPortalConnections = glGetUniformLocation(_program, "tPortalConnections");

    tM = glGetUniformLocation(_program, "tM");
    uInvLengthM = glGetUniformLocation(_program, "uInvLengthM");
    
    tWall = glGetUniformLocation(_program, "tWall");
    uInvLengthWall = glGetUniformLocation(_program, "uInvLengthWall");
    
    uWallOffset = glGetUniformLocation(_program, "uWallOffset");
    uSides = glGetUniformLocation(_program, "uSides");
    
    uITOA = glGetUniformLocation(_program, "uITOA");
    uATOI = glGetUniformLocation(_program, "uATOI");
    uToOrig = glGetUniformLocation(_program, "uToOrig");
    uFromOrig = glGetUniformLocation(_program, "uFromOrig");

    uEyeShift = glGetUniformLocation(_program, "uEyeShift");
    uAbsUnit  = glGetUniformLocation(_program, "uAbsUnit");
    }

shared_ptr<raycaster> our_raycaster;

#ifdef GLES_ONLY
void add(string& tgt, string type, string name, int min_index, int max_index) {
  if(min_index >= max_index) ;
  else
  if(min_index + 1 == max_index)
    tgt += "{ return " + name + "[" + its(min_index) + "]; }";
  else {
    int mid = (min_index + max_index) / 2;
    tgt += "{ if(i<" + its(mid) + ") "; 
    add(tgt, type, name, min_index, mid); 
    tgt += " else ";
    add(tgt, type, name, mid, max_index); 
    tgt += " }";
    }
  }

string build_getter(string type, string name, int index) {
  string s = type + " get_" + name + "(int i) \n";
  add(s, type, name, 0, index);
  return s + "\n";
  }

#define GET(array, index) "get_" array "(" index ")"
#else
#define GET(array, index) array "[" index "]"
#endif

EX hookset<void(string&, string&)> hooks_rayshader;
EX hookset<bool(shared_ptr<raycaster>)> hooks_rayset;

tuple<
  #if CAP_VR
  int, vrhr::eEyes, 
  #endif
  string
  > raycaster_state() {
  return make_tuple(
    #if CAP_VR
    vrhr::state, 
    vrhr::eyes,
    #endif
    intra::in ? "INTRA" : cgi_string()
    );
  }

decltype(raycaster_state()) saved_state;

ld ray_scale = 8;
int max_wall_offset = 512;
int max_celltype = 64;

struct raygen {
  string fsh, vsh, fmain;

  void add_if(const string& seek, const string& function);

  int deg, irays;
  bool asonov;
  bool use_reflect;
  bool many_cell_types;
  bool eyes;

  string getM(string s) {
    if(m_via_texture)
      return "getM(" + s + ")";
    else
      return "uM[" + s + "]";
    };

  string getWall(string s, int coord) {
    if(wall_via_texture)
      return "getWall(" + s + "," + its(coord) + ")";
    else
      return "uWall" + string(coord?"Y" : "X") + "[" + s + "]";
    };

  string getWallstart(string s) {
    if(wall_via_texture)
      return "getWallstart(" + s + ")";
    else
      return "uWallstart[" + s + "]";
    };
  
  void compute_which_and_dist(int flat1, int flat2);
  void apply_reflect(int flat1, int flat2);
  void move_forward();
  void emit_intra_portal(int gid1, int gid2);
  void emit_iterate(int gid1);
  void emit_raystarter();
  void create();

  string f_xpush() { return hyperbolic ? "xpush_h3" : "xpush_s3"; }
  string f_len() { return hyperbolic ? "len_h3" : (sphere && rotspace) ? "len_sr" : sl2 ? "len_sl2" : sphere ? "len_s3" : "len_x"; }
  string f_len_prod() { return in_h2xe() ? "len_h2" : in_s2xe() ? "len_s2" : "len_e2"; }
  void add_functions();
  };

raygen our_raygen;

void raygen::add_if(const string& seek, const string& function) {
  if(fsh.find(seek) != string::npos || fmain.find(seek) != string::npos)
    fsh = function + fsh;
  }

void raygen::compute_which_and_dist(int flat1, int flat2) {
  using glhr::to_glsl;

  if(!is_stepbased()) {

    fmain +=
      "  if(which == -1) {\n";

    if(in_h2xe() && hybrid::underlying == gBinary4)
      fmain += "for(int i=2; i<=4; i+=2) {";
    else if(in_h2xe() && hybrid::underlying == gTernary)
      fmain += "for(int i=3; i<=5; i+=2) {";
    else if(in_h2xe() && hybrid::underlying == gBinaryTiling)
      fmain += "for(int i=0; i<=4; i++) if(i == 0 || i == 4) {";
    else
      fmain += "for(int i="+its(flat1)+"; i<"+(gproduct ? "sides-2" : ((WDIM == 2 || is_subcube_based(variation) || intra::in) && !bt::in()) ? "sides" : its(flat2))+"; i++) {\n";

    fmain += "    mediump mat4 m = " + getM("walloffset+i") + ";\n";

    if(in_h2xe()) fmain +=
        "    mediump float v = ((position - m * position)[2] / (m * tangent - tangent)[2]);\n"
        "    if(v > 1. || v < -1.) continue;\n"
        "    mediump float d = atanh(v);\n"
        "    mediump vec4 next_tangent = position * sinh(d) + tangent * cosh(d);\n"
        "    if(next_tangent[2] < (m * next_tangent)[2]) continue;\n"
        "    d /= xspeed;\n";
    else if(in_s2xe()) fmain +=
        "    mediump float v = ((position - m * position)[2] / (m * tangent - tangent)[2]);\n"
        "    mediump float d = atan(v);\n"
        "    mediump vec4 next_tangent = tangent * cos(d) - position * sin(d);\n"
        "    if(next_tangent[2] > (m * next_tangent)[2]) continue;\n"
        "    d /= xspeed;\n";
    else if(in_e2xe()) fmain +=
        "    mediump float deno = dot(position, tangent) - dot(m*position, m*tangent);\n"
        "    if(deno < 1e-6  && deno > -1e-6) continue;\n"
        "    mediump float d = (dot(m*position, m*position) - dot(position, position)) / 2. / deno;\n"
        "    if(d < 0.) continue;\n"
        "    mediump vec4 next_position = position + d * tangent;\n"
        "    if(dot(next_position, tangent) < dot(m*next_position, m*tangent)) continue;\n"
        "    d /= xspeed;\n";
    else if(hyperbolic) fmain +=
        "    mediump float v = ((position - m * position)[3] / (m * tangent - tangent)[3]);\n"
        "    if(v > 1. || v < -1.) continue;\n"
        "    mediump float d = atanh(v);\n"
        "    mediump vec4 next_tangent = position * sinh(d) + tangent * cosh(d);\n"
        "    if(next_tangent[3] < (m * next_tangent)[3]) continue;\n";
    else if(sphere) fmain +=
        "    mediump float v = ((position - m * position)[3] / (m * tangent - tangent)[3]);\n"
        "    mediump float d = atan(v);\n"
        "    mediump vec4 next_tangent = -position * sin(d) + tangent * cos(d);\n"
        "    if(next_tangent[3] > (m * next_tangent)[3]) continue;\n";
    else fmain +=
        "    mediump float deno = dot(position, tangent) - dot(m*position, m*tangent);\n"
        "    if(deno < 1e-6  && deno > -1e-6) continue;\n"
        "    mediump float d = (dot(m*position, m*position) - dot(position, position)) / 2. / deno;\n"
        "    if(d < 0.) continue;\n"
        "    mediump vec4 next_position = position + d * tangent;\n"
        "    if(dot(next_position, tangent) < dot(m*next_position, m*tangent)) continue;\n";

    fmain +=
        "  if(d < dist) { dist = d; which = i; }\n"
          "}\n";

    if(hyperbolic && reg3::ultra_mirror_in()) {
      fmain += "for(int i="+its(S7*2)+"; i<"+its(S7*2+isize(cgi.heptshape->vertices_only))+"; i++) {\n";
      fmain += "mat4 uMi = " + getM("i") + ";";
      fmain +=
        "    mediump float v = ((position - uMi * position)[3] / (uMi * tangent - tangent)[3]);\n"
        "    if(v > 1. || v < -1.) continue;\n"
        "    mediump float d = atanh(v);\n"
        "    mediump vec4 next_tangent = position * sinh(d) + tangent * cosh(d);\n"
        "    if(next_tangent[3] < (uMi * next_tangent)[3]) continue;\n"
        "    if(d < dist) { dist = d; which = i; }\n"
          "}\n";
      }

    // 20: get to horosphere +uBLevel (take smaller root)
    // 21: get to horosphere -uBLevel (take larger root)

    if(horos()) {
      string push = hyperbolic ? "xpush_h3" : "xpush_h2";
      string w = hyperbolic ? "w" : "z";
      fmain +=
        "for(int i=20; i<22; i++) {\n"
          "mediump float sgn = i == 20 ? -1. : 1.;\n"
          "mediump vec4 zpos = "+push+"(uBLevel*sgn) * position;\n"
          "mediump vec4 ztan = "+push+"(uBLevel*sgn) * tangent;\n"
          "mediump float Mp = zpos."+w+" - zpos.x;\n"
          "mediump float Mt = ztan."+w+" - ztan.x;\n"
          "mediump float a = (Mp*Mp-Mt*Mt);\n"
          "mediump float b = Mp/a;\n"
          "mediump float c = (1.+Mt*Mt) / a;\n"
          "if(b*b < c) continue;\n"
          "if(sgn < 0. && Mt > 0.) continue;\n"
          "mediump float zsgn = (Mt > 0. ? -sgn : sgn);\n"
          "mediump float u = sqrt(b*b-c)*zsgn + b;\n"
          "mediump float v = -(Mp*u-1.) / Mt;\n"
          "if(a < 1e-5) v = (1.-Mp*Mp) / (2. * Mt);\n"
          "mediump float d = asinh(v);\n";
      if(gproduct) fmain += "d /= xspeed;\n";
      fmain +=
          "if(d < 0. && abs(log(position."+w+"*position."+w+"-position.x*position.x)) < uBLevel) continue;\n"
          "if(d < dist) { dist = d; which = i; }\n"
          "}\n";
      }

    if(gproduct) fmain +=
      "if(zspeed > 0.) { mediump float d = (uPLevel - zpos) / zspeed; if(d < dist) { dist = d; which = sides-1; }}\n"
      "if(zspeed < 0.) { mediump float d = (-uPLevel - zpos) / zspeed; if(d < dist) { dist = d; which = sides-2; }}\n";

    fmain += "}\n";

    fmain +=
      "  if(dist < 0.) { dist = 0.; }\n";

    fmain +=
      "  if(which == -1 && dist == 0.) return;\n";
    }
  }

void raygen::move_forward() {
  using glhr::to_glsl;
  if(in_h2xe() && !is_stepbased()) fmain +=
    "  mediump float ch = cosh(dist*xspeed); mediump float sh = sinh(dist*xspeed);\n"
    "  mediump vec4 v = position * ch + tangent * sh;\n"
    "  tangent = tangent * ch + position * sh;\n"
    "  position = v;\n"
    "  zpos += dist * zspeed;\n";
  else if(in_s2xe() && !is_stepbased()) fmain +=
    "  mediump float ch = cos(dist*xspeed); mediump float sh = sin(dist*xspeed);\n"
    "  mediump vec4 v = position * ch + tangent * sh;\n"
    "  tangent = tangent * ch - position * sh;\n"
    "  position = v;\n"
    "  zpos += dist * zspeed;\n";
  else if(in_e2xe() && !is_stepbased()) fmain +=
    "  position = position + tangent * dist * xspeed;\n"
    "  zpos += dist * zspeed;\n";
  else if(hyperbolic && !is_stepbased()) fmain +=
    "  mediump float ch = cosh(dist); mediump float sh = sinh(dist);\n"
    "  mediump vec4 v = position * ch + tangent * sh;\n"
    "  tangent = tangent * ch + position * sh;\n"
    "  position = v;\n";
  else if(sphere && !is_stepbased()) fmain +=
    "  mediump float ch = cos(dist); mediump float sh = sin(dist);\n"
    "  mediump vec4 v = position * ch + tangent * sh;\n"
    "  tangent = tangent * ch - position * sh;\n"
    "  position = v;\n";
  else if(is_stepbased()) {

    bool use_christoffel = true;

    if(sol && nih) fsh +=
      "mediump vec4 christoffel(mediump vec4 pos, mediump vec4 vel, mediump vec4 tra) {\n"
      "  return vec4(-(vel.z*tra.x + vel.x*tra.z)*log(2.), (vel.z*tra.y + vel.y * tra.z)*log(3.), vel.x*tra.x * exp(2.*log(2.)*pos.z)*log(2.) - vel.y * tra.y * exp(-2.*log(3.)*pos.z)*log(3.), 0.);\n"
      "  }\n";
    else if(nih) fsh +=
      "mediump vec4 christoffel(mediump vec4 pos, mediump vec4 vel, mediump vec4 tra) {\n"
      "  return vec4((vel.z*tra.x + vel.x*tra.z)*log(2.), (vel.z*tra.y + vel.y * tra.z)*log(3.), -vel.x*tra.x * exp(-2.*log(2.)*pos.z)*log(2.) - vel.y * tra.y * exp(-2.*log(3.)*pos.z)*log(3.), 0.);\n"
      "  }\n";
    else if(sol) fsh +=
      "mediump vec4 christoffel(mediump vec4 pos, mediump vec4 vel, mediump vec4 tra) {\n"
      "  return vec4(-vel.z*tra.x - vel.x*tra.z, vel.z*tra.y + vel.y * tra.z, vel.x*tra.x * exp(2.*pos.z) - vel.y * tra.y * exp(-2.*pos.z), 0.);\n"
      "  }\n";
    else if(nil) {
      fsh +=
      "mediump vec4 christoffel(mediump vec4 pos, mediump vec4 vel, mediump vec4 tra) {\n"
      "  mediump float x = pos.x;\n"
      "  return vec4(x*vel.y*tra.y - 0.5*dot(vel.yz,tra.zy), -.5*x*dot(vel.yx,tra.xy) + .5 * dot(vel.zx,tra.xz), -.5*(x*x-1.)*dot(vel.yx,tra.xy)+.5*x*dot(vel.zx,tra.xz), 0.);\n"
//        "  return vec4(0.,0.,0.,0.);\n"
      "  }\n";
      use_christoffel = false;
      }
    else if(sl2 || stretch::in()) {
      if(sl2) {
        fsh += "mediump mat4 s_translate(vec4 h) {\n"
          "return mat4(h.w,h.z,h.y,h.x,-h.z,h.w,-h.x,h.y,h.y,-h.x,h.w,-h.z,h.x,h.y,h.z,h.w);\n"
          "}\n";
        }
      else {
        fsh += "mediump mat4 s_translate(vec4 h) {\n"
          "return mat4(h.w,h.z,-h.y,-h.x,-h.z,h.w,h.x,-h.y,h.y,-h.x,h.w,-h.z,h.x,h.y,h.z,h.w);\n"
          "}\n";
        }
      fsh += "mediump mat4 s_itranslate(vec4 h) {\n"
        "h.xyz = -h.xyz; return s_translate(h);\n"
        "}\n";
      if(stretch::mstretch) {
        fsh += "mediump vec4 christoffel(mediump vec4 pos, mediump vec4 vel, mediump vec4 tra) {\n"
          "vel = s_itranslate(toOrig * pos) * toOrig * vel;\n"
          "tra = s_itranslate(toOrig * pos) * toOrig * tra;\n"
          "return fromOrig * s_translate(toOrig * pos) * vec4(\n";

        for(int i=0; i<3; i++) {
          auto &c = stretch::ms_christoffel;
          fsh += "  0.";
          for(int j=0; j<3; j++)
          for(int k=0; k<3; k++)
            if(c[i][j][k])
              fsh += "  + vel["+its(j)+"]*tra["+its(k)+"]*" + to_glsl(c[i][j][k]);
          fsh += "  ,\n";
          }
        fsh += "  0);\n"
          "}\n";
        }
      else
        use_christoffel = false;
      }
    else use_christoffel = false;

    if(use_christoffel) fsh += "mediump vec4 get_acc(mediump vec4 pos, mediump vec4 vel) {\n"
      "  return christoffel(pos, vel, vel);\n"
      "  }\n";

    if(sn::in() && !asonov) fsh += "uniform mediump float uBinaryWidth;\n";

    fmain +=
      "  dist = next < minstep ? 2.*next : next;\n";

    if(nil && !use_christoffel) fsh +=
      "mediump vec4 translate(mediump vec4 a, mediump vec4 b) {\n"
        "return vec4(a[0] + b[0], a[1] + b[1], a[2] + b[2] + a[0] * b[1], b[3]);\n"
        "}\n"
      "mediump vec4 translatev(mediump vec4 a, mediump vec4 t) {\n"
        "return vec4(t[0], t[1], t[2] + a[0] * t[1], 0.);\n"
        "}\n"
      "mediump vec4 itranslate(mediump vec4 a, mediump vec4 b) {\n"
        "return vec4(-a[0] + b[0], -a[1] + b[1], -a[2] + b[2] - a[0] * (b[1]-a[1]), b[3]);\n"
        "}\n"
      "mediump vec4 itranslatev(mediump vec4 a, mediump vec4 t) {\n"
        "return vec4(t[0], t[1], t[2] - a[0] * t[1], 0.);\n"
        "}\n";

    // if(nil) fmain += "tangent = translate(position, itranslate(position, tangent));\n";

    if(use_christoffel) fmain +=
      "mediump vec4 vel = tangent * dist;\n"
      "mediump vec4 acc1 = get_acc(position, vel);\n"
      "mediump vec4 acc2 = get_acc(position + vel / 2., vel + acc1/2.);\n"
      "mediump vec4 acc3 = get_acc(position + vel / 2. + acc1/4., vel + acc2/2.);\n"
      "mediump vec4 acc4 = get_acc(position + vel + acc2/2., vel + acc3/2.);\n"
      "mediump vec4 nposition = position + vel + (acc1+acc2+acc3)/6.;\n";

    if((sl2 || stretch::in()) && use_christoffel) {
      if(sl2) fmain +=
        "nposition = nposition / sqrt(dot(position.zw, position.zw) - dot(nposition.xy, nposition.xy));\n";

      else if(stretch::in()) fmain +=
        "nposition = nposition / sqrt(dot(nposition, nposition));\n";
      }

    if((sl2 || stretch::in()) && !use_christoffel) {
      ld SV = stretch::not_squared();
      ld mul = (sphere?1:-1)-1/SV/SV;
      fmain +=
        "vec4 vel = s_itranslate(position) * tangent * dist;\n"
        "vec4 vel1 = vel; vel1.z *= " + to_glsl(stretch::not_squared()) + ";\n"
        "mediump float vlen = length(vel1.xyz);\n"
        "if(vel.z<0.) vlen=-vlen;\n"
        "float z_part = vel1.z/vlen;\n"
        "float x_part = sqrt(1.-z_part*z_part);\n"
        "const float SV = " + to_glsl(SV) + ";\n"
        "float rparam = x_part / z_part / SV;\n"
        "float beta = atan2(vel.y,vel.x);\n"
        "if(vlen<0.) beta += PI;\n"
        "mediump vec4 nposition, ntangent;\n";

      if(sl2) fmain +=
        "if(rparam > 1.) {\n"
          "float cr = 1./sqrt(rparam*rparam-1.);\n"
          "float sr = rparam*cr;\n"
          "float z = cr * " + to_glsl(mul) + ";\n"
          "float a = vlen / length(vec2(sr, cr/SV));\n"
          "float k = -a;\n"
          "float u = z*a;\n"
          "float xy = sr * sinh(k);\n"
          "float zw = cr * sinh(k);\n"
          "nposition = vec4("
            "-xy*cos(u+beta),"
            "-xy*sin(u+beta),"
            "zw*cos(u)-cosh(k)*sin(u),"
            "zw*sin(u)+cosh(k)*cos(u)"
            ");\n"

          "ntangent = vec4("
            "-sr*cosh(k)*k*cos(u+beta) + u*xy*sin(u+beta),"
            "-sr*cosh(k)*k*sin(u+beta) - u*xy*cos(u+beta),"
            "k*cr*cosh(k)*cos(u)-zw*sin(u)*u-k*sinh(k)*sin(u)-u*cosh(k)*cos(u),"
            "k*cr*cosh(k)*sin(u)+u*zw*cos(u)+k*sinh(k)*cos(u)-u*cosh(k)*sin(u)"
            ");\n"
          "}\n"
        "else {\n"
          "float r = atanh(rparam);\n"
          "float cr = cosh(r);\n"
          "float sr = sinh(r);\n"
          "float z = cr * "+to_glsl(mul)+";\n"
          "float a = vlen / length(vec2(sr, cr/SV));\n"
          "float k = -a;\n"
          "float u = z*a;\n"
          "float xy = sr * sin(k);\n"
          "float zw = cr * sin(k);\n"
          "ntangent = vec4("
             "-sr*cos(k)*k*cos(u+beta) + u*xy*sin(u+beta),"
             "-sr*cos(k)*k*sin(u+beta) - u*xy*cos(u+beta),"
             "k*cr*cos(k)*cos(u)-zw*sin(u)*u+k*sin(k)*sin(u)-u*cos(k)*cos(u),"
             "k*cr*cos(k)*sin(u)+zw*cos(u)*u-k*sin(k)*cos(u)-u*cos(k)*sin(u)"
             ");\n"
          "nposition = vec4("
             "-xy * cos(u+beta),"
             "-xy * sin(u+beta),"
             "zw * cos(u) - cos(k) * sin(u),"
             "zw * sin(u) + cos(k)*cos(u)"
             ");\n"
          "}\n";

      else fmain +=
        "if(true) {\n"
          "float r = atan(rparam);\n"
          "float cr = cos(r);\n"
          "float sr = sin(r);\n"
          "float z = cr * "+to_glsl(mul)+";\n"
          "float a = vlen / length(vec2(sr, cr/SV));\n"
          "float k = a;\n"
          "float u = z*a;\n"
          "float xy = sr * sin(k);\n"
          "float zw = cr * sin(k);\n"
          "ntangent = vec4("
             "sr*cos(k)*k*cos(u+beta) - u*xy*sin(u+beta),"
             "sr*cos(k)*k*sin(u+beta) + u*xy*cos(u+beta),"
             "k*cr*cos(k)*cos(u)-zw*sin(u)*u+k*sin(k)*sin(u)-u*cos(k)*cos(u),"
             "k*cr*cos(k)*sin(u)+zw*cos(u)*u-k*sin(k)*cos(u)-u*cos(k)*sin(u)"
             ");\n"
          "nposition = vec4("
             "xy * cos(u+beta),"
             "xy * sin(u+beta),"
             "zw * cos(u) - cos(k) * sin(u),"
             "zw * sin(u) + cos(k)*cos(u)"
             ");\n"
          "}\n";

      fmain +=
        "ntangent = ntangent / dist;\n"
        "ntangent = s_translate(position) * ntangent;\n"
        "nposition = s_translate(position) * nposition;\n";
      }

    if(nil && !use_christoffel && !eyes) {
      fmain +=
        "mediump vec4 xp, xt;\n"
        "mediump vec4 back = itranslatev(position, tangent);\n"
        "if(back.x == 0. && back.y == 0.) {\n"
        "  xp = vec4(0., 0., back.z*dist, 1.);\n"
        "  xt = back;\n"
        "  }\n"
        "else if(abs(back.z) == 0.) {\n"
        "  xp = vec4(back.x*dist, back.y*dist, back.x*back.y*dist*dist/2., 1.);\n"
        "  xt = vec4(back.x, back.y, dist*back.x*back.y, 0.);\n"
        "  }\n"
        "else if(abs(back.z) < 1e-1) {\n"
// we use the midpoint method here, because the formulas below cause glitches due to mediump float precision
        "  mediump vec4 acc = christoffel(vec4(0,0,0,1), back, back);\n"
        "  mediump vec4 pos2 = back * dist / 2.;\n"
        "  mediump vec4 tan2 = back + acc * dist / 2.;\n"
        "  mediump vec4 acc2 = christoffel(pos2, tan2, tan2);\n"
        "  xp = vec4(0,0,0,1) + back * dist + acc2 / 2. * dist * dist;\n"
        "  xt = back + acc * dist;\n"
        "  }\n"
        "else {\n"
        "  mediump float alpha = atan2(back.y, back.x);\n"
        "  mediump float w = back.z * dist;\n"
        "  mediump float c = length(back.xy) / back.z;\n"
        "  xp = vec4(2.*c*sin(w/2.) * cos(w/2.+alpha), 2.*c*sin(w/2.)*sin(w/2.+alpha), w*(1.+(c*c/2.)*((1.-sin(w)/w)+(1.-cos(w))/w * sin(w+2.*alpha))), 1.);\n"
        "  xt = back.z * vec4("
             "c*cos(alpha+w),"
             "c*sin(alpha+w),"
             "1. + c*c*2.*sin(w/2.)*sin(alpha+w)*cos(alpha+w/2.),"
             "0.);\n"
        "  }\n"
        "mediump vec4 nposition = translate(position, xp);\n";
      }

    if(asonov) {
      fsh += "uniform mediump mat4 uStraighten;\n";
      fmain += "mediump vec4 sp = uStraighten * nposition;\n";
      }

    if(eyes) {
      fmain +=
      "  mediump float t = go + dist;\n";
      fmain += gproduct ?
      "  mediump vec4 v = at1 * t;\n" :
      "  mediump vec4 v = at0 * t;\n";
      fmain +=
      "  v[3] = 1.;\n"
      "  mediump vec4 azeq = uEyeShift * v;\n";
      if(nil) fmain +=
        "  mediump float alpha = atan2(azeq.y, azeq.x);\n"
        "  mediump float w = azeq.z;\n"
        "  mediump float c = length(azeq.xy) / azeq.z;\n"
        "  mediump vec4 xp = vec4(2.*c*sin(w/2.) * cos(w/2.+alpha), 2.*c*sin(w/2.)*sin(w/2.+alpha), w*(1.+(c*c/2.)*((1.-sin(w)/w)+(1.-cos(w))/w * sin(w+2.*alpha))), 1.);\n"
        "  mediump vec4 orig_position = vw * vec4(0., 0., 0., 1.);\n"
        "  mediump vec4 nposition = translate(orig_position, xp);\n";
      else if(gproduct) {
        fmain +=
          "  mediump float alen_xy = length(azeq.xy);\n";
        fmain += "  mediump float nzpos = zpos + azeq.z;\n";
        if(in_h2xe()) {
          fmain += "  azeq.xy *= sinh(alen_xy) / alen_xy;\n";
          fmain += "  azeq.z = cosh(alen_xy);\n";
          }
        else if(in_s2xe()) {
          fmain += "  azeq.xy *= sin (alen_xy) / alen_xy;\n";
          fmain += "  azeq.z = cos(alen_xy);\n";
          }
        else {
          /* euclid */
          fmain += "  azeq.z = 1.;\n";
          }
        fmain += "azeq.w = 0.;\n";
        fmain +=
        "  mediump vec4 nposition = vw * azeq;\n";
        }
      else {
        fmain +=
          "  mediump float alen = length(azeq.xyz);\n";
        if(hyperbolic) fmain +=
          "  azeq *= sinh(alen) / alen;\n"
          "  azeq[3] = cosh(alen);\n";
        else if(sphere) fmain +=
          "  azeq *= sin(alen) / alen;\n"
          "  azeq[3] = cos(alen);\n";
        else /* euclid */ fmain +=
          "  azeq[3] = 1;\n";
        fmain +=
        "  mediump vec4 nposition = vw * azeq;\n";
        }
      }

    else if(hyperbolic) {
      fmain +=
      "  mediump float ch = cosh(dist); mediump float sh = sinh(dist);\n"
      "  mediump vec4 v = position * ch + tangent * sh;\n"
      "  mediump vec4 ntangent = tangent * ch + position * sh;\n"
      "  mediump vec4 nposition = v;\n";
      }

    else if(sphere && !stretch::in()) {
      fmain +=
      "  mediump float ch = cos(dist); mediump float sh = sin(dist);\n"
      "  mediump vec4 v = position * ch + tangent * sh;\n"
      "  mediump vec4 ntangent = tangent * ch - position * sh;\n"
      "  mediump vec4 nposition = v;\n";
      }

    bool reg = hyperbolic || sphere || euclid || sl2 || gproduct;

    if(reg) {
      string s = (rotspace || gproduct) ? "-2" : "";
      fmain +=
    "    mediump float best = "+f_len()+"(nposition);\n"
    "    for(int i=0; i<sides"+s+"; i++) {\n"
    "      mediump float cand = "+f_len()+"(" + getM("walloffset+i") + " * nposition);\n"
    "      if(cand < best) { best = cand; which = i; }\n"
    "      }\n";
      if(rotspace) fmain +=
    "   if(which == -1) {\n"
    "     best = len_rotspace(nposition);\n"
    "     mediump float cand1 = len_rotspace(" + getM("walloffset+sides-2") + "*nposition);\n"
    "     if(cand1 < best) { best = cand1; which = sides-2; }\n"
    "     mediump float cand2 = len_rotspace(" + getM("walloffset+sides-1") + "*nposition);\n"
    "     if(cand2 < best) { best = cand2; which = sides-1; }\n"
    "     }\n";
      if(gproduct) {
        fmain +=
        "if(nzpos > uPLevel) which = sides-1;\n"
        "if(nzpos <-uPLevel) which = sides-2;\n";
        }
      }

    if(nil) fmain +=
      "mediump float rz = (abs(nposition.x) > abs(nposition.y) ?  -nposition.x*nposition.y : 0.) + nposition.z;\n";

    fmain +=
      "if(next >= minstep) {\n";

    string hnilw = to_glsl(nilv::nilwidth / 2);
    string hnilw2 = to_glsl(nilv::nilwidth * nilv::nilwidth / 2);

    if(reg) fmain += "if(which != -1) {\n";
    else if(asonov) fmain +=
        "if(abs(sp.x) > 1. || abs(sp.y) > 1. || abs(sp.z) > 1.) {\n";
    else if(nih) fmain +=
        "if(abs(nposition.x) > uBinaryWidth || abs(nposition.y) > uBinaryWidth || abs(nposition.z) > .5) {\n";
    else if(sol) fmain +=
        "if(abs(nposition.x) > uBinaryWidth || abs(nposition.y) > uBinaryWidth || abs(nposition.z) > log(2.)/2.) {\n";
    else fmain +=
        "if(abs(nposition.x) > "+hnilw+" || abs(nposition.y) > "+hnilw+" || abs(rz) > "+hnilw2+") {\n";

    fmain +=
          "next = dist / 2.; continue;\n"
          "}\n"
        "if(next < maxstep) next = next / 2.;\n"
        "}\n"
      "else {\n";

    if(sn::in()) {
      if(asonov) fmain +=
        "if(sp.x > 1.) which = 4;\n"
        "if(sp.y > 1.) which = 5;\n"
        "if(sp.x <-1.) which = 10;\n"
        "if(sp.y <-1.) which = 11;\n"
        "if(sp.z > 1.) {\n"
          "mediump float best = 999.;\n"
          "for(int i=0; i<4; i++) {\n"
            "mediump float cand = "+f_len()+"(uStraighten * " + getM("i") + " * position);\n"
            "if(cand < best) { best = cand; which = i;}\n"
            "}\n"
          "}\n"
        "if(sp.z < -1.) {\n"
          "mediump float best = 999.;\n"
          "for(int i=6; i<10; i++) {\n"
            "mediump float cand = "+f_len()+"(uStraighten * " + getM("i") + " * position);\n"
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
    else if(nil) fmain +=
        "if(nposition.x > "+hnilw+") which = 3;\n"
        "if(nposition.x <-"+hnilw+") which = 0;\n"
        "if(nposition.y > "+hnilw+") which = 4;\n"
        "if(nposition.y <-"+hnilw+") which = 1;\n"
        "if(rz > "+hnilw2+") which = 5;\n"
        "if(rz <-"+hnilw2+") which = 2;\n";

    fmain +=
        "next = maxstep;\n"
        "}\n";

    if(use_christoffel) fmain +=
      "tangent = tangent + (acc1+2.*acc2+2.*acc3+acc4)/(6.*dist);\n";
    else if(nil && !eyes) fmain +=
      "tangent = translatev(position, xt);\n";
    else if(!eyes)
      fmain +=
      "tangent = ntangent;\n";

    if(!eyes) fmain +=
      "position = nposition;\n";
    else fmain += "vec4 position = nposition;\n";

    if((stretch::in() || sl2) && use_christoffel) {
      fmain +=
        "tangent = s_itranslate(toOrig * position) * toOrig * tangent;\n"
        "tangent[3] = 0.;\n";
      if(stretch::mstretch)
        fmain +=
          "float nvelsquared = dot(tangent.xyz, (uATOI * tangent).xyz);\n";
      else
        fmain +=
          "float nvelsquared = tangent.x * tangent.x + tangent.y * tangent.y + "
            + to_glsl(stretch::squared()) + " * tangent.z * tangent.z;\n";
      fmain +=
        "tangent /= sqrt(nvelsquared);\n"
        "tangent = fromOrig * s_translate(toOrig * position) * tangent;\n";
      }
    }
  else fmain +=
    "position = position + tangent * dist;\n";
  }

void raygen::apply_reflect(int flat1, int flat2) {
  if(gproduct) fmain += "if(reflect && which >= sides-2) { zspeed = -zspeed; continue; }\n";
  if(horos()) {
    fmain +=
    "if(reflect && (which < "+its(flat1)+" || which >= "+its(flat2)+")) {\n";
    if(hyperbolic) fmain +=
    "  mediump float x = -log(position.w - position.x);\n"
    "  mediump vec4 xtan = xpush_h3(-x) * tangent;\n"
    "  mediump float diag = (position.y*position.y+position.z*position.z)/2.;\n"
    "  mediump vec4 normal = vec4(1.-diag, -position.y, -position.z, -diag);\n"
    "  mediump float mdot = dot(xtan.xyz, normal.xyz) - xtan.w * normal.w;\n";
    else fmain +=
    "  mediump float x = -log(position.z - position.x);\n"
    "  mediump vec4 xtan = xpush_h2(-x) * tangent;\n"
    "  mediump float diag = position.y*position.y/2.;\n"
    "  mediump vec4 normal = vec4(1.-diag, -position.y, -diag, 0.);\n"
    "  mediump float mdot = dot(xtan.xy, normal.xy) - xtan.w * normal.w;\n";
    fmain +=
    "  xtan = xtan - normal * mdot * 2.;\n"
    "  tangent = xpush_h(x) * xtan;\n"
    "  continue;\n"
    "  }\n";
    }
  if(asonov) {
    fmain +=
      "  if(reflect) {\n"
      "    if(which == 4 || which == 10) tangent = refl(tangent, position.z, uReflectX);\n"
      "    else if(which == 5 || which == 11) tangent = refl(tangent, position.z, uReflectY);\n"
      "    else tangent.z = -tangent.z;\n"
      "    }\n";
    fsh +=
      "uniform mediump vec4 uReflectX, uReflectY;\n"
      "mediump vec4 refl(mediump vec4 t, float z, mediump vec4 r) {\n"
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
  else {
    fmain +=
    "  if(reflect) {\n"
    "    tangent = " + getM("uMirrorShift+walloffset+which") + " * tangent;\n"
    "    continue;\n"
    "    }\n";
    if(fsh.find("uMirrorShift") == string::npos)
      fsh += "uniform int uMirrorShift;\n";
    }
  }

void raygen::emit_intra_portal(int gid1, int gid2) {

  if(1) {
    intra::resetter ir;
    intra::switch_to(gid2);
    }

  if(gproduct) {
    fmain += "mediump vec4 nposition;\n";
    fmain += "if(pconnection.x != .5) {\n"; // kind != 0
    if(1) {
      string fn = bt::in() ? "to_poco_h2xr_b" : in_h2xe() ? "to_poco_h2xr_h" : "to_poco_s2xr_s";
      string tmul = "";
      if(!bt::in()) {
        fmain +=
        "    mediump mat4 tkt = " + getM("mid+1") + ";\n";
        tmul = "tkt*";
        }
      fmain +=
        "    nposition = position + tangent * xspeed * 1e-3;\n"
        "    position = "+fn+"("+tmul+"position);\n"
        "    position.z = 0.;\n" // zpos - uPLevel;\n"
        "    nposition = "+fn+"("+tmul+"nposition);\n"
        "    nposition.z = zspeed * 1e-3;\n"
        "    if(pconnection.y < .5) { nposition.z = -nposition.z; nposition.x = -nposition.x; position.x = -position.x; }\n";
      }
    fmain += "  } else {\n";
    if(1) {
      string fn = in_h2xe() ? "to_poco_h2xr_e" : "to_poco_s2xr_e";
      fmain +=
        "    nposition = position + tangent * xspeed * 1e-3;\n"
        "    mediump mat4 tkt = " + getM("mid+1") + ";\n"
        "    position = "+fn+"(tkt * position);\n"
        "    position.y = zpos;\n"
        "    nposition = "+fn+"(tkt * nposition);\n"
        "    nposition.y = zpos + zspeed * 1e-3;\n";
      }
    fmain += "  }\n";
    }
  else if(hyperbolic && bt::in()) {
    fmain +=
      "    mediump vec4 nposition = position + tangent * 1e-3;\n"
      "    mediump mat4 tkt = " + getM("mid+1") + ";\n"
      "    position = deparabolici13(position);\n"
      "    nposition = deparabolici13(nposition);\n"
      "    position = tkt * position;\n"
      "    nposition = tkt * nposition;\n"
      "    position.z *= exp(position.y);\n"
      "    nposition.z *= exp(nposition.y);\n";
    }
  else if(sol) {
    fmain +=
      "    mediump vec4 nposition = position + tangent * 1e-3;\n"
      "    mediump mat4 tkt = " + getM("mid+1") + ";\n"
      "    position = tkt * position;\n"
      "    nposition = tkt * nposition;\n"
      "    position.z *= exp(-position.y);\n"
      "    nposition.z *= exp(-nposition.y);\n";
    }
  else {
    fmain +=
      "    mediump vec4 nposition = position + tangent * 1e-3;\n"
      "    mediump mat4 tkt = " + getM("mid+1") + ";\n";
    if(hyperbolic) fmain +=
      "    position = to_poco_h3(tkt * position);\n"
      "    nposition = to_poco_h3(tkt * nposition);\n";
    else if(sphere) fmain +=
      "    position = to_poco_s3(tkt * position);\n"
      "    nposition = to_poco_s3(tkt * nposition);\n";
    else fmain +=
      "    position = tkt * position;\n"
      "    nposition = tkt * nposition;\n";
    }

  fmain +=
    "    mediump mat4 m = " + getM("mid") + ";\n"
    "    position = m * position;\n"
    "    nposition = m * nposition;\n";

  intra::resetter ir;
  intra::switch_to(gid2);

  if(gproduct) {
    fmain += "if(pconnection.z != .5) {\n"; // kind != 0
    if(1) {
      string sgn = in_h2xe() ? "-" : "+";
      string fn = bt::in() ? "from_poco_h2xr_b" : in_h2xe() ? "itkt*from_poco_h2xr_h" : "itkt*from_poco_s2xr_s";

      fmain +=
      "    if(pconnection.w < .5) { position.z = -position.z; nposition.z = -nposition.z; nposition.x = -nposition.x; position.x = -position.x; }\n"
      "    zspeed = (nposition.z - position.z) * 1e3;\n"
      "    zpos = position.z + (pconnection.w - .5) * 16.;\n"
      "    mediump mat4 itkt = " + getM("mid+2") + ";\n"
      "    position = "+fn+"(position);\n"
      "    nposition = "+fn+"(nposition);\n"
      "    tangent = (nposition - position) * 1e3;\n"
      "    mediump float pnorm = tangent.z * position.z "+sgn+" tangent.x * position.x "+sgn+" tangent.y * position.y;\n"
      "    tangent -= position * pnorm;\n"
      "    xspeed = sqrt(tangent.x * tangent.x + tangent.y * tangent.y "+sgn+" tangent.z * tangent.z);\n"
      "    tangent /= xspeed;\n";
      }
    fmain += "  } else {\n";
    if(1) {
      string sgn = in_h2xe() ? "-" : "+";
      string fn = in_h2xe() ? "from_poco_h2xr_e" : "from_poco_s2xr_e";
      fmain +=
      "    mediump mat4 itkt = " + getM("mid+2") + ";\n";
      fmain +=
      "    zpos = position.y;\n"
      "    zspeed = (nposition.y - zpos) * 1e3;\n"
      "    position = itkt * "+fn+"(position);\n"
      "    nposition = itkt * "+fn+"(nposition);\n"
      "    tangent = (nposition - position) * 1e3;\n"
      "    mediump float pnorm = tangent.z * position.z "+sgn+" dot(position.xy, tangent.xy);\n"
      "    tangent -= position * pnorm;\n"
      "    xspeed = sqrt(dot(tangent.xy, tangent.xy) "+sgn+" tangent.z * tangent.z);\n"
      "    tangent /= xspeed;\n"
      "    mediump float l = xspeed*xspeed+zspeed*zspeed;\n"
      "    xspeed /= sqrt(l); zspeed /= sqrt(l);\n";
      }
    fmain += "}\n";
    }
  else if(sol) {
    fmain +=
   "    mediump mat4 itkt = " + getM("mid+2") + ";\n";
    fmain +=
   "    position.z *= exp(position.y);\n"
   "    nposition.z *= exp(nposition.y);\n"
   "    position = itkt * position;\n"
   "    nposition = itkt * nposition;\n"
   "    tangent = (nposition - position) * 1e3;\n"
   "    float next = maxstep;\n";
    }
  else {
    fmain +=
    "    mediump mat4 itkt = " + getM("mid+2") + ";\n";

    if(hyperbolic || sphere) {
      string sgn = hyperbolic ? "-" : "+";
      if(hyperbolic && bt::in()) {
        fmain +=
       "    position.z *= exp(-position.y);\n"
       "    nposition.z *= exp(-nposition.y);\n"
       "    position = itkt * position;\n"
       "    nposition = itkt * nposition;\n"
       "    position = enparabolici13(position);\n"
       "    nposition = enparabolici13(nposition);\n";
         }
      else {
        string he = hyperbolic ? "from_poco_h3" : "from_poco_s3";
        fmain +=
    "    position = itkt * "+he+"(position);\n"
    "    nposition = itkt * "+he+"(nposition);\n";
        }
      fmain +=
    "    tangent = (nposition - position) * 1e3;\n"
    "    mediump float pnorm = position.w * position.w "+sgn+" dot(position.xyz, position.xyz);\n"
    "    position /= sqrt(pnorm);\n"
    "    pnorm = tangent.w * position.w "+sgn+" dot(position.xyz, tangent.xyz);\n"
    "    tangent -= position * pnorm;\n"
    "    mediump float xspeed = sqrt(dot(tangent.xyz, tangent.xyz) "+sgn+" tangent.w * tangent.w);\n"
    "    tangent /= xspeed;\n";
      }

    else fmain +=
    "    position = itkt * position;\n"
    "    nposition = itkt * nposition;\n"
    "    tangent = (nposition - position) * 1e3;\n"
    "    tangent /= dot(tangent.xyz, tangent.xyz);\n";
    }
  }

void raygen::emit_iterate(int gid1) {
  using glhr::to_glsl;

  if(intra::in && gproduct)
    fmain += "  const mediump float uPLevel = " + to_glsl(cgi.plevel/2) + ";\n";

  int flat1 = 0, flat2 = deg;
  if(gproduct || rotspace) flat2 -= 2;

#if CAP_BT
  if(horos()) {
    if(intra::in)
      fmain += "mediump float uBLevel = " + to_glsl(log(bt::expansion()) / 2) + ";\n";
    else
      fsh += "uniform mediump float uBLevel;\n";
    flat1 = bt::dirs_outer();
    flat2 -= bt::dirs_inner();
    }
#endif

  fmain +=
    "  mediump float dist = 100.;\n";

  fmain +=
    "  int which = -1;\n";

  if(intra::in) {
    if(hyperbolic) fmain += "iter += 44;\n";
    else if(!sol) fmain += "iter += 4;\n";
    }

  if(in_e2xe() && !eyes) fmain += "tangent.w = position.w = 0.;\n";

  if(IN_ODS) fmain +=
    "  if(go == 0.) {\n"
    "    mediump float best = "+f_len()+"(position);\n"
    "    for(int i=0; i<sides; i++) {\n"
    "      mediump float cand = "+f_len()+"(" + getM("i") + " * position);\n"
    "      if(cand < best - .001) { dist = 0.; best = cand; which = i; }\n"
    "      }\n"
    "    }\n";

  compute_which_and_dist(flat1, flat2);

  vid.fixed_yz = false;

  // shift d units
  if(use_reflect) fmain +=
    "bool reflect = false;\n";

  move_forward();

  if(!eyes) {
    if(hyperbolic) fmain +=
      "position /= sqrt(position.w*position.w - dot(position.xyz, position.xyz));\n"
      "tangent -= dot(vec4(-position.xyz, position.w), tangent) * position;\n"
      "tangent /= sqrt(dot(tangent.xyz, tangent.xyz) - tangent.w*tangent.w);\n";

    if(in_h2xe()) fmain +=
      "position /= sqrt(position.z*position.z - dot(position.xy, position.xy));\n"
      "tangent -= dot(vec3(-position.xy, position.z), tangent.xyz) * position;\n"
      "tangent /= sqrt(dot(tangent.xy, tangent.xy) - tangent.z*tangent.z);\n";
    }

  if(horos()) {
    string w20, w21;
    if(in_h2xe() && hybrid::underlying == gBinary4) {
      w21 = "  for(int i=0; i<2; i++) {\n";
      w20 = "int i=3; {\n";
      }
    else if(in_h2xe() && hybrid::underlying == gTernary) {
      w21 = "  for(int i=0; i<3; i++) {\n";
      w20 = "int i=4; {\n";
      }
    else if(in_h2xe() && hybrid::underlying == gBinaryTiling) {
      w20 = "int i = sides == 8 ? 5 : position.y < 0. ? 5 : 6; {\n";
      w21 = "for(int i=1; i<4; i++) {\n";
      }
    else {
      w20 = "  for(int i="+its(flat2)+"; i<"+its(S7)+"; i++) {\n";
      w21 = "for(int i=0; i<"+its(flat1)+"; i++) {\n";
      }
    fmain +=
      "if(which == 20) {\n"
      "  mediump float best = 999.;\n"
      +w20+
        "  mediump float cand = "+f_len_prod()+"(" + getM("walloffset+i") + " * position);\n"
        "  if(cand < best) { best = cand; which = i; }\n"
        "  }\n"
        "}\n"
      "if(which == 21) {\n"
        "mediump float best = 999.;\n"
        +w21+
        "  mediump float cand = "+f_len_prod()+"(" + getM("walloffset+i") + " * position);\n"
        "  if(cand < best) { best = cand; which = i; }\n"
        "  }\n"
        "}\n";
    }

  if(volumetric::on) fmain +=
    "if(dist > 0. && go < " + to_glsl(hard_limit) + ") {\n"
    "   if(dist > "+to_glsl(hard_limit)+" - go) dist = "+to_glsl(hard_limit)+" - go;\n"
    "   mediump vec4 col = texture2D(tVolumetric, cid);\n"
    "   mediump float factor = col.w; col.w = 1.;\n"
    "   mediump float frac = exp(-(factor + 1. / uExpDecay) * dist);\n"
    "   gl_FragColor += left * (1.-frac) * col;\n"
    "   left *= frac;\n"
    "   }\n;";

  fmain += "  go = go + dist;\n";

  fmain += "if(which == -1) continue;\n";

  if(gproduct && eyes) fmain += "position.w = -nzpos;\n";
  else if(gproduct) fmain += "position.w = -zpos;\n";

  if(reg3::ultra_mirror_in()) fmain +=
    "if(which >= " + its(S7) + ") {"
    "  tangent = " + getM("which") + " * tangent;\n"
    "  continue;\n"
    "  }\n";

  // apply wall color
  fmain +=
    "  mediump vec2 u = cid + vec2(float(which) / float(uLength), 0);\n"
    "  mediump vec4 col = texture2D(tWallcolor, u);\n"
    "  if(col[3] > 0.0) {\n";

  if(eyes)
    fmain += "    mediump float gou = go / uAbsUnit;\n";
  else
    fmain += "    mediump float gou = go;\n";

  if(hard_limit < NO_LIMIT)
    fmain += "    if(gou > " + to_glsl(hard_limit) + ") { gl_FragDepth = 1.; return; }\n";

  if(!(levellines && disable_texture)) {
    fmain += "  mediump vec4 pos = position;\n";
    if(nil) fmain += "if(which == 2 || which == 5) pos.z = 0.;\n";
    else if(hyperbolic && bt::in()) fmain +=
        "pos = deparabolici13(pos);\n"
        "pos.xyz = pos.zxy;\n";
    else if(hyperbolic || sphere) fmain +=
        "pos /= pos.w;\n";
    else if(gproduct && bt::in()) fmain +=
        "pos.xy = deparabolic12(pos).xy;\n"
        "pos.z = -pos.w; pos.w = 0.;\n"
;
    else if(gproduct) fmain +=
      "pos = vec4(pos.x/pos.z, pos.y/pos.z, -pos.w, 0);\n";
    fmain +=
    "    mediump vec2 inface = map_texture(pos, which+walloffset);\n"
    "    mediump vec3 tmap = texture2D(tTextureMap, u).rgb;\n"
    "    if(tmap.z == 0.) col.xyz *= min(1., (1.-inface.x)/ tmap.x);\n"
    "    else {\n"
    "      mediump vec2 inface2 = tmap.xy + tmap.z * inface;\n"
    "      col.xyz *= texture2D(tTexture, inface2).rgb;\n"
    "      }\n";
    }

  if(volumetric::on)
    fmain += "    mediump float d = uExpStart * exp(-gou / uExpDecay);\n";

  else
    fmain +=
    "    mediump float d = max(1. - gou / uLinearSightRange, uExpStart * exp(-gou / uExpDecay));\n";

  if(!volumetric::on) fmain +=
    "    col.xyz = col.xyz * d + uFogColor.xyz * (1.-d);\n";

  if(nil) fmain +=
    "    if(abs(abs(position.x)-abs(position.y)) < .005) col.xyz /= 2.;\n";

  if(use_reflect) fmain +=
    "  if(col.w == 1.) {\n"
    "    col.w = " + to_glsl(1-reflect_val)+";\n"
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

  if(hyperbolic && !eyes && !intra::in) {
    fmain +=
    "      mediump vec4 t = at0 * tanh(go);\n"
    "      t.w = 1.;\n";
    fmain += "gl_FragColor.xyz *= 0.9999 + 0.0001 * t.z;\n";
    fmain += "gl_FragColor.xyz /= 0.9999 + 0.0001 * t.z;\n";
    }
  else {
    fmain +=
    "      mediump vec4 t = at0 * go;\n"
    "      t.w = 1.;\n";
    }

  if(levellines) {
    if(hyperbolic && !eyes && !intra::in)
      fmain += "gl_FragColor.xyz *= 0.5 + 0.5 * cos(t.z * uLevelLines * 2. * PI);\n";
    else
      fmain += "gl_FragColor.xyz *= 0.5 + 0.5 * cos(go * uLevelLines * 2. * PI);\n";
    if(fsh.find("uLevelLines") == string::npos)
      fsh += "uniform mediump float uLevelLines;\n";
    }

  if(panini_alpha)
    fmain += panini_shader();

  else if(stereo_alpha)
    fmain += stereo_shader();

  #ifndef GLES_ONLY
  fmain +=
    "      gl_FragDepth = (" + to_glsl(-vnear-vfar)+"+t.w*" + to_glsl(2*vnear*vfar)+"/t.z)/" + to_glsl(vnear-vfar)+";\n"
    "      gl_FragDepth = (gl_FragDepth + 1.) / 2.;\n";
  #endif

  if(!use_reflect) fmain +=
    "      return;\n";
  else fmain +=
    "      depthtoset = false;\n";

  fmain +=
    "      }\n"
    "    left *= (1. - col.w);\n"
    "    }\n";

  if(use_reflect)
    apply_reflect(flat1, flat2);

  // next cell
  fmain +=
    "  mediump vec4 connection = texture2D(tConnections, u);\n"
    "  cid = connection.xy;\n";

  if(many_cell_types)
    fmain += "int nwalloffset = int(connection.w * " + to_glsl(max_wall_offset) + ");\n";

  fmain += "  int mid = int(connection.z * 1024.);\n";

  string no_intra_portal = "";

  no_intra_portal +=
    "  mediump mat4 m = " + getM("mid") + " * " + getM("walloffset+which") + ";\n";

  if(eyes)
    no_intra_portal += "  vw = m * vw;\n";

  else no_intra_portal +=
    "  position = m * position;\n"
    "  tangent = m * tangent;\n";

  if(gproduct) no_intra_portal =
    "  if(which == sides-2) { zpos += uPLevel+uPLevel; }\n"
    "  else if(which == sides-1) { zpos -= uPLevel+uPLevel; }\n"
    "  else {\n" + no_intra_portal + "}\n";

  if(stretch::mstretch) no_intra_portal +=
    "  m = s_itranslate(m*vec4(0,0,0,1)) * m;"
    "  fromOrig = m * fromOrig;\n"
    "  m[0][1] = -m[0][1]; m[1][0] = -m[1][0];\n" // inverse
    "  toOrig = toOrig * m;\n";

  if(!intra::in) fmain += no_intra_portal;
  else {

    if(intra::in) {
      int q = isize(intra::data);
      fmain +="  mediump vec4 pconnection = texture2D(tPortalConnections, u);\n";
      fmain += "if(pconnection.x == 0.) {\n";
      fmain += no_intra_portal;
      fmain += "  } else\n";

      for(int gid2=0; gid2<q; gid2++) {
        if(gid2 == q-1)
          fmain += "  {\n";
        else  {
          fmain += "  if(nwalloffset < " + its(intra::data[gid2+1].wallindex) + ") {\n";
          }
        emit_intra_portal(gid1, gid2);
        if(gid2 == q-1)
          fmain += "  }\n";
        else
          fmain += "  } else\n";
        }
      }
    else
      fmain += no_intra_portal;
    }

  if(many_cell_types) {
    fmain +=
      "walloffset = nwalloffset;\n"
      "sides = int(connection.w * " + to_glsl(max_wall_offset * max_celltype) + ") - " + its(max_celltype) + " * walloffset;\n";

    // fmain += "if(sides != 8) { gl_FragColor = vec4(.5,float(sides)/8.,.5,1); return; }";
    }
  }

void enable_raycaster() {
  using glhr::to_glsl;
  auto state = raycaster_state();
  if(state != saved_state) {
    reset_raycaster();
    saved_state = state;
    }

  last_geometry = geometry;

  if(!our_raycaster) {
    auto& g = our_raygen;
    g.create();

    g.fsh += g.fmain;
    callhooks(hooks_rayshader, g.vsh, g.fsh);
    our_raycaster = make_shared<raycaster> (g.vsh, g.fsh);
    }
  full_enable(our_raycaster);
  }

void raygen::add_functions() {

  add_if("xpush_h3",

    "mediump mat4 xpush_h3(float x) { return mat4("
         "cosh(x), 0., 0., sinh(x),\n"
         "0., 1., 0., 0.,\n"
         "0., 0., 1., 0.,\n"
         "sinh(x), 0., 0., cosh(x)"
         ");}\n");

  add_if("xpush_h2",

    "mediump mat4 xpush_h2(float x) { return mat4("
         "cosh(x), 0., sinh(x), 0.,\n"
         "0., 1., 0., 0.,\n"
         "sinh(x), 0., cosh(x), 0.,\n"
         "0., 0., 0., 1."
         ");}\n");

  add_if("xpush_s",

    "mediump mat4 xpush_s(float x) { return mat4("
         "cos(x), 0., 0., sin(x),\n"
         "0., 1., 0., 0.,\n"
         "0., 0., 1., 0.,\n"
         "-sin(x), 0., 0., cos(x)"
         ");}\n"
    );

  add_if("xzspin",

    "mediump mat4 xzspin(float x) { return mat4("
         "cos(x), 0., sin(x), 0.,\n"
         "0., 1., 0., 0.,\n"
         "-sin(x), 0., cos(x), 0.,\n"
         "0., 0., 0., 1."
         ");}\n");

  add_if("yzspin",

    "mediump mat4 yzspin(float x) { return mat4("
         "1., 0., 0., 0.,\n"
         "0., cos(x), sin(x), 0.,\n"
         "0., -sin(x), cos(x), 0.,\n"
         "0., 0., 0., 1."
         ");}\n"
     );

  /* note: this is called deparabolici13 because it orders the coordinates differently from deparabolic13 */
  add_if("deparabolici13",
        "mediump vec4 deparabolici13(mediump vec4 h) {\n"
        "  h /= (1. + h[3]);\n"
        "  h[0] -= 1.;\n"
        "  h /= h.x*h.x + h.y*h.y + h.z * h.z;\n"
        "  h[0] += .5;\n"
        "  mediump vec4 res;\n"
        "  res.x = h.y * 2.;\n"
        "  res.y = h.z * 2.;\n"
        "  res.z = (log(2.) + log(-h.x));\n"
        "  res.w = 1.;\n"
        "  return res;\n"
        "  }\n\n");

  /* note: this is called eeparabolici13 because it orders the coordinates differently from enparabolic13 */
  add_if("enparabolici13",
        "mediump vec4 enparabolici13(mediump vec4 h) {\n"
        "  mediump vec4 res;\n"
        "  float diag = (h.x*h.x + h.y*h.y)/2.;\n"
        "  res.x = sinh(h.z) + diag * exp(-h.z);\n"
        "  res.y = h.x * exp(-h.z);\n"
        "  res.z = h.y * exp(-h.z);\n"
        "  res.w = cosh(h.z) + diag * exp(-h.z);\n"
        "  return res;\n"
        "  }\n\n");

  add_if("to_poco_h3",
        "mediump vec4 to_poco_h3(mediump vec4 pos) {\n"
        "  pos = pos / pos[3];\n"
        "  pos[2] /= sqrt(1.-pos.x*pos.x-pos.y*pos.y);\n"
        "  return pos;\n"
        "  }\n\n");

  add_if("from_poco_h3",
        "mediump vec4 from_poco_h3(mediump vec4 pos) {\n"
        "  pos[2] *= sqrt(1.-pos.x*pos.x-pos.y*pos.y);\n"
        "  float s = 1. - dot(pos.xyz, pos.xyz);\n"
        "  return pos / sqrt(s);\n"
        "  }\n\n");

  add_if("to_poco_s3",
        "mediump vec4 to_poco_s3(mediump vec4 pos) {\n"
        "  pos = pos / pos[3];\n"
        "  pos[2] /= sqrt(1.+pos.x*pos.x+pos.y*pos.y);\n"
        "  return pos;\n"
        "  }\n\n");

  add_if("from_poco_s3",
        "mediump vec4 from_poco_s3(mediump vec4 pos) {\n"
        "  pos[2] *= sqrt(1.+pos.x*pos.x+pos.y*pos.y);\n"
        "  float s = 1. + dot(pos.xyz, pos.xyz);\n"
        "  return pos / sqrt(s);\n"
        "  }\n\n");

  add_if("from_poco_h2xr_h",
          "mediump vec4 from_poco_h2xr_h(mediump vec4 pos) {\n"
          "  float s = 1. - pos.x*pos.x - pos.y * pos.y;\n"
          "  pos.z = 1.;\n"
          "  return pos / sqrt(s);\n"
          "  }\n\n");

  add_if("to_poco_h2xr_h",
          "mediump vec4 to_poco_h2xr_h(mediump vec4 pos) {\n"
          "  pos /= pos[2];\n"
          "  pos.w = 1.;\n"
          "  return pos;\n"
          "  }\n\n");

  add_if("from_poco_h2xr_b",
          "mediump vec4 from_poco_h2xr_b(mediump vec4 pos) {\n"
          "  return enparabolic12(pos);\n"
          "  }\n\n");

  add_if("to_poco_h2xr_b",
          "mediump vec4 to_poco_h2xr_b(mediump vec4 pos) {\n"
          "  pos /= sqrt(pos.z*pos.z-pos.x*pos.x-pos.y*pos.y);\n"
          "  pos = deparabolic12(pos);\n"
          "  return pos;\n"
          "  }\n\n");

  add_if("from_poco_h2xr_e",
          "mediump vec4 from_poco_h2xr_e(mediump vec4 pos) {\n"
            "  return vec4(sinh(pos[2]), sinh(pos[0]) * cosh(pos[2]), cosh(pos[0]) * cosh(pos[2]), 0);\n"
          "  }\n\n");

  add_if("to_poco_h2xr_e",
          "mediump vec4 to_poco_h2xr_e(mediump vec4 pos) {\n"
          "  mediump float x = asinh(pos[0]);\n"
          "  return vec4(asinh(pos[1] / cosh(x)), 0, x, 1);\n"
          "  }\n\n");

  add_if("from_poco_s2xr_s",
          "mediump vec4 from_poco_s2xr_s(mediump vec4 pos) {\n"
          "  float s = 1. + pos.x*pos.x + pos.y * pos.y;\n"
          "  pos.z = 1.;\n"
          "  return pos / sqrt(s);\n"
          "  }\n\n");

  add_if("to_poco_s2xr_s",
          "mediump vec4 to_poco_s2xr_s(mediump vec4 pos) {\n"
          "  pos /= pos[2];\n"
          "  pos.w = 1.;\n"
          "  return pos;\n"
          "  }\n\n");

  add_if("from_poco_s2xr_e",
          "mediump vec4 from_poco_s2xr_e(mediump vec4 pos) {\n"
          "  return vec4(sin(pos[2]), sin(pos[0]) * cos(pos[2]), cos(pos[0]) * cos(pos[2]), 0);\n"
          "  }\n\n");

  add_if("to_poco_s2xr_e",
          "mediump vec4 to_poco_s2xr_e(mediump vec4 pos) {\n"
          "  mediump float x = asin_clamp(pos[0]);\n"
          "  return vec4(asin_clamp(pos[1] / cos(x)), 0, x, 1);\n"
          "  }\n\n");

  add_if("deparabolic12",
        "mediump vec4 deparabolic12(mediump vec4 h) {\n"
        "  h /= (1. + h.z);\n"
        "  h[0] -= 1.;\n"
        "  h /= h.x*h.x + h.y*h.y;\n"
        "  h[0] += .5;\n"
        "  mediump vec4 res;\n"
        "  res.x = (log(2.) + log(-h.x));\n"
        "  res.y = h.y * 2.;\n"
        "  res.w = 1.;\n"
        "  return res;\n"
        "  }\n\n");

  add_if("enparabolic12",
        "mediump vec4 enparabolic12(mediump vec4 h) {\n"
        "  mediump vec4 res;\n"
        "  float diag = h.y*h.y /2.;\n"
        "  res.x = sinh(h.x) + diag * exp(-h.x);\n"
        "  res.y = h.y * exp(-h.x);\n"
        "  res.z = cosh(h.x) + diag * exp(-h.x);\n"
        "  return res;\n"
        "  }\n\n");

  add_if("len_rotspace", "mediump float len_rotspace(vec4 h) { return 1. - h[3]; }\n");

  add_if("len_h3",  "  mediump float len_h3(mediump vec4 x) { return x[3]; }\n");
  add_if("len_sr", "  mediump float len_sr(mediump vec4 x) { return 1.+x.x*x.x+x.y*x.y-x.z*x.z-x.w*x.w; }\n");
  add_if("len_sl2","  mediump float len_sl2(mediump vec4 x) { return 1.+x.x*x.x+x.y*x.y; }\n");
  add_if("len_s3",  "  mediump float len_s3(mediump vec4 x) { return 1.-x[3]; }\n");
  add_if("len_x",  "  mediump float len_x(mediump vec4 x) { return length(x.xyz); }\n");
  add_if("len_h2",  "  mediump float len_h2(mediump vec4 x) { return x[2]; }\n");
  add_if("len_s2",  "  mediump float len_s2(mediump vec4 x) { return 1.-x[2]; }\n");
  add_if("len_e2",  "  mediump float len_e2(mediump vec4 x) { return length(x.xy); }\n");
  }

void raygen::emit_raystarter() {
  if(gproduct) {
    string sgn=in_h2xe() ? "-" : "+";
    fmain +=
    "  position = vw * vec4(0., 0., 1., 0.);\n"
    "  mediump vec4 at1 = uLP * at0;\n";
    if(in_e2xe()) fmain +=
    "  zpos = log(position.z);\n";
    else fmain +=
    "  zpos = log(position.z*position.z"+sgn+"position.x*position.x"+sgn+"position.y*position.y)/2.;\n";
    if(eyes) fmain +=
    "  vw *= exp(-zpos);\n";
    else fmain +=
    "  position *= exp(-zpos);\n"
    "  zspeed = at1.z;\n"
    "  xspeed = length(at1.xy);\n"
    "  tangent = vw * exp(-zpos) * vec4(at1.xy, 0, 0) / xspeed;\n";
    }
  else if(!eyes) {
    fmain +=
      "  position = vw * vec4(0., 0., 0., 1.);\n"
      "  tangent = vw * at0;\n";
    }

  if(eyes) {
    fsh += "mediump uniform mat4 uEyeShift;\n";
    fsh += "mediump uniform float uAbsUnit;\n";
    }

  if(stretch::in()) {
    if(stretch::mstretch) {
      fsh += "mediump uniform mat4 uITOA;\n";
      fsh += "mediump uniform mat4 uATOI;\n";
      fsh += "mediump uniform mat4 uToOrig;\n";
      fsh += "mediump uniform mat4 uFromOrig;\n";
      fsh += "mediump mat4 toOrig;\n";
      fsh += "mediump mat4 fromOrig;\n";
      fmain +=
        "toOrig = uToOrig;\n"
        "fromOrig = uFromOrig;\n";
      fmain +=
        "tangent = s_itranslate(toOrig * position) * toOrig * tangent;\n";
      fmain +=
        "tangent = uITOA * tangent;\n";
      fmain +=
        "tangent = fromOrig * s_translate(toOrig * position) * tangent;\n";
      }
    else {
      fmain +=
        "tangent = s_itranslate(position) * tangent;\n";
      fmain +=
        "tangent[2] /= " + glhr::to_glsl(stretch::not_squared()) + ";\n";
      fmain +=
        "tangent = s_translate(position) * tangent;\n";
      }
    }
  }

void raygen::create() {
  using glhr::to_glsl;
  currentmap->wall_offset(centerover); /* so raywall is not empty and deg is not zero */

  deg = 0;

  auto samples = used_sample_list();
  for(int i=0; i<isize(samples)-1; i++)
    deg = max(deg, samples[i+1].first - samples[i].first);

  if(true) {
    asonov = hr::asonov::in();
    use_reflect = reflect_val && !nil && !levellines;

    many_cell_types = need_many_cell_types();

    vsh =
      "attribute mediump vec4 aPosition;\n"
      "uniform mediump mat4 uProjection;\n"
      "varying mediump vec4 at;\n"
      "void main() { \n"
      "  gl_Position = aPosition; at = uProjection * aPosition; \n"
      "  }\n";

    if(intra::in) {
      irays = 0;
      intra::resetter ir;
      for(int i=0; i<isize(intra::data); i++) {
        intra::switch_to(i);
        irays += isize(cgi.raywall);
        }
      }
    else irays = isize(cgi.raywall);
    string rays = its(irays);

    fsh =
    "varying mediump vec4 at;\n"
    "uniform mediump int uLength;\n"
    "uniform mediump float uIPD;\n"
    "uniform mediump mat4 uStart;\n"
    "uniform mediump vec2 uStartid;\n"
    "uniform mediump sampler2D tConnections;\n"
    "uniform mediump sampler2D tWallcolor;\n"
    "uniform mediump sampler2D tVolumetric;\n"
    "uniform mediump sampler2D tTexture;\n"
    "uniform mediump sampler2D tTextureMap;\n"
    "uniform mediump vec4 uFogColor;\n"
    "uniform mediump float uLinearSightRange, uExpStart, uExpDecay;\n";

    if(intra::in) fsh +=
      "uniform mediump sampler2D tPortalConnections;\n";

    if(wall_via_texture) {
      fsh +=
        "uniform mediump sampler2D tWall;\n"
        "uniform mediump float uInvLengthWall;\n"
        "mediump vec4 getWall(mediump int x, mediump int coord) {\n"
        "  mediump vec4 result;\n"
        "  mediump vec4 v = texture2D(tWall, vec2((float(x)+.5) * uInvLengthWall, (float(coord)+.5) / 4.));\n"
        "  for(int j=0; j<4; j++) result[j] = (v[j] - .5) * " + to_glsl(ray_scale) + ";\n"
        "  return result;\n"
        "  }\n"
        "mediump int getWallstart(mediump int x) {\n"
        "  mediump vec4 v = texture2D(tWall, vec2((float(x)+.5) * uInvLengthWall, 0.625));\n"
        "  return int(v[0] / uInvLengthWall);\n"
        "  }\n";
      }
    else fsh +=
      "uniform mediump vec4 uWallX["+rays+"];\n"
      "uniform mediump vec4 uWallY["+rays+"];\n"
      "uniform mediump int uWallstart["+its(isize(cgi.wallstart))+"];\n";

    if(m_via_texture) {
      fsh +=
        "uniform mediump sampler2D tM;\n"
        "uniform mediump float uInvLengthM;\n"
        "mediump mat4 getM(mediump int x) {\n"
        "  mediump mat4 result;\n"
        "  for(int i=0; i<4; i++) {\n"
        "    mediump vec4 v = texture2D(tM, vec2((float(x)+.5) * uInvLengthM, (float(i)+.5) / 4.));\n"
        "    for(int j=0; j<4; j++) result[j][i] = (v[j] - .5) * " + to_glsl(ray_scale) + ";\n"
        "    }\n"
        "  return result;\n"
        "  }\n";
      }
    else fsh +=
      "uniform mediump mat4 uM[" + its(gms_limit) + "];\n";
    
    #ifdef GLES_ONLY
    if(!wall_via_texture) {
      fsh += build_getter("mediump vec4", "uWallX", irays);
      fsh += build_getter("mediump vec4", "uWallY", irays);
      fsh += build_getter("mediump int", "uWallstart", deg+1);
      }
    if(!m_via_texture)
      fsh += build_getter("mediump mat4", "uM", gms_limit);
    #endif

    if(gproduct || intra::in) fsh +=
      "uniform mediump mat4 uLP;\n";

    if(gproduct || intra::in) fsh +=
      "uniform mediump float uPLevel;\n";

    if(many_cell_types) fsh +=
      "uniform int uWallOffset, uSides;\n";

   if(many_cell_types) {
     fsh += "int walloffset, sides;\n";
     }
   else {
     fsh += "const int walloffset = 0;\n"
       "const int sides = " + its(centerover->type+(WDIM == 2 ? 2 : 0)) + ";\n";
     }

   fsh +=
     "mediump vec2 map_texture(mediump vec4 pos, int which) {\n";

   fsh +=
       "int s = " + getWallstart("which") + ";\n"
       "int e = " + getWallstart("which+1") + ";\n"
       "for(int ix=0; ix<16; ix++) {\n"
         "int i = s+ix; if(i >= e) break;\n"
         "mediump vec2 v = vec2(dot(" + getWall("i", 0) + ", pos), dot(" + getWall("i", 1) + ", pos));\n"
         "if(v.x >= 0. && v.y >= 0. && v.x + v.y <= 1.) return vec2(v.x+v.y, v.x-v.y);\n"
         "}\n"
       "return vec2(1, 1);\n"
       "}\n";

   eyes = is_eyes();

   fmain = "void main() {\n";

   if(use_reflect) fmain += "  bool depthtoset = true;\n";

    if(IN_ODS) fmain +=
    "  mediump float lambda = at[0];\n" // -PI to PI
    "  mediump float phi;\n"
    "  mediump float eye;\n"
    "  if(at.y < 0.) { phi = at.y + PI/2.; eye = uIPD / 2.; }\n" // right
    "  else { phi = at.y - PI/2.; eye = -uIPD / 2.; }\n"
    "  mediump mat4 vw = uStart * xzspin(-lambda) * "+f_xpush()+"(eye) * yzspin(phi);\n"
    "  mediump vec4 at0 = vec4(0., 0., 1., 0.);\n";
    
    else {
      fmain += 
        "  mediump mat4 vw = uStart;\n"
        "  mediump vec4 at0 = at;\n"
        "  gl_FragColor = vec4(0,0,0,1);\n"
        "  mediump float left = 1.;\n"
        "  at0.y = -at.y;\n"
        "  at0.w = 0.;\n";
      
      if(panini_alpha) fmain += 
          "mediump float hr = at0.x*at0.x;\n"
          "mediump float alpha = " + to_glsl(panini_alpha) + ";\n"
          "mediump float A = 1. + hr;\n"
          "mediump float B = -2.*hr*alpha;\n"
          "mediump float C = 1. - hr*alpha*alpha;\n"
          "B /= A; C /= A;\n"
    
          "mediump float hz = B / 2. + sqrt(C + B*B/4.);\n"
          "if(abs(hz) > 1e-3) {"
          "at0.xyz *= hz+alpha;\n"
          "at0.z = hz;\n}"
          " else at0.z = 0.;\n"
          "\n"
          ;

      else if(stereo_alpha) fmain += 
          "mediump float hr = at0.x*at0.x+at0.y*at0.y;\n"
          "mediump float alpha = " + to_glsl(stereo_alpha) + ";\n"
          "mediump float A = 1. + hr;\n"
          "mediump float B = -2.*hr*alpha;\n"
          "mediump float C = 1. - hr*alpha*alpha;\n"
          "B /= A; C /= A;\n"
    
          "mediump float hz = B / 2. + sqrt(C + B*B/4.);\n"
          "if(abs(hz) > 1e-3) {"
          "at0.xyz *= hz+alpha;\n"
          "at0.z = hz;\n}"
          " else at0.z = 0.;\n"
          "\n"
          ;

      fmain +=
        "  at0.xyz = at0.xyz / length(at0.xyz);\n";   

      if(eyes) fmain += "  at0.xyz /= uAbsUnit;\n";
      }
      
    ld s = 1;
    #if CAP_VR
    if(eyes) s *= vrhr::absolute_unit_in_meters;
    #endif
    
    if(is_stepbased() || intra::in) fmain +=
      "  const mediump float maxstep = " + fts(maxstep_current() * s) + ";\n"
      "  const mediump float minstep = " + fts(minstep * s) + ";\n"
      "  mediump float next = maxstep;\n";
    
    string fmain_prod, fmain_nprod;

    fmain += "  mediump vec4 position;\n";
    fmain += "  mediump vec4 tangent;\n";

    if(gproduct || intra::in) {
      fmain += "  mediump float zspeed = 1.;\n";
      fmain += "  mediump float xspeed = 1.;\n";
      fmain += "  mediump float zpos = 0.;\n";
      }

    if(many_cell_types) fmain += "  walloffset = uWallOffset; sides = uSides;\n";

    if(intra::in) {

      int q = isize(intra::data);

      intra::resetter ir;

      for(int gid2=0; gid2<q; gid2++) {
        if(gid2 == q-1)
          fmain += "  {\n";
        else  {
          fmain += "  if(walloffset < " + its(intra::data[gid2+1].wallindex) + ") {\n";
          }
        intra::switch_to(gid2);
        emit_raystarter();
        if(gid2 == q-1)
          fmain += "  }\n";
        else
          fmain += "  } else\n";
        }
      }
    else emit_raystarter();

    fmain +=
      "  mediump float go = 0.;\n"
      "  mediump vec2 cid = uStartid;\n"
      "  for(int iter=0; iter<" + its(max_iter_current()) + "; iter++) {\n";

    if(intra::in) {
      int gi = isize(intra::data);
      for(int i=0; i<gi; i++) {
        if(i == gi-1)
          fmain += "  {\n";
        else  {
          fmain += "  if(walloffset < " + its(intra::data[i+1].wallindex) + ") {\n";
          }
        intra::resetter ir;
        intra::switch_to(i);
        emit_iterate(i);
        if(i == gi-1)
          fmain += "  }\n";
        else
          fmain += "  } else\n";
        }
      }
    else
      emit_iterate(-1);

    fmain +=
      "  }\n"
      "  gl_FragColor.xyz += left * uFogColor.xyz;\n";

    #ifndef GLES_ONLY
    if(use_reflect) fmain +=
      "  if(depthtoset) gl_FragDepth = 1.;\n";
    else fmain +=
      "  gl_FragDepth = 1.;\n";
    #endif

    fmain +=
      "  }";
    }

  add_functions();
  }

void bind_array(vector<array<float, 4>>& v, GLint t, GLuint& tx, int id, int length) {
  if(t == -1) println(hlog, "bind to nothing");
  glUniform1i(t, id);

  if(tx == 0) glGenTextures(1, &tx);

  glActiveTexture(GL_TEXTURE0 + id);
  GLERR("activeTexture");

  glBindTexture(GL_TEXTURE_2D, tx);
  GLERR("bindTexture");

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  GLERR("texParameteri");
  
  #ifdef GLES_ONLY
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, length, isize(v)/length, 0, GL_RGBA, GL_FLOAT, &v[0]);  
  #else
  glTexImage2D(GL_TEXTURE_2D, 0, 0x8814 /* GL_RGBA32F */, length, isize(v)/length, 0, GL_RGBA, GL_FLOAT, &v[0]);  
  #endif
  GLERR("bind_array");
  }

void uniform2(GLint id, array<float, 2> fl) {
  glUniform2f(id, fl[0], fl[1]);
  }

color_t color_out_of_range = 0x0F0800FF;

EX transmatrix get_ms(cell *c, int a, bool mirror) {
  int z = a ? 1 : -1;
  
  if(c->type == 3) {
    hyperpoint h = 
      project_on_triangle(
        hybrid::get_corner(c, a, 0, z),
        hybrid::get_corner(c, a, 1, z),
        hybrid::get_corner(c, a, 2, z)
        );
    transmatrix T = rspintox(h);
    if(mirror) T = T * MirrorX;
    return T * xpush(-2*hdist0(h)) * spintox(h);
    }
  else {
    hyperpoint h = Hypc;
    for(int a=0; a<c->type; a++) {
      hyperpoint corner = hybrid::get_corner(c, a, 0, z);
      h += corner;
      }
    h = normalize(h);
    ld d = hdist0(h);
    if(h[2] > 0) d = -d;
    if(mirror) return MirrorZ * zpush(2*d);
    return zpush(2*d);
    }
  }

int nesting;

EX transmatrix mirrorize(transmatrix T) {
  T = inverse(T);
  hyperpoint h = tC0(T);
  ld d = hdist0(h);
  return rspintox(h) * xpush(d/2) * MirrorX * xpush(-d/2) * spintox(h);
  }

transmatrix protect_prod(transmatrix T) {
  if(MDIM == 3)
    for(int i=0; i<4; i++) T[i][3] = T[3][i] = i == 3;
  return T;
  }

struct raycast_map {

  int saved_frameid;
  int saved_map_version;
  
  vector<cell*> lst;
  map<cell*, int> ids;

  vector<transmatrix> ms;

  int length, per_row, rows, mirror_shift, deg;

  vector<array<float, 4>> connections, wallcolor, texturemap, volumetric, portal_connections;
  
  void apply_shape() {
    length = 4096;
    deg = our_raygen.deg;
    per_row = length / deg;
    rows = next_p2((isize(lst)+per_row-1) / per_row);  
    int q = length * rows;
    connections.resize(q);
    portal_connections.resize(q);
    wallcolor.resize(q);
    texturemap.resize(q);
    volumetric.resize(q);
    }

  void generate_initial_ms(cell *cs) {
    auto sa = used_sample_list();
    
    ms.clear();
    ms.resize(sa.back().first, Id);
    
    intra::resetter ir;

    for(auto& p: sa) {
      int id = p.first;
      cell *c = p.second;
      if(!c) continue;
      intra::may_switch_to(c);
      for(int j=0; j<c->type; j++)
        ms[id+j] = protect_prod(currentmap->ray_iadj(c, j));
      if(WDIM == 2) for(int a: {0, 1}) {
        ms[id+c->type+a] = get_ms(c, a, false);
        }
      }
    
    // println(hlog, ms);
    
    mirror_shift = isize(ms);
    
    if(!sol && !nil && (reflect_val || reg3::ultra_mirror_in())) {
    
      ms.resize(mirror_shift * 2);
      
      for(auto& p: sa) {
        int id = p.first;
        cell *c = p.second;
        if(!c) continue;
        intra::may_switch_to(c);
        for(int j=0; j<c->type; j++)
          ms[mirror_shift+id+j] = protect_prod(mirrorize(ms[id+j]));
        if(WDIM == 2) for(int a: {0, 1}) {
          ms[mirror_shift+id+c->type+a] = get_ms(c, a, true);
          }
        }      

      if(reg3::ultra_mirror_in()) {
        for(auto v: cgi.ultra_mirrors) 
          ms.push_back(v);
        }
      }    

    for(auto p: sa) {
      cell *c = p.second;
      if(!c) continue;
      intra::may_switch_to(c);
      int id =p.first;
      if(gproduct) {
        ms[id+c->type-2] = Id;
        ms[id+c->type-1] = Id;
        }
      }
    }
  
  void generate_cell_listing(cell *cs) {
    manual_celllister cl;
    cl.add(cs);
    bool optimize = !isWall3(cs);
    intra::resetter ir;
    // vector<int> legaldir = { -1 };
    for(int i=0; i<isize(cl.lst); i++) {
      cell *c = cl.lst[i];
      intra::may_switch_to(c);
      if(racing::on && i > 0 && c->wall == waBarrier) continue;
      if(optimize && isWall3(c)) continue;
      forCellIdCM(c2, d, c) {
        // if(reflect_val == 0 && !((1<<d) & legaldir[i])) continue;

        if(intra::in && !intra::intra_id.count(c2)) intra::intra_id[c2] = intra::current;

        if(intra::in) {
          cellwalker cw(c, d);
          auto p = at_or_null(intra::connections, cw);
          if(p) {
            cell *c3 = p->tcw.at;
            if(rays_generate && c3->mpdist > 7) { intra::may_switch_to(c3); setdist(c3, 7, c); intra::may_switch_to(c2); }
            cl.add(c3);
            }
          }

        if(rays_generate) setdist(c2, 7, c);
        /* if(!cl.listed(c2))
          legaldir.push_back(legaldir[i] &~ (1<<((d+3)%6)) ); */
        cl.add(c2);
        if(isize(cl.lst) >= max_cells) goto finish;
        }
      }
    finish:
    lst = cl.lst;
    ids.clear();
    for(int i=0; i<isize(lst); i++) ids[lst[i]] = i;
    }

  array<float, 2> enc(int i, int a) { 
    array<float, 2> res;
    res[0] = ((i%per_row) * deg + a + .5) / length;
    res[1] = ((i / per_row) + .5) / rows;
    return res;
    }

  void generate_connections(cell *c, int id) {
    intra::may_switch_to(c);
    auto& vmap = volumetric::vmap;
    if(volumetric::on) {
      celldrawer dd;
      dd.c = c;
      dd.setcolors();
      int u = (id/per_row*length) + (id%per_row * deg);
      color_t vcolor;
      if(vmap.count(c))
        vcolor = vmap[c];
      else 
        vcolor = (backcolor << 8);
      volumetric[u] = glhr::acolor(vcolor);
      }
    forCellIdEx(c1_real, i, c) {
      cell *c1 = c1_real;
      const intra::connection_data *p = nullptr;
      if(intra::in) {
        cellwalker cw(c, i);
        p = at_or_null(intra::connections, cw);
        if(p) c1 = p->tcw.at;
        }
      int u = (id/per_row*length) + (id%per_row * deg) + i;
      if(!ids.count(c1)) {
        wallcolor[u] = glhr::acolor(color_out_of_range | 0xFF);
        texturemap[u] = glhr::makevertex(0.1,0,0);
        continue;
        }
      auto code = enc(ids[c1], 0);
      connections[u][0] = code[0];
      connections[u][1] = code[1];
      portal_connections[u][0] = 0;
      if(isWall3(c1)) {
        celldrawer dd;
        dd.c = c1;
        dd.setcolors();
        shiftmatrix Vf;
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
      
      int wo = intra::full_wall_offset(c);
      if(wo >= our_raygen.irays) {
        println(hlog, "wo=", wo, " irays = ", our_raygen.irays);
        reset_raycaster();
        return;
        }
      if(p) {
        int k = isize(ms);
        auto bak = geometry;
        ms.push_back(p->T);
        geometry = bak;
        ms.push_back(p->id1.T);
        ms.push_back(p->id2.iT);
        connections[u][2] = (k+.5) / 1024.;
        portal_connections[u][0] = p->id1.kind / 16. + .5;
        portal_connections[u][1] = p->id1.d / 16 + .5;
        portal_connections[u][2] = p->id2.kind / 16. + .5;
        portal_connections[u][3] = p->id2.d / 16 + .5;
        }
      else {
        transmatrix T = currentmap->iadj(c, i) * inverse(ms[wo + i]);
        if(in_e2xe() && i >= c->type-2)
          T = Id;
        T = protect_prod(T);
        for(int k=0; k<=isize(ms); k++) {
          if(k < isize(ms) && !eqmatrix(ms[k], T, 1e-5)) continue;
          if(k == isize(ms)) ms.push_back(T);
          connections[u][2] = (k+.5) / 1024.;
          break;
          }
        }
      intra::resetter ir;
      intra::may_switch_to(c1);
      int wo1 = intra::full_wall_offset(c1);
      if(wo1 >= max_wall_offset)
        println(hlog, "error: wall_offset ", wo1, " exceeds ", max_wall_offset);
      if(c1->type >= max_celltype)
        println(hlog, "error: type " + its(c1->type) + " exceeds ", max_celltype);
      connections[u][3] = (wo1 * 1. / max_wall_offset) + (c1->type + (WDIM == 2 ? 2 : 0) + .5) * 1. / max_wall_offset / max_celltype;
      }
    if(WDIM == 2) for(int a: {0, 1}) {
      celldrawer dd;
      dd.c = c;
      dd.setcolors();
      shiftmatrix Vf;
      dd.set_land_floor(Vf);
      int u = (id/per_row*length) + (id%per_row * deg) + c->type + a;
      wallcolor[u] = glhr::acolor(darkena(dd.fcol, 0, 0xFF));
      if(qfi.fshape) 
        texturemap[u] = floor_texture_map[qfi.fshape->id];
      else
        texturemap[u] = glhr::makevertex(0.1,0,0);
      }
    }
  
  void generate_connections() {
    int id = 0;
    intra::resetter ir;
    for(cell* c: lst) if(!reset_rmap)
      generate_connections(c, id++);
    }
  
  bool gms_exceeded() {
    if(m_via_texture) return false;
    return isize(ms) > gms_array_size;
    }

  void assign_uniforms(raycaster* o) {
    if(!o) return;
    glUniform1i(o->uLength, length);
    GLERR("uniform mediump length");
    
    if(m_via_texture) {
      int mlength = next_p2(isize(ms));
      vector<array<float, 4>> m_map;
      m_map.resize(4 * mlength);
      for(int i=0; i<isize(ms); i++)
        for(int a=0; a<4; a++)
        for(int b=0; b<4; b++) {
          m_map[i+a*mlength][b] = ms[i][a][b]/ray_scale + .5;
          }

      bind_array(m_map, o->tM, txM, 7, mlength);
      glUniform1f(o->uInvLengthM, 1. / mlength);
      }
    else {
      vector<glhr::glmatrix> gms;
      for(auto& m: ms) gms.push_back(glhr::tmtogl_transpose3(m));
      glUniformMatrix4fv(o->uM, isize(gms), 0, gms[0].as_array());
      }
    
    bind_array(wallcolor, o->tWallcolor, txWallcolor, 4, length);
    bind_array(connections, o->tConnections, txConnections, 3, length);
    bind_array(texturemap, o->tTextureMap, txTextureMap, 5, length);
    if(volumetric::on) bind_array(volumetric, o->tVolumetric, txVolumetric, 6, length);
    if(o->tPortalConnections != -1)
      bind_array(portal_connections, o->tPortalConnections, txPortalConnections, 1, length);

    if(o->uMirrorShift != -1) {
      glUniform1i(o->uMirrorShift, mirror_shift);
      }
    }
  
  void create_all(cell *cs) {
    saved_frameid = frameid;
    saved_map_version = mapeditor::map_version;
    generate_initial_ms(cs);
    generate_cell_listing(cs);
    apply_shape();
    generate_connections();
    }
  
  bool need_to_create(cell *cs) {
    if(!fixed_map && frameid != saved_frameid) return true;
    if(saved_map_version != mapeditor::map_version) return true;
    return !ids.count(cs);
    }
  };

unique_ptr<raycast_map> rmap;
EX bool reset_rmap = false;

EX cell* rmap_get_by_id(int id) {
  return rmap->lst[id];
  }

EX int rmap_get_id_of(cell *c) {
  return rmap->ids[c];
  }

EX void reset_raycaster() { 
  our_raycaster = nullptr; 
  reset_rmap = true;
  rots::saved_matrices_ray = {};
  }

EX void reset_raycaster_map() { 
  rmap = nullptr;
  }

EX void load_walls(vector<glvertex>& wallx, vector<glvertex>& wally, vector<GLint>& wallstart) {
  int q = 0;
  if(isize(wallx)) {
    q = isize(wallx);
    wallstart.pop_back();
    }
  for(auto i: cgi.wallstart) wallstart.push_back(q + i);
  dynamicval<eGeometry> g(geometry, gCubeTiling);
  for(auto& m: cgi.raywall) {
    wallx.push_back(glhr::pointtogl(m[0]));
    wally.push_back(glhr::pointtogl(m[1]));
    }
  }

EX void cast() {
  // may call itself recursively in case of bugs -- just in case...
  dynamicval<int> dn(nesting, nesting+1);
  if(nesting > 10) return;
  
  if(isize(cgi.raywall) > our_raygen.irays) reset_raycaster();
    
  enable_raycaster();

  auto& o = our_raycaster;
  
  if(need_many_cell_types() && o->uWallOffset == -1) {
    reset_raycaster();
    cast();
    return;
    }  
  
  if(comparison_mode) 
    glColorMask( GL_TRUE,GL_FALSE,GL_FALSE,GL_TRUE );

  vector<glvertex> screen = {
    glhr::makevertex(-1, -1, 1),
    glhr::makevertex(-1, +1, 1),
    glhr::makevertex(+1, -1, 1),
    glhr::makevertex(-1, +1, 1),
    glhr::makevertex(+1, -1, 1),
    glhr::makevertex(+1, +1, 1)
    };

  ld d = current_display->eyewidth();
  if(vid.stereo_mode == sLR) d = 2 * d - 1;
  else d = -d;

  auto& cd = current_display;
  cd->set_viewport(global_projection);
  cd->set_mask(global_projection);
  
  #if CAP_VR
  if(o->uEyeShift != -1) {
    dynamicval<eGeometry> g(geometry, gCubeTiling);
    transmatrix T = vrhr::eyeshift;
    if(nonisotropic)
      T = inverse(NLP) * T;
    glUniformMatrix4fv(o->uEyeShift, 1, 0, glhr::tmtogl_transpose(T).as_array());
    glUniform1f(o->uAbsUnit, vrhr::absolute_unit_in_meters);
    }
  if(vrhr::rendering_eye()) {
    dynamicval<eGeometry> g(geometry, gCubeTiling);
    glUniformMatrix4fv(o->uProjection, 1, 0, glhr::tmtogl_transpose(vrhr::eyeproj).as_array());
    }
  #else
  if(0) ;
  #endif
  else {
    dynamicval<eGeometry> g(geometry, gCubeTiling);
    transmatrix proj = Id;
    proj = eupush(-global_projection * d, 0) * proj;
    proj = euscale(cd->tanfov / (vid.stereo_mode == sLR ? 2 : 1), cd->tanfov * cd->ysize / cd->xsize) * proj;
    proj = eupush(-((cd->xcenter-cd->xtop)*2./cd->xsize - 1), -((cd->ycenter-cd->ytop)*2./cd->ysize - 1)) * proj;
    glUniformMatrix4fv(o->uProjection, 1, 0, glhr::tmtogl_transpose(proj).as_array());
    }
  
  if(!callhandlers(false, hooks_rayset, o)) {
  
  cell *cs = centerover;

  transmatrix T = cview().T;
  
  if(global_projection)
    T = xpush(vid.ipd * global_projection/2) * T;

  if(nonisotropic) T = NLP * T;
  T = inverse(T);

  virtualRebase(cs, T);
  
  int ray_fixes = 0;
  
  transmatrix msm = stretch::mstretch_matrix;

  back:
  for(int a=0; a<cs->type; a++)
    if(hdist0(currentmap->ray_iadj(cs, a) * tC0(T)) < hdist0(tC0(T))) {
      T = currentmap->iadj(cs, a) * T;
      if(o->uToOrig != -1) {
        transmatrix HT = currentmap->adj(cs, a);
        HT = stretch::itranslate(tC0(HT)) * HT;
        msm = HT * msm;
        }
      cs = cs->move(a);
      ray_fixes++;
      if(ray_fixes > 100) {
        println(hlog, "major ray error");
        return;
        }
      goto back;
      }
  if(ray_fixes) println(hlog, "ray error x", ray_fixes, " centerover = ", centerover, " -> ", cs);
  
  glUniformMatrix4fv(o->uStart, 1, 0, glhr::tmtogl_transpose3(T).as_array());
  if(o->uLP != -1) glUniformMatrix4fv(o->uLP, 1, 0, glhr::tmtogl_transpose3(inverse(NLP)).as_array());
  GLERR("uniform mediump startid");
  glUniform1f(o->uIPD, vid.ipd);
  GLERR("uniform mediump IPD");
  
  if(o->uITOA != -1) {
    glUniformMatrix4fv(o->uITOA, 1, 0, glhr::tmtogl_transpose3(stretch::m_itoa).as_array());   
    glUniformMatrix4fv(o->uATOI, 1, 0, glhr::tmtogl_transpose3(stretch::m_atoi).as_array());   
    }

  if(o->uToOrig != -1) {
    glUniformMatrix4fv(o->uToOrig, 1, 0, glhr::tmtogl_transpose3(msm).as_array());   
    glUniformMatrix4fv(o->uFromOrig, 1, 0, glhr::tmtogl_transpose3(inverse(msm)).as_array());   
    }
  
  if(o->uWallOffset != -1) {
    glUniform1i(o->uWallOffset, intra::full_wall_offset(cs));
    glUniform1i(o->uSides, cs->type + (WDIM == 2 ? 2 : 0));
    }

  vector<glvertex> wallx, wally;
  vector<GLint> wallstart;

  if(intra::in) {
    intra::resetter ir;
    for(int i=0; i<isize(intra::data); i++) {
      intra::switch_to(i);
      load_walls(wallx, wally, wallstart);
      }
    }
  else
    load_walls(wallx, wally, wallstart);

  if(wall_via_texture) {
    int wlength = next_p2(isize(wallx));
    vector<array<float, 4>> w_map;
    w_map.resize(4 * wlength);
    ld minval = 9, maxval = -9;
    for(int i=0; i<isize(wallx); i++) {
      for(int a=0; a<4; a++) {
        w_map[i][a] = wallx[i][a]/ray_scale + .5;
        w_map[i+wlength][a] = wally[i][a]/ray_scale + .5;
        minval = min<ld>(minval, w_map[i][a]);
        minval = min<ld>(minval, w_map[i+wlength][a]);
        maxval = max<ld>(maxval, w_map[i][a]);
        maxval = max<ld>(maxval, w_map[i+wlength][a]);
        }
      }
    // println(hlog, "wallrange = ", tie(minval, maxval), " wallx = ", isize(wallx), " wallstart = ", isize(cgi.wallstart));
    for(int i=0; i<isize(wallstart); i++)
      w_map[i+2*wlength][0] = (wallstart[i]+.5) / wlength;
    bind_array(w_map, o->tWall, txWall, 8, wlength);
    glUniform1f(o->uInvLengthWall, 1. / wlength);
    }
  else {
    glUniform1iv(o->uWallstart, isize(wallstart), &wallstart[0]);  
    glUniform4fv(o->uWallX, isize(wallx), &wallx[0][0]);
    glUniform4fv(o->uWallY, isize(wally), &wally[0][0]);
    }

  if(o->uLevelLines != -1)
    glUniform1f(o->uLevelLines, levellines);
  if(o->uBinaryWidth != -1)
    glUniform1f(o->uBinaryWidth, vid.binary_width/2 * (nih?1:log(2)));
  #if CAP_SOLV
  if(o->uStraighten != -1) {
    glUniformMatrix4fv(o->uStraighten, 1, 0, glhr::tmtogl_transpose(asonov::straighten).as_array());
    }
  if(o->uReflectX != -1) {
    auto h = glhr::pointtogl(tangent_length(spin90() * asonov::ty, 2));
    glUniform4fv(o->uReflectX, 1, &h[0]);
    h = glhr::pointtogl(tangent_length(spin90() * asonov::tx, 2));
    glUniform4fv(o->uReflectY, 1, &h[0]);
    }
  #endif
  if(o->uPLevel != -1)
    glUniform1f(o->uPLevel, cgi.plevel / 2);
  
  #if CAP_BT
  if(o->uBLevel != -1)
    glUniform1f(o->uBLevel, log(bt::expansion()) / 2);
  #endif
  
  if(o->uLinearSightRange != -1)
    glUniform1f(o->uLinearSightRange, sightranges[geometry]);

  glUniform1f(o->uExpDecay, exp_decay_current());
  
  glUniform1f(o->uExpStart, exp_start);

  auto cols = glhr::acolor(darkena(backcolor, 0, 0xFF));
  if(o->uFogColor != -1)
    glUniform4f(o->uFogColor, cols[0], cols[1], cols[2], cols[3]);

  if(reset_rmap) rmap = nullptr, reset_rmap = false;

  if(!rmap) rmap = (unique_ptr<raycast_map>) new raycast_map;
  
  if(rmap->need_to_create(cs)) {
    rmap->create_all(cs);  
    if(reset_rmap) {
      reset_raycaster();
      cast();
      return;
      }
    if(rmap->gms_exceeded()) {
      if(isize(rmap->ms) > gms_limit || can_via_texture) {
        m_via_texture = true;
        wall_via_texture = true;
        println(hlog, "enabling m_via_texture");
        reset_raycaster();
        cast();        
        }
      else {
        gms_array_size = isize(rmap->ms);
        println(hlog, "changing gms_array_size to ", gms_array_size);
        reset_raycaster();
        cast();
        }
      return;
      }
    rmap->assign_uniforms(&*o);
    }

  // we may learn about this now...
  if(need_many_cell_types() && o->uWallOffset == -1) {
    reset_raycaster();
    cast();
    return;
    }

  GLERR("uniform mediump start");
  
  if(!o) { cast(); return; }
  uniform2(o->uStartid, rmap->enc(rmap->ids[cs], 0));
  }

  #if CAP_VERTEXBUFFER
  glhr::bindbuffer_vertex(screen);
  glVertexAttribPointer(hr::aPosition, 4, GL_FLOAT, GL_FALSE, sizeof(glvertex), 0);
  #else
  glVertexAttribPointer(hr::aPosition, 4, GL_FLOAT, GL_FALSE, sizeof(glvertex), &screen[0]);
  #endif
  
  if(ray::comparison_mode)
    glhr::set_depthtest(false);
  else {
    glhr::set_depthtest(true);
    glhr::set_depthwrite(true);
    }
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  if(!floor_textures) {
    println(hlog, "make_floor_textures called");
    make_floor_textures();
    check_cgi();
    }

  glActiveTexture(GL_TEXTURE0 + 0);
  glBindTexture(GL_TEXTURE_2D, floor_textures->renderedTexture);

  GLERR("bind");
  glDrawArrays(GL_TRIANGLES, 0, 6);
  GLERR("finish");
  }

EX namespace volumetric {

EX bool on;

EX map<cell*, color_t> vmap;

int intensity = 16;

EX void enable() {
  if(!on) {
    on = true;
    reset_raycaster();
    }
  }

EX void random_fog() {
  enable();
  for(cell *c: currentmap->allcells())
    vmap[c] = ((rand() % 0x1000000) << 8) | intensity;
  }

EX void menu() {
  cmode = sm::SIDE | sm::MAYDARK;
  gamescreen();
  dialog::init(XLAT("volumetric raycasting"));  

  if(!cheater) {
    dialog::addItem(XLAT("enable the cheat mode for additional options"), 'X');
    dialog::add_action(enable_cheat);
    dialog::addBack();
    dialog::display();
    return;
    }
  
  dialog::addBoolItem(XLAT("active"), on, 'a');
  dialog::add_action([&] {
    on = !on;
    reset_raycaster();
    });
  
  dialog::addSelItem(XLAT("intensity of random coloring"), its(intensity), 'i');
  dialog::add_action([] { 
    dialog::editNumber(intensity, 0, 255, 5, 15, "", "");
    dialog::reaction = random_fog;
    });

  dialog::addItem(XLAT("color randomly"), 'r');
  dialog::add_action(random_fog);

  dialog::addColorItem("color cell under cursor", vmap.count(centerover) ? vmap[centerover] : 0, 'c');
  dialog::add_action([&] {
    enable();
    dialog::openColorDialog(vmap[centerover]); 
    dialog::dialogflags |= sm::SIDE;      
    });

  dialog::addColorItem("color cell under player", vmap.count(cwt.at) ? vmap[cwt.at] : 0, 'p');
  dialog::add_action([&] {
    enable();
    dialog::openColorDialog(vmap[cwt.at]); 
    dialog::dialogflags |= sm::SIDE;      
    });
  
  dialog::addBreak(150);
  dialog::addHelp("This fills all the cells with glowing fog, for cool visualizations");
  dialog::addBreak(150);

  dialog::addBack();
  dialog::display();
  }
  
EX }

EX void configure() {
  cmode = sm::SIDE | sm::MAYDARK;
  gamescreen();
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

  if(is_stepbased()) {
    dialog::addSelItem(XLAT("max step"), fts(maxstep_current()), 'x');
    dialog::add_action([] {
      auto& ms = maxstep_current();
      dialog::editNumber(maxstep_current(), 1e-6, 1, .1,
        &ms == &maxstep_pro ? .05 :
        &ms == &maxstep_nil ? .1 : .5,
        XLAT("max step"), "affects the precision of solving the geodesic equation in Solv");
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

  dialog::addBoolItem(XLAT("volumetric raycasting"), volumetric::on, 'v');
  dialog::add_action_push(volumetric::menu);

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

  dialog::addBoolItem_action(XLAT("the map is fixed (improves performance)"), ray::fixed_map, 'F');
  
  if(gms_array_size > gms_limit && ray::in_use) {
    dialog::addBreak(100);
    dialog::addHelp(XLAT("unfortunately this honeycomb is too complex for the current implementation (%1>%2)", its(gms_array_size), its(gms_limit)));
    }

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
  else if(argis("-ray-range")) {
    PHASEFROM(2);
    shift_arg_formula(exp_start, reset_raycaster);
    shift_arg_formula(exp_decay_current(), reset_raycaster);
    }
  else if(argis("-ray-hard")) {
    PHASEFROM(2);
    shift_arg_formula(hard_limit);
    }
  else if(argis("-ray-out")) {
    PHASEFROM(2); shift(); color_out_of_range = argcolor(32);
    }
  else if(argis("-ray-comp")) {
    PHASEFROM(2);
    comparison_mode = true;
    }
  else if(argis("-ray-sol")) {
    PHASEFROM(2);
    shift(); max_iter_sol = argi();
    shift_arg_formula(maxstep_sol, reset_raycaster);
    reset_raycaster();
    }
  else if(argis("-ray-iter")) {
    PHASEFROM(2);
    shift(); max_iter_current() = argi();
    }
  else if(argis("-ray-step")) {
    PHASEFROM(2);
    println(hlog, "maxstep_current() is ", maxstep_current());
    shift_arg_formula(maxstep_current());
    }
  else if(argis("-ray-cells")) {
    PHASEFROM(2); shift();
    rays_generate = true;
    max_cells = argi();
    }
  else if(argis("-ray-reflect")) {
    PHASEFROM(2); 
    shift_arg_formula(reflect_val, reset_raycaster);
    }
  else if(argis("-ray-cells-no")) {
    PHASEFROM(2); shift();
    rays_generate = false;
    max_cells = argi();
    }
  else if(argis("-ray-random")) {
    start_game();
    shift(); volumetric::intensity = argi();
    volumetric::random_fog();
    }
  else if(argis("-ray-cursor")) {
    start_game();
    volumetric::enable();
    shift(); volumetric::vmap[centerover] = argcolor(32);
    }
  else return 1;
  return 0;
  }

auto hook = addHook(hooks_args, 100, readArgs)
 + addHook(hooks_clearmemory, 40, [] { rmap = {}; });
#endif

#if CAP_CONFIG
void addconfig() {
  param_f(exp_start, "ray_exp_start");
  param_f(exp_decay_exp, "ray_exp_decay_exp");
  param_f(maxstep_sol, "ray_maxstep_sol");
  param_f(maxstep_nil, "ray_maxstep_nil");
  param_f(minstep, "ray_minstep");
  param_f(reflect_val, "ray_reflect_val");
  param_f(hard_limit, "ray_hard_limit");
  addsaver(want_use, "ray_want_use");
  param_f(exp_decay_poly, "ray_exp_decay_poly");
  addsaver(max_iter_iso, "ray_max_iter_iso");
  addsaver(max_iter_sol, "ray_max_iter_sol");
  param_i(max_cells, "ray_max_cells");
  addsaver(rays_generate, "ray_generate");
  param_b(fixed_map, "ray_fixed_map");
  param_i(max_wall_offset, "max_wall_offset");
  param_i(max_celltype, "max_celltype");
  }
auto hookc = addHook(hooks_configfile, 100, addconfig);
#endif

#endif

#if !CAP_RAY
EX always_false in_use;
EX always_false comparison_mode;
EX void reset_raycaster() { }
EX void cast() { }
#endif
EX }
}
