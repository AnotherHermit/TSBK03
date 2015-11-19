///////////////////////////////////////
//
//	Computer Graphics TSBK03
//	Conrad Wahlén - conwa099
//
///////////////////////////////////////

#include "Boid.h"

Boid::Boid() {
	param.previous = 1.0f;
	param.cohesion = 0.03f;
	param.separation = 0.03f;
	param.alignment = 0.03f;
	param.fear = 0.30;

	glGenBuffers(1, &boidBuffer);
	glBindBufferBase(GL_UNIFORM_BUFFER, 13, boidBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, boidBuffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(BoidStruct), &param, GL_STREAM_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Boid::Update() {
	glBindBuffer(GL_UNIFORM_BUFFER, boidBuffer);
	glBufferSubData(GL_UNIFORM_BUFFER, NULL, sizeof(BoidStruct), &param);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
