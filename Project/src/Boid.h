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

#include "AntTweakBar.h"

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

	TwStructMember boidTwMembers[5];
	TwType boidTwStruct;

public:
	Boid();

	bool Init();
	void Update();

	TwType GetBoidTwType() { return boidTwStruct; }

	static void TW_CALL SetBoidCB(const void* value, void* clientData);
	static void TW_CALL GetBoidParamsCB(void* value, void* clientData);
};

#endif // BOID_H
