// Hyperbolic Rogue -- shaders
// Copyright (C) 2011-2019 Zeno Rogue, see 'hyper.cpp' for details

/** \file shaders.cpp
 *  \brief shaders
 */

#include "hyper.h"
namespace hr {

EX ld levellines;
EX bool disable_texture;

#if CAP_GL
#if HDR
constexpr flagtype GF_TEXTURE  = 1;
constexpr flagtype GF_VARCOLOR = 2;
constexpr flagtype GF_LIGHTFOG = 4;
constexpr flagtype GF_LEVELS   = 8;
constexpr flagtype GF_TEXTURE_SHADED  = 16;
constexpr flagtype GF_NO_FOG   = 32;

constexpr flagtype GF_which    = 63;

constexpr flagtype SF_PERS3        = 256;
constexpr flagtype SF_BAND         = 512;
constexpr flagtype SF_USE_ALPHA    = 1024;
constexpr flagtype SF_DIRECT       = 2048;
constexpr flagtype SF_PIXELS       = 4096;
constexpr flagtype SF_HALFPLANE    = 8192;
constexpr flagtype SF_ORIENT       = 16384;
constexpr flagtype SF_BOX          = 32768;
constexpr flagtype SF_ZFOG         = 65536;
constexpr flagtype SF_ODSBOX       = (1<<17);

constexpr flagtype SF_SEMIDIRECT   = (1<<18);
#endif

EX bool solv_all;

#if HDR
/* standard attribute bindings */
/* taken from: https://www.opengl.org/sdk/docs/tutorials/ClockworkCoders/attributes.php */
constexpr int aPosition = 0;
constexpr int aColor = 3;
constexpr int aTexture = 8;

/* texture bindings */
constexpr int INVERSE_EXP_BINDING = 2;
constexpr int AIR_BINDING = 4;
#endif

EX map<string, shared_ptr<glhr::GLprogram>> compiled_programs;

EX map<unsigned, shared_ptr<glhr::GLprogram>> matched_programs;

glhr::glmatrix model_orientation_gl() {
  glhr::glmatrix s = glhr::id;
  for(int a=0; a<GDIM; a++)
    models::apply_orientation(s[a][1], s[a][0]);
  if(GDIM == 3) for(int a=0; a<GDIM; a++)
    models::apply_orientation_yz(s[a][2], s[a][1]);
  return s;
  }

EX void reset_all_shaders() {
  ray::reset_raycaster();
  compiled_programs.clear();
  matched_programs.clear();
  }

EX string panini_shader() {
  return
    "t.w += 1.; t *= 2. / t.w; t.w -= 1.;\n"
    "float s = t.z;\n"
    "float l = length(t.xyz);\n"
    "t /= max(length(t.xz), 1e-2);\n"
    "t.z += " + glhr::to_glsl(panini_alpha) + ";\n"
    "t *= l;\n"
    "t.w = 1.;\n";
  }

EX string stereo_shader() {
  return
    "t.w += 1.; t *= 2. / t.w; t.w -= 1.;\n"
    "float s = t.z;\n"
    "float l = length(t.xyz);\n"
    "t /= max(l, 1e-2);\n"
    "t.z += " + glhr::to_glsl(stereo_alpha) + ";\n"
    "t *= l;\n"
    "t.w = 1.;\n";
  }

EX string shader_lie_log() {
  if(nil) {
    return "vec4 lie_log(vec4 v) { v[2] += v[0] * v[1] / 2.; return v; }\n";
    }
  else if(sol && !nih) {
    return "vec4 lie_log(vec4 v) { if(abs(v[2]) > 1e-6) { v[0] *= -v[2] / (exp(-v[2])-1.); v[1] *= v[2] / (exp(v[2])-1.); } return v; }\n";
    }
  else if(sol && nih) {
    return "vec4 lie_log(vec4 v) { if(abs(v[2]) > 1e-6) { float z = v[2] * log(2); v[0] *= -z / (exp(-z)-1.); z = v[2] * log(3); v[1] *= z / (exp(z)-1.); } return v; }\n";
    }
  else if(nih) {
    return "vec4 lie_log(vec4 v) { if(abs(v[2]) > 1e-6) { float z = v[2] * log(2); v[0] *= z / (exp(z)-1.); z = v[2] * log(3); v[1] *= z / (exp(z)-1.); } return v; }\n";
    }
  else if(hyperbolic) {
    return "vec4 lie_log(vec4 v) { v = deparabolic13(v); v[3] = 1.; /* if(abs(v[0]) > 1e-6) { float m = v[0] / (exp(v[0]) - 1.); v[1] *= m; v[2] *= m; } */ return v; }\n";
    }
  else if(sl2) {
    return shader_rel_log() + "vec4 lie_log(vec4 h) { return rel_log(h); }\n";
    }
  else {
    return "vec4 lie_log(vec4 v) { return v; }\n";
    }
  }

EX string shader_rel_log() {
  if(sl2) return
  "uniform mediump float uIndexSL;\n"
  "vec4 rel_log(vec4 h) {\n"
    "float shift = uIndexSL + atan2(h[2], h[3]); \n"
    "float ca = cos(uIndexSL); float sa = -sin(uIndexSL);\n"
    "vec4 h1 = h;\n"
    "h[2] = h1[2] * ca - h1[3] * sa; h[3] = h1[3] * ca + h1[2] * sa;\n"
    "h[0] = h1[0] * ca - h1[1] * sa; h[1] = h1[1] * ca + h1[0] * sa;\n"
    "h1 = h;"

    "if(h1[3] <= 1. && h1[3] >= -1.) {\n"
    "float r = sqrt(h1[2]*h1[2] - h1[0]*h1[0] - h1[1]*h1[1]);\n"
    "float z = asin_clamp(r);\n"
    "if(h1[3] < 0.) z = PI - z;\n"
    "z += floor(shift / 2. / PI + .5) * 2. * PI;\n"
    "float scale = z/r;\n"
    "h1 = h1 * scale; h1[3] = 1.;\n"
    "} else if(shift > PI || shift < -PI || h1[3] < -1.) { return vec4(0,0,0,1); } else {\n"

    "float r = sqrt(h1[0]*h1[0] + h1[1]*h1[1] - h1[2]*h1[2]);\n"
    "float z = asinh(r);\n"
    "float scale = z/r;\n"
    "h1 = h1 * scale; h1[3] = 1.;\n"
    "}\n"

    "return h1;\n"
    "}\n";

  if(hyperbolic && GDIM == 3) return
    "vec4 rel_log(vec4 h) {\n"
    "  float choice = h[3] * h[3] - h[0] * h[0] - h[1] * h[1];\n"
    "  float z, r;\n"
    "  if(choice > 0.) { r = sqrt(choice); z = asinh(r); }\n"
    "  else { r = sqrt(-choice); z = asin_clamp(r); if(h[2] < 0.) z = PI - z; }\n"
    "  h = h * z / r; h[2] = h[3]; h[3] = 1.;\n"
    "  return h;\n"
    "  }\n";

  println(hlog, "geometry is: ", geometry);
  throw hr_exception("shader_rel_log in wrong geometry");
  }

shared_ptr<glhr::GLprogram> write_shader(flagtype shader_flags) {
  string varying, vsh, fsh, vmain = "void main() {\n", fmain = "void main() {\n";

  vsh += "attribute mediump vec4 aPosition;\n";
  varying += "varying mediump vec4 vColor;\n";

  fmain += "gl_FragColor = vColor;\n";
  
  bool have_texture = false;
  
  if(shader_flags & GF_TEXTURE_SHADED) {
    vsh += "attribute mediump vec3 aTexture;\n";
    varying += "varying mediump vec3 vTexCoord;\n";
    fsh += "uniform mediump sampler2D tTexture;\n";
    vmain += "vTexCoord = aTexture;\n";
    fmain += "gl_FragColor *= texture2D(tTexture, vTexCoord.xy);\n";
    fmain += "gl_FragColor.rgb *= vTexCoord.z;\n";
    }
  else if(shader_flags & GF_TEXTURE) {
    vsh += "attribute mediump vec2 aTexture;\n";
    varying += "varying mediump vec2 vTexCoord;\n";
    fsh += "uniform mediump sampler2D tTexture;\n";
    vmain += "vTexCoord = aTexture;\n",
    have_texture = true;
    }
  if(shader_flags & GF_LEVELS) {
    fsh += "uniform mediump float uLevelLines;\n";
    varying += "varying mediump vec4 vPos;\n";
    fmain += "gl_FragColor.rgb *= 0.5 + 0.5 * cos(vPos.z/vPos.w * uLevelLines * 2. * PI);\n";
    }
  
  if(shader_flags & GF_VARCOLOR) {
    vsh += "attribute mediump vec4 aColor;\n";
    vmain += "vColor = aColor;\n";
    }
  else {
    vmain += "vColor = uColor;\n";
    vsh += "uniform mediump vec4 uColor;\n";
    }
  
  bool have_vfogs = false, have_vfogcolor = false;

  if(shader_flags & GF_LIGHTFOG) {
    vsh += "uniform mediump float uFog;\n";
    vmain += "vFogs = clamp(1.0 + aPosition.z * uFog, 0.0, 1.0);\n";
    have_vfogs = true;
    }

  string coordinator;
  string distfun;
  bool treset = false;
  
  bool dim2 = GDIM == 2;
  bool dim3 = GDIM == 3;
  
  bool skip_t = false;
  
  bool azi_hyperbolic = false;
  
  if(vid.stereo_mode == sODS) {
    shader_flags |= SF_DIRECT | SF_ODSBOX;
    vmain += "// this is ODS shader\n";
    distfun = "aPosition.z";
    }
  else if(pmodel == mdPixel) {
    vmain += "mediump vec4 pos = aPosition; pos[3] = 1.0;\n";
    vmain += "pos = uMV * pos;\n";
    if(shader_flags & GF_LEVELS) vmain += "vPos = pos;\n";  
    vmain += "gl_Position = uP * pos;\n";
    skip_t = true;
    shader_flags |= SF_PIXELS | SF_DIRECT;
    }
  else if(pmodel == mdManual) {
    vmain += "mediump vec4 pos = uMV * aPosition;\n";
    if(shader_flags & GF_LEVELS)
      vmain += "vPos = pos;\n";
    vmain += "gl_Position = uP * pos;\n";
    skip_t = true;
    shader_flags |= SF_DIRECT;
    }
  else if(!vid.consider_shader_projection && semidirect_rendering && models::is_perspective(pmodel)) {
    vmain += "// this\n";
    distfun = "length(t.xyz)";
    shader_flags |= SF_PERS3 | SF_SEMIDIRECT;
    }
  else if(!vid.consider_shader_projection) {
    shader_flags |= SF_PIXELS;
    }        
  else if(among(pmodel, mdDisk, mdBall) && GDIM == 2 && vrhr::rendering() && !sphere && !(hyperbolic && pconf.alpha < 0 && pconf.alpha > -1)) {
    shader_flags |= SF_DIRECT | SF_BOX;
    vsh += "uniform mediump float uAlpha, uDepth, uDepthScaling, uCamera;";
    
    if(hyperbolic) coordinator += 
      "float zlev = sqrt(t.z*t.z-t.x*t.x-t.y*t.y);\n"
      "float zl = uDepth - uDepthScaling * (uDepth - atanh(tanh(uDepth)/zlev));\n"
      "float dd = sqrt(t.x*t.x+t.y*t.y);\n"
      "float d  = acosh(t.z/zlev);\n"
      "float uz = uAlpha + cosh(zl) * cosh(d) * cosh(uCamera) + sinh(zl) * sinh(uCamera);\n"
      "float ux = cosh(zl) * sinh(d) / uz;\n"      
      "t.xy = ux * t.xy / dd;\n"
      "t.z = (sinh(zl) * cosh(uCamera) + sinh(uCamera) * cosh(zl) * cosh(d)) / uz;\n"
      ;
    
    else if(euclid) coordinator +=
      "t.z = uDepth * (1. - (t.z - 1.) * uDepthScaling) + uAlpha + uCamera;\n";

    else if(sphere) coordinator += 
      "float zlev = sqrt(t.z*t.z+t.x*t.x+t.y*t.y);\n"
      "float zl = uDepth - uDepthScaling * (uDepth - atan(tan(uDepth)/zlev));\n"
      "float dd = sqrt(t.x*t.x+t.y*t.y);\n"
      "float d  = acos(t.z/zlev);\n"
      "float uz = uAlpha + cos(zl) * cos(d) * cos(uCamera) + sin(zl) * sin(uCamera);\n"
      "float ux = cos(zl) * sin(d) / uz;\n"
      "t.xy = ux * t.xy / dd;\n"
      "t.z = (sin(uCamera) * cos(zl) * cos(d) - sin(zl) * cos(uCamera)) / uz;\n"
      ;
    }
  else if(pmodel == mdDisk && MDIM == 3 && !spherespecial && !gproduct) {
    shader_flags |= SF_DIRECT;
    }
  else if(glhr::noshaders) {
    shader_flags |= SF_PIXELS;
    }
  else if(pmodel == mdDisk && GDIM == 3 && !spherespecial && !nonisotropic && !gproduct) {
    coordinator += "t /= (t[3] + uAlpha);\n";
    vsh += "uniform mediump float uAlpha;\n";
    shader_flags |= SF_DIRECT | SF_BOX | SF_ZFOG;
    treset = true;
    }
  else if(pmodel == mdConformalSquare && pconf.model_transition == 1) {
    shader_flags |= SF_ORIENT | SF_DIRECT;
    coordinator += "t = uPP * t;", vsh += "uniform mediump mat4 uPP;";
    coordinator += "t = to_square(t);";
    }
  else if(pmodel == mdBand && hyperbolic) {
    shader_flags |= SF_BAND | SF_ORIENT | SF_BOX | SF_DIRECT;
    coordinator += "t = uPP * t;", vsh += "uniform mediump mat4 uPP;";
    if(dim2) coordinator += "mediump float zlev = zlevel(t); t /= zlev;\n";
    if(dim3) coordinator += "mediump float r = sqrt(t.y*t.y+t.z*t.z); float ty = asinh(r);\n";
    if(dim2) coordinator += "mediump float ty = asinh(t.y);\n";
    coordinator += "mediump float tx = asinh(t.x / cosh(ty)); ty = 2.0 * atan(tanh(ty/2.0));\n";
    if(dim2) coordinator += "t[0] = tx; t[1] = ty; t[2] = 1.0; t[3] = 1.0;\n";
    if(dim3) coordinator += "t[0] = tx; t[1] = ty*t.y/r; t[2] = ty*t.z/r; t[3] = 1.0;\n";
    if(dim3) shader_flags |= SF_ZFOG;
    }
  else if(pmodel == mdHalfplane && hyperbolic) {
    shader_flags |= SF_HALFPLANE | SF_ORIENT | SF_BOX | SF_DIRECT;
    if(dim2) shader_flags |= SF_USE_ALPHA;
    coordinator += "t = uPP * t;", vsh += "uniform mediump mat4 uPP;";
    if(dim2) coordinator += 
      "mediump float zlev = zlevel(t); t /= zlev;\n"
      "t.xy /= t.z; t.y += 1.0;\n"
      "mediump float rads = dot(t.xy, t.xy);\n"
      "t.xy /= -rads; t.z = 1.0; t[3] = 1.0;\n";
    if(dim3) coordinator += 
      "t.xyz /= (t.w + 1.0); t.y += 1.0;\n"
      "mediump float rads = dot(t.xyz, t.xyz);\n"
      "t.xyz /= -rads; t[3] = 1.0;\n";        
    if(dim3) shader_flags |= SF_ZFOG;
    }
  else if(pmodel == mdLiePerspective) {
    shader_flags |= SF_PERS3 | SF_DIRECT;
    if(hyperbolic) {
      shader_flags |= SF_ORIENT;
      coordinator += "t = uPP * t;", vsh += "uniform mediump mat4 uPP;";
      }
    coordinator += "t = lie_log(t);\n";
    distfun = "length(t.xyz)";
    vsh += shader_lie_log();
    }
  else if(pmodel == mdRelPerspective) {
    shader_flags |= SF_PERS3 | SF_DIRECT;
    coordinator += "t = rel_log(t);\n";
    distfun = "length(t.xyz)";
    vsh += shader_rel_log();
    }
  else if(pmodel == mdGeodesic) {
    shader_flags |= SF_PERS3 | SF_DIRECT;
    coordinator += "t = inverse_exp(t);\n";
    if(sn::in()) {
      coordinator +=
        "mediump float d = dot(t.xyz, t.xyz);\n"
        "mediump float hz = (1.+d) / (1.-d);\n"
        "mediump float ad = acosh(hz);\n"
        "mediump float m = d == 0. ? 0. : d >= 1. ? 1.e4 : (hz+1.) * ad / sinh(ad);\n";
      #if CAP_VR
      if(vrhr::rendering_eye())
        coordinator += "t.xyz *= ad/d;\n";
      else
      #endif
      coordinator +=
        "t.xyz *= m;\n";
      distfun = "ad";
      }
    else
      distfun = "length(t.xyz)";
    switch(cgclass) {
      #if CAP_SOLV
      case gcSol:
        if(solv_all) {
          vsh += "\n#define SOLV_ALL\n";
          }
        vsh += sn::shader_symsol;
        break;
      case gcNIH:
        vsh += sn::shader_nsym;
        break;
      case gcSolN:
        vsh += sn::shader_nsymsol;
        break;
      #endif
      case gcNil:
        vsh += nilv::nilshader();
        break;
      case gcSL2:      
        vsh += slr::slshader;
        break;
      default:
        println(hlog, "error: unknown geometry in geodesic");
        break;
      }      
    }
  else if(in_h2xe() && pmodel == mdPerspective) {
    shader_flags |= SF_PERS3 | SF_DIRECT;
    coordinator +=      
      "mediump float z = log(t[2] * t[2] - t[0] * t[0] - t[1] * t[1]) / 2.;\n"
      "mediump float r = length(t.xy);\n"
      "mediump float t2 = t[2] / exp(z);\n"
      "mediump float d = t2 >= 1. ? acosh(t2) : 0.;\n"
      "if(r != 0.) r = d / r;\n"
      "t.xy *= r; t.z = z;\n";
    distfun = "sqrt(z*z+d*d)";
    treset = true;
    }
  else if(in_e2xe() && pmodel == mdPerspective) {
    shader_flags |= SF_PERS3 | SF_DIRECT;
    coordinator +=      
      "t.xy /= t.z;\n"
      "t.z = log(t.z);\n";
    distfun = "length(t.xyz)";
    treset = true;
    }
  else if(in_s2xe() && pmodel == mdPerspective) {
    shader_flags |= SF_PERS3 | SF_DIRECT;
    distfun = "length(t.xyz)", treset = true;
    }
  else if(pmodel == mdPerspective) {
    shader_flags |= SF_PERS3 | SF_DIRECT;
    if(sl2) {
      vsh += "uniform mediump float uIndexSL;\n";
      coordinator +=
        "float ca = cos(uIndexSL); float sa = -sin(uIndexSL);\n"
        "vec4 h1 = t;\n"
        "t[2] = h1[2] * ca - h1[3] * sa; t[3] = 1.;\n"
        "t[0] = h1[0] * ca - h1[1] * sa; t[1] = h1[1] * ca + h1[0] * sa;\n";
      }
    #if CAP_VR
    if(vrhr::rendering() && hyperbolic && vrhr::eyes != vrhr::eEyes::truesim) {
      azi_hyperbolic = true;
      coordinator += "mediump vec4 orig_t = t;\n";
      coordinator += 
        "t = t * acosh(t[3]) / length(t.xyz);\n"
        "t[3] = 1.;\n";
      distfun = "length(t.xyz)";
      }
    else 
    #endif
    if(hyperbolic)
      distfun = "acosh(t[3])";
    else if(euclid || nonisotropic || stretch::in() || (sphere && ray::in_use))
      distfun = "length(t.xyz)", treset = true;
    else {
      if(spherephase & 4) coordinator += "t = -t;\n";
      switch(spherephase & 3) {
        case 0: distfun = "(2. * PI - acos(-t[3]))"; coordinator += "t = -t;\n"; break;
        case 1: distfun = "(2. * PI - acos(t[3]))"; coordinator += "t.xyz = -t.xyz;\n"; break;
        case 2: distfun = "acos(-t[3])"; coordinator += "t.w = -t.w;\n"; break;
        case 3: distfun = "acos(t[3])"; break;
        }
      }
    }
  else {
    shader_flags |= SF_PIXELS;
    if(dim3) shader_flags |= SF_ZFOG;
    }
  
  #if CAP_VR
  /* no z-fog in VR */
  if((shader_flags & SF_ZFOG) && vrhr::rendering())
    shader_flags &= ~SF_ZFOG;
  #endif

  if(nil && pmodel == mdPerspective)  {
    vsh += "uniform mediump float uRotCos, uRotSin, uRotNil;\n";
    coordinator +=
      "t.z += (uRotCos * t.x + uRotSin * t.y) * (uRotCos * t.y - uRotSin * t.x) * uRotNil / 2. - " + glhr::to_glsl(nilv::model_used) + " * t.x * t.y / 2.;\n";
    }

  if(!skip_t) {
    vmain += "mediump vec4 t = uMV * aPosition;\n";
    vmain += coordinator;
    if(shader_flags & GF_NO_FOG) {
      vmain += "// no fog used\n";
      }
    else if(GDIM == 3 && WDIM == 2 && hyperbolic && context_fog && cgi.emb->is_same_in_same() && pmodel == mdPerspective) {
      vsh += 
        "uniform mediump mat4 uRadarTransform;\n"
        "uniform mediump sampler2D tAirMap;\n"
        "uniform mediump float uFog;\n"
        "uniform mediump float uFogBase;\n"
        "vec4 color_at(vec4 ending, float dist) {"
        "    vec3 pt = ending.xyz * sinh(dist);\n"
        "    pt.xy /= sqrt(pt.z*pt.z+1.);\n"
        "    pt.xy /= 2. * (1. + sqrt(1.+pt.x*pt.x+pt.y*pt.y));\n"
        "    pt.xy += vec2(.5, .5);\n"
        "    return texture2D(tAirMap, pt.xy);\n"
        "    }\n";
      
      
      if(azi_hyperbolic) vmain += 
        "vec4 ending = uRadarTransform * orig_t;\n";
      else vmain += 
        "vec4 ending = uRadarTransform * t;\n";
      
      have_vfogs = true; have_vfogcolor = true;

      vmain += 
        "float len = acosh(ending.w);\n"
        "float eulen = length(ending.xyz);\n"
        "ending.xyz /= eulen;\n"
        "ending.y *= -1.;\n"
        "vec4 fog = vec4(1e-3,0,1e-3,1e-3);\n"
        "vec4 last = vec4(0,0,0,0);\n"
        "for(int i=0; i<50; i++) {\n"
        "  vec4 px = color_at(ending, ((float(i) + .5) / 50.) * min(len, uFog));\n"
        "  if(px.r < .9 || px.b < .9 || px.g > .1) last = px;\n"
        "  fog += last;\n"
        "  }\n"
        "mediump float fogs = (uFogBase - len / uFog);\n"        
        "if(fogs < 0.) fogs = 0.;\n"
        "vFogs = fogs; vFogColor = fog / fog.w;\n";
      }
    else if(distfun != "") {
      have_vfogs = true;
      if(logfog)
        vmain += "vFogs = uFogBase * exp(- " + distfun + " / uFog);\n";
      else
        vmain += "vFogs = clamp(uFogBase - " + distfun + " / uFog, 0.0, 1.0);\n";
      vsh += 
        "uniform mediump float uFog;\n"
        "uniform mediump float uFogBase;\n";
      }
    if(shader_flags & GF_LEVELS) vmain += "vPos = t;\n";  
    if(treset) vmain += "t[3] = 1.0;\n";
    
    if((shader_flags & SF_PERS3) && panini_alpha && !vrhr::rendering_eye()) {
      vmain += "t = uPP * t;", vsh += "uniform mediump mat4 uPP;";
      /* panini */
      vmain += panini_shader();
      shader_flags |= SF_ORIENT;
      }
    else if((shader_flags & SF_PERS3) && stereo_alpha && !vrhr::rendering_eye()) {
      vmain += stereo_shader();
      }
      
    vmain += "gl_Position = uP * t;\n";
    }

  if(shader_flags & SF_ZFOG) {
    have_vfogs = true;
    vmain += 
      "vFogs = 0.5 - gl_Position.z / 2.0;\n";
    }
  
  if(have_texture) {
    fmain += "gl_FragColor *= texture2D(tTexture, vTexCoord);\n";
    fmain += "if(gl_FragColor.a == 0.) discard;\n";
    }

  if(have_vfogcolor) {
    varying += 
      "varying mediump vec4 vFogColor;\n"
      "varying mediump float vFogs;\n";
    fmain += "gl_FragColor.xyz = gl_FragColor.xyz * vFogs + vFogColor.xyz * (1.0-vFogs);\n";
    }
  else if(have_vfogs) {
    varying += 
      "uniform mediump vec4 uFogColor;\n"
      "varying mediump float vFogs;\n";
    fmain += "gl_FragColor.xyz = gl_FragColor.xyz * vFogs + uFogColor.xyz * (1.0-vFogs);\n";
    }

  vsh += 
    "uniform mediump mat4 uMV;\n"
    "uniform mediump mat4 uP;\n";

  vmain += "}";
  fmain += "}";
  
  fsh += varying;
  fsh += fmain;
  vsh += varying;
  vsh += vmain;
  
  if(glhr::noshaders || !vid.usingGL) fsh = vsh = "";
  
  string both = fsh + "*" + vsh + "*" + llts(shader_flags);
  if(compiled_programs.count(both)) 
    return compiled_programs[both];
  else {
    auto res = make_shared<glhr::GLprogram>(vsh, fsh);
    res->shader_flags = shader_flags;
    return res;
    }    
  }

void display_data::set_projection(int ed, ld shift) {
  flagtype shader_flags = current_display->next_shader_flags;
  unsigned id;
  id = cgclass;
  if(stretch::in()) id = 15;
  id <<= 1; if(GDIM == 3) id |= 1;
  id <<= 1; if(embedded_plane) id |= 1;
  if(GDIM == 2 && hyperbolic && pconf.alpha < 0 && pconf.alpha > -1) id |= 1;
  id <<= 3; id |= vid.stereo_mode;
  id <<= 6; id |= pmodel;
  if(levellines && pmodel != mdPixel) {
    shader_flags |= GF_LEVELS;
    if(disable_texture) shader_flags &=~ GF_TEXTURE;
    }
  id <<= 6; id |= shader_flags;
  id <<= 6; id |= spherephase;
  id <<= 1; if(vid.consider_shader_projection) id |= 1;
  #if CAP_VR
  id <<= 3; id |= vrhr::state;
  if(vrhr::rendering() && vrhr::eyes == vrhr::eEyes::truesim) id += 3;
  #endif
  id <<= 2; id |= (spherespecial & 3);
  if(sol && solv_all) id |= 1;
  if(in_h2xe()) id |= 1;
  if(in_s2xe()) id |= 2;
  if(WDIM == 2 && GDIM == 3 && hyperbolic && context_fog && cgi.emb->is_same_in_same()) id |= 1;
  shared_ptr<glhr::GLprogram> selected;

  if(matched_programs.count(id)) selected = matched_programs[id];
  else {
    selected = write_shader(shader_flags);
    matched_programs[id] = selected;
    }

  if(glhr::current_glprogram != selected) full_enable(selected);

  shader_flags = selected->shader_flags;
  auto cd = current_display;
  
  #if CAP_SOLV
  if(selected->uPRECX != -1) {
    auto &tab = sn::get_tabled();
    
    GLuint invexpid = tab.get_texture_id();
    
    glActiveTexture(GL_TEXTURE0 + INVERSE_EXP_BINDING);
    glBindTexture(GL_TEXTURE_3D, invexpid);

    glActiveTexture(GL_TEXTURE0 + 0);
    
    glhr::set_solv_prec(tab.PRECX, tab.PRECY, tab.PRECZ);
    }
  #endif
  
  #if MAXMDIM >= 4
  if(selected->tAirMap != -1 && airbuf) {
    glActiveTexture(GL_TEXTURE0 + AIR_BINDING);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, airbuf->renderedTexture);
    glUniform1i(selected->tAirMap, AIR_BINDING);
    glActiveTexture(GL_TEXTURE0 + 0);
    glUniformMatrix4fv(selected->uRadarTransform, 1, 0, glhr::tmtogl_transpose3(radar_transform).as_array());
    }
  #endif
  
