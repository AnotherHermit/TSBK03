///////////////////////////////////////
//
//	Computer Graphics TSBK03
//	Conrad Wahlén - conwa099
//
///////////////////////////////////////

#include "Particles.h"

#include "GL_utilities.h"

#include <cstdlib>
#include <math.h>

Particles::Particles(GLuint numParticles, GLfloat initBinSize) {
	setParticles = numParticles;
	particles = setParticles*setParticles*setParticles;
	computeDrawParticles = 0;

	doUpdate = true;

	binParam.bins = setParticles;
	binParam.totalBins = binParam.bins*binParam.bins*binParam.bins;
	binParam.binSize = initBinSize;
	binParam.areaSize = (GLfloat)binParam.bins * binParam.binSize;

	inBufferIndex = 0;
	outBufferIndex = 1;

	prefixArray = (GLuint*)malloc(sizeof(GLuint) * binParam.totalBins);

	//create buffers
	glGenBuffers(3, particleBuffers);
	glGenBuffers(2, binBuffers);
	glGenBuffers(1, &counterBuffer);
	glGenBuffers(1, &binBuffer);
}

Particles::~Particles() {
	glDeleteBuffers(3, particleBuffers);
	glDeleteBuffers(2, binBuffers);
	glDeleteBuffers(1, &counterBuffer);
	glDeleteBuffers(1, &binBuffer);

	glDeleteProgram(computeBin);
	glDeleteProgram(computeSort);
	glDeleteProgram(computeUpdate);
	glDeleteProgram(computeCull);

	free(prefixArray);
}

bool Particles::Init() {
	srand(1);

	GLint isOk = 0;

	SetParticleData();

	isOk += CompileComputeShader(&computeBin, "src/shaders/bin.comp");
	isOk += CompileComputeShader(&computePrefix, "src/shaders/prefix.comp");
	isOk += CompileComputeShader(&computeSort, "src/shaders/sort.comp");
	isOk += CompileComputeShader(&computeUpdate, "src/shaders/update.comp");
	isOk += CompileComputeShader(&computeCull, "src/shaders/cull.comp");

	InitCompute();

	printError("Particles Constructor");
	return isOk == 0;
}

void Particles::SetParticles(GLuint newParticles) {
	setParticles = newParticles;
	particles = setParticles*setParticles*setParticles;

	inBufferIndex = 0;
	outBufferIndex = 1;

	binParam.bins = setParticles;
	binParam.totalBins = binParam.bins*binParam.bins*binParam.bins;
	binParam.areaSize = (GLfloat)binParam.bins * binParam.binSize;

	free(prefixArray);
	prefixArray = (GLuint*)malloc(sizeof(GLuint) * binParam.totalBins);

	SetParticleData();
	InitCompute();
}

void Particles::SetParticleData() {
	particleData.clear();
	particleData.resize(particles);
	unsigned int ind = 0;
	// Create the instance data
	for (unsigned int i = 0; i < setParticles; ++i) {
		for (unsigned int j = 0; j < setParticles; ++j) {
			for (unsigned int k = 0; k < setParticles; ++k) {
				// Generate positions
				particleData[ind].position.x = fmod((float)rand(), binParam.areaSize); // X
				particleData[ind].position.y = fmod((float)rand(), binParam.areaSize); // Y
				particleData[ind].position.z = fmod((float)rand(), binParam.areaSize); // Z

				// Initiate cell
				particleData[ind].bin = 0;

				// Generate velocities
				particleData[ind].velocity.x = ((float)rand() / (float)RAND_MAX) - 0.5f; // X
				particleData[ind].velocity.y = ((float)rand() / (float)RAND_MAX) - 0.5f; // Y
				particleData[ind].velocity.z = ((float)rand() / (float)RAND_MAX) - 0.5f; // Z

				// Just fill it with something
				particleData[ind].ID = ind;

				ind++;
			}
		}
	}
}


