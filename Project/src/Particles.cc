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

Particles::Particles(PartCount count, GLuint numBins, GLfloat initBinSize) {
	partCount = count;
	particles = (GLuint)partCount;

	binParam.bins = numBins;
	binParam.totalBins = (GLuint)pow(binParam.bins, 3); // Equal amout of bins per side
	binParam.binSize = initBinSize;
	binParam.areaSize = (GLfloat)binParam.bins * binParam.binSize;

	doUpdate = false;
	partUpdate = true;

	prefixWorkGroups = GLuint(ceil((float)binParam.totalBins / (float)ELEMENTS_REDUCE_PHASE));
	particleWorkGroups = particles / COMPUTE_THREADS;

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

	binTwMembers[0] = {"Bins per side", TW_TYPE_UINT32, offsetof(BinStruct, bins), " min=16 max=128 step=16 group='Particle Controls' "};
	binTwMembers[1] = {"Search distance", TW_TYPE_FLOAT, offsetof(BinStruct, binSize), " min=5.0 max=200.0 step=5.0 group='Boid Controls' "};
	binTwMembers[2] = {"Bins in total", TW_TYPE_UINT32, offsetof(BinStruct, totalBins), " readonly=true group=Info "};
	binTwMembers[2] = {"Area size", TW_TYPE_FLOAT, offsetof(BinStruct, areaSize), " readonly=true group=Info "};
	binTwStruct = TwDefineStruct("Bins", binTwMembers, 3, sizeof(BinStruct), NULL, NULL);

	numParticlesEV[0] = {COUNT0, "Is that it?"};
	numParticlesEV[1] = {COUNT1, "Still not good"};
	numParticlesEV[2] = {COUNT2, "Barely"};
	numParticlesEV[3] = {COUNT3, "Alright"};
	numParticlesEV[4] = {COUNT4, "Feels good"};
	numParticlesEV[5] = {COUNT5, "Getting heavy"};
	numParticlesEV[6] = {COUNT6, "Lets stop here"};
	numParticlesEV[7] = {COUNT7, "MOARST!!"};
	particleCount = TwDefineEnum("Particle Count", numParticlesEV, 8);
}

Particles::~Particles() {
	glDeleteBuffers(3, particleBuffers);
	glDeleteBuffers(3, binBuffers);
	glDeleteBuffers(1, &drawIndBuffer);
	glDeleteBuffers(1, &binBuffer);

	glDeleteProgram(computeBin);
	glDeleteProgram(computeSort);
	glDeleteProgram(computeUpdatePart);
	glDeleteProgram(computeUpdateBin);
	glDeleteProgram(computeCull);

	printError("destruct particles");
}

bool Particles::Init() {
	GLint isOk = 0;
	isOk += CompileComputeShader(&computeBin, "src/shaders/bin.comp");
	isOk += CompileComputeShader(&computePrefixGather, "src/shaders/prefixGather.comp");
	isOk += CompileComputeShader(&computePrefixReduce, "src/shaders/prefixReduce.comp");
	isOk += CompileComputeShader(&computePrefixSpread, "src/shaders/prefixSpread.comp");
	isOk += CompileComputeShader(&computeSort, "src/shaders/sort.comp");
	isOk += CompileComputeShader(&computeUpdatePart, "src/shaders/updatePart.comp");
	isOk += CompileComputeShader(&computeUpdateBin, "src/shaders/updateBin.comp");
	isOk += CompileComputeShader(&computeCull, "src/shaders/cull.comp");

	SetParticleData();
	InitBuffers();

	printError("init particles");
	return isOk == 0;
}

void Particles::SetParticles(PartCount newCount) {
	partCount = newCount;
	particles = (GLuint)partCount;

	particleWorkGroups = particles / COMPUTE_THREADS;

	SetParticleData();
	ResetBuffers();
}

void Particles::SetBins(GLuint newBins, GLfloat newSize) {
	binParam.bins = newBins;
	binParam.totalBins = (GLuint)pow(binParam.bins, 3);
	binParam.binSize = newSize;
	binParam.areaSize = (GLfloat)binParam.bins * binParam.binSize;

	prefixWorkGroups = (GLuint)ceil((float)binParam.totalBins / (float)ELEMENTS_REDUCE_PHASE);

	SetParticleData();
	ResetBuffers();
}

