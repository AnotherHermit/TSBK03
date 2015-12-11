///////////////////////////////////////
//
//	Computer Graphics TSBK03
//	Conrad Wahlén - conwa099
//
///////////////////////////////////////

#include "Fixtures.h"
#include <iostream>

// ===== General compute shader setup =====

void ComputeTest::InitOpenGL() {
	ASSERT_EQ(0, SDL_Init(SDL_INIT_VIDEO)) << "Failed to init SDL";

	screen = SDL_CreateWindow("Particles!!", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 100, 100, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	ASSERT_FALSE(screen == 0);
	glcontext = SDL_GL_CreateContext(screen);

#ifdef _WINDOWS
	ASSERT_EQ(GLEW_OK, glewInit()) << "Failed to initialize GLEW";
#endif

	CPUTimer = new Timer();
	GPUTimer = new GLTimer();
}

void ComputeTest::ExitOpenGL() {
	if (CPUTimer != nullptr) {
		delete(CPUTimer);
	}
	if (GPUTimer != nullptr) {
		delete(GPUTimer);
	}

	SDL_GL_DeleteContext(glcontext);
	SDL_Quit();
}

// ===== Bun particles =====

void ComputeBin::SetUp() {
	CPUBin = nullptr;

	InitOpenGL();

	GLuint particlesPerSide = *GetParam();
	GLfloat binSize = 20.0f;

	parts = new Particles(particlesPerSide, binSize);
	ASSERT_TRUE(parts->Init());

	CPUBin = (GLint*)malloc(sizeof(GLint) * parts->GetTotalBins());
	memset(CPUBin, 0, sizeof(GLint) * parts->GetTotalBins());
	//count = 0;

	glFinish();
}

void ComputeBin::TearDown() {
	if (CPUTimer != nullptr) {
		std::cout << "[ CPU TIME ] " << CPUTimer->getTimeMS() << " ms" << std::endl;
	}
	
	//count++;
	
	if (GPUTimer != nullptr) {
		std::cout << "[ GPU TIME ] " << GPUTimer->getTimeMS() << " ms" << std::endl;
	}

	if (CPUBin != nullptr) {
		free(CPUBin);
	}
	delete(parts);

	ExitOpenGL();
}

void ComputeBin::CPUSolution() {
	for (size_t i = 0; i < parts->GetParticles(); i++) {
		glm::vec3 pos = parts->GetParticleData()[i].position / 20.0f;
		GLuint bin = (GLuint)floor(pos.x) + (GLuint)floor(pos.y) * parts->GetBins() + (GLuint)floor(pos.z) * parts->GetBins() * parts->GetBins();

		CPUBin[bin]++;
		parts->GetParticleData()[i].bin = bin;
	}
}


// ===== Prefix sum =====

void ComputePrefix::SetUp() {
	CPUBin = nullptr;

	InitOpenGL();

	GLuint particlesPerSide = *GetParam(); // 52 fails (?)
	GLfloat binSize = 20.0f;

	parts = new Particles(particlesPerSide, binSize);
	ASSERT_TRUE(parts->Init());

	CPUBin = (GLint*)malloc(sizeof(GLint) * parts->GetTotalBins());

	totalSum = 0;
	
	srand(time(NULL));
	for (size_t i = 0; i < parts->GetTotalBins(); i++) {
		CPUBin[i] = rand() % 30;
	}

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, parts->GetBinBuffers()[0]); // Bin count buffer
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(GLuint) * parts->GetTotalBins(), CPUBin, GL_STREAM_COPY);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	glFinish();
}

void ComputePrefix::TearDown() {
	if (CPUTimer != nullptr) {
		std::cout << "[ CPU TIME ] " << CPUTimer->getTimeMS() << " ms" << std::endl;
	}
	if (GPUTimer != nullptr) {
		std::cout << "[ GPU TIME ] " << GPUTimer->getTimeMS() << " ms" << std::endl;
	}

	if (CPUBin != nullptr) {
		free(CPUBin);
	}
	delete(parts);

	ExitOpenGL();
}

void ComputePrefix::CPUSolution() {
	GLint sum = 0;
	for (size_t i = 0; i < parts->GetTotalBins(); i++) {
		std::swap(CPUBin[i], sum);
		sum += CPUBin[i];
	}
}

