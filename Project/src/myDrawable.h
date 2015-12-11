///////////////////////////////////////
//
//	Computer Graphics TSBK03
//	Conrad Wahlén - conwa099
//
///////////////////////////////////////

#ifndef myDrawable_H
#define myDrawable_H

#include "Camera.h"

#include "loadobj.h"

#include "glm.hpp"

// ===== myDrawable base class =====

class myDrawable {
public:
	myDrawable() {}
	
	virtual bool Init(GLuint buffer) = 0;
	virtual void Draw(GLuint num) = 0;
};

// ===== Sphere class =====

class Sphere : public myDrawable {
private:
	GLuint program, cullBuffer;
	Model* model;

public:
	Sphere();

	virtual bool Init(GLuint buffer);
	virtual void Draw(GLuint num);
};

// ===== Billboard class =====

class Billboard : public myDrawable {
private:
	GLuint program, texID, vao, cullBuffer;

public:
	Billboard();

	virtual bool Init(GLuint buffer);
	virtual void Draw(GLuint num);
};

#endif // myDrawable_H
