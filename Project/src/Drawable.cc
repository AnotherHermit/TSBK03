///////////////////////////////////////
//
//	Computer Graphics TSBK03
//	Conrad Wahlén - conwa099
//
///////////////////////////////////////

#include "Drawable.h"

#include "LoadTGA.h"
#include "GL_utilities.h"

#include "glm.hpp"
#include "gtx/transform.hpp"

Sphere::Sphere(GLfloat radius)
	: Drawable() {
	program = -1;
	model = nullptr;
	MTWmatrix = glm::scale(glm::vec3(radius));
}

bool Sphere::Init(Camera* setCam, GLuint buffer) {
	cam = setCam;
	cullBuffer = buffer;

	program = loadShaders("src/shaders/particle.vert", "src/shaders/particle.frag");
	glUseProgram(program);

	model = LoadModel("resources/groundsphere.obj");
	NormalizeModel(model);
	BuildModelVAO2(model, program, "inPosition", "inNormal", "inTexCoord");

	glUniformMatrix4fv(glGetUniformLocation(program, "MTWmatrix"), 1, GL_FALSE, glm::value_ptr(MTWmatrix));
	glUniformMatrix4fv(glGetUniformLocation(program, "VTPmatrix"), 1, GL_FALSE, glm::value_ptr(cam->GetVTP()));

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

void Sphere::Update(GLfloat t) {
	glUseProgram(program);

	glUniformMatrix4fv(glGetUniformLocation(program, "WTVmatrix"), 1, GL_FALSE, glm::value_ptr(cam->GetWTV()));
	glUniform1f(glGetUniformLocation(program, "currT"), t);

	printError("Model Update");
}

void Sphere::Draw(GLuint num) {
	glUseProgram(program);

	glBindVertexArray(model->vao);
	if (num > 0) { glDrawElementsInstanced(GL_TRIANGLES, model->numIndices, GL_UNSIGNED_INT, 0L, num); }
	glBindVertexArray(0);

	printError("Draw Spheres");
}

Billboard::Billboard(GLfloat startRadius) {
	program = -1;
	texID = -1;
	vao = -1;
	cullBuffer = -1;
	radius = startRadius;
}

bool Billboard::Init(Camera* setCam, GLuint buffer) {
	cam = setCam;
	cullBuffer = buffer;

	program = loadShadersG("src/shaders/billboard.vert", "src/shaders/billboard.frag", "src/shaders/billboard.geom");
	glUseProgram(program);

	glGenVertexArrays(1, &vao);
	glGenTextures(1, &texID);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texID);
	LoadTGATextureSimple("resources/particle.tga", &texID);
	
	glUniformMatrix4fv(glGetUniformLocation(program, "VTPmatrix"), 1, GL_FALSE, glm::value_ptr(cam->GetVTP()));
	glUniform1f(glGetUniformLocation(program, "radius"), radius);
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

void Billboard::Update(GLfloat t) {
	glUseProgram(program);

	glUniform3f(glGetUniformLocation(program, "cameraPos"), cam->GetPos().x, cam->GetPos().y, cam->GetPos().z);
	glUniformMatrix4fv(glGetUniformLocation(program, "WTVmatrix"), 1, GL_FALSE, glm::value_ptr(cam->GetWTV()));
	glUniform1f(glGetUniformLocation(program, "currT"), t);

	printError("Billboard Update");
}

void Billboard::Draw(GLuint num) {
	glUseProgram(program);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texID);
	glBindVertexArray(vao);
	if (num > 0) { glDrawArrays(GL_POINTS, 0, num); }
	glBindVertexArray(0);

	printError("Draw Billboards");
}
