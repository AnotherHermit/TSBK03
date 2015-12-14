///////////////////////////////////////
//
//	Computer Graphics TSBK03
//	Conrad Wahlén - conwa099
//
///////////////////////////////////////

#define TINYOBJLOADER_IMPLEMENTATION // define this in only *one* .cc
#include "tiny_obj_loader.h"

#include "myDrawable.h"

#include "GL_utilities.h"

#include "glm.hpp"
#include "gtx/transform.hpp"

#include <iostream>

Sphere::Sphere(GLuint inCullBuffer, GLuint inDrawCmdBuffer) {
	cullBuffer = inCullBuffer;
	drawCmdBuffer = inDrawCmdBuffer;
}

bool Sphere::loadModels(const char* path) {
	// Load models
	std::string err;
	bool wasLoaded = tinyobj::LoadObj(shapes, materials, err, path);
	if (!wasLoaded || !err.empty()) {
		std::cerr << err << std::endl;
		return false;
	}
	std::cout << "Loaded following models: " << std::endl;
	for (size_t i = 0; i < shapes.size(); i++) {
		printf("Model (LoD %zd): %s\n", i, shapes[i].name.c_str());
	}
	return true;
}

bool Sphere::Init(const char* path) {
	if (!loadModels(path)) return false;

	// Load shaders
	program = loadShaders("src/shaders/particle.vert", "src/shaders/particle.frag");

	// Create buffers
	glGenVertexArrays(1, &drawVAO);
	glGenBuffers(3, drawBuffers);

	// Initialize Indirect drawing commands
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, drawCmdBuffer);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, 4 * sizeof(DrawElementsIndirectCommand), &drawIndCmd);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	GLuint indexOffset = 0, vertexOffset = 0, useShapes = 4;
	for (size_t i = 0; i < useShapes; i++) {
		drawIndCmd[i].vertexCount = (GLuint)shapes[i].mesh.indices.size();
		drawIndCmd[i].firstVertex = indexOffset;
		indexOffset += (GLuint)shapes[i].mesh.indices.size();

		drawIndCmd[i].baseVertex = vertexOffset;
		vertexOffset += (GLuint)shapes[i].mesh.positions.size() / 3;
	}

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, drawCmdBuffer);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, 4 * sizeof(DrawElementsIndirectCommand), &drawIndCmd);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	// Allocate enough memory for instanced drawing buffers
	glBindBuffer(GL_ARRAY_BUFFER, drawBuffers[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertexOffset * 3, NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, drawBuffers[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertexOffset * 3, NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, drawBuffers[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indexOffset, NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// Fill the instanced drawing buffers with data from models
	glBindBuffer(GL_ARRAY_BUFFER, drawBuffers[0]);
	for (size_t i = 0; i < useShapes; i++) {
		glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLfloat) * drawIndCmd[i].baseVertex * 3, sizeof(GLfloat) * shapes[i].mesh.positions.size(), shapes[i].mesh.positions.data());
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, drawBuffers[1]);
	for (size_t i = 0; i < useShapes; i++) {
		glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLfloat) * drawIndCmd[i].baseVertex * 3, sizeof(GLfloat) * shapes[i].mesh.normals.size(), shapes[i].mesh.normals.data());
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, drawBuffers[2]);
	for (size_t i = 0; i < useShapes; i++) {
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLfloat) * drawIndCmd[i].firstVertex, sizeof(GLfloat) * shapes[i].mesh.indices.size(), shapes[i].mesh.indices.data());
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// Set the GPU pointers for drawing 
	glUseProgram(program);
	glBindVertexArray(drawVAO);

	glBindBuffer(GL_ARRAY_BUFFER, cullBuffer);
	GLuint pPos = glGetAttribLocation(program, "posValue");
	glEnableVertexAttribArray(pPos);
	glVertexAttribPointer(pPos, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribDivisor(pPos, 1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, drawBuffers[0]);
	GLuint vPos = glGetAttribLocation(program, "inPosition");
	glEnableVertexAttribArray(vPos);
	glVertexAttribPointer(vPos, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, drawBuffers[1]);
	GLuint vNorm = glGetAttribLocation(program, "inNormal");
	glEnableVertexAttribArray(vNorm);
	glVertexAttribPointer(vNorm, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, drawBuffers[2]);

	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	
	printError("init Sphere");
	return true;
}

void Sphere::Draw() {
	glUseProgram(program);

	glBindVertexArray(drawVAO);
	glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, 0, 4, 0);
	glBindVertexArray(0);

	printError("Draw Spheres");
}