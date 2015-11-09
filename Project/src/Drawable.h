#ifndef DRAWABLE
#define DRAWABLE

#include "Camera.h"

#include "loadobj.h"

#include "glm.hpp"

// ===== Drawable base class ===== \\

class Drawable {
protected:
	Camera* cam;
public:
	Drawable() {}
	
	virtual bool Init(Camera* cam, GLuint buffer) = 0;
	virtual void Update(GLfloat t) = 0;
	virtual void Draw(GLuint num) = 0;
};

// ===== Sphere class ===== \\

class Sphere : public Drawable {
private:
	// gl Locations
	GLuint program, cullBuffer;
	// Model parameters
	Model* model;
	// Transformation matrices
	glm::mat4 MTWmatrix;

public:
	Sphere(GLfloat radius);

	virtual bool Init(Camera* cam, GLuint buffer);
	virtual void Update(GLfloat t);
	virtual void Draw(GLuint num);
};

// ===== Billboard class ===== \\

class Billboard : public Drawable {
private:
	// gl Locations
	GLuint program, texID, vao, cullBuffer;
	GLfloat radius;

public:
	Billboard(GLfloat startRadius);

	virtual bool Init(Camera* cam, GLuint buffer);
	virtual void Update(GLfloat t);
	virtual void Draw(GLuint num);
};

#endif
