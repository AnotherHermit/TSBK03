#ifndef __TGA_LOADER__
#define __TGA_LOADER__

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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
	
typedef struct TextureData		// Create A Structure for .tga loading.
{
	GLubyte	*imageData;			// Image Data (Up To 32 Bits)
	GLuint	bpp;				// Image Color Depth In Bits Per Pixel.
	GLuint	width;				// Image Width
	GLuint	height;				// Image Height
	GLuint	w;				// Image Width "raw"
	GLuint	h;				// Image Height "raw"
	GLuint	texID;				// Texture ID Used To Select A Texture
	GLfloat	texWidth, texHeight;
} TextureData, *TextureDataPtr;					// Structure Name

bool LoadTGATexture(const char *filename, TextureData *texture);
void LoadTGATextureSimple(const char *filename, GLuint *tex);
void LoadTGASetMipmapping(bool active);
bool LoadTGATextureData(const char *filename, TextureData *texture);

#ifdef __cplusplus
}
#endif

#endif

