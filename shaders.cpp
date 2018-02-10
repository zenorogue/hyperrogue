// Hyperbolic Rogue -- basic GL transformations
// If CAP_SHADER is 0, OpenGL 1.0 is used.
// If CAP_SHADER is 1, GLSL is used.

// Copyright (C) 2011-2018 Zeno Rogue, see 'hyper.cpp' for details

void glError(const char* GLcall, const char* file, const int line) {
  GLenum errCode = glGetError();
  if(errCode!=GL_NO_ERROR) {
    fprintf(stderr, "OPENGL ERROR #%i: in file %s on line %i :: %s\n",errCode,file, line, GLcall);
    }
  }

namespace glhr {

bool glew   = false;

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
  
  // printf("===\ns%s\n===\n", s.c_str());
  
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

enum {
  UNIFORM_MODELVIEWPROJECTION_MATRIX,
  UNIFORM_FOGFACTOR,
  NUM_UNIFORMS
  };

struct GLprogram {
  GLuint _program;
  GLuint vertShader, fragShader;

  GLint uniforms[NUM_UNIFORMS];
  
  GLprogram(string vsh, string fsh) {
    _program = glCreateProgram();
    // printf("creating program %d\n", _program);
    vertShader = compileShader(GL_VERTEX_SHADER, vsh.c_str());
    fragShader = compileShader(GL_FRAGMENT_SHADER, fsh.c_str());
  
    // Attach vertex shader to program.
    glAttachShader(_program, vertShader);
    
    // Attach fragment shader to program.
    glAttachShader(_program, fragShader);
    
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
  
    uniforms[UNIFORM_MODELVIEWPROJECTION_MATRIX] = glGetUniformLocation(_program, "modelViewProjectionMatrix");
    uniforms[UNIFORM_FOGFACTOR] = glGetUniformLocation(_program, "fogfactor");    
    // printf("uniforms: %d %d\n", uniforms[UNIFORM_MODELVIEWPROJECTION_MATRIX], uniforms[UNIFORM_FOGFACTOR]);
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
  if(i) return s + stringbuilder(t...);
  else return stringbuilder(t...); 
  }

void set_modelview(const glmatrix& modelview) {
  glmatrix mvp = modelview * projection;
  glUniformMatrix4fv(current->uniforms[UNIFORM_MODELVIEWPROJECTION_MATRIX], 1, 0, mvp.as_array());
  // glmatrix nm = modelview;
  // glUniformMatrix3fv(current->uniforms[UNIFORM_NORMAL_MATRIX], 1, 0, nm[0]);
  }

#endif

GLfloat *currentvertices;

void vertices(GLfloat *f, int qty) {
  currentvertices = f;
  glVertexPointer(3, GL_FLOAT, 0, f);
  }

void texture_vertices(GLfloat *f, int qty, int stride = 2) {
  glTexCoordPointer(stride, GL_FLOAT, 0, f);
  }

void color_vertices(GLfloat *f, int qty) {
  glColorPointer(4, GL_FLOAT, 0, f);
  }

void color2(int color) {
  unsigned char *c = (unsigned char*) (&color);
  glColor4f(c[3] / 255.0, c[2] / 255.0, c[1]/255.0, c[0] / 255.0);
  }

void color2(int color, ld part) {
  unsigned char *c = (unsigned char*) (&color);
  glColor4f(c[3] / 255.0 * part, c[2] / 255.0 * part, c[1]/255.0 * part, c[0] / 255.0);
  }

void colorClear(int color) {
  unsigned char *c = (unsigned char*) (&color);
  glClearColor(c[3] / 255.0, c[2] / 255.0, c[1]/255.0, c[0] / 255.0);
  }

void be_nontextured() { switch_mode(gmColored); }
void be_textured() { switch_mode(gmTextured); }

void switch_mode(eMode m) {
  if(m == mode) return;
  #if CAP_SHADER
  programs[m]->enable();
  #endif
  flagtype newflags = flags[m] &~ flags[mode];
  flagtype oldflags = flags[mode] &~ flags[m];
  if(newflags & GF_TEXTURE) {
    glEnable(GL_TEXTURE_2D);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    }
  if(oldflags & GF_TEXTURE) {
    glDisable(GL_TEXTURE_2D);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }
  if(newflags & GF_VARCOLOR)
    glEnableClientState(GL_COLOR_ARRAY);
  if(oldflags & GF_VARCOLOR)
    glDisableClientState(GL_COLOR_ARRAY);
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
  }

void fog_max(ld fogmax) {
  #if CAP_SHADER
  glUniform1f(current->uniforms[UNIFORM_FOGFACTOR], 1 / fogmax);
  #else
  glFogf(GL_FOG_END, fogmax);
  #endif
  }

void init() {
  glEnableClientState(GL_VERTEX_ARRAY);

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
    bool lightfog = f & GF_LIGHTFOG;
    
    programs[i] = new GLprogram(stringbuilder(
      // "attribute vec4 position;"
      // "attribute vec3 normal;"
      
      1,       "varying vec4 vColor;",
      texture, "varying vec2 vTexCoord;", 
      
      1,       "uniform mat4 modelViewProjectionMatrix;",
      1,       "uniform float fogfactor;",
      
      1,       "void main() {",  
      texture,   "vTexCoord = gl_MultiTexCoord0.xy;",
      lightfog,  "vColor = gl_Color * clamp(1.0 + gl_Vertex.z * fogfactor, 0.0, 1.0);",
      !lightfog, "vColor = gl_Color;",
      1,         "gl_Position = modelViewProjectionMatrix * gl_Vertex;",
      1,         "}"), 
      
      stringbuilder(
  
      1,       "uniform sampler2D myTexture;",
      1,       "varying vec4 vColor;",
      texture, "varying vec2 vTexCoord;",
      1,       "void main() {",
      texture,   "gl_FragColor = vColor * texture2D(myTexture, vTexCoord);",
      !texture,  "gl_FragColor = vColor;",
      1,         "}"
      ));
    }
  
  glEnableClientState(GL_VERTEX_ARRAY);
  switch_mode(gmColored);
  programs[gmColored]->enable();
  #endif

  #if !CAP_SHADER
  switch_mode(gmColored);
  #endif
  }


}