void Particles::InitCompute() {

	GLuint setCounter[5] = { 0, 1, 0, 0, 0 };

	// Initialize buffers
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, particleBuffers[0]); // Particle data
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(ParticleStruct) * particles, particleData.data(), GL_STREAM_COPY);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, particleBuffers[1]); // Particle data ping pong
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(ParticleStruct) * particles, particleData.data(), GL_STREAM_COPY);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, particleBuffers[2]); // Particle positions / cull output buffer
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(GLfloat) * 3 * particles, NULL, GL_STREAM_COPY);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	printError("init particle data buffers");

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, binBuffers[0]); // Bin count buffer
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(GLuint) * binParam.totalBins, NULL, GL_STREAM_COPY);
	glClearBufferData(GL_SHADER_STORAGE_BUFFER, GL_R32UI, GL_RED, GL_UNSIGNED_INT, NULL);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, binBuffers[1]); // Bin prefix buffer
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(GLuint) * binParam.totalBins, NULL, GL_STREAM_COPY);
	glClearBufferData(GL_SHADER_STORAGE_BUFFER, GL_R32UI, GL_RED, GL_UNSIGNED_INT, NULL);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	printError("init bin data buffers");

	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, counterBuffer); // Buffer for indirect drawing
	glBufferData(GL_ATOMIC_COUNTER_BUFFER, 5 * sizeof(GLuint), setCounter, GL_STREAM_READ);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);

	glBindBuffer(GL_UNIFORM_BUFFER, binBuffer); // Uniform bin information
	glBufferData(GL_UNIFORM_BUFFER, sizeof(BinStruct), &binParam, GL_STREAM_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	printError("init uniform data buffers");

	glBindBuffersBase(GL_SHADER_STORAGE_BUFFER, 0, 3, particleBuffers);
	glBindBuffersBase(GL_SHADER_STORAGE_BUFFER, 3, 2, binBuffers);
	glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, counterBuffer);
	glBindBufferBase(GL_UNIFORM_BUFFER, 11, binBuffer);

	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_ATOMIC_COUNTER_BARRIER_BIT);

#ifndef _TEST
	particleData.clear();
#endif

	printError("init buffer base bindings");
}


void Particles::ComputeBins() {
	glUseProgram(computeBin);
	glDispatchCompute(particles / 64, 1, 1);
	//glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	printError("Do Compute: Bin");
}

void Particles::ComputePrefix() {
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, binBuffers[0]);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(GLuint) * binParam.totalBins, prefixArray);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	GLuint sum = 0;
	for (size_t i = 0; i < binParam.totalBins; i++) {
		std::swap(prefixArray[i], sum);
		sum += prefixArray[i];
	}
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, binBuffers[1]);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(GLuint) * binParam.totalBins, prefixArray, GL_STREAM_COPY);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	//glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	printError("Do Compute: Prefix");
}

void Particles::ComputeSort() {
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, binBuffers[0]);
	glClearBufferData(GL_SHADER_STORAGE_BUFFER, GL_R32UI, GL_RED, GL_UNSIGNED_INT, NULL);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	glUseProgram(computeSort);
	glDispatchCompute(particles / 64, 1, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	printError("Do Compute: Sort");
}

void Particles::ComputeUpdate() {

	if (doUpdate) {
		glUseProgram(computeUpdate);
		glDispatchCompute(particles / 64, 1, 1);
	} else {
		std::swap(inBufferIndex, outBufferIndex);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, particleBuffers[inBufferIndex]);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, particleBuffers[outBufferIndex]);
	}
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, binBuffers[0]);
	glClearBufferData(GL_SHADER_STORAGE_BUFFER, GL_R32UI, GL_RED, GL_UNSIGNED_INT, NULL);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	//glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	printError("Do Compute: Update");
}

void Particles::ComputeCull() {
	//GLuint tempCounter[4];
	GLuint reset = 0;

	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, counterBuffer);
	glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), &reset); // Clear data before since data is used when drawing
	glMemoryBarrier(GL_ATOMIC_COUNTER_BARRIER_BIT);

	glUseProgram(computeCull);
	glDispatchCompute(particles / 64, 1, 1);

	//glGetBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, 5 * sizeof(GLuint), tempCounter);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, counterBuffer);
	glMemoryBarrier(GL_COMMAND_BARRIER_BIT);
	//computeDrawParticles = tempCounter[0];
	//printf("Counter Data: %u, %u, %u, %u\n", tempCounter[0], tempCounter[1], tempCounter[2], tempCounter[3]);

	printError("Do Compute: Culling");
}

void Particles::DoCompute() {
	ComputeBins();
	ComputePrefix();
	ComputeSort();
	ComputeUpdate();
	ComputeCull();
}
