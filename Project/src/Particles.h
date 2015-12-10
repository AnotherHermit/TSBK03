///////////////////////////////////////
//
//	Computer Graphics TSBK03
//	Conrad Wahlén - conwa099
//
///////////////////////////////////////

#ifndef PARTICLES_H
#define PARTICLES_H

#include "Camera.h"
#include "Drawable.h"

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


class Particles {
private:
	// Particle info
	GLuint particles, setParticles;
	std::vector<ParticleStruct> particleData;

	// Bin info
	BinStruct binParam;
	GLuint binBuffer;
	GLuint *prefixArray;

	// Others
	bool doUpdate;

	// Methods
	void SetParticleData();

	// Compute shader threads
	GLuint numThreads;

	// Compute shader stuff
	GLuint particleBuffers[3], binBuffers[3], counterBuffer;
	GLuint computeBin, computePrefixGather, computePrefixReduce, computePrefixSpread, computeSort, computeUpdate, computeCull;
	GLuint computeDrawParticles;
	GLuint inBufferIndex, outBufferIndex;
	GLuint prefixWorkGroups;
	void InitCompute();

public:
	Particles(GLuint numParticles, GLfloat initBinSize);
	~Particles();

	static GLuint MAX_PARTICLES;
	static GLuint MAX_BINS;

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

	GLuint *GetParticlesPtr() { return &particles; }
	GLuint *GetDrawParticlesPtr() { return &computeDrawParticles; }
	ParticleStruct* GetParticleData() { return particleData.data(); }

	const GLint GetParticles() { return particles; }
	const GLint GetDrawParticles() { return computeDrawParticles; }
	const GLint GetSetParticles() { return setParticles; }
	const GLuint GetTotalBins() { return binParam.totalBins; }
	const GLuint GetBins() { return binParam.bins; }
	const GLfloat GetBinSize() { return binParam.binSize; }
	const GLuint GetCullBuffer() { return particleBuffers[2]; }

	GLuint* GetParticleBuffers() { return particleBuffers; }
	GLuint* GetBinBuffers() { return binBuffers; }

	void SetParticles(GLuint newParticles);
};

#endif // PARTICLES_H
