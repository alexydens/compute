/* Include guard */
#if !defined(LOADGL_H)
#define LOADGL_H

/* Includes */
#include <nh_base.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <GL/glext.h>

/* Buffers */
PFNGLGENBUFFERSPROC glGenBuffers = NH_NULL;
PFNGLBINDBUFFERPROC glBindBuffer = NH_NULL;
PFNGLBUFFERDATAPROC glBufferData = NH_NULL;
PFNGLDELETEBUFFERSPROC glDeleteBuffers = NH_NULL;
/* Shaders */
PFNGLCREATESHADERPROC glCreateShader = NH_NULL;
PFNGLSHADERSOURCEPROC glShaderSource = NH_NULL;
PFNGLCOMPILESHADERPROC glCompileShader = NH_NULL;
PFNGLGETSHADERIVPROC glGetShaderiv = NH_NULL;
PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog = NH_NULL;
PFNGLDELETESHADERPROC glDeleteShader = NH_NULL;
/* Programs */
PFNGLCREATEPROGRAMPROC glCreateProgram = NH_NULL;
PFNGLATTACHSHADERPROC glAttachShader = NH_NULL;
PFNGLLINKPROGRAMPROC glLinkProgram = NH_NULL;
PFNGLGETPROGRAMIVPROC glGetProgramiv = NH_NULL;
PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog = NH_NULL;
PFNGLUSEPROGRAMPROC glUseProgram = NH_NULL;
PFNGLDELETEPROGRAMPROC glDeleteProgram = NH_NULL;
/* Uniforms */
PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation = NH_NULL;
PFNGLUNIFORM1FVPROC glUniform1fv = NH_NULL;
PFNGLUNIFORM2FVPROC glUniform2fv = NH_NULL;
PFNGLUNIFORM3FVPROC glUniform3fv = NH_NULL;
PFNGLUNIFORM4FVPROC glUniform4fv = NH_NULL;
PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv = NH_NULL;
PFNGLUNIFORM1FPROC glUniform1f = NH_NULL;
PFNGLUNIFORM1UIPROC glUniform1ui = NH_NULL;
/* Attributes */
PFNGLGETATTRIBLOCATIONPROC glGetAttribLocation = NH_NULL;
PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer = NH_NULL;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray = NH_NULL;
PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray = NH_NULL;
PFNGLGENVERTEXARRAYSPROC glGenVertexArrays = NH_NULL;
PFNGLBINDVERTEXARRAYPROC glBindVertexArray = NH_NULL;
PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays = NH_NULL;
/* Textures */
PFNGLBINDIMAGETEXTUREPROC glBindImageTexture = NH_NULL;
PFNGLCOPYIMAGESUBDATAPROC glCopyImageSubData = NH_NULL;
/* Compute shaders */
PFNGLDISPATCHCOMPUTEPROC glDispatchCompute = NH_NULL;
PFNGLMEMORYBARRIERPROC glMemoryBarrier = NH_NULL;

