#if !CAP_GLEW
#if ISLINUX
extern "C" {
GLAPI void APIENTRY glGenFramebuffers (GLsizei n, GLuint *framebuffers);
GLAPI void APIENTRY glBindFramebuffer (GLenum target, GLuint framebuffer);
GLAPI void APIENTRY glFramebufferTexture (GLenum target, GLenum attachment, GLuint texture, GLint level);
GLAPI GLenum APIENTRY glCheckFramebufferStatus (GLenum target);
GLAPI void APIENTRY glDrawBuffers (GLsizei n, const GLenum *bufs);
GLAPI void APIENTRY glGenRenderbuffers (GLsizei n, GLuint *renderbuffers);
GLAPI void APIENTRY glBindRenderbuffer (GLenum target, GLuint renderbuffer);
GLAPI void APIENTRY glRenderbufferStorage (GLenum target, GLenum internalformat, GLsizei width, GLsizei height);
GLAPI void APIENTRY glFramebufferRenderbuffer (GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
GLAPI void APIENTRY glDeleteRenderbuffers (GLsizei n, const GLuint *renderbuffers);
GLAPI void APIENTRY glDeleteFramebuffers (GLsizei n, const GLuint *framebuffers);
}
#endif

#if ISMAC
#define glFramebufferTexture glFramebufferTextureEXT 
#endif
#endif

renderbuffer::renderbuffer(int x, int y, bool gl) : x(x), y(y) {

  FramebufferName = renderedTexture = depth_stencil_rb = 0;
  srf = NULL; expanded_data = NULL;

  if(gl) {
    tx = next_p2(x);
    ty = next_p2(y);
    s = NULL;
  #if CAP_GLEW
    if(!glew) { 
      glew = true; 
      GLenum err = glewInit();
      if (GLEW_OK != err) {
        addMessage("Failed to initialize GLEW");
        return;
        }
      }
  #endif
  
    FramebufferName = renderedTexture = depth_stencil_rb = 0;
    glGenFramebuffers(1, &FramebufferName);
    glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
    
    glGenTextures(1, &renderedTexture);
    glBindTexture(GL_TEXTURE_2D, renderedTexture);
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, tx, ty, 0,GL_RGB, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  
  #ifdef TEX
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture, 0);  
  #else
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderedTexture, 0);  
  #endif
    GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, DrawBuffers);
    
    glGenRenderbuffers(1, &depth_stencil_rb);
    glBindRenderbuffer(GL_RENDERBUFFER, depth_stencil_rb);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, tx, ty);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_stencil_rb);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depth_stencil_rb);
    
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
      FramebufferName = renderedTexture = 0;
    }
  
  if(!FramebufferName)
    make_surface();
  }

void renderbuffer::make_surface() {
  if(!srf)
    srf = SDL_CreateRGBSurface(SDL_SWSURFACE, x, y, 32,0xff0000,0xff00,0xff,0xff000000);
  }
  
void renderbuffer::enable() {
  if(FramebufferName) {
    glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
    glViewport(0,0,x,y);
    vid.usingGL = true;
    }
  else {
    make_surface();
    s = srf;
    vid.usingGL = false;
    }
  }

void renderbuffer::use_as_texture() {
  if(!renderedTexture) {
    glGenTextures( 1, &renderedTexture);
    glBindTexture( GL_TEXTURE_2D, renderedTexture);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    }
  if(FramebufferName) {
    glBindTexture( GL_TEXTURE_2D, renderedTexture);
    }
  else {
    if(!expanded_data)
      expanded_data = new Uint32[tx * ty];
    for(int y=0; y<ty; y++) for(int x=0; x<tx; x++)
      expanded_data[y*tx + x] = qpixel(srf, x, ty-1-y) | 0xFF000000;
    glBindTexture( GL_TEXTURE_2D, renderedTexture);
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, tx, ty, 0, GL_BGRA, GL_UNSIGNED_BYTE, expanded_data );    
    }
  }

void renderbuffer::disable() {
  if(FramebufferName) {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0,0,s_screen->w,s_screen->h);
    }
  else {
    s = s_screen;
    }
  }
  
renderbuffer::~renderbuffer() {
  if(renderedTexture)
    glDeleteTextures(1, &renderedTexture);
  if(FramebufferName) {
    glDeleteRenderbuffers(1, &depth_stencil_rb);
    glDeleteFramebuffers(1, &FramebufferName);
    }
  if(srf) 
    SDL_FreeSurface(srf);
  if(expanded_data)
    delete[] expanded_data;
  }

SDL_Surface *renderbuffer::render() {
  make_surface();
  if(FramebufferName) {
    glReadPixels(0, 0, vid.xres, vid.yres, GL_BGRA, GL_UNSIGNED_BYTE, srf->pixels);
    for(int y=0; y<vid.yres/2; y++)
    for(int x=0; x<vid.xres; x++)
      swap(qpixel(srf,x,y), qpixel(srf,x,vid.yres-1-y));
    }
  return srf;
  }

void renderbuffer::clear(int col) {
  if(FramebufferName)
    setGLProjection(0);
  else
    SDL_FillRect(srf, NULL, col);
  }