void Particles::SetParticleData() {
	//particleData.clear();
	particleData.resize(particles);

	ParticleStruct tempPart;
	srand((GLuint)time(NULL));

	// Create the instance data
	for (unsigned int ind = 0; ind < particles; ind++) {
		// Generate positions
		tempPart.position.x = fmod((float)rand() / (float)RAND_MAX * (float)binParam.totalBins, binParam.areaSize); // X
		tempPart.position.y = fmod((float)rand() / (float)RAND_MAX * (float)binParam.totalBins, binParam.areaSize); // Y
		tempPart.position.z = fmod((float)rand() / (float)RAND_MAX * (float)binParam.totalBins, binParam.areaSize); // Z

		// Initiate cell
		tempPart.bin = 0;

		// Generate velocities
		tempPart.velocity.x = (((float)rand() / (float)RAND_MAX) - 0.5f) * 30.0f; // X
		tempPart.velocity.y = (((float)rand() / (float)RAND_MAX) - 0.5f) * 30.0f; // Y
		tempPart.velocity.z = (((float)rand() / (float)RAND_MAX) - 0.5f) * 30.0f; // Z

		// Just fill it with something
		tempPart.ID = ind;

		particleData[ind] = tempPart;
	}

	inBufferIndex = 0;
	outBufferIndex = 1;
}

void Particles::InitBuffers() {
	//create buffers
	glGenBuffers(3, particleBuffers);
	glGenBuffers(3, binBuffers);
	glGenBuffers(1, &drawIndBuffer);
	glGenBuffers(1, &binBuffer);

	// Initialize buffers
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, particleBuffers[0]); // Particle data
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(ParticleStruct) * particles, particleData.data(), GL_STREAM_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, particleBuffers[1]); // Particle data ping pong
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(ParticleStruct) * particles, particleData.data(), GL_STREAM_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, particleBuffers[2]); // Particle positions / cull output buffer
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(GLfloat) * 3 * MAX_PARTICLES, NULL, GL_STREAM_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	printError("init particle data buffers");

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, binBuffers[0]); // Bin count buffer
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(GLuint) * binParam.totalBins, NULL, GL_STREAM_DRAW);
	glClearBufferData(GL_SHADER_STORAGE_BUFFER, GL_R32UI, GL_RED, GL_UNSIGNED_INT, NULL);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, binBuffers[1]); // Bin prefix buffer first pass
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(GLuint) * binParam.totalBins, NULL, GL_STREAM_DRAW);
	glClearBufferData(GL_SHADER_STORAGE_BUFFER, GL_R32UI, GL_RED, GL_UNSIGNED_INT, NULL);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, binBuffers[2]); // Bin prefix buffer second pass
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(GLuint) * ELEMENTS_REDUCE_PHASE, NULL, GL_STREAM_DRAW);
	glClearBufferData(GL_SHADER_STORAGE_BUFFER, GL_R32UI, GL_RED, GL_UNSIGNED_INT, NULL);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	printError("init bin data buffers");

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, drawIndBuffer); // Buffer for indirect drawing
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(drawIndCmd), drawIndCmd, GL_STREAM_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	glBindBuffer(GL_UNIFORM_BUFFER, binBuffer); // Uniform bin information
	glBufferData(GL_UNIFORM_BUFFER, sizeof(BinStruct), &binParam, GL_STREAM_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	printError("init uniform data buffers");

	glBindBuffersBase(GL_SHADER_STORAGE_BUFFER, 0, 3, particleBuffers);
	glBindBuffersBase(GL_SHADER_STORAGE_BUFFER, 3, 3, binBuffers);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, drawIndBuffer);
	glBindBufferBase(GL_UNIFORM_BUFFER, 11, binBuffer);
	
	printError("init buffer base bindings");
}