/* Function to load */
bool loadGL(void) {
  /* Load OpenGL functions */

  glGenBuffers = (PFNGLGENBUFFERSPROC) SDL_GL_GetProcAddress("glGenBuffers");
  if (glGenBuffers == NH_NULL) return false;
  glBindBuffer = (PFNGLBINDBUFFERPROC) SDL_GL_GetProcAddress("glBindBuffer");
  if (glBindBuffer == NH_NULL) return false;
  glBufferData = (PFNGLBUFFERDATAPROC) SDL_GL_GetProcAddress("glBufferData");
  if (glBufferData == NH_NULL) return false;
  glDeleteBuffers = (PFNGLDELETEBUFFERSPROC) SDL_GL_GetProcAddress("glDeleteBuffers");
  if (glDeleteBuffers == NH_NULL) return false;

  glCreateShader = (PFNGLCREATESHADERPROC) SDL_GL_GetProcAddress("glCreateShader");
  if (glCreateShader == NH_NULL) return false;
  glShaderSource = (PFNGLSHADERSOURCEPROC) SDL_GL_GetProcAddress("glShaderSource");
  if (glShaderSource == NH_NULL) return false;
  glCompileShader = (PFNGLCOMPILESHADERPROC) SDL_GL_GetProcAddress("glCompileShader");
  if (glCompileShader == NH_NULL) return false;
  glGetShaderiv = (PFNGLGETSHADERIVPROC) SDL_GL_GetProcAddress("glGetShaderiv");
  if (glGetShaderiv == NH_NULL) return false;
  glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC) SDL_GL_GetProcAddress("glGetShaderInfoLog");
  if (glGetShaderInfoLog == NH_NULL) return false;
  glDeleteShader = (PFNGLDELETESHADERPROC) SDL_GL_GetProcAddress("glDeleteShader");
  if (glDeleteShader == NH_NULL) return false;

  glCreateProgram = (PFNGLCREATEPROGRAMPROC) SDL_GL_GetProcAddress("glCreateProgram");
  if (glCreateProgram == NH_NULL) return false;
  glAttachShader = (PFNGLATTACHSHADERPROC) SDL_GL_GetProcAddress("glAttachShader");
  if (glAttachShader == NH_NULL) return false;
  glLinkProgram = (PFNGLLINKPROGRAMPROC) SDL_GL_GetProcAddress("glLinkProgram");
  if (glLinkProgram == NH_NULL) return false;
  glGetProgramiv = (PFNGLGETPROGRAMIVPROC) SDL_GL_GetProcAddress("glGetProgramiv");
  if (glGetProgramiv == NH_NULL) return false;
  glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC) SDL_GL_GetProcAddress("glGetProgramInfoLog");
  if (glGetProgramInfoLog == NH_NULL) return false;
  glUseProgram = (PFNGLUSEPROGRAMPROC) SDL_GL_GetProcAddress("glUseProgram");
  if (glUseProgram == NH_NULL) return false;
  glDeleteProgram = (PFNGLDELETEPROGRAMPROC) SDL_GL_GetProcAddress("glDeleteProgram");
  if (glDeleteProgram == NH_NULL) return false;

  glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC) SDL_GL_GetProcAddress("glGetUniformLocation");
  if (glGetUniformLocation == NH_NULL) return false;
  glUniform1fv = (PFNGLUNIFORM1FVPROC) SDL_GL_GetProcAddress("glUniform1fv");
  if (glUniform1fv == NH_NULL) return false;
  glUniform2fv = (PFNGLUNIFORM2FVPROC) SDL_GL_GetProcAddress("glUniform2fv");
  if (glUniform2fv == NH_NULL) return false;
  glUniform3fv = (PFNGLUNIFORM3FVPROC) SDL_GL_GetProcAddress("glUniform3fv");
  if (glUniform3fv == NH_NULL) return false;
  glUniform4fv = (PFNGLUNIFORM4FVPROC) SDL_GL_GetProcAddress("glUniform4fv");
  if (glUniform4fv == NH_NULL) return false;
  glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC) SDL_GL_GetProcAddress("glUniformMatrix4fv");
  if (glUniformMatrix4fv == NH_NULL) return false;
  glUniform1f = (PFNGLUNIFORM1FPROC) SDL_GL_GetProcAddress("glUniform1f");
  if (glUniform1f == NH_NULL) return false;
  glUniform1ui = (PFNGLUNIFORM1UIPROC) SDL_GL_GetProcAddress("glUniform1ui");
  if (glUniform1ui == NH_NULL) return false;

  glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC) SDL_GL_GetProcAddress("glEnableVertexAttribArray");
  if (glEnableVertexAttribArray == NH_NULL) return false;
  glDisableVertexAttribArray = (PFNGLDISABLEVERTEXATTRIBARRAYPROC) SDL_GL_GetProcAddress("glDisableVertexAttribArray");
  if (glDisableVertexAttribArray == NH_NULL) return false;
  glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC) SDL_GL_GetProcAddress("glVertexAttribPointer");
  if (glVertexAttribPointer == NH_NULL) return false;
  glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC) SDL_GL_GetProcAddress("glGenVertexArrays");
  if (glGenVertexArrays == NH_NULL) return false;
  glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC) SDL_GL_GetProcAddress("glBindVertexArray");
  if (glBindVertexArray == NH_NULL) return false;
  glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC) SDL_GL_GetProcAddress("glDeleteVertexArrays");
  if (glDeleteVertexArrays == NH_NULL) return false;

  glBindImageTexture = (PFNGLBINDIMAGETEXTUREPROC) SDL_GL_GetProcAddress("glBindImageTexture");
  if (glBindImageTexture == NH_NULL) return false;
  glCopyImageSubData = (PFNGLCOPYIMAGESUBDATAPROC) SDL_GL_GetProcAddress("glCopyImageSubData");
  if (glCopyImageSubData == NH_NULL) return false;

  glDispatchCompute = (PFNGLDISPATCHCOMPUTEPROC) SDL_GL_GetProcAddress("glDispatchCompute");
  if (glDispatchCompute == NH_NULL) return false;
  glMemoryBarrier = (PFNGLMEMORYBARRIERPROC) SDL_GL_GetProcAddress("glMemoryBarrier");
  if (glMemoryBarrier == NH_NULL) return false;

  return true;
}

#endif /* LOADGL_H */
