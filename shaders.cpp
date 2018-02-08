// gl matrices
//-------------

// #undef CAP_SHADER
// #define CAP_SHADER 0

namespace glhr {

enum mode { gmColored, gmTextured, gmMAX};

struct glmatrix {
  GLfloat a[4][4];
  GLfloat* operator[] (int i) { return a[i]; }
  GLfloat* as_array() { return a[0]; }
  const GLfloat* as_array() const { return a[0]; }
  };

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

glmatrix& as_glmatrix(GLfloat o[16]) {
  glmatrix& tmp = (glmatrix&) (o[0]);
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

void be_nontextured() { }
void be_textured() { }
void init() { }

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

int compileShader(int type, const char* s) {
  GLint status;
  
  GLint shader = glCreateShader(type);
  glShaderSource(shader, 1, &s, NULL);
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
  UNIFORM_NORMAL_MATRIX,
  NUM_UNIFORMS
  };

struct GLprogram {
  GLuint _program;
  GLuint vertShader, fragShader;

  GLint uniforms[NUM_UNIFORMS];
  
  GLprogram(string vsh, string fsh) {
    _program = glCreateProgram();
    printf("creating program %d\n", _program);
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
    uniforms[UNIFORM_NORMAL_MATRIX] = glGetUniformLocation(_program, "normalMatrix");    
    printf("uniforms: %d %d\n", uniforms[UNIFORM_MODELVIEWPROJECTION_MATRIX], uniforms[UNIFORM_NORMAL_MATRIX]);
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

GLprogram *textured, *nontextured;

void init() {
  projection = id();
    
  for(int i=0; i<2; i++) {
    auto texture_only = [=] (string s) -> string { if(i) return s; else return ""; };
    auto not_texture_only = [=] (string s) -> string { if(!i) return s; else return ""; };
   
    (i==1?textured:nontextured) = new GLprogram(
      // "attribute vec4 position;"
      // "attribute vec3 normal;"
      
      "varying vec4 vColor;"
      + texture_only( "varying vec2 vTexCoord;" ) +
      
      "uniform mat4 modelViewProjectionMatrix;"
      "uniform mat3 normalMatrix;"
      
      "void main() {"
  //      "vec3 eyeNormal = normalize(normalMatrix * normal);"
  //      "vec3 lightPosition = vec3(0.0, 0.0, 1.0);"
  //      "vec4 diffuseColor = vec4(0.4, 0.4, 1.0, 1.0);"
          
  //      "float nDotVP = max(0.0, dot(eyeNormal, normalize(lightPosition)));"
                       
  //      "vColor = diffuseColor * nDotVP;"
        + texture_only("vTexCoord = gl_MultiTexCoord0.xy;") +
        "vColor = gl_Color;"
          
        "gl_Position = modelViewProjectionMatrix * gl_Vertex;"
        "}",
  
      "uniform sampler2D myTexture;"
      "varying vec4 vColor;"
      + texture_only( "varying vec2 vTexCoord;" ) +
      "void main() {"
      + texture_only("  gl_FragColor = vColor * texture2D(myTexture, vTexCoord);")
      + not_texture_only("  gl_FragColor = vColor;") +
      "  }"
      );
    }

  nontextured->enable();
  }

void set_modelview(const glmatrix& modelview) {
  glmatrix mvp = modelview * projection;
  glUniformMatrix4fv(current->uniforms[UNIFORM_MODELVIEWPROJECTION_MATRIX], 1, 0, mvp.as_array());
  // glmatrix nm = modelview;
  // glUniformMatrix3fv(current->uniforms[UNIFORM_NORMAL_MATRIX], 1, 0, nm[0]);
  }

void be_nontextured() { nontextured->enable(); }
void be_textured() { textured->enable(); }

#endif
}
