///////////////////////////////////////
//
//	Computer Graphics TSBK03
//	Conrad Wahlén - conwa099
//
///////////////////////////////////////

#include "Boid.h"

#include "GL_utilities.h"

Boid::Boid() {
	param.previous = 1.0f;
	param.cohesion = 0.18f;
	param.separation = 0.26f;
	param.alignment = 0.25f;
	param.fear = 0.30f;

	boidTwMembers[0] = { "Previous", TW_TYPE_FLOAT, offsetof(BoidStruct, previous), " min=0.0 max=1.0 step=0.01 group='Boid Controls' "};
	boidTwMembers[1] = { "Cohesion", TW_TYPE_FLOAT, offsetof(BoidStruct, cohesion), " min=0.0 max=1.0 step=0.01 group='Boid Controls' "};
	boidTwMembers[2] = { "Separation", TW_TYPE_FLOAT, offsetof(BoidStruct, separation), " min=0.0 max=1.0 step=0.01 group='Boid Controls' "};
	boidTwMembers[3] = { "Alignment", TW_TYPE_FLOAT, offsetof(BoidStruct, alignment), " min=0.0 max=1.0 step=0.01 group='Boid Controls' "};
	boidTwMembers[4] = { "Fear", TW_TYPE_FLOAT, offsetof(BoidStruct, fear), " min=0.0 max=1.0 step=0.01 group='Boid Controls' "};
	boidTwStruct = TwDefineStruct("Boids", boidTwMembers, 5, sizeof(BoidStruct), NULL, NULL);
}

bool Boid::Init() {
	glGenBuffers(1, &boidBuffer);
	glBindBufferBase(GL_UNIFORM_BUFFER, 13, boidBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, boidBuffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(BoidStruct), &param, GL_STREAM_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	printError("camera init");
	return true;
}

void Boid::Update() {
	glBindBuffer(GL_UNIFORM_BUFFER, boidBuffer);
	glBufferSubData(GL_UNIFORM_BUFFER, NULL, sizeof(BoidStruct), &param);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void TW_CALL Boid::SetBoidCB(const void* value, void* clientData) {
	static_cast<Boid*>(clientData)->param = *static_cast<const BoidStruct*>(value);
	static_cast<Boid*>(clientData)->Update();
}


void TW_CALL Boid::GetBoidParamsCB(void* value, void* clientData) {
	*static_cast<BoidStruct*>(value) = static_cast<Boid*>(clientData)->param;
}