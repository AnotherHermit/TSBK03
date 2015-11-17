///////////////////////////////////////
//
//	Computer Graphics TSBK03
//	Conrad Wahlén - conwa099
//
///////////////////////////////////////

#ifndef DRAWABLE_H
#define DRAWABLE_H

#include "Camera.h"

#include "loadobj.h"

#include "glm.hpp"

// ===== Drawable base class ===== \\

class Drawable {
public:
	Drawable() {}
	
	virtual bool Init(GLuint buffer) = 0;
	virtual void Draw(GLuint num) = 0;
};

// ===== Sphere class ===== \\

class Sphere : public Drawable {
private:
	GLuint program, cullBuffer;
	Model* model;

public:
	Sphere();

	virtual bool Init(GLuint buffer);
	virtual void Draw(GLuint num);
};

// ===== Billboard class ===== \\

class Billboard : public Drawable {
private:
	GLuint program, texID, vao, cullBuffer;

public:
	Billboard();

	virtual bool Init(GLuint buffer);
	virtual void Draw(GLuint num);
};

#endif // DRAWABLE_H
