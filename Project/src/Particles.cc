﻿///////////////////////////////////////
//
//	Computer Graphics TSBK03
//	Conrad Wahlén - conwa099
//
///////////////////////////////////////

#include "Particles.h"

#include "GL_utilities.h"

#include <cstdlib>
#include <math.h>

Particles::Particles(GLuint numParticles, GLfloat initRadius) {
	setParticles = numParticles;
	particles = setParticles*setParticles*setParticles;
	drawParticles = 0;
	startMode = 0;
	radius = initRadius;
	oldT = 0;

	speed = 10.0f;
	pre = 1.0f;
	coh = 0.03f;
	sep = 0.03f;
	ali = 0.03f;

	doUpdate = false;
	renderModels = false;

	numBins = setParticles;
	bins = numBins*numBins*numBins;
	binSize = 20.0f;
	displaybin = 0;

	inBufferIndex = 0;
	outBufferIndex = 1;

	prefixArrayIn = (GLuint*)malloc(sizeof(GLuint) * bins);
	prefixArrayOut = (GLuint*)malloc(sizeof(GLuint) * bins);

	//create buffers
	glGenBuffers(3, particleBuffers);
	glGenBuffers(2, binBuffers);
	glGenBuffers(1, &counterBuffer);
}

bool Particles::Init(Camera* setCam) {
	srand(0);

	SetParticleData();

	CompileComputeShader(&computeBin, "src/shaders/bin.comp");
	CompileComputeShader(&computeSort, "src/shaders/sort.comp");
	CompileComputeShader(&computeUpdate, "src/shaders/update.comp");
	CompileComputeShader(&computeCull, "src/shaders/cull.comp");

	InitCompute();
	cam = setCam;

	model = new Sphere(1.0f);
	model->Init(setCam, particleBuffers[2]);

	billboard = new Billboard(1.0f);
	billboard->Init(setCam, particleBuffers[2]);

	printError("Particles Constructor");
	return true;
}

void Particles::SetParticles(GLuint newParticles) {
	SetParticles(newParticles, startMode);
}

void Particles::SetParticles(GLuint newParticles, GLuint newType) {
	setParticles = newParticles;
	particles = setParticles*setParticles*setParticles;
	startMode = newType;

	numBins = setParticles;
	bins = numBins*numBins*numBins;

	SetParticleData();
	InitCompute();
}

void Particles::SetParticleData() {
	float offset;
	float offsetY;

	if (startMode == 1) {
		// Place particles centered over origin
		offset = -((float)setParticles - 1) / 2;
		offsetY = offset;
	} else if (startMode == 2) {
		// Place particles centered over origin but displaced along y.
		offset = -((float)setParticles - 1) / 2;
		offsetY = 100.0f;
	} else {
		// Place all particles in first octant
		offset = 0;
		offsetY = offset;
	}

	particleData.resize(particles);

	unsigned int ind = 0;
	// Create the instance data
	for (unsigned int i = 0; i < setParticles; ++i) {
		for (unsigned int j = 0; j < setParticles; ++j) {
			for (unsigned int k = 0; k < setParticles; ++k) {
				// Generate positions
				particleData[ind].position.x = fmod((float)rand(), (float)numBins * binSize); // X
				particleData[ind].position.y = fmod((float)rand(), (float)numBins * binSize); // Y
				particleData[ind].position.z = fmod((float)rand(), (float)numBins * binSize); // Z

				// Initiate cell
				particleData[ind].bin = 0;

				// Generate velocities
				particleData[ind].velocity.x = ((float)rand() / (float)RAND_MAX) - 0.5; // X
				particleData[ind].velocity.y = ((float)rand() / (float)RAND_MAX) - 0.5; // Y
				particleData[ind].velocity.z = ((float)rand() / (float)RAND_MAX) - 0.5; // Z

				// Just fill it with something
				particleData[ind].ID = ind;

				ind++;
			}
		}
	}
}

void Particles::CompileComputeShader(GLuint* program, const char* path) {
	*program = glCreateProgram();
	GLuint computeShader = glCreateShader(GL_COMPUTE_SHADER);

	char* cs = readFile((char *)path);
	if (cs == NULL) {
		printf("Error reading shader!\n");
	}

	glShaderSource(computeShader, 1, &cs, NULL);
	glCompileShader(computeShader);
	printShaderInfoLog(computeShader, path);
	//get errors 

	glAttachShader(*program, computeShader);
	glLinkProgram(*program);
	//get errors from the program linking.
	printProgramInfoLog(*program, path, NULL, NULL, NULL, NULL);

	printError("Compile compute shader error!");
}

