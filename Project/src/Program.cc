///////////////////////////////////////
//
//	Computer Graphics TSBK03
//	Conrad Wahlén - conwa099
//
///////////////////////////////////////

#include "Program.h"

#include "GL_utilities.h"


#include <iostream>


Program::Program() {
	// Set alla pointers to null
	screen = NULL;
	glcontext = NULL;
	cam = NULL;
	parts = NULL;

	// Window init size
	winWidth = 800;
	winHeight = 800;

	// Start state
	isRunning = true;

	// Time init
	time.startTimer();

	// Set program parameters
	param.radius = 1.0f;
	param.simulationSpeed = 30.0f;

	cameraStartPos = glm::vec3(-100.0, 100.0, -100.0);
	cameraLODLevels = glm::vec4(1000.0f, 400.0f, 70.0f, 20.0f);

	particlesPerSide = COUNT0;
	binsPerSide = 16;
	binSize = 60.0f;
}

int Program::Execute() {
	if (!Init()) {
		std::cout << "\nInit failed. Press enter to quit..." << std::endl;
		getchar();
		return -1;
	}

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
	time.endTimer();
	param.deltaT = time.getLapTime();
	param.currentT = time.getTime();
	FPS = 1.0f / time.getLapTime();
}

bool Program::Init() {
	// SDL, glew and OpenGL init
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
		return false;
	}
	screen = SDL_CreateWindow("Particles!!", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, winWidth, winHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	if (screen == 0) {
		std::cerr << "Failed to set Video Mode: " << SDL_GetError() << std::endl;
		return false;
	}

	glcontext = SDL_GL_CreateContext(screen);
	SDL_SetRelativeMouseMode(SDL_FALSE);
	
#ifdef _WINDOWS
	GLenum err = glewInit();
	if (err != GLEW_OK) {
		std::cerr << "Failed to initialize GLEW: " << glewGetErrorString(err) << std::endl;
		return false;
	}
#endif

	dumpInfo();

	printError("after wrapper inits");

	glGenBuffers(1, &programBuffer);
	glBindBufferBase(GL_UNIFORM_BUFFER, 12, programBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, programBuffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(ProgramStruct), &param, GL_STREAM_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	// Set up the AntBar
	TwInit(TW_OPENGL_CORE, NULL);
	TwWindowSize(winWidth, winWidth);
	antBar = TwNewBar("Particles");
	TwDefine(" Particles refresh=0.1 size='300 420' valueswidth=140 ");
	TwDefine(" Particles help='This program simulates flocking behaviour of lots of particles in 3D.\n"
			 "The simulation is started/paused by pressing SPACE.' ");

	// Set up the camera
	cam = new Camera(cameraStartPos, &winWidth, &winHeight, cameraLODLevels);
	if (!cam->Init()) return false;
	
	// Set up particle system
	parts = new Particles(particlesPerSide, binsPerSide, binSize);
	if (!parts->Init()) return false;

	// Set up boids
	boid = new Boid();
	if (!boid->Init()) return false;

	// Set up different models to render
	spheres = new Sphere(parts->GetCullBuffer(), parts->GetDrawCommandBuffer());
	if(!spheres->Init("resources/Spheres-flat.obj")) return false;

	TwAddVarRO(antBar, "FPS", TW_TYPE_FLOAT, &FPS, " group=Info ");
	TwAddVarRW(antBar, "Sim Speed", TW_TYPE_FLOAT, &param.simulationSpeed, " min=0 max=200 step=5 group=Controls ");
	TwAddVarRW(antBar, "Cam Speed", TW_TYPE_FLOAT, cam->GetSpeedPtr(), " min=0 max=500 step=10 group=Controls ");
	TwAddVarRW(antBar, "Cam Rot Speed", TW_TYPE_FLOAT, cam->GetRotSpeedPtr(), " min=0.0 max=0.010 step=0.001 group=Controls ");
	TwAddVarCB(antBar, "Camera", cam->GetCameraTwType(), cam->SetLODCB, cam->GetCamParamsCB, cam, " opened=true ");
	TwAddVarCB(antBar, "Boids", boid->GetBoidTwType(), boid->SetBoidCB, boid->GetBoidParamsCB, boid, " opened=true ");
	TwAddVarCB(antBar, "Particles", parts->GetParticlesTwType(), parts->SetParticleCB, parts->GetParticleCB, parts, " group='Particle Controls' ");
	TwAddVarRO(antBar, "Particle count", TW_TYPE_UINT32, parts->GetParticlePtr(), " group=Info ");
	TwAddVarCB(antBar, "Bins", parts->GetBinTwType(), parts->SetBinCB, parts->GetBinCB, parts, " opened=true ");
	TwDefine(" Particles/'Boid Controls' group=Controls");
	TwDefine(" Particles/'Particle Controls' group=Controls");

	// Check if AntTweak Setup is ok
	if (TwGetLastError() != NULL) {
		return false;
	}

	return true;
}



void Program::Update() {
	// Upload program params (incl time update)
	UploadParams();

	// Update the camera
	cam->UpdateCamera();
	
	// Update the particles
	parts->DoCompute();

	printError("after update");
}

void Program::Render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	spheres->Draw();

	TwDraw();

	printError("after display");

	SDL_GL_SwapWindow(screen);
}

