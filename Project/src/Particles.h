#ifndef PARTICLES
#define PARTICLES

#include "Camera.h"
#include "Drawable.h"

#include "glm.hpp"

#include <vector>

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
	std::vector<GLfloat> particleData;

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

public:
	Particles(GLuint numParticles, GLfloat initRadius);

	bool Init(Camera* setCam);
	void Update(GLfloat t);
	void Cull();
	void Draw();

	void ToggleDrawModels() { renderModels = !renderModels; }
	void ToggleUpdate() { doUpdate = !doUpdate; }

	GLuint *GetParticlesPtr() { return &particles; }
	GLuint *GetDrawParticlesPtr() { return &drawParticles; }

	const GLint GetParticles() { return particles; }
	const GLint GetDrawParticles() { return drawParticles; }
	const GLint GetSetParticles() { return setParticles; }
	const GLuint GetUpdateProgram() { return updateShader; }
	const GLuint GetCullingProgram() { return cullShader; }

	void SetParticles(GLuint newParticles);
	void SetParticles(GLuint newParticles, GLuint newType);
};

#endif
