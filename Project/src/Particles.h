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
	GLfloat radius;
	GLuint particles, setParticles;
	std::vector<ParticleStruct> particleData;

	// Bin info
	BinStruct binParam;
	GLuint binBuffer;
	GLuint *prefixArrayIn, *prefixArrayOut;

	// Others
	bool doUpdate;

	// Methods
	void SetParticleData();

	// Compute shader stuff
	GLuint particleBuffers[3], binBuffers[2], counterBuffer;
	GLuint computeBin, computeSort, computeUpdate, computeCull;
	GLuint computeDrawParticles;
	GLuint inBufferIndex, outBufferIndex;
	void CompileComputeShader(GLuint* program, const char* path);
	void InitCompute();

public:
	Particles(GLuint numParticles, GLfloat initRadius);

	bool Init();
	void DoCompute();

	void ToggleUpdate() { doUpdate = !doUpdate; }

	GLuint *GetParticlesPtr() { return &particles; }
	GLuint *GetDrawParticlesPtr() { return &computeDrawParticles; }

	const GLint GetParticles() { return particles; }
	const GLint GetDrawParticles() { return computeDrawParticles; }
	const GLint GetSetParticles() { return setParticles; }
	const GLuint GetCullBuffer() { return particleBuffers[2]; }

	void SetParticles(GLuint newParticles);
};

#endif // PARTICLES_H
