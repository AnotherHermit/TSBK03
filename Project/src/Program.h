
#ifndef PROGRAM_H
#define PROGRAM_H

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

#include "Camera.h"
#include "Particles.h"
#include "AntTweakBar.h"


class Program {
private:
	SDL_Window *screen;
	SDL_GLContext glcontext;
	int winWidth;
	int winHeight;

	bool isRunning;

	GLfloat currentTime, deltaTime, FPS;

	Particles *particleSystem;
	Camera *cam;
	TwBar *antBar;

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