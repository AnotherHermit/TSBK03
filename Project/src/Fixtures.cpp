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

	ASSERT_EQ(GLEW_OK, glewInit()) << "Failed to initialize GLEW";

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

	CPUPrefix = (GLint*)malloc(sizeof(GLint) * parts->GetTotalBins());
	memset(CPUPrefix, 0, sizeof(GLint) * parts->GetTotalBins());

	glFinish();
}

void ComputeBin::TearDown() {
	if (CPUTimer != nullptr) {
		std::cout << "[ CPU TIME ]" << CPUTimer->getTimeMS() << " ms" << std::endl;
	}
	if (GPUTimer != nullptr) {
		std::cout << "[ GPU TIME ]" << GPUTimer->getTimeMS() << " ms" << std::endl;
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

void ComputeBin::CPUSolutionPrefix() {
	CPUSolution();

	GLint sum = 0;
	for (size_t i = 0; i < parts->GetTotalBins(); i++) {
		CPUPrefix[i] = sum;
		sum += CPUBin[i];
	}
}

// ===== Prefix sum =====

void ComputePrefix::SetUp() {
	CPUBin = nullptr;
	TempBin = nullptr;
	InitOpenGL();

	GLuint particlesPerSide = *GetParam(); // 52 fails (?)
	GLfloat binSize = 20.0f;

	parts = new Particles(particlesPerSide, binSize);
	ASSERT_TRUE(parts->Init());

	CPUBin = (GLint*)malloc(sizeof(GLint) * parts->GetTotalBins());
	TempBin = (GLint*)malloc(sizeof(GLint) * 1024);
	memset(TempBin, 0, sizeof(GLint) * 1024);


	totalSum = 0;
	
	srand((GLuint)time(NULL));
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
	if (TempBin != nullptr) {
		free(TempBin);
	}

	delete(parts);

	ExitOpenGL();
}


void ComputePrefix::CPUSolutionGather() {
	GLint sum;
	GLuint offset = parts->GetTotalBins() / 1024;
	for (size_t i = 0; i < 1024; i++) {
		sum = 0;
		for (size_t j = 0; j < offset; j++) {
			std::swap(CPUBin[j + offset * i], sum);
			sum += CPUBin[j + offset * i];
		}
		TempBin[i] = sum;
	}
}

void ComputePrefix::CPUSolutionReduce() {
	GLint sum = 0;
	for (size_t i = 0; i < 1024; i++) {
		std::swap(TempBin[i], sum);
		sum += TempBin[i];
	}
}

void ComputePrefix::CPUSolutionSpread() {
	GLint sum = 0;
	for (size_t i = 0; i < 1024; i++) {
		std::swap(TempBin[i], sum);
		sum += TempBin[i];
	}
}


void ComputePrefix::CPUSolution() {
	GLint sum = 0;
	for (size_t i = 0; i < parts->GetTotalBins(); i++) {
		std::swap(CPUBin[i], sum);
		sum += CPUBin[i];
	}
}

