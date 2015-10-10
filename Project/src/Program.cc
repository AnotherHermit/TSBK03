
#include "Program.h"

#include <iostream>

Program::Program()
{
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
	renderModels = false;

	// Time init
	currentTime = (GLfloat)SDL_GetTicks();
	deltaTime = 0;
}

int Program::Execute()
{
	if (!Init()) return -1;
	SDL_Event Event;
	while (isRunning)
	{
		timeUpdate();
		while (SDL_PollEvent(&Event)) OnEvent(&Event);
		CheckKeyDowns();
		Update();
		Render();
	}
	Clean();
	return 0;
}

void Program::timeUpdate()
{
	GLfloat t = (GLfloat)SDL_GetTicks();
	deltaTime = t - currentTime;
	currentTime = t;
}

bool Program::Init()
{
	// SDL, glew and OpenGL init
	SDL_Init(SDL_INIT_VIDEO);
	screen = SDL_CreateWindow("Particles!!", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, winWidth, winHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	glcontext = SDL_GL_CreateContext(screen);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glewExperimental = GL_TRUE;
	glewInit();

	// Other SDL inits
	SDL_SetRelativeMouseMode(SDL_TRUE);

	// Set up particle system
	renderModels = false;
	particleSystem = new Particles(20, 1.0f);

	// Set up the camera
	cam = new Camera(glm::vec3(0.0, 0.0, 50.0));
	cam->SetFrustum(-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 150.0f);

	return true;
}

void Program::OnEvent(SDL_Event *Event)
{
	switch (Event->type){
	case SDL_QUIT:
		isRunning = false;
		break;
	case SDL_KEYDOWN:
		OnKeypress(Event);
		break;
	case SDL_MOUSEMOTION:
		OnMouseMove(Event);
		break;
	default:
		break;
	}
}

void Program::OnKeypress(SDL_Event *Event)
{
	switch (Event->key.keysym.sym)
	{
	case SDLK_ESCAPE:
		isRunning = false;
		break;
	case SDLK_SPACE:
		particleSystem->ToggleUpdate();
		break;
	case SDLK_1:
		particleSystem->SetParticles(particleSystem->GetSetParticles() - 10);
		break;
	case SDLK_2:
		particleSystem->SetParticles(particleSystem->GetSetParticles() + 10);
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
		renderModels = !renderModels;
		break;
	case SDLK_l:
		SDL_SetRelativeMouseMode(SDL_FALSE);
		break;
	case SDLK_h:
		SDL_SetRelativeMouseMode(SDL_TRUE);
		break;
	default:
		break;
	}
}

void Program::OnMouseMove(SDL_Event *Event)
{
	cam->RotateCamera(Event->motion.xrel, Event->motion.yrel);
}

void Program::CheckKeyDowns()
{
	const Uint8 *keystate = SDL_GetKeyboardState(NULL);
	if (keystate[SDL_SCANCODE_W]){
		cam->MoveForward(deltaTime);
	}
	if (keystate[SDL_SCANCODE_S]){
		cam->MoveBackward(deltaTime);
	}
	if (keystate[SDL_SCANCODE_A]){
		cam->MoveLeft(deltaTime);
	}
	if (keystate[SDL_SCANCODE_D]){
		cam->MoveRight(deltaTime);
	}
	if (keystate[SDL_SCANCODE_Q]){
		cam->MoveUp(deltaTime);
	}
	if (keystate[SDL_SCANCODE_E]){
		cam->MoveDown(deltaTime);
	}
}

void Program::Update()
{
	// Update the particles
	particleSystem->Update(currentTime);

	// Cull the non visible particles
	particleSystem->Cull(cam->GetCullingNormals(), cam->GetCullingPoints());
}

void Program::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (renderModels)
		particleSystem->DrawParticles(cam->GetProj(), cam->GetWorldView());
	else
		particleSystem->DrawBillboards(cam->GetProj(), cam->GetWorldView(), cam->GetPos());

	SDL_GL_SwapWindow(screen);
}

void Program::Clean()
{
	SDL_GL_DeleteContext(glcontext);
	SDL_Quit();
}