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


// ===== Sphere class =====

class Sphere {
protected:
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	DrawElementsIndirectCommand drawIndCmd[4];
	GLuint program;
	GLuint cullBuffer, drawBuffers[3], drawCmdBuffer;
	GLuint drawVAO;

	bool loadModels(const char* path);

public:
	Sphere(GLuint inCullBuffer, GLuint inDrawCmdBuffer);

	bool Init(const char* path);
	void Draw();
};

#endif // myDrawable_H
