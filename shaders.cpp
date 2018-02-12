// Hyperbolic Rogue -- basic GL transformations
// If CAP_SHADER is 0, OpenGL 1.0 is used.
// If CAP_SHADER is 1, GLSL is used.

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

flagtype flags[gmMAX] = { 0, GF_TEXTURE, GF_VARCOLOR, GF_TEXTURE | GF_LIGHTFOG 
#if CAP_SHADER
  | GF_VARCOLOR 
#endif
  };

eMode mode;

void switch_mode(eMode m);

struct glmatrix {
  GLfloat a[4][4];
  GLfloat* operator[] (int i) { return a[i]; }
  const GLfloat* operator[] (int i) const { return a[i]; }
  GLfloat* as_array() { return a[0]; }
  const GLfloat* as_array() const { return a[0]; }
  };

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

glmatrix id() {
  glmatrix M;
  for(int i=0; i<4; i++)
  for(int j=0; j<4; j++)
    M[i][j] = (i == j);
  return M;
  }

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

#endif

// /* shaders */

#if CAP_SHADER

glmatrix projection;

void new_projection() {
  projection = id();
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
    char log[logLength];
    glGetShaderInfoLog(shader, logLength, &logLength, log);
    if(logLength > 0)
      printf("compiler log (%d): '%s'\n", logLength, log);
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
  
  GLint uMVP, uFog, uColor, tTexture;
  
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
      char log[logLength];
      glGetProgramInfoLog(_program, logLength, &logLength, log);
      if(logLength > 0)
        printf("linking log (%d): %s\n", logLength, log);
      }
     
    glGetProgramiv(_program, GL_LINK_STATUS, &status);
    if (status == 0) {
      printf("failed to link shader\n");
      exit(1);
      }
    
    // glBindAttribLocation(_program, GLKVertexAttribPosition, "position"); ??
    // glBindAttribLocation(_program, GLKVertexAttribNormal, "normal"); ??
  
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

GLprogram *programs[gmMAX];

string stringbuilder() { return ""; }

template<class... T> string stringbuilder(bool i, const string& s, T... t) { 
  if(i) return s + 
    #if DEBUG_GL
    "\n" + 
    #endif
    stringbuilder(t...);
  else return stringbuilder(t...); 
  }

void set_modelview(const glmatrix& modelview) {
  glmatrix mvp = modelview * projection;
  glUniformMatrix4fv(current->uMVP, 1, 0, mvp.as_array());
  // glmatrix nm = modelview;
  // glUniformMatrix3fv(current->uniforms[UNIFORM_NORMAL_MATRIX], 1, 0, nm[0]);
  }

#endif

void color2(int color, ld part = 1) {
  unsigned char *c = (unsigned char*) (&color);
  GLfloat cols[4];
  for(int i=0; i<4; i++) cols[i] = c[3-i] / 255.0 * part;
  #if CAP_SHADER
  // glUniform4fv(current->uFog, 4, cols);
  glUniform4f(current->uColor, cols[0], cols[1], cols[2], cols[3]);
  #else
  glColor4f(cols[0], cols[1], cols[2], cols[3]);
  #endif
  }

void colorClear(int color) {
  unsigned char *c = (unsigned char*) (&color);
  glClearColor(c[3] / 255.0, c[2] / 255.0, c[1]/255.0, c[0] / 255.0);
  }

void be_nontextured() { switch_mode(gmColored); }
void be_textured() { switch_mode(gmTextured); }