void Program::Clean() {
	glDeleteBuffers(1, &programBuffer);
	TwTerminate();
	SDL_GL_DeleteContext(glcontext);
	SDL_Quit();
}

void Program::UploadParams() {
	// Update program parameters
	glBindBuffer(GL_UNIFORM_BUFFER, programBuffer);
	glBufferSubData(GL_UNIFORM_BUFFER, NULL, sizeof(ProgramStruct), &param);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	printError("program param upload");
}

void Program::OnEvent(SDL_Event *Event) {
	switch (Event->type) {
	case SDL_QUIT:
		isRunning = false;
		break;
	case SDL_WINDOWEVENT:
		switch (Event->window.event) {
		case SDL_WINDOWEVENT_RESIZED:
			SDL_SetWindowSize(screen, Event->window.data1, Event->window.data2);
			SDL_GetWindowSize(screen, &winWidth, &winHeight);
			glViewport(0, 0, winWidth, winHeight);
			TwWindowSize(winWidth, winHeight);
			cam->SetFrustum();
			break;
		}
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
		parts->ToggleUpdate();
		break;
	case SDLK_t:
		parts->TogglePartUpdate();
		break;/*
	case SDLK_r:
		parts->SetParticles(parts->GetParticles());
		break;*/
	case SDLK_f:
		cam->TogglePause();
		SDL_SetRelativeMouseMode(SDL_GetRelativeMouseMode() ? SDL_FALSE : SDL_TRUE);
		break;
	case SDLK_g:
		int isBarHidden;
		TwGetParam(antBar, NULL, "iconified", TW_PARAM_INT32, 1, &isBarHidden);
		if (isBarHidden) {
			TwDefine(" Particles iconified=false ");
		} else {
			TwDefine(" Particles iconified=true ");
		}
		break;
	default:
		break;
	}
}

void Program::OnMouseMove(SDL_Event *Event) {
	if (!SDL_GetRelativeMouseMode())
		TwMouseMotion(Event->motion.x, Event->motion.y);
	else
		cam->RotateCamera(Event->motion.xrel, Event->motion.yrel);
}

void Program::CheckKeyDowns() {
	const Uint8 *keystate = SDL_GetKeyboardState(NULL);
	if (keystate[SDL_SCANCODE_W]) {
		cam->MoveForward(param.deltaT);
	}
	if (keystate[SDL_SCANCODE_S]) {
		cam->MoveForward(-param.deltaT);
	}
	if (keystate[SDL_SCANCODE_A]) {
		cam->MoveRight(-param.deltaT);
	}
	if (keystate[SDL_SCANCODE_D]) {
		cam->MoveRight(param.deltaT);
	}
	if (keystate[SDL_SCANCODE_Q]) {
		cam->MoveUp(param.deltaT);
	}
	if (keystate[SDL_SCANCODE_E]) {
		cam->MoveUp(-param.deltaT);
	}
}