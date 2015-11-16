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
	GLuint padding;
};

class Particles {
private:
	// Particle info
	GLfloat radius;
	GLuint startMode;
	GLuint particles, drawParticles, setParticles;
	std::vector<ParticleStruct> particleData;

	// Bin info
	GLuint bins, numBins;
	GLuint *prefixArrayIn, *prefixArrayOut;
	GLuint displaybin;
	GLfloat binSize;

	// Drawing stuff
	Camera* cam;
	Drawable *model, *billboard;

	// Others
	GLfloat oldT, currT;
	bool doUpdate;
	bool renderModels;

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

	bool Init(Camera* setCam);
	void DoCompute(GLfloat t);
	void Draw();

	void ToggleDrawModels() { renderModels = !renderModels; }
	void ToggleUpdate() { doUpdate = !doUpdate; }

	GLuint *GetParticlesPtr() { return &particles; }
	GLuint *GetDrawParticlesPtr() { return &computeDrawParticles; }
	GLuint *GetDisplayBinPtr() { return &displaybin; }

	const GLint GetParticles() { return particles; }
	const GLint GetDrawParticles() { return drawParticles; }
	const GLint GetSetParticles() { return setParticles; }

	void SetParticles(GLuint newParticles);
	void SetParticles(GLuint newParticles, GLuint newType);
};

#endif // PARTICLES_H
