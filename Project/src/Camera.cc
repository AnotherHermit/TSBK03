///////////////////////////////////////
//
//	Computer Graphics TSBK03
//	Conrad Wahlén - conwa099
//
///////////////////////////////////////

#include "Camera.h"

#include "gtc/matrix_transform.hpp"

#include <iostream>

Camera::Camera(glm::vec3 startpos, GLint* screenWidth, GLint* screenHeight, glm::vec4 inLodLevels) {
	isPaused = true;
	param.position = startpos;
	yvec = glm::vec3(0.0f, 1.0f, 0.0f);

	mspeed = 100.0f;
	rspeed = 0.001f;
	phi = 7 * M_PI / 4;
	theta = M_PI / 2.0f;

	winWidth = screenWidth;
	winHeight = screenHeight;
	param.lodLevels = inLodLevels;

	// Set starting WTVmatrix
	Update();
	SetFrustum();
	UpdateCullingBox();

	glGenBuffers(1, &cameraBuffer);
	glBindBufferBase(GL_UNIFORM_BUFFER, 10, cameraBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, cameraBuffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(CameraParam), &param, GL_STREAM_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);


	cameraTwMembers[0] = {"Cam Pos x", TW_TYPE_FLOAT, offsetof(CameraParam, position.x), " readonly=true group=Info "};
	cameraTwMembers[1] = {"Cam Pos y", TW_TYPE_FLOAT, offsetof(CameraParam, position.y), " readonly=true group=Info "};
	cameraTwMembers[2] = {"Cam Pos z", TW_TYPE_FLOAT, offsetof(CameraParam, position.z), " readonly=true group=Info "};
	cameraTwMembers[3] = {"LOD 1 Dist", TW_TYPE_FLOAT, offsetof(CameraParam, lodLevels.x), " min=0.0 max=2000.0 step=100.0 group=Controls "};
	cameraTwMembers[4] = {"LOD 2 Dist", TW_TYPE_FLOAT, offsetof(CameraParam, lodLevels.y), " min=0.0 max=2000.0 step=100.0 group=Controls "};
	cameraTwMembers[5] = {"LOD 3 Dist", TW_TYPE_FLOAT, offsetof(CameraParam, lodLevels.z), " min=0.0 max=2000.0 step=50.0 group=Controls "};
	cameraTwMembers[6] = {"LOD 4 Dist", TW_TYPE_FLOAT, offsetof(CameraParam, lodLevels.w), " min=0.0 max=2000.0 step=10.0 group=Controls "};
	cameraTwStruct = TwDefineStruct("Camera", cameraTwMembers, 7, sizeof(CameraParam), NULL, NULL);
}

void Camera::ResetCamera(glm::vec3 pos) {
	param.position = pos;

	phi = 7 * M_PI / 4;
	theta = M_PI / 2.0f;
}

void Camera::SetFrustum() {
	GLfloat ratio = (GLfloat)*winWidth / (GLfloat)*winHeight;
	GLfloat width = (ratio > 1.0f) ? 1.0f : ratio;
	GLfloat height = (ratio > 1.0f) ? 1.0f / ratio : 1.0f;

	param.VTPmatrix = glm::frustum(-width, width, -height, height, 1.0f, param.lodLevels.x);

	// Add all normals and vectors before transformation
	// Add order is left, right, bottom, top, far. The near plane check is skipped.
	// All normals are pointing inwards towards the center of the frustum.
	nontransPoints[0] = glm::vec4(0.0, 0.0, 0.0, 1.0);
	nontransPoints[1] = glm::vec4(0.0, 0.0, 0.0, 1.0);
	nontransPoints[2] = glm::vec4(0.0, 0.0, 0.0, 1.0);
	nontransPoints[3] = glm::vec4(0.0, 0.0, 0.0, 1.0);
	nontransPoints[4] = glm::vec4(0.0, 0.0, -param.lodLevels.x, 1.0);
	nontransPoints[5] = glm::vec4(0.0, 0.0, -param.lodLevels.y, 1.0);
	nontransPoints[6] = glm::vec4(0.0, 0.0, -param.lodLevels.z, 1.0);
	nontransPoints[7] = glm::vec4(0.0, 0.0, -param.lodLevels.w, 1.0);

	// The near corners of the frustum
	glm::vec3 a = glm::vec3(-width, height, -1.0f);	// Left, top, near
	glm::vec3 b = glm::vec3(width, height, -1.0f);	// Right, top, near
	glm::vec3 c = glm::vec3(width, -height, -1.0f);	// Right, bottom, near
	glm::vec3 d = glm::vec3(-width, -height, -1.0f);	// Left, bottom, near

	nontransNormals[0] = glm::cross(a, b);
	nontransNormals[1] = glm::cross(b, c);
	nontransNormals[2] = glm::cross(c, d);
	nontransNormals[3] = glm::cross(d, a);
	nontransNormals[4] = glm::vec3(0.0, 0.0, 1.0);
	nontransNormals[5] = glm::vec3(0.0, 0.0, 1.0);
	nontransNormals[6] = glm::vec3(0.0, 0.0, 1.0);
	nontransNormals[7] = glm::vec3(0.0, 0.0, 1.0);
}

void Camera::UpdateCullingBox() {
	for (int i = 0; i < 8; i++) {
		glm::mat4 WTVInv = glm::inverse(param.WTVmatrix);
		glm::vec3 transNormals = glm::normalize(glm::mat3(WTVInv) * nontransNormals[i]);
		glm::vec4 transPoints = WTVInv * nontransPoints[i];

		param.normals[i] = glm::vec4(transNormals, 1.0f);
		param.points[i] = transPoints;
	}
}

void Camera::Update() {
	// Update directions
	heading = glm::normalize(glm::vec3(-sin(theta)*sin(phi), cos(theta), sin(theta)*cos(phi)));
	side = glm::normalize(glm::cross(heading, yvec));
	up = glm::normalize(glm::cross(side, heading));

	// Update camera matrix
	lookp = param.position + heading;
	param.WTVmatrix = lookAt(param.position, lookp, yvec);
}

void Camera::UploadParams() {
	glBindBuffer(GL_UNIFORM_BUFFER, cameraBuffer);
	glBufferSubData(GL_UNIFORM_BUFFER, NULL, sizeof(CameraParam), &param);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Camera::UpdateCamera() {
	Update();
	SetFrustum();
	UpdateCullingBox();
	UploadParams();
}

void Camera::MoveForward(GLfloat deltaT) {
	if (!isPaused) {
		param.position += heading * mspeed * deltaT;
	}
}
void Camera::MoveRight(GLfloat deltaT) {
	if (!isPaused) {
		param.position += side * mspeed * deltaT;
	}
}
void Camera::MoveUp(GLfloat deltaT) {
	if (!isPaused) {
		param.position += up * mspeed * deltaT;
	}
}

void Camera::RotateCamera(GLint dx, GLint dy) {
	if (!isPaused) {
		float eps = 0.001f;

		phi += rspeed * dx;
		theta += rspeed * dy;

		phi = fmod(phi, 2.0f * M_PI);
		theta = theta < M_PI - eps ? (theta > eps ? theta : eps) : M_PI - eps;
	}
}
