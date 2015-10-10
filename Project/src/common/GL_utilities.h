#ifndef _GL_UTILITIES_
#define _GL_UTILITIES_

#ifdef __cplusplus
extern "C" {
#endif

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

char* readFile(const char *file);
void printError(const char *functionName);
void printShaderInfoLog(GLuint obj, const char *fn);
void printProgramInfoLog(GLuint obj, const char *vfn, const char *ffn,
					const char *gfn, const char *tcfn, const char *tefn);
GLuint loadShaders(const char *vertFileName, const char *fragFileName);
GLuint loadShadersG(const char *vertFileName, const char *fragFileName, const char *geomFileName);
GLuint loadShadersGT(const char *vertFileName, const char *fragFileName, const char *geomFileName,
						const char *tcFileName, const char *teFileName);
void dumpInfo(void);

void initKeymapManager();
char keyIsDown(unsigned char c);
char keyPressed(unsigned char c);

// FBO support

//------------a structure for FBO information-------------------------------------
typedef struct
{
	GLuint texid;
	GLuint fb;
	GLuint rb;
	GLuint depth;
	int width, height;
} FBOstruct;

FBOstruct *initFBO(int width, int height, int int_method);
FBOstruct *initFBO2(int width, int height, int int_method, int create_depthimage);
void useFBO(FBOstruct *out, FBOstruct *in1, FBOstruct *in2);
void updateScreenSizeForFBOHandler(int w, int h); // Temporary workaround to inform useFBO of screen size changes

#ifdef __cplusplus
}
#endif

#endif
