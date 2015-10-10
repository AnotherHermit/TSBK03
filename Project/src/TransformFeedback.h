///////////////////////////////////////
//
//	Computer Graphics TSBK07
//	Conrad Wahlén - conwa099
//
///////////////////////////////////////

#ifndef TRANSFORMFEEDBACK
#define TRANSFORMFEEDBACK

// ==== Includes ==== //

#ifdef __APPLE__
	#include <OpenGL/gl3.h>
	#include <SDL2/SDL.h>
#else
	#ifdef  __linux__
		#define GL_GLEXT_PROTOTYPES
		#include <GL/gl.h>
		#include <GL/glu.h>
		#include <GL/glx.h>
		#include <GL/glext.h>
		#include <SDL2/SDL.h>
	#else
		#include "glew.h"
		#include "Windows/sdl2/SDL.h"
	#endif
#endif

void InitTFB(GLuint* const program, const GLchar** shaderFilenames, const GLvoid* const data, 
																		const GLuint particles);

void ResetTFB(const GLuint* const program, const GLvoid* const data, const GLuint particles);

void SetBuffers(const GLvoid* const data, const GLuint particles);
												
void DoUpdate(const GLuint particles);

GLuint DoCulling(const GLuint particles, const GLuint cullingBuffer);

void SwapBuffers();

#endif