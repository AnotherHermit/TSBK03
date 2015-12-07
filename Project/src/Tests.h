///////////////////////////////////////
//
//	Computer Graphics TSBK03
//	Conrad Wahlén - conwa099
//
///////////////////////////////////////

#ifndef TESTS_H
#define TESTS_H

#include <gtest/gtest.h>
#include "Particles.h"
#include "GL_utilities.h"

class ComputeTest : public ::testing::TestWithParam<const int*> {
protected:
	Particles* parts;
	SDL_Window* screen;
	SDL_GLContext glcontext;

	Timer* CPUTimer;
	GLTimer* GPUTimer;

public:
	ComputeTest() {};
	~ComputeTest() {};

	virtual void SetUp();
	virtual void TearDown();
};

void ComputeTest::SetUp() {
	ASSERT_EQ(0, SDL_Init(SDL_INIT_VIDEO)) << "Failed to init SDL";

	screen = SDL_CreateWindow("Particles!!", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 100, 100, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	ASSERT_FALSE(screen == 0);
	glcontext = SDL_GL_CreateContext(screen);

	ASSERT_EQ(GLEW_OK, glewInit()) << "Failed to initialize GLEW";

	GLuint particlesPerSide = *GetParam(); // 52 fails (?)
	GLfloat binSize = 20.0f;

	parts = new Particles(particlesPerSide, binSize);
	ASSERT_TRUE(parts->Init());

	CPUTimer = new Timer();
	GPUTimer = new GLTimer();
}

void ComputeTest::TearDown() {
	if (CPUTimer != nullptr) {
		std::cout << "[ CPU TIME ] " << CPUTimer->getTimeMS() << " ms" << std::endl;
		delete(CPUTimer);
	}
	if (GPUTimer != nullptr) {
		std::cout << "[ GPU TIME ] " << GPUTimer->getTimeMS() << " ms" << std::endl;
		delete(GPUTimer);
	}

	SDL_GL_DeleteContext(glcontext);
	SDL_Quit();
}

#endif // TESTS_H