void Particles::ResetBuffers() {
	// Reset buffers
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, particleBuffers[0]); // Particle data
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(ParticleStruct) * particles, particleData.data(), GL_STREAM_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, particleBuffers[1]); // Particle data ping pong
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(ParticleStruct) * particles, particleData.data(), GL_STREAM_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, binBuffers[0]); // Bin count buffer
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(GLuint) * binParam.totalBins, NULL, GL_STREAM_DRAW);
	glClearBufferData(GL_SHADER_STORAGE_BUFFER, GL_R32UI, GL_RED, GL_UNSIGNED_INT, NULL);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, binBuffers[1]); // Bin prefix buffer first pass
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(GLuint) * binParam.totalBins, NULL, GL_STREAM_DRAW);
	glClearBufferData(GL_SHADER_STORAGE_BUFFER, GL_R32UI, GL_RED, GL_UNSIGNED_INT, NULL);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	glBindBuffer(GL_UNIFORM_BUFFER, binBuffer); // Uniform bin information
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(BinStruct), &binParam);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	printError("reset bin buffers");
}

void Particles::ComputeBins() {
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, binBuffers[0]);
	glClearBufferData(GL_SHADER_STORAGE_BUFFER, GL_R32UI, GL_RED, GL_UNSIGNED_INT, NULL);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	
	glUseProgram(computeBin);
	glDispatchCompute(particleWorkGroups, 1, 1);
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
	glDispatchCompute(particleWorkGroups, 1, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	printError("Do Compute: Sort");
}

void Particles::ComputeUpdate() {
	if (doUpdate) {
		if (partUpdate) {
			glUseProgram(computeUpdatePart);
			glDispatchCompute(particleWorkGroups, 1, 1);
		} else {
			glUseProgram(computeUpdateBin);
			glDispatchCompute(binParam.bins, binParam.bins, binParam.bins);	
		}
	} else {
		std::swap(inBufferIndex, outBufferIndex);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, particleBuffers[inBufferIndex]);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, particleBuffers[outBufferIndex]);
	}
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	printError("Do Compute: Update");
}

void Particles::ComputeCull() {
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, drawIndBuffer);
	glClearBufferSubData(GL_SHADER_STORAGE_BUFFER, GL_R32UI, 0 * sizeof(DrawElementsIndirectCommand) + sizeof(GLuint), sizeof(GLuint), GL_RED, GL_UNSIGNED_INT, NULL); // Clear data before since data is used when drawing
	glClearBufferSubData(GL_SHADER_STORAGE_BUFFER, GL_R32UI, 1 * sizeof(DrawElementsIndirectCommand) + sizeof(GLuint), sizeof(GLuint), GL_RED, GL_UNSIGNED_INT, NULL);
	glClearBufferSubData(GL_SHADER_STORAGE_BUFFER, GL_R32UI, 2 * sizeof(DrawElementsIndirectCommand) + sizeof(GLuint), sizeof(GLuint), GL_RED, GL_UNSIGNED_INT, NULL);
	glClearBufferSubData(GL_SHADER_STORAGE_BUFFER, GL_R32UI, 3 * sizeof(DrawElementsIndirectCommand) + sizeof(GLuint), sizeof(GLuint), GL_RED, GL_UNSIGNED_INT, NULL);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	glUseProgram(computeCull);
	glDispatchCompute(particleWorkGroups, 1, 1);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, drawIndBuffer);
	glMemoryBarrier(GL_COMMAND_BARRIER_BIT);

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

void TW_CALL Particles::SetParticleCB(const void* value, void* clientData) {
	Particles* obj = static_cast<Particles*>(clientData);
	PartCount input = *static_cast<const PartCount*>(value);
	obj->SetParticles(input);
}

void TW_CALL Particles::GetParticleCB(void* value, void* clientData) {
	*static_cast<PartCount*>(value) = static_cast<Particles*>(clientData)->partCount;
}

void TW_CALL Particles::SetBinCB(const void* value, void* clientData) {
	Particles* obj = static_cast<Particles*>(clientData);
	BinStruct input = *static_cast<const BinStruct*>(value);
	obj->SetBins(input.bins, input.binSize);
}

void TW_CALL Particles::GetBinCB(void* value, void* clientData) {
	*static_cast<BinStruct*>(value) = static_cast<Particles*>(clientData)->binParam;
}