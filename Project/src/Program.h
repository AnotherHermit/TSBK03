///////////////////////////////////////
//
//	Computer Graphics TSBK03
//	Conrad Wahl�n - conwa099
//
///////////////////////////////////////

#ifndef PROGRAM_H
#define PROGRAM_H

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

#include "Camera.h"
#include "Particles.h"
#include "Boid.h"

#include "GL_utilities.h"

#include "AntTweakBar.h"

struct ProgramStruct {
	GLfloat currentT;
	GLfloat deltaT;
	GLfloat radius;
	GLfloat simulationSpeed;
};

class Program {
private:
	SDL_Window *screen;
	SDL_GLContext glcontext;
	GLint winWidth, winHeight;

	bool isRunning;

	Timer time;
	GLfloat FPS;
	ProgramStruct param;
	GLuint programBuffer;

	Boid *boid;
	Particles *parts;
	Camera *cam;
	TwBar *antBar;

	// Drawing stuff
	Sphere* spheres;

	// Program params
	glm::vec3 cameraStartPos;
	glm::vec4 cameraLODLevels;
	PartCount particlesPerSide;
	GLuint binsPerSide;
	GLfloat binSize;

	// Methods
	void UploadParams();

public:
	Program();

	int Execute();

	void timeUpdate();

	bool Init();

	void OnEvent(SDL_Event *Event);
	void OnKeypress(SDL_Event *Event);
	void OnMouseMove(SDL_Event *Event);
	void CheckKeyDowns();

	void Update();
	void Render();

	void Clean();
};

#endif // PROGRAM_H