void switch_mode(eMode m) {
  if(m == mode) return;
  GLERR("pre_switch_mode");
  #if CAP_SHADER
  programs[m]->enable();
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
    GLfloat light_ambient[] = { 3.5, 3.5, 3.5, 1.0 };
    GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_position[] = { 0.0, 0.0, 0.0, 1.0 };
  
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
  
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    GLERR("lighting");
  
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    glEnable(GL_FOG);
    glFogi(GL_FOG_MODE, GL_LINEAR);
    glFogf(GL_FOG_START, 0);
#endif
    }
  if(oldflags & GF_LIGHTFOG) {
#if !CAP_SHADER
    glDisable(GL_FOG);
    glDisable(GL_LIGHTING);
#endif
    }
  mode = m;
  GLERR("after_switch_mode");
  current_vertices = NULL;
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
  projection = id();
      
  for(int i=0; i<4; i++) {
    flagtype f = flags[i];
    
    bool texture = f & GF_TEXTURE;
    bool lfog    = f & GF_LIGHTFOG;
    bool varcol  = f & GF_VARCOLOR;
    
    programs[i] = new GLprogram(stringbuilder(

      1,       "attribute mediump vec4 aPosition;",
      texture, "attribute mediump vec2 aTexture;",
      varcol,  "attribute mediump vec4 aColor;",
      // "attribute vec3 normal;"
      
      1,       "varying mediump vec4 vColor;",
      texture, "varying mediump vec2 vTexCoord;", 
      
      1,       "uniform mediump mat4 uMVP;",
      1,       "uniform mediump float uFog;",
      !varcol, "uniform mediump vec4 uColor;",
      
      1,       "void main() {",  
      texture,   "vTexCoord = aTexture;",
      varcol,    "vColor = aColor;",
      !varcol,   "vColor = uColor;",
      lfog,      "vColor = vColor * clamp(1.0 + aPosition.z * uFog, 0.0, 1.0);",
      1,         "gl_Position = uMVP * aPosition;",
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
  
  switch_mode(gmColored);
  programs[gmColored]->enable();
  #endif

  #if !CAP_SHADER
  switch_mode(gmColored);
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
  return h;
  }

glvertex pointtogl(const hyperpoint& t) {
  glvertex h;
  h[0] = t[0]; h[1] = t[1]; h[2] = t[2];
  return h;
  }

struct colored_vertex {
  glvec3 coords;
  glvec4 color;
  colored_vertex(GLfloat x, GLfloat y, GLfloat r, GLfloat g, GLfloat b) {
    coords[0] = x;
    coords[1] = y;
    coords[2] = stereo::scrdist;
    color[0] = r;
    color[1] = g;
    color[2] = b;
    color[3] = 1;
    }
  };

struct textured_vertex {
  glvec3 coords;
  glvec3 texture;
  };

struct ct_vertex {
  glvec3 coords;
  glvec4 color;
  glvec3 texture;
  ct_vertex(const hyperpoint& h, ld x1, ld y1, ld col) {
    coords = pointtogl(h);
    texture[0] = x1;
    texture[1] = y1;
    color[0] = color[1] = color[2] = col;
    color[3] = 1;
    }
  };

#if CAP_VERTEXBUFFER
template<class T> void bindbuffer(T& v) {
  if(current_vertices == buffered_vertices) {
    glBindBuffer(GL_ARRAY_BUFFER, buf_current);
    }
  current_vertices = &v[0];
  glBufferData(GL_ARRAY_BUFFER, size(v) * sizeof(v[0]), &v[0], GL_DYNAMIC_DRAW);    
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
    glBufferData(GL_ARRAY_BUFFER, size(v) * sizeof(v[0]), &v[0], GL_STATIC_DRAW);
    glVertexAttribPointer(glhr::aPosition, 3, GL_FLOAT, GL_FALSE, sizeof(glvertex), 0);
    return;
    }
  bindbuffer(v);
  glVertexAttribPointer(glhr::aPosition, 3, GL_FLOAT, GL_FALSE, sizeof(glvertex), 0);
  #else
  if(current_vertices == &v[0]) return;
  current_vertices = &v[0];
  #if CAP_SHADER
  glVertexAttribPointer(aPosition, 3, GL_FLOAT, GL_FALSE, sizeof(glvertex), &v[0]);
  #else
  glVertexPointer(3, GL_FLOAT, sizeof(glvertex), &v[0]);
  #endif
  #endif
  }

void vertices_texture(const vector<glvertex>& v, const vector<glvertex>& t) {
  #if CAP_TEXTURE
  vertices(v);
  #if CAP_SHADER
  glVertexAttribPointer(aTexture, 3, GL_FLOAT, GL_FALSE, sizeof(glvertex), &t[0]);
  #else
  glTexCoordPointer(stride, GL_FLOAT, 0, &v[0]);
  #endif
  #endif
  }

void prepare(vector<colored_vertex>& v) {
  #if CAP_VERTEXBUFFER
  bindbuffer(v);
  PTR(glhr::aPosition, 3, coords);
  PTR(glhr::aColor, 4, color);
  #else
  if(current_vertices == &v[0]) return;
  current_vertices = &v[0];
  #if CAP_SHADER
  glVertexAttribPointer(aPosition, 3, GL_FLOAT, GL_FALSE, sizeof(colored_vertex), &v[0].coords);
  glVertexAttribPointer(aColor, 4, GL_FLOAT, GL_FALSE, sizeof(colored_vertex), &v[0].color);
  #else
  glVertexPointer(3, GL_FLOAT, sizeof(colored_vertex), &v[0].coords);
  glVertexPointer(3, GL_FLOAT, sizeof(colored_vertex), &v[0].color);
  #endif
  #endif
  }

void prepare(vector<textured_vertex>& v) {
  #if CAP_VERTEXBUFFER
  bindbuffer(v);
  PTR(glhr::aPosition, 3, coords);
  PTR(glhr::aTexture, 2, texture);
  #else
  if(current_vertices == &v[0]) return;
  current_vertices = &v[0];
  #if CAP_SHADER
  glVertexAttribPointer(aPosition, 3, GL_FLOAT, GL_FALSE, sizeof(textured_vertex), &v[0].coords);
  glVertexAttribPointer(aTexture, 3, GL_FLOAT, GL_FALSE, sizeof(textured_vertex), &v[0].texture);
  #else
  glVertexPointer(3, GL_FLOAT, sizeof(textured_vertex), &v[0].coords);
  glVertexPointer(3, GL_FLOAT, sizeof(textured_vertex), &v[0].texture);
  #endif
  #endif
  // color2(col);
  }

void prepare(vector<ct_vertex>& v) {
  #if CAP_VERTEXBUFFER
  bindbuffer(v);
  PTR(glhr::aPosition, 3, coords);
  PTR(glhr::aColor, 4, color);
  PTR(glhr::aTexture, 2, texture);
  #else
  if(current_vertices == &v[0]) return;
  current_vertices = &v[0];
  #if CAP_SHADER
  glVertexAttribPointer(aPosition, 3, GL_FLOAT, GL_FALSE, sizeof(ct_vertex), &v[0].coords);
  glVertexAttribPointer(aColor, 4, GL_FLOAT, GL_FALSE, sizeof(ct_vertex), &v[0].color);
  glVertexAttribPointer(aTexture, 3, GL_FLOAT, GL_FALSE, sizeof(ct_vertex), &v[0].texture);
  #else
  glVertexPointer(3, GL_FLOAT, sizeof(ct_vertex), &v[0].coords);
  glVertexPointer(3, GL_FLOAT, sizeof(ct_vertex), &v[0].texture);
  glVertexPointer(3, GL_FLOAT, sizeof(ct_vertex), &v[0].color);
  #endif
  #endif
  }

void store_in_buffer(vector<glvertex>& v) {
#if CAP_VERTEXBUFFER
  if(!buf_buffered) {
    printf("no buffer yet\n");
    return;
    }
  printf("storing %d in buffer: %p\n", size(v), &v[0]);
  current_vertices = buffered_vertices = &v[0];
  glBindBuffer(GL_ARRAY_BUFFER, buf_buffered);
  glVertexAttribPointer(glhr::aPosition, 3, GL_FLOAT, GL_FALSE, sizeof(glvertex), 0);
  glBufferData(GL_ARRAY_BUFFER, size(v) * sizeof(glvertex), &v[0], GL_STATIC_DRAW);
  printf("Stored.\n");
#endif
  }

}
