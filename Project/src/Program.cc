
#include "Program.h"

#include <iostream>

Program::Program() {
	// Set alla pointers to null
	screen = NULL;
	glcontext = NULL;
	cam = NULL;
	particleSystem = NULL;

	// Window init size
	winWidth = 800;
	winHeight = 800;

	// Start state
	isRunning = true;

	// Time init
	currentTime = (GLfloat)SDL_GetTicks();
	deltaTime = 0;
}

int Program::Execute() {
	if (!Init()) return -1;
	SDL_Event Event;
	while (isRunning) {
		timeUpdate();
		while (SDL_PollEvent(&Event)) OnEvent(&Event);
		CheckKeyDowns();
		Update();
		Render();
	}
	Clean();
	return 0;
}

void Program::timeUpdate() {
	GLfloat t = (GLfloat)SDL_GetTicks();
	deltaTime = t - currentTime;
	currentTime = t;
	FPS = 1000.0f / deltaTime;
}

bool Program::Init() {
	// SDL, glew and OpenGL init
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		std::cerr << "Failed to initialise SDL: " << SDL_GetError() << std::endl;
		return false;
	}
	screen = SDL_CreateWindow("Particles!!", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, winWidth, winHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	if (screen == 0) {
		std::cerr << "Failed to set Video Mode: " << SDL_GetError() << std::endl;
		return false;
	}

	glcontext = SDL_GL_CreateContext(screen);
	// Start mouse hidden or visible.
	SDL_SetRelativeMouseMode(SDL_FALSE);
	printError("After SDL init: ");

	glewInit();
	printError("After GLEW init: ");

	// Set up the camera
	cam = new Camera(glm::vec3(0.0, 0.0, 50.0));
	cam->SetFrustum(-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 150.0f);

	// Set up particle system
	particleSystem = new Particles(16, 1.0f);
	particleSystem->Init(cam);

	printError("After my stuff init: ");
	
	// Set up the AntBar
	TwInit(TW_OPENGL_CORE, NULL);
	TwWindowSize(winWidth, winWidth);
	antBar = TwNewBar("Particles");

	TwDefine(" Particles refresh=0.1 ");
	TwDefine(" Particles valueswidth=fit ");
	TwDefine(" Particles size='230 150' ");

	TwAddVarRO(antBar, "Total Particles", TW_TYPE_INT32, particleSystem->GetParticlesPtr(), "group=Info");
	TwAddVarRO(antBar, "Rendered Particles", TW_TYPE_INT32, particleSystem->GetDrawParticlesPtr(), "group=Info");
	TwAddVarRO(antBar, "FPS", TW_TYPE_FLOAT, &FPS, "group=Info");
	TwAddVarRW(antBar, "MovSpeed", TW_TYPE_FLOAT, cam->SpeedPtr(), " min=0 max=1 step=0.001 group=Controls label='Movement speed' ");
	TwAddVarRW(antBar, "Display Bin", TW_TYPE_UINT32, particleSystem->GetDisplayBinPtr(), " min=0 max=4096 step=4 group=Controls label='Display Bin' ");

	printError("After AntBar init: ");

	return true;
}

void Program::OnEvent(SDL_Event *Event) {
	switch (Event->type) {
	case SDL_QUIT:
		isRunning = false;
		break;
	case SDL_KEYDOWN:
		OnKeypress(Event);
		break;
	case SDL_MOUSEMOTION:
		OnMouseMove(Event);
		break;
	case SDL_MOUSEBUTTONDOWN:
		TwMouseButton(TW_MOUSE_PRESSED, TW_MOUSE_LEFT);
		break;
	case SDL_MOUSEBUTTONUP:
		TwMouseButton(TW_MOUSE_RELEASED, TW_MOUSE_LEFT);
		break;
	default:
		break;
	}
}

void Program::OnKeypress(SDL_Event *Event) {
	TwKeyPressed(Event->key.keysym.sym, TW_KMOD_NONE);
	switch (Event->key.keysym.sym) {
	case SDLK_ESCAPE:
		isRunning = false;
		break;
	case SDLK_SPACE:
		particleSystem->ToggleUpdate();
		break;
	case SDLK_1:
		particleSystem->SetParticles(particleSystem->GetSetParticles() / 2);
		break;
	case SDLK_2:
		particleSystem->SetParticles(particleSystem->GetSetParticles() * 2);
		break;
	case SDLK_3:
		particleSystem->SetParticles(particleSystem->GetSetParticles(), 0);
		cam->ResetCamera(glm::vec3(0, 0, 50));
		break;
	case SDLK_4:
		particleSystem->SetParticles(particleSystem->GetSetParticles(), 1);
		cam->ResetCamera(glm::vec3(0, 0, 50));
		break;
	case SDLK_5:
		particleSystem->SetParticles(particleSystem->GetSetParticles(), 2);
		cam->ResetCamera(glm::vec3(0, 200, 50));
		break;
	case SDLK_r:
		particleSystem->SetParticles(particleSystem->GetSetParticles());
		break;
	case SDLK_t:
		particleSystem->ToggleDrawModels();
		break;
	case SDLK_l:
		cam->TogglePause();
		SDL_SetRelativeMouseMode(SDL_GetRelativeMouseMode() ? SDL_FALSE : SDL_TRUE);
		break;
	default:
		break;
	}
}

void Program::OnMouseMove(SDL_Event *Event) {
	if (!SDL_GetRelativeMouseMode())
		TwMouseMotion(Event->motion.x, Event->motion.y);
	cam->RotateCamera(Event->motion.xrel, Event->motion.yrel);
}

void Program::CheckKeyDowns() {
	const Uint8 *keystate = SDL_GetKeyboardState(NULL);
	if (keystate[SDL_SCANCODE_W]) {
		cam->MoveForward(deltaTime);
	}
	if (keystate[SDL_SCANCODE_S]) {
		cam->MoveForward(-deltaTime);
	}
	if (keystate[SDL_SCANCODE_A]) {
		cam->MoveRight(-deltaTime);
	}
	if (keystate[SDL_SCANCODE_D]) {
		cam->MoveRight(deltaTime);
	}
	if (keystate[SDL_SCANCODE_Q]) {
		cam->MoveUp(deltaTime);
	}
	if (keystate[SDL_SCANCODE_E]) {
		cam->MoveUp(-deltaTime);
	}
}

void Program::Update() {
	// Update the camera
	cam->UpdateCamera();

	// Update the particles
	//particleSystem->Update(currentTime);
	particleSystem->DoCompute(currentTime);

	// Cull the non visible particles
	//particleSystem->Cull();
}

void Program::Render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	particleSystem->Draw();

	TwDraw();

	printError("Display: ");

	SDL_GL_SwapWindow(screen);
}

void Program::Clean() {
	TwTerminate();
	SDL_GL_DeleteContext(glcontext);
	SDL_Quit();
}