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

namespace glhr {

bool glew   = false;

typedef const void *constvoidptr;

constvoidptr current_vertices, buffered_vertices;

GLuint buf_current, buf_buffered;

enum eMode { gmColored, gmTextured, gmVarColored, gmLightFog, gmMAX};

static const flagtype GF_TEXTURE  = 1;
static const flagtype GF_VARCOLOR = 2;
static const flagtype GF_LIGHTFOG = 4;

flagtype flags[gmMAX] = { 0, GF_TEXTURE, GF_VARCOLOR, GF_TEXTURE | GF_LIGHTFOG | GF_VARCOLOR 
  };

eMode mode;

shader_projection current_shader_projection, new_shader_projection;

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

glmatrix translate(ld x, ld y, ld z) {
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

#if !CAP_SHADER

void new_projection() {
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  }

void projection_multiply(const glmatrix& m) {
  glMultMatrixf(m.as_array());
  }

void set_modelview(const glmatrix& m) {
  glMatrixMode(GL_MODELVIEW);
  glLoadMatrixf(m.as_array());
  }

void id_modelview() {
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  }

#endif

// /* shaders */

#if CAP_SHADER

glmatrix projection;

void new_projection() {
  projection = id;
  }

void projection_multiply(const glmatrix& m) {
  projection = m * projection;  
  }

#define glMatrixMode DISABLED
#define glLoadIdentity DISABLED
#define glMultMatrixf DISABLED
#define glScalef DISABLED
#define glTranslatef DISABLED
#define glPushMatrix DISABLED
#define glPopMatrix DISABLED

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

struct GLprogram {
  GLuint _program;
  GLuint vertShader, fragShader;
  
  GLint uMVP, uFog, uColor, tTexture, uMV, uProjection;
  
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
    uColor = glGetUniformLocation(_program, "uColor");
    tTexture = glGetUniformLocation(_program, "tTexture");

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

void set_modelview(const glmatrix& modelview) {
  if(!current) return;
  if(current_shader_projection != shader_projection::standard) {
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
  if(!current) return;
  if(current_shader_projection != shader_projection::standard) { set_modelview(id); return; }
  #if MINIMIZE_GL_CALLS
  if(projection == current_matrix) return;
  current_matrix = projection;
  #endif
  glUniformMatrix4fv(current->uMVP, 1, 0, projection.as_array());
  }

#endif

void color2(color_t color, ld scale) {
  GLfloat cols[4];
  for(int i=0; i<4; i++)
    cols[i] = part(color, 3-i) / 255.0 * scale;
  #if CAP_SHADER
  // glUniform4fv(current->uFog, 4, cols);
  if(!current) return;
  glUniform4f(current->uColor, cols[0], cols[1], cols[2], cols[3]);
  #else
  glColor4f(cols[0], cols[1], cols[2], cols[3]);
  #endif
  }

void colorClear(color_t color) {
  glClearColor(part(color, 3) / 255.0, part(color, 2) / 255.0, part(color, 1) / 255.0, part(color, 0) / 255.0);
}

void be_nontextured(shader_projection sp) { switch_mode(gmColored, sp); }
void be_textured(shader_projection sp) { switch_mode(gmTextured, sp); }

void switch_mode(eMode m, shader_projection sp) {
  if(m == mode && current_shader_projection == sp) return;
  GLERR("pre_switch_mode");
  #if CAP_SHADER
  programs[m][int(sp)]->enable();
  GLERR("after_enable");
  #endif
  flagtype newflags = flags[m] &~ flags[mode];
  flagtype oldflags = flags[mode] &~ flags[m];
  if(newflags & GF_TEXTURE) {
    GLERR("xsm");
    #if CAP_SHADER
    glEnableVertexAttribArray(aTexture);
    GLERR("xsm");
    #else
    glEnable(GL_TEXTURE_2D);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    GLERR("xsm");
    #endif
    }
  if(oldflags & GF_TEXTURE) {
    GLERR("xsm");
    #if CAP_SHADER
    glDisableVertexAttribArray(aTexture);
    GLERR("xsm");
    #else
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisable(GL_TEXTURE_2D);
    GLERR("xsm");
    #endif
    }
  if(newflags & GF_VARCOLOR) {
    #if CAP_SHADER
    GLERR("xsm");
    glEnableVertexAttribArray(aColor);
    #else
    GLERR("xsm");
    glEnableClientState(GL_COLOR_ARRAY);
    GLERR("xsm");
    #endif
    }
  if(oldflags & GF_VARCOLOR) {
    #if CAP_SHADER
    glDisableVertexAttribArray(aColor);
    GLERR("xsm");
    #else
    glDisableClientState(GL_COLOR_ARRAY);
    GLERR("xsm");
    #endif
    }
  if(newflags & GF_LIGHTFOG) {
#if !CAP_SHADER    
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
#endif
    }
  if(oldflags & GF_LIGHTFOG) {
#if !CAP_SHADER
    glDisable(GL_FOG);

     /*
    glDisable(GL_LIGHTING); */
#endif
    }
  mode = m;
  current_shader_projection = sp;
  GLERR("after_switch_mode");
  current_vertices = NULL;
  current_matrix[0][0] = -1e8; // invalid
  current_modelview[0][0] = -1e8;
  current_projection[0][0] = -1e8;
  id_modelview();
  }

void fog_max(ld fogmax) {
  #if CAP_SHADER
  glUniform1f(current->uFog, 1 / fogmax);
  #else
  glFogf(GL_FOG_END, fogmax);
  #endif
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
      }
  #endif
  
  #if CAP_SHADER
  projection = id;
      
  for(int i=0; i<gmMAX; i++) 
  for(int j=0; j<int(shader_projection::MAX); j++) {
    flagtype f = flags[i];
    
    bool texture = f & GF_TEXTURE;
    bool lfog    = f & GF_LIGHTFOG;
    bool varcol  = f & GF_VARCOLOR;
    
    shader_projection sp = shader_projection(j);
    
    bool mps = j != 0;
    bool band = (sp == shader_projection::band);
    bool hp = (sp == shader_projection::halfplane);
    bool sh3 = (sp == shader_projection::standardH3);
    bool sr3 = (sp == shader_projection::standardR3);
    bool ss3 = (sp == shader_projection::standardS3);
    
    bool s3 = (sh3 || sr3 || ss3);
    
    programs[i][j] = new GLprogram(stringbuilder(

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
    
      1,       "float zlevel(vec4 h) {",
      1,       "  return (h[2] < 0.0 ? -1.0 : 1.0) * sqrt(h[2]*h[2] - h[0]*h[0] - h[1]*h[1]);",
      1,       "  }",

      1,       "void main() {",  
      texture,   "vTexCoord = aTexture;",
      varcol,    "vColor = aColor;",
      !varcol,   "vColor = uColor;",
      lfog,      "vColor = vColor * clamp(1.0 + aPosition.z * uFog, 0.0, 1.0);",
      !mps,      "gl_Position = uMVP * aPosition;",
      mps&&!band,"gl_Position = uP * (uMV * aPosition);",

      band||hp,  "vec4 t = uMV * aPosition;",  
      band||hp,  "float zlev = zlevel(t);",
      band||hp,  "t /= zlev;",

      band,      "float ty = asinh(t.y);",
      band,      "float tx = asinh(t.x / cosh(ty));",
      band,      "ty = 2.0 * atan(tanh(ty/2.0));",
      band,      "t[0] = tx; t[1] = ty; t[2] = 1.0; t[3] = 1.0;",
      
      hp,        "t.x /= t.z; t.y /= t.z; t.y = t.y + 1.0; ",
      hp,        "float rads = t.x * t.x + t.y * t.y; ",
      hp,        "t.x /= -rads; t.y /= -rads; t.z = 1.0; t[3] = 1.0;",
      
      s3,        "vec4 t = uMV * aPosition;",
      sh3,       "vColor.xyz = vColor.xyz * (1.0 - acosh(t[3]) / uFog);",
      sr3,       "vColor.xyz = vColor.xyz * (1.0 - sqrt(t[0]*t[0] + t[1]*t[1] + t[2]*t[2]) / 7.);",
      ss3,       "vColor.xyz = vColor.xyz * (1.0 - acos(t[3]) / 1.6);",
      sh3 || sr3,"t[3] = 1.0;",
      
      band || hp || s3,"gl_Position = uP * t;",
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
  programs[gmColored][0]->enable();
  #endif

  #if !CAP_SHADER
  switch_mode(gmColored, 0);
  #endif

  #if CAP_SHADER
  glEnableVertexAttribArray(aPosition);
  #else
  glEnableClientState(GL_VERTEX_ARRAY);
  #endif
  // #endif

  #if CAP_VERTEXBUFFER
  glGenBuffers(1, &buf_current);
  glGenBuffers(1, &buf_buffered);
  current_vertices = NULL;
  buffered_vertices = (void*) &buffered_vertices; // point to nothing
  glBindBuffer(GL_ARRAY_BUFFER, buf_current);
  #endif
  }

hyperpoint gltopoint(const glvertex& t) {
  hyperpoint h;
  h[0] = t[0]; h[1] = t[1]; h[2] = t[2]; 
  if(SHDIM == 4 && MAXDIM == 4) h[3] = t[3];
  return h;
  }

glvertex pointtogl(const hyperpoint& t) {
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

void vertices(const vector<glvertex>& v) {
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
  if(current_vertices == &v[0]) return;
  current_vertices = &v[0];
  #if CAP_SHADER
  glVertexAttribPointer(aPosition, SHDIM, GL_FLOAT, GL_FALSE, sizeof(glvertex), &v[0]);
  #else
  glVertexPointer(3, GL_FLOAT, sizeof(glvertex), &v[0]);
  #endif
  #endif
  }

void vertices_texture(const vector<glvertex>& v, const vector<glvertex>& t) {
  #if CAP_VERTEXBUFFER
  // not implemented!
  #else
  vertices(v);
  #if CAP_SHADER
  glVertexAttribPointer(aTexture, SHDIM, GL_FLOAT, GL_FALSE, sizeof(glvertex), &t[0]);
  #else
  glTexCoordPointer(SHDIM, GL_FLOAT, 0, &v[0]);
  #endif
  #endif
  }

void prepare(vector<colored_vertex>& v) {
  #if CAP_VERTEXBUFFER
  bindbuffer(v);
  PTR(glhr::aPosition, SHDIM, coords);
  PTR(glhr::aColor, 4, color);
  #else
  if(current_vertices == &v[0]) return;
  current_vertices = &v[0];
  #if CAP_SHADER
  glVertexAttribPointer(aPosition, SHDIM, GL_FLOAT, GL_FALSE, sizeof(colored_vertex), &v[0].coords);
  glVertexAttribPointer(aColor, 4, GL_FLOAT, GL_FALSE, sizeof(colored_vertex), &v[0].color);
  #else
  glVertexPointer(SHDIM, GL_FLOAT, sizeof(colored_vertex), &v[0].coords);
  glColorPointer(4, GL_FLOAT, sizeof(colored_vertex), &v[0].color);
  #endif
  #endif
  }

void prepare(vector<textured_vertex>& v) {
  #if CAP_VERTEXBUFFER
  bindbuffer(v);
  PTR(glhr::aPosition, SHDIM, coords);
  PTR(glhr::aTexture, 2, texture);
  #else
  if(current_vertices == &v[0]) return;
  current_vertices = &v[0];
  #if CAP_SHADER
  glVertexAttribPointer(aPosition, SHDIM, GL_FLOAT, GL_FALSE, sizeof(textured_vertex), &v[0].coords);
  glVertexAttribPointer(aTexture, SHDIM, GL_FLOAT, GL_FALSE, sizeof(textured_vertex), &v[0].texture);
  #else
  glVertexPointer(SHDIM, GL_FLOAT, sizeof(textured_vertex), &v[0].coords);
  glTexCoordPointer(2, GL_FLOAT, sizeof(textured_vertex), &v[0].texture);
  #endif
  #endif
  // color2(col);
  }

void prepare(vector<ct_vertex>& v) {
  #if CAP_VERTEXBUFFER
  bindbuffer(v);
  PTR(glhr::aPosition, SHDIM, coords);
  PTR(glhr::aColor, 4, color);
  PTR(glhr::aTexture, 2, texture);
  #else
  if(current_vertices == &v[0]) return;
  current_vertices = &v[0];
  #if CAP_SHADER
  glVertexAttribPointer(aPosition, SHDIM, GL_FLOAT, GL_FALSE, sizeof(ct_vertex), &v[0].coords);
  glVertexAttribPointer(aColor, 4, GL_FLOAT, GL_FALSE, sizeof(ct_vertex), &v[0].color);
  glVertexAttribPointer(aTexture, 2, GL_FLOAT, GL_FALSE, sizeof(ct_vertex), &v[0].texture);
  #else
  glVertexPointer(SHDIM, GL_FLOAT, sizeof(ct_vertex), &v[0].coords);
  glTexCoordPointer(2, GL_FLOAT, sizeof(ct_vertex), &v[0].texture);
  glColorPointer(4, GL_FLOAT, sizeof(ct_vertex), &v[0].color);
  #endif
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

bool current_depthtest;

void set_depthtest(bool b) {
  if(b != current_depthtest) {
    current_depthtest = b;
    if(b) glEnable(GL_DEPTH_TEST);
    else glDisable(GL_DEPTH_TEST);
    }
  }
}
}