void Particles::InitCompute() {

	int setCounter = 0;

	// Initialize buffers
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, particleBuffers[0]);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(ParticleStruct) * particles, particleData.data(), GL_STREAM_COPY);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	particleData.clear();

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, particleBuffers[1]);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(ParticleStruct) * particles, NULL, GL_STREAM_COPY);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, particleBuffers[2]);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(GLfloat) * 3 * particles, NULL, GL_STREAM_COPY);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	printError("init Compute Error 1");

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, binBuffers[0]);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(GLuint) * bins, NULL, GL_STREAM_COPY);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, binBuffers[1]);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(GLuint) * bins, NULL, GL_STREAM_COPY);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	printError("init Compute Error 2");

	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, counterBuffer);
	glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), &setCounter, GL_STREAM_READ);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);

	printError("init Compute Error 3");

	glBindBuffersBase(GL_SHADER_STORAGE_BUFFER, 0, 3, particleBuffers);
	glBindBuffersBase(GL_SHADER_STORAGE_BUFFER, 3, 2, binBuffers);
	glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, counterBuffer);

	// Set unchanging uniforms
	glUseProgram(computeBin);
	glUniform1ui(glGetUniformLocation(computeBin, "numBins"), numBins);
	glUniform1f(glGetUniformLocation(computeBin, "binSize"), binSize);

	glUseProgram(computeCull);
	glUniform1f(glGetUniformLocation(computeCull, "radius"), radius);

	glUseProgram(computeUpdate);
	glUniform1ui(glGetUniformLocation(computeUpdate, "numBins"), numBins);
	glUniform1f(glGetUniformLocation(computeUpdate, "binSize"), binSize);

	printError("init Compute Error 4");
}

void Particles::DoCompute(GLfloat t, GLfloat deltaT) {
	GLuint reset = 0;

	if (renderModels) {
		model->Update(t);
	} else {
		billboard->Update(t);
	}
	
	// ========== Calculate Bin =========
	glUseProgram(computeBin);
	glDispatchCompute(particles / 64, 1, 1);
	//glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, binBuffers[0]);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(GLuint) * bins, prefixArrayIn);
	glClearBufferData(GL_SHADER_STORAGE_BUFFER, GL_R32UI, GL_RED, GL_UNSIGNED_INT, &reset);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	printError("Do Compute: Bin");

	// ========== Calculate Prefix sum =========
	prefixArrayOut[0] = 0;
	for (size_t i = 1; i < bins; i++) {
		prefixArrayOut[i] = prefixArrayIn[i-1] + prefixArrayOut[i-1];
		//std::cout << prefixArrayOut[i] << std::endl;
	}
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, binBuffers[1]);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(GLuint) * bins, prefixArrayOut, GL_STREAM_COPY);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	//glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	printError("Do Compute: Prefix");

	// ========== Sort Particles =========
	glUseProgram(computeSort);
	glDispatchCompute(particles / 64, 1, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	printError("Do Compute: Sort");

	// ========== Update Particles =========
	if (doUpdate) {
		glUseProgram(computeUpdate);
		glUniform1f(glGetUniformLocation(computeUpdate, "deltaT"), deltaT);
		glUniform1f(glGetUniformLocation(computeUpdate, "cohWeight"), coh);
		glUniform1f(glGetUniformLocation(computeUpdate, "sepWeight"), sep);
		glUniform1f(glGetUniformLocation(computeUpdate, "aliWeight"), ali);
		glUniform1f(glGetUniformLocation(computeUpdate, "preWeight"), pre);
		glUniform1f(glGetUniformLocation(computeUpdate, "speed"), speed);
		glDispatchCompute(particles / 64, 1, 1);
	} else {
		inBufferIndex = outBufferIndex;
		outBufferIndex = 1 - inBufferIndex;
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, particleBuffers[inBufferIndex]);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, particleBuffers[outBufferIndex]);
	}
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, binBuffers[0]);
	glClearBufferData(GL_SHADER_STORAGE_BUFFER, GL_R32UI, GL_RED, GL_UNSIGNED_INT, &reset);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	//glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	printError("Do Compute: Update");

	// ========== Cull Particles =========
	glUseProgram(computeCull);
	glUniform3fv(glGetUniformLocation(computeCull, "boxNormals"), 5, cam->GetCullingNormals());
	glUniform3fv(glGetUniformLocation(computeCull, "boxPoints"), 5, cam->GetCullingPoints());
	glUniform1ui(glGetUniformLocation(computeCull, "displaybin"), displaybin);
	glDispatchCompute(particles / 64, 1, 1);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, counterBuffer);
	glGetBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), &computeDrawParticles);
	glClearBufferData(GL_ATOMIC_COUNTER_BUFFER, GL_R32UI, GL_RED, GL_UNSIGNED_INT, &reset);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
	//glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_ATOMIC_COUNTER_BARRIER_BIT);

	printError("Do Compute: Culling");
	
}

void Particles::Draw() {
	if (renderModels) {
		model->Draw(computeDrawParticles);
	} else {
		billboard->Draw(computeDrawParticles);
	}
}
