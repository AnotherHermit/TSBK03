#ifndef DRAWABLE
#define DRAWABLE

#include "Camera.h"

#include "loadobj.h"

#include "glm.hpp"

// ===== Drawable base class ===== \\

class Drawable {
public:
	Drawable() {}
	
	virtual bool Init(Camera* cam, GLuint buffer) = 0;
	virtual void Update(GLfloat t) = 0;
	virtual void Draw(GLuint num) = 0;
};

// ===== Sphere class ===== \\

class Sphere : public Drawable {
private:
	GLuint program, cullBuffer;
	Model* model;
	glm::mat4 MTWmatrix;

	Camera* cam;

public:
	Sphere(GLfloat radius);

	virtual bool Init(Camera* cam, GLuint buffer);
	virtual void Update(GLfloat t);
	virtual void Draw(GLuint num);
};

// ===== Billboard class ===== \\

class Billboard : public Drawable {
private:
	GLuint program, texID, vao, cullBuffer;
	GLfloat radius;

	Camera* cam;

public:
	Billboard(GLfloat startRadius);

	virtual bool Init(Camera* cam, GLuint buffer);
	virtual void Update(GLfloat t);
	virtual void Draw(GLuint num);
};

#endif
