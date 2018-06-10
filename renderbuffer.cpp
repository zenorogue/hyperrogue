// Hyperbolic Rogue
// This file implements the 'renderbuffer', which is an object 
// that can be used to draw a HyperRogue screen into, 
// and then either used as a OpenGL texture, or saved.

// Copyright (C) 2011-2018 Zeno Rogue, see 'hyper.cpp' for details

namespace hr {

#if CAP_GL
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
#endif

renderbuffer::renderbuffer(int x, int y, bool gl) : x(x), y(y) {

  valid = false;
  
  #if CAP_GL
  FramebufferName = renderedTexture = depth_stencil_rb = 0; expanded_data = NULL;
  #endif
  
  #if CAP_SDL
  srf = NULL; 
  #endif  

  # if CAP_GL
  if(gl) {
    resetbuffer rb;
    tx = next_p2(x);
    ty = next_p2(y);
  
    FramebufferName = renderedTexture = depth_stencil_rb = 0;
    GLERR("even before");
    glGenFramebuffers(1, &FramebufferName); //
    GLERR("GenFramebuffer");
    glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
    GLERR("BindFramebuffer");
    
    glGenTextures(1, &renderedTexture);
    glBindTexture(GL_TEXTURE_2D, renderedTexture);
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, tx, ty, 0,GL_RGB, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    GLERR("GenTextures");
  
  #ifdef TEX
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture, 0);  
  #else
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderedTexture, 0);  
  #endif
    GLERR("FramebufferTexture");
    // GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    // glDrawBuffers(1, DrawBuffers);
    
    glGenRenderbuffers(1, &depth_stencil_rb);
    GLERR("GenRenderbuffer");
    glBindRenderbuffer(GL_RENDERBUFFER, depth_stencil_rb);
    GLERR("BindRenderbuffer");
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, tx, ty);
    bool has_depth = true;
    if(glGetError() != GL_NO_ERROR) {
      printf("Could not create: GL_DEPTH24_STENCIL8");
      glRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_INDEX8, tx, ty);
      has_depth = false;
      }
    GLERR("RbS");
    if(has_depth)
      glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_stencil_rb);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depth_stencil_rb);
    GLERR("FrRb");
    
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
      FramebufferName = renderedTexture = 0;
    else
      valid = true;

    printf("Framebuffer remains = %d (%d)\n", FramebufferName, valid);
    GLERR("initialization");
    
    rb.reset();
    }
  #endif

  #if CAP_SDL  
  if(!valid)
    make_surface();
  #endif
  }

#if CAP_SDL
void renderbuffer::make_surface() {
  if(!srf)
    srf = SDL_CreateRGBSurface(SDL_SWSURFACE, x, y, 32,0xff0000,0xff00,0xff,0xff000000);
  }

SDL_Surface *renderbuffer::render() {
  make_surface() ;
  if(FramebufferName) {
    glReadPixels(0, 0, x, y, GL_BGRA, GL_UNSIGNED_BYTE, srf->pixels);
    GLERR("readPixels");
    for(int iy=0; iy<y/2; iy++)
    for(int ix=0; ix<x; ix++)
      swap(qpixel(srf,ix,iy), qpixel(srf,ix,vid.yres-1-iy));
    }
  return srf;
  }
#endif

void renderbuffer::enable() {
  #if CAP_GL
  if(FramebufferName) {
    GLERR("prebind");
    glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
    GLERR("bind");
    vid.usingGL = true;
    return;
    }
  #endif
  #if CAP_SDL
  make_surface();
  s = srf;
  vid.usingGL = false;
  #endif
  }

#if CAP_GL
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
#if CAP_SDL
  else {
    if(!expanded_data)
      expanded_data = new Uint32[tx * ty];
    for(int y=0; y<ty; y++) for(int x=0; x<tx; x++)
      expanded_data[y*tx + x] = qpixel(srf, x, ty-1-y) | 0xFF000000;
    glBindTexture( GL_TEXTURE_2D, renderedTexture);
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, tx, ty, 0, GL_BGRA, GL_UNSIGNED_BYTE, expanded_data );    
    }
#endif
  }
#endif

renderbuffer::~renderbuffer() {
#if CAP_GL
  if(renderedTexture)
    glDeleteTextures(1, &renderedTexture);
  if(FramebufferName) {
    glDeleteRenderbuffers(1, &depth_stencil_rb);
    glDeleteFramebuffers(1, &FramebufferName);
    }
  if(expanded_data)
    delete[] expanded_data;
#endif
#if CAP_SDL
  if(srf) 
    SDL_FreeSurface(srf);
#endif
  }

void renderbuffer::clear(int col) {
  #if CAP_GL
  if(FramebufferName) {
    setGLProjection(0);
    return;
    }
  #endif
  #if CAP_SDL
  SDL_FillRect(srf, NULL, col);
  #endif
  }

resetbuffer::resetbuffer() {
  #if CAP_GL
  drawFboId = 0, readFboId = 0;
  glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &drawFboId);
  glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &readFboId);  
  #endif
  #if CAP_SDL
  sreset = s;
  #endif
  }

void resetbuffer::reset() {
  #if CAP_GL
  glBindFramebuffer(GL_FRAMEBUFFER, drawFboId);
  #endif
  #if CAP_SDL
  s = sreset;
  #endif
  }

}
