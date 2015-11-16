///////////////////////////////////////
//
//	Computer Graphics TSBK03
//	Conrad Wahlén - conwa099
//
///////////////////////////////////////

#include "Camera.h"

#include "gtc/matrix_transform.hpp"

Camera::Camera(glm::vec3 startpos) {
	isPaused = true;
	p = startpos;
	yvec = glm::vec3(0.0f, 1.0f, 0.0f);

	mspeed = 100.0f;
	rspeed = 0.001f;
	phi = M_PI;
	theta = M_PI / 2.0f;

	// Set starting WTVmatrix
	Update();
}

void Camera::ResetCamera(glm::vec3 pos) {
	p = pos;

	phi = M_PI;
	theta = M_PI / 2.0f;

	// Set starting worldView matrix
	Update();
	UpdateCullingBox();
}

void Camera::SetFrustum(GLfloat in_left, GLfloat in_right, GLfloat in_bottom, GLfloat in_top, GLfloat in_near, GLfloat in_far) {
	VTPmatrix = glm::frustum(in_left, in_right, in_bottom, in_top, in_near, in_far);

	// Add all normals and vectors before transformation
	// Add order is left, right, bottom, top, far. The near plane check is skipped.
	// All normals are pointing inwards towards the center of the frustum.
	nontransPoints[0] = glm::vec4(0.0, 0.0, 0.0, 1.0);
	nontransPoints[1] = glm::vec4(0.0, 0.0, 0.0, 1.0);
	nontransPoints[2] = glm::vec4(0.0, 0.0, 0.0, 1.0);
	nontransPoints[3] = glm::vec4(0.0, 0.0, 0.0, 1.0);
	nontransPoints[4] = glm::vec4(0.0, 0.0, -in_far, 1.0);

	// The near corners of the frustum
	glm::vec3 a = glm::vec3(in_left, in_top, -in_near);
	glm::vec3 b = glm::vec3(in_right, in_top, -in_near);
	glm::vec3 c = glm::vec3(in_right, in_bottom, -in_near);
	glm::vec3 d = glm::vec3(in_left, in_bottom, -in_near);

	nontransNormals[0] = glm::cross(a, b);
	nontransNormals[1] = glm::cross(b, c);
	nontransNormals[2] = glm::cross(c, d);
	nontransNormals[3] = glm::cross(d, a);
	nontransNormals[4] = glm::vec3(0.0, 0.0, 1.0);

	UpdateCullingBox();
}

void Camera::UpdateCullingBox() {
	for (int i = 0; i < 5; i++) {
		glm::mat4 WTVInv = glm::inverse(WTVmatrix);
		glm::vec3 transNormals = glm::normalize(glm::mat3(WTVInv) * nontransNormals[i]);
		glm::vec4 transPoints = WTVInv * nontransPoints[i];

		normals[3 * i] = transNormals.x;
		normals[3 * i + 1] = transNormals.y;
		normals[3 * i + 2] = transNormals.z;

		points[3 * i] = transPoints.x;
		points[3 * i + 1] = transPoints.y;
		points[3 * i + 2] = transPoints.z;
	}
}

void Camera::Update() {
	// Update directions
	heading = glm::normalize(glm::vec3(-sin(theta)*sin(phi), cos(theta), sin(theta)*cos(phi)));
	side = glm::normalize(glm::cross(heading, yvec));
	up = glm::normalize(glm::cross(side, heading));

	// Update camera matrix
	lookp = p + heading;
	WTVmatrix = lookAt(p, lookp, yvec);
}

void Camera::UpdateCamera() {
	Update();
	UpdateCullingBox();
}

void Camera::MoveForward(GLfloat deltaT) {
	if (!isPaused) {
		p += heading * mspeed * deltaT;
	}
}
void Camera::MoveRight(GLfloat deltaT) {
	if (!isPaused) {
		p += side * mspeed * deltaT;
	}
}
void Camera::MoveUp(GLfloat deltaT) {
	if (!isPaused) {
		p += up * mspeed * deltaT;
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
