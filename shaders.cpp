// Hyperbolic Rogue -- basic GL transformations
// If CAP_SHADER is 0, OpenGL 1.0 is used.
// If CAP_SHADER is 1, GLSL is used.

namespace hr {

#ifndef DEBUG_GL
#define DEBUG_GL 0
#endif

// Copyright (C) 2011-2018 Zeno Rogue, see 'hyper.cpp' for details

void glError(const char* GLcall, const char* file, const int line) {
  GLenum errCode = glGetError();
  if(errCode!=GL_NO_ERROR) {
    fprintf(stderr, "OPENGL ERROR #%i: in file %s on line %i :: %s\n",errCode,file, line, GLcall);
    }
  }

#ifndef CAP_VERTEXBUFFER
#define CAP_VERTEXBUFFER (ISWEB)
#endif

#if CAP_SHADER && CAP_NOSHADER
#define WITHSHADER(x, y) if(noshaders) y else x
#else
#if CAP_NOSHADER
#define WITHSHADER(x, y) if(1) y
#else
#define WITHSHADER(x, y) if(1) x
#endif
#endif

EX namespace glhr {

#if HDR
struct glmatrix {
  GLfloat a[4][4];
  GLfloat* operator[] (int i) { return a[i]; }
  const GLfloat* operator[] (int i) const { return a[i]; }
  GLfloat* as_array() { return a[0]; }
  const GLfloat* as_array() const { return a[0]; }
  };

glvertex pointtogl(const hyperpoint& t);

enum class shader_projection { standard, band, halfplane, standardH3, standardR3, 
  standardS30, standardS31, standardS32, standardS33, 
  ball, halfplane3, band3, flatten, standardSolv, standardNil,
  MAX 
  };

  inline glvertex makevertex(GLfloat x, GLfloat y, GLfloat z) {
    #if SHDIM == 3
    return make_array(x, y, z);
    #else
    return make_array<GLfloat>(x, y, z, 1);
    #endif
    }
  
  struct colored_vertex {
    glvertex coords;
    glvec4 color;
    colored_vertex(GLfloat x, GLfloat y, GLfloat r, GLfloat g, GLfloat b) {
      coords[0] = x;
      coords[1] = y;
      coords[2] = current_display->scrdist;
      coords[3] = 1;
      color[0] = r;
      color[1] = g;
      color[2] = b;
      color[3] = 1;
      }
    colored_vertex(hyperpoint h, color_t col) {
      coords = pointtogl(h);
      for(int i=0; i<4; i++)
        color[i] = part(col, 3-i) / 255.0;
      }
    };
  
  struct textured_vertex {
    glvertex coords;
    glvec2 texture;
    };
  
  struct ct_vertex {
    glvertex coords;
    glvec4 color;
    glvec2 texture;
    ct_vertex(const hyperpoint& h, ld x1, ld y1, ld col) {
      coords = pointtogl(h);
      texture[0] = x1;
      texture[1] = y1;
      color[0] = color[1] = color[2] = col;
      color[3] = 1;
      }
    };  

#endif

#if CAP_SHADER
bool noshaders = false;
#else
bool noshaders = true;
#endif

bool glew   = false;

bool current_depthtest, current_depthwrite;
ld fogbase;

typedef const void *constvoidptr;

constvoidptr current_vertices, buffered_vertices;
ld current_linewidth;

GLuint buf_current, buf_buffered;

enum eMode { gmColored, gmTextured, gmVarColored, gmLightFog, gmMAX};

static const flagtype GF_TEXTURE  = 1;
static const flagtype GF_VARCOLOR = 2;
static const flagtype GF_LIGHTFOG = 4;

flagtype flags[gmMAX] = { 0, GF_TEXTURE, GF_VARCOLOR, GF_TEXTURE | GF_LIGHTFOG | GF_VARCOLOR 
  };

eMode mode;

EX shader_projection current_shader_projection, new_shader_projection;

void switch_mode(eMode m, shader_projection sp);

void display(const glmatrix& m) {
  for(int i=0; i<4; i++) {
    for(int j=0; j<4; j++)
      printf("%10.5f", m[i][j]);
    printf("\n");
    }
  printf("\n");
  }

glmatrix operator * (glmatrix m1, glmatrix m2) {
  glmatrix res;
  for(int i=0; i<4; i++)
  for(int j=0; j<4; j++) {
    res[i][j] = 0;
    for(int k=0; k<4; k++)
      res[i][j] += m1[i][k] * m2[k][j];
    }
  return res;
  }

glmatrix id = {{{1,0,0,0}, {0,1,0,0}, {0,0,1,0}, {0,0,0,1}}};

glmatrix scale(ld x, ld y, ld z) {
  glmatrix tmp;
  for(int i=0; i<4; i++)
  for(int j=0; j<4; j++)
    tmp[i][j] = (i==j);
  tmp[0][0] = x;
  tmp[1][1] = y;
  tmp[2][2] = z;
  return tmp;
  }

EX glmatrix tmtogl(const transmatrix& T) {
  glmatrix tmp;
  for(int i=0; i<4; i++)
  for(int j=0; j<4; j++)
    tmp[i][j] = T[i][j];
  return tmp;
  }

glmatrix tmtogl_transpose(const transmatrix& T) {
  glmatrix tmp;
  for(int i=0; i<4; i++)
  for(int j=0; j<4; j++)
    tmp[i][j] = T[j][i];
  return tmp;
  }

glmatrix ortho(ld x, ld y, ld z) {
  return scale(1/x, 1/y, 1/z);
  }

glmatrix& as_glmatrix(GLfloat o[16]) {
  glmatrix& tmp = (glmatrix&) (o[0]);
  return tmp;
  }

glmatrix frustum(ld x, ld y, ld vnear = 1e-3, ld vfar = 1e9) {
  GLfloat frustum[16] = {
    GLfloat(1 / x), 0, 0, 0,
    0, GLfloat(1 / y), 0, 0,
    0, 0, GLfloat(-(vnear+vfar)/(vfar-vnear)), -1,
    0, 0, GLfloat(-2*vnear*vfar/(vfar-vnear)), 0};
  return as_glmatrix(frustum);
  }

EX glmatrix translate(ld x, ld y, ld z) {
  glmatrix tmp;
  for(int i=0; i<4; i++)
  for(int j=0; j<4; j++)
    tmp[i][j] = (i==j);
  tmp[3][0] = x;
  tmp[3][1] = y;
  tmp[3][2] = z;
  return tmp;
  }

// ** legacy **

// /* shaders */

glmatrix projection;

void new_projection() {
  WITHSHADER({
    projection = id;
    }, {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    return;
    })
  }

void projection_multiply(const glmatrix& m) {
  WITHSHADER({
    projection = m * projection;  
    }, {
    glMatrixMode(GL_PROJECTION);
    glMultMatrixf(m.as_array());
    })
  }

void init();

int compileShader(int type, const string& s) {
  GLint status;

#if DEBUG_GL  
  printf("===\n%s\n===\n", s.c_str());
#endif
  
  GLint shader = glCreateShader(type);
  const char *ss = s.c_str();
  glShaderSource(shader, 1, &ss, NULL);
  glCompileShader(shader);
  
  GLint logLength;
  glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
  if (logLength > 0) {
    std::vector<char> log(logLength);
    glGetShaderInfoLog(shader, logLength, &logLength, log.data());
    if(logLength > 0)
      printf("compiler log (%d): '%s'\n", logLength, log.data());
    }
  
  glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
  if (status == 0) {
    glDeleteShader(shader);
    printf("failed to compile shader\n");
    shader = 0;
    }
  
  return shader;
  }

// https://www.opengl.org/sdk/docs/tutorials/ClockworkCoders/attributes.php

struct GLprogram *current = NULL;

static const int aPosition = 0;
static const int aColor = 3;
static const int aTexture = 8;

const int INVERSE_EXP_BINDING = 2;

struct GLprogram {
  GLuint _program;
  GLuint vertShader, fragShader;
  
  GLint uMVP, uFog, uFogColor, uColor, tTexture, tInvExpTable, uMV, uProjection, uAlpha, uFogBase, uPRECX, uPRECY, uPRECZ;
  
  GLprogram(string vsh, string fsh) {
    _program = glCreateProgram();
    
    #ifndef GLES_ONLY
    while(vsh.find("mediump ") != string::npos)
      vsh.replace(vsh.find("mediump "), 7, "");
    while(fsh.find("mediump ") != string::npos)
      fsh.replace(fsh.find("mediump "), 7, "");
    #endif

    // printf("creating program %d\n", _program);
    vertShader = compileShader(GL_VERTEX_SHADER, vsh.c_str());
    fragShader = compileShader(GL_FRAGMENT_SHADER, fsh.c_str());
  
    // Attach vertex shader to program.
    glAttachShader(_program, vertShader);
    
    // Attach fragment shader to program.
    glAttachShader(_program, fragShader);
    
    glBindAttribLocation(_program, aPosition, "aPosition");
    glBindAttribLocation(_program, aTexture, "aTexture");
    glBindAttribLocation(_program, aColor, "aColor");

    GLint status;
    glLinkProgram(_program);
      
    GLint logLength;
    glGetProgramiv(_program, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0) {
      std::vector<char> log(logLength);
      glGetProgramInfoLog(_program, logLength, &logLength, log.data());
      if(logLength > 0)
        printf("linking log (%d): %s\n", logLength, log.data());
      }
     
    glGetProgramiv(_program, GL_LINK_STATUS, &status);
    if (status == 0) {
      printf("failed to link shader\n");
      exit(1);
      }
    
    // glBindAttribLocation(_program, GLKVertexAttribPosition, "position"); ??
    // glBindAttribLocation(_program, GLKVertexAttribNormal, "normal"); ??
  
    uMV = glGetUniformLocation(_program, "uMV");
    uProjection = glGetUniformLocation(_program, "uP");
    uMVP = glGetUniformLocation(_program, "uMVP");
    uFog = glGetUniformLocation(_program, "uFog");
    uFogColor = glGetUniformLocation(_program, "uFogColor");
    uFogBase = glGetUniformLocation(_program, "uFogBase");
    uAlpha = glGetUniformLocation(_program, "uAlpha");
    uColor = glGetUniformLocation(_program, "uColor");
    tTexture = glGetUniformLocation(_program, "tTexture");
    tInvExpTable = glGetUniformLocation(_program, "tInvExpTable");

    uPRECX = glGetUniformLocation(_program, "PRECX");
    uPRECY = glGetUniformLocation(_program, "PRECY");
    uPRECZ = glGetUniformLocation(_program, "PRECZ");
    
    #if DEBUG_GL
    printf("uniforms: %d %d %d %d\n", uMVP, uFog, uColor, tTexture);
    #endif
    // printf("attributes: %d\n", position_index);
    }
  
  ~GLprogram() {
    glDeleteProgram(_program);
    if(vertShader) glDeleteShader(vertShader), vertShader = 0;
    if(fragShader) glDeleteShader(fragShader), fragShader = 0;
    current = NULL;
    }
  
  void enable() {
    if(this != current) {
      glUseProgram(_program);    
      current = this;
      }
    }
  
  };

GLprogram *programs[gmMAX][int(shader_projection::MAX)];

string stringbuilder() { return ""; }

template<class... T> string stringbuilder(bool i, const string& s, T... t) { 
  if(i) return s + 
    #if DEBUG_GL
    "\n" + 
    #endif
    stringbuilder(t...);
  else return stringbuilder(t...); 
  }

glmatrix current_matrix, current_modelview, current_projection;

bool operator == (const glmatrix& m1, const glmatrix& m2) {
  for(int i=0; i<4; i++) 
    for(int j=0; j<4; j++)
      if(m1[i][j] != m2[i][j]) return false;
  return true;
  }

bool operator != (const glmatrix& m1, const glmatrix& m2) {
  return !(m1 == m2);
  }

bool uses_mvp(shader_projection sp) { return among(sp, shader_projection::standard, shader_projection::flatten); }

EX void set_modelview(const glmatrix& modelview) {
  #if CAP_NOSHADER
  if(noshaders) {
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(modelview.as_array());
    return;
    }
  #endif
  if(!current) return;
  if(!uses_mvp(current_shader_projection)) {
    if(modelview != current_modelview) {
      current_modelview = modelview;
      glUniformMatrix4fv(current->uMV, 1, 0, modelview.as_array());
      }
    if(projection != current_projection) {
      current_projection = projection;
      glUniformMatrix4fv(current->uProjection, 1, 0, projection.as_array());
      }
    }
  else {
    glmatrix mvp = modelview * projection;
    #if MINIMIZE_GL_CALLS
    if(mvp == current_matrix) return;
    current_matrix = mvp;
    #endif
    glUniformMatrix4fv(current->uMVP, 1, 0, mvp.as_array());
    }
  // glmatrix nm = modelview;
  // glUniformMatrix3fv(current->uniforms[UNIFORM_NORMAL_MATRIX], 1, 0, nm[0]);
  }

void id_modelview() {
  #if CAP_NOSHADER
  if(noshaders) {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    return;
    }
  #endif
  if(!current) return;
  if(!uses_mvp(current_shader_projection)) { set_modelview(id); return; }
  #if MINIMIZE_GL_CALLS
  if(projection == current_matrix) return;
  current_matrix = projection;
  #endif
  glUniformMatrix4fv(current->uMVP, 1, 0, projection.as_array());
  }

EX void color2(color_t color, ld scale IS(1)) {
  GLfloat cols[4];
  for(int i=0; i<4; i++)
    cols[i] = part(color, 3-i) / 255.0 * scale;
    
  WITHSHADER({
    if(!current) return;
    glUniform4f(current->uColor, cols[0], cols[1], cols[2], cols[3]);
    }, {
    glColor4f(cols[0], cols[1], cols[2], cols[3]);
    }
    )
  }

void colorClear(color_t color) {
  glClearColor(part(color, 3) / 255.0, part(color, 2) / 255.0, part(color, 1) / 255.0, part(color, 0) / 255.0);
}

EX void be_nontextured(shader_projection sp IS(new_shader_projection)) { switch_mode(gmColored, sp); }
EX void be_textured(shader_projection sp IS(new_shader_projection)) { switch_mode(gmTextured, sp); }
EX void use_projection(shader_projection sp IS(new_shader_projection)) { switch_mode(mode, sp); }

void switch_mode(eMode m, shader_projection sp) {
  if(m == mode && current_shader_projection == sp) return;
  reset_projection();
  GLERR("pre_switch_mode");
  WITHSHADER({
    programs[m][int(sp)]->enable();
    GLERR("after_enable");
    }, {})
  flagtype newflags = flags[m] &~ flags[mode];
  flagtype oldflags = flags[mode] &~ flags[m];
  if(newflags & GF_TEXTURE) {
    GLERR("xsm");
    WITHSHADER({
      glEnableVertexAttribArray(aTexture);
      GLERR("xsm");
      }, {
      glEnable(GL_TEXTURE_2D);
      glEnableClientState(GL_TEXTURE_COORD_ARRAY);
      GLERR("xsm");
      })
    }
  if(oldflags & GF_TEXTURE) {
    GLERR("xsm");
    WITHSHADER({
      glDisableVertexAttribArray(aTexture);
      GLERR("xsm");
      }, {
      glDisableClientState(GL_TEXTURE_COORD_ARRAY);
      glDisable(GL_TEXTURE_2D);
      GLERR("xsm");
      })
    }
  if(newflags & GF_VARCOLOR) {
    WITHSHADER({
      GLERR("xsm");
      glEnableVertexAttribArray(aColor);
      }, {
      GLERR("xsm");
      glEnableClientState(GL_COLOR_ARRAY);
      GLERR("xsm");
      })
    }
  if(oldflags & GF_VARCOLOR) {
    WITHSHADER({
      glDisableVertexAttribArray(aColor);
      GLERR("xsm");
      }, {
      glDisableClientState(GL_COLOR_ARRAY);
      GLERR("xsm");
      })
    }
  if(newflags & GF_LIGHTFOG) {
    WITHSHADER({}, {
    /*GLfloat light_ambient[] = { 3.5, 3.5, 3.5, 1.0 };
    GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_position[] = { 0.0, 0.0, 0.0, 1.0 };
  
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
  
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    GLERR("lighting");
  
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0); */
 
    glEnable(GL_FOG);
    #ifdef GLES_ONLY
    glFogx(GL_FOG_MODE, GL_LINEAR);
    #else
    glFogi(GL_FOG_MODE, GL_LINEAR);
    #endif
    glFogf(GL_FOG_START, 0);
    })
    }
  if(oldflags & GF_LIGHTFOG) {
    WITHSHADER({}, {glDisable(GL_FOG);})
    }
  WITHSHADER({
    glUniform1f(current->uFogBase, 1); fogbase = 1;
    }, {})
  mode = m;
  current_shader_projection = sp;
  GLERR("after_switch_mode");
  current_vertices = NULL;
  WITHSHADER({
    current_matrix[0][0] = -1e8; // invalid
    current_modelview[0][0] = -1e8;
    current_projection[0][0] = -1e8;
    }, {})
  id_modelview();
  current_linewidth = -1;
  /* if(current_depthwrite) glDepthMask(GL_TRUE);
  else glDepthMask(GL_FALSE);
  if(current_depthtest) glEnable(GL_DEPTH_TEST);
  else glDisable(GL_DEPTH_TEST); */
  }

void fog_max(ld fogmax, color_t fogcolor) {
  WITHSHADER({
    glUniform1f(current->uFog, 1 / fogmax);
  
    GLfloat cols[4];
    for(int i=0; i<4; i++) cols[i] = part(fogcolor, 3-i) / 255.0;
    glUniform4f(current->uFogColor, cols[0], cols[1], cols[2], cols[3]);
    }, {
    glFogf(GL_FOG_END, fogmax);
    })
  }

void set_fogbase(ld _fogbase) {
  WITHSHADER({
    if(fogbase != _fogbase) {
      fogbase = _fogbase;
      glUniform1f(current->uFogBase, fogbase);
      }
    }, {})
  }

void set_ualpha(ld alpha) {
  WITHSHADER({
    glUniform1f(current->uAlpha, alpha);
    }, {})
  }

void init() {

  #if CAP_GLEW
    if(!glew) { 
      glew = true; 
      printf("Initializing GLEW\n");
      GLenum err = glewInit();
      if (GLEW_OK != err) {
        addMessage("Failed to initialize GLEW");
        printf("Failed to initialize GLEW\n");
        return;
        }
      printf("CreateProgram = %p\n", __glewCreateProgram);
      if(!__glewCreateProgram) noshaders = true;
      }
  #endif
  
  #if CAP_SHADER
  projection = id;
  
  if(!noshaders)
  for(int i=0; i<gmMAX; i++) 
  for(int j=0; j<int(shader_projection::MAX); j++) {
    flagtype f = flags[i];
    
    bool texture = f & GF_TEXTURE;
    bool lfog    = f & GF_LIGHTFOG;
    bool varcol  = f & GF_VARCOLOR;
    
    shader_projection sp = shader_projection(j);
    
    bool mps = !uses_mvp(sp);
    bool band = among(sp, shader_projection::band, shader_projection::band3);
    bool hp = among(sp, shader_projection::halfplane, shader_projection::halfplane3);
    bool sh3 = (sp == shader_projection::standardH3);
    bool ssol = (sp == shader_projection::standardSolv);
    bool snil = (sp == shader_projection::standardNil);
    bool sr3 = (sp == shader_projection::standardR3);
    bool ss30 = (sp == shader_projection::standardS30);
    bool ss31 = (sp == shader_projection::standardS31);
    bool ss32 = (sp == shader_projection::standardS32);
    bool ss33 = (sp == shader_projection::standardS33);
    bool ss3 = ss30 || ss31 || ss32 || ss33;
    
    bool s3 = (sh3 || sr3 || ss3 || ssol || snil);
    bool dim3 = s3 || among(sp, shader_projection::ball, shader_projection::halfplane3, shader_projection::band3);
    bool dim2 = !dim3;
    bool ball = (sp == shader_projection::ball);
    bool flatten = (sp == shader_projection::flatten);
    
    programs[i][j] = new GLprogram(stringbuilder(
      1,       "#define PI 3.14159265358979324\n",

      1,       "attribute mediump vec4 aPosition;",
      texture, "attribute mediump vec2 aTexture;",
      varcol,  "attribute mediump vec4 aColor;",
      // "attribute vec3 normal;"
      
      1,       "varying mediump vec4 vColor;",
      texture, "varying mediump vec2 vTexCoord;", 
      
      !mps,    "uniform mediump mat4 uMVP;",
      mps,     "uniform mediump mat4 uMV;",
      mps,     "uniform mediump mat4 uP;",
      1,       "uniform mediump float uFog;",
      1,       "uniform mediump float uFogBase;",
      1,       "uniform mediump vec4 uFogColor;",
      ball,    "uniform mediump float uAlpha;",
      !varcol, "uniform mediump vec4 uColor;",

      1,       "float sinh(float x) {",
      1,       "  return (exp(x) - exp(-x)) / 2.0;",
      1,       "  }",

      1,       "float cosh(float x) {",
      1,       "  return (exp(x) + exp(-x)) / 2.0;",
      1,       "  }",
      
      1,       "float tanh(float x) {",
      1,       "  return sinh(x) / cosh(x);",
      1,       "  }",
      
      1,       "float asinh(float x) {",
      1,       "  return log(sqrt(x*x + 1.0) + x);",
      1,       "  }",
    
      1,       "float acosh(float x) {",
      1,       "  return log(sqrt(x*x - 1.0) + x);",
      1,       "  }",
      
      1,       "float atanh(float x) { return (log(1.+x)-log(1.-x))/2.; }",
    
      1,       "float zlevel(vec4 h) {",
      1,       "  return (h[2] < 0.0 ? -1.0 : 1.0) * sqrt(h[2]*h[2] - h[0]*h[0] - h[1]*h[1]);",
      1,       "  }",
      
      ssol,    solv::solshader,      
      snil,    nilv::nilshader,

      1,       "void main() {",  
      texture,   "vTexCoord = aTexture;",
      varcol,    "vColor = aColor;",
      !varcol,   "vColor = uColor;",
      lfog,      "float fogx = clamp(1.0 + aPosition.z * uFog, 0.0, 1.0);",
      lfog,      "vColor = vColor * fogx + uFogColor * (1.0-fogx);",
      !mps && !flatten,      "gl_Position = uMVP * aPosition;",
      !mps && flatten,   "vec4 pos = aPosition; pos[3] = 1.0; gl_Position = uMVP * pos;",
      ball,      "vec4 t = uMV * aPosition; t /= (t[3] + uAlpha); ",
      mps&&!(band||hp||s3||ball||flatten),"gl_Position = uP * (uMV * aPosition);",
      mps&&flatten, "vec4 pos = aPosition; pos[3] = 1.0; gl_Position = uP * (uMV * pos);",

      band||hp,  "vec4 t = uMV * aPosition;",  
      (band||hp) && dim2,  "float zlev = zlevel(t);",
      (band||hp) && dim2,  "t /= zlev;",

      band&&dim3,"float r = sqrt(t.y*t.y+t.z*t.z); float ty = asinh(r);",
      band&&dim2,"float ty = asinh(t.y);",
      band,      "float tx = asinh(t.x / cosh(ty));",
      band,      "ty = 2.0 * atan(tanh(ty/2.0));",
      band&&dim2,"t[0] = tx; t[1] = ty; t[2] = 1.0; t[3] = 1.0;",
      band&&dim3,"t[0] = tx; t[1] = ty*t.y/r; t[2] = ty*t.z/r; t[3] = 1.0;",
      
      
      hp && dim2, "t.x /= t.z; t.y /= t.z; t.y = t.y + 1.0; ",
      hp && dim2, "float rads = t.x * t.x + t.y * t.y; ",
      hp && dim2, "t.x /= -rads; t.y /= -rads; t.z = 1.0; t[3] = 1.0;",

      hp && dim3, "t.x /= (1.0+t.w); t.y /= (1.0+t.w); t.z /= (1.0+t.w); t.y = t.y + 1.0; ",
      hp && dim3, "float rads = t.x * t.x + t.y * t.y + t.z * t.z; ",
      hp && dim3, "t.x /= -rads; t.y /= -rads; t.z /= -rads; t[3] = 1.0;",
      
      s3,        "vec4 t = uMV * aPosition;",
      ssol,      "t = inverse_exp(t);",
      ssol,      "float d = sqrt(t[0] * t[0] + t[1] * t[1] + t[2] * t[2]);",
      ssol,      "float ad = (d == 0.) ? 0. : (d < 1.) ? min(atanh(d), 10.) : 10.; ",
      ssol,      "float m = ad / d / 11.; t[0] *= m; t[1] *= m; t[2] *= m; ",
      snil,      "t = inverse_exp(t);",
       
      sh3,       "float fogs = (uFogBase - acosh(t[3]) / uFog);",
      sr3||snil, "float fogs = (uFogBase - sqrt(t[0]*t[0] + t[1]*t[1] + t[2]*t[2]) / uFog);",
      ssol,      "float fogs = (uFogBase - ad / uFog);",
      
      ss30,      "float fogs = (uFogBase - (6.284 - acos(t[3])) / uFog); t = -t; ",
      ss31,      "float fogs = (uFogBase - (6.284 - acos(t[3])) / uFog); t.xyz = -t.xyz; ",
      ss32,      "float fogs = (uFogBase - acos(t[3]) / uFog); t.w = -t.w; ", // 2pi
      ss33,      "float fogs = (uFogBase - acos(t[3]) / uFog); ",

      s3,        "vColor.xyz = vColor.xyz * fogs + uFogColor.xyz * (1.0-fogs);",

      sh3 || sr3 || ssol || ball,"t[3] = 1.0;",
      
      band || hp || s3 || ball,"gl_Position = uP * t;",
      dim3 && !s3, "vColor.xyz = vColor.xyz * (0.5 - gl_Position.z / 2.0) + uFogColor.xyz * (0.5 + gl_Position.z / 2.0);",
      1,         "}"), 
      
      stringbuilder(
  
      1,       "uniform mediump sampler2D tTexture;",
      1,       "varying mediump vec4 vColor;",
      texture, "varying mediump vec2 vTexCoord;",
      1,       "void main() {",
      texture,   "gl_FragColor = vColor * texture2D(tTexture, vTexCoord);",
      !texture,  "gl_FragColor = vColor;",
      1,         "}"
      ));
    }
  
  switch_mode(gmColored, shader_projection::standard);
  if(!noshaders) programs[gmColored][0]->enable();
  #endif

  #if !CAP_SHADER
  switch_mode(gmColored, shader_projection::standard);
  #endif

  WITHSHADER(glEnableVertexAttribArray(aPosition);, glEnableClientState(GL_VERTEX_ARRAY);)
  // #endif

  #if CAP_VERTEXBUFFER
  glGenBuffers(1, &buf_current);
  glGenBuffers(1, &buf_buffered);
  current_vertices = NULL;
  buffered_vertices = (void*) &buffered_vertices; // point to nothing
  glBindBuffer(GL_ARRAY_BUFFER, buf_current);
  #endif
  }

EX hyperpoint gltopoint(const glvertex& t) {
  hyperpoint h;
  h[0] = t[0]; h[1] = t[1]; h[2] = t[2]; 
  if(SHDIM == 4 && MAXMDIM == 4) h[3] = t[3];
  return h;
  }

EX glvertex pointtogl(const hyperpoint& t) {
  glvertex h;
  h[0] = t[0]; h[1] = t[1]; h[2] = t[2]; 
  if(SHDIM == 4) h[3] = (DIM == 3) ? t[3] : 1;
  return h;
  }

#if CAP_VERTEXBUFFER
template<class T> void bindbuffer(T& v) {
  if(current_vertices == buffered_vertices || current_vertices == nullptr) {
    glBindBuffer(GL_ARRAY_BUFFER, buf_current);
    }
  current_vertices = &v[0];
  glBufferData(GL_ARRAY_BUFFER, isize(v) * sizeof(v[0]), &v[0], GL_DYNAMIC_DRAW);    
  }

#define PTR(attrib, q, field) \
  glVertexAttribPointer(attrib, q, GL_FLOAT, GL_FALSE, sizeof(v[0]), (void*) ((char*) &v[0].field - (char*) &v[0]));

#endif

void vertices(const vector<glvertex>& v, int vshift = 0) {
  #if CAP_VERTEXBUFFER
  if(&v[0] == buffered_vertices) {
    if(&v[0] == current_vertices) return;
    current_vertices = buffered_vertices;
    glBindBuffer(GL_ARRAY_BUFFER, buf_buffered);
    glVertexAttribPointer(glhr::aPosition, SHDIM, GL_FLOAT, GL_FALSE, sizeof(glvertex), 0);
    return;
    }
  bindbuffer(v);
  glVertexAttribPointer(glhr::aPosition, SHDIM, GL_FLOAT, GL_FALSE, sizeof(glvertex), 0);
  #else
  if(current_vertices == &v[vshift]) return;
  current_vertices = &v[vshift];
  WITHSHADER(
    glVertexAttribPointer(aPosition, SHDIM, GL_FLOAT, GL_FALSE, sizeof(glvertex), &v[vshift]);,
    glVertexPointer(SHDIM, GL_FLOAT, sizeof(glvertex), &v[0]);
    )
  #endif
  }

void vertices_texture(const vector<glvertex>& v, const vector<glvertex>& t, int vshift = 0, int tshift = 0) {
  #if CAP_VERTEXBUFFER
  int q = min(isize(v)-vshift, isize(t)-tshift);
  vector<textured_vertex> tv(q);
  for(int i=0; i<q; i++)
    tv[i].coords = v[vshift+i],
    tv[i].texture[0] = t[tshift+i][0],
    tv[i].texture[1] = t[tshift+i][1];
  prepare(tv);
  #else
  vertices(v, vshift);
  WITHSHADER(
    glVertexAttribPointer(aTexture, SHDIM, GL_FLOAT, GL_FALSE, sizeof(glvertex), &t[tshift]);,
    glTexCoordPointer(SHDIM, GL_FLOAT, 0, &t[tshift]);
    )
  #endif
  }

EX void prepare(vector<colored_vertex>& v) {
  #if CAP_VERTEXBUFFER
  bindbuffer(v);
  PTR(glhr::aPosition, SHDIM, coords);
  PTR(glhr::aColor, 4, color);
  #else
  if(current_vertices == &v[0]) return;
  current_vertices = &v[0];
  WITHSHADER({
    glVertexAttribPointer(aPosition, SHDIM, GL_FLOAT, GL_FALSE, sizeof(colored_vertex), &v[0].coords);
    glVertexAttribPointer(aColor, 4, GL_FLOAT, GL_FALSE, sizeof(colored_vertex), &v[0].color);
    }, {
    glVertexPointer(SHDIM, GL_FLOAT, sizeof(colored_vertex), &v[0].coords);
    glColorPointer(4, GL_FLOAT, sizeof(colored_vertex), &v[0].color);
    })
  #endif
  }

EX void prepare(vector<textured_vertex>& v) {
  #if CAP_VERTEXBUFFER
  bindbuffer(v);
  PTR(glhr::aPosition, SHDIM, coords);
  PTR(glhr::aTexture, 2, texture);
  #else
  if(current_vertices == &v[0]) return;
  current_vertices = &v[0];
  WITHSHADER({
    glVertexAttribPointer(aPosition, SHDIM, GL_FLOAT, GL_FALSE, sizeof(textured_vertex), &v[0].coords);
    glVertexAttribPointer(aTexture, SHDIM, GL_FLOAT, GL_FALSE, sizeof(textured_vertex), &v[0].texture);
    }, {    
    glVertexPointer(SHDIM, GL_FLOAT, sizeof(textured_vertex), &v[0].coords);
    glTexCoordPointer(2, GL_FLOAT, sizeof(textured_vertex), &v[0].texture);
    })
  #endif
  // color2(col);
  }

EX void prepare(vector<ct_vertex>& v) {
  #if CAP_VERTEXBUFFER
  bindbuffer(v);
  PTR(glhr::aPosition, SHDIM, coords);
  PTR(glhr::aColor, 4, color);
  PTR(glhr::aTexture, 2, texture);
  #else
  if(current_vertices == &v[0]) return;
  current_vertices = &v[0];
  WITHSHADER({
    glVertexAttribPointer(aPosition, SHDIM, GL_FLOAT, GL_FALSE, sizeof(ct_vertex), &v[0].coords);
    glVertexAttribPointer(aColor, 4, GL_FLOAT, GL_FALSE, sizeof(ct_vertex), &v[0].color);
    glVertexAttribPointer(aTexture, 2, GL_FLOAT, GL_FALSE, sizeof(ct_vertex), &v[0].texture);
    }, {
    glVertexPointer(SHDIM, GL_FLOAT, sizeof(ct_vertex), &v[0].coords);
    glTexCoordPointer(2, GL_FLOAT, sizeof(ct_vertex), &v[0].texture);
    glColorPointer(4, GL_FLOAT, sizeof(ct_vertex), &v[0].color);
    })
  #endif
  }

void store_in_buffer(vector<glvertex>& v) {
#if CAP_VERTEXBUFFER
  if(!buf_buffered) {
    printf("no buffer yet\n");
    return;
    }
  printf("storing %d in buffer: %p\n", isize(v), &v[0]);
  current_vertices = buffered_vertices = &v[0];
  glBindBuffer(GL_ARRAY_BUFFER, buf_buffered);
  glVertexAttribPointer(glhr::aPosition, SHDIM, GL_FLOAT, GL_FALSE, sizeof(glvertex), 0);
  glBufferData(GL_ARRAY_BUFFER, isize(v) * sizeof(glvertex), &v[0], GL_STATIC_DRAW);
  printf("Stored.\n");
#endif
  }

EX void set_depthtest(bool b) {
  if(b != current_depthtest) {
    current_depthtest = b;
    if(b) glEnable(GL_DEPTH_TEST);
    else glDisable(GL_DEPTH_TEST);
    }
  }


EX void set_depthwrite(bool b) {
  if(b != current_depthwrite) { // <- this does not work ask intended for some reason...
    current_depthwrite = b;
    if(b) glDepthMask(GL_TRUE);
    else glDepthMask(GL_FALSE);
    }
  }

EX void set_linewidth(ld lw) {
  if(lw != current_linewidth) {
    current_linewidth = lw;
    glLineWidth(lw);
    }
  }

EX void switch_to_text(const vector<glvertex>& v, const vector<glvertex>& t) {
  glhr::be_textured();
  dynamicval<eModel> pm(pmodel, mdUnchanged);
  if(!svg::in) current_display->set_all(0);
  vertices_texture(v, t, 0, 0);
  }

}
}

#define glMatrixMode DISABLED
#define glLoadIdentity DISABLED
#define glMultMatrixf DISABLED
#define glScalef DISABLED
#define glTranslatef DISABLED
#define glPushMatrix DISABLED
#define glPopMatrix DISABLED

