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

#define ELEMENTS_REDUCE_PHASE 1024

GLuint Particles::MAX_BINS = 2097152; // 128*128*128 = 2048 * 1024
GLuint Particles::MAX_PARTICLES = 4194304; // 4096 * 1024

Particles::Particles(GLuint numParticles, GLfloat initBinSize) {
	setParticles = numParticles;
	particles = setParticles*setParticles*setParticles;
	computeDrawParticles = 0;

	numThreads = 256;

	doUpdate = false;

	binParam.bins = setParticles;
	binParam.totalBins = binParam.bins*binParam.bins*binParam.bins;
	binParam.binSize = initBinSize;
	binParam.areaSize = (GLfloat)binParam.bins * binParam.binSize;

	prefixWorkGroups = GLuint(ceil((float)binParam.totalBins / (float)ELEMENTS_REDUCE_PHASE));

	inBufferIndex = 0;
	outBufferIndex = 1;

	// Count of particles per LOD, start at 0 and counted by computeCull
	drawIndCmd[0].instanceCount = 0;
	drawIndCmd[1].instanceCount = 0;
	drawIndCmd[2].instanceCount = 0;
	drawIndCmd[3].instanceCount = 0;

	// Where in the cull buffer the positions of each LOD starts
	drawIndCmd[0].baseInstance = 0;
	drawIndCmd[1].baseInstance = MAX_PARTICLES / 4 * 1;
	drawIndCmd[2].baseInstance = MAX_PARTICLES / 4 * 2;
	drawIndCmd[3].baseInstance = MAX_PARTICLES / 4 * 3;
	// rest of the draw commands are set by drawable when the models are read.

	//create buffers
	glGenBuffers(3, particleBuffers);
	glGenBuffers(3, binBuffers);
	glGenBuffers(1, &drawIndBuffer);
	glGenBuffers(1, &binBuffer);
}

Particles::~Particles() {
	glDeleteBuffers(3, particleBuffers);
	glDeleteBuffers(3, binBuffers);
	glDeleteBuffers(1, &drawIndBuffer);
	glDeleteBuffers(1, &binBuffer);

	glDeleteProgram(computeBin);
	glDeleteProgram(computeSort);
	glDeleteProgram(computeUpdate);
	glDeleteProgram(computeCull);
}

bool Particles::Init() {
	srand((GLuint)time(NULL));

	GLint isOk = 0;

	SetParticleData();

	isOk += CompileComputeShader(&computeBin, "src/shaders/bin.comp");
	isOk += CompileComputeShader(&computePrefixGather, "src/shaders/prefixGather.comp");
	isOk += CompileComputeShader(&computePrefixReduce, "src/shaders/prefixReduce.comp");
	isOk += CompileComputeShader(&computePrefixSpread, "src/shaders/prefixSpread.comp");
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

	prefixWorkGroups = (GLuint)ceil((float)binParam.totalBins / (float)ELEMENTS_REDUCE_PHASE);

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
				particleData[ind].position.x = fmod((float)rand() / (float)RAND_MAX * (float)binParam.totalBins, binParam.areaSize); // X
				particleData[ind].position.y = fmod((float)rand() / (float)RAND_MAX * (float)binParam.totalBins, binParam.areaSize); // Y
				particleData[ind].position.z = fmod((float)rand() / (float)RAND_MAX * (float)binParam.totalBins, binParam.areaSize); // Z

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
	// Initialize buffers
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, particleBuffers[0]); // Particle data
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(ParticleStruct) * particles, particleData.data(), GL_STREAM_COPY);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, particleBuffers[1]); // Particle data ping pong
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(ParticleStruct) * particles, particleData.data(), GL_STREAM_COPY);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, particleBuffers[2]); // Particle positions / cull output buffer
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(GLfloat) * 3 * MAX_PARTICLES, NULL, GL_STREAM_COPY);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	printError("init particle data buffers");

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, binBuffers[0]); // Bin count buffer
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(GLuint) * MAX_BINS, NULL, GL_STREAM_COPY);
	glClearBufferData(GL_SHADER_STORAGE_BUFFER, GL_R32UI, GL_RED, GL_UNSIGNED_INT, NULL);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, binBuffers[1]); // Bin prefix buffer
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(GLuint) * MAX_BINS, NULL, GL_STREAM_COPY);
	glClearBufferData(GL_SHADER_STORAGE_BUFFER, GL_R32UI, GL_RED, GL_UNSIGNED_INT, NULL);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, binBuffers[2]); // Bin prefix buffer
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(GLuint) * ELEMENTS_REDUCE_PHASE, NULL, GL_STREAM_COPY);
	glClearBufferData(GL_SHADER_STORAGE_BUFFER, GL_R32UI, GL_RED, GL_UNSIGNED_INT, NULL);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	printError("init bin data buffers");

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, drawIndBuffer); // Buffer for indirect drawing
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(drawIndCmd), drawIndCmd, GL_STREAM_READ);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	glBindBuffer(GL_UNIFORM_BUFFER, binBuffer); // Uniform bin information
	glBufferData(GL_UNIFORM_BUFFER, sizeof(BinStruct), &binParam, GL_STREAM_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	printError("init uniform data buffers");

	glBindBuffersBase(GL_SHADER_STORAGE_BUFFER, 0, 3, particleBuffers);
	glBindBuffersBase(GL_SHADER_STORAGE_BUFFER, 3, 3, binBuffers);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, drawIndBuffer);
	glBindBufferBase(GL_UNIFORM_BUFFER, 11, binBuffer);

	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_ATOMIC_COUNTER_BARRIER_BIT);