  if(selected->uIterations != -1) {
    glhr::set_index_sl(0);
    glhr::set_sv(stretch::not_squared());
    glhr::set_sl_iterations(slr::shader_iterations);
    }

  glhr::new_projection();
  
  #if CAP_VR
  if(!vrhr::rendering_eye()) {
  #else
  if(true) {
  #endif
    if(ed && vid.stereo_mode == sLR) {
      glhr::projection_multiply(glhr::translate(ed, 0, 0));
      glhr::projection_multiply(glhr::scale(2, 1, 1));
      }      
  
    ld tx = (cd->xcenter-cd->xtop)*2./cd->xsize - 1;
    ld ty = (cd->ycenter-cd->ytop)*2./cd->ysize - 1;
    glhr::projection_multiply(glhr::translate(tx, -ty, 0));

    if(pmodel == mdManual) return;
    
    if(pconf.stretch != 1 && (shader_flags & SF_DIRECT) && pmodel != mdPixel) glhr::projection_multiply(glhr::scale(1, pconf.stretch, 1));
  
    if(vid.stereo_mode != sODS)
      eyewidth_translate(ed);
    }
  
  auto bcolor = models::is_perspective(pmodel) ? backcolor : (modelcolor >> 8);

  auto ortho = [&] (ld x, ld y) {
    glhr::glmatrix M = glhr::ortho(x, y, 1);
    if(shader_flags & SF_ZFOG) {
      M[2][2] = 2 / (pconf.clip_max - pconf.clip_min);
      M[3][2] = (pconf.clip_min + pconf.clip_max) / (pconf.clip_max - pconf.clip_min);
      auto cols = glhr::acolor(darkena(bcolor, 0, 0xFF));
      glUniform4f(selected->uFogColor, cols[0], cols[1], cols[2], cols[3]);
      }
    else M[2][2] /= 10000;
    glhr::projection_multiply(M);
    if(nisot::local_perspective_used && (shader_flags & SF_BOX))
      glhr::projection_multiply(glhr::tmtogl_transpose(NLP));
    if(ed && vid.stereo_mode != sODS) {
      glhr::glmatrix m = glhr::id;
      m[2][0] -= ed;
      glhr::projection_multiply(m);
      }
    glhr::id_modelview();
    };

  bool u_alpha = false;

  glhr::glmatrix pp0 = glhr::id;
  
  auto use_mv = [&] {
    #if CAP_VR
    auto cd = current_display;
    if(vrhr::rendering_eye()) {
      glhr::projection_multiply(glhr::tmtogl_transpose(vrhr::hmd_mvp));
      glhr::id_modelview();
      }
    else {
      glhr::projection_multiply(glhr::frustum(cd->tanfov, cd->tanfov * cd->ysize / cd->xsize));
      if(selected->uPP != -1) {
        transmatrix swapz = Id;
        swapz[2][2] = -1;
        pp0 = glhr::tmtogl_transpose(swapz * vrhr::hmd_mv);
        glhr::projection_multiply(glhr::tmtogl(swapz));
        }
      else
        glhr::projection_multiply(glhr::tmtogl_transpose(vrhr::hmd_mv));
      }
    #endif
    };
  
  if(shader_flags & SF_PIXELS) {
    if(vrhr::rendering()) use_mv();
    else ortho(cd->xsize/2, -cd->ysize/2);
    }
  else if(shader_flags & SF_BOX) {
    if(vrhr::rendering()) use_mv();
    else ortho(cd->xsize/current_display->radius/2, -cd->ysize/current_display->radius/2);
    }
  else if(shader_flags & SF_ODSBOX) {
    ortho(M_PI, M_PI);
    glhr::fog_max(1/sightranges[geometry], darkena(bcolor, 0, 0xFF));
    }
  else if(shader_flags & SF_PERS3) {
    if(vrhr::rendering()) use_mv();
    else {
      glhr::projection_multiply(glhr::frustum(cd->tanfov, cd->tanfov * cd->ysize / cd->xsize));
      glhr::projection_multiply(glhr::scale(1, -1, -1));
      if(nisot::local_perspective_used) {
        if(gproduct) {
          for(int i=0; i<3; i++) NLP[3][i] = NLP[i][3] = 0;
          NLP[3][3] = 1;
          }
        if(!(shader_flags & SF_ORIENT))
          glhr::projection_multiply(glhr::tmtogl_transpose(NLP));
        }
      if(ed) {
        glhr::using_eyeshift = true;
        glhr::eyeshift = glhr::tmtogl(xpush(vid.ipd * ed/2));
        }
      }
    glhr::fog_max(1/sightranges[geometry], darkena(bcolor, 0, 0xFF));
    }
  else {
    if(pconf.alpha > -1) {
      // Because of the transformation from H3 to the Minkowski hyperboloid,
      // points with negative Z can be generated in some 3D settings.
      // This happens for points below the camera, but above the plane.
      // These points should still be viewed, though, so we disable the
      // depth clipping
      glhr::projection_multiply(glhr::scale(1,1,0));
      }
    GLfloat sc = current_display->radius / (cd->ysize/2.);
    glhr::projection_multiply(glhr::frustum(cd->xsize / cd->ysize, 1));
    glhr::projection_multiply(glhr::scale(sc, -sc, -1));
    if(pconf.back_and_front)
      glhr::projection_multiply(glhr::scale(-1,-1,-1));
    u_alpha = true;
    }

  if(selected->uRotNil != -1) {
    glUniform1f(selected->uRotCos, models::ocos);
    glUniform1f(selected->uRotSin, models::osin);
    glUniform1f(selected->uRotNil, pconf.rotational_nil);
    }

  if(selected->uPP != -1) {

    glhr::glmatrix pp = glhr::id;
    if(get_shader_flags() & SF_USE_ALPHA)
      pp[3][2] = GLfloat(pconf.alpha);

    pp = pp * pp0;    

    if(nisot::local_perspective_used)
      pp = glhr::tmtogl_transpose(NLP) * pp;

    if(get_shader_flags() & SF_ORIENT) {
      if(GDIM == 3) for(int a=0; a<4; a++) 
        models::apply_orientation_yz(pp[a][1], pp[a][2]);
      for(int a=0; a<4; a++) 
        models::apply_orientation(pp[a][0], pp[a][1]);
      }
    
    glUniformMatrix4fv(selected->uPP, 1, 0, pp.as_array());
    }
  
  if(selected->uAlpha != -1)
    glhr::set_ualpha(pconf.alpha);

  if(selected->uDepth != -1)
    glUniform1f(selected->uDepth, vid.depth);

  if(selected->uCamera != -1)
    glUniform1f(selected->uCamera, vid.camera);

  if(selected->uDepthScaling != -1)
    glUniform1f(selected->uDepthScaling, pconf.depth_scaling);

  if(selected->uLevelLines != -1) {
    glUniform1f(selected->uLevelLines, levellines);
    }

  if(selected->shader_flags & SF_ORIENT)
    glhr::projection_multiply(model_orientation_gl());

  if(selected->shader_flags & SF_BAND)
    glhr::projection_multiply(glhr::scale(1 / 90._deg, 1 / 90._deg, GDIM == 3 ? 1/90._deg : 1));

  if(selected->shader_flags & SF_BAND) {
    glhr::projection_multiply(glhr::translate(shift, 0, 0));
    }

  if(in_h2xe() || in_s2xe()) {
    glhr::projection_multiply(glhr::translate(0, 0, shift));
    }

  if(selected->shader_flags & SF_HALFPLANE) {
    glhr::projection_multiply(glhr::translate(0, 1, 0));      
    glhr::projection_multiply(glhr::scale(-1, 1, 1));
    glhr::projection_multiply(glhr::scale(pconf.halfplane_scale, pconf.halfplane_scale, GDIM == 3 ? pconf.halfplane_scale : 1));
    glhr::projection_multiply(glhr::translate(0, 0.5, 0));
    }      
  
  if(pconf.camera_angle && pmodel != mdPixel) {
    ld cam = pconf.camera_angle * degree;

    GLfloat cc = cos(cam);
    GLfloat ss = sin(cam);
    
    GLfloat yzspin[16] = {
      1, 0, 0, 0,
      0, cc, ss, 0,
      0, -ss, cc, 0,
      0, 0, 0, 1
      };
    
    glhr::projection_multiply(glhr::as_glmatrix(yzspin));
    }
  
  if(u_alpha) {
    glhr::projection_multiply(glhr::translate(0, 0, pconf.alpha));
    if(ed) glhr::projection_multiply(glhr::translate(vid.ipd * ed/2, 0, 0));
    }
  }

EX void add_if(string& shader, const string& seek, const string& function) {
  if(shader.find(seek) != string::npos)
    shader = function + shader;
  }

EX void add_fixed_functions(string& shader) {
  /* from the most complex to the simplest */

  add_if(shader, "to_square",
    "mediump vec4 to_square(mediump vec4 h) {\n"
    "float d = length(h.xy);\n"
    "float x = d / (h.z + 1.);\n"

    "float cos_phiosqrt2 = sqrt(2.) / (x + 1./x);\n"
    "float cos_lambda = -h.y / d;\n"
    "float sin_lambda = h.x / d;\n"
    "float cos_a = cos_phiosqrt2 * (sin_lambda + cos_lambda);\n"
    "float cos_b = cos_phiosqrt2 * (sin_lambda - cos_lambda);\n"
    "float sin_a = sqrt(1. - cos_a * cos_a);\n"
    "float sin_b = sqrt(1. - cos_b * cos_b);\n"
    "float cos_a_cos_b = cos_a * cos_b;\n"
    "float sin_a_sin_b = sin_a * sin_b;\n"
    "float sin2_m = 1.0 + cos_a_cos_b - sin_a_sin_b;\n"
    "float sin2_n = 1.0 - cos_a_cos_b - sin_a_sin_b;\n"
    "float sin_m = sqrt_clamp(sin2_m);\n"
    "float cos_m = sqrt_clamp(1. - sin2_m);\n"
    "if(sin_lambda < 0.) sin_m = -sin_m;\n"
    "float sin_n = sqrt_clamp(sin2_n);\n"
    "float cos_n = sqrt_clamp(1.0 - sin2_n);\n"
    "if(cos_lambda > 0.0) sin_n = -sin_n;\n"
    "#define divby 0.53935260118837935472\n"
    "vec4 res = vec4(ellFaux(cos_m,sin_m,sqrt(2.)/2.) * divby, ellFaux(cos_n,sin_n,sqrt(2.)/2.) * divby, 0, 1);\n"
    "if(x > 1.) {\n"
    "  if(abs(res[0]) > abs(res[1])) {\n"
    "    if(res[0] > 0.) res[0] = 2. - res[0]; else res[0] = -2. - res[0];\n"
    "    }\n"
    "  else {\n"
    "    if(res[1] > 0.) res[1] = 2. - res[1]; else res[1] = -2. - res[1];\n"
    "    }\n"
    "  }\n"
    "return res;\n"
    "}\n");

  add_if(shader, "sqrt_clamp", "mediump float sqrt_clamp(mediump float x) { return x >= 0. ? sqrt(x) : 0.; }\n");
  add_if(shader, "ellFaux", "mediump float ellFaux(mediump float cos_phi, mediump float sin_phi, mediump float k) {\n"
    "return sin_phi * ellRF(cos_phi * cos_phi, 1. - k * k * sin_phi * sin_phi, 1.);\n"
     "}\n");
  add_if(shader, "ellRF", "mediump float ellRF(mediump float x, mediump float y, mediump float z) {\n"
    "float delx = 1., dely = 1., delz = 1.;\n"
    "const float eps = 0.0025;\n"
    "float mean;\n"
    "while(abs(delx) > eps || abs(dely) > eps || abs(delz) > eps) {\n"
    "  float sx = sqrt(x);\n"
    "  float sy = sqrt(y);\n"
    "  float sz = sqrt(z);\n"
    "  float len = sx * (sy+sz) + sy * sz;\n"
    "  float x = .25 * (x+len);\n"
    "  float y = .25 * (y+len);\n"
    "  float z = .25 * (z+len);\n"
    "  mean = (x+y+z)/3.;\n"
    "  delx = (mean-x) / mean;\n"
    "  dely = (mean-y) / mean;\n"
    "  delz = (mean-z) / mean;\n"
    "  }\n"
    "float e2 = delx * dely - delz * delz;\n"
    "float e3 = delx * dely * delz;\n"
    "return ((1.0 + (e2 / 24.0 - 0.1 - 3.0 * e3 / 44.0) * e2+ e3 / 14.) / sqrt(mean));\n"
    "}\n");

  add_if(shader, "tanh", "mediump float tanh(mediump float x) { return sinh(x) / cosh(x); }\n");
  add_if(shader, "sinh", "mediump float sinh(mediump float x) { return (exp(x) - exp(-x)) / 2.0; }\n");
  add_if(shader, "asin_clamp", "mediump float asin_clamp(mediump float x) { return x > 1. ? PI/2. : x < -1. ? -PI/2. : asin(x); }\n");
  add_if(shader, "cosh", "mediump float cosh(mediump float x) { return (exp(x) + exp(-x)) / 2.0; }\n");
  add_if(shader, "asinh", "mediump float asinh(mediump float x) { return log(sqrt(x*x + 1.0) + x); }\n");
  add_if(shader, "acosh", "mediump float acosh(mediump float x) { return log(sqrt(x*x - 1.0) + x); }\n");
  add_if(shader, "atanh", "mediump float atanh(mediump float x) { return (log(1.+x)-log(1.-x))/2.; }\n");
  add_if(shader, "zlevel", "mediump float zlevel(mediump vec4 h) { return (h[2] < 0.0 ? -1.0 : 1.0) * sqrt(h[2]*h[2] - h[0]*h[0] - h[1]*h[1]); }\n");  
  add_if(shader, "atan2", "mediump float atan2(mediump float y, mediump float x) {\n"
    "if(x == 0.) return y > 0. ? PI/2. : -PI/2.;\n"
    "if(x > 0.) return atan(y / x);\n"
    "if(y >= 0.) return atan(y / x) + PI;\n" 
    "if(y < 0.) return atan(y / x) - PI;\n"
    "}\n");

  add_if(shader, "deparabolic13",
        "mediump vec4 deparabolic13(mediump vec4 h) {\n"
        "  h /= (1. + h[3]);\n"
        "  h[0] -= 1.;\n"
        "  h /= h.x*h.x + h.y*h.y + h.z * h.z;\n"
        "  h[0] += .5;\n"
        "  mediump vec4 res;\n"
        "  res.x = (log(2.) + log(-h.x));\n"
        "  res.y = h.y * 2.;\n"
        "  res.z = h.z * 2.;\n"
        "  res.w = 1.;\n"
        "  return res;\n"
        "  }\n\n");

  add_if(shader, "PI", "#define PI 3.14159265358979324\n");
  #ifndef GLES_ONLY
  add_if(shader, "mediump", "#define mediump\n");
  #endif
  }

EX flagtype get_shader_flags() { 
  if(!glhr::current_glprogram) return 0;
  return glhr::current_glprogram->shader_flags;
  }

EX void glapplymatrix(const transmatrix& V) {
  #if CAP_VR
  transmatrix V3;
  bool use_vr = vrhr::rendering();
  if(use_vr) V3 = vrhr::hmd_pre * V;
  const transmatrix& V2 = use_vr ? V3 : V;
  #else
  const transmatrix& V2 = V;
  #endif
  GLfloat mat[16];
  int id = 0;
  
  if(MXDIM == 3) {
    for(int y=0; y<3; y++) {
      for(int x=0; x<3; x++) mat[id++] = V2[x][y];
      mat[id++] = 0;
      }
    mat[12] = 0;
    mat[13] = 0;
    mat[14] = 0;
    mat[15] = 1;
    }
  else {
    for(int y=0; y<4; y++) 
      for(int x=0; x<4; x++) mat[id++] = V2[x][y];
    }
  glhr::set_modelview(glhr::as_glmatrix(mat));
  }

#endif
}
