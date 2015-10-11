#ifndef PARTICLES
#define PARTICLES

#include "Drawable.h"

#include "glm.hpp"

#include <vector>

class Particles : public Drawable
{
private:
	void SetParticleData();
	void InitGlStates();
	void SetCullingState();
	
	//
	const char* shaderFiles[8];
	GLuint cullingBuffer;
	GLuint billboardVAO;
	GLuint programs[4];
	
	// Model data
	GLfloat radius;
	
	// 
	GLuint particles, drawParticles, setParticles;
	GLuint type; 
	std::vector<GLfloat> particleData;

	//
	GLfloat oldT, currT;
	bool doUpdate;
	
public:
	Particles(GLuint numParticles, GLfloat initRadius);
	
	virtual void Update(GLfloat t);
	void Cull(const GLfloat* normals, const GLfloat* points);
	void DrawParticles(const glm::mat4 proj, const glm::mat4 worldView);
	void DrawBillboards(const glm::mat4 proj, const glm::mat4 worldView, const glm::vec3 camerapos);
	
	void ToggleUpdate()
	{doUpdate = !doUpdate;}
	
	GLuint *GetParticlesPtr()
	{return &particles;}
	
	GLuint *GetDrawParticlesPtr()
	{return &drawParticles;}

	const GLint GetParticles()
	{return particles;}
	
	const GLint GetDrawParticles()
	{return drawParticles;}
	
	const GLint GetSetParticles()
	{return setParticles;}
	
	const GLuint GetUpdateProgram()
	{return programs[0];}
	
	const GLuint GetCullingProgram()
	{return programs[1];}
	
	const GLuint GetDrawProgram()
	{return programs[2];}

	void SetParticles(GLuint newParticles);	
	void SetParticles(GLuint newParticles, GLuint newType);
};

#endif