#ifndef _TEST
	particleData.clear();
#endif

	printError("init buffer base bindings");
}


void Particles::ComputeBins() {
	glUseProgram(computeBin);
	glDispatchCompute(particles / numThreads, 1, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	printError("Do Compute: Bin");
}

void Particles::ComputePrefixGather() {
	glUseProgram(computePrefixGather);
	glDispatchCompute(prefixWorkGroups, 1, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	printError("Do Compute: Prefix");
}

void Particles::ComputePrefixReduce() {

	glUseProgram(computePrefixReduce);
	glDispatchCompute(1, 1, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	printError("Do Compute: Prefix");
}

void Particles::ComputePrefixSpread() {

	glUseProgram(computePrefixSpread);
	glDispatchCompute(prefixWorkGroups, 1, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

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
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	printError("Do Compute: Update");
}

void Particles::ComputeCull() {
	GLuint reset = 0;

	DrawElementsIndirectCommand temp;

	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, drawIndBuffer);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0*sizeof(DrawElementsIndirectCommand) + sizeof(GLuint), sizeof(GLuint), &reset); // Clear data before since data is used when drawing
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 1*sizeof(DrawElementsIndirectCommand) + sizeof(GLuint), sizeof(GLuint), &reset);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 2*sizeof(DrawElementsIndirectCommand) + sizeof(GLuint), sizeof(GLuint), &reset);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 3*sizeof(DrawElementsIndirectCommand) + sizeof(GLuint), sizeof(GLuint), &reset);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	glUseProgram(computeCull);
	glDispatchCompute(particles / 64, 1, 1);

	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 2 * sizeof(DrawElementsIndirectCommand) + sizeof(GLuint), sizeof(GLuint), &computeDrawParticles);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 2 * sizeof(DrawElementsIndirectCommand), sizeof(DrawElementsIndirectCommand), &temp);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, drawIndBuffer);
	glMemoryBarrier(GL_COMMAND_BARRIER_BIT);
	//printf("Counter Data: %u, %u, %u, %u\n", tempCounter[0], tempCounter[1], tempCounter[2], tempCounter[3]);

	printError("Do Compute: Culling");
}

void Particles::DoCompute() {
	ComputeBins();
	ComputePrefixGather();
	ComputePrefixReduce();
	ComputePrefixSpread();
	ComputeSort();
	ComputeUpdate();
	ComputeCull();
}
