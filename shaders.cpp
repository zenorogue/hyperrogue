// Hyperbolic Rogue -- shaders
// Copyright (C) 2011-2019 Zeno Rogue, see 'hyper.cpp' for details

/** \file shaders.cpp
 *  \brief shaders
 */

#include "hyper.h"
namespace hr {

EX ld levellines;
EX bool disable_texture;

#if HDR
constexpr flagtype GF_TEXTURE  = 1;
constexpr flagtype GF_VARCOLOR = 2;
constexpr flagtype GF_LIGHTFOG = 4;
constexpr flagtype GF_LEVELS   = 8;

constexpr flagtype GF_which    = 15;

constexpr flagtype SF_PERS3        = 256;
constexpr flagtype SF_BAND         = 512;
constexpr flagtype SF_USE_ALPHA    = 1024;
constexpr flagtype SF_DIRECT       = 2048;
constexpr flagtype SF_PIXELS       = 4096;
constexpr flagtype SF_HALFPLANE    = 8192;
constexpr flagtype SF_ORIENT       = 16384;
constexpr flagtype SF_BOX          = 32768;
constexpr flagtype SF_ZFOG         = 65536;
#endif

#if HDR
/* standard attribute bindings */
/* taken from: https://www.opengl.org/sdk/docs/tutorials/ClockworkCoders/attributes.php */
constexpr int aPosition = 0;
constexpr int aColor = 3;
constexpr int aTexture = 8;

/* texture bindings */
constexpr int INVERSE_EXP_BINDING = 2;
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

shared_ptr<glhr::GLprogram> write_shader(flagtype shader_flags) {
  string varying, vsh, fsh, vmain = "void main() {\n", fmain = "void main() {\n";

  vsh += "attribute mediump vec4 aPosition;\n";
  varying += "varying mediump vec4 vColor;\n";

  fmain += "gl_FragColor = vColor;\n";
  if(shader_flags & GF_TEXTURE) {
    vsh += "attribute mediump vec2 aTexture;\n";
    varying += "varying mediump vec2 vTexCoord;\n";
    fsh += "uniform mediump sampler2D tTexture;\n";
    vmain += "vTexCoord = aTexture;\n",
    fmain += "gl_FragColor *= texture2D(tTexture, vTexCoord);\n";
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
  
  if(shader_flags & GF_LIGHTFOG) {
    vmain += "float fogx = clamp(1.0 + aPosition.z * uFog, 0.0, 1.0); vColor = vColor * fogx + uFogColor * (1.0-fogx);\n";      
    vsh += "uniform mediump float uFog;\n";
    vsh += "uniform mediump vec4 uFogColor;\n";
    }

  string coordinator;
  string distfun;
  bool treset = false;
  
  bool dim2 = GDIM == 2;
  bool dim3 = GDIM == 3;
  
  bool skip_t = false;
  
  if(pmodel == mdPixel) {
    vmain += "vec4 pos = aPosition; pos[3] = 1.0;\n";
    vmain += "pos = uMV * pos;\n";
    if(shader_flags & GF_LEVELS) vmain += "vPos = pos;\n";  
    vmain += "gl_Position = uP * pos;\n";
    skip_t = true;
    shader_flags |= SF_PIXELS | SF_DIRECT;
    }
  else if(pmodel == mdManual) {
    vmain += "vec4 pos = uMV * aPosition;\n";
    if(shader_flags & GF_LEVELS)
      vmain += "vPos = pos;\n";
    vmain += "gl_Position = uP * pos;\n";
    skip_t = true;
    shader_flags |= SF_DIRECT;
    }
  else if(!vid.consider_shader_projection) {
    shader_flags |= SF_PIXELS;
    }
  else if(pmodel == mdDisk && MDIM == 3 && !spherespecial && !prod) {
    shader_flags |= SF_DIRECT;
    }
  else if(glhr::noshaders) {
    shader_flags |= SF_PIXELS;
    }
  else if(pmodel == mdDisk && GDIM == 3 && !spherespecial && !nonisotropic && !prod) {
    coordinator += "t /= (t[3] + uAlpha);\n";
    vsh += "uniform mediump float uAlpha;";
    shader_flags |= SF_DIRECT | SF_BOX | SF_ZFOG;
    treset = true;
    }
  else if(pmodel == mdBand && hyperbolic) {
    shader_flags |= SF_BAND | SF_ORIENT | SF_BOX | SF_DIRECT;
    coordinator += "t = uPP * t;", vsh += "uniform mediump mat4 uPP;";
    if(dim2) coordinator += "float zlev = zlevel(t); t /= zlev;\n";
    if(dim3) coordinator += "float r = sqrt(t.y*t.y+t.z*t.z); float ty = asinh(r);\n";
    if(dim2) coordinator += "float ty = asinh(t.y);\n";
    coordinator += "float tx = asinh(t.x / cosh(ty)); ty = 2.0 * atan(tanh(ty/2.0));\n";
    if(dim2) coordinator += "t[0] = tx; t[1] = ty; t[2] = 1.0; t[3] = 1.0;\n";
    if(dim3) coordinator += "t[0] = tx; t[1] = ty*t.y/r; t[2] = ty*t.z/r; t[3] = 1.0;\n";
    if(dim3) shader_flags |= SF_ZFOG;
    }
  else if(pmodel == mdHalfplane && hyperbolic) {
    shader_flags |= SF_HALFPLANE | SF_ORIENT | SF_BOX | SF_DIRECT;
    if(dim2) shader_flags |= SF_USE_ALPHA;
    coordinator += "t = uPP * t;", vsh += "uniform mediump mat4 uPP;";
    if(dim2) coordinator += 
      "float zlev = zlevel(t); t /= zlev;\n"
      "t.xy /= t.z; t.y += 1.0;\n"
      "float rads = dot(t.xy, t.xy);\n"
      "t.xy /= -rads; t.z = 1.0; t[3] = 1.0;\n";
    if(dim3) coordinator += 
      "t.xyz /= (t.w + 1.0); t.y += 1.0;\n"
      "float rads = dot(t.xyz, t.xyz);\n"
      "t.xyz /= -rads; t[3] = 1.0;\n";        
    if(dim3) shader_flags |= SF_ZFOG;
    }
  else if(pmodel == mdGeodesic) {
    shader_flags |= SF_PERS3 | SF_DIRECT;
    coordinator += "t = inverse_exp(t);\n";
    if(sn::in()) {
      coordinator +=
        "float d = sqrt(t[0] * t[0] + t[1] * t[1] + t[2] * t[2]);\n"
        "float ad = (d == 0.) ? 0. : (d < 1.) ? min(atanh(d), 10.) : 10.;\n"
        "float m = ad / d; t[0] *= m; t[1] *= m; t[2] *= m;\n";
      distfun = "ad";
      }
    else
      distfun = "length(t.xyz)";
    switch(cgclass) {
      #if CAP_SOLV
      case gcSolNIH:
        switch(sn::geom()) {
          case gSol:
            vsh += sn::shader_symsol;
            break;
          case gNIH:
            vsh += sn::shader_nsym;
            break;
          case gSolN:
            vsh += sn::shader_nsymsol;
            break;
          default:
            println(hlog, "error: unknown sn geometry");
          }            
        treset = true;
        break;
      #endif
      case gcNil:
        vsh += nilv::nilshader;
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
      "float z = log(t[2] * t[2] - t[0] * t[0] - t[1] * t[1]) / 2.;\n"
      "float r = length(t.xy);\n"
      "float t2 = t[2] / exp(z);\n"
      "float d = t2 >= 1. ? acosh(t2) : 0.;\n"
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
    if(hyperbolic)
      distfun = "acosh(t[3])", treset = true;
    else if(euclid || nonisotropic)
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

  if(!skip_t) {
    vmain += "vec4 t = uMV * aPosition;\n";
    vmain += coordinator;
    if(distfun != "") {
      vmain += "float fogs = (uFogBase - " + distfun + " / uFog);\n";
      vmain += "vColor.xyz = vColor.xyz * fogs + uFogColor.xyz * (1.0-fogs);\n";
      vsh += 
        "uniform mediump float uFog;\n"
        "uniform mediump float uFogBase;\n"
        "uniform mediump vec4 uFogColor;\n";
      }
    if(shader_flags & GF_LEVELS) vmain += "vPos = t;\n";  
    if(treset) vmain += "t[3] = 1.0;\n";
    vmain += "gl_Position = uP * t;\n";
    }

  vsh += 
    "uniform mediump mat4 uMV;\n"
    "uniform mediump mat4 uP;\n";

  if(shader_flags & SF_ZFOG) {
    vmain += 
      "float pz = 0.5 + gl_Position.z / 2.0;\n"
      "vColor.xyz = vColor.xyz * (1.-pz) + uFogColor.xyz * pz;\n";
    vsh += 
        "uniform mediump vec4 uFogColor;\n";
    }
  
  vmain += "}";
  fmain += "}";
  
  fsh += varying;
  fsh += fmain;
  vsh += varying;
  vsh += vmain;
  
  if(glhr::noshaders) fsh = vsh = "";
  
  string both = fsh + "*" + vsh + "*" + its(shader_flags);
  if(compiled_programs.count(both)) 
    return compiled_programs[both];
  else {
    auto res = make_shared<glhr::GLprogram>(vsh, fsh);
    res->shader_flags = shader_flags;
    return res;
    }    
  }

void display_data::set_projection(int ed) {  
  flagtype shader_flags = current_display->next_shader_flags;
  unsigned id;
  id = geometry;
  id <<= 6; id |= pmodel;
  if(levellines && pmodel != mdPixel) {
    shader_flags |= GF_LEVELS;
    if(disable_texture) shader_flags &=~ GF_TEXTURE;
    }
  id <<= 6; id |= shader_flags;
  id <<= 6; id |= spherephase;
  id <<= 1; if(vid.consider_shader_projection) id |= 1;
  id <<= 2; id |= (spherespecial & 3);
  if(in_h2xe()) id |= 1;
  if(in_s2xe()) id |= 2;
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
  
  if(selected->uIterations != -1) {
    glhr::set_index_sl(0);
    glhr::set_sl_iterations(slr::steps);
    }

  glhr::new_projection();

  if(ed && vid.stereo_mode == sLR) {
    glhr::projection_multiply(glhr::translate(ed, 0, 0));
    glhr::projection_multiply(glhr::scale(2, 1, 1));
    }      

  ld tx = (cd->xcenter-cd->xtop)*2./cd->xsize - 1;
  ld ty = (cd->ycenter-cd->ytop)*2./cd->ysize - 1;
  glhr::projection_multiply(glhr::translate(tx, -ty, 0));

  if(pmodel == mdManual) return;
  
  if(vid.stretch != 1 && (shader_flags & SF_DIRECT)) glhr::projection_multiply(glhr::scale(vid.stretch, 1, 1));

  eyewidth_translate(ed);
  
  auto ortho = [&] (ld x, ld y) {
    glhr::glmatrix M = glhr::ortho(x, y, 1);
    if(shader_flags & SF_ZFOG) {
      using models::clip_max; 
      using models::clip_min;
      M[2][2] = 2 / (clip_max - clip_min);
      M[3][2] = (clip_min + clip_max) / (clip_max - clip_min);
      auto cols = glhr::acolor(darkena(backcolor, 0, 0xFF));
      glUniform4f(selected->uFogColor, cols[0], cols[1], cols[2], cols[3]);
      }
    else M[2][2] /= 1000;
    glhr::projection_multiply(M);
    if(nisot::local_perspective_used() && (shader_flags & SF_BOX))
      glhr::projection_multiply(glhr::tmtogl_transpose(NLP));
    if(ed) {
      glhr::glmatrix m = glhr::id;
      m[2][0] -= ed;
      glhr::projection_multiply(m);
      }
    glhr::id_modelview();
    };

  if(shader_flags & SF_PIXELS) ortho(cd->xsize/2, -cd->ysize/2);
  else if(shader_flags & SF_BOX) ortho(cd->xsize/current_display->radius/2, -cd->ysize/current_display->radius/2);
  else if(shader_flags & SF_PERS3) {
    glhr::projection_multiply(glhr::frustum(current_display->tanfov, current_display->tanfov * cd->ysize / cd->xsize));
    glhr::projection_multiply(glhr::scale(1, -1, -1));
    if(nisot::local_perspective_used()) {
      if(prod) {
        for(int i=0; i<3; i++) NLP[3][i] = NLP[i][3] = 0;
        NLP[3][3] = 1;
        }
      glhr::projection_multiply(glhr::tmtogl_transpose(NLP));
      }
    if(ed) {
      glhr::using_eyeshift = true;
      glhr::eyeshift = glhr::tmtogl(xpush(vid.ipd * ed/2));
      }
    glhr::fog_max(1/sightranges[geometry], darkena(backcolor, 0, 0xFF));
    }
  else {
    if(vid.alpha > -1) {
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
    glhr::projection_multiply(glhr::translate(0, 0, vid.alpha));
    if(ed) glhr::projection_multiply(glhr::translate(vid.ipd * ed/2, 0, 0));
    }

  if(selected->uPP != -1) {
    glhr::glmatrix pp = glhr::id;
    if(get_shader_flags() & SF_USE_ALPHA)
      pp[3][2] = GLfloat(vid.alpha);

    if(get_shader_flags() & SF_ORIENT) {
      if(GDIM == 3) for(int a=0; a<4; a++) 
        models::apply_orientation_yz(pp[a][1], pp[a][2]);
      for(int a=0; a<4; a++) 
        models::apply_orientation(pp[a][0], pp[a][1]);
      }
    
    glUniformMatrix4fv(selected->uPP, 1, 0, pp.as_array());
    }
  
  if(selected->uAlpha != -1)
    glhr::set_ualpha(vid.alpha);

  if(selected->uLevelLines != -1) {
    glUniform1f(selected->uLevelLines, levellines);
    }

  if(selected->shader_flags & SF_ORIENT)
    glhr::projection_multiply(model_orientation_gl());

  if(selected->shader_flags & SF_BAND)
    glhr::projection_multiply(glhr::scale(2 / M_PI, 2 / M_PI, GDIM == 3 ? 2/M_PI : 1));

  if(selected->shader_flags & SF_HALFPLANE) {
    glhr::projection_multiply(glhr::translate(0, 1, 0));      
    glhr::projection_multiply(glhr::scale(-1, 1, 1));
    glhr::projection_multiply(glhr::scale(models::halfplane_scale, models::halfplane_scale, GDIM == 3 ? models::halfplane_scale : 1));
    glhr::projection_multiply(glhr::translate(0, 0.5, 0));
    }      
  
  if(vid.camera_angle && pmodel != mdPixel) {
    ld cam = vid.camera_angle * degree;

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
  }

EX void add_if(string& shader, const string& seek, const string& function) {
  if(shader.find(seek) != string::npos)
    shader = function + shader;
  }

EX void add_fixed_functions(string& shader) {
  /* from the most complex to the simplest */

  add_if(shader, "tanh", "float tanh(float x) { return sinh(x) / cosh(x); }\n");
  add_if(shader, "sinh", "float sinh(float x) { return (exp(x) - exp(-x)) / 2.0; }\n");
  add_if(shader, "cosh", "float cosh(float x) { return (exp(x) + exp(-x)) / 2.0; }\n");
  add_if(shader, "asinh", "float asinh(float x) { return log(sqrt(x*x + 1.0) + x); }\n");
  add_if(shader, "acosh", "float acosh(float x) { return log(sqrt(x*x - 1.0) + x); }\n");
  add_if(shader, "atanh", "float atanh(float x) { return (log(1.+x)-log(1.-x))/2.; }\n");
  add_if(shader, "zlevel", "float zlevel(vec4 h) { return (h[2] < 0.0 ? -1.0 : 1.0) * sqrt(h[2]*h[2] - h[0]*h[0] - h[1]*h[1]); }\n");  
  add_if(shader, "atan2", "float atan2(float y, float x) {\n"
    "if(x == 0.) return y > 0. ? PI/2. : -PI/2.;\n"
    "if(x > 0.) return atan(y / x);\n"
    "if(y >= 0.) return atan(y / x) + PI;\n" 
    "if(y < 0.) return atan(y / x) - PI;\n"
    "}\n");

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
  GLfloat mat[16];
  int id = 0;
  
  if(MDIM == 3) {
    for(int y=0; y<3; y++) {
      for(int x=0; x<3; x++) mat[id++] = V[x][y];
      mat[id++] = 0;
      }
    mat[12] = 0;
    mat[13] = 0;
    mat[14] = 0;
    mat[15] = 1;
    }
  else {
    for(int y=0; y<4; y++) 
      for(int x=0; x<4; x++) mat[id++] = V[x][y];
    }
  glhr::set_modelview(glhr::as_glmatrix(mat));
  }


}
