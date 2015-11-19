///////////////////////////////////////
//
//	Computer Graphics TSBK03
//	Conrad Wahlén - conwa099
//
///////////////////////////////////////

#ifndef BOID_H
#define BOID_H

#ifdef __APPLE__
#	include <OpenGL/gl3.h>
#	include <SDL2/SDL.h>
#else
#	ifdef  __linux__
#		define GL_GLEXT_PROTOTYPES
#		include <GL/gl.h>
#		include <GL/glu.h>
#		include <GL/glx.h>
#		include <GL/glext.h>
#		include <SDL2/SDL.h>
#	else
#		include "glew.h"
#		include "Windows/sdl2/SDL.h"
#	endif
#endif

struct BoidStruct {
	GLfloat previous;
	GLfloat cohesion;
	GLfloat separation;
	GLfloat alignment;
	GLfloat fear;
};

class Boid {
private:
	GLuint boidBuffer;
	BoidStruct param;

public:
	Boid();

	void Update();

	GLfloat* GetPreviousPtr() { return &param.previous; }
	GLfloat* GetCohesionPtr() { return &param.cohesion; }
	GLfloat* GetSeparationPtr() { return &param.separation; }
	GLfloat* GetAlignmentPtr() { return &param.alignment; }
	GLfloat* GetFearPtr() { return &param.fear; }

};

#endif // BOID_H
