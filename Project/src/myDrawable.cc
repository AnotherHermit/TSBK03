///////////////////////////////////////
//
//	Computer Graphics TSBK03
//	Conrad Wahlén - conwa099
//
///////////////////////////////////////

#include "myDrawable.h"

#include "LoadTGA.h"
#include "GL_utilities.h"

#include "glm.hpp"
#include "gtx/transform.hpp"

Sphere::Sphere()
	: myDrawable() {
	program = -1;
	model = nullptr;
}

bool Sphere::Init(GLuint buffer) {
	cullBuffer = buffer;

	program = loadShaders("src/shaders/particle.vert", "src/shaders/particle.frag");
	glUseProgram(program);

	model = LoadModel("resources/groundsphere.obj");
	NormalizeModel(model);
	BuildModelVAO2(model, program, "inPosition", "inNormal", "inTexCoord");

	glBindBuffer(GL_ARRAY_BUFFER, cullBuffer);
	glBindVertexArray(model->vao);

	GLuint drPosAttr = glGetAttribLocation(program, "posValue");
	glEnableVertexAttribArray(drPosAttr);
	glVertexAttribPointer(drPosAttr, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribDivisor(drPosAttr, 1);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	printError("Init Sphere");
	return true;
}

void Sphere::Draw(GLuint num) {
	glUseProgram(program);

	glBindVertexArray(model->vao);
	if (num > 0) { glDrawElementsInstanced(GL_TRIANGLES, model->numIndices, GL_UNSIGNED_INT, 0L, num); }
	glBindVertexArray(0);

	printError("Draw Spheres");
}

Billboard::Billboard() {
	program = -1;
	texID = -1;
	vao = -1;
	cullBuffer = -1;
}

bool Billboard::Init(GLuint buffer) {
	cullBuffer = buffer;

	program = loadShadersG("src/shaders/billboard.vert", "src/shaders/billboard.frag", "src/shaders/billboard.geom");
	glUseProgram(program);

	glGenVertexArrays(1, &vao);
	glGenTextures(1, &texID);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texID);
	LoadTGATextureSimple("resources/particle.tga", &texID);

	glUniform1i(glGetUniformLocation(program, "texUnit"), 0);

	glBindBuffer(GL_ARRAY_BUFFER, cullBuffer);
	glBindVertexArray(vao);

	GLuint drPosAttr = glGetAttribLocation(program, "posValue");
	glEnableVertexAttribArray(drPosAttr);
	glVertexAttribPointer(drPosAttr, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	printError("Init Billboard");
	return true;
}

void Billboard::Draw(GLuint num) {
	glUseProgram(program);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texID);
	glBindVertexArray(vao);
	glDrawArraysIndirect(GL_POINTS, 0);
	glBindVertexArray(0);

	printError("Draw Billboards");
}
