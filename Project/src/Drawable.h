#ifndef DRAWABLE
#define DRAWABLE


#include "loadobj.h"
#include "glm.hpp"

class Drawable
{
protected:
	// gl Locations
	GLuint program, textureId;
	
	// Names of shader variables
	const char* inputNames[3];
	
	// Model parameters
	Model* model;
	glm::vec3 baseColor;

	// Transformation matrices
	glm::mat4 modelWorld;
	glm::mat4 modelInit;
	
public:
	Drawable();
	
	void SetProgram(const char** shaderLoc);
	void SetModel(const char* modelLoc, glm::mat4 initTransform);
	void SetTexture(const char* textureLoc);
	void SetColor(glm::vec3 colorVec);
	
	virtual void Update(GLfloat t);
	virtual void Draw();
};

#endif
