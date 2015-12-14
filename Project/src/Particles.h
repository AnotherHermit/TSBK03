///////////////////////////////////////
//
//	Computer Graphics TSBK03
//	Conrad Wahlén - conwa099
//
///////////////////////////////////////

#ifndef PARTICLES_H
#define PARTICLES_H

#define ELEMENTS_REDUCE_PHASE 1024
#define COMPUTE_THREADS 512

// 128*128*128 = 2048 * 1024
#define MAX_BINS 2097152

// 4096 * 1024
#define MAX_PARTICLES 4194304 

#include "Camera.h"
#include "myDrawable.h"

#include "GL_utilities.h"

#include "glm.hpp"

#include <vector>

struct ParticleStruct {
	glm::vec3 position;
	GLuint bin;
	glm::vec3 velocity;
	GLuint ID;
};

struct BinStruct {
	GLuint bins;
	GLuint totalBins;
	GLfloat binSize;
	GLfloat areaSize;
};

enum PartCount {
	COUNT0 = 32 * 1024,
	COUNT1 = 64 * 1024,
	COUNT2 = 128 * 1024,
	COUNT3 = 256 * 1024,
	COUNT4 = 512 * 1024,
	COUNT5 = 1024 * 1024,
	COUNT6 = 2048 * 1024,
	COUNT7 = 4096 * 1024,
};

class Particles {
private:
	// Particle info
	PartCount partCount;
	GLuint particles;
	std::vector<ParticleStruct> particleData;

	// Bin info
	BinStruct binParam;
	GLuint binBuffer;

	// Others
	bool doUpdate, partUpdate;

	// Methods
	void SetParticleData();
	void ResetBuffers();
	void InitBuffers();

	// Compute shader stuff
	DrawElementsIndirectCommand drawIndCmd[4];
	GLuint particleBuffers[3], binBuffers[3], drawIndBuffer;
	GLuint computeBin, computePrefixGather, computePrefixReduce, computePrefixSpread, computeSort, computeUpdatePart, computeUpdateBin, computeCull;
	GLuint inBufferIndex, outBufferIndex;
	GLuint prefixWorkGroups, particleWorkGroups;

	// AntTweakBar stuff
	TwStructMember binTwMembers[3];
	TwType binTwStruct;
	TwEnumVal numParticlesEV[8];
	TwType particleCount;

public:
	Particles(PartCount count, GLuint numBins, GLfloat initBinSize);
	~Particles();

	bool Init();
	void DoCompute();

	void ComputeBins();
	void ComputePrefixGather();
	void ComputePrefixReduce();
	void ComputePrefixSpread();
	void ComputeSort();
	void ComputeUpdate();
	void ComputeCull();

	void ToggleUpdate() { doUpdate = !doUpdate; }
	void TogglePartUpdate() { partUpdate = !partUpdate; }

	GLuint* GetParticlePtr() { return &particles; }
	ParticleStruct* GetParticleData() { return particleData.data(); }

	const GLint GetParticles() { return particles; }
	const GLuint GetTotalBins() { return binParam.totalBins; }
	const GLuint GetBins() { return binParam.bins; }
	const GLfloat GetBinSize() { return binParam.binSize; }
	const GLuint GetCullBuffer() { return particleBuffers[2]; }

	GLuint* GetParticleBuffers() { return particleBuffers; }
	GLuint* GetBinBuffers() { return binBuffers; }
	GLuint GetDrawCommandBuffer() { return drawIndBuffer; }

	void SetParticles(PartCount newCount);
	void SetBins(GLuint newBins, GLfloat newSize);

	TwType GetBinTwType() { return binTwStruct; }
	TwType GetParticlesTwType() { return particleCount; }

	static void TW_CALL SetParticleCB(const void* value, void* clientData);
	static void TW_CALL GetParticleCB(void* value, void* clientData);

	static void TW_CALL SetBinCB(const void* value, void* clientData);
	static void TW_CALL GetBinCB(void* value, void* clientData);
};

#endif // PARTICLES_H
