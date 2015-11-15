#ifndef PARTICLES
#define PARTICLES

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
	// Transform feedback
	GLuint updateVAO[2], cullingVAO[2];
	GLuint updateBuffer[2], cullingBuffer;
	GLuint updateShader, cullShader;
	GLuint qId;
	GLint currVAO, currTFB;

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
	void InitGLStates();

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
	void Update(GLfloat t);
	void Cull();
	void Draw();


	void ToggleDrawModels() { renderModels = !renderModels; }
	void ToggleUpdate() { doUpdate = !doUpdate; }

	GLuint *GetParticlesPtr() { return &particles; }
	GLuint *GetDrawParticlesPtr() { return &computeDrawParticles; }
	GLuint *GetDisplayBinPtr() { return &displaybin; }

	const GLint GetParticles() { return particles; }
	const GLint GetDrawParticles() { return drawParticles; }
	const GLint GetSetParticles() { return setParticles; }
	const GLuint GetUpdateProgram() { return updateShader; }
	const GLuint GetCullingProgram() { return cullShader; }

	void SetParticles(GLuint newParticles);
	void SetParticles(GLuint newParticles, GLuint newType);
};

#endif
