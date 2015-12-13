///////////////////////////////////////
//
//	Computer Graphics TSBK03
//	Conrad Wahlén - conwa099
//
///////////////////////////////////////

#ifndef myDrawable_H
#define myDrawable_H

#include "tiny_obj_loader.h"

#include "Camera.h"

#include "GL_utilities.h"

#include "glm.hpp"



// ===== myDrawable base class =====

class myDrawable {
public:
	myDrawable() {}
	
	virtual bool Init(GLuint inCullBuffer, GLuint inDrawCmdBuffer) = 0;
	virtual void Draw(GLuint num) = 0;
};

// ===== Sphere class =====

class Sphere : public myDrawable {
protected:
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	DrawElementsIndirectCommand drawIndCmd[4];
	GLuint program;
	GLuint cullBuffer, drawBuffers[3], drawCmdBuffer;
	GLuint drawVAO;

public:
	Sphere();

	virtual bool Init(GLuint inCullBuffer, GLuint inDrawCmdBuffer);
	virtual void Draw(GLuint num);
};

// ===== Billboard class =====

class Billboard : public myDrawable {
private:
	GLuint program, texID, vao, cullBuffer;

public:
	Billboard();

	virtual bool Init(GLuint inCullBuffer, GLuint notUsed);
	virtual void Draw(GLuint num);
};

#endif // myDrawable_H
